import math
import numpy
import cv2

def CvtCharToClass(chargiven):
	charclass = ord(chargiven)
	if charclass > 64:
		charclass -= 55
	elif charclass > 47:
		charclass -= 48
	else:
		charclass = -1
	return charclass

def warpImg(image, imageClass, forOrientationPureDetector = False):
	# args:
	# image
	# forOrientationPureDetector -- if true, will rotate anywhere from 0 to 360 degrees, and return how much it was rotated (rounded to increments of 5 degrees)
	#                         if false, will only do small rotations, on the order of +/- 5 degrees
	
	# TODO:
	#	warp based on character class:
	#	some characters can be mirrored (horizontally or vertically depending on the character)
	#	that would hugely help
	
	center = tuple(numpy.array(image.shape)/2)
	shape = tuple(image.shape)
	if len(shape) > 2:
		shape = (shape[0],shape[1])
	
	imageClass = (imageClass % 145)
	if imageClass < 144: #
		imageClass = (imageClass % 36)
		
		if numpy.random.randint(0,2) == 0:
			horizflippers = [CvtCharToClass(i) for i in ['0','8','A','H','I','M','O','T','U','V','W','X','Y']]
			if imageClass in horizflippers:
				image = numpy.fliplr(image)
		
		if numpy.random.randint(0,2) == 0:
			vertiflippers = [CvtCharToClass(i) for i in ['0','3','8','B','C','D','E','H','I','K']]
			if imageClass in vertiflippers:
				image = numpy.flipud(image)
		
		if numpy.random.randint(0,2) == 0:
			bothflippers = [CvtCharToClass(i) for i in ['0','8','H','I','N','O','S','X','Z']]
			if imageClass in bothflippers:
				image = numpy.flipud(numpy.fliplr(image))
	
	######totalwarp = numpy.array([[1., 0., 0.], [0., 1., 0.]])
	
	angleScalar = 0.9
	angleoffset = 0.0
	if forOrientationPureDetector:
		angleScalar = 0.02
		angleoffset = numpy.random.uniform(0.0, 360.0)  #random rotation
	
	anglenoise = angleScalar * numpy.random.uniform(-6.8, 8.0)  #negative angles make it look more italicized
	
	angrad = math.radians(anglenoise + angleoffset)
	shearA = numpy.random.uniform(-3.0,3.7)/float(center[0])  #negative A makes it look more italicized
	shearB = numpy.random.uniform(-3.7,3.0)/float(center[0])  #positive B makes it look slightly more italicized
	maxTransDist = 0.19*float(center[0])
	translateX = numpy.random.uniform(-maxTransDist, maxTransDist)
	translateY = numpy.random.uniform(-maxTransDist, maxTransDist)
	rescaleAmt = numpy.random.uniform(0.5,1.4) #if scaled slightly too big, that's OK; that's how AlexNet works anyway (it crops to ~85% size, discarding the other ~15%)
	
	# now create 3 affine transformation matrices, one for each warp type (translate, rotate, shear), and then combine them
	# order: translate to centered about origin; rotate; rescale; shear; translate back and offset
	
	transmatFirst = numpy.array([[1., 0.,-center[0]], [0., 1.,-center[1]], [0., 0., 1.]])
	transmatLastt = numpy.array([[1., 0., center[0]+translateX], [0., 1., center[1]+translateY], [0., 0., 1.]])
	rotation = numpy.array([[math.cos(angrad), math.sin(angrad), 0.], [-math.sin(angrad), math.cos(angrad), 0.], [0., 0., 1.]])
	shear = numpy.array([[1., shearA, 0.],[shearB, 1., 0.],[0.,0.,1.]])
	rescale = numpy.array([[rescaleAmt, 0., 0.], [0., rescaleAmt, 0.], [0., 0., 1.]])
	
	totalwarp = numpy.dot(transmatLastt, numpy.dot(shear, numpy.dot(rescale, numpy.dot(rotation,transmatFirst))))[:2,:]
	
	
	if forOrientationPureDetector:
		return (cv2.warpAffine(image, totalwarp, dsize=shape, flags=cv2.INTER_LINEAR), round(angleoffset / 5.0)) #every 5 degrees is a new class
	else:
		return cv2.warpAffine(image, totalwarp, dsize=shape, flags=cv2.INTER_LINEAR)

