from PIL import Image, ImageTk
import cv2
import math
import numpy as np
import pickle
import sys

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

try:
	color_db=pickle.load(open("color_db.p","rb"))
except:
	raise BaseException
#examples
#color_db.append({"lab":bgr_to_lab(rgb_to_bgr([2,132,130])),"name":12})
#color_db.append({"lab":bgr_to_lab(rgb_to_bgr([0,139,139])),"name":12})
#color_db.append({"lab":bgr_to_lab(rgb_to_bgr([0,128,128])),"name":12})
#color_db.append({"lab":bgr_to_lab(rgb_to_bgr([0,154,154])),"name":12})
#color_db.append({"lab":bgr_to_lab(rgb_to_bgr([0,103,103])),"name":12})

#color_db.append({"lab":bgr_to_lab(rgb_to_bgr([0,103,103])),"name":12})
#color_db.append({"lab":bgr_to_lab(rgb_to_bgr([194,118,63])),"name":4})
#color_db.append({"lab":bgr_to_lab(rgb_to_bgr([194,118,63])),"name":4})



#save new colors
pickle.dump(color_db,open('color_db.p',"wb"))
