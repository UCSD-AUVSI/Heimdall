import numpy as np
import cv2
from convolutional_mlp_ch74k import test_saved_net_on_image_in_memory

def doOCR(givenCSEG, optionalArgs):
	
	print "Python OCR (this is the Python)"
	
	char = test_saved_net_on_image_in_memory(givenCSEG, "weights.pkl")
	
	return (char, 0.9, 0.0, 'B', 0.75, 0.0)

