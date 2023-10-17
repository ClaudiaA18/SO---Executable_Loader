/*
 * Loader Implementation
 *
 * 2022, Operating Systems
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <signal.h>
#include <fcntl.h>

#include "exec_parser.h"

static so_exec_t *exec;
static int fd;

static void segv_handler(int signum, siginfo_t *info, void *context)
{
	struct sigaction sa;
	so_seg_t* seg;
	uintptr_t page_fault_addr = (uintptr_t) info->si_addr;
	int page_nr;
	int offset;

	if (signum != SIGSEGV || page_fault_addr == NULL) {
		sa.sa_sigaction(signum, info, context);
		return;
	}
	for (int i = 0; i < exec->segments_no; i++) {
		seg = &exec->segments[i];
		if (page_fault_addr < (seg->vaddr + seg->mem_size)) {
			page_nr = (page_fault_addr - seg->vaddr)/getpagesize();

			if (info->si_code == SEGV_MAPERR) {
				void *addr_map = (void *)(getpagesize()*page_nr + seg->vaddr);
				void *ret = mmap(addr_map, getpagesize(), PERM_R | PERM_W | PERM_X,  MAP_FIXED | MAP_SHARED | MAP_ANONYMOUS, -1, 0);

				if (ret == NULL)
					fprintf(stderr, "Error");
				offset = getpagesize()*page_nr;
				fseek(fd, 0, SEEK_SET);
				lseek(fd, seg->offset + getpagesize()*page_nr, SEEK_SET);
				if (offset > seg->file_size)
					memset(ret, 0, getpagesize());
				else if (seg->file_size - offset < getpagesize())
					fread(fd, ret, seg->file_size - offset);
				else
					read(fd, ret, getpagesize());
				int ret2 = mprotect(ret, getpagesize(), seg->perm);

				if (ret == -1)
					fprintf(stderr, "Error");
				return;
		}
		}
	}
	sa.sa_sigaction(signum, info, context);
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
	fd = open(path, O_RDWR);
	exec = so_parse_exec(path);
	if (!exec)
		return -1;

	so_start_exec(exec, argv);

	return -1;
}
