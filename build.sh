#!/bin/bash

# ============================================================================
# RetroSpaceOS - Script de Compilación
# ============================================================================
# Este script compila todos los componentes del sistema operativo y genera
# una imagen ISO booteable que se puede ejecutar en QEMU o hardware real.
# ============================================================================

set -e  # Salir en caso de error

# Colores para mensajes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

echo -e "${CYAN}============================================${NC}"
echo -e "${CYAN}   RetroSpaceOS - Sistema de Compilación   ${NC}"
echo -e "${CYAN}============================================${NC}"
echo ""

# 1. Limpiar archivos anteriores
echo -e "${YELLOW}[1/6]${NC} Limpiando compilación anterior..."
rm -f boot.o kernel.o interrupts.o RetroSpaceOS.bin
rm -rf iso/ RetroSpaceOS.iso

# 2. Compilar bootloader
echo -e "${YELLOW}[2/6]${NC} Compilando bootloader (boot.asm)..."
nasm -f elf32 bootloader/boot.asm -o boot.o
if [ $? -eq 0 ]; then
    echo -e "  ${GREEN}✓${NC} boot.o generado"
else
    echo -e "  ${RED}✗${NC} Error compilando bootloader"
    exit 1
fi

# 3. Compilar manejadores de interrupciones
echo -e "${YELLOW}[3/6]${NC} Compilando interrupciones (interrupts.asm)..."
nasm -f elf32 bootloader/interrupts.asm -o interrupts.o
if [ $? -eq 0 ]; then
    echo -e "  ${GREEN}✓${NC} interrupts.o generado"
else
    echo -e "  ${RED}✗${NC} Error compilando interrupciones"
    exit 1
fi

# 4. Compilar código fuente C
echo -e "${YELLOW}[4/6]${NC} Compilando código fuente (src/*.c)..."
CFLAGS="-m32 -c -ffreestanding -O2 -Wall -Wextra -fno-pie -fno-stack-protector -fno-exceptions -nostdlib -mno-red-zone -I./include"

# Compilar cada archivo .c en src/
for file in src/*.c; do
    filename=$(basename "$file" .c)
    echo -e "  Compilando ${filename}.c..."
    gcc $CFLAGS "$file" -o "${filename}.o"
    if [ $? -ne 0 ]; then
        echo -e "  ${RED}✗${NC} Error compilando ${filename}.c"
        exit 1
    fi
done

echo -e "  ${GREEN}✓${NC} Código C compilado correctamente"

# 5. Enlazar todos los objetos
echo -e "${YELLOW}[5/6]${NC} Enlazando objetos..."
# Recopilar todos los archivos .o generados (boot.o, interrupts.o y los de src/)
OBJ_FILES="boot.o interrupts.o"
for file in src/*.c; do
    filename=$(basename "$file" .c)
    OBJ_FILES="$OBJ_FILES ${filename}.o"
done

ld -m elf_i386 -T linker.ld -o RetroSpaceOS.bin $OBJ_FILES
if [ $? -eq 0 ]; then
    echo -e "  ${GREEN}✓${NC} RetroSpaceOS.bin generado"
else
    echo -e "  ${RED}✗${NC} Error en el enlazado"
    exit 1
fi

# 6. Crear imagen ISO
echo -e "${YELLOW}[6/6]${NC} Creando imagen ISO booteable..."
mkdir -p iso/boot/grub
cp RetroSpaceOS.bin iso/boot/
cp bootloader/grub.cfg iso/boot/grub/
grub-mkrescue -o RetroSpaceOS.iso iso/ 2>/dev/null
if [ $? -eq 0 ]; then
    echo -e "  ${GREEN}✓${NC} RetroSpaceOS.iso generado"
else
    echo -e "  ${RED}✗${NC} Error creando ISO"
    exit 1
fi

# Mostrar información del binario
echo ""
echo -e "${CYAN}============================================${NC}"
echo -e "${GREEN}         Compilación Exitosa!              ${NC}"
echo -e "${CYAN}============================================${NC}"
echo ""
echo -e "Archivos generados:"
echo -e "  ${CYAN}•${NC} RetroSpaceOS.bin - $(ls -lh RetroSpaceOS.bin | awk '{print $5}')"
echo -e "  ${CYAN}•${NC} RetroSpaceOS.iso - $(ls -lh RetroSpaceOS.iso | awk '{print $5}')"
echo ""
echo -e "Para ejecutar, usa: ${YELLOW}./run.sh${NC}"
echo ""
