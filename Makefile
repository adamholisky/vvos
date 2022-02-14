.DEFAULT_GOAL := all

#Support variables
ROOT_DIR = $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
BUILD_LOG = $(ROOT_DIR)/build.log
SHELL :=/bin/bash -O globstar
VPATH = $(shell find ./kernel -type d -printf "kernel/%P:")
SOURCES_C = $(shell ls kernel/**/*.c)
SOURCES_CPP = $(shell ls kernel/**/*.cpp)
SOURCES_ASM = $(shell ls kernel/**/*.s)
SOURCES_ASMS = $(shell ls kernel/**/*.S)
OBJECTS_C = $(patsubst %.c, build/%.o, $(shell ls kernel/**/*.c | xargs -n 1 basename))
OBJECTS_CPP = $(patsubst %.cpp, build/%.o, $(shell ls kernel/**/*.cpp | xargs -n 1 basename))
OBJECTS_ASM = $(patsubst %.s, build/%.o, $(shell ls kernel/**/*.s | xargs -n 1 basename))
OBJECTS_ASMS = $(patsubst %.S, build/%.o, $(shell ls kernel/**/*.S | xargs -n 1 basename))

#Compile programs and flags
CC = /usr/local/osdev/bin/i686-elf-gcc
DEFINES = -DPAGING_PAE \
		  -DGRAPHICS_ON \
		  -DBITS_32 \
		  #-DDF_COM4_ONLY
#		  -DGRAPHICS_ON
CFLAGS = $(DEFINES) -Wno-write-strings -ffreestanding -fno-omit-frame-pointer -O0 -g -I$(ROOT_DIR)/kernel/include -I$(ROOT_DIR)/libvv/include -I$(ROOT_DIR)/libcvv/include
CFLAGS_END = -nostdlib -lgcc
ASM = /usr/local/osdev/bin/i686-elf-as
AFLAGS = $(CFLAGS) -I$(ROOT_DIR)/kernel/include -I$(ROOT_DIR)/libvv/include -I$(ROOT_DIR)/libcvv/include

#Support program and flags
OBJDUMP = /usr/local/osdev/bin/i686-elf-objdump
QEMU = qemu-system-i386 
QEMU_COMMON = 	-drive format=raw,if=ide,file=$(ROOT_DIR)/vv_hd.img \
				-device isa-debug-exit,iobase=0xf4,iosize=0x04 \
				-nic user,ipv6=off,model=e1000,mac=52:54:98:76:54:32 \
				-m 4G \
				-serial tcp:192.168.0.100:21,nodelay=on,reconnect=0 \
				-serial stdio \
				-serial tcp::6699,nodelay=on,server=on,wait=no \
				-serial file:$(ROOT_DIR)/serial_out.txt \
				-no-reboot
QEMU_DISPLAY_NONE =	-display none
QEMU_DISPLAY_NORMAL = -vga std
QEMU_DEBUG_COMMON = -S -gdb tcp::5894 
QEMU_DEBUG_LOGGING = -D $(ROOT_DIR)/qemu_debug_log.txt -d int,cpu_reset 

# -serial tcp:192.168.0.100:6699,nodelay=on,server=on,wait=no \
# -serial null \

export

all: debug_dump install

build/versionv.bin: $(SOURCES_C) $(SOURCES_CPP) $(SOURCES_ASM) $(SOURCES_ASMS) $(OBJECTS_C) $(OBJECTS_CPP) $(OBJECTS_ASM) $(OBJECTS_ASMS)
	$(CC) -T kernel/build_support/linker.ld -o build/versionv.bin $(CFLAGS) /usr/local/osdev/lib/gcc/i686-elf/11.2.0/libgcc.a libcvv/vvlibc.o $(OBJECTS_C) $(OBJECTS_CPP) $(OBJECTS_ASM) $(OBJECTS_ASMS) $(CFLAGS_END)
	objdump -x -D -S build/versionv.bin > objdump.txt
	readelf -a build/versionv.bin > elfdump.txt
	@>&2 printf "[Build] Done\n"

build/%.o: %.c
	@>&2 printf "[Build] $<\n"
	$(eval OBJNAME := $(shell basename $@))
	$(CC) $(CFLAGS) $(CFLAGS_END) -c $< -o build/$(OBJNAME) >> $(BUILD_LOG)

build/%.o: %.cpp
	@>&2 printf "[Build] $<\n"
	$(eval OBJNAME := $(shell basename $@))
	$(CC) $(CFLAGS) $(CFLAGS_END) -fpermissive -c $< -o build/$(OBJNAME) >> $(BUILD_LOG)

#build/%.o: kernel/**/%.s
build/%.o: %.s
	@>&2 printf "[Build] $<\n"
	$(eval OBJNAME := $(shell basename $@))
	$(ASM) -c $< -o build/$(OBJNAME) >> $(BUILD_LOG)

build/%.o: %.S
	@>&2 printf "[Build] $<\n"
	$(eval OBJNAME := $(shell basename $@))
	$(CC) $(AFLAGS) $(CFLAGS_END) -c $< -o build/$(OBJNAME) >> $(BUILD_LOG)

debug_dump:
	@>&2 echo [Build] Makefile Debug Dump
	@make debug_dump_stage2 >> $(BUILD_LOG)

debug_dump_stage2:
	@echo "Start Debug Dump"
	@echo "----------"
	@echo "vpath:" $(VPATH)
	@echo "----------"
	@echo "*.s:" $(SOURCES_ASM)
	@echo " "
	@echo "*.o:" $(OBJECTS_ASM)
	@echo "----------"
	@echo "*.S:" $(SOURCES_ASMS)
	@echo " "
	@echo "*.o:" $(OBJECTS_ASMS)
	@echo "----------"
	@echo "*.c:" $(SOURCES_C)
	@echo " "
	@echo "*.o:" $(OBJECTS_C)
	@echo "----------"
	@echo "*.cpp:" $(SOURCES_CPP)
	@echo " "
	@echo "*.o:" $(OBJECTS_CPP)
	@echo "----------"
	@echo "End Debug Dump"
	@echo " "

install:
	@make install_stage2 >> $(BUILD_LOG)
	@>&2 printf "[Install] Done\n"

install_stage2: build/versionv.bin
	@>&2 echo [Install] Installing to vv_hd.img
	mount hd_mount_dir
	cp build/versionv.bin -f hd_mount_dir/boot/versionv.bin
	umount hd_mount_dir 

run: install
	$(QEMU) $(QEMU_COMMON) $(QEMU_DISPLAY_NORMAL) 

run_debug: install
	$(QEMU) $(QEMU_COMMON) $(QEMU_DISPLAY_NORMAL) $(QEMU_DEBUG_COMMON)

run_ng: install
	$(QEMU) $(QEMU_COMMON) $(QEMU_DISPLAY_NONE) && echo "Exit Code: $$?" || echo "Exit Code: $$?"

run_ng_debug: install
	$(QEMU) $(QEMU_COMMON) $(QEMU_DISPLAY_NONE) $(QEMU_DEBUG_COMMON)

gdb:
	gdb -q --command=/usr/local/osdev/versions/v/kernel/build_support/main_debug.gdb 

clean:
	@rm -rf build.log
	@make clean_stage_2 >> $(BUILD_LOG)

clean_stage_2:
	rm -rf build/*.o 
	rm -rf build/*.bin 
	rm -rf objdump.txt 
	rm -rf versionv.iso
	@>&2 echo [Clean] Done
