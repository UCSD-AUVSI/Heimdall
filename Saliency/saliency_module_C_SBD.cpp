/**
 * @file saliency_module_canny.cpp
 * @brief Saliency algorithm based on Canny edge detection and cv::SimpleBlobDetector
 * @author Eric Lo, Jason Bunk
 */

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "SharedUtils/SharedUtils.hpp"
#include "Saliency/saliency_module_C_SBD.hpp"


std::vector<cv::Mat>& SaliencyModule_C_SBD::do_saliency(cv::Mat input_image)
{
    returned_cropped_images.clear();

    std::vector<SaliencySettings>::iterator siter = settings.begin();
    for(; siter != settings.end(); siter++)
    {
        do_saliency_with_setting(input_image, *siter);
    }


    if(write_output_to_folder && returned_cropped_images.empty() == false)
    {
        int img_num=0;
        for(std::vector<cv::Mat>::iterator rciter = returned_cropped_images.begin(); rciter != returned_cropped_images.end(); rciter++)
        {
            std::string filename = output_folder_to_save_crops + std::string("/zsal_") + to_istring(write_output_to_folder_name_incrementer) + std::string("_roi_") + to_istring(img_num) + std::string(".jpg");
            cv::imwrite(filename.c_str(), *rciter);

            std::cout << std::endl << filename << std::endl << std::endl;

            img_num++;
        }
    }
    write_output_to_folder_name_incrementer++;


    return returned_cropped_images;
}


void SaliencyModule_C_SBD::do_saliency_with_setting(cv::Mat input_image, SaliencySettings& setting)
{
  cv::Mat resized, dst, src_gray, detected_edges;
  std::string filenamestr;


if(input_image.cols < 20 || input_image.rows < 20)
    return;


  /// Step 1: resize to 100x smaller (start w/ 18MP)
  cv::resize(input_image, resized, cv::Size(0,0), 0.1, 0.1);

  /// Create a matrix of the same type and size as resized (for dst)
  dst.create( resized.size(), resized.type() );


  ConvertMat_UsingSettings(resized, resized, setting.ColorConvert_CV_color_space, setting.ColorConvert_desired_CV_color_channels, false);


    if(write_output_to_folder && write_ALL_output_not_just_the_crops)
    {
        cv::imwrite(output_folder_to_save_crops + std::string("/z_saliency") + to_istring(write_output_to_folder_name_incrementer) + std::string("_step0_after_cielab.jpg"), resized);
    }

  /// Convert the image to grayscale
  ///////////cv::vector<cv::Mat> spl;

  std::vector<cv::Mat> spl(3);
  cv::split(resized, spl);
  int channel;
  dst = cv::Scalar::all(0);
  for(channel=0; channel < 3; ++channel)
  {
     /////////////src_gray = spl[channel];
     spl[channel].copyTo(src_gray);

     /// Noise reduction
     cv::blur(src_gray, src_gray, cv::Size(3,3) );

     /// Canny detector
     cv::Canny(src_gray, detected_edges, setting.Canny_low_threshold, setting.Canny_high_threshold, setting.Canny_kernel_size);


     if(write_output_to_folder && write_ALL_output_not_just_the_crops)
     {
        filenamestr = output_folder_to_save_crops + std::string("/z_saliency_step5_after_canny") + to_istring(channel) + std::string(".jpg");
        cv::imwrite(filenamestr.c_str(), detected_edges);
     }


     /// Using Canny's output as a mask, we display our result
     resized.copyTo(dst, detected_edges);
  }


  // blob detection
  cv::SimpleBlobDetector::Params params;
  params.minDistBetweenBlobs = 10.0f;
  params.filterByInertia = false;
  params.filterByConvexity = false;
  params.filterByColor = true;
  params.filterByCircularity = false;
  params.filterByArea = true;
  params.minArea = 0.1f;
  params.maxArea = 100.0f;

  cv::Ptr<cv::FeatureDetector> blob_detector = new cv::SimpleBlobDetector(params);
  blob_detector->create("SimpleBlob");


  cv::vector<cv::KeyPoint> keypoints;
  blob_detector->detect(dst, keypoints);

  cv::Mat out;
  out.create(dst.size(), dst.type());
  cv::drawKeypoints( dst, keypoints, out, CV_RGB(0,255,0), cv::DrawMatchesFlags::DEFAULT);


  cv::Rect toCrop;
  cv::Mat cropped;

  //crop size should depend on expected target size... or be determined by the size of the blob!
  int cropsize_x, cropsize_y;
  int startx, starty;

  // crop from the full res image
  for(int i=0; i<keypoints.size(); i++)
  {
//-------------------------------------------------------
     if(write_output_to_folder)
     {
    std::cout << std::string("z_saliency") << to_istring(write_output_to_folder_name_incrementer) << std::string("_roi_")
        << to_istring(i) << std::string("  size: ") << keypoints[i].size << std::endl;
     }
//-------------------------------------------------------


    //formula for crop size:
    //  sqrt(area) converts units from (length)^2 to (length)... (best for squares, okay for anything else)
    //  then multiply this (length) unit by a scale... roughly 130 is a good starting point

    cropsize_y = cropsize_x = static_cast<int>(sqrt(keypoints[i].size) * 160.0);
    if((cropsize_x % 2) != 0)
    {
        cropsize_y = (++cropsize_x);
    }


if(cropsize_x > input_image.cols || cropsize_y > input_image.rows)
    return;


    cropped.create(cv::Size(cropsize_x,cropsize_y), out.type());


    startx = (int)keypoints[i].pt.x * 10 - (cropsize_x/2);
    starty = (int)keypoints[i].pt.y * 10 - (cropsize_y/2);
    if(startx < 0)
	startx = 0;
    if(starty < 0)
	starty = 0;
    if(startx > input_image.cols - cropsize_x)
	startx = input_image.cols - cropsize_x;
    if(starty > input_image.rows - cropsize_y)
	starty = input_image.rows - cropsize_y;

    toCrop = cv::Rect(startx, starty, cropsize_x, cropsize_y);
    cropped = cv::Mat(input_image, toCrop);

        returned_cropped_images.push_back(cv::Mat());
        cropped.copyTo(*returned_cropped_images.rbegin());
  }


  if(write_output_to_folder && write_ALL_output_not_just_the_crops)
  {
    filenamestr = output_folder_to_save_crops + std::string("/z_saliency") + to_istring(write_output_to_folder_name_incrementer) + std::string("_out.jpg");
    cv::imwrite(filenamestr.c_str(), out);
  }
}

