import cv2
import math
import numpy as np
import pickle
import sys
import os


def getColor(color, testCode):
	print "starting Get Color"
	try:
		color_db=pickle.load(open("/home/marcof/Documents/AUVSI/Heimdall/build/Recognition/ColorClassifier/PythonColorClassifier/ColorClassifier/Python/color_db.p","rb"))
	except :
		print "Exception "+ str(sys.exc_info()[0])
		raise BaseException
	cielab_output = []
	name = []
	check=[]
	for dictionary in color_db:
		cielab=dictionary["lab"]
		cielab_output.append(cielab)
		#add name to names array
		name.append(dictionary["name"])
		check.append({"cielab":cielab,"name":dictionary["name"]})
	print len(check)
	print check[0]
	print check[1]
	#put cielab data into matrix
	trainData=np.matrix(cielab_output, dtype=np.float32)
	#put names which are numbers right now into a matrix
	responses = np.matrix(name, dtype=np.float32)
	#turn test point into matrix
	newcomer=np.matrix(color, dtype=np.float32)

	knn = cv2.KNearest()
	# train the data
	knn.train(trainData,responses)
	# find nearest
	print "pre ret"
	print newcomer
	ret, results, neighbours ,dist = knn.find_nearest(newcomer, 3)
	print "ret"
	output = ""
	#get results
	if testCode == 1 :
		print "result: ", results,"\n"
		print "neighbours: ", neighbours,"\n"
		print "distance: ", dist
	for name,val in COLOR_TO_NUMBER.iteritems():
		if val==int(results[0][0]):
			output = name
	if testCode == 1:
		print output
		#Check Answer
		blank_image=np.zeros((100,100,3),np.uint8)
		blank_image[:]=newcomer
		blank_image = cv2.cvtColor(blank_image,cv2.COLOR_LAB2BGR)
		cv2.imshow("test",blank_image)
		cv2.waitKey(0)
	print "Color="+output
	print "end"
	return output

COLOR_TO_NUMBER = {"White":1,"Black":2,"Red":3,"Orange":4,"Yellow":5,"Blue":6,"Green":7,"Purple":8,"Pink":9,"Brown":10,"Grey":11,"Teal":12}

def bgr_to_lab(bgr):
	print bgr
	#create blank image 1x1 pixel
	blank_image=np.zeros((1,1,3),np.uint8)
	print blank_image
	#set image pixels to bgr input
	blank_image[:]= bgr
	print blank_image
	#turn into LAB
	try:
		cielab = cv2.cvtColor(blank_image,cv2.COLOR_BGR2LAB)
	except :
		print "Exception "+ str(sys.exc_info()[0])
		raise BaseException
	print "after try"
	print cielab
	return cielab[0][0]
def lab_to_bgr(lab):
	#create blank image 1x1 pixel
	blank_image=np.zeros((1,1,3),np.uint8)
	#set image pixels to bgr input
	blank_image[:]= lab
	#turn into LAB
	bgr = cv2.cvtColor(blank_image,cv2.COLOR_LAB2BGR)
	return bgr[0][0]
def rgb_to_bgr(rgb):
	return tuple(reversed(rgb))

def doColorClassification(givenSColor, givenCColor, optionalArgs):
	
	print "Python Color Classification (this is the Python)"
	print sys.version
	print givenSColor
	print givenCColor
	print cv2.__version__

	if len(givenSColor) != 3:
		print "WARNING: SColor wasn't a 3-element list!!!"
	if len(givenCColor) != 3:
		print "WARNING: CColor wasn't a 3-element list!!!"
	print "To BGR"
	bgrS = rgb_to_bgr(givenSColor)
	bgrC = rgb_to_bgr(givenCColor)
	print "TO LAB"
	labS = bgr_to_lab(bgrS)
	labC = bgr_to_lab(bgrC)
	print "getColor"
	returnedSColor = getColor(labS, 1)
	returnedCColor = getColor(labC, 1)
	
	return (returnedSColor, returnedCColor)

#doColorClassification([0,0,0],[0,0,0],1)