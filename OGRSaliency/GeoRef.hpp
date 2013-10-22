#ifndef GEOREF_H
#define GEOREF_H

#include "Backbone/IMGData.hpp"
#include "Backbone/Algorithm.hpp"

class GeoRef : Algorithm {
	public:
		static void execute(imgdata_t &data);
};

#endif
