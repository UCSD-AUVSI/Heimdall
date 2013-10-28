#ifndef SHAPESEGMENTATION_H
#define SHAPESEGMENTATION_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class ShapeSegmentation : AUVSI_Algorithm{
	public:
		static void execute(imgdata_t &data);
};

#endif
