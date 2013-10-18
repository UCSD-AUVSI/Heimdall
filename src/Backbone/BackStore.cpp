#include <iostream>

#include <map>
#include <stdexcept>
#include <mutex>

#include "Backbone.hpp"
#include "IMGData.hpp"

using std::cout;
using std::endl;

std::mutex store_lock;

std::map<int, imgdata_t> store;

void img_print(imgdata_t data){
	cout << "ID: " << data.id << endl;
	cout << "ORD: " <<  data.orthorectDone << endl;
	cout << "GRD: " << data.georefDone << endl;
	cout << "SALD: " << data.saliencyDone << endl;
	cout << "SSEGD: " << data.ssegDone << endl;
	cout << "CSEGD: " << data.csegDone << endl;
	cout << "SD: " << data.sDone << endl;
	cout << "CD: " << data.cDone << endl;
	cout << "VER: " << data.verified << endl << endl;
}

bool img_check(imgdata_t& data){
	return (data.sDone == data.cDone);
}

void orUpdate(imgdata_t& first, imgdata_t& second){
	first.orthorectDone |= second.orthorectDone;
	first.georefDone |= second.georefDone;
	first.saliencyDone |= second.saliencyDone;
	first.ssegDone |= second.ssegDone;
	first.csegDone |= second.csegDone;
	first.sDone |= second.sDone;
	first.cDone |= second.cDone;
	first.verified |= second.verified;
}

bool img_update(imgdata_t& data){
	store_lock.lock();

	imgdata_t& img = store[data.id];
	orUpdate(img, data);
	orUpdate(data, img);

	store_lock.unlock();

	return img_check(data);
}

bool img_delete(imgdata_t data){
	return store.erase(data.id)==1?true:false;
}
