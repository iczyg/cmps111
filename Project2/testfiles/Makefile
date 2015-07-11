#Author: Eric Kaneshige
#Email: etkanesh@ucsc.edu
#CREATED: 5/2/2014

all: increments checksum ticketchanger

increments: increments.o
	cc -o increments increments.o -lfl

checksum: checksum.o
	cc -o checksum checksum.o -lfl


increments.o: increments.c
	cc -c increments.c

checksum.o: checksum.c
	cc -c checksum.c


clean:
	rm -r increments.o checksum.o ticketchanger.o *~