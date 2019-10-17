#!/bin/bash

export PATH="$HOME/opt/cross/bin:$PATH"

mkdir build

echo "Compile stdlib files"
cd stdlib

for file in $(find . -maxdepth 1 -type f -name "*.c")
do
	temp=$(echo "${file%.*}")
	name=$(echo $temp | sed 's/^..//')
	echo "Compilling $name"
	i686-elf-gcc -c $name.c -o ../build/$name.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Wno-unused-function -Wno-unused-variable -Wno-unused-parameter
done

echo "Compile asembler files"
i686-elf-as boot.s -o build/boot.o

echo "Compile kernel files"
i686-elf-gcc -c kernel/kernel.c -o build/kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

echo "Link kernel"
i686-elf-gcc -T linker.ld -o build/iferros.bin -ffreestanding -O2 -nostdlib build/*.o -lgcc

echo "Make system image"
mkdir -p isodir/boot/grub
cp build/iferros.bin isodir/boot/iferros.bin
cp grub.cfg isodir/boot/grub/grub.cfg
grub-mkrescue -o iferros.iso isodir

echo "Run qemu emulator"
qemu-system-i386 -cdrom iferros.iso