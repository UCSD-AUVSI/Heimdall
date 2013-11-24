#ifndef ORGR_H
#define ORGR_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class ORGRF : AUVSI_Algorithm {
	public:
		static void execute(imgdata_t *data);
};

#endif
