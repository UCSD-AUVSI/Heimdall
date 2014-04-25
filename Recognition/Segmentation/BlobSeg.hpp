#ifndef BLOBSEG_H
#define BLOBSEG_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class BlobSeg : public AUVSI_Algorithm
{
	public:
		static void execute(imgdata_t *data, std::string args);
};

#endif
