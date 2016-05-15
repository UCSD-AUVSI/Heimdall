import cv2
import numpy as np
from pypermutohedralfiltercpplib import PermutohedralBilateralFilter
import sys
import time

# get image filename as command-line argument
if len(sys.argv) <= 1:
	print("usage:  {image-file}")
	quit()
imgfilename = sys.argv[1]

# load image from disk
img = cv2.imread(imgfilename,cv2.IMREAD_COLOR)
img = img.astype(np.float32) / 255.0

# resize longest side to 600 pix if too big
if img.shape[0] > img.shape[1]:
	fsc = 600.0 / img.shape[0]
else:
	fsc = 600.0 / img.shape[1]
img = cv2.resize(img,(0,0),fx=fsc,fy=fsc)

# define the function that's called every time one of the trackbars is moved
def updateWindow(xxx):
	# get parameters from trackbars
	sigmaSpace = max(5,cv2.getTrackbarPos('sigmaSpace','ImageWindow'))
	sigmaColor = max(1,cv2.getTrackbarPos('sigmaColor*1000','ImageWindow')) / 1000.
	
	# filter
	timebefore = time.time()
	blurredImg = PermutohedralBilateralFilter(img, sigmaSpace, sigmaColor)
	timeafter = time.time()
	print("computation time: "+str(timeafter-timebefore)+" sec")
	
	# display the result
	cv2.imshow('ImageWindow',blurredImg)

#set up the window everything will take place in
cv2.namedWindow('ImageWindow')

# create trackbars for color change
cv2.createTrackbar('sigmaSpace','ImageWindow',5,100,updateWindow)
cv2.createTrackbar('sigmaColor*1000','ImageWindow',0,100,updateWindow)

#Creates the window for the first time
updateWindow(0)
#wait for keypress to end the program
cv2.waitKey(0)
