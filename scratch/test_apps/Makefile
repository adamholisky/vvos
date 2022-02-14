.DEFAULT_GOAL := all

SOURCES := $(wildcard *.c)
OBJECTS := $(patsubst %.c, build_objout/%.o, $(SOURCES))
VVS := $(patsubst %.c, build_objout/%.vvs, $(SOURCES))

#Support variables
ROOT_DIR = $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
BUILD_LOG = $(ROOT_DIR)/build.log

#Compile programs and flags
CC = /usr/local/osdev/bin/i686-elf-gcc
CFLAGS = -ffreestanding -fno-omit-frame-pointer -O2 -nostdlib -static-libgcc -lgcc -g -I../libcvv/include -I../libvv/include -I../kernel/include
LD = /usr/local/osdev/bin/i686-elf-ld
ASM = /usr/local/osdev/bin/i686-elf-as
ALAGS = $(C_OPTS) -I$(ROOT_DIR)/src/include

#Support program and flags
OBJDUMP = /usr/local/osdev/bin/i686-elf-objdump

GENASMOUT := $(shell ./generateasm.sh )
SOURCESASM := $(wildcard ./build_temp/*.s)
AMSWRAPERS := $(patsubst %.s, ./build_objout/_%.o, $(SOURCESASM))

export

all: genasm genappwrapper $(OBJECTS) genapp
	echo $(OBJECTS)

#	$(CC) -fPIC -c alpha.c -o build_objout/alpha.o
#	$(LD) -fPIC -soname=alphaso -shared -o ../../build/alpha.vvs build_objout/alpha.o

./build_objout/%.o : %.c
	$(CC) -fPIC $(CFLAGS) -c $< -o $@
	$(LD) -nostdlib -fPIC -shared --entry=__start -o ./build_objout/$*.vvs ./build_temp/app.o ./build_objout/$*.o
	objdump -x -s -d build_objout/$*.vvs > ./build_temp/$*_objdump.txt
	readelf -a build_objout/$*.vvs > ./build_temp/$*_elfdump.txt
	$(CC) -fPIC $(CFLAGS) -c ./build_temp/$*.s -o ./build_objout/$*.bin
	
genasm:
	./generateasm.sh

genapp:
	./buildapp.sh

genappwrapper:
	$(CC) -fPIC $(CFLAGS) -c ./build_support/app.c -o ./build_temp/app.o

clean:
	rm -rf build_objout/*.o
	rm -rf build_objout/*.vvs
	rm -rf build_objout/*.bin
	rm -rf build_temp/*.s
	rm -rf build_temp/*.o

# TODO:  objcopy --input binary --output elf32-i386 --binary i386 magic.txt magic_txt.o