from qrtools import QR
myCode = QR(filename =u"/home/cameron_ellis/Desktop/sample_QR_codes/qrcode_grass.jpg")
if myCode.decode():
	print ("Data type: " + myCode.data_type + ": " + myCode.data)

#made possible through this website
#https://ralgozino.wordpress.com/2011/06/13/how-to-create-and-decode-a-qr-code-in-python-using-qrtools/
