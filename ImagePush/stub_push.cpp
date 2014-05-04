#include <string>
#include <iostream>
#include <thread>

#include "ImagePush/stub_push.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"

using std::cout;
using std::endl;

bool StubPush :: pause = false;

void StubPush :: execute(imgdata_t *imdata, std::string args){
    if(StubPush::pause){
        std::chrono::milliseconds dura(1000);
        std::this_thread::sleep_for(dura);
    }
    else{
        StubPush::pause = true;
    }
    std::vector<unsigned char> *newarr = new std::vector<unsigned char>();
    newarr->push_back('A');
    imdata->image_data->push_back(newarr);
    
    cout << "Stub Push" << endl;
}
