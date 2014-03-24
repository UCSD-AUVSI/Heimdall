#pragma once

#include "SharedUtils/SharedUtils.hpp"
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "skynet_VisionUtil.hpp"

namespace Skynet {

	typedef int BlobId;
	typedef cv::Vec<unsigned char, 1> MaskElement;
	typedef cv::Vec<BlobId, 1> BlobIdElement;
	typedef cv::Vec<unsigned char, 3> HistColor;
	typedef cv::Vec<unsigned char, 3> FalseColor;
	typedef cv::Vec<unsigned char, 1> FalseColor_1channel;
	typedef cv::Vec<unsigned char, 2> FalseColor_2channel;

	#define MaxColorDistanceForMerge 7.5f; // 5 = 85% pretty, 7.5=90% pretty, histogram bins are in range 0-NUM_HISTOGRAM_BINS
	#define NUM_HISTOGRAM_BINS 20 // 25 works well

	class ColorBlob;

	class ColorBlobStatistics : public CachedStatistics
	{
	public:
		ColorBlobStatistics(ColorBlob * p) : CachedStatistics() { parent = p; cachedArea = -1.0f; }

		PixelColor getMostCommonColor(const cv::Mat& img, float area);
	private:
		ColorBlob * parent;
		MRef<PixelColor> * mostCommonColor;
		float cachedArea;

		void updateStatistics() { throw myexception("Not implemented"); }
	};

	class ColorBlob
	{
	public:
		ColorBlob(BlobId bId, cv::Size sizeIn);
		~ColorBlob();
		bool hasSameColor(ColorBlob * blob, const cv::Mat& img);
		void expandToContainBlob(ColorBlob * blob, cv::Mat *blobIds);
		void expandToContainBlob(ColorBlob * blob) {expandToContainBlob(blob, NULL);}
		void calculateBlobColor(const cv::Mat& img);
		void drawIntoImg(cv::Mat& img);
		void drawIntoFalseColorImg(cv::Mat& img, FalseColor color);
		void drawFilledIntoFalseColorImg(cv::Mat& img, unsigned char color);
		PixelColor mostCommonColor(const cv::Mat& img);

        float CalculateTotalPerimeters();
        float CalculateCircularity();

		float area();
		bool isInterior(float acceptable_fraction_of_blobs_pixels_that_touch_edge);
		PixelColor getBlobColor() { return *blobColor; }
		void setBlobColor(PixelColor color);

		void EliminateTinyNoiseSpeckles(int minimum_num_pixels_in_speck);

		void addPoint(cv::Point pt, cv::Mat blobIds);
		void addPoint(cv::Point pt) {addPoint(pt, cv::Mat());}

		static void runTests();

		BlobId id;
		PixelColor* blobColor;

#if 1
        float saved_last_fraction_of_blobs_pixels_along_border;
		cv::Mat* GetMyMaskEvenThoughItIsPrivate() {return mask;}
#endif
	private:
		cv::Size *size;
		cv::Mat *mask;
		ColorBlobStatistics * stats;

		cv::Mat clonedMaskWithNoSmallBlobs(int minimum_num_pixels_in_speck);
		PixelColor getARandomPixel(const cv::Mat& img);
		cv::MatND calcHistogram(const cv::Mat& img);
		PixelColor getHistogramPeak(cv::MatND hist);
		unsigned char maskValueAt(cv::Point pt);

		void testSameColor();
		void testExpandToContainBlob();
		void testCalcBlobColor();
		void testAddPoint();
	};

}
