import numpy as np
import cv2
import main
import os
import getopt, sys


def ObjDetectFull(origImg, fileNameBase):
	
	crops = main.doSaliency(origImg)
	
	folderToPlaceCrops = "../../../../../../output_images"
	
	if len(crops) > 0 and len(folderToPlaceCrops) > 1:
		for ii in range(len(crops)):
			cv2.imwrite(folderToPlaceCrops + "/" + fileNameBase + "_crop" + str(ii) + "_" + str(crops[ii][1]) + ",,," + str(crops[ii][2]) + "_.png", crops[ii][0])
	



def usage():
	print "requires argument: -f [FOLDER] or --folder=[FOLDER]"


def mainn(argv):
	
	#====================================================================
	#	filename of image to load is a command line argument
	
	folderWithPics = ""
	
	try:
		opts, args = getopt.getopt(argv, "f:", ["folder="])
	except getopt.GetoptError:
		usage()
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-f", "--folder"):
			folderWithPics = arg
	
	if len(folderWithPics) < 1:
		usage()
		sys.exit(2)
	
	if folderWithPics.endswith("/") == False:
		folderWithPics = (folderWithPics + "/")
	
	print "processing files in folder: \'" + folderWithPics + "\'"
	
	#====================================================================
	
	for file in os.listdir(folderWithPics):
		if file.endswith(".jpg") or file.endswith(".JPG"):
			print "processing file \'" + file + "\'"
			img = cv2.imread(folderWithPics+file)
			ObjDetectFull(img, file)
	
	
	


if __name__ == "__main__":
	mainn(sys.argv[1:])
