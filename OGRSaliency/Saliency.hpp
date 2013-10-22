#ifndef SALIENCY_H
#define SALIENCY_H

#include "Backbone/IMGData.hpp"
#include "Backbone/Algorithm.hpp"

class Saliency : Algorithm{
	public:
		static void execute(imgdata_t &data);
};

#endif
