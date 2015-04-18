"""
Please! See http://www.tutorialspoint.com/python/tk_button.htm for some guidance on managing this GUI

TODO: check that there's any input in the entry field before pressing the button
TODO: make a help window - panel, tab...something
TODO: color status?
TODO: Flight time?
TODO: Graphic image of the gimbal angle

"""
#using Tkinter so anyone with Python installed can run it immediately
from Tkinter import * 
import tkMessageBox

#To initialize Tkinter, need to create a Tk root widget, 
#which is a window with a title bar and other decoration provided by window manager
#root widget has to be created before any other widgets and can only be one root widget
root = Tk()

#TODO: redo the confirmation windows
#Confirmation windows
def confirmBeginImaging():
	result = tkMessageBox.askquestion("Begin Imaging?", "Please confirm that you with to begin imaging")
	if result == 'yes':
		print "Beginning imaging..."
		#put method calls here
	else:
		print "Did not start imaging"
			

def confirmStopImaging():
	result = tkMessageBox.askquestion("Stop Imaging?", "Please confirm that you with to end imaging")
	if result == 'yes':
		print "Stopping imaging"
		#put method calls here
	else:
		print "Did not stop imaging"

def confirmBeginMission():
	result = tkMessageBox.askquestion("Begin Mission?", "Please confirm that you wish to begin the mission")
	if result == 'yes':
		print "Beginning mission"
		#put method calls here	
	else:
		print "Did not begin mission"

def confirmEndMission():
	result = tkMessageBox.askqintuestion("End Mission?", "Please confirm mission termination")
	if result == 'yes':
		print "Ending mission. Landing plane"
		#put method calls here
	else:
		print "Did not end mission"

def confirmSendImagesToJudges():
	result = tkMessageBox.askquestion("Send images to judges?", "Please confirm that you wish to send captured images for judging")
	if result == 'yes':
		print "Sending Images"
		#put method calls here
	else:
		print "Did not send images to judges"

def confirmGetReimagingWayPoints():
	result = tkMessageBox.askquestion("Get reimaging waypoints?", "Please confirm that you with to get reimaging waypoints")
	if result == 'yes':
		print "Reimaging waypoints"
		#put method calls here
	else:
		print "Did not reimage waypoints"

def handler():
    if tkMessageBox.askquestion("Quit?", "Are you sure you want to quit? This will end the mission."):
        root.quit()

#Name of panel
root.title("UCSD AUVSI Human Operator GUI")	
root.protocol("WM_DELETE_WINDOW", handler)

##Headers
systemTitle = Label(root, text = "System", font = "bold").grid(row = 0, column = 0)
systemStatus = Label(root, text = "Status", width = 15, font = "bold").grid(row = 0, column = 3)
#System Names
mDIPA = Label(root, relief = RIDGE, text = "Mission Director IP", width = 18).grid(row= 1, column = 0)
missionDirector = Label(root, relief = RIDGE, text = "Mission Director", width = 18).grid(row=2, column = 0)
planeOBCIP = Label(root, relief = RIDGE, text = "Plane OBC IP", width = 18).grid(row=3, column = 0)
planeOBC = Label(root, relief = RIDGE, text = "Plane OBC", width = 18).grid(row=4, column = 0)
heimdallIP = Label(root, relief = RIDGE, text = "Heimdall IP", width = 18).grid(row=5, column = 0)
heimdall = Label(root, relief = RIDGE, text = "Heimdall", width = 18).grid(row=6, column = 0)
mavProxyIP = Label(root, relief = RIDGE, text = "Mav Proxy IP", width = 18).grid(row = 7, column = 0)
mavProxy = Label(root, relief = RIDGE, text = "Mav Proxy", width = 18).grid(row = 8, column = 0)
gimbalAngle = Label(root, relief = RIDGE, text = "Current Gimbal Angle", width = 18).grid(row = 9, column = 0)

#Status Labels
mDIPAStatus = Label(root, text = "Not connected", width = 30).grid(row=1, column = 3)
missionDirectorStatus = Label(root, text = "Default Status").grid(row=2, column = 3)
planeOBCIPStatus = Label(root, text = "Not connected").grid(row=3, column = 3)
planeOBCStatus = Label(root, text = "Default Status", width = 30).grid(row=4, column = 3)
heimdallIPStatus = Label(root, text = "Not connected").grid(row=5, column = 3)
heimdallStatus = Label(root, text = "Default Status").grid(row=6, column = 3)
mavProxyIPStatus = Label(root, text = "Not connected").grid(row=7, column = 3)
mavProxyStatus  = Label(root, text = "Default Status").grid(row=8, column = 3)
gimbalStatus = Label(root, text = "Default Status").grid(row = 9, column = 3)
	
#####Entry fields
mDIPAEntry = Entry(root, width = 30).grid(row = 1, column = 1)
missionDirectorEntry = Entry(root, width = 30).grid(row = 2, column = 1)
planeOBCIPEntry = Entry(root, width = 30).grid(row = 3, column = 1)
planeOBCEntry = Entry(root, width = 30).grid(row = 4, column = 1)
heimdallIPEntry = Entry(root, width = 30).grid(row = 5, column = 1)
heimdallEntry = Entry(root, width = 30).grid(row = 6, column = 1)
mavProxyIPEntry = Entry(root, width = 30).grid(row = 7, column = 1)
mmavProxyEntry = Entry(root, width = 30).grid(row = 8, column = 1)
gimbalEntry = Entry(root, width = 30).grid(row = 9, column = 1)

#####Buttons
connectButton = Button(root, text = "Connect", width = 5).grid(row = 1, column = 2)
sendButton = Button(root, text = "Send", width = 5).grid(row = 2, column = 2)
connect2Button = Button(root, text = "Connect", width = 5).grid(row = 3, column = 2)
send2Button = Button(root, text = "Send", width = 5).grid(row = 4, column = 2)
connect3Button = Button(root, text = "Connect", width = 5).grid(row = 5, column = 2)
send3Button = Button(root, text = "Send", width = 5).grid(row = 6, column = 2)
connect4Button = Button(root, text = "Connect", width =5).grid(row = 7, column = 2)
send4Button = Button(root, text = "Send", width= 5).grid(row = 8, column = 2)
send7Button = Button(root, text = "Send", width = 5).grid(row = 9, column = 2)

beginMissionButton = Button(root, text = "Begin Mission", width = 15, 
	command = confirmBeginMission).grid(row = 10, column = 0)
endMissionButton = Button(root, text = "End Mission", width = 15, 
	command = confirmEndMission).grid(row = 10, column = 1)
startImagingButton = Button(root, text = "Start imaging", width = 15, 
	command = confirmBeginImaging).grid(row = 11, column = 0)
stopImagingButton = Button(root, text = "Stop imaging", width = 15, 
	command = confirmStopImaging).grid(row = 11, column = 1)
sendImagesButton = Button(root, text = "Send images to judges", width = 15, 
	command = confirmSendImagesToJudges).grid(row = 12, column = 0)
getReimageWaypointsButton = Button(root, text = "Get re-image waypoints", width = 15, 
	command = confirmGetReimagingWayPoints).grid(row = 12, column = 1)

root.mainloop(), 
