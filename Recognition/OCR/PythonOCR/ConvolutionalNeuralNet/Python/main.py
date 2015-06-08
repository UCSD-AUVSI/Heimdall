import numpy as np
import cv2
from convolutional_mlp_ch74k import test_saved_net_on_image_in_memory

def doOCR(givenCSEG, optionalArgs):
	
	print "Python OCR (this is the Python)"
	
	if len(givenCSEG.shape) > 2:
		grayscaleCSEG = cv2.cvtColor(givenCSEG, cv2.COLOR_BGR2GRAY)
	else:
		grayscaleCSEG = givenCSEG
	
	inrows, incols = grayscaleCSEG.shape
	
	#------------------------------
	# OCR settings
	
	needImgSize = 40  #images must be resized to this since the network has been trained for this
	
	#anglesInbetween = 3 #predict with 22.5 degree precision (90/(3+1))
	anglesInbetween = 1 #predict with 45 degree precision (90/(1+1))
	
	weightsfilebase = "weights/cnn40x40theano_paramsWb_6704kims_score_12.1350546177_lr0.05_mom0.5_batch106.pkl"
	returnDetailedInfo = True
	numTopGuessesToReturn = 2
	
	#------------------------------
	
	if inrows != needImgSize or incols != needImgSize:
		resizedCSEG = cv2.resize(grayscaleCSEG, (needImgSize,needImgSize)).astype(float)
	else:
		resizedCSEG = grayscaleCSEG.astype(float)
	
	(chars,orientations,confidences) = test_saved_net_on_image_in_memory(resizedCSEG, weightsfilebase,
									needImgSize,
									anglesInbetween=anglesInbetween,
									returnDetailedInfo=returnDetailedInfo,
									numTopGuessesToReturn=numTopGuessesToReturn)
	
	return (chars[-1], confidences[-1], orientations[-1], chars[-2], confidences[-2], orientations[-2])







