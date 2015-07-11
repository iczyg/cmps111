/* This test file demonstrates compatibility with 
   the existing MINIX filesystem */

#include <stdio.h>
#include <lib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl1.h>
#include <string.h>

int main(void) {
	char buffer[100] = "test data\n";
	char readbuffer[100];
	int fp;
	int i=0;
	fp = open("testfile.txt", O_RDWR);
	if (fd < 0) {
		printf("Failed to open test file\n");
	}

	metawrite(fp, &buffer, 100);
	metaread(fd, &readbuffer, 100);
	while (readbuffer[i] != '/0') {
		printf("%c", readbuffer[i]);
		i++;
	}
	return 0;
}