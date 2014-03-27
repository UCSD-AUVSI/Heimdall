#pragma once

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <vector>


class Segmentation_CSEG_SSEG_Merger
{
public:
	static void DoModule(cv::Mat cropped_target_image,
        std::vector<cv::Mat>* input_SSEGs,
        std::vector<cv::Scalar>* input_sseg_colors,
        std::vector<cv::Mat>* input_CSEGs,
        std::vector<cv::Scalar>* input_cseg_colors);
};
