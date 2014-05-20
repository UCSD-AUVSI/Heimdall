/**
 * @file Utils_SharedUtils_OpenCV.cpp
 * @brief Utility functions related to manipulation of OpenCV color spaces and channels
 * @author Jason Bunk
 */

#include "SharedUtils/SharedUtils_OpenCV.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include <iostream>
#include <string>


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
cv::Mat Average_Several_SingleChannel_CVMats(std::vector<cv::Mat>* input_mats, float max_acceptable_fractional_difference/*=1.0f*/)
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
            consoleOutput.Level2() << std::string("SEG averaging normed err: ") << to_sstring(single_total_error_value) << std::endl;
            //============================================



            if(single_total_error_value > 1.0f)
                consoleOutput.Level0() << std::endl << std::endl << std::string("something weird happened when calculating the error of an averaged cv::Mat") << std::endl << std::endl << std::endl;


            if(single_total_error_value > max_acceptable_fractional_difference)
            {
                retval_average.release();
                retval_average = cv::Mat();

                consoleOutput.Level1() << "Average_Several_SingleChannel_CVMats() said there was too much difference between the Mats, so couldn't find an accurate average, so returned nothing!" << std::endl;
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
