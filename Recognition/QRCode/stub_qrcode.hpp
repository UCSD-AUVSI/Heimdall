#ifndef STUB_QRCODE_H
#define STUB_QRCODE_H

#include "Backbone/IMGData.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"

class StubQRCode : AUVSI_Algorithm{
	public:
		static void execute(imgdata_t *imdata, std::string args);
};

#endif
