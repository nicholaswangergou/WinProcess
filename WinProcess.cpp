#include "WinProcess.h"
#include <string>


WinProcess::WinProcess()
{
	m_hStdOutPipeRead = NULL;
	m_hStdOutPipeWrite = NULL;
	m_hStdErrPipeRead = NULL;
	m_hStdErrPipeWrite = NULL;
	onFinishedCallback = NULL;
	m_pProcFinishUserdata = NULL;
}

WinProcess::~WinProcess()
{
	if (m_hStdOutPipeRead)
		CloseHandle(m_hStdOutPipeRead);
	if (m_hStdOutPipeWrite)
		CloseHandle(m_hStdOutPipeWrite);
	if (m_hStdErrPipeRead)
		CloseHandle(m_hStdErrPipeRead);
	if (m_hStdErrPipeWrite)
		CloseHandle(m_hStdErrPipeWrite);

	m_hStdOutPipeRead = NULL;
	m_hStdOutPipeWrite = NULL;
	m_hStdErrPipeRead = NULL;
	m_hStdErrPipeWrite = NULL;
}

int WinProcess::start(const char *cmdline)
{
	if (!cmdline) {
		return -2;
	}

	//Set up Pipe
	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = true;
	saAttr.lpSecurityDescriptor = NULL;

	if (!CreatePipe(&m_hStdOutPipeRead, &m_hStdOutPipeWrite, &saAttr, 0)) {
		return -1;
	}

	if (!CreatePipe(&m_hStdErrPipeRead, &m_hStdErrPipeWrite, &saAttr, 0)) {
		return -1;
	}

	//Create Child Process
	STARTUPINFOA startInfo;
	BOOL bSuccess = FALSE;

	ZeroMemory(&m_hProcInfo, sizeof(PROCESS_INFORMATION));
	ZeroMemory(&startInfo, sizeof(startInfo));
	startInfo.cb = sizeof(startInfo);
	startInfo.hStdError = m_hStdErrPipeWrite;
	startInfo.hStdOutput = m_hStdOutPipeWrite;
	startInfo.dwFlags |= STARTF_USESTDHANDLES;
	
	char buff_[MAX_PATH] = { 0 };
	if (cmdline)
		std::snprintf(buff_, MAX_PATH - 1, "%s", cmdline);

	bSuccess = CreateProcessA(NULL, buff_, NULL, NULL, true, 0, NULL, NULL, &startInfo, &m_hProcInfo);
	if (!bSuccess) {
		return -1;
	}

	return 0;
}

int WinProcess::stop()
{
	BOOL bSuccess = TerminateProcess(m_hProcInfo.hProcess, NULL);
	if (onFinishedCallback) {
		(*onFinishedCallback)(m_pProcFinishUserdata);
	}

	return bSuccess ? 0 : -1;
}

bool WinProcess::isRunning()
{
	DWORD status = 0;
	if (m_hProcInfo.hProcess != NULL) {
		BOOL bSuccess = GetExitCodeProcess(m_hProcInfo.hProcess, &status);
		if (bSuccess) {
			if (status == STILL_ACTIVE) {
				return true;
			}
		}
	}
	return false;
}

int WinProcess::exec(const char *cmdline)
{
	if (start(cmdline) >= 0) {
		wait();
		return 0;
	}

	return -1;
}

void WinProcess::setProcFinishedCallback(ProcFinishedCallback cb, void *userdata)
{
	onFinishedCallback = cb;
	m_pProcFinishUserdata = userdata;
}

int WinProcess::readStandardOuptut(char * data, int len)
{
	BOOL bSuccess = FALSE;
	DWORD size = 0;
	bSuccess = ReadFile(m_hStdOutPipeRead, data, len, &size, NULL);
	if (bSuccess) {
		return size;
	}
	return -1;
}

int WinProcess::readStandardError(char * data, int len)
{
	BOOL bSuccess = FALSE;
	DWORD size = 0;
	bSuccess = ReadFile(m_hStdErrPipeRead, data, len, &size, NULL);
	if (bSuccess) {
		return size;
	}
	return -1;
}

void WinProcess::wait()
{
	WaitForSingleObject(m_hProcInfo.hProcess, INFINITE);
	CloseHandle(m_hStdOutPipeWrite);
	CloseHandle(m_hStdErrPipeWrite);
	m_hStdOutPipeWrite = NULL;
	m_hStdErrPipeWrite = NULL;
	if (onFinishedCallback) {
		(*onFinishedCallback)(m_pProcFinishUserdata);
	}
}
