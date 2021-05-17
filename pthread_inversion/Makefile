CFLAGS=-fsanitize=address -mfpmath=sse -fstack-protector-all -W -Wall -Wextra -Wunused -Wcast-align -Werror -pedantic -pedantic-errors -Wfloat-equal -Wpointer-arith -Wformat-security -Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wcast-align -Wno-long-long -std=gnu99 -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wold-style-definition -Wdeclaration-after-statement -Wbad-function-cast -Wnested-externs -lm -O3 -lpthread

LDFLAGS=-fsanitize=address 

all: prog

prog: main2.o create.o solve2.o matrix.o
	gcc main2.o create.o solve2.o matrix.o -o prog $(LDFLAGS)

main.o: main2.c matrix.h algorithm.h
	gcc $(CFLAGS) main2.c -c

create.o: create.c matrix.h
	gcc $(CFLAGS) create.c -c

solve.o: solve2.c algorithm.h
	gcc $(CFLAGS) solve2.c -c

matrix.o: matrix.c matrix.h
	gcc $(CFLAGS) matrix.c -c

clean:
	rm -rf *.o prog 
