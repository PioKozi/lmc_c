lmc: main.c
	clang -Wall -g -O0 main.c -o lmc

clean:
	rm -f lmc
