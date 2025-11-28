#!/bin/bash

# 1. Limpia y recompila
rm -f boot.o kernel.o RetroSpaceOS.bin
nasm -f elf32 bootloader/boot.asm -o boot.o
gcc -m32 -c src/kernel.c -o kernel.o -ffreestanding -O2 -Wall -Wextra -fno-pie
ld -m elf_i386 -T linker.ld -o RetroSpaceOS.bin boot.o kernel.o

# 2. Prepara estructura ISO
rm -rf iso/ RetroSpaceOS.iso
mkdir -p iso/boot/grub
cp RetroSpaceOS.bin iso/boot/
cp bootloader/grub.cfg iso/boot/grub/


# 3. Crea ISO booteable
grub-mkrescue -o RetroSpaceOS.iso iso/
