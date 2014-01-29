#include <iostream>

using std::cout;
using std::endl;

#include "Recognition/ShapeRec/TemplateSRec.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"

void TemplateSRec :: execute(imgdata_t *data){
	setDone(data, TEMPLATE_SREC);
	cout << "Template Shape Rec" << endl;	
}
