/**
 * @file test_data_results_shaperec.cpp
 * @brief Sets up results container for shaperec module testing.
 * @author Jason Bunk
 */

#include "test_data_results_shaperec.hpp"
#include "SharedUtils/SharedUtils.hpp"



#define PRINTVAR(varname) (consoleOutput.Level1() << #varname << std::string(": ") << varname << std::endl);

#define PRINTVAR_TO_FILE(varname, fileoutput) \
    (consoleOutput.Level1() << #varname << std::string(": ") << varname << std::endl); \
    (fileoutput << #varname << std::string(": ") << varname << std::endl)



void test_data_results_shaperec::Print()
{
    PRINTVAR(shapenamer_attempts);
    PRINTVAR(shapenamer_attempts_in_which_there_was_a_target);
    PRINTVAR(shapenamer_attempts_in_which_there_was_NOT_a_target);
    PRINTVAR(shapenamer_wrong_guesses);
    PRINTVAR(shapenamer_singleguess_successes);
    PRINTVAR(shapenamer_1stguess_successes);
    PRINTVAR(shapenamer_total_partial_successes);
    PRINTVAR(shapenamer_did_not_find_a_shape_when_there_was_one);
    PRINTVAR(shapenamer_did_not_find_a_shape_when_there_was_NOT_one);
}



void test_data_results_shaperec::PrintToFile(std::fstream & outfile)
{
	if(outfile.is_open() && outfile.good())
    {
		outfile << std::endl << std::endl;

        PRINTVAR_TO_FILE(shapenamer_attempts, outfile);
        PRINTVAR_TO_FILE(shapenamer_attempts_in_which_there_was_a_target, outfile);
        PRINTVAR_TO_FILE(shapenamer_attempts_in_which_there_was_NOT_a_target, outfile);
        PRINTVAR_TO_FILE(shapenamer_wrong_guesses, outfile);
        PRINTVAR_TO_FILE(shapenamer_singleguess_successes, outfile);
        PRINTVAR_TO_FILE(shapenamer_1stguess_successes, outfile);
        PRINTVAR_TO_FILE(shapenamer_total_partial_successes, outfile);
        PRINTVAR_TO_FILE(shapenamer_did_not_find_a_shape_when_there_was_one, outfile);
        PRINTVAR_TO_FILE(shapenamer_did_not_find_a_shape_when_there_was_NOT_one, outfile);

//------------------------------------------------

    if(shapenamer_attempts > 0)
    {
        outfile << std::endl << std::string("shapenamer success rate for naming targets (assuming SSEG succeeded): ")
     <<(static_cast<float>(shapenamer_1stguess_successes)/static_cast<float>(shapenamer_attempts_in_which_there_was_a_target) * 100.0f) << std::string(" %");

        outfile << std::endl << std::string("shapenamer success rate for saying non-targets aren't targets (assuming SSEG succeeded): ")
     <<(static_cast<float>(shapenamer_did_not_find_a_shape_when_there_was_NOT_one)/static_cast<float>(shapenamer_attempts_in_which_there_was_NOT_a_target) * 100.0f) << std::string(" %") << std::endl;
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




void CheckValidityOfResults_shaperec	(std::ostream* PRINTHERE, test_data_results_shaperec* optional_results_info,
	ShapeRec_ResultsContainer& results_to_check, std::string* correct_shape_name)
{
    if(results_to_check.empty())
    {
        consoleOutput.Level3() << "no good guesses of a shape name!";

        if(correct_shape_name != nullptr && correct_shape_name->empty())
        {
            consoleOutput.Level1() << "shapenamer was right! (said there was no shape, and there wasn't)";
            //TRY_GET_OPTIONAL_INFO(shapenamer_singleguess_successes)++;
            //TRY_GET_OPTIONAL_INFO(shapenamer_1stguess_successes)++;
            //TRY_GET_OPTIONAL_INFO(shapenamer_total_partial_successes)++;
            TRY_GET_OPTIONAL_INFO(shapenamer_did_not_find_a_shape_when_there_was_NOT_one)++;
        }
        else
        {
            consoleOutput.Level1() << "shapenamer was wrong! (did not find a shape, when there was one)";
            TRY_GET_OPTIONAL_INFO(shapenamer_did_not_find_a_shape_when_there_was_one)++;
        }
    }
    else if(results_to_check.size() == 1)
    {
        consoleOutput.Level3() << "one good guess found:  ";

#if DO_MAYBE_PRINTS
        if(PRINTHERE != nullptr)
            results_to_check[0].PrintMe(PRINTHERE);
#endif

        if(correct_shape_name != nullptr)
        {
            if(correct_shape_name->empty()==false && !__stricmp(
                get_chars_before_delim(results_to_check[0].reference_shape_name,'_').c_str(),
                correct_shape_name->c_str()))
            {
                consoleOutput.Level1() << "shapenamer was right! (found one shape, correct)";
                TRY_GET_OPTIONAL_INFO(shapenamer_singleguess_successes)++;
                TRY_GET_OPTIONAL_INFO(shapenamer_1stguess_successes)++;
                TRY_GET_OPTIONAL_INFO(shapenamer_total_partial_successes)++;
            }
            else
            {
                consoleOutput.Level1() << "shapenamer was wrong! (found one shape, but was incorrect)";
                TRY_GET_OPTIONAL_INFO(shapenamer_wrong_guesses)++;
            }
        }
    }
    else if(results_to_check.size() == 2)
    {
#if DO_MAYBE_PRINTS
        if(PRINTHERE != nullptr)
        {
            consoleOutput.Level3() << "two good, close guesses found:" << std::endl;
            consoleOutput.Level3() << "1st guess:\t";
            results_to_check.results[0].PrintMe(PRINTHERE);
            consoleOutput.Level3() << "2nd guess:\t";
            results_to_check.results[1].PrintMe(PRINTHERE);
        }
#endif

        if(correct_shape_name != nullptr)
        {
            if(correct_shape_name->empty()==false && !__stricmp(
                get_chars_before_delim(results_to_check[0].reference_shape_name,'_').c_str(),
                correct_shape_name->c_str()))
            {
                consoleOutput.Level1() << "shapenamer was partially right! (guess 1/2 was correct)";
                TRY_GET_OPTIONAL_INFO(shapenamer_1stguess_successes)++;
                TRY_GET_OPTIONAL_INFO(shapenamer_total_partial_successes)++;
            }
            else if(correct_shape_name->empty()==false && !__stricmp(
                get_chars_before_delim(results_to_check[1].reference_shape_name,'_').c_str(),
                correct_shape_name->c_str()))
            {
                consoleOutput.Level1() << "shapenamer was partially right! (guess 2/2 was correct)";
                TRY_GET_OPTIONAL_INFO(shapenamer_total_partial_successes)++;
            }
            else
            {
                consoleOutput.Level1() << "shapenamer was wrong! (made two guesses, but neither were correct)";
                TRY_GET_OPTIONAL_INFO(shapenamer_wrong_guesses)++;
            }
        }
    }
	else
	{
		consoleOutput.Level1() << "CheckValidityOfResults_shaperec() given too many guesses for shapes!" << std::endl << "      Try narrowing your possibilities to only 1 or 2 shapes." << std::endl;
	}
}



void UpdateResultsAttemptsData_shaperec (std::ostream* PRINTHERE, test_data_results_shaperec* optional_results_info,
	ShapeRec_ResultsContainer& results_to_check, std::string* correct_shape_name)
{
    TRY_GET_OPTIONAL_INFO(shapenamer_attempts)++;

    if(correct_shape_name != nullptr && correct_shape_name->empty())
    {
        TRY_GET_OPTIONAL_INFO(shapenamer_attempts_in_which_there_was_NOT_a_target)++;
    }
    else
    {
        TRY_GET_OPTIONAL_INFO(shapenamer_attempts_in_which_there_was_a_target)++;
    }
}

