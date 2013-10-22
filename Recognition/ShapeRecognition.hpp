#ifndef SHAPERECOGNITION_H
#define SHAPERECOGNITION_H

#include "Backbone/IMGData.hpp"
#include "Backbone/Algorithm.hpp"

class ShapeRecognition : Algorithm{
	public:
		static void execute(imgdata_t &data);
};

#endif
