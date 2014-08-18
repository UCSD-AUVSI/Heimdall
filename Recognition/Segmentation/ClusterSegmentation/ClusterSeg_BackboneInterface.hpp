#ifndef CLUSTER_SEG_BACKBONE_INTERFACE_H
#define CLUSTER_SEG_BACKBONE_INTERFACE_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class ClusterSeg : public AUVSI_Algorithm
{
	public:
		static void execute(imgdata_t *data, std::string args);
};

#endif
