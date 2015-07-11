/* Checksum.c 
   Usage: ./checksum <filename>
   Adds up all the bytes in a checksum, copys the contents of the file to another file,
   and prints out the results of the checksum (checksum, and bytes read)

*/

#include <stdio.h>
#include <stdlib.h>


unsigned checkSum(void *buf, size_t len, unsigned int sum) {
    size_t i;
    unsigned char *buffer = (unsigned char *)buf;
    for (i = 0; i < len; i++) {
       sum += (*buffer++);
    }
    return sum;
}

int main(int argc, char* argv[]) {
    FILE *fp;
    FILE *newfile;
    size_t len;
    char *buf;
    long fsize;
    int cs;
    char ch;

	if (argc != 2) {
	   printf("Usage: %s <filename>\n", argv[0]);
	   exit(-1);
	}
    
    /* Open the specified file */
	fp = fopen(argv[1], "r");
	if (fp == NULL) {
	   printf("Unable to open %s\n", argv[1]);
	   exit(-1);
	}

	/*Open the file that is to be copied to */
	newfile = fopen("copyfile", "w+");
    if (newfile == NULL) {
    	printf("Unable to create file");
    	exit(-1);
    }


	/* Obtain the file size */
	fseek(fp, 0, SEEK_END);
	fsize = ftell(fp);
	rewind(fp);

	/* Copy file to buf */
	buf = (char*) malloc(sizeof(char)*fsize);
	
	/*read and write the files in 1kb blocks*/
	while (! feof (fp)) {
		printf("reading block\n");
        len += fread(buf, 1, 1024, fp); 
		printf("writing block\n");
		fwrite (buf, 1, len, newfile);
		printf("1kb read and written\n");
	}
	
	if (len != fsize) {
		printf("Error reading file %s", argv[1]);
		exit(-1);
	}
	
	cs = checkSum(buf, len, 0);
	printf("%d bytes were read in total\n", len);
	printf("Checksum of %s is %d\n", argv[1], cs);

	fclose(newfile);
	fclose(fp);
	free(buf);

}
