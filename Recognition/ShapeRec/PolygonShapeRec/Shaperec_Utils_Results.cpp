/**
 * @file Shaperec_Utils_Results.cpp
 * @brief Sets up shape recognition module results containers, to be used by both the full shape recognition module and the single-image-input helper "mini-module".
 * @author Jason Bunk
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "Shaperec_Utils_Results.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include <math.h>



void ShapeRec_Result::PrintMe(std::ostream* PRINTHERE)
{
if(PRINTHERE == nullptr) return;

#if CONSIDER_CV_SHAPEMATCH_ALGORITHMS
	(*PRINTHERE) << "shape " << reference_shape << ":\tmethod 1: " << match_amount_method1
		<< "   \tmethod 2: " << match_amount_method2 << "   \tmethod 3: " << match_amount_method3 << "\tmethod 4: " << metric_method44 << std::endl;
#else
	(*PRINTHERE) << "shape " << reference_shape_name << ":\tangle:" << relative_angle_to_reference << "\tmetric: " << metric_method44 << "   \tabs. conf: " << SHAPEREC_METHOD4_ABSOLUTE_CONFIDENCE_FORMULA(metric_method44) << std::endl;
#endif
}

void ShapeRec_ResultsContainer::PrintAllResults(std::ostream* PRINTHERE)
{
if(PRINTHERE == nullptr) return;

	std::vector<ShapeRec_Result>::iterator iter = results.begin();
	for(; iter != results.end(); iter++)
		iter->PrintMe(PRINTHERE);
}

void ShapeRec_ResultsContainer::PrintWinnerOfResults(std::ostream* PRINTHERE)
{
if(PRINTHERE == nullptr) return;

#if CONSIDER_CV_SHAPEMATCH_ALGORITHMS
	std::string winnershape_method1;
	std::string winnershape_method2;
	std::string winnershape_method3;
#endif
	std::string winnershape_method4;
#if CONSIDER_CV_SHAPEMATCH_ALGORITHMS
	double winneramount_method1 = 9999999999.0;
	double winneramount_method2 = 9999999999.0;
	double winneramount_method3 = 9999999999.0;
#endif
	double winneramount_method4 = 9999999999.0;

	for(int aaa=0; aaa < results.size(); aaa++)
	{
#if CONSIDER_CV_SHAPEMATCH_ALGORITHMS
		if(results[aaa].match_amount_method1 < winneramount_method1)
		{
			winnershape_method1 = results[aaa].reference_shape;
			winneramount_method1 = results[aaa].match_amount_method1;
		}
		if(results[aaa].match_amount_method2 < winneramount_method2)
		{
			winnershape_method2 = results[aaa].reference_shape;
			winneramount_method2 = results[aaa].match_amount_method2;
		}
		if(results[aaa].match_amount_method3 < winneramount_method3)
		{
			winnershape_method3 = results[aaa].reference_shape;
			winneramount_method3 = results[aaa].match_amount_method3;
		}
#endif
		if(results[aaa].metric_method44 < winneramount_method4)
		{
			winnershape_method4 = results[aaa].reference_shape_name;
			winneramount_method4 = results[aaa].metric_method44;
		}
	}

#if CONSIDER_CV_SHAPEMATCH_ALGORITHMS
	(*PRINTHERE) << "best guess of method 1 is shape " << winnershape_method1 << std::endl;
	(*PRINTHERE) << "best guess of method 2 is shape " << winnershape_method2 << std::endl;
	(*PRINTHERE) << "best guess of method 3 is shape " << winnershape_method3 << std::endl;
#endif
	(*PRINTHERE) << "best guess of method 4 is shape " << winnershape_method4 << std::endl;
}

void ShapeRec_ResultsContainer::PrintTopTwoWinnersOfResults(std::ostream* PRINTHERE)
{
if(PRINTHERE == nullptr) return;

	double relative_confidence = 0.0;
	double absolute_confidence = 0.0;
	std::vector<ShapeRec_Result>::iterator runnerup;
	std::vector<ShapeRec_Result>::iterator runnerup22;

#if CONSIDER_CV_SHAPEMATCH_ALGORITHMS
	std::sort(results.begin(), results.end(), ShapeRec_Result::SortByMethod1);
	(*PRINTHERE) << "best guess of method 1 is a " << ShapeRecognizer::ShapeNamesTable[results.begin()->reference_shape] << "    \t(shape " << results.begin()->reference_shape << ")" << std::endl;
	runnerup = results.begin();
	runnerup++;
	relative_confidence = sqrt((runnerup->match_amount_method1 - results.begin()->match_amount_method1) / runnerup->match_amount_method1);
	absolute_confidence = (results.begin()->match_amount_method1 < 1.0) ? pow(1.0 - results.begin()->match_amount_method1, 9.0) : 0.0;
	(*PRINTHERE) << "\trelative confidence: " << relative_confidence << "   \tabsolute confidence: " << absolute_confidence << std::endl;


	std::sort(results.begin(), results.end(), ShapeRec_Result::SortByMethod2);
	(*PRINTHERE) << "best guess of method 2 is a " << ShapeRecognizer::ShapeNamesTable[results.begin()->reference_shape] << "    \t(shape " << results.begin()->reference_shape << ")" << std::endl;
	runnerup = results.begin();
	runnerup++;
	relative_confidence = sqrt((runnerup->match_amount_method2 - results.begin()->match_amount_method2) / runnerup->match_amount_method2);
	absolute_confidence = (results.begin()->match_amount_method2 < 1.0) ? pow(1.0 - results.begin()->match_amount_method2, 9.0) : 0.0;
	(*PRINTHERE) << "\trelative confidence: " << relative_confidence << "   \tabsolute confidence: " << absolute_confidence << std::endl;


	std::sort(results.begin(), results.end(), ShapeRec_Result::SortByMethod3);
	(*PRINTHERE) << "best guess of method 3 is a " << ShapeRecognizer::ShapeNamesTable[results.begin()->reference_shape] << "    \t(shape " << results.begin()->reference_shape << ")" << std::endl;
	runnerup = results.begin();
	runnerup++;
	relative_confidence = sqrt((runnerup->match_amount_method3 - results.begin()->match_amount_method3) / runnerup->match_amount_method3);
	absolute_confidence = (results.begin()->match_amount_method3 < 1.0) ? pow(1.0 - results.begin()->match_amount_method3, 9.0) : 0.0;
	(*PRINTHERE) << "\trelative confidence: " << relative_confidence << "   \tabsolute confidence: " << absolute_confidence << std::endl;
#endif

	if(results.empty() == false)
	{
		std::sort(results.begin(), results.end(), ShapeRec_Result::SortByMethod44descending);
		(*PRINTHERE) << "best guess of method 4 is a " << results.begin()->reference_shape_name << std::endl;
		runnerup = results.begin();
		runnerup++;
		relative_confidence = ((runnerup == results.end()) ? 1.0 : sqrt((runnerup->metric_method44 - results.begin()->metric_method44) / runnerup->metric_method44));
		absolute_confidence = results.begin()->metric_method44;
		(*PRINTHERE) << "\trelative confidence: " << relative_confidence << "   \tmatch metric value: " << absolute_confidence << std::endl;

		if(runnerup != results.end())
		{
			runnerup22 = runnerup;
			runnerup22++;
			(*PRINTHERE) << "---------------" << std::endl;
			(*PRINTHERE) << "SECOND guess of method 4 is a " << runnerup->reference_shape_name << std::endl;
			relative_confidence = ((runnerup22 == results.end()) ? 1.0 : sqrt((runnerup22->metric_method44 - runnerup->metric_method44) / runnerup22->metric_method44));
			absolute_confidence = runnerup->metric_method44;
			(*PRINTHERE) << "\trelative confidence (to #3): " << relative_confidence << "   \tmatch metric value: " << absolute_confidence << std::endl;
		}
	}
}



std::vector<ShapeRec_Result>
ShapeRec_ResultsContainer::GetTopResults(	double absolute_confidence_threshold,
											double relative_confidence_threshold,
											std::ostream* PRINTHERE,
											bool print_winner_results/*=false*/, bool print_all_results/*=false*/)
{
	return GetTopResults(absolute_confidence_threshold, 1.0, 1.0, relative_confidence_threshold, PRINTHERE, print_winner_results, print_all_results);
}


std::vector<ShapeRec_Result>
ShapeRec_ResultsContainer::GetTopResults(	double max_absolute_confidence_threshold,
											double min_absolute_confidence_threshold,
											double max_relative_confidence_threshold,
											double min_relative_confidence_threshold,
											std::ostream* PRINTHERE,
											bool print_winner_results/*=false*/,
											bool print_all_results/*=false*/)
{
	std::vector<ShapeRec_Result> returned_shape_guesses;
	if(results.empty())
        return returned_shape_guesses;
	
	print_all_results = true;
	PRINTHERE = &std::cout;

	if(print_all_results && PRINTHERE != nullptr)
	{
		PrintAllResults(PRINTHERE);
		(*PRINTHERE) << "--------------------------------------------" << std::endl;
	}

	double relative_confidence = 0.0;
	double absolute_confidence = 0.0;
	std::vector<ShapeRec_Result>::iterator runnerup;
	std::vector<ShapeRec_Result>::iterator runnerup22;


	//this considers algorithm 4 only, which means not any of the 3 OpenCV algorithms


	std::sort(results.begin(), results.end(), ShapeRec_Result::SortByMethod44descending);

	runnerup = results.begin();
	runnerup++;
	if(runnerup != results.end())
		relative_confidence = sqrt((runnerup->metric_method44 - results.begin()->metric_method44) / runnerup->metric_method44);
	else
		//there was only one (reasonable) shape
		relative_confidence = max_relative_confidence_threshold + 0.001;
		//relative_confidence = ((max_relative_confidence_threshold + min_relative_confidence_threshold) * 0.5);
	
	absolute_confidence = SHAPEREC_METHOD4_ABSOLUTE_CONFIDENCE_FORMULA(results.begin()->metric_method44);


	if(print_all_results && PRINTHERE != nullptr)
	{
		(*PRINTHERE) << "--------------------" << std::endl;
		(*PRINTHERE) << "best guess of method 4 is a " << results.begin()->reference_shape_name << "    \t, angle:" << results.begin()->relative_angle_to_reference << std::endl;
		(*PRINTHERE) << "\trelative confidence: " << relative_confidence << "   \tabsolute confidence: " << absolute_confidence << std::endl;
		(*PRINTHERE) << "--------------------" << std::endl;
	}

	if((absolute_confidence >= max_absolute_confidence_threshold)
	|| (absolute_confidence >= min_absolute_confidence_threshold && relative_confidence >= max_relative_confidence_threshold))
	{
		if(print_winner_results && print_all_results==false && PRINTHERE != nullptr)
		{
            (*PRINTHERE) << "best guess of method 4 is a " << results.begin()->reference_shape_name << "    \t angle:" << results.begin()->relative_angle_to_reference << std::endl;
            (*PRINTHERE) << "\trelative confidence: " << relative_confidence << "   \tabsolute confidence: " << absolute_confidence << std::endl;
            (*PRINTHERE) << "--------------------" << std::endl;
		}
		returned_shape_guesses.push_back(*results.begin());


		//if the first shape has a high relative confidence, we won't care what the second guess's absolute confidence is,
		//we'll assume the first guess is a good guess

		if(relative_confidence < min_relative_confidence_threshold && runnerup != results.end())
		{
			runnerup22 = runnerup;
			runnerup22++;

			if(runnerup22 != results.end())
				relative_confidence = sqrt((runnerup22->metric_method44 - runnerup->metric_method44) / runnerup22->metric_method44);

			absolute_confidence = SHAPEREC_METHOD4_ABSOLUTE_CONFIDENCE_FORMULA(runnerup->metric_method44);

			if((absolute_confidence >= max_absolute_confidence_threshold)
			|| (absolute_confidence >= min_absolute_confidence_threshold && relative_confidence >= max_relative_confidence_threshold))
			{
				if(print_winner_results && PRINTHERE != nullptr)
				{
					(*PRINTHERE) << "a SECOND guess of method 4 is a " << runnerup->reference_shape_name << "    \t angle:" << results.begin()->relative_angle_to_reference << std::endl;
					(*PRINTHERE) << "\trelative confidence (to #3): " << relative_confidence << "   \tabsolute confidence: " << absolute_confidence << std::endl;
					(*PRINTHERE) << "--------------------" << std::endl;
				}

				returned_shape_guesses.push_back(*runnerup);
			}
		}
	}

	return returned_shape_guesses;
}



void ShapeRec_ResultsContainer::EliminateDuplicates()
{
    std::map<std::string, shape_duplicate_eliminator>           shaperec_shapes_map;
    std::map<std::string, shape_duplicate_eliminator>::iterator shaperec_shapes_iter;


    std::vector<ShapeRec_Result>::iterator shape_iter  = results.begin();

//-----------------------------

    for(; shape_iter != results.end(); shape_iter++)
    {
        shaperec_shapes_iter = shaperec_shapes_map.find(shape_iter->reference_shape_name);

        if(shaperec_shapes_iter == shaperec_shapes_map.end()) //not a duplicate
        {
            shaperec_shapes_map[shape_iter->reference_shape_name] = shape_duplicate_eliminator();
        }

        shaperec_shapes_map[shape_iter->reference_shape_name].metric_method44_total_multiplies_of_1_minus_confidence *= (1.0 - SHAPEREC_METHOD4_ABSOLUTE_CONFIDENCE_FORMULA(shape_iter->metric_method44));
        shaperec_shapes_map[shape_iter->reference_shape_name].num_instances++;
        shaperec_shapes_map[shape_iter->reference_shape_name].total_metric_method44_sum += shape_iter->metric_method44;
        shaperec_shapes_map[shape_iter->reference_shape_name].angles_to_reference.push_back(shape_iter->relative_angle_to_reference);
    }

    results.clear();

    shaperec_shapes_iter = shaperec_shapes_map.begin();
    for(; shaperec_shapes_iter != shaperec_shapes_map.end(); shaperec_shapes_iter++)
    {
        results.push_back(ShapeRec_Result(
        shaperec_shapes_iter->first,
        GetMeanAngle(shaperec_shapes_iter->second.angles_to_reference),
        SHAPEREC_METHOD4_ABSOLUTE_CONFIDENCE_FORMULA_INVERSE(1.0 - shaperec_shapes_iter->second.metric_method44_total_multiplies_of_1_minus_confidence)
        ));

#if SHAPEREC_SORT_PRIMARILY_BY_MULTIPLIED_CONFIDENCES
//#pragma message("ShapeRec compiled to sort primarily by MULTIPLIED confidences!")

        (results.rbegin())->metric_method44
            = SHAPEREC_METHOD4_ABSOLUTE_CONFIDENCE_FORMULA_INVERSE(1.0 - shaperec_shapes_iter->second.metric_method44_total_multiplies_of_1_minus_confidence);

        (results.rbegin())->multi_seg_image_processed_averaged_metric_method44
            = (shaperec_shapes_iter->second.total_metric_method44_sum / static_cast<double>(shaperec_shapes_iter->second.num_instances));
#else
#pragma message("ShapeRec compiled to sort primarily by AVERAGED confidences! This is probably worse than multiplied confidences...")

        (results.rbegin())->metric_method44
            = (shaperec_shapes_iter->second.total_metric_method44_sum / static_cast<double>(shaperec_shapes_iter->second.num_instances));

        (results.rbegin())->multi_seg_image_processed_averaged_metric_method44
            = SHAPEREC_METHOD4_ABSOLUTE_CONFIDENCE_FORMULA_INVERSE(1.0 - shaperec_shapes_iter->second.metric_method44_total_multiplies_of_1_minus_confidence);
#endif
    }

    SortAllByMethod44();
}
