#ifndef SALIENCY_H
#define SALIENCY_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class Saliency : AUVSI_Algorithm{
	public:
		static void execute(imgdata_t *data);
};

#endif
