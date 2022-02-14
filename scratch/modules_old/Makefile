.DEFAULT_GOAL := all

SOURCES := $(wildcard *.c)
OBJECTS := $(patsubst %.c, build/%.o, $(SOURCES))
VVS := $(patsubst %.c, build/%.vvs, $(SOURCES))

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
AMSWRAPERS := $(patsubst %.s, ./build/_%.o, $(SOURCESASM))

export

all: genasm genappwrapper $(OBJECTS) genapp
	echo $(OBJECTS)

#	$(CC) -fPIC -c alpha.c -o build_objout/alpha.o
#	$(LD) -fPIC -soname=alphaso -shared -o ../../build/alpha.vvs build_objout/alpha.o

./build/%.o : %.c
	$(CC) -fPIC $(CFLAGS) -c $< -o $@
	$(LD) -nostdlib -fPIC -shared --entry=__start -o ./build/$*.vvs ./build_temp/app.o ./build/$*.o
	objdump -x -s -d build/$*.vvs > ./build_temp/$*_objdump.txt
	readelf -a build/$*.vvs > ./build_temp/$*_elfdump.txt
	$(CC) -fPIC $(CFLAGS) -c ./build_temp/$*.s -o ./build/$*.bin
	
genasm:
	./generateasm.sh

genapp:
	./buildapp.sh

genappwrapper:
	$(CC) -fPIC $(CFLAGS) -c ./build_support/app.c -o ./build_temp/app.o

clean:
	rm -rf build/*.o
	rm -rf build/*.vvs
	rm -rf build/*.bin
	rm -rf build_temp/*.s
	rm -rf build_temp/*.o

# TODO:  objcopy --input binary --output elf32-i386 --binary i386 magic.txt magic_txt.o