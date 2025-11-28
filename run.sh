#!/bin/bash

# Script para ejecutar RetroSpaceOS en QEMU

echo "🚀 Iniciando RetroSpaceOS en QEMU..."
echo ""

# Verificar si existe la ISO
if [ ! -f "RetroSpaceOS.iso" ]; then
    echo "❌ Error: RetroSpaceOS.iso no encontrado"
    echo "   Ejecuta primero: bash build.sh"
    exit 1
fi

# Opciones por defecto
MEMORY="512M"
SERIAL="stdio"
DISPLAY="sdl"

# Parsear argumentos
while [[ $# -gt 0 ]]; do
    case $1 in
        -m|--memory)
            MEMORY="$2"
            shift 2
            ;;
        -d|--debug)
            DEBUG=1
            shift
            ;;
        -n|--no-display)
            DISPLAY="none"
            shift
            ;;
        -k|--kernel)
            USE_KERNEL_DIRECT=1
            shift
            ;;
        -h|--help)
            echo "Uso: bash run.sh [opciones]"
            echo ""
            echo "Opciones:"
            echo "  -m, --memory <size>    Cantidad de RAM (default: 512M)"
            echo "  -k, --kernel           Arrancar kernel directo (sin ISO)"
            echo "  -d, --debug            Habilitar servidor GDB en puerto 1234"
            echo "  -n, --no-display       Ejecutar sin display gráfico"
            echo "  -h, --help             Mostrar esta ayuda"
            exit 0
            ;;
        *)
            echo "❌ Opción desconocida: $1"
            echo "   Usa -h para ver ayuda"
            exit 1
            ;;
    esac
done

# Construir comando QEMU
CMD="qemu-system-i386"

if [ "$USE_KERNEL_DIRECT" = "1" ]; then
    echo "⚠️  Usando arranque directo del kernel (sin ISO)"
    CMD="$CMD -kernel RetroSpaceOS.bin"
else
    CMD="$CMD -cdrom RetroSpaceOS.iso -boot d"
fi

CMD="$CMD -m $MEMORY"
CMD="$CMD -serial $SERIAL"

if [ "$DISPLAY" = "none" ]; then
    CMD="$CMD -display none"
elif [ "$DISPLAY" != "" ]; then
    CMD="$CMD -display $DISPLAY"
fi

if [ "$DEBUG" = "1" ]; then
    CMD="$CMD -s -S"
    echo "🐛 Modo DEBUG activado"
    echo "   Servidor GDB en localhost:1234"
    echo "   En otra terminal ejecuta:"
    echo "   $ gdb RetroSpaceOS.bin"
    echo "   (gdb) target remote localhost:1234"
    echo "   (gdb) continue"
    echo ""
fi

echo "💾 Memoria: $MEMORY"
echo "📺 Display: $DISPLAY"
echo ""
echo "Ejecutando: $CMD"
echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

# Ejecutar QEMU
$CMD

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "✅ RetroSpaceOS terminado"
