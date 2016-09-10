HIDATO_DEBUG_C_FLAGS=-g -c -Wall -Wextra -Waggregate-return -Wcast-align -Wcast-qual -Wconversion -Wformat=2 -Winline -Wlong-long -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -Wno-import -Wpointer-arith -Wredundant-decls -Wreturn-type -Wshadow -Wstrict-prototypes -Wswitch -Wwrite-strings

hidato_debug.exe: hidato_debug.o
	gcc -g -o hidato_debug.exe hidato_debug.o

hidato_debug.o: hidato.c hidato_debug.make
	gcc ${HIDATO_DEBUG_C_FLAGS} -o hidato_debug.o hidato.c
