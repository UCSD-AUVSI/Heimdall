#include <iostream>

using std::cout;
using std::endl;

#include "Backbone.hpp"
#include "Algorithm.hpp"
#include "IMGData.hpp"
#include "Algs.hpp"

void CharacterSegmentation :: execute(imgdata_t &data){
	setDone(data, CSEG);
	cout << "Char Seg" << endl;	
}
