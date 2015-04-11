
import json
import os

#-----------------------------------------------------------
#
def callback(data):
	print "received message from Heimdall: \"" + str(data) + "\""
	print "todo: handle messages from Heimdall"

	json_data = json.loads(data)
	command = json_data["command"]
	json_data = json_data["send"]
