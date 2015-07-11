/* This test file demonstrates that changing the metadata does not
   change the regular file contents. */

#include <stdio.h>
#include <lib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl1.h>
#include <string.h>
#include <sys/stat.h>

int main(void) {
	char buffer[100] = "metadata that should not be changed\n";
	char reg[100] = "regular file contents\n";
	char readreg[100];
	int fp;
	int i=0;

	fp = open("testfile.txt", O_RDWR | O_CREAT, S_IRWXU);
	if (fp == -1) {
		printf("Failed to open file\n");
	}
    write(fp, &reg, 100);
	metawrite(fp, &buffer, 100);
    read(fp, &readreg, 100);
    while(readreg[i] != '\0') {
    	printf("%c", readbuffer[i]);
        i++;
    }
    printf("\n -regular file contents- should have been printed\n");
    return 0;
}