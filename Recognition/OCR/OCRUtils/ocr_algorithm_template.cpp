/**
 * @file ocr_algorithm_template.cpp
 * @brief Template OCR algorithm
 * @author Jason Bunk
 */

#include "ocr_algorithm_template.hpp"
#include "Recognition/OCR/OCRUtils/ocr_results.hpp"
#include <opencv2/opencv.hpp>
#include "SharedUtils/SharedUtils.hpp"

void OCRModuleAlgorithm_Template::do_SiftThroughCandidates(OCR_ResultsContainer & given_results,
								int max_num_reported_letters,
								double cutoff_confidence)
{
	//this method assumes that each letter has an attached confidence value
	//if the OCR algorithm doesn't return confidence values, some other means of sorting must be used

	given_results.EliminateDuplicates_ByAveraging();
	
	if(given_results.empty() == false)
	{
		given_results  =
		given_results.GetTopNResults(
			max_num_reported_letters,
			cutoff_confidence >= 0.0 ? cutoff_confidence : cutoff_confidence_of_final_result);
		
		given_results.SortByConfidence();
	}
}
