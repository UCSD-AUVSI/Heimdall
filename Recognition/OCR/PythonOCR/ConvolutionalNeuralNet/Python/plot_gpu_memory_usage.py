import subprocess
import time
import matplotlib.pyplot as plt

X = []
Y = []

sleeptime = 0.01
graphlength = 30
plottedtime = 0.0

fig = plt.figure()
fig.show()
starttime = -1000001.0

while True:
	endtime = time.clock()
	if starttime > -1000000.0:
		plottedtime = plottedtime + (endtime - starttime)
	starttime = time.clock()
	
	process = subprocess.Popen(['nvidia-smi'], stdout=subprocess.PIPE)
	out, err = process.communicate()
	outspl = str(out).split('\n')
	
	linecounter = 0
	linewithmemusage = -1
	for line in outspl:
		linecounter = linecounter + 1
		if 'Memory-Usage' in line:
			linewithmemusage = linecounter + 3
		if linecounter == linewithmemusage and 'MiB' in line:
			linespl = line.split()
			memusagethistime = str(linespl[8]).replace('MiB','')
			print(memusagethistime)
			X.append(plottedtime)
			Y.append(int(memusagethistime))
			break
	
	def PopFrontToLength(thevar, thelen):
		if len(thevar) > thelen:
			extrapts = len(thevar) - thelen
			del thevar[:extrapts]
		return thevar
	
	X = PopFrontToLength(X, min(1000,int(graphlength/sleeptime)))
	Y = PopFrontToLength(Y, min(1000,int(graphlength/sleeptime)))
	
	plt.clf()
	plt.plot(X,Y)
	plt.ylabel('GPU mem (MiB)')
	fig.canvas.draw()
	
	time.sleep(sleeptime)


