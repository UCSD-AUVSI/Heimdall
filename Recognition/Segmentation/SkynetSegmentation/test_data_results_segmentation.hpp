#ifndef ___TEST_DATA_RESULTS_SEGMENTATION_H_________
#define ___TEST_DATA_RESULTS_SEGMENTATION_H_________


#include <iostream>
#include <fstream>
#include <map>
#include <string>


class Segmenter_Module_Settings;


enum test_data_results__testtype
{
	testtype__unknown,
	testtype__per_segmentation,
	testtype__per_fullsize_image
};


class test_data_results_segmentation
{
public:
	test_data_results__testtype my_test_type;

	int CSEG_attempts_total;
	int CSEG_attempts_in_which_there_WAS_a_target;
    int CSEG_num_times_it_segmented_something;
    int CSEG_failed_to_return_anything_when_there_was_NO_target;
    int CSEG_failed_to_return_anything_when_there_WAS_a_target;
    int CSEG_returned_something_when_there_was_NO_target;
    int CSEG_returned_something_when_there_WAS_a_target;
    std::map<std::string,bool> CSEG_names_of_targets_it_failed_on;

	int SSEG_attempts_total;
	int SSEG_attempts_in_which_there_WAS_a_target;
    int SSEG_num_times_it_segmented_something;
    int SSEG_failed_to_return_anything_when_there_was_NO_target;
    int SSEG_failed_to_return_anything_when_there_WAS_a_target;
    int SSEG_returned_something_when_there_was_NO_target;
    int SSEG_returned_something_when_there_WAS_a_target;
    std::map<std::string,bool> SSEG_names_of_targets_it_failed_on;


//-------

	void Reset()
	{
		CSEG_attempts_total = 0;
		CSEG_attempts_in_which_there_WAS_a_target = 0;
		CSEG_num_times_it_segmented_something = 0;
		CSEG_failed_to_return_anything_when_there_was_NO_target = 0;
		CSEG_failed_to_return_anything_when_there_WAS_a_target = 0;
		CSEG_returned_something_when_there_was_NO_target = 0;
		CSEG_returned_something_when_there_WAS_a_target = 0;

		SSEG_attempts_total = 0;
		SSEG_attempts_in_which_there_WAS_a_target = 0;
		SSEG_num_times_it_segmented_something = 0;
		SSEG_failed_to_return_anything_when_there_was_NO_target = 0;
		SSEG_failed_to_return_anything_when_there_WAS_a_target = 0;
		SSEG_returned_something_when_there_was_NO_target = 0;
		SSEG_returned_something_when_there_WAS_a_target = 0;
	}

	test_data_results_segmentation() :
		my_test_type(testtype__unknown),

		CSEG_attempts_total(0),
		CSEG_attempts_in_which_there_WAS_a_target(0),
		CSEG_num_times_it_segmented_something(0),
		CSEG_failed_to_return_anything_when_there_was_NO_target(0),
		CSEG_failed_to_return_anything_when_there_WAS_a_target(0),
		CSEG_returned_something_when_there_was_NO_target(0),
		CSEG_returned_something_when_there_WAS_a_target(0),

		SSEG_attempts_total(0),
		SSEG_attempts_in_which_there_WAS_a_target(0),
		SSEG_num_times_it_segmented_something(0),
		SSEG_failed_to_return_anything_when_there_was_NO_target(0),
		SSEG_failed_to_return_anything_when_there_WAS_a_target(0),
		SSEG_returned_something_when_there_was_NO_target(0),
		SSEG_returned_something_when_there_WAS_a_target(0)
	{}

	void Print();
	void PrintToFile(std::fstream& outfile, Segmenter_Module_Settings* settings_used);


    //"segmentation_successes" can be variable (0-5ish) per cropped image... but reduce this to 0 or 1 per image
	void Update_My_CSEG_Data_DueToOther(test_data_results_segmentation* other_tester);
    //"segmentation_successes" can be variable (0-5ish) per cropped image... but reduce this to 0 or 1 per image
	void Update_My_SSEG_Data_DueToOther(test_data_results_segmentation* other_tester);
};


void UpdateResultsAttemptsData_CSEG(std::ostream* PRINTHERE, test_data_results_segmentation* optional_results_info,
    bool there_was_a_target);
void UpdateResultsAttemptsData_SSEG(std::ostream* PRINTHERE, test_data_results_segmentation* optional_results_info,
    bool there_was_a_target);


void CheckValidityOfResults_CSEG(std::ostream* PRINTHERE, test_data_results_segmentation* optional_results_info,
    bool segmentation_returned_something, bool there_was_a_target, std::string* name_of_target_image);
void CheckValidityOfResults_SSEG(std::ostream* PRINTHERE, test_data_results_segmentation* optional_results_info,
    bool segmentation_returned_something, bool there_was_a_target, std::string* name_of_target_image);


#endif
