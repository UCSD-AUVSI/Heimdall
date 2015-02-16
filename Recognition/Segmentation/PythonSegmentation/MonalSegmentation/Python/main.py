import numpy as np
import cv2
import pykmeansppcpplib


def doSegmentation(cropImg, optionalArgs):
	
	imrows,imcols = cropImg.shape[:2]
	print "Monal-python-segmentation is processing an image of size: " + str(imcols) + "x" + str(imrows)
	print "note: currently this only returns color-clustered images, not the shape or char masks"
	
	#-------------------------------------------------------------------------
	#	Placeholder code (stub) instead of a real segmentation algorithm,
	#	because this is just an example
	#-------------------------------------------------------------------------
	cropf32 = np.float32(cropImg)
	cropf32copy = cropf32.copy() #the cluster function messes up this original image, so make a copy
	
	shapeSegMask = np.ones((imrows,imcols), np.uint8)
	charSegMask = np.zeros((imrows,imcols), np.uint8)
	
	for ii in range(imrows):
		if ii > int(float(imrows)*0.25) and ii < int(float(imrows)*0.75):
			for jj in range(imcols):
				if jj > int(float(imcols)*0.25) and jj < int(float(imcols)*0.75):
					charSegMask[ii][jj] = 255
	
	shapeSeg = pykmeansppcpplib.ClusterKmeansPPwithMask(cropf32, shapeSegMask, 3, 8, 20)
	charSeg = pykmeansppcpplib.ClusterKmeansPPwithMask(cropf32copy, charSegMask, 3, 8, 20)
	
	shapeColor = (0,0,0)
	charColor = (0,0,0)
	#-------------------------------------------------------------------------
	
	return (shapeSeg, shapeColor, charSeg, charColor)

