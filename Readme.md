VersionV is the fifth iteration of my hobby operating system. It is not meant to be a replacement for anything substantial, but instead is meant to explore the underlaying workings of modern operating system concepts.

## Goals
1. Having pursued a product management path after coding for a long time, I find myself getting too far away from the code. Thus, this project should allow me to keep my engineering skills sharp, without detracting from other important skills and steps in my professional development.
1. Practice modern good coding standards in C, C++, ASM, and assorted scripting languages
1. Test architectural ideas and concepts that will advance my overall understanding of how delecate, mission critical systems ought to be constructed
1. Use build tools to enable near full automation of the environment
1. Focus on effective debugging, code comprehension, and art; not optimization (for the most part)
1. Run on an embedded system, powering other electronic projects I'd like to do
1. Support x86, x86_64, and AMD64

## Restrictions

Code and concepts used in this repository are in no way linked to the work that I do in my day job. There is no real overlap in the contextual nor practical applications of what I and/or my teams produce in my professional life. 

All work done on this, including code and research, takes place outside of any contractual obligations I have to fulfill elsewhere. 

Where necessary and appropriate under ethical and compliance gudiance, signoff for making this repository public has been obtained.

## Status

It works?

## Todo

Stuff.

## Notes
### Graphics
If the kernel's cli options contains `-graphics_off`:
* `Video:` Blank
* `Serial 2:` stdio, with console i/o redirected to the host console
* `Serial 4:` writes to serial_out.txt, with debugf only directing to here

If the kernel's cli options contains `-graphics_on`:
* `Video:` Standard
* `Serial 2:` gets debugf output
* `Serial 4:` writes to serial_out.txt, gets debugf output

x86_64

../binutils-2.39/configure --target=i686-elf --prefix=/usr/local/osdev --with-sysroot --disable-nls --disable-werror
../binutils-2.39/configure --target=i686-elf,x86_64-elf,arm-none-eabi,aarch64-none-elf,riscv64-none-elf --prefix=/usr/local/osdev --with-sysroot --disable-nls --disable-werror


../gcc-12.2.0/configure --target=i686-elf --prefix=/usr/local/osdev --disable-nls --enable-languages=c,c++ --without-headers
../gcc-12.2.0/configure --target=x86_64-elf --prefix=/usr/local/osdev --disable-nls --enable-languages=c,c++ --without-headers
../gcc-12.2.0/configure --target=arm-none-eabi --prefix=/usr/local/osdev --disable-nls --enable-languages=c,c++ --without-headers
../gcc-12.2.0/configure --target=aarch64-none-elf --prefix=/usr/local/osdev --disable-nls --enable-languages=c,c++ --without-headers

make all-gcc && make all-target-libgcc && make install-gcc && make install-target-libgcc
