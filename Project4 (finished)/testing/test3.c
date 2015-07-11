/* This test file demonstrates that changing the regular file contents 
   does not change the metadata. */

#include <stdio.h>
#include <lib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl1.h>
#include <string.h>
#include <sys/stat.h>

int main(void) {
	char buffer[100] = "metadata that should not be changed\n";
	char readbuffer[100];
	char reg[100] = "regular file contents\n";
	int fp;
	int i=0;

	fp = open("test.txt", O_RDWR | O_CREAT, S_IRWXU);
	if (fp == -1) {
		printf("Failed to open file\n");
	}

	metawrite(fp, &buffer, 100);
    write(fp, &reg, 100);
    metaread(fp, &readbuffer, 100);
    while(readbuffer[i] != '\0') {
    	printf("%c", readbuffer[i]);
        i++;
    }
    printf(" -metadata that should not be changed- should have been printed\n");
    return 0;
}