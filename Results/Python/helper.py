from math import radians, cos, sin, asin, sqrt
import numpy as np 
import cv2
from PIL import Image
import time
from matplotlib import pyplot as plt
#import drawMatches
import copy
import pickle
import os


def xstr(s):
	"""
	Turns none into a string
	"""
	if s is None:
		return "None"
	return str(s)

def haversine(lon1, lat1, lon2, lat2):
	"""
	Calculate the great circle distance between two points 
	on the earth (specified in decimal degrees)
	"""
	# convert decimal degrees to radians 
	lon1, lat1, lon2, lat2 = map(radians, [lon1, lat1, lon2, lat2])
	# haversine formula 
	dlon = lon2 - lon1 
	dlat = lat2 - lat1 
	a = sin(dlat/2)**2 + cos(lat1) * cos(lat2) * sin(dlon/2)**2
	c = 2 * asin(sqrt(a)) 
	km = 6367 * c
	return km*3280.84 #to feet

def create_matrix(length):
	matrix=[]
	for x in range(0,length):
		inside=[]
		for y in range(0,length):
			if(y<x):
				inside.append(-1)
			else:
				inside.append(0)
		matrix.append(inside)
	return matrix

def matrix_sim(matrix,targets):
	#print enumerate(matrix)
	idx = 0
	for row in matrix:
		idy = 0
		for item in row:
			if(idy<idx):
				print "----------------------------------------------"
				print targets[idx]
				print targets[idy]
				distance = targets[idx].check_distance(targets[idy])
				element = targets[idx].check_elements(targets[idy])
				sim = distance + 100*element
				print "Distance: "+str(distance)
				print "Element#: " +str(element)
				print "Combined: "+str(sim)
				print "-----------------------------------------------"
				matrix[idx,idy] = sim
			idy = idy+1;
		idx = idx+1
	return matrix


"""

Not used right now

"""
###############################################################################

def match(element,dist):
	return dist*.999+element*.001

def cluter_matches(matches):
	copy_matches = matches[:]
	remove_matches=[]
	for match in matches:
		if match in copy_matches:
			copy_matches.remove(match)
			cpy = copy.deepcopy(match)
			print cpy
			for others in matches:
				if cpy != others and others in copy_matches:
					for num in match:
						if num in others:
							copy_matches.remove(others)
							cpy_others = copy.deepcopy(others)
							cpy_others.remove(num)
							if cpy_others[0] not in cpy:
								cpy.append(cpy_others[0])
			remove_matches.append(cpy)
	return remove_matches

def aggregate(targets,dist_matrix,element_matrix):
	matches = []
	for idx,row in enumerate(element_matrix):
			for idy,item in enumerate(row):
				if(item!=-1):
					element = item
					dist = dist_matrix[idx][idy]
					if(match(element,dist)>.0007):
						print dist, element, [idx,idy]
						matches.append([idx,idy])
	return matches
