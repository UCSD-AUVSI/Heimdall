#ifndef EXPERIMENT_VERIF_H
#define EXPERIMENT_VERIF_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

#include <fstream>

class ExperimentVerify : AUVSI_Algorithm {
	public:
		static void execute(imgdata_t *data, std::string args);
};

#endif
