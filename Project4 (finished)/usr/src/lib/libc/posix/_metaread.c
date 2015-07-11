#include <lib.h>
#include <unistd.h>
#include <stdio.h>

/*based off of _read.c*/

PUBLIC ssize_t metaread(fd, buffer, nbytes)
int fd;
void *buffer;
size_t nbytes;
{
    message m;

	m.m1_i1 = fd;
	m.m1_i2 = nbytes;
	m.m1_p1 = (char *) buffer;
    return(_syscall(VFS_PROC_NR, METAREAD, &m));
}