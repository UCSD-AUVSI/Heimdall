#include <iostream>

using std::cout;
using std::endl;

#include "Recognition/CharacterSegmentation.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/IMGData.hpp"
//#include "Backbone/Algs.hpp"

void CharacterSegmentation :: execute(imgdata_t &data){
	setDone(data, CSEG);
	cout << "Char Seg" << endl;	
}
