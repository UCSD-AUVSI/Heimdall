
import json
import os
from Networking.send_message_to_client import send_message_to_client
from Networking import ports
import time
from kmllistener import globalvar_connection_kml_create as KMLCreate

#-----------------------------------------------------------
#
def callback(data,FromIPaddr):
	print "recieved message from ground station: \""+ str(data) + "\""

	json_data = json.loads(data)
	command = json_data["cmd"]
	args = json_data["args"]

	if command == "status":
		if "hello" in args:
			send_message_to_client(json.dumps({"cmd":"status","args":{"hellojason":"hallo"}}),ports.outport_MissionDirector, ports.IPaddr_MissionDirector)
	if command == "get_top_filename":
		# get top filename
		output = {}
		output["cmd"] = "send_image_path"
		output["args"] = {}
		output["args"]["filename"] = "/images/BestPic.jpg"

		send_message_to_client(json.dumps(output),ports.outport_MissionDirector, ports.IPaddr_MissionDirector)
		print "forwarded message from Heimdall to MissionDirector"
	
	if command == "telemdata":
		# update the results kml file
		KMLCreate.addGPSCoord(args)
		print "gps added"

	if command == "send_judges":
		# use sendjudges.sh to send create the text file
		path = "../Results/Python/"
		print path+"sendjudges.sh "+path
		os.system(path+"sendjudges.sh "+path)	
