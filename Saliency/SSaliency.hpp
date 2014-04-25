#ifndef SSALIENCY_H
#define SSALIENCY_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class SSaliency : AUVSI_Algorithm{
	public:
		static void execute(imgdata_t *data, std::string args);
};

class SaliencyModule_C_SBD;
extern SaliencyModule_C_SBD* global_SSaliency_module_instance; //instantiated in SSaliency.cpp

#endif
