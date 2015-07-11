/* This test file demonstrates that creating 1000 files and adding
   metadata to them then deleting them does not decrease the free space
   on the filesystem */

#include <stdio.h>
#include <lib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl1.h>
#include <string.h>
#include <sys/stat.h>

int main(void) {
	char message[100] = "Metadata to be added to 1000 files!\n";
	int fp;
	int i;
	char str[50];
	char c[30];
	char *filename = "testfile";
    
    /* Make 1000 testfiles with metadata formatted 
       testfile0.txt, testfile2.txt,...testfile1000.txt */
	for (i=0; i < 1000; i++) {
		sprintf(c, "%d", i);
		strcpy(str, c); 
		strcpy(str, ".txt");
		fp = open(str, O_RDWR | O_CREAT, S_IRWXU);
		if (fp == -1) {
			printf("Failed to open file");
		}
		metawrite(fp, &message, 100);
		memset(&str, 0, sizeof(char) * 50);
		memset(&c, 0, sizeof(char) * 30);
	}

	/* Now remove the testfiles */
	for (i=0; i < 1000; i++) {
		sprintf(c, "%d", i);
		strcpy(str, c); 
		strcpy(str, ".txt");
		remove(str);
		memset(&str, 0, sizeof(char) * 50);
		memset(&c, 0, sizeof(char) * 30);
	}
	printf("Added and removed 1000 files with metadata!");
	return 0;
}

