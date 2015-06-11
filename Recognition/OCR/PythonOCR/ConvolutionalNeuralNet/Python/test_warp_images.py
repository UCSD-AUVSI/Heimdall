import os, sys
import numpy as np
import cv2
from WarpTrainingImages import warpImg

if len(sys.argv) <= 1:
	print("usage: {img-folder}")
	quit()
imfolder = str(sys.argv[1])

destsize = 40

images = []
for (dirpath, dirnames, filenames) in os.walk(imfolder):
	for filename in filenames:
		try:
			imgread = cv2.imread(dirpath+filename, cv2.CV_LOAD_IMAGE_GRAYSCALE)
			if np.size(imgread) > 1:
				images.append(cv2.resize(np.asarray(imgread, dtype=np.float32), (destsize,destsize)))
				print("found "+str(filename))
		except:
			aaa = 123
if len(images) == 0:
	print("didnt find any images in that folder!")

numTimesToWarpEach = 600
imidx = 0
allimgs = None
for image in images:
	warpedAvg = np.zeros(image.shape, dtype=np.float32)
	for ii in range(numTimesToWarpEach):
		warpedAvg += warpImg(image)
	warpedAvg /= (255.0*float(numTimesToWarpEach))
	
	if allimgs is None:
		allimgs = np.reshape(warpedAvg,(1,destsize*destsize))
	else:
		allimgs = np.concatenate([allimgs, np.reshape(warpedAvg,(1,destsize*destsize))])
	
	print("warped "+str(imidx+1)+" so far")
	cv2.imwrite("/home/ucsdauvsi/AUVSI/CSEGS________/warpeddddd/"+str(imidx)+".png", warpedAvg*255.0)
	#cv2.imshow(str(imidx), warpedAvg)
	imidx += 1

cv2.imshow("all", allimgs)
cv2.waitKey(0)
