import numpy as np
import cv2


def tryQRCodeRead(givenSaliencyCroppedObjectImage, optionalArgs):
	
	print "Python QR Code Reader (this is the Python) -- tryQRCodeRead()"
	
	cv2.imshow("givenSaliencyCroppedObjectImage", givenSaliencyCroppedObjectImage)
	
	readerSuccessfullyReadMessage = False
	
	if readerSuccessfullyReadMessage:
		return (True, "secret-message-read-by-qr-code-library")
	else:
		return (False, "")

