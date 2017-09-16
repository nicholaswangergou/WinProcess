#include <iostream>

#include <Windows.h>
#include <Shellapi.h>
#include "WinProcess.h"


void onProcFinished(void * userdata) {
	WinProcess * ptr = (WinProcess *)userdata;
	char buff[4096];
	int size = 0;
	size = ptr->readStandardOuptut(buff, 4096);
	if (size > 0) {
		buff[size] = '\0';
	}

	std::cout << __FUNCTION__ << buff << std::endl;	
}

void onStdOutReady(void * userdata) {
	WinProcess * ptr = (WinProcess *)userdata;
	char buff[4096];
	int size = 0;
	size = ptr->readStandardOuptut(buff, 4096);
	if (size > 0) {
		buff[size] = '\0';
	}

	std::cout << __FUNCTION__ << buff << std::endl;
}

void onStdErrReady(void * userdata) {
	WinProcess * ptr = (WinProcess *)userdata;
	char buff[4096];
	int size = 0;
	size = ptr->readStandardOuptut(buff, 4096);
	if (size > 0) {
		buff[size] = '\0';
	}

	std::cout << __FUNCTION__ << buff << std::endl;
}

int testWinCallback() {
	WinProcess proc;
	proc.setProcFinishedCallback(onProcFinished, &proc);
	proc.setStdOutReadyCallback(onStdOutReady, &proc);
	proc.setStdErrReadyCallback(onStdErrReady, &proc);
	std::string cmd = "dir";

	proc.start(cmd.c_str());
	while (proc.isRunning()) {
		onProcFinished(&proc);
	}

	proc.wait();
	return 0;
}

int main(int argc, char ** argv) {
	testWinCallback();

	return 0;
}