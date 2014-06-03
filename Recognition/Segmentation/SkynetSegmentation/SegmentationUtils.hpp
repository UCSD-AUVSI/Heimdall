#pragma once

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <vector>

void SaveBlobColorCheckImage(cv::Mat original_image,
							cv::Mat segmented_blob,
							cv::Scalar color,
							std::string name_of_image);
							
cv::Mat CreateBlobColorCheckImage(cv::Mat original_image,
							cv::Mat segmented_blob,
							cv::Scalar color);
