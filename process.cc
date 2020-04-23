#include "process.h"
#include <tlhelp32.h>
#undef Process32First
#undef Process32Next
#undef PROCESSENTRY32


DWORD ProcessApi::open(HWND hwnd)
{
	DWORD pid = 0;
	GetWindowThreadProcessId(hwnd, &pid);
	return open(pid);
}

DWORD ProcessApi::open(DWORD pid)
{
	//dwProcessId = pid;
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	return hProcess ? pid : NULL;
}

#include <stdio.h>

SIZE_T ProcessApi::memAlloc(SIZE_T size)
{
	return (SIZE_T)VirtualAllocEx(hProcess, 0, size, 
		MEM_COMMIT, PAGE_EXECUTE_READWRITE);
}

void ProcessApi::memFree(SIZE_T addr)
{
	VirtualFreeEx(hProcess, (LPVOID)addr, 0, MEM_RELEASE);
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

DWORD ProcessApi::mainThread(void)
{
#ifdef _WIN64
	SIZE_T ptid = __readgsdword(0x30)+72;
#else
	SIZE_T ptid = __readfsdword(0x18)+36;
#endif
	DWORD tid = 0; read(ptid, &tid, 4);
	return tid;
}

DWORD ProcessApi::find(const char* name)
{
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	PROCESSENTRY32 entry = {sizeof(PROCESSENTRY32)};
	DWORD pid = 0;
	BOOL hasData = Process32First(snapshot, &entry);
	while(hasData) {
		if(!stricmp(entry.szExeFile, name)) {
			pid = entry.th32ProcessID; break; }
		hasData = Process32Next(snapshot, &entry);
	}
	CloseHandle(snapshot);
	return pid;
}

DWORD ProcessApi::mainThread(DWORD pid)
{
	ProcessApi pa;
	if(!pa.open(pid)) return 0;
	DWORD tid = pa.mainThread();
	pa.close(); return tid;
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