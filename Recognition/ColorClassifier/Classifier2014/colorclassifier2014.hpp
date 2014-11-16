#ifndef COLOR_CLASSIFIER_2014_H
#define COLOR_CLASSIFIER_2014_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class ColorClassifier2014 : AUVSI_Algorithm{
	public:
		static void execute(imgdata_t *imdata, std::string args);
};

#endif
