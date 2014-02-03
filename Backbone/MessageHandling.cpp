#include <iostream>
#include <vector>

#include <zmq.hpp>

#include "Backbone/MessageHandling.hpp"
#include "Backbone/IMGData.hpp"
#include "Backbone/Backbone.hpp"

using std::cout;
using std::endl;

int messageSizeNeeded(imgdata_t *data){
	if(data->image_data->size()){
		data->image_data_size = data->image_data->at(0)->size();
	}

	if(data->sseg_image_sizes->size() != data->sseg_image_data->size()){
		for(std::vector<std::vector<unsigned char>*>::iterator i = data->sseg_image_data->begin();
				i < data->sseg_image_data->end(); ++i){
			data->sseg_image_sizes->push_back((*i)->size());
		}
		data->sseg_image_size_count = data->sseg_image_sizes->size();
	}

	if(data->cseg_image_sizes->size() != data->cseg_image_data->size()){
		for(std::vector<std::vector<unsigned char>*>::iterator i = data->cseg_image_data->begin();
				i < data->cseg_image_data->end(); ++i){
			data->cseg_image_sizes->push_back((*i)->size());
		}
		data->cseg_image_size_count = data->cseg_image_sizes->size();
	}

	int len = sizeof(imgdata_t);
	len += data->image_data_size;
	len += data->sseg_image_size_count * sizeof(uint32_t);
	len += data->cseg_image_size_count * sizeof(uint32_t);

	for(std::vector<uint32_t>::iterator i = data->sseg_image_sizes->begin();
			i < data->sseg_image_sizes->end(); ++i){
		len += *i;
	}
	for(std::vector<uint32_t>::iterator i = data->cseg_image_sizes->begin();
			i < data->cseg_image_sizes->end(); ++i){
		len += *i;
	}
	return len;
}

void printMsgBytes(zmq::message_t *data){
	unsigned char arr[data->size()];
	memcpy(arr, data->data(), data->size());

	cout << "Printing Message (" << data->size() << "): " << endl;
	for(int i = 0; i < data->size(); i++){
		printf("%x ", arr[i]);
	}
	cout << endl;
}
void printStructBytes(imgdata_t *data){
	unsigned char arr[sizeof(imgdata_t)];
	memcpy(arr, data, sizeof(imgdata_t));

	cout << "Printing Struct (" << sizeof(imgdata_t) << "): " << endl;
	for(int i = 0; i < sizeof(imgdata_t); i++){
		printf("%x ", arr[i]);
	}
	cout << endl;
}

unsigned char *linearizeData(imgdata_t *data, int *retlen){
	int start = 0;

	*retlen = messageSizeNeeded(data);

	unsigned char *arr = new unsigned char[*retlen];

	memcpy(arr, data, sizeof(imgdata_t));
	start += sizeof(imgdata_t);

	if(data->image_data_size){
		memcpy(arr + start, &(*(data->image_data->at(0)))[0], data->image_data_size);
		start += data->image_data_size;
	}

	if(data->sseg_image_size_count){
		memcpy(arr + start, &(*(data->sseg_image_sizes))[0], data->sseg_image_size_count * sizeof(uint32_t));
		start += data->sseg_image_size_count * sizeof(uint32_t);
	}

	if(data->cseg_image_size_count){
		memcpy(arr + start, &(*(data->cseg_image_sizes))[0], data->cseg_image_size_count * sizeof(uint32_t));
		start += data->cseg_image_size_count * sizeof(uint32_t);
	}

	for(std::vector<std::vector<unsigned char>*>::iterator i = data->sseg_image_data->begin();
			i < data->sseg_image_data->end(); ++i){
		memcpy(arr + start, &(*(*i))[0], (*i)->size());
		start += (*i)->size();
	}

	for(std::vector<std::vector<unsigned char>*>::iterator i = data->cseg_image_data->begin();
			i < data->cseg_image_data->end(); ++i){
		memcpy(arr + start, &(*(*i))[0], (*i)->size());
		start += (*i)->size();
	}
	return arr;
}

void packMessageData(zmq::message_t *msg, imgdata_t *data){
	int len = 0;
	unsigned char *arr = linearizeData(data, &len);
	memcpy((unsigned char*)msg->data(), arr, len);
	delete [] arr;
}

void expandData(imgdata_t *data, unsigned char *arr){
	if(arr == 0){
		data->image_data = new std::vector<std::vector<unsigned char>*>();
		data->sseg_image_data = new std::vector<std::vector<unsigned char>*>();
		data->cseg_image_data = new std::vector<std::vector<unsigned char>*>();
		data->sseg_image_sizes = new std::vector<uint32_t>();
		data->cseg_image_sizes = new std::vector<uint32_t>();
		return;
	}
	int start = 0;

	memcpy(data, arr, sizeof(imgdata_t));
	start += sizeof(imgdata_t);

	data->image_data = new std::vector<std::vector<unsigned char>*>();
	data->sseg_image_data = new std::vector<std::vector<unsigned char>*>();
	data->cseg_image_data = new std::vector<std::vector<unsigned char>*>();

	unsigned char img_arr[data->image_data_size];
	memcpy(img_arr, arr + start, data->image_data_size);
	start += data->image_data_size;
	if(data->image_data_size){
		data->image_data->push_back(new std::vector<unsigned char>(img_arr, img_arr + data->image_data_size));
	}

	uint32_t sseg_image_sizes_arr[data->sseg_image_size_count];
	memcpy(sseg_image_sizes_arr, arr+start, data->sseg_image_size_count * sizeof(uint32_t));
	start += data->sseg_image_size_count * sizeof(uint32_t);
	data->sseg_image_sizes = new std::vector<uint32_t>(sseg_image_sizes_arr, sseg_image_sizes_arr + data->sseg_image_size_count);

	uint32_t cseg_image_sizes_arr[data->cseg_image_size_count];
	memcpy(cseg_image_sizes_arr, arr+start, data->cseg_image_size_count * sizeof(uint32_t));
	start += data->cseg_image_size_count * sizeof(uint32_t);
	data->cseg_image_sizes = new std::vector<uint32_t>(cseg_image_sizes_arr, cseg_image_sizes_arr + data->cseg_image_size_count);

	for(std::vector<uint32_t>::iterator i = data->sseg_image_sizes->begin();
			i < data->sseg_image_sizes->end(); ++i){
		unsigned char sseg_image_arr[*i];
		memcpy(sseg_image_arr, arr+start, *i);
		start += *i; 
		data->sseg_image_data->push_back(new std::vector<unsigned char>(sseg_image_arr, sseg_image_arr + *i));
	}

	for(std::vector<uint32_t>::iterator i = data->cseg_image_sizes->begin();
			i < data->cseg_image_sizes->end(); ++i){
		unsigned char cseg_image_arr[*i];
		memcpy(cseg_image_arr, arr+start, *i);
		start += *i; 
		data->cseg_image_data->push_back(new std::vector<unsigned char>(cseg_image_arr, cseg_image_arr + *i));
	}
}

void unpackMessageData(imgdata_t *data, zmq::message_t *msg){
	expandData(data, (unsigned char*)msg->data());
}
