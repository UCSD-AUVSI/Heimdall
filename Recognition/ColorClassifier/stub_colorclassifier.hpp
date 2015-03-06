#ifndef STUB_COLOR_CLASSIFIER_H
#define STUB_COLOR_CLASSIFIER_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class StubColorClassifier : AUVSI_Algorithm{
	public:
		static void execute(imgdata_t *imdata, std::string args);
};

#endif
