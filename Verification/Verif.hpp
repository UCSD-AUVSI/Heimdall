#ifndef VERIF_H
#define VERIF_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class Stub_Verify : AUVSI_Algorithm {
	public:
		static void execute(imgdata_t *data);
};

#endif
