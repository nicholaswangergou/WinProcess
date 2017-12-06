#include <iostream>
#include <Windows.h>
#include <Shellapi.h>
#include "WinProcess.h"

void onStdOutReady(void * userdata) {
	WinProcess * ptr = (WinProcess *)userdata;
	char buff[4096];
	int size = 0;
	size = ptr->readStandardOuptut(buff, 4096);
	if (size > 0) {
		buff[size] = '\0';
		std::cout << __FUNCTION__ << buff << std::endl;
	}
}

void onStdErrReady(void * userdata) {
	WinProcess * ptr = (WinProcess *)userdata;
	char buff[4096];
	int size = 0;
	size = ptr->readStandardError(buff, 4096);
	if (size > 0) {
		buff[size] = '\0';
		std::cout << __FUNCTION__ << buff << std::endl;
	}
}

void onProcFinished(void * userdata) {
	onStdOutReady(userdata);
	onStdErrReady(userdata);
}

int testWinCallback() {
	WinProcess proc;
	proc.setProcFinishedCallback(onProcFinished, &proc);

	std::string param = "dir";

	if (proc.exec(param.c_str()) < 0) {
		perror("error");
	}
	return 0;
}

int main(int argc, char ** argv) {
	testWinCallback();

	return 0;
}