/**
 * @file Shaperec_ModuleMain.cpp
 * @brief The full "black box" style shape recognition module: given several input SSEG's, return a shape name.
          Runs the single-input "Shaperec_SingleImgAlg" several times to gather results, and condenses those results into one shape result.
 * @author Jason Bunk
 */

#include "Shaperec_ModuleMain.hpp"
#include "test_data_results_shaperec.hpp"
#include "Shaperec_SingleImgAlg_Turning.hpp"
#include "SharedUtils/SharedUtils.hpp"
using std::cout; using std::endl;


#define TRY_GET_OPTIONAL_INFO(variable) (optional_results_info == nullptr) ? 0 : optional_results_info->variable


ShapeRecModule_Main::ShapeRecModule_Main(std::string folder_with_reference_shapes)
{
	single_shape_namer_algorithm = new ShapeRecModuleAlgorithm_SingleImage_Turning();
	single_shape_namer_algorithm->filefolder_containing_reference_shapes = folder_with_reference_shapes;
	
	std::vector<std::string> possibleFolders;
	possibleFolders.push_back("bin/");
	possibleFolders.push_back("../bin/");
	possibleFolders.push_back("build/bin/");
	possibleFolders.push_back("../build/bin/");
	possibleFolders.push_back("../../bin/");
	possibleFolders.push_back("../../../bin/");
	
	std::string folder_found(folder_with_reference_shapes);
	int possibleFolderIndex = 0;
	while(check_if_directory_exists(folder_found) == false && possibleFolderIndex < ((int)possibleFolders.size())) {
		folder_found = (possibleFolders[possibleFolderIndex]+folder_with_reference_shapes);
		possibleFolderIndex++;
	}
	
	if(check_if_directory_exists(folder_found)) {
		consoleOutput.Level0() << "found shaperec folder \'" << folder_found << "\'" << endl;
		
		single_shape_namer_algorithm->filefolder_containing_reference_shapes = folder_found;
		if(CountNumImagesInFolder(folder_found) > 0) {
			TryPrintAllFileNamesInFolder(folder_found, consoleOutput.Level2());
		} else {
			for(int j=0; j<30; j++) {
				consoleOutput.Level0() << "ERROR: SHAPE REC: WAS GIVEN A FOLDER WITH NO REFERENCE SHAPE IMAGES!!!!!!!!" << endl;
				consoleOutput.Level0() << "       tried to look in the folder \"" << folder_found << "\"" << endl;
			}
		}
	} else {
		for(int j=0; j<30; j++) {
			consoleOutput.Level0() << "ERROR: SHAPE REC: WAS NOT GIVEN A VALID FOLDER TO FIND REFERENCE SHAPES!!!!" << endl;
			consoleOutput.Level0() << "       tried to look in the folder \"" << folder_found << "\"" << endl;
		}
	}
}



void ShapeRecModule_Main::DoModule(std::vector<cv::Mat>* input_SSEG_images,

//-----------------------------------------------------------------------------------
    std::ostream* PRINT_TO_FILE_HERE/*=nullptr*/,
    std::string* folder_path_of_output_saved_images/*=nullptr*/,
    bool save_images_and_results/*=false*/,
    std::string* name_of_target_image/*=nullptr*/,
    test_data_results_shaperec* optional_results_info/*=nullptr*/,
    std::string* correct_shape_name/*=nullptr*/,
    const char* correct_ocr_character/*=nullptr*/)
//-----------------------------------------------------------------------------------
{
    last_obtained_results.clear();


    if(input_SSEG_images != nullptr && single_shape_namer_algorithm != nullptr)
    {
        consoleOutput.Level3() << std::string("shaperec: number of input SSEGs:") << to_istring(input_SSEG_images->size()) << std::endl;

        if(input_SSEG_images->empty())
        {
            consoleOutput.Level1() << "shaperec: no SSEGs; returned without even updating shaperec attempts" << std::endl;
            return;
        }

//-----------------------------------------------------------------------------------
	if(correct_shape_name != nullptr) {
            UpdateResultsAttemptsData_shaperec(&consoleOutput.Level3(), optional_results_info, last_obtained_results, correct_shape_name);
	}
//-----------------------------------------------------------------------------------


        std::vector<cv::Mat>::iterator SSEG_iter = input_SSEG_images->begin();
        for(; SSEG_iter != input_SSEG_images->end(); SSEG_iter++)
        {
            /*bool found_a_shape =*/ single_shape_namer_algorithm->RecognizeShape(*SSEG_iter, false);


            last_obtained_results.results.insert(last_obtained_results.results.end(),
                single_shape_namer_algorithm->last_obtained_results.results.begin(),
                single_shape_namer_algorithm->last_obtained_results.results.end());
        }


        //now, example contents: square, square, square, circle, triangle

        last_obtained_results.EliminateDuplicates();

        //now, example contents: square, circle, triangle

        last_obtained_results.results =
        last_obtained_results.GetTopResults(
            single_shape_namer_algorithm->max_absolute_confidence_threshold,
            single_shape_namer_algorithm->min_absolute_confidence_threshold,
            single_shape_namer_algorithm->max_relative_confidence_threshold,
            single_shape_namer_algorithm->min_relative_confidence_threshold,
            PRINT_TO_FILE_HERE,true,true);

        //now, example contents: square

//-----------------------------------------------------------------------------------
	if(correct_shape_name != nullptr) {
        	CheckValidityOfResults_shaperec(&consoleOutput.Level3(), optional_results_info, last_obtained_results, correct_shape_name);
	}
//-----------------------------------------------------------------------------------
    }
    else
        consoleOutput.Level0() << "WARNING: ShapeRecModule_Main::DoModule() didn't do anything because of null pointers!" << std::endl;
}






