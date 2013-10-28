#ifndef SEGMENTATION_H
#define SEGMENTATION_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

using namespace cv;

class Segmentation : public AUVSI_Algorithm
{
	public:
		static void execute(imgdata_t &data);
                static void help();
};

class Segmenter{
        public:
                void setMarkers(Mat &markerImage){
                    markerImage.convertTo(markers, CV_32S);
                }
                Mat process(Mat &image){
                    watershed(image,markers);
                    markers.convertTo(markers,CV_8U);
                    return markers;
                }

        private:
                Mat markers;
        
};
#endif
