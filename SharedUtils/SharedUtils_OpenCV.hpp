#pragma once

#include <string>
#include <vector>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>


typedef float			 myColor_1f;
typedef cv::Vec<float,2> myColor_2f;
typedef cv::Vec<float,3> myColor_3f;
typedef unsigned char	 		 myColor_1ub;
typedef cv::Vec<unsigned char,2> myColor_2ub;
typedef cv::Vec<unsigned char,3> myColor_3ub;

float GetColorEuclideanLength(myColor_3f input);
float GetColorEuclideanLength(myColor_3f c1, myColor_3f c2);


void ConvertMat_UsingSettings(cv::Mat & source_mat, cv::Mat & destination_mat, int preprocess_CV_conversion_type, bool const*const which_channels_to_keep, bool delete_unwanted_channels);

std::string GetNameOfCVColorSpace(int CV_colorspace_conversion_type);

#define COLORSPACE_CONVERSIONTYPE_KEEPRGB -1234567


cv::Mat Average_Several_SingleChannel_CVMats(std::vector<cv::Mat>* input_mats, float max_acceptable_fractional_difference=1.00001f, std::string error_msg_comment="");
cv::Scalar Average_Several_CVColors(std::vector<cv::Scalar>* input_colors);

cv::Mat FillInteriorsOfBlob(cv::Mat img, unsigned char fill_color);

double GetLengthOfCVScalar(cv::Scalar input);

int GetContourOfGreatestArea(std::vector<std::vector<cv::Point>> & contours,
                            double* returned_area_of_largest=nullptr,
                            double* returned_total_area=nullptr);

int GetAreaIntersectionOf_CSEG_in_the_bounding_convex_polygon_of_SSEG(cv::Mat & image_where_contours_came_from,
									std::vector<cv::Point> & contour__SSEG,
									std::vector<std::vector<cv::Point>> & contours__CSEG,
									int& returned_area_of_CSEG);

int GetIntersectionAreaOfContours(	cv::Mat & image_where_contours_came_from,
									std::vector<std::vector<cv::Point>> & first_contours,
									std::vector<std::vector<cv::Point>> & second_contours);

void saveImage(cv::Mat& img, std::string filename);

void Rotate_CV_Mat(cv::Mat& src, double angle, cv::Mat& dst);

void cv_convertToFloatingType(cv::Mat & processMe);
void cv_matchTemplate_ExpectingNearlyExactMatch(cv::Mat bigImage, cv::Mat littleTemplate, cv::Mat & resultOutput);



