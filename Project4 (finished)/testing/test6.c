/* This test file demonstrates that changing the metadata of an original file
   does not change the metadata of the copied file */

#include <stdio.h>
#include <lib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl1.h>
#include <string.h>
#include <sys/stat.h>

int main(void) {
	char message[100] = "Added text to the original file\n";
	char origbuffer[100];
	char buffer[100];
	int fp; 
	int fp2;
	int i = 0;

	fp = open("README.txt", O_RDWR | O_CREAT, S_IRWXU);
	if (fp == -1) {
		printf("Failed to open file");
	}
	fp2 = open("copy.txt", O_RDWR | O_CREAT, S_IRWXU);
    if (fp2 == -1) {
		printf("Failed to open file");
	}
	metawrite(fp, &message, 100);
	metaread(fp, &origbuffer, 100);

	/* Metadata from README.txt - should have more text */

	while(origbuffer[i] != '\0') {
    	printf("%c", origbuffer[i]);
        i++;
    }

    /* Metadata from copy.txt - shouldn't have changed */
    
    metaread(fp2, &buffer, 100);
    while(buffer[i] != '\0') {
    	printf("%c", buffer[i]);
        i++;
    }
    return 0;
}