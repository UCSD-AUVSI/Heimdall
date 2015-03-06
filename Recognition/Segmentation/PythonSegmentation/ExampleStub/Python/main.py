import numpy as np
import cv2


def doSegmentation(cropImg, optionalArgs):
	
	imheight,imwidth = cropImg.shape[:2]
	print "python-segmentation is processing an image of size: " + str(imwidth) + "x" + str(imheight)
	
	#-------------------------------------------------------------------------
	#	Placeholder code (stub) instead of a real segmentation algorithm,
	#	because this is just an example
	#-------------------------------------------------------------------------
	charSeg = np.zeros((imwidth,imheight), np.uint8)
	#shapeSeg = np.zeros((imwidth,imheight), np.uint8)
	cropGray = cv2.cvtColor(cropImg, cv2.COLOR_BGR2GRAY)
	(junk, shapeSeg) = cv2.threshold(cropGray, 127, 255, cv2.THRESH_BINARY)
	
	shapeColor = (0,0,0)
	charColor = (0,0,0)
	#-------------------------------------------------------------------------
	
	return (shapeSeg, shapeColor, charSeg, charColor)

