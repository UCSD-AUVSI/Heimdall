#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"
#include "Backbone/MessageHandling.hpp"

#include <iostream>

using std::cout;
using std::endl;

//TODO: Change according to change in boolean mapping (see IMGData.cpp)

void setDone(imgdata_t *data, AlgClass alg){
    switch(alg){
        case ORTHORECT:     data->orthorectDone = true;     break;
        case SALIENCY:      data->saliencyDone = true;      break;
        case SEG:           data->segDone = true;           break;
        case SREC:          data->sDone = true;             break;
        case OCR:           data->cDone = true;             break;
        case VERIF:         data->verified = true;          break;
    }   
}

void img_print(imgdata_t* data){
    cout << "ID: "              << data->id << endl;
    cout << "ORTHORECTD: "      << data->orthorectDone << endl;
    cout << "SALD: "            << data->saliencyDone << endl;
    cout << "SEGD: "            << data->segDone << endl;
    cout << "SD: "              << data->sDone << endl;
    cout << "CD: "              << data->cDone << endl;
    cout << "VER: "             << data->verified << endl << endl;
}

// TODO: Add consequences if imgdata_t is not initialized
void initEmptyIMGData(imgdata_t *data){
    data->image_data =          new std::vector<unsigned char>();
    data->sseg_image_data =     new std::vector<std::vector<unsigned char>*>();
    data->cseg_image_data =     new std::vector<std::vector<unsigned char>*>();
    data->sseg_image_sizes =    new std::vector<uint32_t>();
    data->cseg_image_sizes =    new std::vector<uint32_t>();
    data->initialized = true;
}

// Clears all of the data in the struct that will not be properly freed
// automatically (so, the pointers)
void clearIMGData(imgdata_t *data){
    if(data->initialized = false){
        cout << "Data not initialized before clear!" << endl;
        return;
    }

    if(data->image_data){
        data->image_data->clear();
        delete data->image_data;
    }

    if(data->sseg_image_data){
        for(std::vector<std::vector<unsigned char>*>::iterator i = data->sseg_image_data->begin();
                i < data->sseg_image_data->end(); ++i){
            (*i)->clear();
            delete *i;
        }
        data->sseg_image_data->clear();
        delete data->sseg_image_data;
    }

    if(data->cseg_image_data){
        for(std::vector<std::vector<unsigned char>*>::iterator i = data->cseg_image_data->begin();
                i < data->cseg_image_data->end(); ++i){
            (*i)->clear();
            delete *i;
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
    data->initialized = false;
}

void copyIMGData(imgdata_t *dest, imgdata_t *src){
    int retlen = 0;
    unsigned char *src_arr = linearizeData(src, &retlen);
    expandData(dest, src_arr);
}
