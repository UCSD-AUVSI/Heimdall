import sys
from Networking import server_multiport
from Networking import ports
from Clients.MissionDirector import listener_MissionDirector
from kmllistener import globalvar_connection_kml_create as KMLCreate

#-----------------------------------------------------------
# main(): setup and start server
#
def main(argv):
	
	KMLCreate.connection.threadedconnect()

	# Setup several parallel listeners
	ports_and_callbacks = []
	ports_and_callbacks.append((ports.listenport_MissionDirector, listener_MissionDirector.callback))
	
	# Start server and wait here for keyboard interrupt
	s = server_multiport.server()
	s.start(ports_and_callbacks, True)


#-----------------------------------------------------------
# execute main()... this needs to be at the end
#
if __name__ == "__main__":
	main(sys.argv[1:])







