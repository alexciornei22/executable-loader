#include "exec_parser.h"
#include <stddef.h>
#include <unistd.h>

// get the segment for a memory address
int get_segment(void *addr, so_exec_t *exec);

// get the page for an address in a segment
int get_page_in_segment(void *addr, so_seg_t segment);

// check if a page index is mapped within a segment
int is_page_mapped(int page_idx, so_seg_t segment);

size_t min(size_t a, size_t b);
