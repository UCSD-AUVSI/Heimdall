import cv2
import pycanny3dcpplib
import sys

# get image filename as command-line argument
if len(sys.argv) <= 1:
	print("usage:  {image-file}")
	quit()
imgfilename = sys.argv[1]

# load image from disk
img = cv2.imread(imgfilename,cv2.IMREAD_COLOR)

# resize longest side to 600 pix if too big
if img.shape[0] > img.shape[1]:
	fsc = 600.0 / img.shape[0]
else:
	fsc = 600.0 / img.shape[1]
img = cv2.resize(img,(0,0),fx=fsc,fy=fsc)

# fixed argument (no trackbar for this)
kernelSize = 3

# define the function that's called every time one of the trackbars is moved
def updateWindow(xxx):
	# get parameters from trackbars
	cannyThreshLow  = cv2.getTrackbarPos('low','ImageWindow')
	cannyThreshHigh = cv2.getTrackbarPos('high','ImageWindow')
	gaussianBlurPixRadius = cv2.getTrackbarPos('blur','ImageWindow')
	
	# OpenCV requires blur radii to be an odd integer greater than zero; enforce this
	if gaussianBlurPixRadius == 0: #if it's zero
		gaussianBlurPixRadius = 1
	if (gaussianBlurPixRadius % 2) == 0: #if it's even (modulo)
		gaussianBlurPixRadius = (gaussianBlurPixRadius + 1)
	
	# Gaussian blur
	blurredImg = cv2.GaussianBlur(img, (gaussianBlurPixRadius,gaussianBlurPixRadius), 0)
	
	# Canny edge detection
	edgeImgResult = pycanny3dcpplib.CIELAB(blurredImg, cannyThreshLow, cannyThreshHigh, kernelSize)
	#edgeImgResult = cv2.Canny(blurredImg, cannyThreshLow, cannyThreshHigh)
	
	# display the result
	cv2.imshow('ImageWindow',edgeImgResult)


#set up the window everything will take place in
cv2.namedWindow('ImageWindow')

# create trackbars for color change
cv2.createTrackbar('low','ImageWindow',0,100,updateWindow)
cv2.createTrackbar('high','ImageWindow',0,100,updateWindow)
cv2.createTrackbar('blur','ImageWindow',0,20,updateWindow)

#Creates the window for the first time
updateWindow(0)
#wait for keypress to end the program
cv2.waitKey(0)
