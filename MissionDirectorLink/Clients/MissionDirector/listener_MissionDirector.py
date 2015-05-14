
import json
import os
from Networking.send_message_to_client import send_message_to_client
from Networking import ports
import time

#-----------------------------------------------------------
#
def callback(data):
	json_data = json.loads(data)
	cmd = json_data["cmd"]

	print "\nreceived "+cmd +" message from MissionDirector"
	print json_data

	if cmd == "get_top_filename":
		# get top filename
		output = {}
		output["cmd"] = "send_image_path"
		output["filename"] = "/images/BestPic.jpg"
		time.sleep(10)

		send_message_to_client(json.dumps(output),9981)
		print "forwarded message from Heimdall to MissionDirector"
