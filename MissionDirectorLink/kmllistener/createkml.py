import os
import threading
import time
from collections import deque
from target import *
import pickle
import simplekml

PATH = "../Results/Python/"

class CreateKML(object):
	
	def __init__(self):
		self.listener_started = False
		self.create_lock = threading.Lock()
		self.gps_queue = deque([])	
	
	def threadedconnect(self):
		if self.listener_started == False:
			self.listener_started = True
			self.mythread = threading.Thread(target=self.run)
			self.mythread.daemon = True
			self.mythread.start()
	
	def run(self):
		while True:
			self.create()
			time.sleep(.5)		

	def addGPSCoord(self,gps_coord):
		if len(list(gps_queue)) >= 10:
			self.gps_queue.pop()
		self.gps_queue.appendleft(gps_coord)	
		
	def create(self):
		self.create_lock.acquire()
		#create
		kml = simplekml.Kml()	
		print "creating"	
		# get current queue as list
		gps_list = list(self.gps_queue)

		for gps in gps_list:
		        kml.newpoint(coords=[(gps.lon,gps.lat)])
		# get pickle list
		try:
			targets = pickle.load(open(PATH+"targets.p","rb"))
		except:
			print "load failed"
			targets = []
		for target in targets:
		        kml.newpoint(name=str(target.shape+","+target.scolor+","+str(round(target.long,5))+","+str(round(target.lat,5))),coords=[(target.long,target.lat)])
		
		kml.save(PATH+"targets.kml")	
		print "kml saved"	
		self.create_lock.release()					
