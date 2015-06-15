import getopt, sys, math
import numpy as np
import cv2
import pykmeansppcpplib

'''
bp::object ClusterKmeansPPwithMask(PyObject *filteredCropImage, PyObject *maskForClustering,
				int k_num_cores, int num_lloyd_iterations, int num_kmeanspp_iterations, bool print_debug_console_output,
				double use5DclusteringScale)
'''

def KClusterSeg(limf, scale5D, numclusters):
	
	use5DclusteringScale = (1.0 / scale5D) * (0.5 * (float(limf.shape[0]) + float(limf.shape[1])))
	
	labf = np.copy(cv2.cvtColor(limf,cv2.COLOR_BGR2LAB))
	
	allones = np.ones((limf.shape[0],limf.shape[1],1),np.uint8)
	
	tresults = pykmeansppcpplib.ClusterKmeansPPwithMask(labf, allones, numclusters, 20, 30, False, use5DclusteringScale)
	
	return (cv2.cvtColor(tresults[0], cv2.COLOR_LAB2BGR), tresults[3])


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
	
	limf = np.float32(loaded_image_mat) / 255.0
	limf2 = np.copy(limf)
	limf3 = np.copy(limf)
	
	firstSuperPixRGB, pot1 = KClusterSeg(limf, 150., 21)
	
	secondRegularRGB, pot2 = KClusterSeg(firstSuperPixRGB, -1., 3)
	
	straightTo3, pot3 = KClusterSeg(limf2, -1., 3)
	
	#====================================================================
	#	display image and results
	
	print("limf min,max == "+str(np.amin(limf))+", "+str(np.amax(limf)))
	print("superpix min,max == "+str(np.amin(firstSuperPixRGB))+", "+str(np.amax(firstSuperPixRGB)))
	
	postsuperpixdiffL1 = np.sum(cv2.absdiff(limf3, secondRegularRGB))
	straight2diffL1 = np.sum(cv2.absdiff(limf3, straightTo3))
	superpixVSk3L1 = np.sum(cv2.absdiff(firstSuperPixRGB, secondRegularRGB))
	
	postsuperpixdiffL2 = cv2.absdiff(limf3, secondRegularRGB)
	postsuperpixdiffL2 = math.sqrt(np.sum(cv2.multiply(postsuperpixdiffL2,postsuperpixdiffL2)))
	straight2diffL2 = cv2.absdiff(limf3, straightTo3)
	straight2diffL2 = math.sqrt(np.sum(cv2.multiply(straight2diffL2,straight2diffL2)))
	superpixVSk3L2 = cv2.absdiff(firstSuperPixRGB, secondRegularRGB)
	superpixVSk3L2 = math.sqrt(np.sum(cv2.multiply(superpixVSk3L2,superpixVSk3L2)))
	
	print("post-superpix diff L1 == "+str(postsuperpixdiffL1)+", L2 == "+str(postsuperpixdiffL2))
	print("straight2k3 diff L1 == "+str(straight2diffL1)+", L2 == "+str(straight2diffL2))
	print("superpixVSk3 diff L1 == "+str(superpixVSk3L1)+", L2 == "+str(superpixVSk3L2))
	
	cv2.imshow("original image", loaded_image_mat)
	cv2.imshow("superpixels", firstSuperPixRGB)
	cv2.imshow("straight-to-k3", straightTo3)
	cv2.imshow("post-superpix k3", secondRegularRGB)
	cv2.waitKey(0) #wait for keypress
	


# execute main()... this needs to be at the end
if __name__ == "__main__":
	testimg(sys.argv)
