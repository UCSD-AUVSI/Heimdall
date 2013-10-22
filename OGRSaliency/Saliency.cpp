#include <string>
#include <iostream>

using std::cout;
using std::endl;

#include "OGRSaliency/Saliency.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/Algorithm.hpp"
#include "Backbone/IMGData.hpp"
//#include "Backbone/Algs.hpp"

void Saliency :: execute(imgdata_t &data){
	setDone(data, SALIENCY);
	cout << "Saliency" << endl;
}
