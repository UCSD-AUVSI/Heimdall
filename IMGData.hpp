#ifndef IMGDATA_H
#define IMGDATA_H

struct imgdata_t{
	int id;
	bool orthorectDone = false;
	bool georefDone = false;
	bool saliencyDone = false;
	bool ssegDone = false;
	bool csegDone = false;
	bool sDone = false;
	bool cDone = false;
	bool verified = false;
};

void setDone(imgdata_t &data, alg_t alg);

#endif
