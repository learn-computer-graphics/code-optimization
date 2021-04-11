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

void convertRGBToGrayscale(const cv::Mat& in, cv::Mat& out)
{
	ZoneScoped
	assert(in.cols == out.cols);
	assert(in.rows == out.rows);

	for (int i = 0; i < in.rows; i++)
	{
		for (int j = 0; j < in.cols; j++)
		{
			out.data[i * out.step + j] = 0.114 * in.data[i * in.step + j * 3] + 0.587 * in.data[i * in.step + j * 3 + 1] + 0.299 * in.data[i * in.step + j * 3 + 2];
		}
	}
}

int main(int argc, char** argv)
{
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
	const int width = capture.get(cv::CAP_PROP_FRAME_WIDTH);
	const int height = capture.get(cv::CAP_PROP_FRAME_HEIGHT);
	cv::Mat frame3C = cv::Mat(height, width, CV_8UC3);
	cv::Mat frame1C = cv::Mat(height, width, CV_8UC1);
	cv::Mat segmentationMap = cv::Mat(height, width, CV_8UC1);

	bool isFirstFrame = true;
	std::cout << "Capture is opened" << std::endl;
	for (;;)
	{
		ZoneScopedN("MainLoop")

		capture.read(frame3C);
		if (frame3C.empty())
			break;

		{
			ZoneScopedN("ConvertToGrayscale")
			// cv::cvtColor(frame3C, frame1C, cv::COLOR_BGR2GRAY);
			convertRGBToGrayscale(frame3C, frame1C);
		}
		
		// Init vibe
		if (isFirstFrame)
		{
			isFirstFrame = false;
			model = libvibeModel_Sequential_New();
			libvibeModel_Sequential_AllocInit_8u_C1R(model, frame1C.data, frame1C.cols, frame1C.rows);
		}
		
		// Apply Vibe background substraction
		libvibeModel_Sequential_Segmentation_8u_C1R(model, frame1C.data, segmentationMap.data);
		libvibeModel_Sequential_Update_8u_C1R(model, frame1C.data, segmentationMap.data);
		cv::medianBlur(segmentationMap, segmentationMap, 3);

		cv::morphologyEx(segmentationMap, frame1C, cv::MORPH_OPEN, kernel1); // opening to remove noise
		cv::morphologyEx(frame1C, segmentationMap, cv::MORPH_CLOSE, kernel2); // closing to fill gaps
		cv::connectedComponentsWithStats(segmentationMap, labels, stats, centroids); // count the number of connected components

		int peoples = 0;
		for (int i = 0; i < stats.rows; i++)
		{
			const int x = stats.at<int>(cv::Point(0, i));
			const int y = stats.at<int>(cv::Point(1, i));
			const int w = stats.at<int>(cv::Point(2, i));
			const int h = stats.at<int>(cv::Point(3, i));

			const bool isAcceptableSize = w < frame1C.rows - 50 && w > 50;
			if (isAcceptableSize)
			{
				cv::Scalar color(255, 0, 0);
				cv::Rect rect(x, y, w, h);
				cv::rectangle(frame3C, rect, color);
				peoples++;
			}
		}

		cv::imshow("frame", frame3C);
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
