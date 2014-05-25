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
	float HistSeg_MINIMUM_SPECK_SIZE_THRESHOLD; //any piece of any blob with an individual area less than this is removed (is fraction of image size)
	float HistSeg_MINIMUM_BLOB_SIZE_THRESHOLD; //any blob with a combined area less than this is removed (is fraction of image size)
	float HistSeg_PERCENT_OF_CROP_EDGE_TOUCHED_ACCEPTABLE;
	float HistSeg_PERCENT_OF_BLOB_TOUCHING_EDGES_ACCEPTABLE;
	bool  HistSeg_FILL_IN_SHAPE_BLOB_BEFORE_RETURNING;

	int  preprocess_CV_conversion_type;
	bool preprocess_channels_to_keep[3];

//----

	Segmenter_Module_Settings() :
				HistSeg_COLOR_DISTANCE_THRESHOLD(100.0f),
				HistSeg_COLOR_DISTANCE_TO_PREVIOUSLYFOUND_THRESHOLD(50.0f),
				HistSeg_MERGE_COLOR_DISTANCE(30.0f),
				HistSeg_NUM_VALID_COLORS(10),
				HistSeg_NUM_BINS(10),
				HistSeg_BLUR_PREPROCESS_RADIUS_PIXELS(1),
				HistSeg_MINIMUM_SPECK_SIZE_THRESHOLD(0.001f), //fraction of image size
				HistSeg_MINIMUM_BLOB_SIZE_THRESHOLD(0.00000001f), //fraction of image size
				HistSeg_FILL_IN_SHAPE_BLOB_BEFORE_RETURNING(true),


				//what % of the edge can be touched by a blob?
				//one whole side would be about 25% which is 0.25
				HistSeg_PERCENT_OF_CROP_EDGE_TOUCHED_ACCEPTABLE(0.14f),
				//what is the acceptable % of the blob's pixels that can touch the edge?
				//for reference, a rectangle/square with one side along the edge is on the order of 1% (0.01)
				HistSeg_PERCENT_OF_BLOB_TOUCHING_EDGES_ACCEPTABLE(0.004f),
				preprocess_CV_conversion_type(-1) //not a valid conversion
				{
					preprocess_channels_to_keep[0] = true;
					preprocess_channels_to_keep[1] = true;
					preprocess_channels_to_keep[2] = true;
				}
};

#endif
