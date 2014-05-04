#ifndef ___TEST_DATA_RESULTS_SHAPEREC_H_________
#define ___TEST_DATA_RESULTS_SHAPEREC_H_________


#include <iostream>
#include <fstream>
#include "Shaperec_Utils_Results.hpp"
#include <map>
#include <string>




class test_data_results_shaperec
{
public:
    int shapenamer_attempts;
    int shapenamer_attempts_in_which_there_was_a_target;
    int shapenamer_attempts_in_which_there_was_NOT_a_target;
    int shapenamer_wrong_guesses;
    int shapenamer_singleguess_successes; //it only reported a single shape, the correct one
    int shapenamer_1stguess_successes; //the top guess was correct; if other wrong ones were reported, they were said to be less likely
    int shapenamer_total_partial_successes; //the correct shape was listed, even if among others that were wrong
    int shapenamer_did_not_find_a_shape_when_there_was_one;
    int shapenamer_did_not_find_a_shape_when_there_was_NOT_one;

//-------

	void Reset()
	{
		shapenamer_attempts = 0;
		shapenamer_attempts_in_which_there_was_a_target = 0;
		shapenamer_attempts_in_which_there_was_NOT_a_target = 0;
		shapenamer_wrong_guesses = 0;
		shapenamer_singleguess_successes = 0;
		shapenamer_1stguess_successes = 0;
		shapenamer_total_partial_successes = 0;
		shapenamer_did_not_find_a_shape_when_there_was_one = 0;
		shapenamer_did_not_find_a_shape_when_there_was_NOT_one = 0;
	}

	test_data_results_shaperec() :
		shapenamer_attempts(0),
		shapenamer_attempts_in_which_there_was_a_target(0),
		shapenamer_attempts_in_which_there_was_NOT_a_target(0),
		shapenamer_wrong_guesses(0),
		shapenamer_singleguess_successes(0),
		shapenamer_1stguess_successes(0),
		shapenamer_total_partial_successes(0),
		shapenamer_did_not_find_a_shape_when_there_was_one(0),
		shapenamer_did_not_find_a_shape_when_there_was_NOT_one(0)
	{}

	void Print();
	void PrintToFile(std::fstream& outfile);
};


void UpdateResultsAttemptsData_shaperec (std::ostream* PRINTHERE, test_data_results_shaperec* optional_results_info,
	ShapeRec_ResultsContainer& results_to_check, std::string* correct_shape_name);


void CheckValidityOfResults_shaperec	(std::ostream* PRINTHERE, test_data_results_shaperec* optional_results_info,
	ShapeRec_ResultsContainer& results_to_check, std::string* correct_shape_name);


#endif
