import cv2
import numpy as np
from pypermutohedralfiltercpplib import PermutohedralBilateralFilter
import pycanny3dcpplib
import sys

# get image filename as command-line argument
if len(sys.argv) <= 1:
	print("usage:  {image-file}")
	quit()
imgfilename = sys.argv[1]

# load image as floating point type
img = cv2.imread(imgfilename,cv2.IMREAD_COLOR)
img = img.astype(np.float32) / 255.0

# resize longest side to 600 pix if too big
if img.shape[0] > img.shape[1]:
	fsc = 600.0 / img.shape[0]
else:
	fsc = 600.0 / img.shape[1]
img = cv2.resize(img,(0,0),fx=fsc,fy=fsc)

# fixed argument (no trackbar for this)
cannySobelKernelSize = 3

def mynormed(i2m):
	ssmin = np.amin(i2m)
	ssmax = np.amax(i2m)
	return (i2m-ssmin)/(ssmax-ssmin)

def myflatten(im):
	ch = im.shape[2]
	return np.reshape(im,(im.size/ch,ch))

# estimate variance of noise about median
def samplevar(im):
	medf = cv2.medianBlur(im, ksize=3)
	mdf = (im-medf)
	cv2.imshow("mdf-before",mynormed(mdf))
	left = myflatten(mdf[:,:100,:])
	topp = myflatten(mdf[:100,:,:])
	rigt = myflatten(mdf[:,-100:,:])
	bott = myflatten(mdf[-100:,:,:])
	alls = np.concatenate((left,topp,rigt,bott),axis=0)
	return np.sqrt(np.var(alls[:,0])+np.var(alls[:,1])+np.var(alls[:,2]))

colorsigma = samplevar(img)
print("colorsigma == "+str(colorsigma))

# define the function that's called every time one of the trackbars is moved
def updateWindow(xxx):
	# get parameters from trackbars
	threshscale = max(1,cv2.getTrackbarPos('threshscale*10','ImageWindow')) / 10.0
	cannyThreshLow  = cv2.getTrackbarPos('low','ImageWindow') * threshscale
	cannyThreshHigh = cv2.getTrackbarPos('high','ImageWindow') * threshscale
	colorspacescale = max(1,cv2.getTrackbarPos('cspscale','ImageWindow')) / 2000.0
	gaussianBlurPixRadius = cv2.getTrackbarPos('blur','ImageWindow')
	blurType = cv2.getTrackbarPos('blurType','ImageWindow')
	
	if gaussianBlurPixRadius >= 1:
		# OpenCV requires blur radii to be an odd integer greater than zero; enforce this
		if gaussianBlurPixRadius == 0: #if it's zero
			gaussianBlurPixRadius = 1
		if (gaussianBlurPixRadius % 2) == 0: #if it's even (modulo)
			gaussianBlurPixRadius = (gaussianBlurPixRadius + 1)
		# blur
		#blurredImg = cv2.GaussianBlur(img, (gaussianBlurPixRadius,gaussianBlurPixRadius), 0)
		
		spacesigma = 0.3*((gaussianBlurPixRadius-1)*0.5 - 1) + 0.8   # formula from OpenCV documentation, see getGaussianKernel
		
		#blurredImg = cv2.bilateralFilter(img, gaussianBlurPixRadius, colorsigma, spacesigma)
		blurredImg = PermutohedralBilateralFilter(img, spacesigma, float(colorsigma))
		if blurType > 10:
			blurredImg = cv2.medianBlur(blurredImg,ksize=3)
	else:
		blurredImg = img
	
	medf = cv2.medianBlur(blurredImg, ksize=3)
	cv2.imshow("mdf-after",mynormed(blurredImg-medf))
	
	# Canny edge detection
	cv2.imshow('orig',img)
	cv2.imshow('blurredImg',blurredImg)
	#print("blurredImg.shape == "+str(blurredImg.shape))
	#edgeImgResult = pycanny3dcpplib.RGB(blurredImg, cannyThreshLow, cannyThreshHigh, cannySobelKernelSize)
	edgeImgResult = pycanny3dcpplib.CIELAB(blurredImg, cannyThreshLow, cannyThreshHigh, cannySobelKernelSize, colorspacescale)
	#edgeImgResult = pycanny3dcpplib.vanilla(blurredImg, cannyThreshLow, cannyThreshHigh, cannySobelKernelSize)
	#edgeImgResult = cv2.Canny(blurredImg[:,:,0], cannyThreshLow, cannyThreshHigh)
	
	# display the result
	cv2.imshow('ImageWindow',edgeImgResult)


#set up the window everything will take place in
cv2.namedWindow('ImageWindow')

# create trackbars for color change
cv2.createTrackbar('threshscale*10','ImageWindow',0,100,updateWindow)
cv2.createTrackbar('low','ImageWindow',0,500,updateWindow)
cv2.createTrackbar('high','ImageWindow',0,200,updateWindow)
cv2.createTrackbar('cspscale','ImageWindow',0,200,updateWindow)
cv2.createTrackbar('blur','ImageWindow',0,40,updateWindow)
cv2.createTrackbar('blurType','ImageWindow',0,20,updateWindow)

#Creates the window for the first time
updateWindow(0)
#wait for keypress to end the program
cv2.waitKey(0)
