#ifndef ORTHORECT_H
#define ORTHORECT_H

#include "Backbone/IMGData.hpp"
#include "Backbone/Algorithm.hpp"

class Orthorect : Algorithm {
	public:
		static void execute(imgdata_t &data);
};

#endif
