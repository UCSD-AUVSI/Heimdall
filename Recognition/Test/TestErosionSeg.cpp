#include <iostream>

#include "Recognition/Segmentation/ErosionSeg.hpp"
#include "Backbone/IMGData.hpp"

using std::cout;
using std::endl;

std::string filename = "./foo.jpg";

int main( int argc, char** argv )
{
    cout << "Test Segmentation" << endl;

    imgdata_t t;
    
	t.image_data = new std::vector<unsigned char>();
	cv::imencode(".jpg", cv::imread(filename), *(t.image_data));

    setDone(&t,STUB_ORGR);
    setDone(&t,STUB_SALIENCY);

	ErosionSeg::execute(&t);

}
