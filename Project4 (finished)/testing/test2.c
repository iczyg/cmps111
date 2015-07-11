/* This test file demonstrates adding a note to a README.txt
   file, creating the file if it does not exist, and then 
   reading back the note */

#include <stdio.h>
#include <lib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(void) {
	char message[100] = "This file is awesome!\n";
	char readbuffer[100];
	int fp;
	int i = 0;

	fp = open("README.txt", O_RDWR | O_CREAT, S_IRWXU);
	if (fp == -1) {
	   printf("Failed to open file\n");
	}

	metawrite(fp, &message, 100);
    metaread(fd, &readbuffer, 100);
    while(readbuffer[i] != '\0') {
    	printf("%c", readbuffer[i]);
        i++;
    }
    return 0;
}