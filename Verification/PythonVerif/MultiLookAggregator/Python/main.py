import numpy as np
import cv2
import pydbscancpplib

def doVerif(QRCodeProbability,
		ShapeColorVals, ShapeColorStr, ShapeName, ShapeNameConfidence,
		CharColorVals, CharColorStr, CharNames, CharNameConfidences,
		TargetLat, TargetLong, TargetOrientation, optionalArgs):
	
	print "MultilookAggregator (this is the Python)"
	
	pairwiseDistancesMat = np.zeros((6,6), np.float32)
	
	pairwiseDistancesMat[1,0] = 0.8
	
	pairwiseDistancesMat[2,0] = 0.03
	pairwiseDistancesMat[2,1] = 0.9
	
	pairwiseDistancesMat[3,0] = 0.05
	pairwiseDistancesMat[3,1] = 0.7
	pairwiseDistancesMat[3,2] = 0.04
	
	pairwiseDistancesMat[4,0] = 0.85
	pairwiseDistancesMat[4,1] = 0.1
	pairwiseDistancesMat[4,2] = 0.68
	pairwiseDistancesMat[4,3] = 0.74
	
	pairwiseDistancesMat[5,0] = 0.9
	pairwiseDistancesMat[5,1] = 0.8
	pairwiseDistancesMat[5,2] = 0.9
	pairwiseDistancesMat[5,3] = 0.8
	pairwiseDistancesMat[5,4] = 0.9
	
	# result should be: ((0,2,3),(1,4))
	
	print "================================================================================="
	returnedClusters = pydbscancpplib.TargetClusterDBSCAN(pairwiseDistancesMat, 1, 0.3)
	print "returnedClusters: ("+str(len(returnedClusters))+" clusters)"
	for cluster in returnedClusters:
		print "--- "+str(cluster)
	print "================================================================================="
	
	if len(ShapeColorVals) != 3:
		print "WARNING: ShapeColorVals wasn't a 3-element list!!!"
	if len(CharColorVals) != 3:
		print "WARNING: CharColorVals wasn't a 3-element list!!!"
	
	print "shape: "+ShapeName+", with color ("+str(ShapeColorVals[0])+","+str(ShapeColorVals[1])+","+str(ShapeColorVals[2])+") i.e. "+ShapeColorStr
	print "char: "+CharName+", with color ("+str(CharColorVals[0])+","+str(CharColorVals[1])+","+str(CharColorVals[2])+") i.e. "+CharColorStr
	print "target (lat, long) == ("+str(TargetLat)+", "+str(TargetLong)+"), orientation: "+str(TargetOrientation)
	
