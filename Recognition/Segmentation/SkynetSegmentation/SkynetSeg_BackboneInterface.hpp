#ifndef SKYNETSEG_BACKBONEINTERFACE_H
#define SKYNETSEG_BACKBONEINTERFACE_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class Segmentation_SSEG_and_CSEG_and_Merger;

//instantiated in SkynetSeg_BackboneInterface.cpp
//extern Segmentation_SSEG_and_CSEG_and_Merger* global_SkynetSegmentation_module_instance;
extern bool skynetseg_is_currently_running;


class SkynetSeg : public AUVSI_Algorithm
{
	public:
		static void execute(imgdata_t *imdata);
};

#endif
