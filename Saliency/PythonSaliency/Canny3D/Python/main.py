import numpy as np
import cv2
import GetCannyEdgesPython
import EdgesToContours


def doSaliency(fullsizeImg, firstFourArgs):
	
	#imheight,imwidth = fullsizeImg.shape[:2]
	#print "python is processing an image of size: " + str(imwidth) + "x" + str(imheight)
	
	'''cThreshLow = 100
	cHighRatioTimes5 = 30
	blurRadius = 11
	resizePct = 20'''
	cThreshLow = firstFourArgs[0]
	cHighRatioTimes5 = firstFourArgs[1]
	blurRadius = int(round(firstFourArgs[2]))
	resizePct = firstFourArgs[3]
	
	# these settings probably don't need to be changed...
	kernelSize = 5
	minTargetPixelWidth = 20
	maxTargetPixelWidth = 350 # ...except this, if we fly at lower altitudes
	
	# calculate high threshold, and resize input image
	cThreshHigh = (cThreshLow*cHighRatioTimes5/5.0)
	resizedImg = cv2.resize(fullsizeImg, (0,0), fx = resizePct/100.0, fy = resizePct/100.0, interpolation = cv2.INTER_LINEAR)
	
	# edge detect using C++ Canny3D
	edgeImg = GetCannyEdgesPython.GetEdges(resizedImg, blurRadius, cThreshLow, cThreshHigh, kernelSize, False)
	
	# find targets: uses edges to find closed blobs
	crops = EdgesToContours.GetCroppedObjects(fullsizeImg, edgeImg, resizePct, minTargetPixelWidth, maxTargetPixelWidth, True)
	
	return crops

