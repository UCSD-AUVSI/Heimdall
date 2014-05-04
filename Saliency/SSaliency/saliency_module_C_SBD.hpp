#ifndef SALIENCY_MODULE_CSBD_H_
#define SALIENCY_MODULE_CSBD_H_

#include "saliency_module_template.hpp"

class SaliencyModule_C_SBD : public SaliencyModule_Template
{
    protected:
        void do_saliency_with_setting(cv::Mat input_image, SaliencySettings& setting);

        int write_output_to_folder_name_incrementer;

    public:
        SaliencyModule_C_SBD() : SaliencyModule_Template(), write_output_to_folder_name_incrementer(0) {}

        std::vector<cv::Mat>& do_saliency(cv::Mat input_image);
};


#endif
