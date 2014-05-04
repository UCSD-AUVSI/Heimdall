/**
 * @file ocr_results.cpp
 * @brief OCR utility functions; also sets up utility OCR results containers, to be used by any OCR module.
 * @author Jason Bunk
 */

#include "ocr_results.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include <map>
#include <math.h>


std::string letters_with_only_one_orientation ("ABCDEFG  JKLM  PQR TUVW Y abcdefghijk mn pqr tuvw y  1234567 9");
std::string letters_with_only_two_orientations(       "HI    N    S    X Z           l      s    x z        8 ");
std::string letters_with_more_than_two_orients(              "O                         o           0");

std::string letters_that_are_easily_mixed_up  (  "C     I   MNOP  S  VWX Z bcd  g    lmnopq s uvwx z01    6  9");

bool letter_has_only_one_orientation(char letter)
{
	if(letter == ' '){return false;}
	return (letters_with_only_one_orientation.find(letter) != letters_with_only_one_orientation.npos);
}
bool letter_has_two_orientations(char letter)
{
	if(letter == ' '){return false;}
	return (letters_with_only_two_orientations.find(letter) != letters_with_only_two_orientations.npos);
}
bool letter_is_difficult_and_easily_mixed_up(char letter)
{
	if(letter == ' '){return false;}
	return (letters_that_are_easily_mixed_up.find(letter) != letters_that_are_easily_mixed_up.npos);
}


std::string OCR_Result::GetCharacterAsString()
{
    if(character == 0)
        return std::string();

    char retstr[2];
    retstr[0]=character;
    retstr[1]=0;
    return std::string(retstr);
}

void OCR_Result::PrintMe(std::ostream* PRINTHERE)
{
    if(PRINTHERE != nullptr)
    {
        (*PRINTHERE) << "character:  " << character //<< "  (ascii #" << ((int)(character)) << ")"
            << ", confidence: " << confidence << ", relative angle to source image: "
            << relative_angle_of_character_to_source_image << std::endl;
    }
}


OCR_Result OCR_ResultsContainer::GetResultFromLetter(char letter_known_to_be_in_results)
{
	std::vector<OCR_Result>::iterator iter = results.begin();
	for(; iter != results.end(); iter++) {
		if(iter->character == letter_known_to_be_in_results) {
			return *iter; //return the first found; if sorted by confidence, that'll probably be the one you want
		}
	}
	return OCR_Result();
}

bool OCR_ResultsContainer::ContainsLetter(char what_letter)
{
	std::vector<OCR_Result>::iterator iter = results.begin();
	for(; iter != results.end(); iter++) {
		if(iter->character == what_letter) {
			return true;
		}
	}
	return false;
}

void OCR_ResultsContainer::EliminateDuplicates_BySavingHighestConfidence()
{
	SortByConfidence();
	
	std::map<char, bool> ocr_characters_map;
	std::vector<OCR_Result>::iterator results_iter = results.begin();
	
	while(results_iter != results.end())
	{
		if(ocr_characters_map.find(results_iter->character) == ocr_characters_map.end())
		{
			ocr_characters_map[results_iter->character] = true;
			results_iter++;
		}
		else
		{
			results_iter = results.erase(results_iter);
		}
	}
}

void OCR_ResultsContainer::EliminateDuplicates_ByAveraging()
{
//merges duplicate letter results (from one saliency crop) from different CSEGs
//note: crucially, this averages the confidences of the top results...
//which means a letter that appears only a few times with high confidence is preferred

    std::map<char, OCR_duplicate_eliminator>                    ocr_characters_map;
    std::map<char, OCR_duplicate_eliminator>::iterator          ocr_characters_iter;



    std::vector<OCR_Result>::iterator ocr_iter = results.begin();

    for(; ocr_iter != results.end(); ocr_iter++)
    {
        ocr_characters_iter = ocr_characters_map.find(ocr_iter->character);

        if(ocr_characters_iter == ocr_characters_map.end()) //not a duplicate
            ocr_characters_map[ocr_iter->character] = OCR_duplicate_eliminator();

        ocr_characters_map[ocr_iter->character].confidence_total += ocr_iter->confidence;
        ocr_characters_map[ocr_iter->character].num_instances++;
        ocr_characters_map[ocr_iter->character].angles_to_reference.push_back(ocr_iter->relative_angle_of_character_to_source_image);
    }

    results.clear();

    ocr_characters_iter = ocr_characters_map.begin();
    for(; ocr_characters_iter != ocr_characters_map.end(); ocr_characters_iter++)
    {
        results.push_back(OCR_Result(

        (ocr_characters_iter->second.confidence_total / static_cast<double>(ocr_characters_iter->second.num_instances)),
        GetMeanAngle(ocr_characters_iter->second.angles_to_reference),
        ocr_characters_iter->first

        ));
    }

    SortByConfidence();
}


void OCR_ResultsContainer::EliminateCharactersBelowConfidenceLevel(double minimum_confidence)
{
	SortByConfidence();
	std::vector<OCR_Result>::iterator iter = results.begin();
	for(; iter != results.end(); iter++)
	{
		if(iter->confidence < minimum_confidence)
			break;
	}

	if(iter != results.end())
		results.erase(iter, results.end());
}


void OCR_ResultsContainer::KeepOnlyTopFractionOfCharacters(double fraction)
{
	SortByConfidence();
	int num_to_keep = RoundDoubleToInteger(fraction * static_cast<double>(results.size()));
	if(num_to_keep > 0 && num_to_keep < results.size())
		results.erase(results.begin()+num_to_keep, results.end());
}


void OCR_ResultsContainer::GetTopCharactersWithHighestConfidences(OCR_ResultFinal*& returned_result1, OCR_ResultFinal*& returned_result2,
							double threshold_percent_difference_of_guesses_to_report_guess2,
							std::ostream* PRINTHERE/*=nullptr*/)
{
	if(returned_result1 != nullptr)
	{
		delete returned_result1; returned_result1=nullptr;
	}
	if(returned_result2 != nullptr)
	{
		delete returned_result2; returned_result2=nullptr;
	}

	std::map<char,double> character_totalconfidences;
	std::map<char,int> character_appearances;

	std::vector<OCR_Result>::iterator resiter = results.begin();
	for(; resiter != results.end(); resiter++)
	{
		character_totalconfidences[resiter->character] += resiter->confidence;
		character_appearances[resiter->character] += 1;
	}

	char chosen_char=0;		double chosen_char_total_confidence=0.0;
	char chosen_char_runnerup=0;	double chosen_char_total_confidence_runnerup=0.0;

	std::map<char,double>::iterator citer = character_totalconfidences.begin();
	for(; citer != character_totalconfidences.end(); citer++)
	{
		if(citer->second > chosen_char_total_confidence)
		{
			chosen_char_runnerup = chosen_char;
			chosen_char = citer->first;

			chosen_char_total_confidence_runnerup = chosen_char_total_confidence;
			chosen_char_total_confidence = citer->second;
		}
	}

	if(chosen_char != 0)
	{
		returned_result1 = new OCR_ResultFinal(chosen_char_total_confidence / static_cast<double>(character_appearances[chosen_char]),
							chosen_char_total_confidence,
							GetMeanAngle_FromOCRResults(results, chosen_char),
							chosen_char);
	}
	if(chosen_char_runnerup != 0)
	{
		if((fabs(chosen_char_total_confidence - chosen_char_total_confidence_runnerup) / chosen_char_total_confidence)
				< threshold_percent_difference_of_guesses_to_report_guess2)
		{
		returned_result2 = new OCR_ResultFinal(chosen_char_total_confidence_runnerup / static_cast<double>(character_appearances[chosen_char_runnerup]),
							chosen_char_total_confidence_runnerup,
							GetMeanAngle_FromOCRResults(results, chosen_char_runnerup),
							chosen_char_runnerup);
		}

        if(PRINTHERE != nullptr)
        {
            (*PRINTHERE) << "percent difference between top two guesses: "
            << (fabs(chosen_char_total_confidence - chosen_char_total_confidence_runnerup) / chosen_char_total_confidence) << std::endl;
        }
	}
}


std::vector<OCR_Result> OCR_ResultsContainer::GetTopNResults(int max_num_results_sought, double cutoff_confidence_of_final_result)
{
    std::vector<OCR_Result> returned_results(results);
    std::map<char, bool> checkmap;
    std::map<char, bool>::iterator checkmap_iter;

	SortByConfidence();


    //first: keep only the results with the minimum confidence
    for(std::vector<OCR_Result>::iterator riter = returned_results.begin(); riter != returned_results.end();)
    {
        if(riter->confidence < cutoff_confidence_of_final_result)
            riter = returned_results.erase(riter);
        else
            riter++;
    }


    //second: eliminate duplicates
    //since the list has been sorted by confidence,
    //	only the highest-confidence instance of a letter will be kept
    for(std::vector<OCR_Result>::iterator riter = returned_results.begin(); riter != returned_results.end();)
    {
        if(checkmap.empty())
        {
            checkmap[riter->character] = true;
            riter++;
        }
        else
        {
            checkmap_iter = checkmap.find(riter->character);

            if(checkmap_iter != checkmap.end()) //duplicate entry
                riter = returned_results.erase(riter);
            else
            {
                checkmap[riter->character] = true;
                riter++;
            }
        }
    }


    if(returned_results.size() > max_num_results_sought)
        returned_results.erase(returned_results.begin()+max_num_results_sought, returned_results.end());


    return returned_results;
}


/*static*/ double OCR_ResultsContainer::GetMeanConfidence(std::vector<OCR_Result> & theresults, char character)
{
	double conftotal = 0.0;
	double numfound = 0.0;

	std::vector<OCR_Result>::iterator iter = theresults.begin();
	for(; iter != theresults.end(); iter++)
	{
		if(iter->character == character)
		{
			conftotal += iter->confidence;
			numfound += 1.0;
		}
	}
	return conftotal / numfound;
}


void Add_Componentwise_AngleVectors(std::vector<OCR_Result> & input_letters,
													char whichletter,
													double & returned_x,
													double & returned_y,
													int & returned_num_appearances)
{
	std::vector<OCR_Result>::iterator iter = input_letters.begin();
	for(; iter != input_letters.end(); iter++)
	{
		if(iter->character == whichletter)
		{
			returned_x += cos(iter->relative_angle_of_character_to_source_image * 0.017453292519943296); //to radians
			returned_y += sin(iter->relative_angle_of_character_to_source_image * 0.017453292519943296);
			returned_num_appearances++;
		}
	}
}

			
void Add_Componentwise_AngleVectors(std::vector<OCR_angle_sorter> & input_letters,
													double & returned_x,
													double & returned_y,
													int & returned_num_appearances)
{
	std::vector<OCR_angle_sorter>::iterator iter = input_letters.begin();
	for(; iter != input_letters.end(); iter++)
	{
		returned_x += cos(iter->its_angle * 0.017453292519943296); //to radians
		returned_y += sin(iter->its_angle * 0.017453292519943296);
		returned_num_appearances++;
	}
}


double CalculateMeanAndStdDev(double & xx, double & yy, double num_appearances, double* returned_standard_deviance)
{
	if(returned_standard_deviance != nullptr || num_appearances < 1.5) {
		(*returned_standard_deviance) = 2.0;
	}
	
	//for the math behind this function, see:
	//https://en.wikipedia.org/wiki/Directional_statistics
	
	if((fabs(xx)+fabs(yy)) < 0.000001)
		return 0.0;
	
	if(returned_standard_deviance != nullptr) {
		double mean_length_sq = pow(xx/num_appearances,2.0) + pow(yy/num_appearances,2.0);
		(*returned_standard_deviance) = sqrt(log(1.0/mean_length_sq));
	}

	return (atan2(yy,xx) * 57.295779513082321); //back to degrees
}


/*static*/ double OCR_ResultsContainer::GetMeanAngle_FromOCRResults(std::vector<OCR_Result> & theresults, char character, double* returned_standard_deviance/*=nullptr*/)
{	
	double xx = 0.0, yy = 0.0;
	int num_appearances = 0;
	Add_Componentwise_AngleVectors(theresults, character, xx, yy, num_appearances);
	return CalculateMeanAndStdDev(xx, yy, static_cast<double>(num_appearances), returned_standard_deviance);
}


/*static*/ double OCR_ResultsContainer::GetMeanAngle_From_OCR_angle_sorter(std::vector<OCR_angle_sorter> & thebin, double* returned_standard_deviance/*=nullptr*/)
{
	double xx = 0.0, yy = 0.0;
	int num_appearances = 0;
	Add_Componentwise_AngleVectors(thebin, xx, yy, num_appearances);
	return CalculateMeanAndStdDev(xx, yy, static_cast<double>(num_appearances), returned_standard_deviance);
}


/*static*/ double OCR_ResultsContainer::Get_DoubleBinned_StdDev_FromOCRResults(std::vector<OCR_Result> & theresults, char character)
{
	double bin_left_meanangle, bin_rght_meanangle;
	double bin_left_stddev,  bin_rght_stddev;
	
	std::vector<double> saved_total_stddevs;
	
	std::vector<OCR_angle_sorter> bin_left;
	std::vector<OCR_angle_sorter> bin_rght;
	
	std::vector<OCR_Result>::iterator iter = theresults.begin();
	for(; iter != theresults.end(); iter++)
	{
		if(iter->character == character)
		{
			bin_left.push_back(OCR_angle_sorter(character, iter->relative_angle_of_character_to_source_image));
		}
	}
	
	std::sort(bin_left.begin(), bin_left.end(), OCR_angle_sorter::SortByAngle);
	
	while(bin_left.empty() == false)
	{
		bin_left_meanangle = GetMeanAngle_From_OCR_angle_sorter(bin_left, &bin_left_stddev);
		bin_rght_meanangle = GetMeanAngle_From_OCR_angle_sorter(bin_left, &bin_rght_stddev);
		
		saved_total_stddevs.push_back(bin_left_meanangle + bin_rght_meanangle);
		
		
		//--------------
		std::cout << "leftbin tot_stddev: " << saved_total_stddevs.back() << ", \t";
		for(std::vector<OCR_angle_sorter>::iterator biniter = bin_left.begin(); biniter != bin_left.end(); biniter++) {
			std::cout << biniter->the_letter;
		}
		std::cout << std::endl;
		//--------------
		
		
		bin_rght.push_back(bin_left.back());
		bin_left.pop_back();
	}
	
	if(saved_total_stddevs.empty() == false) {
		std::sort(saved_total_stddevs.begin(), saved_total_stddevs.end());
		
		//--------------
		std::cout << "returned total stddev: " << saved_total_stddevs.front() << std::endl;
		//--------------
		
		return saved_total_stddevs.front();
	}
	return 5.0;
}


double AverageTwoAngles(double angle1, double angle2)
{
	double xx = 0.0;
	double yy = 0.0;

	xx += cos(angle1 * 0.017453292519943296); //to radians
	yy += sin(angle1 * 0.017453292519943296);

	xx += cos(angle2 * 0.017453292519943296); //to radians
	yy += sin(angle2 * 0.017453292519943296);

	if((fabs(xx)+fabs(yy)) < 0.000001)
		return 0.0;

	return (atan2(yy,xx) * 57.295779513082321); //back to degrees
}

