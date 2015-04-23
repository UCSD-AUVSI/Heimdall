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
import pydbscancpplib
from collections import Counter



def doVerif(ShapeColorVals, ShapeColorStr, ShapeName, CharColorVals, CharColorStr, CharName, TargetLat, TargetLong, TargetOrientation, OriginalImageFilename, imageData, askFJorClusrters):    
    
    # create the new target
    new_target = Target(scolor = ShapeColorStr,shape=ShapeName,shape_conf=1, ccolor = CharColorStr, char = CharName,char_confs=[1], lat = TargetLat, long = TargetLong)
    
    # create directory and add image to it
    path = "output_images_directory"
    if not os.path.isdir(path):
        os.mkdir(path,0755)
    new_file_name = OriginalImageFilename+"_"+str(TargetLat)+"_"+str(TargetLong)+".jpg"
    #save target
    cv2.imwrite(os.path.join(path, new_file_name), imageData)

    # pass the target to the results
    add_target(new_target)

def sendJudges():
    #format {"target":combinedTarget,"conf":confidenceLevel}
    ranked_targets = get_ranked_results()
    #get top 6
    #save as judges.txt
    with open("judges.txt","w") as f:
        f.write('{:>12}  {:>12}  {:>12} {:>12} {:>12} {:>12}\n'.format("ShapeName", "ShapeColor", "Character", "CharColor","Lat","Long"))
        for x in range(0,6):
            if(len(ranked_targets)>x):
                f.write(ranked_targets[x].judges_output())



def getTopLoc():
    # send top 50 lat long to other module
    # Change to inlcude all targets not just clusters
    top_results=[]
    ranked_targets = get_ranked_results()
    for x in range(0,50):
        if(len(ranked_targets)>x):
            top_results.append([ranked_targets[x].lat,ranked_targets[x].long])

    return top_results
    


class Target():
    """
    Defines a target with shape, shape color, char, char color, lat ,and long
    """
    def __init__(self,scolor=None,shape=None,shape_conf=0,ccolor=None,char=None,char_confs=None,lat=0,long=0,overall_conf=0):
        self.shape=shape
        self.shape_conf = shape_conf
        self.scolor=scolor
        self.ccolor=ccolor
        self.char=char
        self.char_confs = char_confs
        self.lat=lat
        self.long=long
        self.overall_conf=overall_conf
    def check_distance(self,target):
        """
        Calculate distance between two targets
        """
        dist = haversine(target.long,target.lat,self.long,self.lat)
        if(dist==0):
            dist = .001
	print "Pre dist: "+str(dist)
        return dist
    def check_elements(self,target):
        """
        Calculates similarity between two targets attributes
        """
        total=0
        if(self.shape == target.shape and self.shape != ""):
            total+=1
        if(self.char == target.char and self.char != ""):
            total+=1
        if(self.scolor == target.scolor and self.scolor != ""):
            total+=1
        if(self.ccolor == target.ccolor and self.ccolor != ""):
            total+=1
        return (0.5 - total/4)
    def __str__(self):
        """
        Prints out the target
        """
        return '{:>12}  {:>12}  {:>12} {:>12} {:>12} {:>12}'.format(xstr(self.shape), xstr(self.scolor), xstr(self.char), xstr(self.ccolor), str(self.lat), str(self.long))
    def judges_output(self):
        """
        Prints judges output
        """
        return '{:>12}  {:>12}  {:>12} {:>12} {:>12} {:>12}\n'.format(xstr(self.shape), xstr(self.scolor), xstr(self.char), xstr(self.ccolor),xstr(self.lat),xstr(self.long))

def guess_target(targets):
    """
    Return combined target from a cluster of targets
    with a confidence
    """
    # get list of each attribute
    shapes=[]
    scolors=[]
    chars=[]
    ccolors=[]
    total = 0.0
    for target in targets:
        total = total+1.0
        shapes.append(target.shape)
        scolors.append(target.scolor)
        chars.append(target.char)
        ccolors.append(target.ccolor)

    # get the chars in a dict
    chars_confs = aggregateList(chars)
    shapes_conf = aggregateList(shapes)
    scolors_conf= aggregateList(scolors)
    ccolors_conf =  aggregateList(ccolors)

    top_char = highestItem(chars_confs)
    top_shape = highestItem(shapes_conf)
    top_sc = highestItem(scolors_conf)
    top_cc = highestItem(ccolors_conf)

    print top_char
    print top_shape
    print top_sc
    print top_cc
    print (top_char["conf"]*top_shape["conf"]*top_sc["conf"]*top_cc["conf"])
    print total
    #calc overall confidence
    overall_conf = (top_char["conf"]*top_shape["conf"]*top_sc["conf"]*top_cc["conf"])/total
    #create the combine Target
    combined = Target(scolor=top_sc["name"],shape =top_shape["name"], shape_conf =1 , ccolor=top_cc["name"],
        char=top_char["name"],char_confs =1, lat=targets[0].lat,long=targets[0].long, overall_conf=overall_conf)

    #return the target and confidence
    return combined

def aggregateList(lst):
    """
    Return most common item in a list with its occurance amount
    """
    common = {}
    total = 0
    #place items in the dict with how often they are seen
    for item in lst:
        total = total+1
        if item in common:
            common[item] = common[item]+1;
        else:
            common[item] = 1;

    return common

def highestItem(common):
    #get greatest item in dict
    highest = 0
    name= ""
    for result in common:
        if(common[result]>=highest):
	    if result != "":
            	highest = common[result]
            	name = result
    if name== "":
        highest = 0
    if highest == 0:
        highest = .5
    return {"name":name,"conf":highest}



def get_ranked_results():
    """
    Gets clusters of targets, ranks them and merges the targets into one
    returns the list
    """
    try:
        targets = pickle.load(open("targets.p","rb"))
    except:
        targets=[]

    print "Targets"
    for target in targets:
        print target
    print ""
    # create dist matrix
    matrix = np.zeros((len(targets),len(targets)), np.float32)
    sim_matrix = matrix_sim(matrix,targets)

    if len(sim_matrix) > 3 :
        #cluster targets
        returnedClusters = pydbscancpplib.TargetClusterDBSCAN(sim_matrix, 1, 200)
        outputTargets=[]
        for cluster in returnedClusters:
            print "Cluster"
            clusterTargets = []
            for item in cluster:
                print targets[item]
                clusterTargets.append(targets[item])
            print ""
            #find what we think the target is
            outputTargets.append(guess_target(clusterTargets))

        #rank targets by conf
        rankedTargets = sorted(outputTargets, key=lambda target:target.overall_conf,reverse=True)
        print "Final Targets List with Confidence"
        count = 0
        for item in rankedTargets:
            print str(item) + " conf="+ str(item.overall_conf)
            count = count +1



    return rankedTargets

def add_target(new_target):
    """
    Takes in new target saves targets with pickle
    """
    # get old targets
    try:
        targets = pickle.load(open("targets.p","rb"))
    except:
        targets=[]
    #append new target
    targets.append(new_target)
    
    #save targets as pickle
    pickle.dump(targets,open('targets.p',"wb"))
