
import json
import os
from Networking.send_message_to_client import send_message_to_client
from Networking import ports
import time
from kmllistener import globalvar_connection_kml_create as KMLCreate

#-----------------------------------------------------------
#
def callback(data):
	print "recieved message from ground station: \""+ str(data) + "\""

	json_data = json.loads(data)
	command = json_data["command"]
	args = json_data["args"]


	if command == "get_top_filename":
		# get top filename
		output = {}
		output["cmd"] = "send_image_path"
		output["filename"] = "/images/BestPic.jpg"
		time.sleep(10)

		send_message_to_client(json.dumps(output),9981)
		print "forwarded message from Heimdall to MissionDirector"
	
	if command == "telemdata":
		# update the results kml file
		KMLCreate.addGPSCoord(args)
		print "gps added"

