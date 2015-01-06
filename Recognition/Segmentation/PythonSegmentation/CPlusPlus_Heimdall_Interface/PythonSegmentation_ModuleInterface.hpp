#ifndef _PYTHON_SEGMENTATION_MODULE_INTERFACE_H_
#define _PYTHON_SEGMENTATION_MODULE_INTERFACE_H_

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class PythonSegmentation : AUVSI_Algorithm{
	public:
		static void execute(imgdata_t *data, std::string args);
};

#endif
