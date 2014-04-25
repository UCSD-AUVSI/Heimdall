#ifndef GUISALIENCY_H
#define GUISALIENCY_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class GUISaliency : AUVSI_Algorithm{
	public:
		static void execute(imgdata_t *data, std::string args);
};

#endif
