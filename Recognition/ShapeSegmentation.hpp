#ifndef SHAPESEGMENTATION_H
#define SHAPESEGMENTATION_H

#include "Backbone/IMGData.hpp"
#include "Backbone/Algorithm.hpp"

class ShapeSegmentation : Algorithm{
	public:
		static void execute(imgdata_t &data);
};

#endif
