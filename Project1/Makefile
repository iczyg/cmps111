# Jason Choi, jawchoi
# 4/21/2013
# Assignment 1
# CMPS 111

CSOURCE	= myshell.c lex.yy.c 
OBJECTS	= ${CSOURCE:.c=.o}
EXECBIN	= myshell
GCC = gcc -g -O0 -Wall -Wextra -std=gnu99

all : ${EXECBIN}

${EXECBIN} : ${OBJECTS}
	${GCC} -o $@ ${OBJECTS} -lfl

myshell.o: myshell.c
	${GCC} -c myshell.c

lex.yy.o: lex.yy.c
	${GCC} -c lex.yy.c

lex.yy.c: lex.c
	flex lex.c

clean :
	- rm ${OBJECTS}
