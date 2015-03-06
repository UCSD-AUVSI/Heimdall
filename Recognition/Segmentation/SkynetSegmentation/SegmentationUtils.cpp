#include "SegmentationUtils.hpp"
#include "SharedUtils/SharedUtils_OpenCV.hpp"

void SaveBlobColorCheckImage(cv::Mat original_image,
							cv::Mat segmented_blob,
							cv::Scalar color,
							std::string name_of_image)
{
	cv::Mat background;
	original_image.copyTo(background);
	background.setTo(color);
	original_image.copyTo(background, segmented_blob);
	
	saveImage(background, name_of_image);
}


cv::Mat CreateBlobColorCheckImage(cv::Mat original_image,
							cv::Mat segmented_blob,
							cv::Scalar color)
{
	cv::Mat background;
	original_image.copyTo(background);
	background.setTo(color);
	original_image.copyTo(background, segmented_blob);
	
	return background;
}
