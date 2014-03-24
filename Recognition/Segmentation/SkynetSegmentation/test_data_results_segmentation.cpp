/**
 * @file test_data_results_segmentation.cpp
 * @brief Sets up results container for segmentation module testing.
 * @author Jason Bunk
 */

#include "test_data_results_segmentation.hpp"
#include "SharedUtils/SharedUtils_OpenCV.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include "SegmentationSettings.hpp"



#define PRINTVAR(varname) (consoleOutput.Level1() << #varname << std::string(": ") << varname << std::endl);

#define PRINTVAR_TO_FILE(varname, fileoutput) \
    (consoleOutput.Level1() << #varname << std::string(": ") << varname << std::endl); \
    (fileoutput << #varname << std::string(": ") << varname << std::endl)



void test_data_results_segmentation::Print()
{
    PRINTVAR(SSEG_attempts_total);
    PRINTVAR(SSEG_attempts_in_which_there_WAS_a_target);
	PRINTVAR(SSEG_num_times_it_segmented_something);
	PRINTVAR(SSEG_failed_to_return_anything_when_there_was_NO_target);
	PRINTVAR(SSEG_failed_to_return_anything_when_there_WAS_a_target);
	PRINTVAR(SSEG_returned_something_when_there_was_NO_target);
	PRINTVAR(SSEG_returned_something_when_there_WAS_a_target);

    PRINTVAR(CSEG_attempts_total);
    PRINTVAR(CSEG_attempts_in_which_there_WAS_a_target);
	PRINTVAR(CSEG_num_times_it_segmented_something);
	PRINTVAR(CSEG_failed_to_return_anything_when_there_was_NO_target);
	PRINTVAR(CSEG_failed_to_return_anything_when_there_WAS_a_target);
	PRINTVAR(CSEG_returned_something_when_there_was_NO_target);
	PRINTVAR(CSEG_returned_something_when_there_WAS_a_target);
}


//"CSEG_successes" can variable per cropped image... but reduce this to either 0 or 1 per image
void test_data_results_segmentation::Update_My_CSEG_Data_DueToOther(test_data_results_segmentation* other_tester)
{
    if(other_tester != nullptr)
    {
        if(other_tester->CSEG_attempts_total > 0)
            CSEG_attempts_total++;

        if(other_tester->CSEG_attempts_in_which_there_WAS_a_target > 0)
            CSEG_attempts_in_which_there_WAS_a_target++;

        if(other_tester->CSEG_num_times_it_segmented_something > 0)
            CSEG_num_times_it_segmented_something++;

        if(other_tester->CSEG_returned_something_when_there_was_NO_target > 0)
            CSEG_returned_something_when_there_was_NO_target++;

        if(other_tester->CSEG_returned_something_when_there_WAS_a_target > 0)
            CSEG_returned_something_when_there_WAS_a_target++;

        if(other_tester->CSEG_failed_to_return_anything_when_there_was_NO_target == other_tester->CSEG_attempts_total)
        {
            CSEG_failed_to_return_anything_when_there_was_NO_target++;
        }

        if(other_tester->CSEG_failed_to_return_anything_when_there_WAS_a_target == other_tester->CSEG_attempts_total)
        {
            CSEG_failed_to_return_anything_when_there_WAS_a_target++;

            for(std::map<std::string,bool>::iterator iter = other_tester->CSEG_names_of_targets_it_failed_on.begin(); iter != other_tester->CSEG_names_of_targets_it_failed_on.end(); iter++)
            {
                CSEG_names_of_targets_it_failed_on[iter->first] = iter->second;
            }
        }
    }
}

//"SSEG_successes" can variable per cropped image... but reduce this to either 0 or 1 per image
void test_data_results_segmentation::Update_My_SSEG_Data_DueToOther(test_data_results_segmentation* other_tester)
{
    if(other_tester != nullptr)
    {
        if(other_tester->SSEG_attempts_total > 0)
            SSEG_attempts_total++;

        if(other_tester->SSEG_attempts_in_which_there_WAS_a_target > 0)
            SSEG_attempts_in_which_there_WAS_a_target++;

        if(other_tester->SSEG_num_times_it_segmented_something > 0)
            SSEG_num_times_it_segmented_something++;

        if(other_tester->SSEG_returned_something_when_there_was_NO_target > 0)
            SSEG_returned_something_when_there_was_NO_target++;

        if(other_tester->SSEG_returned_something_when_there_WAS_a_target > 0)
            SSEG_returned_something_when_there_WAS_a_target++;

        if(other_tester->SSEG_failed_to_return_anything_when_there_was_NO_target == other_tester->SSEG_attempts_total)
        {
            SSEG_failed_to_return_anything_when_there_was_NO_target++;
        }

        if(other_tester->SSEG_failed_to_return_anything_when_there_WAS_a_target == other_tester->SSEG_attempts_total)
        {
            SSEG_failed_to_return_anything_when_there_WAS_a_target++;

            for(std::map<std::string,bool>::iterator iter = other_tester->SSEG_names_of_targets_it_failed_on.begin(); iter != other_tester->SSEG_names_of_targets_it_failed_on.end(); iter++)
            {
                SSEG_names_of_targets_it_failed_on[iter->first] = iter->second;
            }
        }
    }
}




void test_data_results_segmentation::PrintToFile(std::fstream & outfile, Segmenter_Module_Settings * settings_used)
{
	if(outfile.is_open() && outfile.good())
    {
		outfile << std::endl << std::endl;

        PRINTVAR_TO_FILE(SSEG_attempts_total, outfile);
        PRINTVAR_TO_FILE(SSEG_attempts_in_which_there_WAS_a_target, outfile);
        PRINTVAR_TO_FILE(SSEG_num_times_it_segmented_something, outfile);
        PRINTVAR_TO_FILE(SSEG_failed_to_return_anything_when_there_was_NO_target, outfile);
        PRINTVAR_TO_FILE(SSEG_failed_to_return_anything_when_there_WAS_a_target, outfile);
        PRINTVAR_TO_FILE(SSEG_returned_something_when_there_was_NO_target, outfile);
        PRINTVAR_TO_FILE(SSEG_returned_something_when_there_WAS_a_target, outfile);

        PRINTVAR_TO_FILE(CSEG_attempts_total, outfile);
        PRINTVAR_TO_FILE(CSEG_attempts_in_which_there_WAS_a_target, outfile);
        PRINTVAR_TO_FILE(CSEG_num_times_it_segmented_something, outfile);
        PRINTVAR_TO_FILE(CSEG_failed_to_return_anything_when_there_was_NO_target, outfile);
        PRINTVAR_TO_FILE(CSEG_failed_to_return_anything_when_there_WAS_a_target, outfile);
        PRINTVAR_TO_FILE(CSEG_returned_something_when_there_was_NO_target, outfile);
        PRINTVAR_TO_FILE(CSEG_returned_something_when_there_WAS_a_target, outfile);

//------------------------------------------------
    if(my_test_type == testtype__unknown)
    {
        outfile << std::endl << std::string("test_data_results_segmentation: type unknown") << std::endl;
    }

    if(SSEG_attempts_total > 0)
    {
        outfile << std::endl << std::string("SSEG success rate (of images that had targets): ")
     <<(static_cast<float>(SSEG_returned_something_when_there_WAS_a_target)/static_cast<float>(SSEG_attempts_in_which_there_WAS_a_target) * 100.0f) << std::string(" %");

        outfile << std::endl << std::string("SSEG success rate for failing when there was NOT a target: ")
     <<(static_cast<float>(SSEG_failed_to_return_anything_when_there_was_NO_target)/static_cast<float>((SSEG_attempts_total - SSEG_attempts_in_which_there_WAS_a_target)) * 100.0f) << std::string(" %") << std::endl;

        if(SSEG_names_of_targets_it_failed_on.empty()==false)
        {
        outfile << std::string("==================================================") << std::endl << std::string("SSEG failed on these targets:") << std::endl << std::endl;
        for(std::map<std::string,bool>::iterator sfiter = SSEG_names_of_targets_it_failed_on.begin(); sfiter != SSEG_names_of_targets_it_failed_on.end(); sfiter++)
        {
            outfile << sfiter->first << std::endl;
        }
        outfile << std::string("==================================================") << std::endl;
        }
    }
    if(CSEG_attempts_total > 0)
    {
        outfile << std::endl << std::string("CSEG success rate (of images that had targets): ")
     <<(static_cast<float>(CSEG_returned_something_when_there_WAS_a_target)/static_cast<float>(CSEG_attempts_in_which_there_WAS_a_target) * 100.0f) << std::string(" %");

        outfile << std::endl << std::string("CSEG success rate for failing when there was NOT a target: ")
     <<(static_cast<float>(CSEG_failed_to_return_anything_when_there_was_NO_target)/static_cast<float>((CSEG_attempts_total - CSEG_attempts_in_which_there_WAS_a_target)) * 100.0f) << std::string(" %") << std::endl;

        if(CSEG_names_of_targets_it_failed_on.empty()==false)
        {
        outfile << std::string("==================================================") << std::endl << std::string("CSEG failed on these targets:") << std::endl << std::endl;
        for(std::map<std::string,bool>::iterator sfiter = CSEG_names_of_targets_it_failed_on.begin(); sfiter != CSEG_names_of_targets_it_failed_on.end(); sfiter++)
        {
            outfile << sfiter->first << std::endl;
        }
        outfile << std::string("==================================================") << std::endl;
        }
    }

//------------------------------------------------

		if(settings_used != nullptr && (my_test_type == testtype__per_segmentation || my_test_type == testtype__unknown))
		{
        outfile << std::endl << std::string("histogram: COLOR_DISTANCE_THRESHOLD ") << settings_used->HistSeg_COLOR_DISTANCE_THRESHOLD;
        outfile << std::endl << std::string("histogram: MERGE_COLOR_DISTANCE ") << settings_used->HistSeg_MERGE_COLOR_DISTANCE;
        outfile << std::endl << std::string("histogram: NUM_VALID_COLORS ") << settings_used->HistSeg_NUM_VALID_COLORS;
        outfile << std::endl << std::string("histogram: NUM_BINS ") << settings_used->HistSeg_NUM_BINS;
        outfile << std::endl << std::string("histogram: BLUR_PREPROCESS_RADIUS_PIXELS ") << settings_used->HistSeg_BLUR_PREPROCESS_RADIUS_PIXELS;

		outfile << std::endl << std::string("CV color convert type: ") << GetNameOfCVColorSpace(settings_used->preprocess_CV_conversion_type);
		outfile << std::endl << std::string("channels: [")
					<< (settings_used->preprocess_channels_to_keep[0] ? 1 : 0) << std::string(",")
					<< (settings_used->preprocess_channels_to_keep[1] ? 1 : 0) << std::string(",")
					<< (settings_used->preprocess_channels_to_keep[2] ? 1 : 0) << std::string("]") << std::endl;
		}

        outfile << std::flush;
    }
    else
    {
		Print();
    }
}




//#################################################################################################################


#define TRY_GET_OPTIONAL_INFO(variable) (optional_results_info == nullptr) ? 0 : optional_results_info->variable




void UpdateResultsAttemptsData_CSEG(std::ostream* PRINTHERE, test_data_results_segmentation* optional_results_info,
    bool there_was_a_target)
{
    TRY_GET_OPTIONAL_INFO(CSEG_attempts_total)++;

    if(there_was_a_target)
    {
        TRY_GET_OPTIONAL_INFO(CSEG_attempts_in_which_there_WAS_a_target)++;
    }
}
void UpdateResultsAttemptsData_SSEG(std::ostream* PRINTHERE, test_data_results_segmentation* optional_results_info,
    bool there_was_a_target)
{
    TRY_GET_OPTIONAL_INFO(SSEG_attempts_total)++;

    if(there_was_a_target)
    {
        TRY_GET_OPTIONAL_INFO(SSEG_attempts_in_which_there_WAS_a_target)++;
    }
}


void CheckValidityOfResults_CSEG(std::ostream* PRINTHERE, test_data_results_segmentation* optional_results_info,
    bool CSEG_returned_something, bool there_was_a_target, std::string* name_of_target_image)
{
    if(CSEG_returned_something)
    {
        TRY_GET_OPTIONAL_INFO(CSEG_num_times_it_segmented_something)++;
    }


    if(CSEG_returned_something)
    {
        if(there_was_a_target)
        {
            TRY_GET_OPTIONAL_INFO(CSEG_returned_something_when_there_WAS_a_target)++;
        }
        else
        {
            TRY_GET_OPTIONAL_INFO(CSEG_returned_something_when_there_was_NO_target)++;
        }
    }
    else
    {
        if(there_was_a_target)
        {
            TRY_GET_OPTIONAL_INFO(CSEG_failed_to_return_anything_when_there_WAS_a_target)++;

            if(optional_results_info != nullptr && name_of_target_image != nullptr)
                optional_results_info->CSEG_names_of_targets_it_failed_on[*name_of_target_image] = false;
        }
        else
        {
            TRY_GET_OPTIONAL_INFO(CSEG_failed_to_return_anything_when_there_was_NO_target)++;
        }
    }
}


void CheckValidityOfResults_SSEG(std::ostream* PRINTHERE, test_data_results_segmentation* optional_results_info,
    bool SSEG_returned_something, bool there_was_a_target, std::string* name_of_target_image)
{
    if(SSEG_returned_something)
    {
        TRY_GET_OPTIONAL_INFO(SSEG_num_times_it_segmented_something)++;
    }


    if(SSEG_returned_something)
    {
        if(there_was_a_target)
        {
            TRY_GET_OPTIONAL_INFO(SSEG_returned_something_when_there_WAS_a_target)++;
        }
        else
        {
            TRY_GET_OPTIONAL_INFO(SSEG_returned_something_when_there_was_NO_target)++;
        }
    }
    else
    {
        if(there_was_a_target)
        {
            TRY_GET_OPTIONAL_INFO(SSEG_failed_to_return_anything_when_there_WAS_a_target)++;

            if(optional_results_info != nullptr && name_of_target_image != nullptr)
                optional_results_info->SSEG_names_of_targets_it_failed_on[*name_of_target_image] = false;
        }
        else
        {
            TRY_GET_OPTIONAL_INFO(SSEG_failed_to_return_anything_when_there_was_NO_target)++;
        }
    }
}
