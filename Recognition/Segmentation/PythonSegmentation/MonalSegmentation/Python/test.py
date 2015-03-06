import getopt, sys
import numpy as np
import cv2
import main


def testimg(argv):
	#====================================================================
	#	filename of image to load is a command line argument
	
	filename_of_image = ""
	
	try:
		opts, args = getopt.getopt(argv, "i:", ["image="])
	except getopt.GetoptError:
		usage()
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-i", "--image"):
			filename_of_image = arg
	
	#====================================================================
	#	load the image
	
	loaded_image_mat = cv2.imread(filename_of_image)
	
	#check if it failed to load
	if np.size(loaded_image_mat) <= 1:
		print "ERROR: COULD NOT OPEN IMAGE FILE: " + filename_of_image
		return
	
	#====================================================================
	#	segmentation
	
	(shapeSeg, shapeColor, charSeg, charColor) = main.doSegmentation(loaded_image_mat, 0)
	
	#====================================================================
	#	display image and results
	
	cv2.imshow("original image", loaded_image_mat)
	cv2.imshow("shape seg", shapeSeg/255.0) #images are floating point from 0.0 to 255.0, convert from 0.0 to 1.0 for imshow
	cv2.imshow("char seg", charSeg/255.0)
	
	#wait for keypress
	cv2.waitKey(0)
	


# execute main()... this needs to be at the end
if __name__ == "__main__":
	testimg(sys.argv[1:])
