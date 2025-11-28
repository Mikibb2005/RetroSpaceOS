#!/bin/bash

# Script para capturar una screenshot del OS
echo "Iniciando QEMU y capturando screenshot..."

# Iniciar QEMU con monitor QMP
qemu-system-i386 \
  -cdrom RetroSpaceOS.iso \
  -boot d \
  -m 512M \
  -display sdl \
  -qmp unix:/tmp/qemu-monitor.sock,server,nowait &

QEMU_PID=$!
echo "QEMU PID: $QEMU_PID"

# Esperar a que QEMU inicie
sleep 3

# Capturar screenshot via QMP
echo '{"execute":"qmp_capabilities"}' | socat - UNIX-CONNECT:/tmp/qemu-monitor.sock
echo '{"execute":"screendump","arguments":{"filename":"screenshot.ppm"}}' | socat - UNIX-CONNECT:/tmp/qemu-monitor.sock

sleep 1

# Matar QEMU
kill $QEMU_PID 2>/dev/null

echo "Screenshot capturada en screenshot.ppm"
