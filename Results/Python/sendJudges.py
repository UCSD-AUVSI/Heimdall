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
import pydbscancpplib
from collections import Counter

def sendJudges():
    #format {"target":combinedTarget,"conf":confidenceLevel}
    ranked_targets = get_ranked_results()
    #get top 6
    #save as judges.txt
    with open("../judges.txt","w") as f:
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
        targets = pickle.load(open("../targets.p","rb"))
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