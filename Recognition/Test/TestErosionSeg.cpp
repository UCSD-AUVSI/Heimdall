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
    
	cv::imencode(".jpg", cv::imread(filename), t.image_data);

    setDone(&t,ORGR);
    setDone(&t,SALIENCY);

	ErosionSeg::execute(&t);

}
