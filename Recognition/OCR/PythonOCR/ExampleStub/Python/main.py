import numpy as np
import cv2


def doOCR(givenCSEG, optionalArgs):
	
	print "Python OCR (this is the Python)"
	
	cv2.imshow("PythonOCR given CSEG", givenCSEG)
	cv2.waitKey(0)
	cv2.destroyAllWindows()
	
	return ('A', 0.9, 0.0, 'B', 0.75, 0.0)

