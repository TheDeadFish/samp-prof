#include <stdshit.h>
#include "proflog.h"

void ProfLog::alloc()
{
	this->free();
	logData64k = xCalloc(nCHUNK);
}

void ProfLog::free()
{
	if(logData64k) {
		for(int i = 0; i < nCHUNK; i++) 
			::free(logData64k[i]);
		free_ref(logData64k);
	}
	sortData4k.clear();
}

int compar(const ProfLog::PageCount_t& a, const ProfLog::PageCount_t& b) {
	return b.count - a.count; }

void ProfLog::build4k_64K(size_t addr, u16** data)
{
	size_t base;
	size_t count;

	for(size_t i = 0; i < 65536; i++)
	{
		u16* dataPos = data[i];
		if(dataPos == NULL) {
			addr += 65536; continue; }

		goto LOOP_BEGIN;
		for(;; addr++) {
			if((addr % 4096) == 0) {
				if(count) sortData4k.push_back(base, count);
				if((addr % 65536) == 0) break;
		LOOP_BEGIN:
				count = 0; base = addr;
			}

			count += *dataPos++;
		}
	}
}

void ProfLog::build4k(void)
{
	build4k_64K(0, logData64k);
	qsort(sortData4k.data, sortData4k.len, compar);
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
