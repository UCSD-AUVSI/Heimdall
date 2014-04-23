/**
 * @file ocr_algorithm_gocr.cpp
 * @brief GOCR-based OCR algorithm
 * @author Jason Bunk
 */

#include "ocr_algorithm_gocr.hpp"
#include "Recognition/OCR/OCRUtils/ocr_results.hpp"
#include <opencv2/opencv.hpp>
#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/SharedUtils_OpenCV.hpp"


bool OCRModuleAlgorithm_GOCR::TryToInitializeMe()
{
	//assume it's there
	return true;
}


void OCRModuleAlgorithm_GOCR::do_SiftThroughCandidates(OCR_ResultsContainer & given_results,
								int max_num_reported_letters,
								double cutoff_confidence)
{
	//this method assumes that each letter has an attached confidence value
	//if the OCR algorithm doesn't return confidence values, some other means of sorting must be used
	
	std::map<char, int> char_appearances;
	std::map<char, double> char_std_deviances;
	std::map<char, double> char_confidences;
	std::map<char, double> char_angles;
	
	std::vector<OCR_Result>::iterator riter = given_results.results.begin();
	for(; riter != given_results.results.end(); riter++)
	{
		if(riter->character != ' ')
		{
			if(char_appearances.find(riter->character) == char_appearances.end())
			{
				char_appearances[riter->character] = 0;
				
				double letter_standard_deviance = 0.0;
				char_angles[riter->character] = OCR_ResultsContainer::GetMeanAngle_FromOCRResults(given_results.results, riter->character, &letter_standard_deviance);
				char_std_deviances[riter->character] = letter_standard_deviance;
			}
			char_appearances[riter->character]++;
		}
	}
	
	
	given_results.clear();
	
	
	std::map<char, int>::iterator miter = char_appearances.begin();
	for(; miter != char_appearances.end(); miter++)
	{
		if(char_appearances[miter->first] >= 3)
		{
			char_confidences[miter->first] = (static_cast<double>(char_appearances[miter->first]) / char_std_deviances[miter->first]);
		}
		else
		{
			char_confidences[miter->first] = 0.0;
		}
		
		given_results.PushBackNew(char_confidences[miter->first], char_angles[miter->first], miter->first);
	}
	
	
	given_results.SortByConfidence();
	
	
	std::map<char, int> new_chars____test_for_printing;
	riter = given_results.results.begin();
	for(; riter != given_results.results.end(); riter++)
	{
		if(riter->character != ' ')
		{
			if(new_chars____test_for_printing.find(riter->character) == new_chars____test_for_printing.end())
			{
				new_chars____test_for_printing[riter->character] = 0;
				consoleOutput.Level2() << "letter: " << riter->character << ", num appearances: " << char_appearances[riter->character] << ", std dev: " << char_std_deviances[riter->character] << ", \"conf\": " << char_confidences[riter->character] << std::endl;
			}
		}
	}
}


void OCRModuleAlgorithm_GOCR::RotateAndRunOCR(cv::Mat matsrc, double angle_amount, bool return_empty_characters)
{
	cv::Mat mat_rotated;
	Rotate_CV_Mat(matsrc, angle_amount, mat_rotated);



	std::string temp_file_on_disk__name("temp_delete_me_please.png");

	cv::imwrite(temp_file_on_disk__name, mat_rotated);

	std::ostringstream oout;
	//oout << "gocr -m 130 -C A-Z -i " << temp_file_on_disk__name << " -a " << certainty_lower_bound;
	//oout << "gocr -C A-Z -i " << temp_file_on_disk__name << " -a " << certainty_lower_bound;
	
	oout << "gocr -C ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 -i " << temp_file_on_disk__name << " -a " << certainty_lower_bound;

	FILE* pipe = popen(oout.str().c_str(), "r");
	if(!pipe){
		remove(temp_file_on_disk__name.c_str());
		return;
	}

	char buffer[128];
	std::string gocroutput = "";
	while(!feof(pipe)){
		if(fgets(buffer, 128, pipe) != NULL){
			gocroutput += buffer;
		}
	}
	pclose(pipe);
	remove(temp_file_on_disk__name.c_str());

	if(gocroutput.length() <= 2){
		char foundchar = gocroutput.front();
		if(foundchar == '_') {
			foundchar = ' ';
		}
		if(foundchar != ' ' || return_empty_characters) {
			last_obtained_results.PushBackNew(certainty_lower_bound+1, angle_amount, foundchar);
		}
	}
}


bool OCRModuleAlgorithm_GOCR::do_OCR(cv::Mat letter_binary_mat, std::ostream* PRINTHERE/*=nullptr*/, bool return_empty_characters/*=false*/)
{
	last_obtained_results.clear();


	//rotate a bunch and detect chars

	double angle_current=0.0;
	double angle_delta = (360.0 / static_cast<double>(num_angles_to_check));
	for(int bb=0; bb<num_angles_to_check; bb++)
	{
		RotateAndRunOCR(letter_binary_mat, angle_current, return_empty_characters);
		angle_current += angle_delta;
	}


	return (last_obtained_results.empty() == false);
}

