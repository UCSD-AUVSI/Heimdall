#pragma once

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "skynet_VisionUtil.hpp"
#include "skynet_SegmenterTemplate.hpp"

namespace Skynet {

	extern float COLOR_DISTANCE_THRESHOLD;
	extern float MERGE_COLOR_DISTANCE; // approx one bin
	extern int NUM_VALID_COLORS;
	extern int NUM_BINS; // was 20 - 98% ugly
	extern float MINIMUM_BLOB_SIZE_THRESHOLD; //depends how well Saliency can crop around the target...
                                                //sometimes the target is small within the crop


	template<typename T> class MRef;
	class ColorBlob;

	class HistogramSegmenter : public Segmenter
	{
	public:
		HistogramSegmenter(void);
		~HistogramSegmenter();

		virtual std::vector<ColorBlob*> findBlobs(cv::Mat colorImg, cv::Mat* returned_binned_mat) override;
		virtual cv::Mat secondSegmentation(PixelColor color) override;
	private:
		cv::Mat reduceColorsWithBinning();
		std::vector<ColorBlob*> findBlobWithHistogram(cv::Mat* returned_binned_mat);
		void setImageWithPreprocessing(cv::Mat colorImg);

		std::vector<PixelColor> biggestColorsInHistogram(cv::MatND hist, int numColors, int channels_of_src_img);
		PixelColor getAndRemoveBiggestColorInHistogram(cv::MatND hist, int channels_of_src_img);
		void getBiggestIndexInHist(cv::MatND hist, int idxOfMax[3]);
		void zeroOutBinAtIndex(cv::MatND hist, int index[3], int channels_of_src_img);
		std::vector<PixelColor> mergeCloseColors(std::vector<PixelColor>& colors);
		PixelColor convertBinToColor(int idxOfBiggest[3]);
		cv::Mat redrawImageWithColors(cv::Mat& input, std::vector<PixelColor>& validColors);
		PixelColor convertToValidColor(PixelColor inputColor, std::vector<PixelColor>& validColors);
		std::vector<ColorBlob*> segmentImageIntoBlobsWithColors(cv::Mat& input, std::vector<PixelColor>& validColors);
		std::vector<ColorBlob*> makeBlobsWithColors(std::vector<PixelColor>& colors);
		void drawImageIntoBlobs(cv::Mat& input, std::vector<ColorBlob*>& blobs);
		void drawPixelIntoBlobs(cv::Point pt, PixelColor pixelColor, std::vector<ColorBlob*>& blobs);

		cv::Mat mImg;
		//int numBins;
		float binsToRGBRatio;
	};

}
