/**
 * @file Utils_SharedUtils_OpenCV.cpp
 * @brief Utility functions related to manipulation of OpenCV color spaces and channels
 * @author Jason Bunk
 */

#include "SharedUtils/SharedUtils_OpenCV.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include <iostream>
#include <string>


float GetColorEuclideanLength(myColor_3f input)
{
	return sqrt(input[0]*input[0] + input[1]*input[1] + input[2]*input[2]);
}
float GetColorEuclideanLength(myColor_3f c1, myColor_3f c2)
{
	return sqrt(  pow(c1[0]-c2[0],2)
				+ pow(c1[1]-c2[1],2)
				+ pow(c1[2]-c2[2],2));
}

/**
 * This function can manipulate color spaces/channels of cv::Mat.
 * For instance, extracting just the AB channels of color space LAB (a very useful manipulation),
 * or extracting just the Hue channel of HSV.
 */

void ConvertMat_UsingSettings(cv::Mat & source_mat, cv::Mat & destination_mat, int preprocess_CV_conversion_type, bool const*const which_channels_to_keep, bool delete_unwanted_channels)
{
    if(preprocess_CV_conversion_type >= 0)
    {
        cv::cvtColor(source_mat, destination_mat, preprocess_CV_conversion_type);
        std::vector<cv::Mat> mat_channels(3);
        cv::split(destination_mat, mat_channels);


        if(delete_unwanted_channels == false)
        {
            if(which_channels_to_keep[0] == false)
                mat_channels[0] = cv::Scalar(0);

            if(which_channels_to_keep[1] == false)
                mat_channels[1] = cv::Scalar(0);

            if(which_channels_to_keep[2] == false)
                mat_channels[2] = cv::Scalar(0);
        }
        else
        {
            if(which_channels_to_keep[0] == false)
            {
                mat_channels.erase(mat_channels.begin());

                if(which_channels_to_keep[1] == false)
                {
                    mat_channels.erase(mat_channels.begin());
                }
                else if(which_channels_to_keep[2] == false)
                {
                    mat_channels.erase(mat_channels.begin() + 1);
                }
            }
            else if(which_channels_to_keep[1] == false)
            {
                mat_channels.erase(mat_channels.begin() + 1);

                if(which_channels_to_keep[2] == false)
                    mat_channels.erase(mat_channels.begin() + 1);
            }
            else if(which_channels_to_keep[2] == false)
                mat_channels.erase(mat_channels.begin() + 2);
        }


        cv::merge(mat_channels, destination_mat);
    }
    else if((&source_mat) != (&destination_mat)) //if source and destination are different mats (OpenCV can handle this case anyway)
    {
        source_mat.copyTo(destination_mat);
    }
}


std::string GetNameOfCVColorSpace(int CV_colorspace_conversion_type)
{
    switch(CV_colorspace_conversion_type)
    {
        case CV_BGR2Lab:
            return "CieLab";
        case CV_BGR2Luv:
            return "CieLuv";
        case CV_BGR2HSV:
            return "HSV";
        case CV_BGR2HLS:
            return "HLS";
        case COLORSPACE_CONVERSIONTYPE_KEEPRGB:
            return "RGB";
    }

    return to_istring(CV_colorspace_conversion_type);
}


//Returns an averaged cv::Mat in floating point format (CV_32F)
//
cv::Mat Average_Several_SingleChannel_CVMats(std::vector<cv::Mat>* input_mats, float max_acceptable_fractional_difference/*=1.0f*/, std::string error_msg_comment/*=""*/)
{
    if(input_mats != nullptr && input_mats->empty()==false)
    {
        std::vector<cv::Mat>::iterator inputsegs_iter = input_mats->begin();

        cv::Mat temp_add_this;
        cv::Mat retval_average(cv::Size(inputsegs_iter->cols, inputsegs_iter->rows), CV_32F, cv::Scalar(0.0, 0.0, 0.0, 0.0));

        for(; inputsegs_iter != input_mats->end(); inputsegs_iter++)
        {
            inputsegs_iter->convertTo(temp_add_this, CV_32F); //don't modify the original mats... convert to floating point, then place in the temp mat

            retval_average += temp_add_this;
        }
        retval_average /= static_cast<float>(input_mats->size());


        if(max_acceptable_fractional_difference < 1.0f && max_acceptable_fractional_difference > 0.0f)
        {
            inputsegs_iter = input_mats->begin();


            //get temp_add_this to be the averaged mat, but in the same type as the input source
            if(inputsegs_iter->type() != retval_average.type())
                retval_average.convertTo(temp_add_this, inputsegs_iter->type());
            else
                temp_add_this = retval_average;


            cv::Mat current_summed_error;
            cv::Mat total_summed_errors;
            temp_add_this.copyTo(total_summed_errors);
            total_summed_errors.setTo(0);


            for(; inputsegs_iter != input_mats->end(); inputsegs_iter++)
            {
                cv::absdiff(*inputsegs_iter, temp_add_this, current_summed_error);
                cv::add(current_summed_error, total_summed_errors, total_summed_errors);
            }
            cv::Scalar total_error_value = cv::sum(total_summed_errors);
            double single_total_error_value = GetLengthOfCVScalar(total_error_value);
            single_total_error_value /= static_cast<double>(input_mats->size());

            //============================================
            consoleOutput.Level4() << std::string("calculating average mat...unnormed err: ") << to_sstring(single_total_error_value);
            //============================================


            double maxVal = 0.0;
            cv::minMaxLoc(retval_average, nullptr, &maxVal);
            if(maxVal < 1.0)
                maxVal = 1.0;
            else
                maxVal = 255.0;

            single_total_error_value /= maxVal; //if the input was a binary image, this is now the # of pixels that are wrong (error was 0 or 1)
            //if it wasn't a binary image, this is now the total normalized error,
            //                      where each pixel could have an error between 0 and 1


            //============================================
            consoleOutput.Level4() << std::string(", 1st normed err: ") << to_sstring(single_total_error_value);
            //============================================


            //now normalize to the whole image... if it was binary, and 50% of pixels were wrong, this should become 0.5
            //
            single_total_error_value /= static_cast<double>(retval_average.cols * retval_average.rows);



            //============================================
            consoleOutput.Level4() << std::string(", final ");
            consoleOutput.Level2() << std::string("averaging normed err: ") << to_sstring(single_total_error_value) << ",  message: " << error_msg_comment << std::endl;
            //============================================



            if(single_total_error_value > 1.0f)
                consoleOutput.Level0() << std::endl << std::endl << std::string("something weird happened when calculating the error of an averaged cv::Mat") << std::endl << std::endl << std::endl;


            if(single_total_error_value > max_acceptable_fractional_difference)
            {
                retval_average.release();
                retval_average = cv::Mat();

                consoleOutput.Level1() << "Average_Several_SingleChannel_CVMats() said: couldn't find an accurate average, so returned nothing!" << std::endl;
                consoleOutput.Level1() << "Average_Several_SingleChannel_CVMats() message: " << error_msg_comment << std::endl;
            }
        }


        return retval_average;
    }

    return cv::Mat();
}


cv::Scalar Average_Several_CVColors(std::vector<cv::Scalar>* input_colors)
{
    if(input_colors != nullptr && input_colors->empty()==false)
    {
        cv::Scalar retval(0.0, 0.0, 0.0, 0.0);

        std::vector<cv::Scalar>::iterator iter = input_colors->begin();
        for(; iter != input_colors->end(); iter++)
        {
            retval += (*iter);
        }
        retval /= static_cast<double>(input_colors->size());
        return retval;
    }
    return cv::Scalar();
}


cv::Mat FillInteriorsOfBlob(cv::Mat img, unsigned char fill_color)
{
	if(img.type() == CV_8U) {
        //algorithm: do flood fill outside the shape; then everything that wasn't filled by this, is now the shape
        //first, add a 1-pixel-wide black border around the entire image
        //use that border to start the flood fill, and the flood fill will creep inward
        //
		cv::Mat largermat(img.rows+2, img.cols+2, CV_8U, cv::Scalar(0));
        cv::Rect toCrop = cv::Rect(1, 1, img.cols, img.rows);
        cv::Mat submatrix_of_largermat_that_represents_crop = largermat(toCrop);
        img.copyTo(submatrix_of_largermat_that_represents_crop);
        
        cv::floodFill(largermat, cv::Point(0,0), 1);
        
        cv::Mat outsideShape = (submatrix_of_largermat_that_represents_crop - img);
        
		submatrix_of_largermat_that_represents_crop.setTo(fill_color, outsideShape == 0);
		return submatrix_of_largermat_that_represents_crop;
	}
	else {
		consoleOutput.Level1() << "warning: FillInteriorOfBlob() was called, but it refused to fill it!" << std::endl;
	}
	return img;
}


double GetLengthOfCVScalar(cv::Scalar input)
{
    return sqrt(input[0]*input[0] + input[1]*input[1] + input[2]*input[2] + input[3]*input[3]);
}



int GetContourOfGreatestArea(std::vector<std::vector<cv::Point>> & contours,
        double* returned_area_of_largest/*=nullptr*/,
        double* returned_total_area/*=nullptr*/)
{
    if(contours.size() > 1)
    {
        std::map<int, double> areas;
        int aaa=0; int contourssize=contours.size();
        for(; aaa < contourssize; aaa++)
        {
            areas[aaa] = cv::contourArea(contours[aaa]);

            if(returned_total_area != nullptr)
            {
                (*returned_total_area) += areas[aaa];
            }
        }
        int largest_contour=0;
        double largest_contour_area=0.0;
        for(aaa=0; aaa < contourssize; aaa++)
        {
            if(areas[aaa] > largest_contour_area)
            {
                largest_contour = aaa;
                largest_contour_area = areas[aaa];
            }
        }

        if(returned_area_of_largest != nullptr)
            (*returned_area_of_largest) = largest_contour_area;

        return largest_contour;
    }
    else if(contours.size() == 1)
    {
        if(contours.empty() == false && returned_area_of_largest != nullptr)
            (*returned_area_of_largest) = cv::contourArea(contours[0]);

        if(returned_total_area != nullptr)
        {
            if(returned_area_of_largest != nullptr)
                (*returned_total_area) = (*returned_area_of_largest);
            else
                (*returned_total_area) = cv::contourArea(contours[0]);
        }
        return 0; //first in the array
    }
    else
    {
        if(returned_area_of_largest != nullptr)
            (*returned_area_of_largest) = 0.0;

        if(returned_total_area != nullptr)
            (*returned_total_area) = 0.0;

        return 0; //first in the array
    }
}


int GetAreaIntersectionOf_CSEG_in_the_bounding_convex_polygon_of_SSEG(cv::Mat & image_where_contours_came_from,
									std::vector<cv::Point> & contour__SSEG,
									std::vector<std::vector<cv::Point>> & contours__CSEG,
									int& returned_area_of_CSEG)
{
	cv::Mat drawn_SSEG_contour(image_where_contours_came_from.rows,
								image_where_contours_came_from.cols,
								CV_8U,
								cv::Scalar(0));
	cv::Mat drawn_CSEG_contours;
	drawn_SSEG_contour.copyTo(drawn_CSEG_contours);
	
#if 0
	cv::Mat drawn_SSEG_contour_not_convex;
	drawn_SSEG_contour.copyTo(drawn_SSEG_contour_not_convex);
#endif
	
	std::vector<cv::Point> output_convex_hull;
	cv::convexHull(contour__SSEG, output_convex_hull);
	
	if(output_convex_hull.size() > 2) {
		std::vector<std::vector<cv::Point>> output_convex_hull_vecvec;
		output_convex_hull_vecvec.push_back(output_convex_hull);
		
		cv::drawContours(drawn_SSEG_contour, output_convex_hull_vecvec, -1, 1, CV_FILLED);
		cv::drawContours(drawn_CSEG_contours, contours__CSEG, -1, 1, CV_FILLED);
		
		cv::Mat intersection_of_contours;
		cv::bitwise_and(drawn_SSEG_contour, drawn_CSEG_contours, intersection_of_contours);
		
#if 0
		std::vector<std::vector<cv::Point>> contour__SSEG_vecvec;
		contour__SSEG_vecvec.push_back(contour__SSEG);
		cv::drawContours(drawn_SSEG_contour_not_convex, contour__SSEG_vecvec, -1, 1, CV_FILLED);
		
		cv::Mat drawtesterr;
		drawn_SSEG_contour.copyTo(drawtesterr);
		drawtesterr.setTo(0);
		
		drawtesterr += (drawn_SSEG_contour_not_convex * 63);
		drawtesterr += (drawn_SSEG_contour * 63);
		drawtesterr += (drawn_CSEG_contours * 63);
		drawtesterr += (intersection_of_contours * 63);
		
		cv::imshow("both contours, and their intersection:", drawtesterr);
		cv::waitKey(0);
		cv::destroyAllWindows();
#endif
		returned_area_of_CSEG = cv::countNonZero(drawn_CSEG_contours);
		return cv::countNonZero(intersection_of_contours);
	}
	consoleOutput.Level1() << "weird SSEG didn't have a convex hull??" << std::endl;
	return 0;
}


int GetIntersectionAreaOfContours(	cv::Mat & image_where_contours_came_from,
									std::vector<std::vector<cv::Point>> & first_contours,
									std::vector<std::vector<cv::Point>> & second_contours)
{
	cv::Mat first_drawn_contours(image_where_contours_came_from.rows,
								image_where_contours_came_from.cols,
								CV_8U,
								cv::Scalar(0));
	cv::Mat second_drawn_contours;
	first_drawn_contours.copyTo(second_drawn_contours);
	
	cv::drawContours(first_drawn_contours,  first_contours, -1, 1, CV_FILLED);
	cv::drawContours(second_drawn_contours, second_contours, -1, 1, CV_FILLED);
	
	cv::Mat intersection_of_contours;
	cv::bitwise_and(first_drawn_contours, second_drawn_contours, intersection_of_contours);
	
#if 0
	cv::Mat drawtesterr;
	first_drawn_contours.copyTo(drawtesterr);
	drawtesterr.setTo(0);
	
	drawtesterr += (first_drawn_contours * 84);
	drawtesterr += (second_drawn_contours * 84);
	drawtesterr += (intersection_of_contours * 84);
	
	cv::imshow("both contours, and their intersection:", drawtesterr);
	cv::waitKey(0);
	cv::destroyAllWindows();
#endif
	
	return cv::countNonZero(intersection_of_contours);
}

void saveImage(cv::Mat& img, std::string filename)
{
    if(img.channels() == 2)
    {
        std::vector<cv::Mat> mat_2channels(2);
        std::vector<cv::Mat> mat_3channels(3);

        cv::Mat new_mat_to_write;
        img.copyTo(new_mat_to_write);

        cv::split(new_mat_to_write, mat_2channels);
        mat_3channels[0] = mat_2channels[0];
        mat_3channels[1] = mat_2channels[1];
        mat_3channels[2] = cv::Mat(img.rows, img.cols, CV_32FC1, cv::Scalar(0.0f));
        cv::merge(mat_3channels, new_mat_to_write);

        cv::imwrite(filename, new_mat_to_write);
    }
    else
        cv::imwrite(filename, img);
}

void Rotate_CV_Mat(cv::Mat& src, double angle, cv::Mat& dst)
{
	int len = std::max(src.cols, src.rows);
	cv::Point2f pt(len/2., len/2.);
	cv::Mat r = cv::getRotationMatrix2D(pt, angle, 1.0);
	cv::warpAffine(src, dst, r, cv::Size(len, len));
}

void cv_convertToFloatingType(cv::Mat & processMe)
{
	if(processMe.channels() == 1) {
		processMe.convertTo(processMe,CV_32FC1);
	} else if(processMe.channels() == 2) {
		processMe.convertTo(processMe,CV_32FC2);
	} else if(processMe.channels() == 3) {
		processMe.convertTo(processMe,CV_32FC3);
	}
}

void cv_matchTemplate_ExpectingNearlyExactMatch(cv::Mat bigImage, cv::Mat littleTemplate, cv::Mat & resultOutput)
{
	assert(bigImage.rows > littleTemplate.rows);
	assert(bigImage.cols > littleTemplate.cols);
	assert(bigImage.type() == littleTemplate.type());
	assert(bigImage.channels() == littleTemplate.channels());
	assert( bigImage.type() == CV_8UC1 ||
			bigImage.type() == CV_8UC2 ||
			bigImage.type() == CV_8UC3);/* ||
			bigImage.type() == CV_32FC1 ||
			bigImage.type() == CV_32FC2 ||
			bigImage.type() == CV_32FC3);*/
	
	cv_convertToFloatingType(littleTemplate);
	cv_convertToFloatingType(bigImage);
	
	/*if(bigImage.type() == CV_8UC1) {
		bigImage.convertTo(bigImage,CV_32SC1); //signed 32-bit int
	} else if(bigImage.type() == CV_8UC2) {
		bigImage.convertTo(bigImage,CV_32SC2);
	} else if(bigImage.type() == CV_8UC3) {
		bigImage.convertTo(bigImage,CV_32SC3);
	}
	if(littleTemplate.type() == CV_8UC1) {
		littleTemplate.convertTo(littleTemplate,CV_32SC1);
	} else if(littleTemplate.type() == CV_8UC2) {
		littleTemplate.convertTo(littleTemplate,CV_32SC2);
	} else if(littleTemplate.type() == CV_8UC3) {
		littleTemplate.convertTo(littleTemplate,CV_32SC3);
	}
	
	/*std::cout << "littleTemplate.channels() == " << littleTemplate.channels() << std::endl;
	std::cout << "bigImage.channels() == " << bigImage.channels() << std::endl;
	std::cout << "littleTemplate.at<float>(0,0,0) == " << littleTemplate.at<cv::Vec3f>(0,0)[0] << std::endl;
	std::cout << "littleTemplate.at<float>(0,0,1) == " << littleTemplate.at<cv::Vec3f>(0,0)[1] << std::endl;
	std::cout << "bigImage.at<float>(0,0,0) == " << bigImage.at<float>(0,0,0) << std::endl;
	std::cout << "bigImage.at<float>(0,0,1) == " << bigImage.at<float>(0,0,1) << std::endl;*/
	
	int nchannels = littleTemplate.channels();
	
	resultOutput = cv::Mat(bigImage.rows - littleTemplate.rows + 1, bigImage.cols - littleTemplate.cols + 1, CV_32F);
	float thisdiff;
	float thisresult;
	
	if(nchannels == 3) {
		for(int ii=0; ii<resultOutput.rows; ii++) {
			for(int jj=0; jj<resultOutput.cols; jj++) {
				for(int iT=0; iT<littleTemplate.rows; iT++) {
					for(int jT=0; jT<littleTemplate.cols; jT++) {
						for(int kk=0; kk<nchannels; kk++) {
							thisdiff = littleTemplate.at<cv::Vec3f>(iT,jT)[kk] - bigImage.at<cv::Vec3f>(ii+iT,jj+jT)[kk];
							thisresult = (resultOutput.at<float>(ii,jj) += (thisdiff*thisdiff));
						}
						
						//this skips clearly falsly matching templates,
						//but if it takes forever, a correct match will also take forever!!
						
						if(thisresult > 200000.0f) {
							iT = littleTemplate.rows;
							jT = littleTemplate.cols;
						}
					}
				}
			}
			//if(ii % 10 == 0)
				std::cout << "rows done: " << ii << std::endl;
		}
	}
}





