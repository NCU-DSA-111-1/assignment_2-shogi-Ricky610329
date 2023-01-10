build = $(wildcard build/*.o)
CC = gcc -pthread -c
CFLAG = -g

main:main.o
	${CC} $(build) -o bin/main  -leb -lm${CFLAGS}
main.o:
	${CC} src/main.c -o build/main.o ${CFLAGS}   


clean:                             
	@rm -rf *.o   main
