#include <iostream>

#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"
#include "Recognition/Segmentation/BlobSeg.hpp"

using std::cout;
using std::endl;

void BlobSeg :: execute(imgdata_t *imdata, std::string args){
	setDone(imdata, SEG);
	cout << "Blob Segmentation" << endl;	
}	
