/**
 * @file test_data_results_ocr.cpp
 * @brief Sets up results container for OCR module testing.
 * @author Jason Bunk
 */

#include "test_data_results_ocr.hpp"
#include "SharedUtils/SharedUtils.hpp"



#define PRINTVAR(varname) (consoleOutput.Level1() << #varname << std::string(": ") << varname << std::endl);

#define PRINTVAR_TO_FILE(varname, fileoutput) \
    (consoleOutput.Level1() << #varname << std::string(": ") << varname << std::endl); \
(fileoutput << #varname << std::string(": ") << varname << std::endl)



void test_data_results_ocr::Print()
{
    PRINTVAR(OCR_attempts);
    PRINTVAR(OCR_attempts_in_which_there_was_a_target);
    PRINTVAR(OCR_attempts_in_which_there_was_NOT_a_target);
    PRINTVAR(OCR_wrong_guesses);
    PRINTVAR(OCR_1st_successes);
    PRINTVAR(OCR_partial_successes);
    PRINTVAR(OCR_did_not_find_a_letter_when_there_was_one);
    PRINTVAR(OCR_did_not_find_a_letter_when_there_was_NOT_one);
}



void test_data_results_ocr::PrintToFile(std::fstream & outfile)
{
    if(outfile.is_open() && outfile.good())
    {
        outfile << std::endl << std::endl;

        PRINTVAR_TO_FILE(OCR_attempts, outfile);
        PRINTVAR_TO_FILE(OCR_attempts_in_which_there_was_a_target, outfile);
        PRINTVAR_TO_FILE(OCR_attempts_in_which_there_was_NOT_a_target, outfile);
        PRINTVAR_TO_FILE(OCR_wrong_guesses, outfile);
        PRINTVAR_TO_FILE(OCR_1st_successes, outfile);
        PRINTVAR_TO_FILE(OCR_partial_successes, outfile);
        PRINTVAR_TO_FILE(OCR_did_not_find_a_letter_when_there_was_one, outfile);
        PRINTVAR_TO_FILE(OCR_did_not_find_a_letter_when_there_was_NOT_one, outfile);

        //------------------------------------------------


        if(OCR_attempts > 0)
        {
            outfile << std::endl << std::string("OCR success rate for finding the letter (assuming CSEG succeeded): ")
                << (static_cast<float>(OCR_1st_successes)/static_cast<float>(OCR_attempts_in_which_there_was_a_target) * 100.0f) << std::string(" %");

            outfile << std::endl << std::string("OCR success rate for saying non-targets aren't targets (assuming CSEG succeeded): ")
                << (static_cast<float>(OCR_did_not_find_a_letter_when_there_was_NOT_one)/static_cast<float>(OCR_attempts_in_which_there_was_NOT_a_target) * 100.0f) << std::string(" %") << std::endl << std::endl;
        }

        //------------------------------------------------


        outfile << std::flush;
    }
    else
    {
        Print();
    }
}




//#################################################################################################################



#define TRY_GET_OPTIONAL_INFO(variable) (optional_results_info == nullptr) ? 0 : optional_results_info->variable





void CheckValidityOfResults_ocr			(std::ostream* PRINTHERE, test_data_results_ocr* optional_results_info,
        OCR_ResultsContainer& results_to_check, const char* correct_ocr_character)
{
    if(results_to_check.empty()) //OCR module didn't find a letter
    {
        if(correct_ocr_character != nullptr)
        {
            if((*correct_ocr_character) == 0) //null character means there wasn't supposed to be a target in the image
            {
                //TRY_GET_OPTIONAL_INFO(OCR_1st_successes)++;
                //TRY_GET_OPTIONAL_INFO(OCR_partial_successes)++;
                TRY_GET_OPTIONAL_INFO(OCR_did_not_find_a_letter_when_there_was_NOT_one)++;
            }
            else
            {
                TRY_GET_OPTIONAL_INFO(OCR_did_not_find_a_letter_when_there_was_one)++;
            }
        }
    }
    else
    {
        bool found_the_correct_letter = false;

        if(correct_ocr_character != nullptr)
        {
            if((*correct_ocr_character) != 0)
            {
                if(results_to_check.results[0].character == (*correct_ocr_character))
                {
                    TRY_GET_OPTIONAL_INFO(OCR_1st_successes)++;
                    TRY_GET_OPTIONAL_INFO(OCR_partial_successes)++;
                    found_the_correct_letter = true;
                }
                else
                {
                    for(std::vector<OCR_Result>::iterator ocr_results = results_to_check.results.begin();
                            ocr_results != results_to_check.results.end(); ocr_results++)
                    {
                        if(ocr_results->character == (*correct_ocr_character))
                        {
                            TRY_GET_OPTIONAL_INFO(OCR_partial_successes)++;
                            found_the_correct_letter = true;
                            break;
                        }
                    }
                }
            }


            if(found_the_correct_letter == false)
            {
                TRY_GET_OPTIONAL_INFO(OCR_wrong_guesses)++;
            }
        }
    }
}


void UpdateResultsAttemptsData_ocr      (std::ostream* PRINTHERE, test_data_results_ocr* optional_results_info,
        OCR_ResultsContainer& results_to_check, const char* correct_ocr_character)
{
    TRY_GET_OPTIONAL_INFO(OCR_attempts)++;

    if(correct_ocr_character == nullptr || *correct_ocr_character == 0)
    {
        TRY_GET_OPTIONAL_INFO(OCR_attempts_in_which_there_was_NOT_a_target)++;
    }
    else
    {
        TRY_GET_OPTIONAL_INFO(OCR_attempts_in_which_there_was_a_target)++;
    }
}


