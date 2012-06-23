all: watchdog-ctl

CFLAGS?=-std=c99 -pedantic -Wall -W -O3
OBJ=main.c watchdog-api.c

watchdog-ctl: $(OBJ)
	gcc $(OBJ) -o watchdog-ctl $(CFLAGS)

main.o: main.c
	gcc -c main.c

watchdog-api.o: watchdog-api.c
	gcc -c watchdog-api.c

run:
	./watchdog-ctl

clean:
	rm -rf *o watchdog-ctl
