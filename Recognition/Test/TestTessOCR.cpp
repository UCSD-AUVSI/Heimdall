#include "Recognition/OCR/TesseractOCR/TessOCR_BackboneInterface.hpp"
#include "Recognition/OCR/TesseractOCR/ocr_module_main.hpp"
#include "Recognition/OCR/TesseractOCR/ocr_algorithm_tesseract.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/OS_FolderBrowser_tinydir.h"
#include "SharedUtils/cvplot.h"


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
			std::string full_filename_str = std::string(file.path);
			std::cout << full_filename_str << std::endl;
			
			
			std::vector<cv::Mat> given_CSEGs;
			given_CSEGs.push_back(cv::imread(full_filename_str, CV_LOAD_IMAGE_GRAYSCALE));
			
			if(given_CSEGs.rbegin()->empty() == false)
			{
				found_at_least_one_CSEG = true;
//---------------------------------------------------------------------------------------
				cv::imshow("Display Image", *given_CSEGs.rbegin());
				
				
				global_TessOCR_module_instance->RotateAndGetLetterCandidates(&given_CSEGs);//, true);
				OCR_ResultsContainer saved_results_at_all_angles = global_TessOCR_module_instance->last_obtained_results;
				saved_results_at_all_angles.SortByConfidence();
				global_TessOCR_module_instance->SiftThroughCandidates(4);
				
				
				if(global_TessOCR_module_instance->last_obtained_results.size() != 1) {
					std::cout << "warning, OCR didn't successfully find ONE (and only one) good letter, should probably retry" << std::endl;
				}
				
				
				if(global_TessOCR_module_instance->last_obtained_results.empty() == false)
				{
					//global_TessOCR_module_instance->last_obtained_results.SortByAngle();
					
					std::cout << "OCR found (" << to_istring(global_TessOCR_module_instance->last_obtained_results.size()) << " GOOD letters): " << std::endl;
					std::cout << "OCR reported its best guess to be: " << global_TessOCR_module_instance->GetBestCandidate() << std::endl;
					
					
					//collect data for plotting
					std::map<char, std::vector<unsigned char>> character_plots;
					std::string all_plotted_chars;
					int letter_vec_iter_pos = -1;
					int angle_iter_pos = -1;
					int max_angle_iter_pos = saved_results_at_all_angles.size() / 2;
					int delta_angle_of_step = RoundDoubleToInteger(360.0 / static_cast<double>(max_angle_iter_pos));
					
					std::vector<OCR_Result>::iterator char_results_iter;
					for(char_results_iter = saved_results_at_all_angles.results.begin();
						char_results_iter != saved_results_at_all_angles.results.end();
						char_results_iter++)
					{
						letter_vec_iter_pos++;
						angle_iter_pos = (letter_vec_iter_pos / 2); // (0/2) == 0, (1/2) == 0, (2/2) == 1, (3/2) == 1, ...
						
						if(character_plots.find(char_results_iter->character) == character_plots.end())
						{
							character_plots[char_results_iter->character] = std::vector<unsigned char>(max_angle_iter_pos, 0);
							all_plotted_chars.push_back(char_results_iter->character);
							
							if(global_TessOCR_module_instance->last_obtained_results.ContainsLetter(char_results_iter->character)) {
								//char_results_iter->PrintMe(&(std::cout));
								global_TessOCR_module_instance->last_obtained_results.GetResultFromLetter(char_results_iter->character).PrintMe(&std::cout);
							}
						}
						character_plots[char_results_iter->character][angle_iter_pos]++;
					}
					
					
					//now draw the plots
					int plotcol_i = 0;
					std::map<char, std::vector<unsigned char>>::iterator letter_plot_iter = character_plots.begin();
					for(; letter_plot_iter != character_plots.end(); letter_plot_iter++)
					{
						if(global_TessOCR_module_instance->last_obtained_results.ContainsLetter(letter_plot_iter->first))
						{
							//step argument can be "delta_angle_of_step" if (360 / delta_angle) is a whole number
							CvPlot::plot(all_plotted_chars, &(letter_plot_iter->second[0]), letter_plot_iter->second.size(), 1, plotcol_i);
							CvPlot::label(char_to_string(letter_plot_iter->first));
							
							plotcol_i++;
						}
					}
					
					cv::waitKey(0);
					CvPlot::clear(all_plotted_chars);
					cv::destroyAllWindows();
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

