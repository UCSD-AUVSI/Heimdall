#ifndef SHAPEREC_BACKBONEINTERFACE_HPP
#define SHAPEREC_BACKBONEINTERFACE_HPP

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"


class ShapeRecModule_Main;
//instantiated in ShapeRec_BackboneInterface.cpp
extern ShapeRecModule_Main* global_PolygonShapeRec_instance;



class PolygonShapeRec : AUVSI_Algorithm
{
	public:
		static void execute(imgdata_t *imdata, std::string args);
};

#endif
