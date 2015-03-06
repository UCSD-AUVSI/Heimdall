#include <iostream>

using std::cout;
using std::endl;

#include "colorclassifier2014.hpp"
#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"
#include "SharedUtils/SharedUtils.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

//expects values to be normalized to the range 0 to 255
static std::string ConvertColorToString2014(float r, float g, float b);


void ColorClassifier2014 :: execute(imgdata_t *imdata, std::string args) {
    imdata->scolor = ConvertColorToString2014(imdata->scolorR, imdata->scolorG, imdata->scolorB);
    imdata->ccolor = ConvertColorToString2014(imdata->ccolorR, imdata->ccolorG, imdata->ccolorB);
	cout << "color classification 2014 done" << endl;	
	setDone(imdata, COLORCLASS);
}


#define Print3ElementVectorCol(INPVEC) std::string("(")<<to_sstring(INPVEC[0])<<std::string(",")<<to_sstring(INPVEC[1])<<std::string(",")<<to_sstring(INPVEC[2])<<std::string(")")


//NOTE: expects input colors to be normalized as 0-255
//it won't work well otherwise!
std::string ConvertColorToString2014(float r, float g, float b)
{
	std::vector<float> given_color_in_CIElab = ConvertOneColorPoint(r,g,b,CV_BGR2Lab);
	
    std::map<std::string, std::vector<float>> color_map = {
        {"Black",		{0,     0,     0    }},
        //{"Gray",		{127.5, 127.5, 127.5}},
        {"White",		{255,   255,   255  }},

        {"Red",			{200, 0,   0  }},
        {"Red",			{200, 20, 20  }},
        {"Red",			{255, 0,   0  }},
        {"Red",			{255, 20, 20  }},
        {"Dark Red",	{120, 0,   0  }},
        {"Dark Red",	{120, 20, 20  }},
        
        {"Green",		{0,   255, 0  }},
        {"Green",		{20,  255, 20 }},
        {"Green",		{0,   180, 0  }},
        {"Green",		{20,  180, 20 }},
        {"Dark Green",	{0,   120, 0  }},
        {"Dark Green",	{20,   120, 20  }},
        
        {"Blue",		{0,   0,   255}},
        {"Blue",		{20, 20,   255}},
        {"Blue",		{20, 20,   200}},
        {"Blue",		{0,   0,   200}},
        {"Dark Blue",	{0,   0,   120}},
        {"Dark Blue",	{20, 20,   120}},

        {"Yellow",		{200, 200, 20 }},
        {"Yellow",		{200, 200, 0  }},
        {"Yellow",		{255, 255, 20 }},
        {"Yellow",		{255, 255, 0  }},
        
        {"Fuchsia",		{255, 0,   255}},
        {"Aqua",		{0,   255, 255}},

        {"Orange",		{255,   127.5, 0    }},
        {"Orange",		{207,   110,  16    }},
        {"Orange",		{237,   142,  50    }},
        
        //{"Maroon",		{127.5, 0,     0    }}, //renamed "dark red"
        {"Purple",		{127.5, 0,     127.5}},
        {"Olive",		{122,   122,   0    }},
        {"Teal",		{0,     127.5, 127.5}},
    };
    
    //-----------------------------------------------
    //-----------------------------------------------
    std::map<std::string, std::vector<float>>::iterator red_iter;
    red_iter = color_map.begin();
    red_iter++;
    red_iter++;
    std::vector<float> ref_color__red = ConvertOneColorPoint(red_iter->second[0],
															 red_iter->second[1],
															 red_iter->second[2],
															 CV_BGR2Lab);
	
	consoleOutput.Level3() << "red (255,0,0) in CIELab == " << Print3ElementVectorCol(ref_color__red) <<std::endl;
	consoleOutput.Level3() << "given input color in CIELab == " << Print3ElementVectorCol(given_color_in_CIElab) <<std::endl;
    //-----------------------------------------------
    //-----------------------------------------------
															 
    
    
    float distance = 100000.0f;
    std::string color = "";

    for(auto &x: color_map){
		std::vector<float> ref_color_in_CIELab = ConvertOneColorPoint(x.second[0],
																	  x.second[1],
																	  x.second[2],
																	  CV_BGR2Lab);
        float curr_dist = sqrt( 
                pow(ref_color_in_CIELab[0] - given_color_in_CIElab[0], 2) +
                pow(ref_color_in_CIELab[1] - given_color_in_CIElab[1], 2) +
                pow(ref_color_in_CIELab[2] - given_color_in_CIElab[2], 2));

        if (curr_dist < distance) {
            distance = curr_dist;
            color = x.first;
        }
    }
    
    //consoleOutput.Level3() << "@@@@@@@@@@@@@@@@@@@@@@@ color given to color namer: " << std::endl << given_color_in_CIElab << std::endl;
    //consoleOutput.Level3() << to_sstring(given_color_in_CIElab.at<MyPixelColorType>(0,0)[0]) << "," << to_sstring(given_color_in_CIElab.at<MyPixelColorType>(0,0)[1]) << "," << to_sstring(given_color_in_CIElab.at<MyPixelColorType>(0,0)[2]) << std::endl;
    //consoleOutput.Level3() << "and said it was " << color << std::endl << "@@@@@@@@@@@@@@@@@@@@@@@" << std::endl;

    return color;
}


 /*
    //these are RGB
    std::map<std::string, std::vector<double>> color_map = {
        {"Black",   {0, 0, 0}},
        {"White",   {255, 255, 255}},

        {"Red",     {255, 0, 0}},
        {"Green",   {0, 255, 0}},
        {"Blue",    {0, 0, 255}},

        {"Yellow",  {255, 255, 0}},
        {"Fuchsia", {255, 0, 255}},
        {"Aqua",    {0, 255, 255}},

        {"Orange",  {255, 128, 0}},
        {"Maroon",  {128, 0, 0}},
        {"Purple",  {128, 0, 128}},
        {"Olive",   {128, 128, 0}},
        {"Teal",    {0, 128, 128}},
    };
    //below are CIELab
        {"Black",	{0, 0, 0}},
        {"Gray",	{50, 0, 0}},
        {"White",	{100, 0, 0}},

        {"Red",		{53.2406, 80.0942, 67.2015}},
        {"Green",	{87.7351, -86.1813, 83.1775}},
        {"Blue",	{32.2957, 79.187, -107.862}},

        {"Yellow",	{97.1395, -21.5524, 94.4758}},
        {"Fuchsia",	{60.3235, 98.2352, -60.8255}},
        {"Aqua",	{91.1133, -48.0886, -14.131}},

        {"Orange",	{66.9565, 43.0733, 73.9576}},
        {"Maroon",	{25.4184, 47.9108, 37.9052}},
        {"Purple",	{29.6552, 58.7624, -36.3846}},
        {"Olive",	{51.6779, -12.8922, 56.5136}},
        {"Teal",	{48.0731, -28.7656, -8.45287}},
    */
    
    
//------------------------------------------------
