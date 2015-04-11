import cv2
import math
import numpy as np
import pickle
import sys
try:
	color_db=pickle.load(open("color_db.p","rb"))
	count = 0
	for d in color_db:
		count = count + 1
	print count
except:
	raise BaseException
