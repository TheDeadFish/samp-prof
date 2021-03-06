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
	
	
	
	
	
	u16* getData64k(size_t addr);


	enum { nCHUNK = 65536 };
	
#ifdef _WIN64
	u16*** logData4G;
#else
	u16** logData64k;
#endif

	struct PageCount_t {
		size_t addr; size_t count; };
	xarray<PageCount_t> sortData4k;

	void build4k_64K(size_t addr, u16** data);
	void free_64K(u16** data);

};

