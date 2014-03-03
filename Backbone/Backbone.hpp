#ifndef BACKBONE_H
#define BACKBONE_H

//Server Ports
enum zmqport_t{
	NO_PORT = 0,
	IMAGES_PULL = 1820,
	IMAGES_PUSH,
	IMAGES_PUB,
	ORGR_PULL,
	ORGR_PUSH,
	SALIENCY_PULL,
	SALIENCY_PUSH,
	SALIENCY_PUB,
	SEG_PULL,
	CSEG_PUSH,
	SSEG_PUSH,
	TARGET_PULL,
	TARGET_PUSH,
	TARGET_PUB,
	VERIFIED_PULL,
	VERIFIED_PUB
};

//Used to refer to various algorithms
enum alg_t{
	NONE,

	//ORGR
	STUB_ORGR,

	//Saliency
	SSALIENCY,
	STUB_SALIENCY,
	GUISAL,

	//Segmentation
	BLOB_SEG,
	GUIREC,

	//Shape Recognition
	TEMPLATE_SREC,

	//OCR
	TESS_OCR,

	//Verification
	STUB_VERIF
};

//Used to refer to classes of algorithms
enum algclass_t{
	ORGR,
	SALIENCY,
	SEG,
	SREC,
	OCR,
	VERIF
};

#endif
