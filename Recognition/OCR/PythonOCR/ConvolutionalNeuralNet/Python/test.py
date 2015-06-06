import getopt, sys
import numpy as np
import cv2
import main
import RandomJunkPolygonGenerator

def testimg(sysargv):
	#====================================================================
	#	filename of image to load is a command line argument
	
	for iii in range(14):
		cv2.imshow(str(iii), RandomJunkPolygonGenerator.returnImage())
	cv2.waitKey(0)
	
	if len(sysargv) <= 1:
		print("usage:  {image-file}  {optional:show-images?}")
		quit()
	
	filename_of_image = str(sysargv[1])
	showImages = True
	if len(sysargv) > 2:
		if int(sysargv[2]) == 0:
			showImages = False
	
	#====================================================================
	#	load the image
	
	loaded_image_mat = cv2.imread(filename_of_image)
	
	#check if it failed to load
	if np.size(loaded_image_mat) <= 1:
		print "ERROR: COULD NOT OPEN IMAGE FILE: " + filename_of_image
		return
	
	#====================================================================
	#	segmentation
	
	(c1char, c1conf, c1angle, c2char, c2conf, c2angle) = main.doOCR(loaded_image_mat, 0)
	
	#====================================================================
	#	display image and results
	
	print("top result: char == \'"+c1char+"\' at angle "+str(c1angle)+" with confidence "+str(c1conf))
	print("second-best result: char == \'"+c2char+"\' at angle "+str(c2angle)+" with confidence "+str(c2conf))
	


# execute main()... this needs to be at the end
if __name__ == "__main__":
	testimg(sys.argv)
