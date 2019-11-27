#include <stdshit.h>
#include <conio.h>
#include "process.h"
#include "proflog.h"

const char progName[] = "dfprof";

ThreadApi thread;
ProfLog profLog;

void openThreadByTitle(const char* name)
{
	HWND hwnd = FindWindowA(0, name);
	if(!thread.open(hwnd)) {
		printf("failed to open thread by window"); 
		exit(1); }
}

void openThreadByTid(int tid)
{
	if(!thread.open(tid)) {
		printf("failed to open thread by tid"); 
		exit(1); }
}


static bool profStop;
static HANDLE profThread;



void profile_show()
{
	profLog.build4k();
	for(int iPage : profLog.sortData4k) {
		auto page = profLog.getPageInfo(iPage);
		printf("%08X: %d\n", page.addr, page.total);
		for(auto& line : page) {
			printf("  %08X: %d\n", line.addr, line.total);	}
	}
}

DWORD WINAPI profile_loop(
  LPVOID lpParameter
) {
	CONTEXT context = {};
	context.ContextFlags = CONTEXT_CONTROL;
	
	while(!profStop) {
		thread.suspend(true);
		thread.getCtx(&context);
		thread.suspend(false);
		profLog.log(context.Eip);	
		Sleep(1);
	}
	
	profile_show();
	
	return 0;
}

void profile_start(void)
{
	profStop = false;
	profThread = CreateThread(
		0,0,profile_loop, 0,0,0);
}

void profile_stop(void)
{
	profStop = true;
	WaitForSingleObject(profThread, INFINITE);
	CloseHandle(profThread);
}

int main(int argc, char** argv)
{
	int tid = atoi(argv[1]);
	printf("%d\n", tid);
	openThreadByTid(tid);
	profile_start();
	getch();
	profile_stop();
	return 0;
}
