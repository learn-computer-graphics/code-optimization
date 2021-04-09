#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/video.hpp>
#include <iostream>

/**
 * @brief Detects number of peoples in the area and draw it as rectangles
 * @returns int - number of people detected
 */
int detectAndDraw(const cv::HOGDescriptor& hog, cv::Mat& img)
{
	std::vector<cv::Rect> found, found_filtered;
	double time = static_cast<double>(cv::getTickCount());
	hog.detectMultiScale(img, found, 0, cv::Size(8, 8), cv::Size(16, 16), 1.07, 2);
	time = static_cast<double>(cv::getTickCount()) - time;
	std::cout << "detection time = " << (time * 1000. / cv::getTickFrequency()) << " ms" << std::endl;

	for (size_t i = 0; i < found.size(); i++)
	{
		cv::Rect r = found[i];
		size_t j;
		// Do not add small detections inside a bigger detection.
		for (j = 0; j < found.size(); j++)
			if (j != i && (r & found[j]) == r)
				break;
		if (j == found.size())
			found_filtered.push_back(r);
	}

	for (size_t i = 0; i < found_filtered.size(); i++)
	{
		cv::Rect r = found_filtered[i];
		// The HOG detector returns slightly larger rectangles than the real objects,
		// so we slightly shrink the rectangles to get a nicer output.
		r.x += cvRound(r.width * 0.1);
		r.width = cvRound(r.width * 0.8);
		r.y += cvRound(r.height * 0.07);
		r.height = cvRound(r.height * 0.8);
		rectangle(img, r.tl(), r.br(), cv::Scalar(0, 255, 0), 3);
	}

	return found_filtered.size();
}

int main(int argc, char** argv)
{
	cv::HOGDescriptor hog;
	hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());
	std::cout << "Built with OpenCV " << CV_VERSION << std::endl;
	cv::Mat cam_image;
	cv::VideoCapture capture;

	capture.open(0);
	if (!capture.isOpened())
	{
		std::cout << "No capture" << std::endl;
		return 0;
	}

	std::cout << "Capture is opened" << std::endl;
	for (;;)
	{
		capture.read(cam_image);
		if (cam_image.empty())
			break;

		int num_people = detectAndDraw(hog, cam_image);
		std::cout << "People count: " << num_people << std::endl;
		cv::imshow("HOG Descriptor based People counter", cam_image);
		if (cv::waitKey(5) >= 0)
			break;
	}

	std::cout << "video completed succesfully" << std::endl;
	return 0;
}
