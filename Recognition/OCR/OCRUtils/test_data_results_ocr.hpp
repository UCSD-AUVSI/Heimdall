#ifndef ___TEST_DATA_RESULTS_OCR_H_________
#define ___TEST_DATA_RESULTS_OCR_H_________


#include <iostream>
#include <fstream>
#include "ocr_results.hpp"
#include <map>
#include <string>




class test_data_results_ocr
{
public:
    int OCR_attempts;
    int OCR_attempts_in_which_there_was_a_target;
    int OCR_attempts_in_which_there_was_NOT_a_target;
    int OCR_wrong_guesses;
    int OCR_1st_successes; //successes where the letter was its top guess
    int OCR_partial_successes; //any success where the letter was listed as a possible guess
    int OCR_did_not_find_a_letter_when_there_was_one;
    int OCR_did_not_find_a_letter_when_there_was_NOT_one;

//-------

	void Reset()
	{
		OCR_attempts = 0;
		OCR_attempts_in_which_there_was_a_target = 0;
		OCR_attempts_in_which_there_was_NOT_a_target = 0;
		OCR_wrong_guesses = 0;
		OCR_1st_successes = 0;
		OCR_partial_successes = 0;
		OCR_did_not_find_a_letter_when_there_was_one = 0;
		OCR_did_not_find_a_letter_when_there_was_NOT_one = 0;
	}

	test_data_results_ocr() :
		OCR_attempts(0),
		OCR_attempts_in_which_there_was_a_target(0),
		OCR_attempts_in_which_there_was_NOT_a_target(0),
		OCR_wrong_guesses(0),
		OCR_1st_successes(0),
		OCR_partial_successes(0),
		OCR_did_not_find_a_letter_when_there_was_one(0),
		OCR_did_not_find_a_letter_when_there_was_NOT_one(0)
	{}

	void Print();
	void PrintToFile(std::fstream& outfile);
};


void UpdateResultsAttemptsData_ocr      (std::ostream* PRINTHERE, test_data_results_ocr* optional_results_info,
	OCR_ResultsContainer& results_to_check, const char* correct_ocr_character);


void CheckValidityOfResults_ocr			(std::ostream* PRINTHERE, test_data_results_ocr* optional_results_info,
	OCR_ResultsContainer& results_to_check, const char* correct_ocr_character);



#endif
