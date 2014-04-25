#ifndef TESSOCR_H
#define TESSOCR_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class TessOCR :  AUVSI_Algorithm
{
	public:
		static void execute(imgdata_t *data, std::string args);
};

#endif
