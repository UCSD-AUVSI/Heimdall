#ifndef STUBOCR_H
#define STUBOCR_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class StubOCR : AUVSI_Algorithm
{
	public:
		static void execute(imgdata_t *data);
};

#endif
