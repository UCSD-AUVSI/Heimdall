#include "Segmentation_CSEG_SSEG_Merger.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/SharedUtils_OpenCV.hpp"



/*static*/ void Segmentation_CSEG_SSEG_Merger::DoModule(//cv::Mat cropped_target_image,
        std::vector<cv::Mat>* input_SSEGs,
        std::vector<cv::Scalar>* input_sseg_colors,
        std::vector<cv::Mat>* input_CSEGs,
        std::vector<cv::Scalar>* input_cseg_colors,
		float HistSeg_CROP_RESIZE_AMOUNT,
        std::string* folder_path_of_output_saved_images/*=nullptr*/,
		bool save_images_and_results/*=false*/,
		std::string* name_of_crop/*=nullptr*/
		)
{
    if(  (input_SSEGs != nullptr && input_SSEGs->empty()==false && input_sseg_colors != nullptr)
    &&   (input_CSEGs != nullptr && input_CSEGs->empty()==false && input_cseg_colors != nullptr)  )
    {
        if(input_SSEGs->size() > 1)
            consoleOutput.Level0() << std::endl << std::endl << "why are you still producing more than one SSEG?" << std::endl << std::endl << std::endl;
        if(input_CSEGs->size() > 1)
            consoleOutput.Level0() << std::endl << std::endl << "why are you still producing more than one CSEG?" << std::endl << std::endl << std::endl;

        if(input_SSEGs->begin()->type() != input_CSEGs->begin()->type()
        || input_SSEGs->begin()->type() != CV_8U)
        {
            consoleOutput.Level0() << std::endl << std::endl << "at this point, both the SSEG and the CSEG should be in single-channel, 8-bit unsigned char format!" << std::endl << std::endl << std::endl;
            return;
        }



        //overlay the CSEG onto the SSEG
        cv::max(*input_SSEGs->begin(), *input_CSEGs->begin(), *input_SSEGs->begin());


        cv::Mat try_dilating_this;
        input_SSEGs->begin()->copyTo(try_dilating_this);


        //cv::imshow("before dilation", try_dilating_this);


        int dilation_size = 6;
        cv::Mat element_dilation = cv::getStructuringElement(cv::MORPH_CROSS,
                                cv::Size(2*dilation_size + 1, 2*dilation_size + 1),
                                cv::Point(dilation_size, dilation_size) );
        cv::dilate(try_dilating_this, try_dilating_this, element_dilation);


        //cv::imshow("after dilation", try_dilating_this);

		
		//fill in interior holes in the shape
		try_dilating_this = FillInteriorsOfBlob(try_dilating_this, 255);
		



        //cv::imshow("filled in specks", try_dilating_this);



        //now we need to erode it back to the size of the original shape
        //dilation and erosion aren't 100% reversible, even on the outer perimeter where there shouldn't be any filling in,
        //but this seems pretty nearly reversible (dilating-and-eroding a shape without a CSEG doesn't change very much)

        int erosion_size = 5;
        cv::Mat element_erosion = cv::getStructuringElement(cv::MORPH_CROSS,
                                cv::Size(2*erosion_size + 1, 2*erosion_size + 1),
                                cv::Point(erosion_size, erosion_size) );
        cv::erode(try_dilating_this, try_dilating_this, element_erosion);




        //cv::imshow("dilated back to where it was", try_dilating_this);
		//cv::waitKey(0);
		//cv::destroyAllWindows();


		//save it
        try_dilating_this.copyTo(*input_SSEGs->begin());
		
#if 0
			cv::imshow("11 seg almost done", try_dilating_this);
			cv::waitKey(0); cv::destroyAllWindows();
#endif
		
		//double check that the CSEGs and SSEGs are in 0-255 format, with no other values in between
		if(input_SSEGs->empty() == false) {
			cv::threshold(*input_SSEGs->begin(), *input_SSEGs->begin(), 127, 255, CV_THRESH_BINARY);
		}
		if(input_CSEGs->empty() == false) {
			cv::threshold(*input_CSEGs->begin(), *input_CSEGs->begin(), 127, 255, CV_THRESH_BINARY);
		}
        
#if 0
			cv::imshow("22 seg almost done", try_dilating_this);
			cv::waitKey(0); cv::destroyAllWindows();
#endif
        
//========================================================================================
//========================================================================================
#if 0
	if(save_images_and_results && folder_path_of_output_saved_images != nullptr && name_of_crop != nullptr)
	{
		if(input_SSEGs->empty()) {
			consoleOutput.Level2() << "no SSEG for the merger module!" << std::endl;
		} else {
	saveImage(*input_SSEGs->begin(),
		*folder_path_of_output_saved_images + "/" + (*name_of_crop) + std::string("_SSEG") + std::string(".jpg"));
		}
		
		if(input_CSEGs->empty()) {
			consoleOutput.Level2() << "no CSEG for the merger module!" << std::endl;
		} else {
	saveImage(*input_CSEGs->begin(),
		*folder_path_of_output_saved_images + "/" + (*name_of_crop) + std::string("_CSEG") + std::string(".jpg"));
		}
	}
	else {
		std::cout << "NO DICE ###############################################################################################################################################################" << std::endl;
	}
#endif
//========================================================================================
//========================================================================================
        //now check to see if the SSEG is valid (if it's mostly 1 contiguous shape)

		
		
        std::vector<std::vector<cv::Point>> FoundContours_SSEG;
        //findContours destroys the input image
        input_SSEGs->begin()->copyTo(try_dilating_this);
        cv::findContours(try_dilating_this, FoundContours_SSEG, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0,0));
        if(FoundContours_SSEG.empty()==false)
        {
            double total_area_of_all_contours = 0.0;
            double area_of_largest_contour = 0.0;

            int largest_contour_indx = GetContourOfGreatestArea(FoundContours_SSEG, &area_of_largest_contour, &total_area_of_all_contours);


            if((area_of_largest_contour / total_area_of_all_contours) < 0.95)
            {
                consoleOutput.Level3() << std::string("percent area of largest SSEG: ")
					<< to_sstring(area_of_largest_contour / total_area_of_all_contours) << std::endl;

                consoleOutput.Level1() << std::endl << std::string("the merger module decided to toss an ugly looking SSEG!") << std::endl;
                //consoleOutput.Level3() << std::string("################################################################################") << std::endl;
                consoleOutput.Level4() << std::string("################################################################################") << std::endl;
				
                input_SSEGs->clear();
                input_CSEGs->clear();
            }
            else
            {
                consoleOutput.Level2() << std::endl << std::string("percent area of largest SSEG (saved to final SSEG): ")
					<< to_sstring(area_of_largest_contour / total_area_of_all_contours) << std::endl << std::endl;
				
				try_dilating_this.setTo(0);
				cv::drawContours(try_dilating_this, FoundContours_SSEG, largest_contour_indx, 255, CV_FILLED);
				try_dilating_this.copyTo(*input_SSEGs->begin());
			}
			
			
//==================================================================
#if 0
			try_dilating_this.setTo(0);
			cv::drawContours(try_dilating_this, FoundContours_SSEG, -1, 120, 1);
			cv::drawContours(try_dilating_this, FoundContours_SSEG, largest_contour_indx, 255, 1);
			try_dilating_this.copyTo(*input_SSEGs->begin());
			cv::imshow("33 seg almost done", try_dilating_this);
			cv::waitKey(0); cv::destroyAllWindows();
#endif
//==================================================================


//this may need to be left out (here line #181)
#if 1
			//=================================================================================
			//next: check if the CSEG is a reasonable size compared to the SSEG
			
			if(input_SSEGs->empty() == false && input_CSEGs->empty() == false)
			{
				int area_of_CSEG = cv::countNonZero(*input_CSEGs->begin());
				int area_of_SSEG = cv::countNonZero(*input_SSEGs->begin());
				float fractional_area_of_CSEG_compared_to_SSEG = ((float)(area_of_CSEG)) / ((float)(area_of_SSEG));
				consoleOutput.Level2() << std::endl << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ fractional area of CSEG (calculation without contours): " << to_sstring(fractional_area_of_CSEG_compared_to_SSEG) << std::endl << std::endl;
				
				if(fractional_area_of_CSEG_compared_to_SSEG < 0.0f || fractional_area_of_CSEG_compared_to_SSEG > 0.4f)
				{
					consoleOutput.Level1() << std::endl << std::string("the merger module tossed the segs because the CSEG was either too small or too large compared to the SSEG!") << std::endl;
					input_SSEGs->clear();
					input_CSEGs->clear();
				}
			}
#endif

//this may need to be left out (here line #202)
#if 1
			//=================================================================================
			//next: see how much the CSEG and SSEG intersect, compared to the area of the CSEG;
			//				ideally the CSEG would be 100% inside the SSEG
			
			if(input_SSEGs->empty() == false && input_CSEGs->empty() == false)
			{
				std::vector<std::vector<cv::Point>> FoundContours_CSEG;
				//findContours destroys the input image
				input_CSEGs->begin()->copyTo(try_dilating_this);
				cv::findContours(try_dilating_this, FoundContours_CSEG, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0,0));
				if(FoundContours_CSEG.empty() == false)
				{
					int area_of_CSEG_filled = 0;
					int area_of_intersection = GetAreaIntersectionOf_CSEG_in_the_bounding_convex_polygon_of_SSEG(try_dilating_this,
													FoundContours_SSEG[largest_contour_indx],
													FoundContours_CSEG,
													area_of_CSEG_filled);
					
					float fraction_of_CSEG_NOT_in_the_SSEG = 1.0f - (((float)(area_of_intersection)) / ((float)(area_of_CSEG_filled)));
					
					consoleOutput.Level2() << "area of CSEG (calculation AFTER filling contours): " << to_istring(area_of_CSEG_filled) << std::endl;
					consoleOutput.Level2() << "fraction of the CSEG that is NOT in the convex hull of the SSEG: " << to_sstring(fraction_of_CSEG_NOT_in_the_SSEG) << std::endl;
					
					if(fraction_of_CSEG_NOT_in_the_SSEG > 0.15f) {
						consoleOutput.Level1() << std::endl << std::string("the merger module tossed the segs because the CSEG was too much outside of the SSEG!") << std::endl;
						input_SSEGs->clear();
						input_CSEGs->clear();
					}
				}
				else
				{
					consoleOutput.Level1() << std::endl << std::string("the merger module tossed the segs because the CSEG didn't have any contours!") << std::endl;
					input_SSEGs->clear();
					input_CSEGs->clear();
				}
			}
#endif
        }
    }
	
    if(input_CSEGs != nullptr && input_SSEGs != nullptr)
    {
        if((input_CSEGs->empty() || cv::countNonZero(*input_CSEGs->begin())==0) && input_SSEGs->empty()==false)
        {
            consoleOutput.Level1() << std::endl << std::string("the merger module decided to toss the SSEG because there was no CSEG!") << std::endl;
            //consoleOutput.Level3() << std::string("################################################################################") << std::endl;
            consoleOutput.Level4() << std::string("################################################################################") << std::endl;

            input_SSEGs->clear();
            input_CSEGs->clear();
        }
    }
}
