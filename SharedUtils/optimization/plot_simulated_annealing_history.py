import sys
if len(sys.argv) < 2:
	print("usage:  {historyfile}")
	quit()

iters = []
scores = []
tempscalars = []
negacceptrates = []

numloops = 0
with open(sys.argv[1],'r') as histfile:
	for line in histfile:
		if 'score: ' in line and ', numLoopsDone: ' in line and numloops < 700:
			splitted = line.replace(': ', ', ').replace('\n',', ').split(', ')
			#print(str(splitted))
			scores.append(float(splitted[1]))
			iters.append(float(int(splitted[3])))
			tempscalars.append(0.5*float(splitted[7]))
			negacceptrates.append(0.01*float(splitted[9]))
			numloops = (numloops + 1)

import matplotlib.pyplot as plt

###plt.subplot(121)

plt.plot(iters,scores,c='b',label='score')
plt.plot(iters,tempscalars,c='r',label='temp. scalar')
plt.plot(iters,negacceptrates,c='g',label='neg. acc. rate')
###plt.legend(bbox_to_anchor=(1.05, 1), loc=2, borderaxespad=0.)
plt.xlabel('iteration')
plt.ylabel('metric score')
plt.title('Simulated Annealing Optimization')
plt.show()

