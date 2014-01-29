#include <iostream>
#include <vector>

#include <zmq.hpp>

#include "Backbone/MessageHandling.hpp"
#include "Backbone/IMGData.hpp"
#include "Backbone/Backbone.hpp"

using std::cout;
using std::endl;

int messageSizeNeeded(imgdata_t *data){
	int len = sizeof(imgdata_t) 
		+ data->image_data->size();
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
	
	data->image_data_size = data->image_data->size();
	
	*retlen = messageSizeNeeded(data);
	unsigned char *arr = new unsigned char[*retlen];

	memcpy(arr, data, sizeof(imgdata_t));
	start += sizeof(imgdata_t);
	
	memcpy(arr + start, &(*(data->image_data))[0], data->image_data_size);
	start += data->image_data_size;
	
	return arr;
}

void packMessageData(zmq::message_t *msg, imgdata_t *data){
	int len = 0;
	unsigned char *arr = linearizeData(data, &len);
	memcpy(msg->data(), arr, len);
	delete [] arr;
}

void expandData(imgdata_t *data, unsigned char *arr){
	if(arr == 0){
		data->image_data = new std::vector<unsigned char>();
		return;
	}
	
	int start = 0;
	
	memcpy(data, arr, sizeof(imgdata_t));
	start += sizeof(imgdata_t);

	unsigned char img_arr[data->image_data_size];
	
	memcpy(img_arr, arr + start, data->image_data_size);
	start += data->image_data_size;
	
	data->image_data = new std::vector<unsigned char>(img_arr, img_arr + data->image_data_size);
}

void unpackMessageData(imgdata_t *data, zmq::message_t *msg){
	expandData(data, (unsigned char*)msg->data());
}
