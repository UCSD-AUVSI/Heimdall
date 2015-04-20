import cProfile
import test

profilefunc = "test.testimg"
parg0 = "zeroth_arg"
parg1 = "/media/C:/LinuxShared/AUVSI/targets/crops/2014_testset_targets/1_0__crop.jpg"
parg2 = "0" #dont display anything

cProfile.run(profilefunc+"([\'"+parg0+"\', \'"+parg1+"\', \'"+parg2+"\'])")

'''
	result: total of 12.8 seconds on 1_0__crop.jpg on i7-2620m
		the slowest parts are:
			5.3 seconds on main.getShapeColors()
			4.8 seconds on main.makeMasks()
			2.7 seconds on everything else
'''
