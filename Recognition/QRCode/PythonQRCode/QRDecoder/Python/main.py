from qrtools import QR

def doQRDecoder(fileLocation):
	myCode = QR(filename = fileLocation)
	if myCode.decode():
		print ("Data type: " + myCode.data_type + ": " + myCode.data)

#made possible through this website
#https://ralgozino.wordpress.com/2011/06/13/how-to-create-and-decode-a-qr-code-in-python-using-qrtools/
