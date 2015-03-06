#ifndef BLOB_SALIENCY_MODULE_TEMPLATE_H_
#define BLOB_SALIENCY_MODULE_TEMPLATE_H_

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "Backbone/IMGData.hpp"

class Blob_Saliency_Settings
{
    public:

        int converted_color_space;
        bool keep_color_channel[3];

        int canny_low_thresh[3];
        int canny_high_thresh[3];
        int blur_kernel_size[3]; //changing this (kernel size) can cause OpenCV to crash

        double max_ellipse_area_base;
        double min_ellipse_area_base;

        Blob_Saliency_Settings() :
            converted_color_space(CV_BGR2Lab),//options: CV_BGR2Lab, CV_BGR2Luv
            canny_low_thresh{100, 30, 30},
            canny_high_thresh{200, 80, 80},
            blur_kernel_size{4,3,3}, 
            keep_color_channel{true, true, true},
            max_ellipse_area_base(400.0f),
            min_ellipse_area_base(10.0f)
        {
            /* Uncomment if your compiler doesnt play nice with the C++ 11 array initializer
               canny_low_thresh[0] = 70;
               canny_low_thresh[1] = 30;
               canny_low_thresh[2] = 30;
               
               canny_high_thresh[0] = 200;
               canny_high_thresh[1] = 70;
               canny_high_thresh[2] = 70;
                
               blur_kernel_size[0] = 4;
               blur_kernel_size[1] = 3;
               blur_kernel_size[2] = 3;
                
               keep_color_channel[0]=true;
               keep_color_channel[1]=true;
               keep_color_channel[2]=true;
               */
        }

};


class Blob_Saliency_Module_Template
{
    public:
        bool write_internal_images;
        std::string output_dir;

        std::vector<Blob_Saliency_Settings> settings;

        std::vector<cv::Mat> return_crops;
        std::vector<std::pair<double,double>> return_geolocs;

        //---------------------------

        Blob_Saliency_Module_Template() : write_internal_images(false), output_dir("") {}
        virtual ~Blob_Saliency_Module_Template() {}

        virtual void do_saliency(cv::Mat input_image, imgdata_t *imdata) = 0;
};

#endif
