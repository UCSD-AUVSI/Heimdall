#ifndef STUB_OCR_H
#define STUB_OCR_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class StubOCR : AUVSI_Algorithm
{
	public:
		static void execute(imgdata_t *imdata, std::string args);
};

#endif
