#include <string>
#include <iostream>

using std::cout;
using std::endl;

#include "ORGRSaliency/Saliency.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/IMGData.hpp"
#include "Backbone/Algs.hpp"

void Saliency :: execute(imgdata_t *data){
	setDone(data, SALIENCY);
	cout << "Saliency" << endl;
}
