#ifndef STUB_SHAPEREC_H
#define STUB_SHAPEREC_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class StubSRec : AUVSI_Algorithm{
	public:
		static void execute(imgdata_t *imdata, std::string args);
};

#endif
