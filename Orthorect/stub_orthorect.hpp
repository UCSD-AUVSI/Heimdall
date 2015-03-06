#ifndef STUB_ORTHORECT_H
#define STUB_ORTHORECT_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class StubOrthorect : AUVSI_Algorithm {
	public:
		static void execute(imgdata_t *data, std::string args);
};

#endif
