.DEFAULT_GOAL := all

SOURCES := $(wildcard *.c)
OBJECTS := $(patsubst %.c, build/%.o, $(SOURCES))

#Support variables
ROOT_DIR = $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
BUILD_LOG = $(ROOT_DIR)/build.log

#Compile programs and flags
CC = gcc
CFLAGS = -O2 -g
LD = ld
ASM = as
ALAGS = $(C_OPTS)

#Support program and flags
OBJDUMP = objdump

export

all: $(OBJECTS)
	$(CC) -o ./termgui $(OBJECTS)
	objdump -x -s -d ./termgui > ./objdump.txt
	readelf -a ./termgui > ./elfdump.txt

generate_reflection:
	gtags
	ctags --c-kinds=mstvl --excmd=number termgui.c termgui.h
	php $(ROOT_DIR)/parse_ctags.php c_reflect

./build/%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	
clean:
	rm -rf build/*.o
	rm -rf objdump.txt
	rm -rf elfdump.txt
	rm -rf termgui