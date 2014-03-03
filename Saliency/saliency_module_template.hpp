#ifndef ____SALIENCY_MODULE_TEMPLATE_H_______
#define ____SALIENCY_MODULE_TEMPLATE_H_______

#include <opencv/cv.h>
#include <opencv/highgui.h>


class SaliencySettings
{
public:

int ColorConvert_CV_color_space;
bool ColorConvert_desired_CV_color_channels[3];


int Canny_low_threshold;
int Canny_high_threshold;
int Canny_kernel_size; //changing this (kernel size) can cause OpenCV to crash


    SaliencySettings() :
        ColorConvert_CV_color_space(CV_BGR2Luv),//options: CV_BGR2Lab, CV_BGR2Luv
        Canny_low_threshold(50),
        Canny_high_threshold(120),
        Canny_kernel_size(3) //changing this (kernel size) can cause OpenCV to crash
        {
            ColorConvert_desired_CV_color_channels[0]=false;
            ColorConvert_desired_CV_color_channels[1]=true;
            ColorConvert_desired_CV_color_channels[2]=true;
        }

};


class SaliencyModule_Template
{
public:
    bool write_ALL_output_not_just_the_crops;
    bool write_output_to_folder;
    std::string output_folder_to_save_crops;


    std::vector<SaliencySettings> settings;


    std::vector<cv::Mat> returned_cropped_images;

//---------------------------

    SaliencyModule_Template() : write_ALL_output_not_just_the_crops(false), write_output_to_folder(false) {}
    virtual ~SaliencyModule_Template() {}


    virtual std::vector<cv::Mat>& do_saliency(cv::Mat input_image) = 0;
};


#endif
