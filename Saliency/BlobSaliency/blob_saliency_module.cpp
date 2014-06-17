/**
 * @file saliency_module_canny.cpp
 * @brief Saliency algorithm based on Canny edge detection and cv::SimpleBlobDetector
 * @author Eric Lo, Jason Bunk
 */

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <iostream>

#include "blob_saliency_module.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/intersection.hpp"
#include "SharedUtils/SharedUtils_OpenCV.hpp"

using std::cout;
using std::endl;

void Blob_Saliency_Module::do_saliency(cv::Mat input_image, imgdata_t *imdata){
    return_crops.clear();
    return_geolocs.clear();

    std::vector<Blob_Saliency_Settings>::iterator siter = settings.begin();
    for(; siter != settings.end(); siter++) {
        do_saliency_with_setting(input_image, *siter, imdata);
        write_crops_name_incrementer++;
    }
}

//Helper functions and variables for trig functions
const double kPI = 4 * atan(1);
double to_degrees(double radians){
    return radians * 180 / kPI;
}
double to_radians(double degrees){
    return degrees * kPI / 180;
}

// Calculate pixels per feet (resolution) of given image, AFTER scaling
double calculate_px_per_feet(double horiz_cols, double altitude, double scalefactor){
    cout << "alt: " << altitude << endl;
    double focal_length = 35, crop_factor = 1.6; 
    double equiv_foc_len = focal_length * crop_factor;

    double h_fov = 2 * atan(36/(2*equiv_foc_len)); 

    double h_ground = 2 * altitude * tan(h_fov/2);
    double px_per_feet = horiz_cols/h_ground;

    return px_per_feet * scalefactor;
}

// Calculate pixel area (pixel^2) from two dimensions (feet)
double calculate_area(double horiz_cols, double altitude, double scalefactor, double dimen1, double dimen2){
    double px_per_feet = calculate_px_per_feet(horiz_cols, altitude, scalefactor);
    return ((px_per_feet * dimen1) * (px_per_feet * dimen2));
}

// Calculate max area of blobs to look for
double calculate_max_area(double horiz_cols, double altitude, double scalefactor){
    return calculate_area(horiz_cols, altitude, scalefactor, 6, 6); //max dimens in feet
}

// Calculate min area of blobs to look for (thresholded at a small blob size)
double calculate_min_area(double horiz_cols, double altitude, double scalefactor){
    double min_area = calculate_area(horiz_cols, altitude, scalefactor, 1.5, 1.5); //max dimens in feet
    return (min_area <= 5.0f)?5.0f:min_area;
}

std::pair<double, double> find_target_geoloc(int targetrow, int targetcol, int imrows, int imcols, double planelat, double planelongt, double planeheading, double pxperfeet){
    planeheading = planeheading - kPI; //Gimbal is reversed in plane

    double rowdiff = targetrow - (double)imrows/2; //row diff from center (and center of plane)
    double coldiff = targetcol - (double)imcols/2; //col diff from center (and center of plane)

    // Tranlate to polar coordinates, in feet
    double centerdiff = sqrt(pow(rowdiff, 2) + pow(coldiff, 2));
    double centerfeetdiff = centerdiff / pxperfeet;
    double centerangle = atan(coldiff/-rowdiff) + (((-rowdiff)>0)?0.0:kPI);
    
    // Project to Lat/Long
    double latfeetdiff = centerfeetdiff * cos(planeheading + centerangle);
    double longtfeetdiff = centerfeetdiff * sin(planeheading + centerangle);

    // Convert Lat/Long feet differences into degrees to get final lat/long
    double target_lat = planelat + latfeetdiff/365221.43; //365221 feet in 1 degree of latitude arc, small angle assumptions for field; 
    double longt_deg_to_feet = kPI * 20890566 * cos(to_radians(target_lat)) / 180; //Radius of circle at this lat, (PI*R)/(180)
    double target_longt = planelongt + longtfeetdiff/longt_deg_to_feet;
    
    cout << "heading: " << planeheading << endl;
    cout << "imrows: " << imrows << " imcols: " << imcols << endl;
    cout << "rd: " << rowdiff << endl;
    cout << "cd: " << coldiff << endl;
    cout << "ppf: " << pxperfeet << endl;
    cout << "centerdiff: " << centerdiff << endl;
    cout << "centerfeetdiff: " << centerfeetdiff << endl;
    cout << "centerangle: " << centerangle << endl;
    printf("lat diff: %.7f\tdeg, %.3f feet\n", latfeetdiff/365221, latfeetdiff);
    printf("long diff: %.7f\tdeg, %.3f feet\n", longtfeetdiff/longt_deg_to_feet, longtfeetdiff);

    return std::pair<double, double>(target_lat, target_longt);
}

void Blob_Saliency_Module::do_saliency_with_setting(cv::Mat input_image, Blob_Saliency_Settings& setting, imgdata_t *imdata){
    if(input_image.cols < 20 || input_image.rows < 20)
        return;

    cv::Mat resized, src_gray, detected_edges, contour_draw, filt_contour_draw;
    std::string filenamestr;

    // Bunch of adjustable settings for saliency
    double scalefactor = 0.15, relative_scalefactor = scalefactor/0.1f;
    double min_elongation = 0.3f;
    double min_blob_ellipse_area_ratio = 0.3f;
    int canny_kernel_size = 3;
    int cropborder = 5;
    int erdi_iters = (int)(3 * relative_scalefactor);
    double max_ellipse_area = setting.max_ellipse_area_base * (pow(relative_scalefactor,2)), 
           min_ellipse_area = setting.min_ellipse_area_base * (pow(relative_scalefactor,2));
    
    // If we have been given altitude data
    if(imdata->planealt > 0.0f){
        max_ellipse_area = calculate_max_area(input_image.cols, imdata->planealt, scalefactor);
        min_ellipse_area = calculate_min_area(input_image.cols, imdata->planealt, scalefactor);
        cout << " Max: " << max_ellipse_area << " Min: " << min_ellipse_area << endl;
    }

    cv::RNG rng(8123);
    cv::Scalar color_blue(255,0,0);

    // Step 1: resize to 1/(scalefactor**2) smaller (start w/ 18MP)
    cv::resize(input_image, resized, cv::Size(0,0), scalefactor, scalefactor, CV_INTER_AREA);

    if(write_internal_images) {
        cv::imwrite(output_dir + "/z_saliency_" + to_istring(write_crops_name_incrementer) + "_before_cielab.jpg", resized);
    }

    ConvertMat_UsingSettings(resized, resized, setting.converted_color_space, setting.keep_color_channel, false);

    std::vector<cv::Mat> spl(3);
    cv::split(resized, spl);
    cv::vector<cv::RotatedRect> final_ellipses;
    filt_contour_draw = cv::Mat::zeros(resized.size(), CV_8UC3);
    for(int channel = 2; channel >= 0; --channel) {
        if(!setting.keep_color_channel[channel]){
            continue;
        }

        detected_edges = cv::Mat::zeros(spl[0].size(), CV_8UC3);

        spl[channel].copyTo(src_gray);

        /// Noise reduction
        cv::blur(src_gray, src_gray, cv::Size(setting.blur_kernel_size[channel], setting.blur_kernel_size[channel]));

        if(write_internal_images) {
            filenamestr = output_dir + "/z_saliency_" + to_istring(write_crops_name_incrementer) + "_before_input_canny_" + std::to_string(channel) + ".jpg";
            cv::imwrite(filenamestr.c_str(), src_gray);
        }

        cv::Mat canny_output;
        /// Canny detector
        cv::Canny(src_gray, detected_edges, setting.canny_low_thresh[channel], setting.canny_high_thresh[channel], canny_kernel_size, true);

        // Initialize matrices to hold contours
        contour_draw = cv::Mat::zeros(detected_edges.size(), CV_8UC3);

        if(write_internal_images) {
            filenamestr = output_dir + "/z_saliency_" + to_istring(write_crops_name_incrementer) + "_canny_before_in_channel_" + std::to_string(channel) + ".jpg";
            cv::imwrite(filenamestr.c_str(), detected_edges);
        }

        //Dilation and erosion rounds to connect incomplete edges
        cv::dilate(detected_edges, detected_edges, cv::Mat(), cv::Point(-1,-1), erdi_iters);
        cv::erode(detected_edges, detected_edges, cv::Mat(), cv::Point(-1,-1), erdi_iters);

        if(write_internal_images) {
            filenamestr = output_dir + "/z_saliency_" + to_istring(write_crops_name_incrementer) + "_canny_post_in_channel_" + std::to_string(channel) + ".jpg";
            cv::imwrite(filenamestr.c_str(), detected_edges);
        }

        //Find contours
        std::vector<std::vector<cv::Point>> contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours(detected_edges, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

        cout << "Found " << contours.size() << " contour(s)" << endl;

        // Didn't find anything in image, leave
        if(contours.size() == 0){
            continue;
        }

        // Draw contours onto output image
        if(write_internal_images){
            int idx = 0;
            for(; idx >= 0; idx = hierarchy[idx][0]){
                cv::Scalar color (rand()&255, rand()&255, rand()&255);
                drawContours(contour_draw, contours, idx, color, CV_FILLED, 8, hierarchy);
            }
        }

        // Filter out contours that dont match our criteria
        bool filter[contours.size()];
        for(int i = 0; i < contours.size(); i++){
            filter[i] = false;
            std::vector<cv::Point> contour = contours[i];

            if(contour.size() < 5){
                continue; //fitEllipse needs >= 5 points
            }

            cv::Moments moment = cv::moments(contour);

            cv::RotatedRect ellipse = fitEllipse(contour);
            double elongation = (ellipse.size.width > ellipse.size.height)?
                ellipse.size.height/ellipse.size.width:
                ellipse.size.width/ellipse.size.height;
            double ellipse_area = ellipse.size.width * ellipse.size.height;
            double blob_ellipse_area_ratio = moment.m00/ellipse_area;

            if(write_internal_images){
                cv::ellipse(contour_draw, ellipse, color_blue);

                cv::Point2f textpt(ellipse.center);
                textpt.y += 15;
                textpt.x -= 15;
                textpt.x -= 15;
                std::string area_text = std::to_string(ellipse_area);
                area_text = area_text.substr(0, area_text.size() - 2);
                putText(contour_draw, area_text, textpt, 0, .4, color_blue);
            }

            // Check ellipse fit of contour against our criteria
            if(moment.m00 < max_ellipse_area &&
                    moment.m00 > min_ellipse_area &&
                    elongation > min_elongation && 
                    blob_ellipse_area_ratio > min_blob_ellipse_area_ratio){

                bool intersects = false;
                int j;
                for(j = 0; j < final_ellipses.size(); j++){
                    if(rotatedRectangleIntersection(final_ellipses[j], ellipse)){
                        intersects = true;
                        break;
                    }
                }
                if (!intersects) {
                    filter[i] = true;
                    final_ellipses.push_back(ellipse);
                }
                else {
                    //Choose larger contour as our actual
                    cv::RotatedRect &prev_ellipse = final_ellipses[j];
                    if(ellipse_area > (prev_ellipse.size.width * prev_ellipse.size.height)){
                        filter[i] = true;
                        std::swap(prev_ellipse, ellipse);
                    }
                }
            }
        }

        if(write_internal_images) {
            filenamestr = output_dir + "/z_saliency_" + to_istring(write_crops_name_incrementer) + "_contours_before_filter_in_channel_" + std::to_string(channel) + ".jpg";
            cv::imwrite(filenamestr.c_str(), contour_draw);

            int count = 0;
            int idx = 0;
            for(; idx >= 0; idx = hierarchy[idx][0]){
                if(filter[count++]){
                    cv::Scalar color (rand()&255, rand()&255, rand()&255);
                    drawContours(filt_contour_draw, contours, idx, color, CV_FILLED, 8, hierarchy);
                }
            }
        }
    }

    cv::Rect toCrop;
    cv::Mat cropped;

    //crop size should depend on expected target size... or be determined by the size of the blob!
    int cropsize;
    int startx, starty;

    // crop from the full res image
    for(cv::RotatedRect ellipse : final_ellipses) {
        if(write_internal_images){
            cropsize = (ellipse.size.width>ellipse.size.height)?ellipse.size.width:ellipse.size.height + cropborder * 2;
            if(cropsize % 2 != 0) cropsize++;

            startx = (int)ellipse.center.x - (cropsize/2);
            starty = (int)ellipse.center.y - (cropsize/2);
            toCrop = cv::Rect(startx, starty, cropsize, cropsize);

            cv::rectangle(filt_contour_draw, toCrop, color_blue);
        }
        double rescalefactor = 1/scalefactor;
        cropsize =(int)((ellipse.size.width>ellipse.size.height)?ellipse.size.width:ellipse.size.height * rescalefactor + cropborder * 2 * rescalefactor);
        if(cropsize % 2 != 0) cropsize++;

        cropped.create(cv::Size(cropsize,cropsize), filt_contour_draw.type());

        startx = (int)ellipse.center.x * rescalefactor - (cropsize/2);
        starty = (int)ellipse.center.y * rescalefactor - (cropsize/2);
        if(startx < 0)
            startx = 0;
        if(starty < 0)
            starty = 0;
        if(startx > input_image.cols - cropsize)
            startx = input_image.cols - cropsize;
        if(starty > input_image.rows - cropsize)
            starty = input_image.rows - cropsize;

        toCrop = cv::Rect(startx, starty, cropsize, cropsize);

        cropped = cv::Mat(input_image, toCrop);

        return_crops.push_back(cropped);
        return_geolocs.push_back(find_target_geoloc(starty + cropsize/2, startx + cropsize/2, input_image.rows, input_image.cols,
                    imdata->planelat, imdata->planelongt, imdata->planeheading, calculate_px_per_feet(input_image.cols, imdata->planealt, 1)));
    }

    if(write_internal_images) {
        filenamestr = output_dir + "/z_saliency_" + to_istring(write_crops_name_incrementer) + "_contours_post_filter.jpg";
        cv::imwrite(filenamestr.c_str(), filt_contour_draw);
    }
}
