#include "Recognition/OCR/TesseractOCR/TessOCR_BackboneInterface.hpp"
#include "Recognition/OCR/TesseractOCR/ocr_module_main.hpp"
#include "Recognition/OCR/TesseractOCR/ocr_algorithm_tesseract.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/OS_FolderBrowser_tinydir.h"


int main(int argc, char** argv) {
	std::cout << "Test Tesseract Character Recognition" << std::endl;
	
	if(argc < 2)
	{
        std::cout << "usage:  [PATH TO FOLDER CONTAINING IMAGES]" << std::endl;
		return 1;
	}
	std::string folder_path_str(argv[1]);
	assert(global_TessOCR_module_instance != nullptr);
	
	int num_files_found = 0;
	bool found_at_least_one_CSEG = false;
	
	
tinydir_dir dir;
tinydir_open(&dir, folder_path_str.c_str());

while(dir.has_next)
{
    tinydir_file file;
    tinydir_readfile(&dir, &file);

	if(file.is_dir == false)
	{
		num_files_found++;
		std::string filename_str = std::string(file.name);
		
		if(filename_extension_is_image_type(get_extension_from_filename(filename_str)))
		{
			std::string full_filename_str = (folder_path_str + std::string("/") + filename_str);
			std::cout << full_filename_str << std::endl;
			
			
			std::vector<cv::Mat> given_CSEGs;
			given_CSEGs.push_back(cv::imread(full_filename_str, CV_LOAD_IMAGE_GRAYSCALE));
			
			if(given_CSEGs.rbegin()->empty() == false)
			{
				found_at_least_one_CSEG = true;
//---------------------------------------------------------------------------------------
				cv::imshow("Display Image", *given_CSEGs.rbegin());
				
				//global_TessOCR_module_instance->my_ocr_algorithm->max_num_characters_to_report = 3;
				global_TessOCR_module_instance->my_ocr_algorithm->cutoff_confidence_of_final_result = 1.0;
				global_TessOCR_module_instance->DoModule(&given_CSEGs, true);
				
				if(global_TessOCR_module_instance->last_obtained_results.empty() == false)
				{
					//the above module_instance returns up to 3 characters...
					//but we can only return 1 answer, the most confident one (the first one in the vector)
					//std::cout << "" global_TessOCR_module_instance->last_obtained_results.results.begin()->GetCharacterAsString();
					
					std::cout << "OCR found:" << std::endl;
					std::vector<OCR_Result>::iterator char_results_iter;
					for(char_results_iter = global_TessOCR_module_instance->last_obtained_results.results.begin();
						char_results_iter != global_TessOCR_module_instance->last_obtained_results.results.end();
						char_results_iter++)
					{
						char_results_iter->PrintMe(&(std::cout));
					}
				}
//---------------------------------------------------------------------------------------
			}
			else {
				std::cout << "error loading image" << std::endl;
			}
		}
	}


    tinydir_next(&dir);
}
tinydir_close(&dir);

	if(found_at_least_one_CSEG == false) {
		std::cout << "didn't find any images in this folder! folder had " << to_istring(num_files_found) << " files in this folder" << std::endl;
	}
}

