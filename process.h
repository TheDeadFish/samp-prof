#pragma once
#include <windows.h>

struct ProcessApi
{
	HANDLE hProcess;
	DWORD dwProcessId;

	BOOL open(DWORD pid);
	BOOL open(HWND hwnd);
	void close(void) { CloseHandle(hProcess); }
	
	SIZE_T read(SIZE_T addr, void* ptr, SIZE_T size);
	SIZE_T write(SIZE_T addr, void* ptr, SIZE_T size);
	DWORD mainThread(void);
	
	// static api
	static DWORD find(const char* name);
	static DWORD mainThread(DWORD pid);
	static DWORD mainThread(const char* name) {
		return mainThread(find(name)); }
};

struct ThreadApi
{
	HANDLE hThread;
	DWORD dwThreadId;

	BOOL open(DWORD tid);
	BOOL open(HWND hwnd);
	void close(void);	
	
	void suspend(bool bSuspend);
	BOOL getCtx(LPCONTEXT ctx);
};
