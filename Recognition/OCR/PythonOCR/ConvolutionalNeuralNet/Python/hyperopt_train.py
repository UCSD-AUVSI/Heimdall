import hyperopt
from hyperopt import hp
import time, os, subprocess

def checkIsFloat(instr):
    try:
        a = float(instr)
        return True
    except ValueError:
        return False
    return False

def TryTest(arrgs):
    
    l3parms, l2parms, l1nfilt, l1fsize, l1fpool, l0nfilt, l0fsize, l0fpool, fc1nn, fc2nn = arrgs
    
    print("=====================================================================================================")
    print(str(arrgs))
    print("=====================================================================================================")
    
    l3nfilt = l3parms['l3nfilt']
    l3fsize = l3parms['l3fsize']
    l3fpool = l3parms['l3fpool']
    l2nfilt = l2parms['l2nfilt']
    l2fsize = l2parms['l2fsize']
    l2fpool = l2parms['l2fpool']
    
    #l0nfilt, l0fsize, l0fpool, l1nfilt, l1fsize, l1fpool, l2nfilt, l2fsize, l2fpool, l3nfilt, l3fsize, l3fpool, fc1nn, fc2nn
    #str(int(l0nfilt)), str(int(l0fsize)), str(int(l0fpool)), str(int(l1nfilt)), str(int(l1fsize)), str(int(l1fpool)), str(int(l2nfilt)), str(int(l2fsize)), str(int(l2fpool)), str(int(l3nfilt)), str(int(l3fsize)), str(int(l3fpool)), str(int(fc1nn)), str(int(fc2nn))
    
    currfilepath = os.path.dirname(os.path.abspath(__file__))
    coutgiven = subprocess.check_output(['python', 'convolutional_mlp_ch74k.py', 'train2', '0.05', '0.5', str(int(l0nfilt)), str(int(l0fsize)), str(int(l0fpool)), str(int(l1nfilt)), str(int(l1fsize)), str(int(l1fpool)), str(int(l2nfilt)), str(int(l2fsize)), str(int(l2fpool)), str(int(l3nfilt)), str(int(l3fsize)), str(int(l3fpool)), str(int(fc1nn)), str(int(fc2nn))], cwd=currfilepath)
    
    foundnumerics = 0
    lastnumeric = 1.0
    for line in coutgiven.splitlines():
        print(line)
        if checkIsFloat(line):
            foundnumerics += 1
            lastnumeric = float(line)
        else:
            foundnumerics = 0
    if foundnumerics == 4:
        print("~=~=~=~=~=~= result: "+str(lastnumeric))
        print("~=~=~=~=~=~= params: "+str(arrgs))
        return lastnumeric
    return 1.0

#l0nfilt, l0fsize, l0fpool, l1nfilt, l1fsize, l1fpool, l2nfilt, l2fsize, l2fpool, l3nfilt, l3fsize, l3fpool, fc1nn, fc2nn

myspace = [
            hp.choice('l3', [{'l3nfilt': -1, 'l3fsize': -1, 'l3fpool': -1,},
                             {'l3nfilt': hp.quniform('l3nfilt', 50, 1000, 1), 'l3fsize': hp.quniform('l3fsize', 1, 7, 1), 'l3fpool': hp.quniform('l3fpool', 1, 3, 1),},
                                 ]),
            hp.choice('l2', [{'l2nfilt': -1, 'l2fsize': -1, 'l2fpool': -1,},
                             {'l2nfilt': hp.quniform('l2nfilt', 50, 1000, 1), 'l2fsize': hp.quniform('l2fsize', 1, 7, 1), 'l2fpool': hp.quniform('l2fpool', 1, 3, 1),},
                                 ]),
            hp.quniform('l1nfilt', 50, 1000, 1), hp.quniform('l1fsize', 1, 7, 1), hp.quniform('l1fpool', 1, 3, 1),
            hp.quniform('l0nfilt', 50, 1000, 1), hp.quniform('l0fsize', 1, 7, 1), hp.quniform('l0fpool', 1, 3, 1),
            hp.quniform('fc1nn', 50, 3000, 1),
            hp.quniform('fc2nn', 50, 3000, 1),
        ]

trials = hyperopt.Trials()
best = hyperopt.fmin(TryTest, space=myspace,
		algo=hyperopt.tpe.suggest,
		max_evals=1000,
		trials=trials)

