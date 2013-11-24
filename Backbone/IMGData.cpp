#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"

//TODO: Change according to change in boolean mapping (see IMGData.cpp)

void setDone(imgdata_t *data, alg_t alg){
	switch(alg){
		case ORGR:			data->orgrDone = true; 											break;
		case SALIENCY: 		data->saliencyDone = true;										break;
		case GUISAL: 		data->saliencyDone = true;										break;
		case GUIREC:		data->segDone = true; data ->sDone = true; data->cDone = true; 	break;
		case BLOB_SEG: 		data->segDone = true; 											break;
		case EROSION_SEG: 	data->segDone = true;				 							break;
		case TEMPLATE_SREC: data->sDone = true;												break;
		case TESS_OCR: 		data->cDone = true;												break;
		case VERIF: 		data->verified = true;											break;
	}	
}
