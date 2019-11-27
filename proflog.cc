#include <stdshit.h>
#include "proflog.h"

void ProfLog::alloc()
{
	this->free();
	logData64 = xMalloc(nLINES);
	logData4k = xCalloc(nPAGES);
	sortData4k.data = xMalloc(nPAGES);
}

void ProfLog::free()
{
	free_ref(logData64);
	free_ref(logData4k);
	sortData4k.clear();
}

static REGCALL(3)
int compar(ProfLog* This,	const int& a, const int& b) {
	return This->logData4k[b] - This->logData4k[a]; }
	
void ProfLog::build4k(void)
{
	std::fill_n(logData4k, nPAGES, 0);
	std::fill_n(sortData4k.data, nPAGES, 0);
	for(int i = 0; i < nLINES; i++) {
		logData4k[i/64] += logData64[i]; }
	for(int i = 0; i < nPAGES; i++) {
		sortData4k.len += !!logData4k[i];
		sortData4k[i] = i; }
	xqsort(sortData4k.data, nPAGES, compar, this);
}

static REGCALL(3)
int compar2(ProfLog* This,	
	const ProfLog::LineInfo& a, 
	const ProfLog::LineInfo& b) {
	return b.total - a.total; }

ProfLog::PageInfo ProfLog::getPageInfo(int slot)
{
	PageInfo pi = {};
	pi.addr = slot*4096;
	for(int i = 0; i < 64; i++) {
		pi[i].addr = pi.addr + i*64;
		pi[i].total = logData64[slot*64+i];
		if(pi[i].total) { pi.nLines++;
			pi.total += pi[i].total; }
	}
	xqsort(pi.lines, 64, compar2, this);
	return pi;
}