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
    double max_absolute_confidence_threshold;
    double min_absolute_confidence_threshold;
    double max_relative_confidence_threshold;
    double min_relative_confidence_threshold;
//-------------
    std::string filefolder_containing_reference_shapes;



	//returns a vector of possible names of the shape (in last_obtained_results)
	//empty if it doesn't think the passed image contains a real shape
	//1 name if it finds one good possibility
	//2 names if it finds two good, close possibilities
	virtual bool RecognizeShape(cv::Mat target_binary_image, bool show_stuff_while_working) = 0;


	ShapeRecModuleAlgorithm_SingleImage_Template() :
	/*
		max_absolute_confidence_threshold(0.60),    //these settings are best used with averaged-confidence shaperec
		min_absolute_confidence_threshold(0.54),
		max_relative_confidence_threshold(0.50),
		min_relative_confidence_threshold(0.25)
    */
		max_absolute_confidence_threshold(0.50),    //these settings are best used with averaged-confidence shaperec
		min_absolute_confidence_threshold(0.45),
		max_relative_confidence_threshold(0.50),
		min_relative_confidence_threshold(0.25)
    /*
		max_absolute_confidence_threshold(0.75),    //these can be used with (1-conf) multiplied-confidence shaperec
		min_absolute_confidence_threshold(0.75),
		max_relative_confidence_threshold(0.35),
		min_relative_confidence_threshold(0.35)
    */
	{}
	virtual ~ShapeRecModuleAlgorithm_SingleImage_Template() {}
};



#endif
