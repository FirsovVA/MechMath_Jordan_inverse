CFLAGS=-O3 -fsanitize=address -mfpmath=sse -fstack-protector-all -W -Wall -Wextra -Wunused -Wcast-align -Werror -pedantic -pedantic-errors -Wfloat-equal -Wpointer-arith -Wformat-security -Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wcast-align -Wno-long-long -std=gnu99 -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wold-style-definition -Wdeclaration-after-statement -Wbad-function-cast -Wnested-externs

LDFLAGS=-fsanitize=address 

all: prog

prog: main.o create.o solve.o matrix.o
	mpicc main.o create.o solve.o matrix.o -o prog $(LDFLAGS)

main.o: main.c matrix.h algorithm.h
	mpicc $(CFLAGS) main.c -c

create.o: create.c matrix.h
	mpicc $(CFLAGS) create.c -c

solve.o: solve.c algorithm.h
	mpicc $(CFLAGS) solve.c -c

matrix.o: matrix.c matrix.h
	mpicc $(CFLAGS) matrix.c -c

clean:
	rm -rf *.o prog 
