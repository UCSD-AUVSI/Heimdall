from main import *

while True:
	test_point=bgr_to_lab(np.random.randint(0,255,(1,3)))
	test_point2=bgr_to_lab(np.random.randint(0,255,(1,3)))
	doColorClassification(test_point,test_point2,0)
