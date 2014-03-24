
#include "skynet_Segmenter_SingleImageReturn.hpp"
#include "skynet_SegmenterTemplate.hpp"
#include "skynet_HistogramSegmenter.hpp"
#include "skynet_VisionUtil.hpp"
#include "skynet_ColorBlob.hpp"
#include <algorithm> //circularity sorter
#include "SharedUtils/SharedUtils_OpenCV.hpp"
#include "SharedUtils/SharedUtils.hpp"


using namespace Skynet;


Segmenter_SingleImageReturn::Segmenter_SingleImageReturn(void)
{
	segmenter = new HistogramSegmenter();
}


cv::Mat Segmenter_SingleImageReturn::findShape(cv::Mat colorImg,
                        const Segmenter_Module_Settings & attempt_settings,
                        cv::Scalar* input_color_of_previously_found_shapeblob,
                        cv::Scalar* returned_color_of_blob,
                        cv::Mat* mask_of_returned_shape_blob,
                        std::string name_of_process_calling_this_function,
                        cv::Mat* returned_mat_of_binned_histogram/*=nullptr*/,
                        cv::Mat* returned_mat_preprocessed/*=nullptr*/)
{
    if(colorImg.empty())
    {
        consoleOutput.Level0() << "WARNING: Segmenter_SingleImageReturn::findShape() given an empty cv::Mat (with zero pixels)!" << std::endl;
        return colorImg;
    }
    //consoleOutput.Level2() << std::string("---- beginning segmentation from: ") << name_of_process_calling_this_function << std::endl;

    cv::Mat converted_mat;
    ConvertMat_UsingSettings(colorImg, converted_mat,
                            attempt_settings.preprocess_CV_conversion_type,
                            attempt_settings.preprocess_channels_to_keep, true);

    //---------------------------------------------------------
    //Set global ("extern") variables used by Skynet's histogram segmenter.
    //
    Skynet::COLOR_DISTANCE_THRESHOLD = attempt_settings.HistSeg_COLOR_DISTANCE_THRESHOLD;
    Skynet::MERGE_COLOR_DISTANCE = attempt_settings.HistSeg_MERGE_COLOR_DISTANCE;
    Skynet::NUM_VALID_COLORS = attempt_settings.HistSeg_NUM_VALID_COLORS;
    Skynet::NUM_BINS = attempt_settings.HistSeg_NUM_BINS;
    Skynet::MINIMUM_BLOB_SIZE_THRESHOLD = attempt_settings.HistSeg_MINIMUM_BLOB_SIZE_THRESHOLD;
    //---------------------------------------------------------


    cv::blur(converted_mat, converted_mat, cv::Size(attempt_settings.HistSeg_BLUR_PREPROCESS_RADIUS_PIXELS,attempt_settings.HistSeg_BLUR_PREPROCESS_RADIUS_PIXELS));
    if(returned_mat_preprocessed != nullptr)
        converted_mat.copyTo(*returned_mat_preprocessed);


	std::vector<ColorBlob*> * blobList = segmenter->findBlobs(converted_mat, returned_mat_of_binned_histogram);


	ClearBlobsOfTinyNoiseSpeckles(blobList, 64); //any specks smaller than 8x8 pixels will be eliminated


	std::vector<ColorBlob*> * largeBlobs = getLargeBlobs(blobList, converted_mat.size());


	std::vector<ColorBlob*> * interiorBlobs = getInteriorBlobs(largeBlobs, attempt_settings.HistSeg_PERCENT_TOUCHING_EDGES_ACCEPTABLE);


    //0.1 seems to work well as a cutoff circularity
    //note that the returned results are sorted by circularity (descending; most circular first)
    //
    std::vector<ColorBlob*>* roundishBlobs = getRoundishBlobs_ByCircularity(interiorBlobs, 0.1f);
//consoleOutput.Level2() << std::string("num roundish blobs: ") << to_istring(roundishBlobs->size()) << std::endl;



    if(roundishBlobs->empty())
        return cv::Mat();


    //Almost done! Get ready for the last step, in case it needs to be done.
    ColorBlob* chosenShapeBlob = nullptr;
    cv::Mat chosenShapeBlob_Mask;


    //Last step: If we know a color we want to ignore, then eliminate blobs too close to that color
    //
    if(input_color_of_previously_found_shapeblob != nullptr)
    {
        std::vector<cv::Mat> all_blob_masks;
        std::vector<cv::Scalar> colors_of_blobs;

        for(int i=0; i<roundishBlobs->size(); i++)
        {
            all_blob_masks.push_back(getShapeFromBlob((*roundishBlobs)[i], converted_mat.size(), attempt_settings.HistSeg_FILL_IN_SHAPE_BLOB_BEFORE_RETURNING));

            //calculate each blob's color by averaging the colors in the blob's region in the
            //                  original RGB image (colorImg; before colorspace conversion)
            colors_of_blobs.push_back(cv::mean(colorImg, *all_blob_masks.rbegin()));
        }

        //set this for "eliminateBlobsTooCloseToInputColor"
        Skynet::COLOR_DISTANCE_THRESHOLD = attempt_settings.HistSeg_COLOR_DISTANCE_TO_PREVIOUSLYFOUND_THRESHOLD;

        std::vector<ColorBlob*>* final_acceptable_blobs =
            eliminateBlobsTooCloseToInputColor(roundishBlobs, &all_blob_masks, &colors_of_blobs, input_color_of_previously_found_shapeblob);


        if(final_acceptable_blobs->empty())
            return cv::Mat();

        chosenShapeBlob = *(final_acceptable_blobs->begin());
        chosenShapeBlob_Mask = *all_blob_masks.begin(); //this list was passed to "eliminateBobs..." because it trimmed this list too,
                                                    //so its beginning still corresponds to the beginning of "final_acceptable_blobs"
    }
    else
    {
        chosenShapeBlob = *(roundishBlobs->begin());
        chosenShapeBlob_Mask = getShapeFromBlob(*(roundishBlobs->begin()), converted_mat.size(), attempt_settings.HistSeg_FILL_IN_SHAPE_BLOB_BEFORE_RETURNING);

        (*returned_color_of_blob) = cv::mean(colorImg, chosenShapeBlob_Mask);
    }



//consoleOutput.Level3() << std::string("Num large blobs: ") << largeBlobs->size());
//consoleOutput.Level3() << std::string("Num interior blobs: ") << interiorBlobs->size());

consoleOutput.Level3() << std::string("==========================================================================") << std::endl;
consoleOutput.Level3() << std::string("================================= circularity of accepted shape: ") << to_sstring(chosenShapeBlob->CalculateCircularity()) << std::endl;
consoleOutput.Level3() << std::string("===============fraction of pixels that were touching an edge: ") << to_sstring(chosenShapeBlob->saved_last_fraction_of_blobs_pixels_along_border) << std::endl;




	if(mask_of_returned_shape_blob != nullptr)
		chosenShapeBlob->drawIntoFalseColorImg(*mask_of_returned_shape_blob, FalseColor(255));




	if(chosenShapeBlob_Mask.empty())
		consoleOutput.Level3() << "No shape found!";
#if 0
    else
	{
		//if(filename.empty() == false)
			saveImage(chosenShapeBlob_Mask, std::string("output_images/") + to_istring(rand()) + std::string("_segmented_shape.bmp"));
	}
#endif


	//if (filename != nullptr)
	//	saveBlobsToFalseColorImage(largeBlobs, std::string("blob_d_big"));
	//if (filename != nullptr)
	//	saveBlobsToFalseColorImage(interiorBlobs, std::string("blob_e_interior"));

	return chosenShapeBlob_Mask;
}


#define FOUR_PI_FOR_CIRC 12.566370614359173f

std::vector<ColorBlob*> *
Segmenter_SingleImageReturn::getRoundishBlobs_ByCircularity(std::vector<ColorBlob*> * blobList, float cutoff_circularity)
{
    std::vector<ColorBlob*> * circularBlobs = new std::vector<ColorBlob *>();
    float circularity = 0.0f;

    std::vector<ColorBlobCircularitySorter> circularity_sorter;

#if 0
int blobcounter = 0;
#endif

	for(std::vector<ColorBlob*>::iterator bliter = blobList->begin(); bliter != blobList->end(); bliter++)
	{
        circularity = (*bliter)->CalculateCircularity();

#if 0
blobcounter++;
float areaof = (*bliter)->area();
float perimiterof = (*bliter)->CalculateTotalPerimeters();
consoleOutput.Level3() << std::string("blob") << to_istring(blobcounter) << std::string(" area: ") << to_sstring(areaof) << std::endl;
consoleOutput.Level3() << std::string("blob") << to_istring(blobcounter) << std::string(" perimeter: ") << to_sstring(perimiterof) << std::endl;
consoleOutput.Level3() << std::string("blob") << to_istring(blobcounter) << std::string(" circularity: ") << to_sstring(circularity) << std::endl;
#endif

		if(circularity > cutoff_circularity)
		{
			circularity_sorter.push_back(ColorBlobCircularitySorter(circularity, *bliter));
        }
    }


    if(circularity_sorter.empty()==false)
    {
        std::sort(circularity_sorter.begin(), circularity_sorter.end(), ColorBlobCircularitySorter::SortDescending);

        for(std::vector<ColorBlobCircularitySorter>::iterator iter = circularity_sorter.begin(); iter != circularity_sorter.end(); iter++)
        {
            circularBlobs->push_back(iter->thisblob);
        }
    }

    return circularBlobs;
}


std::vector<ColorBlob *> *
Segmenter_SingleImageReturn::getLargeBlobs(std::vector<ColorBlob *> * blobList, cv::Size imgSize)
{
	float imgArea = (float)imgSize.width * imgSize.height;
	float minimumBlobSize = ((float)(imgArea))*MINIMUM_BLOB_SIZE_THRESHOLD;

	std::vector<ColorBlob*> * bigBlobs = new std::vector<ColorBlob *>();
	//for each (ColorBlob * blob in blobList)
	for(std::vector<ColorBlob*>::iterator bliter = blobList->begin(); bliter != blobList->end(); bliter++)
	{
		if((*bliter)->area() > minimumBlobSize)
			bigBlobs->push_back(*bliter);
    }
	return bigBlobs;
}


std::vector<ColorBlob*> *
Segmenter_SingleImageReturn::getInteriorBlobs(std::vector<ColorBlob *> * blobList, float acceptable_fraction_of_blobs_pixels_that_touch_edge)
{
	std::vector<ColorBlob*>* interiorBlobs = new std::vector<ColorBlob *>();
	//for each (ColorBlob * blob in blobList)
	for(std::vector<ColorBlob*>::iterator bliter = blobList->begin(); bliter != blobList->end(); bliter++)
	{
		if ((*bliter)->isInterior(acceptable_fraction_of_blobs_pixels_that_touch_edge))
			interiorBlobs->push_back(*bliter);
    }
	return interiorBlobs;
}


std::vector<ColorBlob*>*
Segmenter_SingleImageReturn::eliminateBlobsTooCloseToInputColor(std::vector<ColorBlob*>* blobList,
                                                                std::vector<cv::Mat>* masks_of_blobList,
                                                                std::vector<cv::Scalar>* colors_of_bloblists,
                                                                cv::Scalar* color_to_eliminate)
{
    //start by copying from blobList, then we'll literally eliminate them from the new list (instead of adding one-by-one to the new list)
    std::vector<ColorBlob*>* acceptable_blobs = new std::vector<ColorBlob*>(*blobList);


    std::vector<ColorBlob*>::iterator cbiter = acceptable_blobs->begin();
    std::vector<cv::Mat>::iterator miter = masks_of_blobList->begin();
    std::vector<cv::Scalar>::iterator sciter = colors_of_bloblists->begin();

    for(; cbiter != acceptable_blobs->end(); )
    {
        if(Skynet::calcColorDistance(*sciter, *color_to_eliminate) <= (0.99f * Skynet::COLOR_DISTANCE_THRESHOLD))
        {
            consoleOutput.Level3() << std::string("------------this blob's color was NOT accepted: ")
            << to_sstring((*sciter)[0]) << std::string(",")
            << to_sstring((*sciter)[1]) << std::string(",")
            << to_sstring((*sciter)[2]) << std::string(",")
            << to_sstring((*sciter)[3]) << std::string(",")
             << std::endl;

            cbiter = acceptable_blobs->erase(cbiter);
            miter = masks_of_blobList->erase(miter);
            sciter = colors_of_bloblists->erase(sciter);
        }
        else
        {
            consoleOutput.Level3() << std::string("------------color of an ACCEPTED blob: ")
            << to_sstring((*sciter)[0]) << std::string(",")
            << to_sstring((*sciter)[1]) << std::string(",")
            << to_sstring((*sciter)[2]) << std::string(",")
            << to_sstring((*sciter)[3]) << std::string(",")
             << std::endl;

            cbiter++;
            miter++;
            sciter++;
        }
    }

    return acceptable_blobs;
}


void Segmenter_SingleImageReturn::ClearBlobsOfTinyNoiseSpeckles(std::vector<ColorBlob*>* blobList, int minimum_num_pixels_in_speck)
{
	for(std::vector<ColorBlob*>::iterator iter = blobList->begin(); iter != blobList->end(); iter++)
	{
        (*iter)->EliminateTinyNoiseSpeckles(minimum_num_pixels_in_speck);
    }
}


ColorBlob *
Segmenter_SingleImageReturn::getBiggestBlob(std::vector<ColorBlob *> * blobList)
{
	if(blobList->empty())
		return nullptr;

	ColorBlob* blob = *(blobList->begin());
	for(std::vector<ColorBlob*>::iterator candidate_iter = blobList->begin(); candidate_iter != blobList->end(); candidate_iter++)
		if ((*candidate_iter)->area() > blob->area())
			blob = (*candidate_iter);
	return blob;
}

cv::Mat
Segmenter_SingleImageReturn::getShapeFromBlob(ColorBlob * blob, cv::Size imgSize, bool fill_in_blob)
{
	cv::Mat shape;
	if (blob != nullptr)
	{
		shape = cv::Mat::zeros(imgSize, CV_8UC1);

        if(fill_in_blob)
            blob->drawFilledIntoFalseColorImg(shape, 255);
        else
            blob->drawIntoFalseColorImg(shape, FalseColor(255,255,255));

		shape.convertTo(shape, CV_8UC1);
		shape = shape > 0;
	}
	else
		shape = cv::Mat();

	return shape;
}

int blobs_saved_counter_globalll = 0;

void
Segmenter_SingleImageReturn::saveBlobsToFalseColorImage(std::vector<ColorBlob*>* blobList, std::string filename)//, cv::Size bsize)
{
	//cv::Mat outputImg = cv::Mat(bsize, CV_8UC3);
	//outputImg.setTo(0);
	//int index = 0;
	//for each (ColorBlob ^ blob in blobList)
	for(std::vector<ColorBlob*>::iterator iter = blobList->begin(); iter != blobList->end(); iter++)
	{
        saveImage(*((*iter)->GetMyMaskEvenThoughItIsPrivate()), filename + to_istring(++blobs_saved_counter_globalll) + std::string(".bmp"));

		//FalseColor blobColor = hashIndexToFalseColor(index++);
		//blob->drawIntoFalseColorImg(outputImg, blobColor);
	}
	//saveImage(outputImg, filename);
}


std::vector<std::vector<cv::Point>>* GetContoursOfMinimumSize__DestroysInputImage(cv::Mat input_mat_black_and_white, float blob_minimum_size_fraction_of_mat)
{
    std::vector<std::vector<cv::Point>>* FoundContours = new std::vector<std::vector<cv::Point>>();
	cv::findContours(input_mat_black_and_white, *FoundContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0,0));

    if(FoundContours->empty()==false)
    {
        float area_of_contour = 0.0f;
        float min_allowed_size_of_blob = static_cast<float>(input_mat_black_and_white.rows * input_mat_black_and_white.cols);
        min_allowed_size_of_blob *= blob_minimum_size_fraction_of_mat;

        std::vector<std::vector<cv::Point>>::iterator contouriter = FoundContours->begin();
        for(; contouriter != FoundContours->end();)
        {
            area_of_contour = cv::contourArea(*contouriter);

            if(area_of_contour < min_allowed_size_of_blob)
            {
                contouriter = FoundContours->erase(contouriter);
            }
            else
                contouriter++;
        }
    }
    return FoundContours;
}
