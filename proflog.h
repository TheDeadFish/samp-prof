#include <stdshit.h>
#pragma once

struct ProfLog
{
	ProfLog() { ZINIT; alloc(); }
	~ProfLog() { free(); }
	void alloc(); void free();
	void log(size_t addr) { logData64[addr/64]++; }
	void build4k(void);
	
	
	struct LineInfo {
 		int addr, total; };
	struct PageInfo { int addr, total;
		int nLines; LineInfo lines[64];
		DEF_BEGINEND(LineInfo, lines, nLines);
	};
	
	
	// 
	
	
	
	
	//xarray<int> get64() { return {logData64, LOG_SIZE}; }
	//xarray<int> get4k() { return {logData4k, LOG_SIZE/64}; }
	
	PageInfo getPageInfo(int slot);
	
	
	int sorted(int i) { return sortData4k[i]; }
	
	
	
	
	enum { nLINES = 32*1024*1024 };
	enum { nPAGES = nLINES/64 };
	
	
	
	
	
	int* logData64; int* logData4k;
	xarray<int> sortData4k;
};

