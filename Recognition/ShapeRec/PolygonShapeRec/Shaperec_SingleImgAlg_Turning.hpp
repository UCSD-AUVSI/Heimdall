#ifndef ____SHAPEREC_MODULE_TURNING_H____
#define ____SHAPEREC_MODULE_TURNING_H____

#include "Shaperec_Utils_Results.hpp"
#include "Shaperec_SingleImgAlg_template.hpp"
#include <opencv/cv.h>
#include <opencv/highgui.h>



class ShapeRecModuleAlgorithm_SingleImage_Turning : public ShapeRecModuleAlgorithm_SingleImage_Template
{
public:
	double polynomialapprox_deviance_fraction_of_perimeter;



	ShapeRecModuleAlgorithm_SingleImage_Turning();

	//returns a vector of possible names of the shape (in last_obtained_results)
	//empty if it doesn't think the passed image contains a real shape
	//1 name if it finds one good possibility
	//2 names if it finds two good, close possibilities
	virtual bool RecognizeShape(cv::Mat target_binary_image, bool show_stuff_while_working=false);

protected:
	static void DrawPolynomialOverContour(cv::Mat & img_to_draw_into, std::vector<cv::Point> & contour, double perimeter_deviance_fraction);

	ShapeRec_Result CompareReferenceMatToTargetContour(cv::Mat & refr_mat, std::vector<cv::Point> & target_contour, std::vector<cv::Point> & target_polynomial_contour);

	void CompareTargetContourToAllReferenceShapes(std::vector<ShapeRec_Result> & returned_results, std::vector<cv::Point> & target_contour, std::vector<cv::Point> & target_polynomial_contour, const std::string& folder_containing_reference_images);

};



#endif
