#ifndef ____SHAPEREC_MODULE_TEMPLATE_H____
#define ____SHAPEREC_MODULE_TEMPLATE_H____

#include "Shaperec_Utils_Results.hpp"
#include <opencv/cv.h>
#include <opencv/highgui.h>



class ShapeRecModuleAlgorithm_SingleImage_Template
{
public:
    ShapeRec_ResultsContainer last_obtained_results;

//-------------
//settings:
    double max_absolute_confidence_threshold; //if abs conf is higher than max, will always be considered
    double min_absolute_confidence_threshold; //if abs conf is higher than min AND relative conf is higher than max, will be considered
    double max_relative_confidence_threshold;
    double min_relative_confidence_threshold; //if shape is being considered, and relative conf is less than this, then report the top 2 results
//-------------
    std::string filefolder_containing_reference_shapes;



	//returns a vector of possible names of the shape (in last_obtained_results)
	//empty if it doesn't think the passed image contains a real shape
	//1 name if it finds one good possibility
	//2 names if it finds two good, close possibilities
	virtual bool RecognizeShape(cv::Mat target_binary_image, bool show_stuff_while_working) = 0;


	ShapeRecModuleAlgorithm_SingleImage_Template() :

		max_absolute_confidence_threshold(0.30),    //best when approxPolyDP is used with 0.001 == polynomialapprox_deviance_fraction_of_perimeter, which is noisier (i.e. lower typical thresholds)
		min_absolute_confidence_threshold(0.16),
		max_relative_confidence_threshold(0.2),
		min_relative_confidence_threshold(0.15)

/*
		max_absolute_confidence_threshold(0.50),    //used in 2014; best when approxPolyDP is used with 0.01 == polynomialapprox_deviance_fraction_of_perimeter
		min_absolute_confidence_threshold(0.45),
		max_relative_confidence_threshold(0.50),
		min_relative_confidence_threshold(0.25)
*/
	{}
	virtual ~ShapeRecModuleAlgorithm_SingleImage_Template() {}
};



#endif
