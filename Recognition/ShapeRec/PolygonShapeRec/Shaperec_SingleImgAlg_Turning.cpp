/**
 * @file Shaperec_SingleImgAlg_Turning.cpp
 * @brief A shape recognition single-input "mini-module" that uses the algorithm in Shaperec_Utils_TurningAlgorithm.
 *        This may return anywhere from 0 to 2 shapes. It should be used within a main shaperec "black box", to be run several times (for several SSEG's) within that black box.
 * @author Jason Bunk
 */

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <string>
#include <fstream>
#include "Shaperec_Utils_TurningAlgorithm.hpp"
#include "Shaperec_SingleImgAlg_Turning.hpp"
#include "Shaperec_Utils_Results.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/SharedUtils_OpenCV.hpp"
#include "SharedUtils/OS_FolderBrowser_tinydir.h"



ShapeRecModuleAlgorithm_SingleImage_Turning::ShapeRecModuleAlgorithm_SingleImage_Turning()
	: polynomialapprox_deviance_fraction_of_perimeter(0.01)
{}



void ShapeRecModuleAlgorithm_SingleImage_Turning::DrawPolynomialOverContour(cv::Mat & img_to_draw_into, std::vector<cv::Point> & contour, double perimeter_deviance_fraction)
{
	//approximate the contour by a polynomial
	std::vector<cv::Point> generated_polynomial;
	cv::approxPolyDP(contour, generated_polynomial, cv::arcLength(contour, true)*perimeter_deviance_fraction, true);

	//draw the polynomial
	int lastpt = (generated_polynomial.size() - 1);
	for(int bb=0; bb<lastpt; bb++)
	{
		cv::line(img_to_draw_into, generated_polynomial[bb], generated_polynomial[bb+1], cv::Scalar(0,100,0));
	}
	cv::line(img_to_draw_into, generated_polynomial[lastpt], generated_polynomial[0], cv::Scalar(0,100,0));
}




ShapeRec_Result ShapeRecModuleAlgorithm_SingleImage_Turning::CompareReferenceMatToTargetContour(cv::Mat & refr_mat,
                                                                                    std::vector<cv::Point> & target_contour,
                                                                                    std::vector<cv::Point> & target_polynomial_contour)
{
	//find shape outlines (external outlines of shapes, not internal features like letters inside shapes)
	std::vector<std::vector<cv::Point>> RefrContours;
	cv::findContours(refr_mat, RefrContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0,0));
												//should this be CV_CHAIN_APPROX_NONE ?

	//we'll assume that there was only one contour found in the reference shape's image
	//if the reference shape images are nice and clean, this will be true

#if CONSIDER_CV_SHAPEMATCH_ALGORITHMS
	//compare the reference and target contour
	double match_method1 = cv::matchShapes(RefrContours[0], target_contour, CV_CONTOURS_MATCH_I1, 0);
	double match_method2 = cv::matchShapes(RefrContours[0], target_contour, CV_CONTOURS_MATCH_I2, 0);
	double match_method3 = cv::matchShapes(RefrContours[0], target_contour, CV_CONTOURS_MATCH_I3, 0);
#endif

//----------
	//approximate the reference contour by a polynomial
	std::vector<cv::Point> refr_polynomial_contour;
	cv::approxPolyDP(RefrContours[0], refr_polynomial_contour, cv::arcLength(RefrContours[0], true)*polynomialapprox_deviance_fraction_of_perimeter, true);

	//for the new algorithm, polygons MUST be counterclockwise (doesn't affect OpenCV algorithms)
	//by default OpenCV generates them clockwise
	std::reverse(refr_polynomial_contour.begin(), refr_polynomial_contour.end());

	ComparisonReturnedTable alt_method_table(CompareCVPolygons(refr_polynomial_contour, target_polynomial_contour));
	double metric_method44 = alt_method_table.metric;
//----------

#if CONSIDER_CV_SHAPEMATCH_ALGORITHMS
	//the 0 returned is an unknown shape, you'll need to fill it in with the shape you called this function with!
	return ShapeRec_Result("", match_method1, match_method2, match_method3, metric_method44);
#else
	return ShapeRec_Result("", alt_method_table.relative_angle_to_reference, metric_method44);
#endif
}



void ShapeRecModuleAlgorithm_SingleImage_Turning::CompareTargetContourToAllReferenceShapes(std::vector<ShapeRec_Result> & returned_results,
                                                                            std::vector<cv::Point> & target_contour,
                                                                            std::vector<cv::Point> & target_polynomial_contour,
                                                                            const std::string& folder_containing_reference_images)
{
	returned_results.clear();

	if(folder_containing_reference_images.empty())
	{
		consoleOutput.Level0() << "ShapeRecognizer - warning - was not given a folder containing reference images!!" << std::endl;
	}


	bool found_at_least_one = false;
	std::string ref_filename;
	std::ifstream myfile;


    tinydir_dir dir;
    std::string filename;
    std::string filename_full_path;
    std::string filename_extension;

	//-----------------------------------------------------------------
	// open reference shape files from disk (and compare each one to the shape)
	// they shouldn't be loaded from disk every single time shape recognition is done;
	// the contours from reference shapes should be saved in memory when the program first starts up
	//-----------------------------------------------------------------
	
    tinydir_open(&dir, folder_containing_reference_images.c_str());
	
    while(dir.has_next)
    {
        tinydir_file file;
        tinydir_readfile(&dir, &file);


        if(file.is_dir == false)
        {
            filename = std::string(file.name);

            if(filename.size() > 4)
            {
                filename_extension = eliminate_extension_from_filename(filename);
                //now "filename" has neither the extension nor the folder path

                if(filename_extension_is_image_type(filename_extension))
                {
                    filename_full_path = std::string(file.path);

                    cv::Mat ref_mat = cv::imread(filename_full_path, CV_LOAD_IMAGE_GRAYSCALE);

                    if(ref_mat.empty() == false)
                    {
                        found_at_least_one = true;

                        cv::threshold(ref_mat, ref_mat, 128, 255, cv::THRESH_BINARY);

                        returned_results.push_back(CompareReferenceMatToTargetContour(ref_mat, target_contour, target_polynomial_contour));
                        returned_results.rbegin()->reference_shape_name = filename;
                    }
                }
            }
        }
        tinydir_next(&dir);
    }

    tinydir_close(&dir);

#if 0
	for(int aaa=0; aaa < ShapeRecognizer::ShapeNamesTable.size(); aaa++)
	{
		ref_filename = (folder_containing_reference_images + std::string("/") + to_istring(aaa) + std::string(".jpg"));

		myfile.open(ref_filename, std::ifstream::in);
		if(myfile.is_open()) //just check if the file exists
		{
			myfile.close();
			found_at_least_one = true;

			cv::Mat ref_mat = cv::imread(ref_filename, 0);
			cv::threshold(ref_mat, ref_mat, 128, 255, cv::THRESH_BINARY);

			returned_results.push_back(CompareReferenceMatToTargetContour(ref_mat, target_contour, target_polynomial_contour));
			returned_results.rbegin()->reference_shape = aaa;
		}
	}
#endif

	if(found_at_least_one == false)
		consoleOutput.Level0() << "\nDid not find any reference shape images!!" << std::endl;
}


bool ShapeRecModuleAlgorithm_SingleImage_Turning::RecognizeShape(cv::Mat target_binary_image,
                                                    bool show_stuff_while_working/*=false*/)
{
	last_obtained_results.clear();


	cv::Mat mat_for_display;
	target_binary_image.copyTo(mat_for_display);
	cv::threshold(mat_for_display, mat_for_display, 128, 255, CV_THRESH_BINARY);

	//find contours in target image
	std::vector<std::vector<cv::Point>> TargContours;
	cv::findContours(mat_for_display, TargContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0,0));

    if(TargContours.empty())
        return false;


	if(show_stuff_while_working)
	{
        consoleOutput.Level3() << "found " << TargContours.size() << " contours in the SSEG" << std::endl;

		cv::cvtColor(mat_for_display, mat_for_display, CV_GRAY2BGR);
		mat_for_display = cv::Scalar(0);
    }


	//assume the largest contour is the desired shape
	double area_of_contour = 0.0;
	int whichshape_in_target = GetContourOfGreatestArea(TargContours, &area_of_contour);
	double area_of_whole_image = static_cast<double>(mat_for_display.rows * mat_for_display.cols);

	//if(show_stuff_while_working)
    //    consoleOutput.Level3() << "area of contour: " << area_of_contour << "    \tarea of whole image: " << area_of_whole_image << std::endl;




	//approximate the target contour by a polynomial
	std::vector<cv::Point> target_polynomial_contour;
	cv::approxPolyDP(TargContours[whichshape_in_target], target_polynomial_contour, cv::arcLength(TargContours[whichshape_in_target], true)*polynomialapprox_deviance_fraction_of_perimeter, true);

	//for the new algorithm, polygons MUST be counterclockwise (doesn't affect OpenCV algorithms)
	//by default OpenCV generates them clockwise
	std::reverse(target_polynomial_contour.begin(), target_polynomial_contour.end());



	if(show_stuff_while_working)
	{
		//display all contours, and highlight the largest found contour in red
		cv::drawContours(mat_for_display, std::vector<std::vector<cv::Point>>(1, target_polynomial_contour), -1, cv::Scalar(0,255,0));
		cv::drawContours(mat_for_display, TargContours, -1, cv::Scalar(255,255,255));
		cv::drawContours(mat_for_display, TargContours, whichshape_in_target, cv::Scalar(0,0,255));

		cv::imshow("ALL contours found in target", mat_for_display);
		cv::waitKey(0);
		cv::destroyAllWindows();
	}


	//check to make sure the shape found is realistically sized (greater than ~5% of the image and less than ~95% of the image)
	double relative_area = (area_of_contour / area_of_whole_image);
	if(relative_area >= 0.94)
	{
		//this happens if the binary threshold wasn't good (it made most of the image white)

		consoleOutput.Level1() << "shaperec: no shape found, contour in the SSEG is pretty much the outline of the whole image" << std::endl;

		if(show_stuff_while_working)
			CV_WAIT_KEY_OPTIONAL;

		return false;
	}
	else if(relative_area <= 0.01)
	{
		//this happens if the binary threshold wasn't good (it made most of the image black),
		//or there are no clear large-scale shapes?

		consoleOutput.Level1() << "shaperec: no shape found, contour in the SSEG is too small to be considered an interesting shape" << std::endl;

		if(show_stuff_while_working)
			CV_WAIT_KEY_OPTIONAL;

		return false;
	}


	//**** another possible way to get rid of junk images with no shapes?
	//     if there are a bunch of similarly sized shapes, like patches of dirt, ignore them?




	//compare target shape's contour to all reference shapes
	ShapeRec_ResultsContainer results_container;
	CompareTargetContourToAllReferenceShapes(results_container.results,
										TargContours[whichshape_in_target],
										target_polynomial_contour,
										filefolder_containing_reference_shapes);


	last_obtained_results = results_container.GetTopResults(
													max_absolute_confidence_threshold,
													min_absolute_confidence_threshold,
													max_relative_confidence_threshold,
													min_relative_confidence_threshold,
													show_stuff_while_working ? (&consoleOutput.Level3()) : nullptr,
													show_stuff_while_working,
													show_stuff_while_working);

	//CV_WAIT_KEY_OPTIONAL;
	return (last_obtained_results.empty() == false);
}
