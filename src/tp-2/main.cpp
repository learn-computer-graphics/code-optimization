#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/video.hpp>
#include <iostream>

int main(int argc, char** argv)
{
	cv::Ptr<cv::BackgroundSubtractor> pBackSub(cv::createBackgroundSubtractorKNN(500, 150, false));
	cv::Mat frame;
	cv::Mat cleanedFrame;
	cv::Mat temp;
	cv::Mat kernel1(5, 5, CV_8U, 1);
	cv::Mat kernel2(80, 80, CV_8U, 1);
	cv::VideoCapture capture;

	capture.open(0);
	if (!capture.isOpened())
	{
		std::cout << "No capture" << std::endl;
		return 0;
	}
	capture.set(cv::CAP_PROP_AUTO_EXPOSURE, -1);

	std::cout << "Capture is opened" << std::endl;
	for (;;)
	{
		capture.read(frame);
		if (frame.empty())
			break;

		cv::Mat labels(frame.size(), CV_32S);
		pBackSub->apply(frame, cleanedFrame); // Remove background, and set image to black and white
		cv::morphologyEx(cleanedFrame, temp, cv::MORPH_OPEN, kernel1); // opening to remove noise
		cv::morphologyEx(temp, cleanedFrame, cv::MORPH_CLOSE, kernel2); // closing to fill gaps
		const int nLabels = cv::connectedComponents(cleanedFrame, labels); // count the number of connected components
		// TODO filter the number of connected components by their size

		cv::imshow("frame", frame);
		cv::imshow("detection", cleanedFrame);
		std::cout << "Connected components : " << nLabels << std::endl;
		if (cv::waitKey(5) >= 0)
			break;
	}

	std::cout << "video completed succesfully" << std::endl;
	return 0;
}
