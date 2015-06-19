import numpy as np
import cv2


def doVerif(ShapeColorVals, ShapeColorStr, ShapeName, CharColorVals, CharColorStr, Char1Name, Char1conf, Char2Name, Char2conf, TargetLat, TargetLong, TargetOrientation, OriginalImageFilename, optionalArgs):
	
	print "Python Verif (this is the Python)"
	
	if len(ShapeColorVals) != 3:
		print "WARNING: ShapeColorVals wasn't a 3-element list!!!"
	if len(CharColorVals) != 3:
		print "WARNING: CharColorVals wasn't a 3-element list!!!"
	
	print "shape: "+ShapeName+", with color ("+str(ShapeColorVals[0])+","+str(ShapeColorVals[1])+","+str(ShapeColorVals[2])+") i.e. "+ShapeColorStr
	print "char(1): "+Char1Name+", conf "+str(Char1conf)+", with color ("+str(CharColorVals[0])+","+str(CharColorVals[1])+","+str(CharColorVals[2])+") i.e. "+CharColorStr
	print "char(2): "+Char2Name+", conf "+str(Char2conf)+", with color ("+str(CharColorVals[0])+","+str(CharColorVals[1])+","+str(CharColorVals[2])+") i.e. "+CharColorStr
	print "target (lat, long) == ("+str(TargetLat)+", "+str(TargetLong)+"), orientation: "+str(TargetOrientation)

