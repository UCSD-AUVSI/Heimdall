#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"

#include <iostream>

using std::cout;
using std::endl;

//TODO: Change according to change in boolean mapping (see IMGData.cpp)

void setDone(imgdata_t *data, alg_t alg){
	switch(alg){
		case STUB_ORGR:			data->orgrDone = true; 											break;
		case STUB_SALIENCY: 	data->saliencyDone = true;										break;
		case GUISAL: 			data->saliencyDone = true;										break;
		case GUIREC:			data->segDone = true; data ->sDone = true; data->cDone = true; 	break;
		case BLOB_SEG: 			data->segDone = true; 											break;
		case EROSION_SEG: 		data->segDone = true;				 							break;
		case TEMPLATE_SREC: 	data->sDone = true;												break;
		case TESS_OCR: 			data->cDone = true;												break;
		case STUB_VERIF: 		data->verified = true;											break;
	}	
}

void img_print(imgdata_t* data){
	cout << "ID: " << data->id << endl;
	cout << "ORGRD: " <<  data->orgrDone << endl;
	cout << "SALD: " << data->saliencyDone << endl;
	cout << "SEGD: " << data->segDone << endl;
	cout << "SD: " << data->sDone << endl;
	cout << "CD: " << data->cDone << endl;
	cout << "VER: " << data->verified << endl << endl;
}


void freeIMGData(imgdata_t *data){
	data->image_data->clear();
	delete data->image_data;
}
