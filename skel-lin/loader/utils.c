#include "utils.h"

int get_segment(void *addr, so_exec_t *exec)
{
	for (int i = 0; i < exec->segments_no; i++) {
		so_seg_t segment = exec->segments[i];

		if (addr < (void *)segment.vaddr || addr >= (void *)segment.vaddr + segment.mem_size)
			continue;
		return i;
	}

	// not found in any segment
	return -1;
}

int get_page_in_segment(void *addr, so_seg_t segment)
{
	ptrdiff_t diff = (char *)addr - (char *)segment.vaddr;

	int page = diff / getpagesize();

	return page;
}

int is_page_mapped(int page_idx, so_seg_t segment)
{
	return ((int *)(segment.data))[page_idx];
}

size_t min(size_t a, size_t b)
{
	return (a < b) ? a : b;
}