#include "common.h"
#include "memory/cache.h"

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);
int is_mmio(hwaddr_t);
uint32_t mmio_read(hwaddr_t, size_t, int);
void mmio_write(hwaddr_t, size_t, uint32_t, int);
lnaddr_t seg_translate(swaddr_t, size_t, uint8_t);
hwaddr_t page_translate(lnaddr_t addr);
extern uint8_t current_sreg;
/* Memory accessing interfaces */

uint32_t hwaddr_read(hwaddr_t addr, size_t len)
{
	//redirect caache_read()
	int index = is_mmio(addr);
	if (index >= 0)
	{
		return mmio_read(addr, len, index);
	}
	uint32_t offset = addr & (CACHE_BLOCK_SIZE - 1);
	uint32_t block = cache_read(addr);
	uint8_t temp[4];
	memset(temp, 0, sizeof(temp));
	if (offset + len >= CACHE_BLOCK_SIZE) //addr too long && cache_read again
	{
		uint32_t _block = cache_read(addr + len);
		memcpy(temp, cache[block].data + offset, CACHE_BLOCK_SIZE - offset);
		memcpy(temp + CACHE_BLOCK_SIZE - offset, cache[_block].data, len - (CACHE_BLOCK_SIZE - offset));
	}
	else
	{
		memcpy(temp, cache[block].data + offset, len);
	}
	int zero = 0;
	uint32_t cnt = unalign_rw(temp + zero, 4) & (~0u >> ((4 - len) << 3));
	printf("time: %d\n", tol_time);
	return cnt;
}

void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data)
{
	//dram_write(addr, len, data); redirect
	int index = is_mmio(addr);
	if (index >= 0)
	{
		mmio_write(addr, len, data, index);
		return;
	}
	cache_write(addr, len, data);
}

uint32_t lnaddr_read(lnaddr_t addr, size_t len)
{
	printf("%x\n",addr);
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	uint32_t now_offset = addr & 0xfff;
	if (now_offset + len - 1 > 0xfff)
	{
		// Assert(0,"Cross the page boundary");
		size_t l = 0xfff - now_offset + 1;
		uint32_t addr_r = lnaddr_read(addr, l);
		uint32_t addr_l = lnaddr_read(addr + l, len - l);
		uint32_t val = (addr_l << (l << 3)) | addr_r;
		return val;
	}
	else
	{
		hwaddr_t hwaddr = page_translate(addr);
		return hwaddr_read(hwaddr, len);
	}
	// return hwaddr_read(addr, len);
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data)
{
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	uint32_t now_offset = addr & 0xfff;
	if (now_offset + len - 1 > 0xfff)
	{
		// Assert(0,"Cross the page boundary");
		size_t l = 0xfff - now_offset + 1;
		lnaddr_write(addr, l, data & ((1 << (l << 3)) - 1));
		lnaddr_write(addr + l, len - l, data >> (l << 3));
	}
	else
	{
		hwaddr_t hwaddr = page_translate(addr);
		hwaddr_write(hwaddr, len, data);
	}
	// hwaddr_write(addr, len, data);
}

uint32_t swaddr_read(swaddr_t addr, size_t len)
{
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_t lnaddr = seg_translate(addr, len, current_sreg);
	return lnaddr_read(lnaddr, len);
}

void swaddr_write(swaddr_t addr, size_t len, uint32_t data)
{
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_t lnaddr = seg_translate(addr, len, current_sreg);
	lnaddr_write(lnaddr, len, data);
}
