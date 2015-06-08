import cv2
import libpypolyshaperec
import sys
sysargs = sys.argv[1:]
if len(sysargs) == 0:
	print("usage:  {sseg-image}")
	quit()

ssegimg = cv2.imread(sysargs[0], cv2.CV_LOAD_IMAGE_GRAYSCALE)

shapefound = libpypolyshaperec.doBPyShapeRec([ssegimg])

print("Shaperec found: \""+shapefound+"\"")

