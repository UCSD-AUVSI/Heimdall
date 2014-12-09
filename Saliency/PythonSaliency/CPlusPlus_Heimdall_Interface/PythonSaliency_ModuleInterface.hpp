#ifndef _PYTHON_SALIENCY_MODULE_INTERFACE_H_
#define _PYTHON_SALIENCY_MODULE_INTERFACE_H_

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class PythonSaliency : AUVSI_Algorithm{
	public:
		static void execute(imgdata_t *data, std::string args);
};

#endif
