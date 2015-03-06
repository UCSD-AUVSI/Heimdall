#pragma once

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <list>
#include "skynet_VisionUtil.hpp"
#include "SegmentationSettings.hpp"


namespace Skynet {



	class ColorBlob;
	class Segmenter;

	class Segmenter_SingleImageReturn
	{
	public:
		Segmenter_SingleImageReturn(void);
		~Segmenter_SingleImageReturn();

		cv::Mat findShape(cv::Mat colorImg,
                        const Segmenter_Module_Settings & attempt_settings,
                        float crop_was_resized_how_much,
                        float input_MINIMUM_SPECK_SIZE_THRESHOLD,
                        cv::Scalar* input_color_of_previously_found_shapeblob,
                        cv::Scalar* returned_color_of_blob,
                        cv::Mat* mask_of_returned_shape_blob,
                        std::string name_of_process_calling_this_function,
                        std::string folder_to_save_intermediate_images,
                        int seg_setting_index_for_testing,
                        cv::Mat* returned_mat_of_binned_histogram=nullptr,
                        cv::Mat* returned_mat_preprocessed=nullptr);


        static void saveBlobsToFalseColorImage(std::vector<ColorBlob*>& blobList, std::string filename);//, cv::Size bsize);

	private:
        std::vector<ColorBlob*> getRoundishBlobs_ByCircularity(std::vector<ColorBlob*>& blobList, float cutoff_circularity);

		std::vector<ColorBlob*> getLargeBlobs(std::vector<ColorBlob*>& blobList, float img_area);
		
		std::vector<ColorBlob*> getInteriorBlobs(std::vector<ColorBlob*>& blobList,
												float acceptable_fraction_of_border_pixels_that_can_be_in_the_blob,
												float acceptable_fraction_of_blobs_pixels_that_touch_edge);

		std::vector<ColorBlob*> eliminateBlobsTooCloseToInputColor(std::vector<ColorBlob*>& blobList,
                                                                    std::vector<cv::Mat>& masks_of_blobList,
                                                                    std::vector<cv::Scalar>& colors_of_blobList,
                                                                    cv::Scalar* color_to_eliminate,
                                                                    float max_distance_threshold);

        static void ClearBlobsOfTinyNoiseSpeckles(std::vector<ColorBlob*>& blobList, int minimum_num_pixels_in_speck);

		ColorBlob * getBiggestBlob(std::vector<ColorBlob*>& blobList);
		cv::Mat getShapeFromBlob(ColorBlob * blob, cv::Size imgSize, bool fill_in_blob);
		Segmenter * segmenter;
	};


	class ColorBlobCircularitySorter
	{
	public:
        float circularity;
        ColorBlob* thisblob;


        ColorBlobCircularitySorter() : circularity(0.0f), thisblob(nullptr) {}
        ColorBlobCircularitySorter(float CIRCULARITY, ColorBlob* THISBLOB) : circularity(CIRCULARITY), thisblob(THISBLOB) {}


        static bool SortDescending(const ColorBlobCircularitySorter &lhs, const ColorBlobCircularitySorter &rhs) {return (lhs.circularity > rhs.circularity);}
	};
}

std::vector<std::vector<cv::Point>>* GetContoursOfMinimumSize__DestroysInputImage(cv::Mat input_mat_black_and_white, float blob_minimum_size_fraction_of_mat);

//
