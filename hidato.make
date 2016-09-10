HIDATO_C_FLAGS=-c -O2 -Wall -Wextra -Waggregate-return -Wcast-align -Wcast-qual -Wconversion -Wformat=2 -Winline -Wlong-long -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -Wno-import -Wpointer-arith -Wredundant-decls -Wreturn-type -Wshadow -Wstrict-prototypes -Wswitch -Wwrite-strings

hidato.exe: hidato.o
	gcc -o hidato.exe hidato.o

hidato.o: hidato.c hidato.make
	gcc ${HIDATO_C_FLAGS} -o hidato.o hidato.c
