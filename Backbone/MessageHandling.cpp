#include <iostream>
#include <vector>
#include <zmq.hpp>
#include <cstring>

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
	
	//the +1 in these is to make space for the null character \0 that terminates the string
	len += strlen(imdata->shape.c_str()) + 1;
	len += strlen(imdata->character.c_str()) + 1;
	len += strlen(imdata->scolor.c_str()) + 1;
	len += strlen(imdata->ccolor.c_str()) + 1;

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

void printMsgBytesInfo(zmq::message_t *msg){
	cout << "printMsgBytesInfo - size of msg (bytes): " << msg->size() << endl;
}

void printStructBytesInfo(imgdata_t *imdata){
	cout << "printStructBytesInfo - sizeof imgdata_t (bytes): " << sizeof(imgdata_t) << endl;
	cout << "printStructBytesInfo - estimated size needed in this struct: " << messageSizeNeeded(imdata) << endl;
}

void printMsgBytes(zmq::message_t *msg){
	std::vector<unsigned char> arr(msg->size(),0);
	memcpy(&arr[0], msg->data(), msg->size());

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

#define MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(DATAMEMBER) \
	memcpy(arr+start, &(imdata -> DATAMEMBER), sizeof(imdata -> DATAMEMBER)); \
	start += sizeof(imdata -> DATAMEMBER);
	
#define MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(DATAMEMBER) \
	memcpy(&(imdata -> DATAMEMBER), arr+start, sizeof(imdata -> DATAMEMBER)); \
	start += sizeof(imdata -> DATAMEMBER);
	

unsigned char *linearizeData(imgdata_t *imdata, int *retlen){
	int start = 0;

	*retlen = messageSizeNeeded(imdata);
	unsigned char *arr = new unsigned char[*retlen];

	//memcpy(arr, imdata, sizeof(imgdata_t));
	//start += sizeof(imgdata_t);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(id);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(cropid);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(initialized);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(image_data_size);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(sseg_image_size_count);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(cseg_image_size_count);
	
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(orgrDone);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(saliencyDone);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(segDone);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(sDone);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(cDone);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(verified);
	
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(planelat);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(planelongt);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(planealt);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(planeheading);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(targetlat);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(targetlongt);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(targetorientation);
	
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(internal_num_of_saved_cseg_and_sseg);
		
	
	//the +1 in these is the null character \0 that terminates the string
	memcpy(arr + start, imdata->shape.c_str(), strlen(imdata->shape.c_str()) + 1);
	start += (strlen(imdata->shape.c_str()) + 1);
	
	memcpy(arr + start, imdata->character.c_str(), strlen(imdata->character.c_str()) + 1);
	start += (strlen(imdata->character.c_str()) + 1);
	
	memcpy(arr + start, imdata->scolor.c_str(), strlen(imdata->scolor.c_str()) + 1);
	start += (strlen(imdata->scolor.c_str()) + 1);
	
	memcpy(arr + start, imdata->ccolor.c_str(), strlen(imdata->ccolor.c_str()) + 1);
	start += (strlen(imdata->ccolor.c_str()) + 1);
	

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

	//memcpy(imdata, arr, sizeof(imgdata_t));
	//start += sizeof(imgdata_t);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(id);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(cropid);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(initialized);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(image_data_size);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(sseg_image_size_count);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(cseg_image_size_count);
	
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(orgrDone);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(saliencyDone);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(segDone);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(sDone);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(cDone);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(verified);
	
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(planelat);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(planelongt);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(planealt);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(planeheading);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(targetlat);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(targetlongt);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(targetorientation);
	
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(internal_num_of_saved_cseg_and_sseg);
	
	
	//the +1 in these is the null character \0 that terminated the string
	imdata->shape = std::string(reinterpret_cast<char*>(arr+start));
	start += (imdata->shape.size() + 1);
	imdata->character = std::string(reinterpret_cast<char*>(arr+start));
	start += (imdata->character.size() + 1);
	imdata->scolor = std::string(reinterpret_cast<char*>(arr+start));
	start += (imdata->scolor.size() + 1);
	imdata->ccolor = std::string(reinterpret_cast<char*>(arr+start));
	start += (imdata->ccolor.size() + 1);

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
