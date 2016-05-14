import pycanny3dcpplib
import EdgesToContours
import numpy as np
import cv2

def GetEdges(givenImg, gBlurRadius, cLowThresh, cHighThresh, kernelSize, boolReturnBlurred=False):
	
	# OpenCV requires blur radii to be an odd integer greater than zero; enforce this
	if gBlurRadius == 0: #if it's zero
		gBlurRadius = 1
	if (gBlurRadius % 2) == 0: #if it's even (modulo)
		gBlurRadius = (gBlurRadius + 1)
	
	# Gaussian blur
	#blurredImg = cv2.GaussianBlur(givenImg, (gBlurRadius,gBlurRadius), 0)
	blurredImg = cv2.medianBlur(givenImg, gBlurRadius)
	
	# Canny edge detection
	edgeImg = pycanny3dcpplib.CIELAB(blurredImg, cLowThresh, cHighThresh, kernelSize)
	
	if boolReturnBlurred == True:
		return edgeImg,blurredImg
	
	return edgeImg
