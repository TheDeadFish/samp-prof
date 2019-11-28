#include <stdshit.h>
#include <conio.h>
#include "process.h"
#include "proflog.h"

const char progName[] = "dfprof";

ThreadApi thread;
ProfLog profLog;

#define ERR_EXIT(...) ({ \
	fprintf(stderr, __VA_ARGS__); exit(1); })

void openThreadByTitle(const char* name)
{
	HWND hwnd = FindWindowA(0, name);
	if(!hwnd) ERR_EXIT("could not find window");
	if(!thread.open(hwnd)) {
		ERR_EXIT("failed to open thread by window"); }
}

void openThreadByTid(int tid)
{
	if(!thread.open(tid)) {
		ERR_EXIT("failed to open thread by tid"); }
}

void openThreadByName(cch* name)
{	
	int tid = ProcessApi::mainThread(name);
	if(!tid) ERR_EXIT("could not find process by name");
	return openThreadByTid(tid);
}



static bool profStop;
static HANDLE profThread;
static bool show_bytes;


void profile_show()
{
	profLog.build4k();
	for(int iPage : profLog.sortData4k) {
		auto page = profLog.getPageInfo(iPage);
		printf("%08X: %d\n", page.addr, page.total);
		for(auto& line : page) {
			printf("  %08X: %d\n", line.addr, line.total);	
			if(show_bytes) {
				auto xxx = profLog.getLineInfo(line.addr);
				for(auto& x : xxx)
					printf("    %08X: %d\n", x.addr, x.total);				
			}
		}
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
	
	// print ussage message
	if(argc < 2) {
		printf("deadfish sampling profiler\n");
		printf("  ussage: dfprof <w/n:?>\n");
		printf("  w: window name\n  n: exe name\n");
		return 1;
	}

	// get options
	if(!strcmp(argv[1], "-b")) {
		show_bytes=1; argv++; }
	
	// parse argument	
	char* arg = argv[1];
	if((!arg[0] || arg[1] != ':'))
		ERR_EXIT("bad argument");
	switch(arg[0]) {
	case 'w': openThreadByTitle(arg+2); break;
	case 'n': openThreadByName(arg+2); break;
	case 't': openThreadByTid(atoi(arg+2)); break;
	default: ERR_EXIT("bad argument");
	}
	
	// run the profiler
	profile_start();
	getch();
	profile_stop();
	return 0; 
}
