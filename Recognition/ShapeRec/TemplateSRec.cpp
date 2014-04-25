#include <iostream>

using std::cout;
using std::endl;

#include "Recognition/ShapeRec/TemplateSRec.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"

void TemplateSRec :: execute(imgdata_t *data, std::string args){
	setDone(data, SREC);
	cout << "Template Shape Rec" << endl;	
}
