#include <iostream>

using std::cout;
using std::endl;

#include "Recognition/CharacterRecognition.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/Algorithm.hpp"
#include "Backbone/IMGData.hpp"
//#include "Backbone/Algs.hpp"

void CharacterRecognition :: execute(imgdata_t &data){
	setDone(data, OCR);
	cout << "OCR" << endl;
}
