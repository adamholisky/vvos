.DEFAULT_GOAL := all

#Support variables
ROOT_DIR = $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
BUILD_LOG = $(ROOT_DIR)/build.log
SHELL :=/bin/bash -O globstar
VPATH = $(shell find ./src -type d -printf "src/%P:")
SOURCES_C = $(shell ls src/**/*.c)
#SOURCES_ASM = $(shell ls kernel/**/*.s)
#SOURCES_ASMS = $(shell ls kernel/**/*.S)
OBJECTS_C = $(patsubst %.c, build/%.o, $(shell ls src/**/*.c | xargs -n 1 basename))
#OBJECTS_ASM = $(patsubst %.s, build/%.o, $(shell ls kernel/**/*.s | xargs -n 1 basename))
#OBJECTS_ASMS = $(patsubst %.S, build/%.o, $(shell ls kernel/**/*.S | xargs -n 1 basename))

#Compile programs and flags
CC = /usr/local/osdev/bin/i686-elf-gcc
ASM = /usr/local/osdev/bin/i686-elf-as
LD = /usr/local/osdev/bin/i686-elf-ld
DEFINES = -DPAGING_PAE \
		  -DGRAPHICS_OFF \
		  -DBITS_32 
CFLAGS = $(DEFINES) -Wno-write-strings -fcompare-debug-second -ffreestanding -fno-omit-frame-pointer -O0 -g -I$(ROOT_DIR)/src -I$(ROOT_DIR)/../../../libcvv/libc/include
CFLAGS_END = -nostdlib -lgcc
AFLAGS = $(CFLAGS)

#Support program and flags
OBJDUMP = /usr/local/osdev/bin/i686-elf-objdump

export
all: install

libmyshare.so: $(SOURCES_C)
	$(CC) $(CFLAGS) -fPIC -o build/myshare.o -c $< $(CFLAGS_END)
	$(LD) -g -nostdlib -fPIC -shared -o libmyshare.so build/myshare.o
	$(OBJDUMP) -x -D -S libmyshare.so > objdump.txt
	readelf -a build/myshare.o > elfdump_o.txt
	readelf -a libmyshare.so > elfdump.txt
	@>&2 printf "[Build] Done\n"

install: libmyshare.so
# Do something here to put it into the os or drive path

clean:
	rm -rf *.so
	rm -rf build/*.so.*
	rm -rf objdump.txt 
	rm -rf elfdump.txt
	@>&2 echo [Clean] Done