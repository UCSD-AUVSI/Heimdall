#include "Segmentation_CSEG_SSEG_Merger.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/SharedUtils_OpenCV.hpp"



/*static*/ void Segmentation_CSEG_SSEG_Merger::DoModule(//cv::Mat cropped_target_image,
        std::vector<cv::Mat>* input_SSEGs,
        std::vector<cv::Scalar>* input_sseg_colors,
        std::vector<cv::Mat>* input_CSEGs,
        std::vector<cv::Scalar>* input_cseg_colors,
		float HistSeg_CROP_RESIZE_AMOUNT)
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


        //cv::waitKey(0);

//========================================================================================
//========================================================================================
        //now check to see if the SSEG is valid (if it's mostly 1 contiguous shape)


        std::vector<std::vector<cv::Point>> FoundContours;

        cv::findContours(try_dilating_this, FoundContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0,0));
        if(FoundContours.empty()==false)
        {
            double total_area_of_all_contours = 0.0;
            double area_of_largest_contour = 0.0;

            int largest_contour_indx = GetContourOfGreatestArea(FoundContours, &area_of_largest_contour, &total_area_of_all_contours);



            if((area_of_largest_contour / total_area_of_all_contours) < 0.85)
            {
                consoleOutput.Level3() << std::string("percent area of largest SSEG: ")
                << to_sstring(area_of_largest_contour / total_area_of_all_contours) << std::endl;

                consoleOutput.Level1() << std::endl << std::string("the merger module decided to toss an ugly looking SSEG!") << std::endl;
                consoleOutput.Level3() << std::string("################################################################################") << std::endl;
                consoleOutput.Level4() << std::string("################################################################################") << std::endl;

                input_SSEGs->clear();
                input_CSEGs->clear();
            }
            else
                consoleOutput.Level3() << std::endl << std::string("percent area of largest SSEG: ")
                << to_sstring(area_of_largest_contour / total_area_of_all_contours) << std::endl << std::endl;
        }
    }

    if(input_CSEGs != nullptr && input_SSEGs != nullptr)
    {
        if(input_CSEGs->empty() && input_SSEGs->empty()==false)
        {
            consoleOutput.Level1() << std::endl << std::string("the merger module decided to toss the SSEG because there was no CSEG!") << std::endl;
            consoleOutput.Level3() << std::string("################################################################################") << std::endl;
            consoleOutput.Level3() << std::string("################################################################################") << std::endl;
            consoleOutput.Level4() << std::string("################################################################################") << std::endl;

            input_SSEGs->clear();
            input_CSEGs->clear();
        }
    }
}
