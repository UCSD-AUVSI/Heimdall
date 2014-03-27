#ifndef ____OCR_RESULTS_H___
#define ____OCR_RESULTS_H___

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>


double AverageTwoAngles(double angle1, double angle2);


class OCR_Result
{
public:
	double confidence;
	double relative_angle_of_character_to_source_image;
	char character;

	OCR_Result() : confidence(0.0), relative_angle_of_character_to_source_image(0.0), character(0) {}
	OCR_Result(double CONFIDENCE, double ANGLE, char CHARACTER) : confidence(CONFIDENCE),
									relative_angle_of_character_to_source_image(ANGLE),
									character(CHARACTER) {}
	OCR_Result(const OCR_Result &rhs) : confidence(rhs.confidence),
						relative_angle_of_character_to_source_image(rhs.relative_angle_of_character_to_source_image),
						character(rhs.character) {}

	static bool SortByConfidence(const OCR_Result &lhs, const OCR_Result &rhs) {return (lhs.confidence > rhs.confidence);}
	static bool SortByCharacter(const OCR_Result &lhs, const OCR_Result &rhs) {return (lhs.character > rhs.character);}

    std::string GetCharacterAsString();
	void PrintMe(std::ostream* PRINTHERE);
};



class OCR_ResultFinal : public OCR_Result
{
public:
	double total_net_confidence;

	OCR_ResultFinal() : OCR_Result(), total_net_confidence(0.0) {}
	OCR_ResultFinal(double CONFIDENCE, double TOTAL_NET_CONFIDENCE, double ANGLE, char CHARACTER)
			: OCR_Result(CONFIDENCE,ANGLE,CHARACTER), total_net_confidence(TOTAL_NET_CONFIDENCE) {}
	OCR_ResultFinal(const OCR_ResultFinal &rhs) : OCR_Result(rhs), total_net_confidence(rhs.total_net_confidence) {}
};



class OCR_ResultsContainer
{
public:
	std::vector<OCR_Result> results;
//----
public:

    OCR_ResultsContainer() {}
    OCR_ResultsContainer(const std::vector<OCR_Result> & other_results) : results(other_results) {}


	void clear() {results.clear();}
	bool empty() {return results.empty();}
    size_t size() {return results.size();}
    OCR_Result& operator[] (int x) {return results[x];}

	void PushBackNew(double CONFIDENCE, double ANGLE, char CHARACTER)
	{
		results.push_back(OCR_Result(CONFIDENCE,ANGLE,CHARACTER));
	}

	void EliminateDuplicates();

    //descending order
	void SortByConfidence() {std::sort(results.begin(), results.end(), OCR_Result::SortByConfidence);}
	void SortByCharacter() {std::sort(results.begin(), results.end(), OCR_Result::SortByCharacter);}

	void EliminateCharactersBelowConfidenceLevel(double minimum_confidence);

	//i.e. sort descending by confidence and then trim off the top fraction of characters
	void KeepOnlyTopFractionOfCharacters(double fraction);

	void GetTopCharactersWithHighestConfidences(OCR_ResultFinal*& returned_result1, OCR_ResultFinal*& returned_result2,
							double threshold_percent_difference_of_guesses_to_report_guess2,
							std::ostream* PRINTHERE=nullptr);

    std::vector<OCR_Result> GetTopNResults(int max_num_results_sought, double cutoff_confidence_of_final_result);

	void PrintMyResults(std::ostream* PRINTHERE)
	{
        if(results.empty() == false)
        {
            for(int aa=0; aa < results.size(); aa++)
            {
                results[aa].PrintMe(PRINTHERE);
            }
		}
	}

//--- static

	static double GetMeanConfidence(std::vector<OCR_Result> & theresults, char character);

	static double GetMeanAngle_FromOCRResults(std::vector<OCR_Result> & theresults, char character);
};



class OCR_duplicate_eliminator
{
public:
    std::vector<double> angles_to_reference;
    double confidence_total;
    int num_instances;

    OCR_duplicate_eliminator() : confidence_total(0.0), num_instances(0) {}
};




#endif
