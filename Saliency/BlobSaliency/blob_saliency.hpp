#ifndef BLOB_SALIENCY_H_
#define BLOB_SALIENCY_H_

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class Blob_Saliency_Module;
extern Blob_Saliency_Module* global_sal_instance; //instantiated in BlobSaliency.cpp

class BlobSaliency : AUVSI_Algorithm{
	public:
		static void execute(imgdata_t *data, std::string args);
};

#endif
