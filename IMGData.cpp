#include "Backbone.hpp"
#include "IMGData.hpp"

void setDone(imgdata_t &data, alg_t alg){
	switch(alg){
		case ORTHORECT:	data.orthorectDone = true; 					break;
		case GEOREF: 	data.georefDone = true;						break;
		case SALIENCY: 	data.saliencyDone = true;					break;
		case SEG: 		data.csegDone = true; data.ssegDone = true;	break;
		case SSEG: 		data.ssegDone = true;						break;
		case CSEG: 		data.csegDone = true;						break;
		case SREC: 		data.sDone = true;							break;
		case OCR: 		data.cDone = true;							break;
		case VERIF: 	data.verified = true;						break;
	}	
}
