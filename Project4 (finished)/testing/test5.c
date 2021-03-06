/* This test file demonstrates that copying a file with metadata
   will copy the metadata as well.

   Run test5.sh to copy the README.txt file generated by test2.c 
   This file will then verify that metacopy.txt holds the metadata in README.txx */

#include <stdio.h>
#include <lib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl1.h>
#include <string.h>
#include <sys/stat.h>

int main(void) {
	char buffer[100];
	int fp; 
	int i = 0;

	fp = open("metacopy.txt", O_RDWR | O_CREAT, S_IRWXU);
	if (fp == -1) {
		printf("Failed to open file");
	}

	metaread(fp, &buffer, 100);

	while(buffer[i] != '\0') {
    	printf("%c", buffer[i]);
        i++;
    }
    printf("-This file is awesome!- should have been printed\n");
}