from math import radians, cos, sin, asin, sqrt
import numpy as np 
import cv2
from PIL import Image
import time
import copy
from operator import itemgetter
import pickle
import os
from collections import Counter

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
