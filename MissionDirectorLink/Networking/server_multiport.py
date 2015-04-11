#-----------------------------------------------------------
# Multi-port parallel listen server; use start_server()
# Waits for keyboard interrupt
#

import select, socket, sys, time, threading
import json


#-------------------------------------------------------------------
# Starts a listen server, listening on as many ports as you provide
#
# Argument: a list of 2-tuples, each has: (port, callbackfunction)
#
# When messages are received on those ports, the callbacks are
#   called with the data that was provided in the message
#
class server:
	
	def __init__(self):
		self.keep_running = threading.Event()
		self.threads = []
		self.readytostart = True
	
	def stop(self):
		print "Beginning server shutdown..."
		self.keep_running.clear()
		for thread in self.threads:
			thread.join()
		print "Server done shutting down."
		self.readytostart = True
	
	def start(self, ports_and_callbacks, wait_for_interrupt):
		
		if self.readytostart == True:
			self.readytostart = False
			
			# this will maintain an "on" state, that will shut down the threads when switched
			self.keep_running.set()
			
			self.threads = []
			for port_and_callback in ports_and_callbacks:
				self.threads.append(threading.Thread(target=start_port_listener, args=(port_and_callback[0], port_and_callback[1], self.keep_running)))
			for thread in self.threads:
				thread.daemon = True
				thread.start()
			
			print "Server is listening!"
		
			if wait_for_interrupt:
				try:
					while True:
						time.sleep(0.1)
				except KeyboardInterrupt:
					self.stop()




#-----------------------------------------------------------
# Listen on one port (don't use this outside this file)
# When a message is received, give it to the callback
#
def start_port_listener(port, callback, keep_running_until_interrupt):
	
	listensocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	listensocket.bind(("localhost", port))
	listensocket.listen(0) # 0 for debugging; 3-5 for release version
	
	while keep_running_until_interrupt.is_set():
		
		rr,rw,err = select.select([listensocket],[],[],1)
		if rr:
			#print "port "+str(port)+" is waiting for a new client!"
			(clientsocket, caddress) = listensocket.accept()
			#print "port "+str(port)+" found a client at address: ", caddress
			
			while keep_running_until_interrupt.is_set(): #keep receiving from this client until client shuts down
				#print "port "+str(port)+" is waiting for data from client..."
				data = clientsocket.recv(1024)
				if not data: break
				#print "port "+str(port)+" received data from client: \"" + str(data) + "\""
				
				# turn json string into dictionary
				callback(data)
			
			clientsocket.close()
	
	listensocket.close()
	print "Socket on port "+str(port)+" closed!"

