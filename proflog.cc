#include <stdshit.h>
#include "proflog.h"

void ProfLog::alloc()
{
	this->free();
	logData64k = xCalloc(nCHUNK);
	logData4k = xCalloc(nPAGES);
	sortData4k.data = xMalloc(nPAGES);
}

void ProfLog::free()
{
	if(logData64k) {
		for(int i = 0; i < nCHUNK; i++) 
			::free(logData64k[i]);
		free_ref(logData64k);
	}
	free_ref(logData4k);
	sortData4k.clear();
}

static REGCALL(3)
int compar(ProfLog* This,	const int& a, const int& b) {
	return This->logData4k[b/4096] - This->logData4k[a/4096]; }
	
void ProfLog::build4k(void)
{
	std::fill_n(logData4k, nPAGES, 0);
	std::fill_n(sortData4k.data, nPAGES, 0);
	for(int i = 0; i < nLINES; i++) {
		logData4k[i/64] += getLineCount(i*64); }
	for(int i = 0; i < nPAGES; i++) {
		sortData4k.len += !!logData4k[i];
		sortData4k[i] = i*4096; }
	xqsort(sortData4k.data, nPAGES, compar, this);
}

static REGCALL(3)
int compar2(ProfLog* This,	
	const ProfLog::LineInfo& a, 
	const ProfLog::LineInfo& b) {
	return b.total - a.total; }

ProfLog::PageInfo ProfLog::getPageInfo(size_t addr)
{
	PageInfo pi = {};
	pi.addr = addr;
	for(int i = 0; i < 64; i++) {
		pi[i].addr = pi.addr + i*64;
		pi[i].total = getLineCount(addr+i*64);
		if(pi[i].total) { pi.nLines++;
			pi.total += pi[i].total; }
	}
	xqsort(pi.lines, 64, compar2, this);
	return pi;
}

ProfLog::PageInfo ProfLog::getLineInfo(size_t addr)
{
	PageInfo pi = {};
	pi.addr = addr;
	u16* x = getData64k(addr);
	if(!x) return pi;
	for(int i = 0; i < 64; i++) {
		pi[i].addr = pi.addr + i;
		pi[i].total = x[i];
		if(pi[i].total) { pi.nLines++;
			pi.total += pi[i].total; }
	}
	xqsort(pi.lines, 64, compar2, this);
	return pi;
}

void ProfLog::log(size_t addr)
{
	u16*& x = logData64k[addr>>16];
	if(!x) { x = xCalloc(0x10000); }
	x[addr&0xFFFF]++;
}

int ProfLog::getLineCount(size_t addr)
{
	int count = 0;
	u16* x = getData64k(addr);
	if(x) { for(int i = 0; i < 64; i++) {
		count += x[i]; }}
	return count;
}
