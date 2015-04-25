from math import radians, cos, sin, asin, sqrt
import numpy as np 
import cv2
from PIL import Image
import time
import copy
from operator import itemgetter
import pickle
import os
from helper import *
from target import *
from collections import Counter
import json

PATH = "Results/"

def doVerif(ShapeColorVals, ShapeColorStr, ShapeName, CharColorVals, CharColorStr, CharName, TargetLat, TargetLong, TargetOrientation, OriginalImageFilename, imageData, askFJorClusrters):    
    
    # create the new target
    new_target = Target(scolor = ShapeColorStr,shape=ShapeName,shape_conf=1, ccolor = CharColorStr, char = CharName,char_confs=[1], lat = TargetLat, long = TargetLong)
    
    # create directory and add image to it
    path = PATH + "images"
    if not os.path.isdir(path):
        os.mkdir(path,0755)
    new_file_name = str(ShapeName)+"_"+ShapeColorStr+"_"+str(TargetLat)+"_"+str(TargetLong)+".jpg"
    #save target
    cv2.imwrite(os.path.join(path, new_file_name), imageData)

    # pass the target to the results
    add_target(new_target)



def add_target(new_target):
    """
    Takes in new target saves targets with pickle
    """
    # get old targets 
    try:
        targets = pickle.load(open(PATH+"targets.p","rb"))
    except:
        targets=[]
    #append new target
    targets.append(new_target)
    
    objects = []
    for target in targets:
        objects.append(target.__dict__)
    #json_objects = json.dumps(objects)
    with open(PATH+ 'targets.json', 'wb') as outfile:
        json.dump(objects, outfile)

    #save targets as pickle
    pickle.dump(targets,open(PATH+'targets.p',"wb"))
