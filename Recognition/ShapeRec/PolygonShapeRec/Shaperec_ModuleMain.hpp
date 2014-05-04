#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <opencv/cv.h>
#include "Shaperec_Utils_Results.hpp"
class ShapeRecModuleAlgorithm_SingleImage_Template;
class test_data_results_shaperec;



class ShapeRecModule_Main
{
public:
	ShapeRec_ResultsContainer last_obtained_results;

    //this will be automatically created IFF you create this class by giving it a string
    ShapeRecModuleAlgorithm_SingleImage_Template* single_shape_namer_algorithm;


//----

    ShapeRecModule_Main() : single_shape_namer_algorithm(nullptr) {}
    ShapeRecModule_Main(std::string folder_with_reference_shapes);


	void DoModule(std::vector<cv::Mat>* input_SSEG_images,

        std::ostream* PRINT_TO_FILE_HERE=nullptr,
        std::string* folder_path_of_output_saved_images=nullptr,
		bool save_images_and_results=false,
		std::string* name_of_target_image=nullptr,
		test_data_results_shaperec* optional_results_info=nullptr,
		std::string* correct_shape_name=nullptr,
		const char* correct_ocr_character=nullptr);
};


