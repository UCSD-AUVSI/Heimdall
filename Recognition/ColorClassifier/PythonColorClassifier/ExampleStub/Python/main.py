import numpy as np
import cv2


def doColorClassification(givenSColor, givenCColor, optionalArgs):
	
	print "Python Color Classification (this is the Python)"
	
	
	if len(givenSColor) != 3:
		print "WARNING: SColor wasn't a 3-element list!!!"
	if len(givenCColor) != 3:
		print "WARNING: CColor wasn't a 3-element list!!!"
	
	
	returnedSColor = "scolor:(" + str(givenSColor[0]) + "," + str(givenSColor[1]) + "," + str(givenSColor[2]) + ")"
	returnedCColor = "ccolor:(" + str(givenCColor[0]) + "," + str(givenCColor[1]) + "," + str(givenCColor[2]) + ")"
	
	return (returnedSColor, returnedCColor)

