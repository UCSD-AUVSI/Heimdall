#ifndef SPECTRAL_RESIDUAL_SALIENCY_H_
#define SPECTRAL_RESIDUAL_SALIENCY_H_

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class SpectralResidualSaliency : AUVSI_Algorithm{
	public:
		static void execute(imgdata_t *data, std::string args);
};

#endif
