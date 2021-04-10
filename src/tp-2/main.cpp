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
	cv::Mat labels;
	cv::Mat stats;
	cv::Mat centroids;
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

		
		pBackSub->apply(frame, cleanedFrame); // Remove background, and set image to black and white
		cv::morphologyEx(cleanedFrame, temp, cv::MORPH_OPEN, kernel1); // opening to remove noise
		cv::morphologyEx(temp, cleanedFrame, cv::MORPH_CLOSE, kernel2); // closing to fill gaps

		
		cv::connectedComponentsWithStats(cleanedFrame, labels, stats, centroids); // count the number of connected components
		int peoples = 0;
		for (int i = 0; i < stats.rows; i++)
		{
			const int x = stats.at<int>(cv::Point(0, i));
			const int y = stats.at<int>(cv::Point(1, i));
			const int w = stats.at<int>(cv::Point(2, i));
			const int h = stats.at<int>(cv::Point(3, i));

			const bool isAcceptableSize = w < frame.rows - 50 && w > 50;
			if (isAcceptableSize)
			{
				cv::Scalar color(255, 0, 0);
				cv::Rect rect(x, y, w, h);
				cv::rectangle(frame, rect, color);
				peoples++;
			}
		}

		cv::imshow("frame", frame);
		cv::imshow("detection", cleanedFrame);
		std::cout << "Peoples detected : " << peoples << std::endl;
		if (cv::waitKey(5) >= 0)
			break;
	}

	std::cout << "video completed succesfully" << std::endl;
	return 0;
}
