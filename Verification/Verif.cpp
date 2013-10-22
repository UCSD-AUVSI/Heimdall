#include <string>
#include <iostream>

using std::cout;
using std::endl;

#include "Verification/Verif.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/Algorithm.hpp"
#include "Backbone/IMGData.hpp"
//#include "Backbone/Algs.hpp"

void Verify :: execute(imgdata_t &data){
	setDone(data, VERIF);
	cout << "Verification" << endl << endl;
}
