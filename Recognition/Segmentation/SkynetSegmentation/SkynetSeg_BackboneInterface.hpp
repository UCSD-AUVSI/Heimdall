#ifndef SKYNETSEG_BACKBONEINTERFACE_H
#define SKYNETSEG_BACKBONEINTERFACE_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

//class Segmentation_SSEG_and_CSEG_and_Merger;
//instantiated in SkynetSeg_BackboneInterface.cpp
//extern Segmentation_SSEG_and_CSEG_and_Merger* global_SkynetSegmentation_module_instance;

class SkynetSeg : public AUVSI_Algorithm
{
	public:
		static void execute(imgdata_t *imdata, std::string args);
};

#endif
