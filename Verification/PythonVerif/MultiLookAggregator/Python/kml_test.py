import simplekml
import pickle
import os
from main import *
kml = simplekml.Kml()
print os.listdir(os.getcwd())
print os.getcwd()
try:
	targets = pickle.load(open("targets.p","rb"))
except:
	#print sys.argv[0]()
	print "load failed"
	targets=[]
print "Targets"
for target in targets:
	print "target"
	print target
        kml.newpoint(name=str(target.shape+","+target.scolor+","+str(round(target.long,5))+","+str(round(target.lat,5))),coords=[(target.long,target.lat)])
#kml.newpoint(name="Kirstenbogch", coords=[(18.432314,-33.988862)])
kml.save("targets.kml")
