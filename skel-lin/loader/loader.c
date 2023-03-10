/*
 * Loader Implementation
 *
 * 2022, Operating Systems
 */
#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <signal.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "exec_parser.h"
#include "utils.h"

static so_exec_t *exec;
// executable file descriptor
static int fd;

static void segv_handler(int signum, siginfo_t *info, void *context)
{
	int segment_idx = get_segment(info->si_addr, exec);

	if (segment_idx < 0)
		// run default handler if address is not in segment
		signal(SIGSEGV, SIG_DFL);
	else {
		so_seg_t segment = exec->segments[segment_idx];
		int page = get_page_in_segment(info->si_addr, segment);

		if (is_page_mapped(page, segment))
			signal(SIGSEGV, SIG_DFL);

		void *vaddr = mmap((void *)segment.vaddr + page * getpagesize(), getpagesize(), PROT_WRITE, MAP_FIXED | MAP_ANON | MAP_PRIVATE, -1, 0);

		if (vaddr == MAP_FAILED)
			signal(SIGSEGV, SIG_DFL);

		if (page * getpagesize() < segment.file_size) {
			lseek(fd, segment.offset + page * getpagesize(), SEEK_SET);
			read(fd, vaddr, min(segment.file_size - page * getpagesize(), getpagesize()));
		}

		mprotect(vaddr, getpagesize(), segment.perm);

		// save data about mapped page
		((int *)(segment.data))[page] = 1;
	}
}

int so_init_loader(void)
{
	int rc;
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_sigaction = segv_handler;
	sa.sa_flags = SA_SIGINFO;
	rc = sigaction(SIGSEGV, &sa, NULL);
	if (rc < 0) {
		perror("sigaction");
		return -1;
	}
	return 0;
}

int so_execute(char *path, char *argv[])
{
	exec = so_parse_exec(path);
	if (!exec)
		return -1;

	for (int i = 0; i < exec->segments_no; i++)
		exec->segments[i].data = calloc(exec->segments[i].mem_size / getpagesize(), sizeof(int));

	fd = open(path, O_RDONLY);
	so_start_exec(exec, argv);

	return -1;
}
