import cv2
import math
import numpy as np
import pickle
import sys

def doColorClassification(givenSColor, givenCColor, optionalArgs):
	
	print "Python Color Classification (this is the Python)"
	
	
	if len(givenSColor) != 3:
		print "WARNING: SColor wasn't a 3-element list!!!"
	if len(givenCColor) != 3:
		print "WARNING: CColor wasn't a 3-element list!!!"

	returnedSColor = getColor(givenSColor, 1)
	returnedCColor = getColor(givenCColor, 1)
	
	return (returnedSColor, returnedCColor)

def getColor(color, testCode):
	try:
		color_db=pickle.load(open("color_db.p","rb"))
	except:
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
	ret, results, neighbours ,dist = knn.find_nearest(newcomer, 3)

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
	return output

COLOR_TO_NUMBER = {"White":1,"Black":2,"Red":3,"Orange":4,"Yellow":5,"Blue":6,"Green":7,"Purple":8,"Pink":9,"Brown":10,"Grey":11,"Teal":12}

def bgr_to_lab(bgr):
	#create blank image 1x1 pixel
	blank_image=np.zeros((1,1,3),np.uint8)
	#set image pixels to bgr input
	blank_image[:]= bgr
	#turn into LAB
	cielab = cv2.cvtColor(blank_image,cv2.COLOR_BGR2LAB)
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
