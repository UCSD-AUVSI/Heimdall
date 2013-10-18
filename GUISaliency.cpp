#include <string>
#include <iostream>

using std::cout;
using std::endl;

#include "Backbone.hpp"
#include "Algorithm.hpp"
#include "IMGData.hpp"
#include "Algs.hpp"

void GUISaliency :: execute(imgdata_t &data){
	setDone(data, SALIENCY);
	cout << "GUI Saliency" << endl;
}
