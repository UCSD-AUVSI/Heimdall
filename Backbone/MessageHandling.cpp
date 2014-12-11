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
    imdata->image_data_size = imdata->image_data->size();

    imdata->sseg_image_sizes->clear();
    for(std::vector<std::vector<unsigned char>*>::iterator i = imdata->sseg_image_data->begin();
            i < imdata->sseg_image_data->end(); ++i){
        imdata->sseg_image_sizes->push_back((*i)->size());
    }
    imdata->sseg_image_size_count = imdata->sseg_image_sizes->size();

    imdata->cseg_image_sizes->clear();
    for(std::vector<std::vector<unsigned char>*>::iterator i = imdata->cseg_image_data->begin();
            i < imdata->cseg_image_data->end(); ++i){
        imdata->cseg_image_sizes->push_back((*i)->size());
    }
    imdata->cseg_image_size_count = imdata->cseg_image_sizes->size();
}

int messageSizeNeeded(imgdata_t *imdata){
    if(!imdata->initialized){
        return 0;
    }

    populateSizes(imdata);

    // This is the size of ALL elements of imgdata_t,
    // including non-POD (which are not copied over with
    // the regular struct elements). We are probably wasting
    // a small amount of space in arrays with this, but
    // at this point it's little more than a won't-fix TODO
	int len = sizeof(imgdata_t);
	
	//the +1 in these is to make space for the null character \0 that terminates the string
	len += imdata->shape.size() + 1;
	len += imdata->character.size() + 1;
	len += imdata->scolor.size() + 1;
	len += imdata->ccolor.size() + 1;
	len += imdata->name_of_original_image_file_for_debugging.size() + 1;

	len += imdata->image_data_size;
	len += imdata->sseg_image_size_count * sizeof(uint32_t);
	len += imdata->cseg_image_size_count * sizeof(uint32_t);

    for(uint32_t imlen : *imdata->sseg_image_sizes){
        len += imlen;
    }
    for(uint32_t imlen : *imdata->cseg_image_sizes){
        len += imlen;
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

// Copy POD (Plain Old Data) elements of IMGData into array
void copyPODToArr(imgdata_t *imdata, unsigned char *arr, int &start){
    MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(id);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(cropid);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(initialized);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(image_data_size);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(sseg_image_size_count);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(cseg_image_size_count);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(num_crops_in_this_image);
	
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(orthorectDone);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(saliencyDone);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(segDone);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(sDone);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(cDone);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(cclDone);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(verified);
	
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(scolorR);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(scolorG);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(scolorB);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(ccolorR);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(ccolorG);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(ccolorB);
	
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(planelat);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(planelongt);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(planealt);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(planeroll);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(planepitch);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(planeheading);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(targetlat);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(targetlongt);
	MEMCPY_PACK_IMDATA_DATA_MEMBER_INTO_ARR(targetorientation);

    // We allocated sizeof(imgdata_t) for the struct, 
    // and that is != sizeof(imgdata_t POD), so advance
    // start until we get to where we want it
    start = sizeof(imgdata_t);
}

// Copy POD (Plain Old Data) elements of IMGData from array
void copyPODFromArray(imgdata_t *imdata, unsigned char *arr, int &start){
    MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(id);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(cropid);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(initialized);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(image_data_size);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(sseg_image_size_count);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(cseg_image_size_count);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(num_crops_in_this_image);
	
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(orthorectDone);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(saliencyDone);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(segDone);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(sDone);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(cDone);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(cclDone);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(verified);
	
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(scolorR);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(scolorG);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(scolorB);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(ccolorR);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(ccolorG);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(ccolorB);
	
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(planelat);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(planelongt);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(planealt);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(planeroll);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(planepitch);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(planeheading);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(targetlat);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(targetlongt);
	MEMCPY_READ_IMDATA_DATA_MEMBER_FROM_ARR(targetorientation);

    // We allocated sizeof(imgdata_t) for the struct, 
    // and that is != sizeof(imgdata_t POD), so advance
    // start until we get to where we want it
    start = sizeof(imgdata_t);
}

unsigned char *linearizeData(imgdata_t *imdata, int *retlen){
    int start = 0;

    *retlen = messageSizeNeeded(imdata);
    unsigned char *arr = new unsigned char[*retlen];

    if(!imdata->initialized){
        return arr;
    }

    copyPODToArr(imdata, arr, start);

	//the +1 in these is the null character \0 that terminates the string
	memcpy(arr + start, imdata->shape.c_str(), imdata->shape.size() + 1);
	start += imdata->shape.size() + 1;
	
	memcpy(arr + start, imdata->character.c_str(), imdata->character.size() + 1);
	start += imdata->character.size() + 1;
	
    memcpy(arr + start, imdata->scolor.c_str(), imdata->scolor.size() + 1);
	start += imdata->scolor.size() + 1;
	
	memcpy(arr + start, imdata->ccolor.c_str(), imdata->ccolor.size() + 1);
	start += imdata->ccolor.size() + 1;
	
	memcpy(arr + start, imdata->name_of_original_image_file_for_debugging.c_str(), imdata->name_of_original_image_file_for_debugging.size() + 1);
	start += imdata->name_of_original_image_file_for_debugging.size() + 1;
	
    memcpy(arr + start, &(*(imdata->image_data))[0], imdata->image_data_size);
    start += imdata->image_data_size;

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

// Expand an array (typically from a ZMQ Message) into an imdata_t instance
void expandData(imgdata_t *imdata, unsigned char *arr){
    if(arr == 0){
        initEmptyIMGData(imdata);
        return;
    }
    else if(imdata->initialized){
        //If imdata already has stuff in it, we want to empty it
        //Only have to worry about pointers, POD will be overwritten
        clearIMGData(imdata);
        initEmptyIMGData(imdata);
    }
    else if(!imdata->initialized){
        initEmptyIMGData(imdata);
    }

    int start = 0;
    copyPODFromArray(imdata, arr, start);

    //Copy Strings
	//the +1 in these is the null character \0 that terminated the string
	imdata->shape = reinterpret_cast<char*>(arr+start);
	start += (imdata->shape.size() + 1);
	imdata->character = reinterpret_cast<char*>(arr+start);
	start += (imdata->character.size() + 1);
	imdata->scolor = reinterpret_cast<char*>(arr+start);
	start += (imdata->scolor.size() + 1);
	imdata->ccolor = reinterpret_cast<char*>(arr+start);
	start += (imdata->ccolor.size() + 1);
	imdata->name_of_original_image_file_for_debugging = reinterpret_cast<char*>(arr+start);
	start += (imdata->name_of_original_image_file_for_debugging.size() + 1);

    //Copy main image/image crop
    unsigned char *img_arr = new unsigned char[imdata->image_data_size];

    memcpy(img_arr, arr + start, imdata->image_data_size);
    start += imdata->image_data_size;

    imdata->image_data = new std::vector<unsigned char>(img_arr, img_arr + imdata->image_data_size);
    delete[] img_arr;

    //Copy size lists for sseg/cseg crops
    if(imdata->sseg_image_size_count){
        uint32_t sseg_image_sizes_arr[imdata->sseg_image_size_count];

        memcpy(sseg_image_sizes_arr, arr+start, imdata->sseg_image_size_count * sizeof(uint32_t));
        start += imdata->sseg_image_size_count * sizeof(uint32_t);

        imdata->sseg_image_sizes->insert(imdata->sseg_image_sizes->end(), &sseg_image_sizes_arr[0], &sseg_image_sizes_arr[imdata->sseg_image_size_count]);   
    }

    if(imdata->cseg_image_size_count){
        uint32_t cseg_image_sizes_arr[imdata->cseg_image_size_count];

        memcpy(cseg_image_sizes_arr, arr+start, imdata->cseg_image_size_count * sizeof(uint32_t));
        start += imdata->cseg_image_size_count * sizeof(uint32_t);
        
        imdata->cseg_image_sizes->insert(imdata->cseg_image_sizes->end(), &cseg_image_sizes_arr[0], &cseg_image_sizes_arr[imdata->cseg_image_size_count]);   
    }

    //Copy actual cseg/sseg crops
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

void packMessageData(zmq::message_t *msg, imgdata_t *imdata){
    int len = 0;
    unsigned char *arr = linearizeData(imdata, &len);
    memcpy((unsigned char*)msg->data(), arr, len);
    delete [] arr;
}

void unpackMessageData(imgdata_t *imdata, zmq::message_t *msg){
    expandData(imdata, (unsigned char*)msg->data());
}
