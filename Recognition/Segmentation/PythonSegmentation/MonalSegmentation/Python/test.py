import getopt, sys
import numpy as np
import cv2
import main



#f=open('shapeList.txt','w')
#k=open('shapenumList.txt','w')
#diff=open('shapeUnrec.txt', 'w')
diff=open('saliencyPart2junkShaperec.txt','w')
#def testimg(sysargv):
global counn
counn=0
for tc in range(99,203):

	kk=str(tc)+"junk.png"
	#kk=str(tc)+"_0__crop.jpg"
	#kk=str(tc)+"_0__crop.png"
	print "\n"+kk

	
	loaded_image_mat = cv2.imread(kk)



	
	#====================================================================
	#	segmentation
	(shapeSeg, shapeColor, charSeg, charColor,s) = main.doSegmentation(loaded_image_mat, 0,tc)
	
	#====================================================================
	#	display image and results
	#p=("\"" + s +"\", ")
	#f.write(p)
	#j=(str(tc)+". " + s +"\n")
	#k.write(j)'
	if(s):
		p=(str(tc)+". " + s +"\n")
		diff.write(p)
		counn=counn+1
	
	
	"""
	actualShapes = ["Square", "QuarterCircle", "Rectangle", "Triangle", "Star", "Square", "Triangle", "Rectangle", "Square", "Plus", "Trapezoid", "Semicircle", "Triangle", "Trapezoid", "Semicircle", "Trapezoid", "Triangle", "Square", "Trapezoid", "Star", "Plus", "Star", "Circle", "Circle", "Star", "Square", "Semicircle", "QuarterCircle", "Rectangle", "Square", "Triangle", "Triangle", "Rectangle", "Star", "Trapezoid", "Rectangle", "Square", "Star", "Star", "Triangle", "Plus", "Square", "Diamond", "Rectangle", "Triangle", "Circle", "Diamond", "Plus", "Triangle", "Rectangle", "Semicircle", "Diamond", "Square", "Plus", "Rectangle", "Circle", "Trapezoid", "Semicircle", "Diamond", "Plus", "Trapezoid", "Triangle", "Circle", "Semicircle", "Star", "Triangle", "Square", "Square", "Circle", "Trapezoid", "Rectangle", "Plus", "Semicircle", "Triangle", "Diamond", "Square", "QuarterCircle", "Square", "Circle", "Circle", "Circle", "Square", "Rectangle", "Trapezoid", "Diamond", "Square", "Triangle", "Triangle", "Triangle", "Triangle", "Triangle", "Trapezoid", "Trapezoid", "Trapezoid", "Star", "Star", "Star", "Star", "Star", "Triangle", "Square", "Square", "Square", "Square", "Rectangle", "Rectangle", "Rectangle", "Rectangle", "Rectangle", "QuarterCircle", "QuarterCircle", "Semicircle", "Trapezoid", "Semicircle", "Diamond", "Plus", "Circle", "Circle", "Circle", "Plus", "Plus", "Plus", "Plus", "Plus"]
	if(actualShapes[tc]!=s):
		if(not s):
			diff.write("shape " + str(tc) + ": returned Nothing, actually is " +actualShapes[tc] +"\n")
		else:
			diff.write("shape " + str(tc) + ": got "+s+" , actually is " +actualShapes[tc] +"\n")
	"""
"""
	cv2.imshow(kk, loaded_image_mat)
	cv2.imshow("shape seg", shapeSeg) #images are floating point from 0.0 to 255.0, convert from 0.0 to 1.0 for imshow
	cv2.imshow("char seg", charSeg)
	cv2.waitKey(0) #wait for keypress

#"""
"""



def testimg(sysargv):
	#====================================================================
	#	filename of image to load is a command line argument
	
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
	
	(shapeSeg, shapeColor, charSeg, charColor) = main.doSegmentation(loaded_image_mat, 0)
	
	#====================================================================
	#	display image and results
	
	if showImages:
		cv2.imshow("original image", loaded_image_mat)
		cv2.imshow("shape seg", shapeSeg*255.0) #images are floating point from 0.0 to 255.0, convert from 0.0 to 1.0 for imshow
		cv2.imshow("char seg", charSeg*255.0)
		cv2.waitKey(0) #wait for keypress
	


# execute main()... this needs to be at the end
if __name__ == "__main__":
	testimg(sys.argv)
#"""
diff.write("count: " + str(counn))
