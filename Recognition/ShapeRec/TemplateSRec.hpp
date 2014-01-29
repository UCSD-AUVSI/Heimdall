#ifndef TEMPLATESHAPEREC_H
#define TEMPLATESHAPEREC_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class TemplateSRec : AUVSI_Algorithm{
	public:
		static void execute(imgdata_t *data);
};

#endif
