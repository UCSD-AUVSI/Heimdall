#ifndef ____SEGMENTATION_SETTINGS_H_____
#define ____SEGMENTATION_SETTINGS_H_____


class Segmenter_Module_Settings
{
public:
	float HistSeg_COLOR_DISTANCE_THRESHOLD;
	float HistSeg_COLOR_DISTANCE_TO_PREVIOUSLYFOUND_THRESHOLD;
	float HistSeg_MERGE_COLOR_DISTANCE;
	int   HistSeg_NUM_VALID_COLORS;
	int   HistSeg_NUM_BINS;
	int   HistSeg_BLUR_PREPROCESS_RADIUS_PIXELS;
	float HistSeg_MINIMUM_BLOB_SIZE_THRESHOLD;
	float HistSeg_PERCENT_TOUCHING_EDGES_ACCEPTABLE;
	bool  HistSeg_FILL_IN_SHAPE_BLOB_BEFORE_RETURNING;

	int  preprocess_CV_conversion_type;
	bool preprocess_channels_to_keep[3];

//----

	Segmenter_Module_Settings() :
				HistSeg_COLOR_DISTANCE_THRESHOLD(100.0f),
				HistSeg_COLOR_DISTANCE_TO_PREVIOUSLYFOUND_THRESHOLD(50.0f),
				HistSeg_MERGE_COLOR_DISTANCE(30.0f),
				HistSeg_NUM_VALID_COLORS(8),
				HistSeg_NUM_BINS(10),
				HistSeg_BLUR_PREPROCESS_RADIUS_PIXELS(1),
				HistSeg_MINIMUM_BLOB_SIZE_THRESHOLD(0.015),
				HistSeg_FILL_IN_SHAPE_BLOB_BEFORE_RETURNING(true),


				//what % touching the edge is acceptable?
                //for reference, a rectangle/square with one side along the edge is on the order of 1% (0.01)
                //
				HistSeg_PERCENT_TOUCHING_EDGES_ACCEPTABLE(0.003f),
				preprocess_CV_conversion_type(-1) //not a valid conversion
				{
					preprocess_channels_to_keep[0] = true;
					preprocess_channels_to_keep[1] = true;
					preprocess_channels_to_keep[2] = true;
				}
};

#endif
