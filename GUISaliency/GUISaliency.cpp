#include <string>
#include <iostream>

using std::cout;
using std::endl;

#include "GUISaliency/GUISaliency.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/Algorithm.hpp"
#include "Backbone/IMGData.hpp"
//#include "Backbone/Algs.hpp"

void GUISaliency :: execute(imgdata_t &data){
	setDone(data, SALIENCY);
	cout << "GUI Saliency" << endl;
}
