#include <string>
#include <iostream>

using std::cout;
using std::endl;

#include "Recognition/Segmentation.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/AUVSI_Algorithm.hpp"
#include "Backbone/IMGData.hpp"
//#include "Backbone/Algs.hpp"


void Segmentation :: execute(imgdata_t &data){

        Mat binary;
        Mat image;
        Mat fg;
        Mat bg;
        Mat result;
      

        if(data.orthorectDone == false &&
          data.georefDone == false &&
          data.saliencyDone == false){

          cout <<"Error, Image pre-processing not finished"
                 " before passing to segmentation\n" <<endl;
          return;
        }
      
        image = data.image_data; 

        if(image.empty()){cout <<"image is null"<<endl; return;} 

        cvtColor(image, binary, CV_BGR2GRAY);
        threshold(binary, binary, 100, 255, THRESH_BINARY);
        /////////////////
        erode(binary, fg, Mat(), Point(-1,-1),3);

        //////////////////
        dilate(binary,bg, Mat(), Point(-1,-1),3);
        threshold(bg,bg,1,128, THRESH_BINARY_INV);

        ////////////////
        Mat markers(binary.size(), CV_8U, Scalar(0));
        markers = fg + bg;
        ///////////////
        Segmenter seg;
        seg.setMarkers(markers);

        result = seg.process(image);
        result.convertTo(result,CV_8U);

        imshow("result", result);
        waitKey(8000);
        setDone(data, CSEG);
	setDone(data, SSEG);
}

void Segmentation :: help(){
  cout << "This program demonstrates segmentation\n"<<endl;
}




