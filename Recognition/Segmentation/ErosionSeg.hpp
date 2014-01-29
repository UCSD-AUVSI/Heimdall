#ifndef EROSIONSEG_H
#define EROSIONSEG_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "opencv2/opencv.hpp"

class ErosionSeg : public AUVSI_Algorithm
{
	public:
		static void execute(imgdata_t *data);
		static void help();
};

class Segmenter{
	public:
		void setMarkers(cv::Mat &markerImage){
			markerImage.convertTo(markers, CV_32S);
		}
		cv::Mat process(cv::Mat &image){
			watershed(image,markers);
			markers.convertTo(markers,CV_8U);
			return markers;
		}

	private:
		cv::Mat markers;
};

#endif
