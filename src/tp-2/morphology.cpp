// Original Author: Hairong Qi (C) hqi@utk.edu at 02/06/06, from http://web.eecs.utk.edu/~hqi/ece472-572/code/morph.cpp
// Modified for performance and simplicity

#include <iostream>
#include <cstdlib>
#include <cmath>

#include "morphology.h"
#include "tracy/Tracy.hpp"

using namespace std;

void bdilate(const cv::Mat& in, cv::Mat& out, const cv::Mat& se) 
{
    ZoneScoped

    for (int i = 0; i < in.rows; i++)
    {
        for (int j = 0; j < in.cols; j++)
        {
            if (in.data[i * in.step + j] > 0)
            {
                // if the foreground pixel is not zero, then fill in the pixel
                // covered by the s.e.
                for (int m = 0; m < se.rows; m++)
                {
                    for (int n = 0; n < se.cols; n++) 
                    {
                        if ((i + m) >= 0 && (j + n) >= 0 && 
                            (i + m) < in.rows && (j + n) < in.cols)
                        {
                            const int index = (i + m) * in.step + (j + n);
                            if (out.data[index] == 0)
                            {
                                out.data[index] = (se.data[m * se.step + n] > 0) ? 255 : 0;
                            }
                        }
                    }
                }
            }
        }
    }
}

void berode(const cv::Mat& in, cv::Mat& out, const cv::Mat& se)
{
    ZoneScoped

    // count the number of foreground pixels in the s.e.
    int sum = 0;
    for (int i = 0; i < se.rows; i++)
    {
        for (int j = 0; j < se.cols; j++)
        {
            if (se.data[i * se.step + j] > 0)
                sum++;
        }
    }

    for (int i = 0; i < in.rows; i++)
    {
        for (int j = 0; j < in.cols; j++) 
        {
            if (in.data[i * in.step + j] > 0)
            {     
                int count = 0;
                for (int m = 0; m < se.rows; m++)
                {
                    for (int n = 0; n < se.cols; n++) 
                    {
                        if ((i + m) >= 0 && (j + n) >= 0 && (i + m) < in.rows && (j + n) < in.cols)
                            count += in.data[(i + m) * in.step + j + n] > 0 && se.data[m * se.step + n] > 0;
                    }
                }

                // if all se foreground pixels are included in the foreground of 
                // the current pixel's neighborhood, then enable this pixel in erosion
                if (sum == count)
                    out.data[i * in.step + j] = 255;
            }
        }
    }
}

