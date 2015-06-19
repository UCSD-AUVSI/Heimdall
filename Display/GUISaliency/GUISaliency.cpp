#include <string>
#include <iostream>

using std::cout;
using std::endl;

#include "GUISaliency/GUISaliency.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/IMGData.hpp"

void GUISaliency :: execute(imgdata_t *data, std::string args){
	setDone(data, SALIENCY);
	cout << "GUI Saliency" << endl;
}
