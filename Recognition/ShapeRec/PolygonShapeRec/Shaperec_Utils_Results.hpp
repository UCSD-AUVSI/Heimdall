#ifndef __SHAPEREC_RESULTS_H____
#define __SHAPEREC_RESULTS_H____

#include <string>
#include <vector>
#include <algorithm>

#define SHAPEREC_METHOD4_ABSOLUTE_CONFIDENCE_FORMULA(themetric4) std::max(0.0, (1.0-(themetric4)))
#define SHAPEREC_METHOD4_ABSOLUTE_CONFIDENCE_FORMULA_INVERSE(absconff) std::max(0.0, (1.0-(absconff)))


class ShapeRec_Result
{
public:
    std::string reference_shape_name;

	double relative_angle_to_reference;

#if CONSIDER_CV_SHAPEMATCH_ALGORITHMS
	double match_amount_method1;
	double match_amount_method2;
	double match_amount_method3;
#endif

	double metric_method44;
	double multi_seg_image_processed_averaged_metric_method44; //used later, after compiling multiple results from multiple segmentations

//------

#if CONSIDER_CV_SHAPEMATCH_ALGORITHMS
	ShapeRec_Result() : match_amount_method1(0.0), match_amount_method2(0.0), match_amount_method3(0.0), metric_method44(0.0) {}
	ShapeRec_Result(const std::string & SHAPE, double METHOD1, double METHOD2, double METHOD3, double MMETHOD44)
		: reference_shape(SHAPE), match_amount_method1(METHOD1), match_amount_method2(METHOD2), match_amount_method3(METHOD3), metric_method44(MMETHOD44) {}
#else
	ShapeRec_Result() : relative_angle_to_reference(0.0), metric_method44(0.0), multi_seg_image_processed_averaged_metric_method44(0.0) {}
	ShapeRec_Result(const std::string & SHAPE, double RELATIVE_ANGLE, double MMETHOD44)
		: reference_shape_name(SHAPE), relative_angle_to_reference(RELATIVE_ANGLE), metric_method44(MMETHOD44) {}
#endif

	void PrintMe(std::ostream* PRINTHERE);

#if CONSIDER_CV_SHAPEMATCH_ALGORITHMS
	static bool SortByMethod1(const ShapeRec_Result &lhs, const ShapeRec_Result &rhs) {return (lhs.match_amount_method1 < rhs.match_amount_method1);}
	static bool SortByMethod2(const ShapeRec_Result &lhs, const ShapeRec_Result &rhs) {return (lhs.match_amount_method2 < rhs.match_amount_method2);}
	static bool SortByMethod3(const ShapeRec_Result &lhs, const ShapeRec_Result &rhs) {return (lhs.match_amount_method3 < rhs.match_amount_method3);}
#endif
	static bool SortByMethod44descending(const ShapeRec_Result &lhs, const ShapeRec_Result &rhs) {return (lhs.metric_method44 < rhs.metric_method44);}
};



class ShapeRec_ResultsContainer
{
public:
	std::vector<ShapeRec_Result> results;



    void clear() {results.clear();}
    bool empty() {return results.empty();}
    size_t size() {return results.size();}
    ShapeRec_Result& operator[] (int x) {return results[x];}

//--------------------

    ShapeRec_ResultsContainer() {}
    ShapeRec_ResultsContainer(const std::vector<ShapeRec_Result> & others_results) : results(others_results) {}


	/*
	first function:
	if you don't care about max/min stuff:
	in this case, increasing relative thresh will increase the number of times the "a close second guess was found!" message will appear


	second function:
	if you do:
	max absolute confidence means any match above that confidence will always be reported
	min absolute confidence means, if the shape is >= min absolute confidence AND it's relative confidence is >= max relative, it's reported
	*/


	std::vector<ShapeRec_Result> GetTopResults(	double absolute_confidence_threshold,
											double relative_confidence_threshold,
											std::ostream* PRINTHERE,
											bool print_winner_results=false, bool print_all_results=false);


	std::vector<ShapeRec_Result> GetTopResults(	double max_absolute_confidence_threshold,
											double min_absolute_confidence_threshold,
											double max_relative_confidence_threshold,
											double min_relative_confidence_threshold,
											std::ostream* PRINTHERE,
											bool print_winner_results=false, bool print_all_results=false);


	void SortAllByMethod44() {std::sort(results.begin(), results.end(), ShapeRec_Result::SortByMethod44descending);}


	void PrintTopTwoWinnersOfResults(std::ostream* PRINTHERE);
	void PrintAllResults(std::ostream* PRINTHERE);
protected:
	void PrintWinnerOfResults(std::ostream* PRINTHERE);

public:

    /**
    * This method (EliminateDuplicates) is important because the SSEG module will give multiple segmentations
    * of the same shape, and since we'll run the ShapeRecModuleAlgorithm on each segmentation
    * (and save each result to here), we should get duplicate results if the segmentations all look similar.
    **/
    void EliminateDuplicates();
};




//sorting by multiplied confidences is better; they both produce the same amount of false-positives
#define SHAPEREC_SORT_PRIMARILY_BY_MULTIPLIED_CONFIDENCES 1


class shape_duplicate_eliminator
{
public:
    std::vector<double> angles_to_reference;
    double metric_method44_total_multiplies_of_1_minus_confidence;
    double total_metric_method44_sum;
    int num_instances;

    shape_duplicate_eliminator() : metric_method44_total_multiplies_of_1_minus_confidence(1.0),
                                    total_metric_method44_sum(0.0),
                                    num_instances(0) {}
};


#endif
