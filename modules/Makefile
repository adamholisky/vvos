.DEFAULT_GOAL := all

SHELL :=/bin/bash -O globstar
VPATH = $(shell find ./src -type d -printf "src/%P:")
SOURCES := $(shell ls src/**/*.cpp)
OBJECTS := $(patsubst %.cpp, build/%.o, $(shell ls src/**/*.cpp | xargs -n 1 basename))

#Support variables
ROOT_DIR = $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
BUILD_LOG = $(ROOT_DIR)/build.log

#Compile programs and flags
CC = /usr/local/osdev/bin/i686-elf-gcc
CFLAGS = -ffreestanding -fpermissive -fno-omit-frame-pointer -O2 -nostdlib -static-libgcc -lgcc -g -I../libcvv/include -I../kernel/include
LD = /usr/local/osdev/bin/i686-elf-ld
ASM = /usr/local/osdev/bin/i686-elf-as

#Support program and flags
OBJDUMP = /usr/local/osdev/bin/i686-elf-objdump

export

all: $(SOURCES) $(OBJECTS)
	@echo $(OBJECTS)
	@cp -f build/*.vvs /usr/local/osdev/versions/v/os_root/bin

build/%.o: %.cpp
	$(CC) -fPIC $(CFLAGS) -c $< -o $@
	$(LD) -nostdlib -fPIC -shared --entry=main -o ./build/$*.vvs ./build/$*.o
	objdump -x -s -d build/$*.vvs > ./build_temp/$*_objdump.txt
	readelf -a build/$*.vvs > ./build_temp/$*_elfdump.txt

clean:
	rm -rf build_objout/*.o
	rm -rf build_temp/*.s
	rm -rf build_temp/*.o
