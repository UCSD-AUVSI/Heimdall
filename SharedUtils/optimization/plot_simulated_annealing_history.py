import sys
if len(sys.argv) < 2:
	print("usage:  {historyfile}")
	quit()

X = []
Y = []

with open(sys.argv[1],'r') as histfile:
	for line in histfile:
		if 'score: ' in line and ', numLoopsDone: ' in line:
			splitted = line.replace(': ', ', ').split(', ')
			#print(str(splitted))
			X.append(float(int(splitted[3])))
			Y.append(float(splitted[1]))

import matplotlib.pyplot as plt
plt.plot(X,Y)
plt.show()
