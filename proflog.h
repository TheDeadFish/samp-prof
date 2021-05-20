#include <stdshit.h>
#pragma once

struct ProfLog
{
	ProfLog() { ZINIT; alloc(); }
	~ProfLog() { free(); }
	void alloc(); void free();
	void log(size_t addr);
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
	
	
	int getLineCount(size_t addr);
	PageInfo getPageInfo(size_t addr);
	PageInfo getLineInfo(size_t addr);
	
	
	
	
	
	u16* getData64k(size_t addr) {
		u16* x = logData64k[addr>>16];
		return x ? notNull(x+(addr&0xFFFF)) : 0; }
	
	
	
	
	
	enum { nLINES = 32*1024*1024 };
	enum { nPAGES = nLINES/64 };
	enum { nCHUNK = 65536 };
	
	u16** logData64k; int* logData4k;
	xarray<size_t> sortData4k;
};

