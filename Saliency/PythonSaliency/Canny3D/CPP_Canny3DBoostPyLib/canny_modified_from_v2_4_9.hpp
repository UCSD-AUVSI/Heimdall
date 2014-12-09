#ifndef __CANNY_MODIFIED_FROM_V2_4_9__HPP__
#define __CANNY_MODIFIED_FROM_V2_4_9__HPP__

/*
	Note: INPUTS MUST BE NORMALIZED FROM 0 TO 255 !!!!!!!
	
	(i.e. even for floating point, NOT from 0 to 1)
*/

void cppCannyBunk_RGB( const cv::Mat& image, cv::Mat& edges,
                double threshold1, double threshold2,
                int apertureSize );
                
void cppCannyBunk_CIELAB( const cv::Mat& image, cv::Mat& edges,
                double threshold1, double threshold2,
                int apertureSize );

#endif
