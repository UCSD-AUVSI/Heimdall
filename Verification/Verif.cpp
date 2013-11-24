#include <string>
#include <iostream>

#include "Verification/Verif.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/IMGData.hpp"
#include "Backbone/Algs.hpp"

using std::cout;
using std::endl;

void Stub_Verify :: execute(imgdata_t *data){
	setDone(data, STUB_VERIF);
	cout << "Stub Verification" << endl << endl;
}
