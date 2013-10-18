#include <string>
#include <iostream>

using std::cout;
using std::endl;

#include "Backbone.hpp"
#include "Algorithm.hpp"
#include "IMGData.hpp"
#include "Algs.hpp"

void Orthorect :: execute(imgdata_t &data){
	setDone(data, ORTHORECT);
	cout << "OrthoRect" << endl;
}
