import socket, threading
import json

#todo: don't always send to "localhost"


def send_message_to_client(msg, port, IPaddr="localhost"):
	
	# Use this to dispatch the message to another thread so the main thread can't freeze
	#thread = threading.Thread(target=private___dispatch_msg, args=(msg))
	#thread.daemon = True
	#thread.start()
	
	# Send message using the main thread (TCP may freeze)
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((IPaddr,port))
	s.send(msg)
	s.close()



#--------------------------------------------------------------------------------------
# Use this to dispatch the message to another thread so the main thread can't freeze
#
def private___dispatch_msg(msg, port, IPaddr="localhost"):
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((IPaddr,port))
	s.send(msg)
	s.close()
