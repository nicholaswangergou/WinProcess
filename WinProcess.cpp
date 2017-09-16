#include "WinProcess.h"


WinProcess::WinProcess()
{
	m_hStdOutPipeRead = NULL;
	m_hStdOutPipeWrite = NULL;
	m_hStdErrPipeRead = NULL;
	m_hStdErrPipeWrite = NULL;
	onFinishedCallback = NULL;
	m_pProcFinishUserdata = NULL;
	m_bKeepRunning = true;
}

WinProcess::~WinProcess()
{
	if (m_strCmdLine) {
		delete m_strCmdLine;
	}

	
}

int WinProcess::start(const char * cmd)
{
	m_strCmdLine = _strdup(cmd);

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

	bSuccess = CreateProcessA(NULL, m_strCmdLine, NULL, NULL, true, 0, NULL, NULL, &startInfo, &m_hProcInfo);
	if (!bSuccess) {
		return -1;
	}

	return 0;
}

int WinProcess::stop()
{
	if (onFinishedCallback) {
		(*onFinishedCallback)(m_pProcFinishUserdata);
	}

	BOOL bSuccess = TerminateProcess(m_hProcInfo.hProcess, NULL);
	m_bKeepRunning = false;

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

void WinProcess::setProcFinishedCallback(ProcFinishedCallback cb, void *userdata)
{
	onFinishedCallback = cb;
	m_pProcFinishUserdata = userdata;
}

void WinProcess::setStdOutReadyCallback(StdOutReadyCallback cb, void * userdata)
{
	onStdOutReadyCallback = cb;
	m_pStdOutUserdata = userdata;
}

void WinProcess::setStdErrReadyCallback(StdErrReadyCallback cb, void * userdata)
{
	onStdErrReadyCallback = cb;
	m_pStdErrUserdata = userdata;
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
}

void WinProcess::loop()
{
	HANDLE handles[2] = { m_hStdOutPipeRead, m_hStdErrPipeRead };
	while (m_bKeepRunning) {
		DWORD obj = WaitForMultipleObjects(2, handles, false, 1000);
		int index = obj - WAIT_OBJECT_0;
		switch (index)
		{
		case 0:
			if (onStdOutReadyCallback) {
				(*onStdOutReadyCallback)(m_pStdOutUserdata);
			}
			break;
		case 1:
			if (onStdErrReadyCallback) {
				(*onStdErrReadyCallback)(m_pStdErrUserdata);
			}
			break;
		default:
			break;
		}
	}
}
