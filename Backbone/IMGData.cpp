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
		case SSALIENCY: 		data->saliencyDone = true;										break;
		case GUISAL: 			data->saliencyDone = true;										break;
		case GUIREC:			data->segDone = true; data ->sDone = true; data->cDone = true; 	break;
		case BLOB_SEG: 			data->segDone = true; 											break;
		case SKYNET_SEG: 		data->segDone = true; 											break;
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

// TODO: Add consequences if imgdata_t is not initialized
void initEmptyIMGData(imgdata_t *data){
	data->image_data = new std::vector<std::vector<unsigned char>*>();
	data->sseg_image_data = new std::vector<std::vector<unsigned char>*>();
	data->cseg_image_data = new std::vector<std::vector<unsigned char>*>();
	data->sseg_image_sizes = new std::vector<uint32_t>();
	data->cseg_image_sizes = new std::vector<uint32_t>();
	data->initialized = true;
}

void clearIMGData(imgdata_t *data){
	if(data->image_data){
		for(std::vector<std::vector<unsigned char>*>::iterator i = data->image_data->begin();
				i < data->image_data->end(); ++i){
			(*i)->clear();
		}
		data->image_data->clear();
		delete data->image_data;
	}

	if(data->sseg_image_data){
		for(std::vector<std::vector<unsigned char>*>::iterator i = data->sseg_image_data->begin();
				i < data->sseg_image_data->end(); ++i){
			(*i)->clear();
		}
		data->sseg_image_data->clear();
		delete data->sseg_image_data;
	}

	if(data->cseg_image_data){
		for(std::vector<std::vector<unsigned char>*>::iterator i = data->cseg_image_data->begin();
				i < data->cseg_image_data->end(); ++i){
			(*i)->clear();
		}
		data->cseg_image_data->clear();
		delete data->cseg_image_data;
	}

	if(data->sseg_image_sizes){
		data->sseg_image_sizes->clear();
		delete data->sseg_image_sizes;
	}

	if(data->cseg_image_sizes){
		data->cseg_image_sizes->clear();
		delete data->cseg_image_sizes;
	}
}
