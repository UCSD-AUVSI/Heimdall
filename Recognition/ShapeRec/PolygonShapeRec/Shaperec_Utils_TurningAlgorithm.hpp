#ifndef __SIM2_H___
#define __SIM2_H___

class ComparisonReturnedTable
{
public:
	double metric;
	double relative_angle_to_reference;
	int tr_ii;
	int tr_ff;
	double hht0_err;
	double sslope_err;
	
	ComparisonReturnedTable();
	ComparisonReturnedTable(double aaa, double bbb, int ccc, int ddd, double eee, double fff);
	void PrintMe();
};

ComparisonReturnedTable CompareCVPolygons(std::vector<cv::Point>& poly_reference, std::vector<cv::Point>& poly_target);

#endif
