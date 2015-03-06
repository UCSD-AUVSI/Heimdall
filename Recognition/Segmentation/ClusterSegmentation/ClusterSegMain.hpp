#pragma once

#include <opencv/cv.h>
#include <opencv/highgui.h>

void DoModule_ClusterSeg(cv::Mat input_cropped_target_image,
						cv::Mat* returned_SSEG,
						cv::Scalar* returned_SSEG_color,
						cv::Mat* returned_CSEG,
						cv::Scalar* returned_CSEG_color,
						
						std::string* folder_path_of_output_saved_images=nullptr,
						bool save_images_and_results=false,
						std::string* name_of_target_image=nullptr);
