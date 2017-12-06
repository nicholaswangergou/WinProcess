#pragma once

#include <Windows.h>
#include <tchar.h>
#include <thread>


typedef void(*ProcFinishedCallback)(void * userdata);

class WinProcess
{
public:
	WinProcess();
	virtual ~WinProcess();

	int start(const char *cmdline);
	void wait();
	int stop();
	bool isRunning();
	int exec(const char *cmdline);
	
	int readStandardOuptut(char *data, int len);
	int readStandardError(char *data, int len);


	void setProcFinishedCallback(ProcFinishedCallback cb, void *userdata);

private:
	HANDLE m_hStdOutPipeRead;
	HANDLE m_hStdOutPipeWrite;
	HANDLE m_hStdErrPipeRead;
	HANDLE m_hStdErrPipeWrite;
	PROCESS_INFORMATION m_hProcInfo;
	ProcFinishedCallback onFinishedCallback;
	void * m_pProcFinishUserdata;
};

