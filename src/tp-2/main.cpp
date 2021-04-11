#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/video.hpp>
#include <iostream>

#include "tracy/Tracy.hpp"
#include "vibe-background-sequential.h"

#define TRACY_NO_EXIT

int main(int argc, char** argv)
{
	cv::Mat frame;
	cv::Mat segmentationMap;
	cv::Mat temp;
	cv::Mat kernel1(5, 5, CV_8U, 1);
	cv::Mat kernel2(40, 40, CV_8U, 1);
	cv::Mat labels;
	cv::Mat stats;
	cv::Mat centroids;
	vibeModel_Sequential_t* model = nullptr;
	cv::VideoCapture capture;

	capture.open(0);
	if (!capture.isOpened())
	{
		std::cout << "No capture" << std::endl;
		return 0;
	}
	capture.set(cv::CAP_PROP_AUTO_EXPOSURE, -1);

	bool isFirstFrame = true;
	std::cout << "Capture is opened" << std::endl;
	for (;;)
	{
		ZoneScopedN("MainLoop")

		capture.read(frame);
		if (frame.empty())
			break;

		cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);

		// Init vibe
		if (isFirstFrame)
		{
			isFirstFrame = false;
			segmentationMap = cv::Mat(frame.rows, frame.cols, CV_8UC1);
			model = libvibeModel_Sequential_New();
			libvibeModel_Sequential_AllocInit_8u_C1R(model, frame.data, frame.cols, frame.rows);
		}
		
		// Apply Vibe background substraction
		libvibeModel_Sequential_Segmentation_8u_C1R(model, frame.data, segmentationMap.data);
		libvibeModel_Sequential_Update_8u_C1R(model, frame.data, segmentationMap.data);
		cv::medianBlur(segmentationMap, segmentationMap, 3);

		cv::morphologyEx(segmentationMap, temp, cv::MORPH_OPEN, kernel1); // opening to remove noise
		cv::morphologyEx(temp, segmentationMap, cv::MORPH_CLOSE, kernel2); // closing to fill gaps
		cv::connectedComponentsWithStats(segmentationMap, labels, stats, centroids); // count the number of connected components

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
		cv::imshow("Segmentation", segmentationMap);
		std::cout << "Peoples detected : " << peoples << std::endl;
		if (cv::waitKey(5) >= 0)
			break;
	}
	capture.release();
	libvibeModel_Sequential_Free(model);

	std::cout << "video completed succesfully" << std::endl;
	return 0;
}
