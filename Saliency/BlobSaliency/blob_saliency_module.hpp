#ifndef BLOB_SALIENCY_MODULE_H_
#define BLOB_SALIENCY_MODULE_H_

#include "blob_saliency_module_template.hpp"
#include "Backbone/IMGData.hpp"

class Blob_Saliency_Module : public Blob_Saliency_Module_Template
{
    protected:
        void do_saliency_with_setting(cv::Mat input_image, Blob_Saliency_Settings& setting, imgdata_t *imdata);

        int write_crops_name_incrementer;

    public:
        Blob_Saliency_Module() : Blob_Saliency_Module_Template(), write_crops_name_incrementer(0) {}

        void do_saliency(cv::Mat input_image, imgdata_t *imdata);
};


#endif
