#include "process.h"

BOOL ProcessApi::open(HWND hwnd)
{
	DWORD pid = 0;
	GetWindowThreadProcessId(hwnd, &pid);
	return open(pid);
}

BOOL ProcessApi::open(DWORD pid)
{
	dwProcessId = pid;
	hProcess = OpenThread(PROCESS_ALL_ACCESS, FALSE, pid);
	return !!hProcess;
}

SIZE_T ProcessApi::read(SIZE_T addr, void* ptr, SIZE_T size)
{
	SIZE_T NumberOfBytesRead = 0;
	ReadProcessMemory(hProcess, (LPVOID)addr,
		ptr, size, &NumberOfBytesRead);
	return NumberOfBytesRead;
}

SIZE_T ProcessApi::write(SIZE_T addr, void* ptr, SIZE_T size)
{
	SIZE_T NumberOfBytesWritten = 0;
	WriteProcessMemory(hProcess, (LPVOID)addr,
		ptr, size, &NumberOfBytesWritten);
	return NumberOfBytesWritten;
}

BOOL ThreadApi::open(HWND hwnd)
{
	return open(GetWindowThreadProcessId(hwnd, 0));
}

BOOL ThreadApi::open(DWORD tid)
{
	dwThreadId = tid;
	hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, tid);
	return !!hThread;
}

void ThreadApi::suspend(bool bSuspend)
{
	if(bSuspend)
		SuspendThread(hThread);
	else
		ResumeThread(hThread);
}

BOOL ThreadApi::getCtx(LPCONTEXT ctx)
{
	return GetThreadContext(hThread, ctx);
}