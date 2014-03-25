#ifndef STUBSEG_H
#define STUBSEG_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class StubSeg : public AUVSI_Algorithm
{
	public:
		static void execute(imgdata_t *data);
};

#endif
