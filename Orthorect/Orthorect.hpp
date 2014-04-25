#ifndef ORTHORECT_H
#define ORTHORECT_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class Stub_Orthorect : AUVSI_Algorithm {
	public:
		static void execute(imgdata_t *data, std::string args);
};

#endif
