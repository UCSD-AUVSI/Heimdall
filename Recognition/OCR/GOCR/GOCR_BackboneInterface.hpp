#ifndef GOCR_BACKBONEINTERFACE_H
#define GOCR_BACKBONEINTERFACE_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "ocr_algorithm_gocr.hpp"

//instantiated in GOCR_BackboneInterface.cpp
extern OCRModuleAlgorithm_GOCR* global_GOCR_module_instance;

class GOCRBackboneInterface : AUVSI_Algorithm
{
	public:
		static void execute(imgdata_t *imdata, std::string args);
};

#endif
