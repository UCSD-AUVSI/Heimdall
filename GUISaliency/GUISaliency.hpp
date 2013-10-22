#ifndef GUISALIENCY_H
#define GUISALIENCY_H

#include "Backbone/IMGData.hpp"
#include "Backbone/Algorithm.hpp"

class GUISaliency : Algorithm{
	public:
		static void execute(imgdata_t &data);
};

#endif
