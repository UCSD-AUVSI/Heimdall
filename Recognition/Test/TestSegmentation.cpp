#include <iostream>

#include "Recognition/Segmentation.hpp"

using std::cout;
using std::endl;


string filename = "./foo.jpg";

int main( int argc, char** argv )
{
    cout << "Test Segmentation" << endl;

    imgdata_t t;
    t.image_data = imread(filename);

    setDone(t,ORTHORECT);
    setDone(t,GEOREF);
    setDone(t,SALIENCY);

    Segmentation seg;
    seg.execute(t);

}
