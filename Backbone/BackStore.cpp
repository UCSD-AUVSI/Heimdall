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

static std::map<std::pair<int,int>, unsigned char *> store;

bool img_check(imgdata_t *data){
	//all of these must be done in order to continue to verification
    return (data->sDone == data->cDone && data->cDone == data->cclDone);
}

void saveLargerString(std::string &str1, std::string &str2){
    if(str1.size() > str2.size()){
        str2 = str1;
    }
    else{
        str1 = str2;
    }
}
   
void orUpdate(imgdata_t *first, imgdata_t *second){
    first->orthorectDone    |= second->orthorectDone;
    first->saliencyDone     |= second->saliencyDone;
    first->segDone          |= second->segDone;
    first->qrcDone          |= second->qrcDone;
    first->sDone            |= second->sDone;
    first->cDone            |= second->cDone;
    first->cclDone          |= second->cclDone;
    first->verified         |= second->verified;

    saveLargerString(first->shape, second->shape);
    saveLargerString(first->character1, second->character1);
    saveLargerString(first->character2, second->character2);
    saveLargerString(first->scolor, second->scolor);
    saveLargerString(first->ccolor, second->ccolor);
    saveLargerString(first->qrCodeMessage, second->qrCodeMessage);
    saveLargerString(first->name_of_original_image_file_for_debugging, second->name_of_original_image_file_for_debugging);
}

// TODO: Currently, BackStore does not save image data properly
// Need to properly and intelligently save data
void updateImageData(imgdata_t *previmg, imgdata_t *newimg){}

bool img_update(imgdata_t *data){
    store_lock.lock();
    std::pair<int,int> datakey(data->id, data->cropid);
    unsigned char *&imgarr = store[datakey];
        
    if(!data->initialized){
        initEmptyIMGData(data);
    }

    imgdata_t img;
    if(store.find(datakey) != store.end()){
        expandData(&img, imgarr);
    }
    else{
        initEmptyIMGData(&img);
    }
    
    if(img.id != data->id){
        img.id = data->id;
    }

    orUpdate(&img, data);
    orUpdate(data, &img);

    updateImageData(&img, data);

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
    std::pair<int,int> datakey(data->id, data->cropid);
    try {
        delete [] store.at(datakey);
    }
    catch(const std::out_of_range& oor){
        return false;
    }
    return (store.erase(datakey)==1);
}
