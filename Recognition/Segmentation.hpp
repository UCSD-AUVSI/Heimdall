#ifndef SEGMENTATION_H
#define SEGMENTATION_H

#include "Backbone/IMGData.hpp"
#include "Backbone/Algorithm.hpp"

class Segmentation : Algorithm{
	public:
		static void execute(imgdata_t &data);
};

#endif
