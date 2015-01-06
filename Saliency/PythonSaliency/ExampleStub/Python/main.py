import numpy as np
import cv2


def doSaliency(fullsizeImg, optionalArgs):
	
	imheight,imwidth = fullsizeImg.shape[:2]
	print "python-saliency is processing an image of size: " + str(imwidth) + "x" + str(imheight)
	
	#---------------------------------------------------------------------------------------------------------
	#	Placeholder code (stub) instead of a real saliency algorithm,
	#	because this is just an example. Returns full image.
	#---------------------------------------------------------------------------------------------------------
	returnedCrops = []
	returnedCrops.append((fullsizeImg,0,0)) # 3-tuple; first the "target"; next its lat/long coordinates 0,0
	#---------------------------------------------------------------------------------------------------------
	
	return returnedCrops

