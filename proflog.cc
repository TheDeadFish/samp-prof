#include <stdshit.h>
#include "proflog.h"

void ProfLog::alloc()
{
	this->free();
#ifdef _WIN64
	logData4G = xCalloc(nCHUNK);
#else
	logData64k = xCalloc(nCHUNK);
#endif
}

#define ENUM_64K(xxx, ...) if(xxx) { \
	for(size_t i = 0; i < nCHUNK; i++) { \
		auto& x = xxx[i]; __VA_ARGS__; }}

void ProfLog::free_64K(u16** data)
{
	ENUM_64K(data, ::free(x));
	free_ref(data);
}

void ProfLog::free()
{
#ifdef _WIN64
	ENUM_64K(logData4G, free_64K(x));
	free_ref(logData4G);
#else
	free_64K(logData64k);
#endif
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
#ifdef _WIN64
	ENUM_64K(logData4G,
		if(x) build4k_64K(i*65536, x));
#else
	build4k_64K(0, logData64k));
#endif
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

u16* ProfLog::getData64k(size_t addr)
{
#ifdef _WIN64
		u16** logData64k = logData4G[addr>>32];
		if(!logData64k) return NULL;
		addr &= 0xFFFFFFFF;
#endif
		u16* x = logData64k[addr>>16];
		return x ? (x+(addr&0xFFFF)) : 0;
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
#ifdef _WIN64
	u16**& logData64k = logData4G[addr>>32];
	if(!logData64k) logData64k = xCalloc(0x10000);
	addr &= 0xFFFFFFFF;
#endif
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
