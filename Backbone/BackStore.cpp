#include <iostream>

#include <map>
#include <stdexcept>
#include <mutex>

#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"
#include "Backbone/MessageHandling.hpp"

using std::cout;
using std::endl;

std::mutex store_lock;

static std::map<int, unsigned char *> store;

bool img_check(imgdata_t *data){
	return (data->sDone == data->cDone);
}

void orUpdate(imgdata_t *first, imgdata_t *second){
	first->orgrDone 	|= second->orgrDone;
	first->saliencyDone |= second->saliencyDone;
	first->segDone 		|= second->segDone;
	first->sDone 		|= second->sDone;
	first->cDone 		|= second->cDone;
	first->verified 	|= second->verified;
}

// TODO: Currently, BackStore does not save image data properly
// Need to properly and intelligently save data
void updateImageData(imgdata_t *previmg, imgdata_t *newimg){}

bool img_update(imgdata_t *data){
	store_lock.lock();
	unsigned char *&imgarr = store[data->id];
		
	imgdata_t img;
	expandData(&img, imgarr);
	
	if(img.id != data->id){
		img.id = data->id;
	}

	orUpdate(&img, data);
	orUpdate(data, &img);

	updateImageData(&img, data);

	if(!data->initialized){
		initEmptyIMGData(data);
	}

	// TODO: Make this less inefficient. Currently always deallocates
	// and reallocates an entire array, which can be a significant 
	// length. should try and make it only update what is necessary
	int len = 0;
	unsigned char *newimgarr = linearizeData(&img, &len);
	
	unsigned char *temp = imgarr;
	imgarr = newimgarr;
	delete [] temp;
	
	store_lock.unlock();
	return img_check(data);
}

bool img_delete(imgdata_t *data){
	try {
		delete [] store.at(data->id);
	}
	catch(const std::out_of_range& oor){
		return false;
	}
	return store.erase(data->id)==1?true:false;
}
