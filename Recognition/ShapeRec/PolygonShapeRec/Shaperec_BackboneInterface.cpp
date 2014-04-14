#include <iostream>
#include "Backbone/Backbone.hpp"
#include "Backbone/IMGData.hpp"
#include <opencv2/highgui/highgui.hpp>
#include "SharedUtils/SharedUtils.hpp"
#include "Shaperec_BackboneInterface.hpp"
#include "Shaperec_ModuleMain.hpp"

//declared in Shaperec_BackboneInterface.hpp
/*extern*/ ShapeRecModule_Main* global_PolygonShapeRec_instance = nullptr;

void PolygonShapeRec::execute(imgdata_t *imdata) {
	std::cout << "Polygon Shape Rec" << std::endl;
	
	if(global_PolygonShapeRec_instance == nullptr) {
		global_PolygonShapeRec_instance = new ShapeRecModule_Main("./shaperec_reference_shapes");
	}
	
	if(imdata->sseg_image_data->empty() == false)
	{
		std::vector<cv::Mat> given_SSEGs;
		
		std::vector<std::vector<unsigned char>*>::iterator sseg_iter;
		for(sseg_iter = imdata->sseg_image_data->begin();
			sseg_iter != imdata->sseg_image_data->end();
			sseg_iter++)
		{
			given_SSEGs.push_back(cv::imdecode(**sseg_iter, CV_LOAD_IMAGE_ANYDEPTH));
		}
		
		global_PolygonShapeRec_instance->DoModule(&given_SSEGs);
		
		if(global_PolygonShapeRec_instance->last_obtained_results.empty() == false) {
			
			ShapeRec_Result &bestresult = (*global_PolygonShapeRec_instance->last_obtained_results.results.begin());
			
			imdata->shape = bestresult.reference_shape_name;
			
			//Shaperec is not a good one to determine orientation by, because most of the shapes aren't unambiguously orientable...
			//OCR should be the one determining orientation
			//imdata->targetorientation = (imdata->planeheading + bestresult.relative_angle_to_reference);
			
			std::cout << "ShapeRec found the shape to be \'" << imdata->shape << std::string("\'") << std::endl;
		}
		else {
			std::cout << "ShapeRec said there was no shape, or it didn't recognize it." << std::endl;
		}
	}
	else {
		std::cout << "ShapeRec wasn't given any SSEGs!" << std::endl;
	}
	
	setDone(imdata, POLYGON_SREC);
}
