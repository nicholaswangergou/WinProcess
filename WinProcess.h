#pragma once

#include <Windows.h>
#include <tchar.h>
#include <thread>


typedef void(*ProcFinishedCallback)(void * userdata);
typedef void(*StdOutReadyCallback)(void * userdata);
typedef void(*StdErrReadyCallback)(void * userdata);

class WinProcess
{
public:
	WinProcess();
	virtual ~WinProcess();

	int start(const char * cmd);
	int stop();
	bool isRunning();
	
	int readStandardOuptut(char * data, int len);
	int readStandardError(char * data, int len);
	void wait();

	void setProcFinishedCallback(ProcFinishedCallback cb, void *userdata);
	void setStdOutReadyCallback(StdOutReadyCallback cb, void * userdata);
	void setStdErrReadyCallback(StdErrReadyCallback cb, void * userdata);

private:
	void loop();

private:
	HANDLE m_hStdOutPipeRead;
	HANDLE m_hStdOutPipeWrite;
	HANDLE m_hStdErrPipeRead;
	HANDLE m_hStdErrPipeWrite;
	PROCESS_INFORMATION m_hProcInfo;

	char * m_strCmdLine;

	ProcFinishedCallback onFinishedCallback;
	void * m_pProcFinishUserdata;

	StdOutReadyCallback onStdOutReadyCallback;
	void * m_pStdOutUserdata;

	StdErrReadyCallback onStdErrReadyCallback;
	void * m_pStdErrUserdata;

	std::thread m_LoopThread;
	bool m_bKeepRunning;
};

