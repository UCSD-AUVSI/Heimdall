#ifndef ____SALIENCY_MODULE_CSBD_H_______
#define ____SALIENCY_MODULE_CSBD_H_______


#include "Saliency/saliency_module_template.hpp"



class SaliencyModule_C_SBD : public SaliencyModule_Template
{
protected:
    void do_saliency_with_setting(cv::Mat input_image, SaliencySettings& setting);

    int write_output_to_folder_name_incrementer;

public:
    //bool write_output_to_folder;
    //std::vector<SaliencySettings> settings;

    //std::vector<cv::Mat> returned_cropped_images;

//---------------------------

    SaliencyModule_C_SBD() : SaliencyModule_Template(), write_output_to_folder_name_incrementer(0) {}

    std::vector<cv::Mat>& do_saliency(cv::Mat input_image);
};


#endif
