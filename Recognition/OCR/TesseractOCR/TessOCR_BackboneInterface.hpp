#ifndef TESSOCR_BACKBONEINTERFACE_H
#define TESSOCR_BACKBONEINTERFACE_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class AnyOCR_Module_Main;

//instantiated in TessOCR_BackboneInterface.cpp
extern AnyOCR_Module_Main* global_TessOCR_module_instance;


class TessOCR : AUVSI_Algorithm
{
	public:
		static void execute(imgdata_t *imdata);
};

#endif
