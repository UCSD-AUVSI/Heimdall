#include <iostream>

#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"
#include "Recognition/Segmentation/BlobSeg.hpp"

using std::cout;
using std::endl;

void BlobSeg :: execute(imgdata_t *data){
	setDone(data, BLOB_SEG);
	cout << "Blob Segmentation" << endl;	
}	
