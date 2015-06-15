import simplekml
import pickle
import os
from target import *
while True:
	time.sleep(.7)
	kml = simplekml.Kml()
	try:
		targets = pickle.load(open("../targets.p","rb"))
	except:
		#print sys.argv[0]()
		print "load failed"
		targets=[]
		continue
	print "Targets"
	for target in targets:
		print "target"
		print target
        	kml.newpoint(name=str(target.shape+","+target.scolor+","+str(round(target.long,5))+","+str(round(target.lat,5))),coords=[(target.long,target.lat)])
#kml.newpoint(name="Kirstenbogch", coords=[(18.432314,-33.988862)])
	kml.save("../targets.kml")
