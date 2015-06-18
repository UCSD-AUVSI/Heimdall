import sys
from Networking import server_multiport
from Networking import ports
from Clients.MissionDirector import listener_MissionDirector
from kmllistener import globalvar_connection_kml_create as KMLCreate

#-----------------------------------------------------------
# main(): setup and start server
#
def main(argv):
	if len(argv) < 1:
		print "usage: {ip-address-for-listen}"
		quit()
	
	#KMLCreate.connection.threadedconnect()

	# Setup several parallel listeners
	ports_and_callbacks = []
	ports_and_callbacks.append((ports.listenport_MissionDirector, listener_MissionDirector.callback, server_multiport.SSLSecurityDetails(False)))
	ports.IPaddr_MissionDirector = argv[0]	
	# Start server and wait here for keyboard interrupt
	s = server_multiport.server()
	s.start(ports_and_callbacks,argv[0], True, True)


#-----------------------------------------------------------
# execute main()... this needs to be at the end
#
if __name__ == "__main__":
	main(sys.argv[1:])







