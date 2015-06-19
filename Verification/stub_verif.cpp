#include <string>
#include <iostream>

#include "Verification/stub_verif.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"

using std::cout;
using std::endl;

void StubVerify :: execute(imgdata_t *imdata, std::string args){
    cout << "Stub Verify" << endl << endl;
    
	if(imdata->shape.empty())
		cout << "no shape found!" << endl;
	else
		cout << "shape found: \'" << imdata->shape << "\'" << endl;
	
	if(imdata->character1.empty())
		cout << "no character 1 found!" << endl;
	else
		cout << "character 1 found: \'" << imdata->character1 << "\'" << endl;
	
    setDone(imdata, VERIF);
}
