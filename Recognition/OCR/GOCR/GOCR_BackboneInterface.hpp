#ifndef GOCR_BACKBONEINTERFACE_H
#define GOCR_BACKBONEINTERFACE_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"


class AnyOCR_Module_Main;

//instantiated in GOCR_BackboneInterface.cpp
extern AnyOCR_Module_Main* global_GOCR_module_instance;


class GOCRBackboneInterface : AUVSI_Algorithm
{
	public:
		static void execute(imgdata_t *imdata);
};

#endif
