#include "EnableKeyPressToExit.hpp"
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <thread>

/*extern*/ bool global__key_has_been_pressed = false;


static void WaitingThreadGoHere(bool exit_immediately)
{
	getchar();
	if(exit_immediately) {
		exit(0);
	}
	global__key_has_been_pressed = true;
}

void EnableKeyPressToExit(bool exit_immediately)
{
	global__key_has_been_pressed = false;
	std::thread* keypress_waitingthread = new std::thread(WaitingThreadGoHere, exit_immediately);
}
