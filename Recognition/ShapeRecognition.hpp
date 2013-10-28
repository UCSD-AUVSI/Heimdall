#ifndef SHAPERECOGNITION_H
#define SHAPERECOGNITION_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class ShapeRecognition : AUVSI_Algorithm{
	public:
		static void execute(imgdata_t &data);
};

#endif
