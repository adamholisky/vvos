#!/bin/bash
# 
# Setup the initial dev environment that will be expected for VOS to compile
# 
# Cross-compiler build commands:
#     ../binutils-2.39/configure --target=i686-elf --prefix=/usr/local/osdev --with-sysroot --disable-nls --disable-werror && make && make install
#     ../gcc-12.2.0/configure --target=i686-elf --prefix=/usr/local/osdev --disable-nls --enable-languages=c,c++ --without-headers && make all-gcc && make all-target-libgcc
#     make install-gcc && make install-target-libgcc

mkdir /usr/local/osdev
mkdir /usr/local/osdev/share
mkdir /usr/local/osdev/versions
mkdir /usr/local/osdev/versions/v

cd /usr/local/osdev/share

packages=("build-essential" "bison" "flex" "libgmp3-dev" "libmpc-dev" "libmpfr-dev" "texinfo")

for package in ${packages[@]}; do 
    sudo apt --assume-yes install $package
done

file="binutils-2.39.tar.gz";
source="https://ftp.gnu.org/gnu/binutils/binutils-2.39.tar.gz"
if [ -f "$file" ]; then
    echo "$file already exists."
else
    wget $source
fi
tar -xf $file

file="gcc-12.2.0.tar.gz";
source="https://ftp.gnu.org/gnu/gcc/gcc-12.2.0/gcc-12.2.0.tar.gz"
if [ -f "$file" ]; then
    echo "$file already exists."
else
    wget $source
fi
tar -xf $file

