CC = gcc

all: vos-dwarf

run: all 
	./vos-dwarf

vos-dwarf: vos-dwarf.c
	$(CC) $< -g -o vos-dwarf

clean:
	rm vos-dwarf