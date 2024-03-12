# -*-Makefile-*-

CC = clang
OPT = -o


run: build
	${TMPDIR}hhpp

build: main.c pathData.txt
	$(CC) ./main.c $(OPT) ${TMPDIR}hhpp

install:
	${CC} ./main.c ${OPT} ~/Developer/system_script/hhpp
