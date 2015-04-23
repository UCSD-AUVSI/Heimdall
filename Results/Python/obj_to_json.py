import pickle
import os
from target import *
import json
try:
	targets = pickle.load(open("../targets.p","rb"))
except:
	#print sys.argv[0]()
	print "load failed"
	targets=[]
objects = []
for target in targets:
	objects.append(target.__dict__)
#json_objects = json.dumps(objects)
with open('../targets.json', 'w') as outfile:
    json.dump(objects, outfile)
