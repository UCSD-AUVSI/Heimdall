#ifndef STUB_SALIENCY_H_
#define STUB_SALIENCY_H_

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class StubSaliency : AUVSI_Algorithm{
	public:
		static void execute(imgdata_t *data, std::string args);
};

#endif
