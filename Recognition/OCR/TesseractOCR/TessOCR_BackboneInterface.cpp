#include <iostream>
#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"
#include "TessOCR_BackboneInterface.hpp"
#include "ocr_module_main.hpp"
#include "ocr_algorithm_tesseract.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include <opencv2/highgui/highgui.hpp>

/*extern*/ TessOCR_Module_Main* global_TessOCR_module_instance = new TessOCR_Module_Main();

void TessOCR :: execute(imgdata_t *imdata) {
	std::cout << "Tesseract OCR" << std::endl;

	if(imdata->cseg_image_data->empty() == false)
	{
        std::vector<cv::Mat> given_CSEGs;

        std::vector<std::vector<unsigned char>*>::iterator cseg_iter;
        for(cseg_iter = imdata->cseg_image_data->begin();
            cseg_iter != imdata->cseg_image_data->end();
            cseg_iter++)
        {
            given_CSEGs.push_back(cv::imdecode(**cseg_iter, CV_LOAD_IMAGE_ANYDEPTH));
        }

        global_TessOCR_module_instance->my_ocr_algorithm->max_num_characters_to_report = 3;
		global_TessOCR_module_instance->DoModule(&given_CSEGs);

        if(global_TessOCR_module_instance->last_obtained_results.empty() == false)
        {
            //the above module_instance returns up to 3 characters...
            //but we can only return 1 answer, the most confident one (the first one in the vector)
            imdata->character = global_TessOCR_module_instance->last_obtained_results.results.begin()->GetCharacterAsString();

            consoleOutput.Level1() << "OCR found:" << std::endl;
            std::vector<OCR_Result>::iterator char_results_iter;
            for(char_results_iter = global_TessOCR_module_instance->last_obtained_results.results.begin();
                char_results_iter != global_TessOCR_module_instance->last_obtained_results.results.end();
                char_results_iter++)
            {
                char_results_iter->PrintMe(&(consoleOutput.Level1()));
            }
        }
	}

	setDone(imdata, TESS_OCR);
}
