#include <iostream>
using std::cout; using std::endl;
#include "stub_qrcode.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"

void StubQRCode :: execute(imgdata_t *imdata, std::string args) {
	cout << "Stub QR Code" << endl;	
	setDone(imdata, QRCODE);
}
