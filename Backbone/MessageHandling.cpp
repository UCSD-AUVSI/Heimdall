#include <iostream>
#include <vector>

#include <zmq.hpp>

#include "Backbone/MessageHandling.hpp"
#include "Backbone/IMGData.hpp"
#include "Backbone/Backbone.hpp"

using std::cout;
using std::endl;

void populateSizes(imgdata_t *imdata){
	if(imdata->image_data->size()){
		imdata->image_data_size = imdata->image_data->back()->size();
	}

	if(imdata->sseg_image_sizes->size() != imdata->sseg_image_data->size()){
		for(std::vector<std::vector<unsigned char>*>::iterator i = imdata->sseg_image_data->begin();
				i < imdata->sseg_image_data->end(); ++i){
			imdata->sseg_image_sizes->push_back((*i)->size());
		}
		imdata->sseg_image_size_count = imdata->sseg_image_sizes->size();
	}

	if(imdata->cseg_image_sizes->size() != imdata->cseg_image_data->size()){
		for(std::vector<std::vector<unsigned char>*>::iterator i = imdata->cseg_image_data->begin();
				i < imdata->cseg_image_data->end(); ++i){
			imdata->cseg_image_sizes->push_back((*i)->size());
		}
		imdata->cseg_image_size_count = imdata->cseg_image_sizes->size();
	}
}

int messageSizeNeeded(imgdata_t *imdata){
	populateSizes(imdata);

	int len = sizeof(imgdata_t);
	len += imdata->image_data_size;
	len += imdata->sseg_image_size_count * sizeof(uint32_t);
	len += imdata->cseg_image_size_count * sizeof(uint32_t);

	for(std::vector<uint32_t>::iterator i = imdata->sseg_image_sizes->begin();
			i < imdata->sseg_image_sizes->end(); ++i){
		len += *i;
	}
	for(std::vector<uint32_t>::iterator i = imdata->cseg_image_sizes->begin();
			i < imdata->cseg_image_sizes->end(); ++i){
		len += *i;
	}
	return len;
}

void printMsgBytes(zmq::message_t *msg){
	unsigned char arr[msg->size()];
	memcpy(arr, msg->data(), msg->size());

	cout << "Printing Message (" << msg->size() << "): " << endl;
	for(int i = 0; i < msg->size(); i++){
		printf("%x ", arr[i]);
	}
	cout << endl;
}

void printStructBytes(imgdata_t *imdata){
	unsigned char arr[sizeof(imgdata_t)];
	memcpy(arr, imdata, sizeof(imgdata_t));

	cout << "Printing Struct (" << sizeof(imgdata_t) << "): " << endl;
	for(int i = 0; i < sizeof(imgdata_t); i++){
		printf("%x ", arr[i]);
	}
	cout << endl;
}

unsigned char *linearizeData(imgdata_t *imdata, int *retlen){
	int start = 0;

	*retlen = messageSizeNeeded(imdata);
	unsigned char *arr = new unsigned char[*retlen];

	memcpy(arr, imdata, sizeof(imgdata_t));
	start += sizeof(imgdata_t);

	if(imdata->image_data_size){
		memcpy(arr + start, &(*(imdata->image_data->back()))[0], imdata->image_data_size);
		start += imdata->image_data_size;
	}

	if(imdata->sseg_image_size_count){
		memcpy(arr + start, &(*(imdata->sseg_image_sizes))[0], imdata->sseg_image_size_count * sizeof(uint32_t));
		start += imdata->sseg_image_size_count * sizeof(uint32_t);
	}

	if(imdata->cseg_image_size_count){
		memcpy(arr + start, &(*(imdata->cseg_image_sizes))[0], imdata->cseg_image_size_count * sizeof(uint32_t));
		start += imdata->cseg_image_size_count * sizeof(uint32_t);
	}

	for(std::vector<std::vector<unsigned char>*>::iterator i = imdata->sseg_image_data->begin();
			i < imdata->sseg_image_data->end(); ++i){
		memcpy(arr + start, &(**i)[0], (*i)->size());
		start += (*i)->size();
	}

	for(std::vector<std::vector<unsigned char>*>::iterator i = imdata->cseg_image_data->begin();
			i < imdata->cseg_image_data->end(); ++i){
		memcpy(arr + start, &(**i)[0], (*i)->size());
		start += (*i)->size();
	}
	return arr;
}

void packMessageData(zmq::message_t *msg, imgdata_t *imdata){
	int len = 0;
	unsigned char *arr = linearizeData(imdata, &len);
	memcpy((unsigned char*)msg->data(), arr, len);
	delete [] arr;
}

void expandData(imgdata_t *imdata, unsigned char *arr){
	if(arr == 0){
		initEmptyIMGData(imdata);
		return;
	}
	int start = 0;

	memcpy(imdata, arr, sizeof(imgdata_t));
	start += sizeof(imgdata_t);

	imdata->image_data = new std::vector<std::vector<unsigned char>*>();
	imdata->sseg_image_data = new std::vector<std::vector<unsigned char>*>();
	imdata->cseg_image_data = new std::vector<std::vector<unsigned char>*>();

	if(imdata->image_data_size){
		unsigned char *img_arr = new unsigned char[imdata->image_data_size];

		memcpy(img_arr, arr + start, imdata->image_data_size);
		start += imdata->image_data_size;

		//valgrind doesn't like this ("definitely lost")
		imdata->image_data->push_back(new std::vector<unsigned char>(img_arr, img_arr + imdata->image_data_size));
		delete[] img_arr;
	}

	if(imdata->sseg_image_size_count){
		uint32_t *sseg_image_sizes_arr = new uint32_t[imdata->sseg_image_size_count];

		memcpy(sseg_image_sizes_arr, arr+start, imdata->sseg_image_size_count * sizeof(uint32_t));
		start += imdata->sseg_image_size_count * sizeof(uint32_t);

		imdata->sseg_image_sizes = new std::vector<uint32_t>(sseg_image_sizes_arr, sseg_image_sizes_arr + imdata->sseg_image_size_count);
		delete[] sseg_image_sizes_arr;
	}
	else{
		imdata->sseg_image_sizes = new std::vector<uint32_t>();
	}

	if(imdata->cseg_image_size_count){
		uint32_t *cseg_image_sizes_arr = new uint32_t[imdata->cseg_image_size_count];

		memcpy(cseg_image_sizes_arr, arr+start, imdata->cseg_image_size_count * sizeof(uint32_t));
		start += imdata->cseg_image_size_count * sizeof(uint32_t);
		
		imdata->cseg_image_sizes = new std::vector<uint32_t>(cseg_image_sizes_arr, cseg_image_sizes_arr + imdata->cseg_image_size_count);
		delete[] cseg_image_sizes_arr;
	}
	else{
		imdata->cseg_image_sizes = new std::vector<uint32_t>();
	}

	for(std::vector<uint32_t>::iterator i = imdata->sseg_image_sizes->begin();
			i < imdata->sseg_image_sizes->end(); ++i){
		uint32_t curr_im_size = *i;

		unsigned char *sseg_image_arr = new unsigned char[curr_im_size];
		memcpy(sseg_image_arr, arr+start, curr_im_size);
		start += curr_im_size; 

		imdata->sseg_image_data->push_back(new std::vector<unsigned char>(sseg_image_arr, sseg_image_arr + curr_im_size));
		delete[] sseg_image_arr;
	}

	for(std::vector<uint32_t>::iterator i = imdata->cseg_image_sizes->begin();
			i < imdata->cseg_image_sizes->end(); ++i){
		uint32_t curr_im_size = *i;

		unsigned char *cseg_image_arr = new unsigned char[curr_im_size];
		memcpy(cseg_image_arr, arr+start, curr_im_size);
		start += curr_im_size; 

		imdata->cseg_image_data->push_back(new std::vector<unsigned char>(cseg_image_arr, cseg_image_arr + curr_im_size));
		delete[] cseg_image_arr;
	}

	imdata->initialized = true;
}

void unpackMessageData(imgdata_t *imdata, zmq::message_t *msg){
	expandData(imdata, (unsigned char*)msg->data());
}
