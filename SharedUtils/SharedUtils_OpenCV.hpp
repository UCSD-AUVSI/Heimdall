#pragma once

#include <string>
#include <vector>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>


void ConvertMat_UsingSettings(cv::Mat & source_mat, cv::Mat & destination_mat, int preprocess_CV_conversion_type, bool const*const which_channels_to_keep, bool delete_unwanted_channels);

std::string GetNameOfCVColorSpace(int CV_colorspace_conversion_type);

#define COLORSPACE_CONVERSIONTYPE_KEEPRGB -1234567


cv::Mat Average_Several_SingleChannel_CVMats(std::vector<cv::Mat>* input_mats, float max_acceptable_fractional_difference=1.00001f);
cv::Scalar Average_Several_CVColors(std::vector<cv::Scalar>* input_colors);

double GetLengthOfCVScalar(cv::Scalar input);

int GetContourOfGreatestArea(std::vector<std::vector<cv::Point>> & contours,
                            double* returned_area_of_largest=nullptr,
                            double* returned_total_area=nullptr);

void saveImage(cv::Mat& img, std::string filename);
