import numpy as np
import cv2

def GetShapeContours(edgeImg, areaCutoffMin, areaCutoffMax):
	
	imheight,imwidth = edgeImg.shape[:2]
	
	ksize = 8
	numIters = 1
	kkernelDilat = np.ones((ksize,ksize),np.uint8)
	kkernelErode = np.ones((ksize+1,ksize+1),np.uint8)
	
	# dilate to connect pieces of a shape, if a shape is split up
	edgeImg = cv2.dilate(edgeImg, kkernelDilat, iterations=1)
	
	#set boundary to zero so one edge can't cut the image entirely in half
	edgeImg[0,:] = 0
	edgeImg[imheight-1,:] = 0
	edgeImg[:,0] = 0
	edgeImg[:,imwidth-1] = 0
	
	# get external contours (e.g. if a shape contains a letter, get only the shape)
	ctours,hrch = cv2.findContours(edgeImg, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
	
	# draw back into what is now a blank black image
	cv2.drawContours(edgeImg, ctours, -1, 255, -1)
	
	# erode by 1 additional pixel to eliminate things that were speckles or thin lines (not closed shapes)
	edgeImg = cv2.erode(edgeImg, kkernelErode, iterations=1)
	
	# now filter by size to ensure we get rid of speckles left behind by that last erode
	ctours,hrch = cv2.findContours(edgeImg, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
	
	finalCtours = []
	for contour in ctours:
		thisArea = cv2.contourArea(contour)
		if (thisArea >= areaCutoffMin and thisArea < areaCutoffMax):
			finalCtours.append(contour)
		#print "thisArea == " + str(thisArea)
	
	return finalCtours
	#return ctours

def SimplifyEdgeImg(edgeImg, areaCutoffMin, areaCutoffMax):
	
	edgeImgCopy = edgeImg[:]
	
	# get contours of shapes
	finalCtours = GetShapeContours(edgeImg, areaCutoffMin, areaCutoffMax)
	
	# draw those shapes
	edgeImg = edgeImgCopy#np.zeros(edgeImg.shape, np.uint8)
	
	if len(finalCtours) > 0:
		cv2.drawContours(edgeImg, finalCtours, -1, 127, 2)
	
	return edgeImg


def GetCropRectangles(edgeImg, areaCutoffMin, areaCutoffMax, cropAreaCutoffMax):
	
	fractionOfCropToBeTarget = 0.5
	
	finalCropRects = []
	imheight,imwidth = edgeImg.shape[:2]
	
	finalCtours = GetShapeContours(edgeImg, areaCutoffMin, areaCutoffMax)
	
	if len(finalCtours) > 0:
		cropRects = []
		for contour in finalCtours:
			crop = cv2.boundingRect(contour)
			
			#print "croprect BEFOR == " + str(crop)
			
			if((crop[2]*crop[3]) <= cropAreaCutoffMax):
				
				croprectPadded = [crop[0], crop[1], crop[2], crop[3]]
			
				padPixelsH = int((crop[2]*0.5)*((1.0/fractionOfCropToBeTarget)-1.0))
				padPixelsV = int((crop[3]*0.5)*((1.0/fractionOfCropToBeTarget)-1.0))
			
				#left
				if crop[0] < padPixelsH:
					croprectPadded[2] = (crop[2] + crop[0])
					croprectPadded[0] = 0
				else:
					croprectPadded[2] = (crop[2] + padPixelsH) #increase width of box
					croprectPadded[0] = (crop[0] - padPixelsH) #move box to the left
			
				#top
				if crop[1] < padPixelsV:
					croprectPadded[3] = (crop[3] + crop[1])
					croprectPadded[1] = 0
				else:
					croprectPadded[3] = (crop[3] + padPixelsV)
					croprectPadded[1] = (crop[1] - padPixelsV)
			
				#right
				if (croprectPadded[0]+croprectPadded[2]+padPixelsH) > imwidth:
					croprectPadded[2] = (imwidth - croprectPadded[0])
					croprectPadded[0] = croprectPadded[0]
				else:
					croprectPadded[2] = (croprectPadded[2] + padPixelsH)
					croprectPadded[0] = croprectPadded[0]
			
				#bottom
				if (croprectPadded[1]+croprectPadded[3]+padPixelsV) > imheight:
					croprectPadded[3] = (imheight - croprectPadded[1])
					croprectPadded[1] = croprectPadded[1]
				else:
					croprectPadded[3] = (croprectPadded[3] + padPixelsV)
					croprectPadded[1] = croprectPadded[1]
			
				#print "croprect AFTER == " + str(croprectPadded)
			
				finalCropRects.append(croprectPadded)
	
	return finalCropRects


def GetCroppedObjects(originalImg, edgeImg, resizePct, widthCutoffMin, widthCutoffMax, boolReturnPositionOfCrops):
	
	rszRatio = (resizePct/100.0)
	
	cropRects = GetCropRectangles(edgeImg, (widthCutoffMin*rszRatio)**2, (widthCutoffMax*rszRatio)**2, (widthCutoffMax*1.15*rszRatio)**2)
	
	returnedCrops = []
	
	#returned crops will contain tuples: (CropImgData, CropX, CropY)
	#where (CropX,CropY) is the location of the upper-left corner in the original image
	
	if boolReturnPositionOfCrops == True:
		for cr in cropRects:
			returnedCrops.append((originalImg[int(cr[1]/rszRatio):(int(cr[1]/rszRatio)+int(cr[3]/rszRatio)), int(cr[0]/rszRatio):(int(cr[0]/rszRatio)+int(cr[2]/rszRatio)), :], int(cr[0]/rszRatio), int(cr[1]/rszRatio)))
	else:
		for cr in cropRects:
			returnedCrops.append(originalImg[int(cr[1]/rszRatio):(int(cr[1]/rszRatio)+int(cr[3]/rszRatio)), int(cr[0]/rszRatio):(int(cr[0]/rszRatio)+int(cr[2]/rszRatio)), :])
	
	return returnedCrops









