# 📚 Índice de Documentación - RetroSpaceOS

Navegación rápida a toda la documentación del proyecto.

---

## 🚀 Inicio Rápido

**Si es tu primera vez aquí, empieza por:**

1. 📖 [README.md](README.md) - Visión general del proyecto
2. 📊 [docs/RESUMEN.md](docs/RESUMEN.md) - Estado actual y próximos pasos
3. 🗺️ [docs/ROADMAP.md](docs/ROADMAP.md) - Plan de desarrollo detallado

---

## 📂 Estructura del Proyecto

### Código Fuente
- [`bootloader/boot.asm`](bootloader/boot.asm) - Bootloader en Assembly
- [`bootloader/grub.cfg`](bootloader/grub.cfg) - Configuración de GRUB
- [`src/kernel.c`](src/kernel.c) - ⭐ **KERNEL PRINCIPAL** (aquí trabajas)
- [`linker.ld`](linker.ld) - Script del linker

### Scripts de Compilación y Ejecución
- [`build.sh`](build.sh) - Compila el proyecto
- [`run.sh`](run.sh) - Ejecuta el OS en QEMU

---

## 📚 Documentación

### 📖 Guías Principales

| Documento | Descripción | Cuándo usar |
|-----------|-------------|-------------|
| [README.md](README.md) | Documentación completa del proyecto | Primera lectura, referencia general |
| [docs/ROADMAP.md](docs/ROADMAP.md) | Plan de desarrollo paso a paso | Mientras implementas cada fase |
| [docs/RESUMEN.md](docs/RESUMEN.md) | Estado actual del proyecto | Para saber qué está hecho y qué falta |
| [docs/QUICK_REFERENCE.md](docs/QUICK_REFERENCE.md) | Referencia rápida | Búsquedas rápidas de información |
| [docs/PROGRESS.md](docs/PROGRESS.md) | Tracking de progreso | Marcar tareas completadas |
| [docs/ejemplos.c](docs/ejemplos.c) | Código de referencia | Cuando necesites ejemplos de implementación |

### 📋 Detalle de Cada Documento

#### 📖 README.md
**Tamaño**: ~350 líneas  
**Contiene**:
- Descripción del proyecto
- Requisitos e instalación
- Instrucciones de compilación
- Guía de desarrollo por fases
- Arquitectura del sistema
- Debugging y troubleshooting
- Recursos externos

**Empieza aquí si**: Es tu primera vez con el proyecto

---

#### 🗺️ docs/ROADMAP.md
**Tamaño**: ~450 líneas  
**Contiene**:
- Progreso general con gráficos
- 8 fases de desarrollo detalladas
- Tareas específicas para cada fase
- Código de ejemplo para implementar
- Verificaciones de cada fase
- Estimaciones de tiempo y complejidad

**Empieza aquí si**: Estás listo para empezar a programar

---

#### 📊 docs/RESUMEN.md
**Tamaño**: ~380 líneas  
**Contiene**:
- Estado actual del proyecto
- Lo que está funcionando
- Lo que falta por hacer
- Estructura de archivos
- Cómo empezar paso a paso
- Métricas del proyecto
- Objetivos de aprendizaje

**Empieza aquí si**: Quieres un overview rápido

---

#### 🔍 docs/QUICK_REFERENCE.md
**Tamaño**: ~350 líneas  
**Contiene**:
- Comandos rápidos
- Direcciones de memoria importantes
- Puertos de E/S
- Tabla de colores VGA
- Flags de IDT
- Tabla de interrupciones
- Instrucciones Assembly
- Tips de debugging
- FAQs

**Empieza aquí si**: Necesitas consultar algo específico rápidamente

---

#### 📊 docs/PROGRESS.md
**Tamaño**: ~380 líneas  
**Contiene**:
- Checklist de todas las tareas (50+)
- Secciones para notas por tarea
- Registro de sesiones de trabajo
- Sistema de logros
- Estadísticas personales

**Empieza aquí si**: Quieres trackear tu progreso

---

#### 💡 docs/ejemplos.c
**Tamaño**: ~650 líneas  
**Contiene**:
- Implementaciones completas de referencia
- Ejemplos para todas las fases:
  - Terminal VGA
  - IDT e interrupciones
  - Timer (PIT)
  - Teclado PS/2
  - Gestión de memoria
  - Procesos y multitarea
  - Sistema de archivos
  - Shell
- Funciones utilitarias (strcmp, memcpy, itoa, etc.)
- Configuración del PIC

**Empieza aquí si**: Necesitas ver cómo implementar una función específica

---

## 🎯 Flujo de Trabajo Recomendado

```
┌─────────────────────────────────────────────────────┐
│ 1. Lee README.md (visión general)                   │
└──────────────────┬──────────────────────────────────┘
                   │
                   ▼
┌─────────────────────────────────────────────────────┐
│ 2. Lee docs/RESUMEN.md (estado actual)              │
└──────────────────┬──────────────────────────────────┘
                   │
                   ▼
┌─────────────────────────────────────────────────────┐
│ 3. Abre docs/ROADMAP.md (plan de trabajo)           │
└──────────────────┬──────────────────────────────────┘
                   │
                   ▼
┌─────────────────────────────────────────────────────┐
│ 4. Comienza Fase 1 en src/kernel.c                  │
│    - Lee los TODOs en el código                     │
│    - Consulta docs/ejemplos.c cuando necesites      │
│    - Ten docs/QUICK_REFERENCE.md abierto            │
└──────────────────┬──────────────────────────────────┘
                   │
                   ▼
┌─────────────────────────────────────────────────────┐
│ 5. Compila y prueba (build.sh + run.sh)             │
└──────────────────┬──────────────────────────────────┘
                   │
                   ▼
┌─────────────────────────────────────────────────────┐
│ 6. Marca progreso en docs/PROGRESS.md               │
└──────────────────┬──────────────────────────────────┘
                   │
                   ▼
┌─────────────────────────────────────────────────────┐
│ 7. Repite desde paso 4 para siguiente tarea         │
└─────────────────────────────────────────────────────┘
```

---

## 🔍 Búsqueda Rápida por Tema

### Quiero aprender sobre...

#### 🎨 **Colores VGA**
- docs/QUICK_REFERENCE.md (línea 70)
- src/kernel.c (línea 40-60)

#### 💻 **Terminal VGA**
- docs/ROADMAP.md - Fase 1 (línea 80)
- docs/ejemplos.c (línea 20-150)
- src/kernel.c (línea 110-230)

#### ⚡ **Interrupciones (IDT)**
- docs/ROADMAP.md - Fase 2 (línea 250)
- docs/ejemplos.c (línea 180-280)
- docs/QUICK_REFERENCE.md (línea 110-160)

#### ⏱️ **Timer (PIT)**
- docs/ROADMAP.md - Fase 3 (línea 420)
- docs/ejemplos.c (línea 300-350)
- docs/QUICK_REFERENCE.md (línea 45)

#### ⌨️ **Teclado**
- docs/ROADMAP.md - Fase 4 (línea 500)
- docs/ejemplos.c (línea 380-420)
- docs/QUICK_REFERENCE.md (línea 50)

#### 🧠 **Memoria**
- docs/ROADMAP.md - Fase 5 (línea 580)
- docs/ejemplos.c (línea 450-520)

#### 🔄 **Procesos**
- docs/ROADMAP.md - Fase 6 (línea 650)
- docs/ejemplos.c (línea 540-580)

#### 💾 **Sistema de Archivos**
- docs/ROADMAP.md - Fase 7 (línea 720)
- docs/ejemplos.c (línea 600-640)

#### 💻 **Shell**
- docs/ROADMAP.md - Fase 8 (línea 790)
- docs/ejemplos.c (línea 660-720)

---

## 🆘 Troubleshooting - ¿Dónde buscar?

| Problema | Dónde buscar |
|----------|--------------|
| No compila | README.md - Sección "Problemas Comunes" |
| Triple fault | docs/QUICK_REFERENCE.md - FAQ |
| No aparece nada en pantalla | docs/QUICK_REFERENCE.md - FAQ |
| No sé qué hacer | docs/ROADMAP.md - Fase actual |
| Necesito ejemplo de código | docs/ejemplos.c |
| Error en QEMU | README.md - Sección "Debugging" |
| No entiendo un concepto | README.md - Sección "Recursos" |

---

## 📊 Estadísticas del Proyecto

```
Total de líneas de código y documentación: ~3,400 líneas

Distribución:
├── Código (kernel + bootloader): ~600 líneas
├── Documentación: ~2,200 líneas
├── Ejemplos de código: ~650 líneas
└── Scripts y configs: ~100 líneas

Archivos:
├── Código fuente: 4 archivos
├── Documentación: 6 archivos
├── Scripts: 2 archivos
└── Total: 12 archivos principales
```

---

## 🎓 Rutas de Aprendizaje

### 🟢 Principiante (Nunca hice un OS)
1. README.md
2. docs/RESUMEN.md
3. docs/ROADMAP.md - Solo Fase 1
4. docs/ejemplos.c - Ejemplos de Terminal
5. Implementa Fase 1 completa antes de continuar

### 🟡 Intermedio (Conozco algo de OS)
1. docs/RESUMEN.md
2. docs/ROADMAP.md
3. src/kernel.c - Lee TODOs
4. Empieza directo con Fase 1
5. Consulta docs/ejemplos.c cuando lo necesites

### 🔴 Avanzado (Experiencia con OS)
1. src/kernel.c - Comprende la estructura
2. docs/ROADMAP.md - Vista rápida
3. Implementa directamente
4. docs/QUICK_REFERENCE.md para consultas

---

## 🔗 Enlaces Externos Importantes

Estos recursos están mencionados en la documentación:

- **OSDev Wiki**: https://wiki.osdev.org/
- **Intel Manuals**: https://software.intel.com/sdm
- **Bran's Tutorial**: http://www.osdever.net/bkerndev/
- **James Molloy's Tutorial**: http://www.jamesmolloy.co.uk/tutorial_html/
- **r/osdev**: https://reddit.com/r/osdev

---

## 📝 Comandos Más Usados

```bash
# Compilar
bash build.sh

# Ejecutar
bash run.sh

# Ejecutar con debug
bash run.sh -d

# Limpiar
rm -f *.o *.bin
rm -rf iso/

# Ver estructura
tree -I '.git'

# Contar líneas
wc -l src/kernel.c
```

---

## ✅ Checklist de Inicio

Antes de empezar a programar, asegúrate de:

- [ ] Haber leído README.md completo
- [ ] Entender la estructura del proyecto
- [ ] Tener QEMU instalado y funcionando
- [ ] Poder compilar exitosamente (`bash build.sh`)
- [ ] Poder ejecutar el OS (`bash run.sh`)
- [ ] Ver el mensaje inicial en QEMU
- [ ] Tener docs/ROADMAP.md abierto
- [ ] Tener docs/ejemplos.c a mano
- [ ] Haber creado un repo Git (recomendado)

---

## 🎯 Objetivos por Fase

- **Fase 1**: Terminal funcional con scroll ⏱️ 2-4h
- **Fase 2**: Interrupciones funcionando ⏱️ 6-10h
- **Fase 3**: Timer generando ticks ⏱️ 3-5h
- **Fase 4**: Entrada de teclado ⏱️ 4-6h
- **Fase 5**: Memoria dinámica ⏱️ 10-15h
- **Fase 6**: Multitarea básica ⏱️ 15-20h
- **Fase 7**: Sistema de archivos ⏱️ 10-15h
- **Fase 8**: Shell interactivo ⏱️ 8-12h

**Total estimado**: 55-85 horas

---

## 🏆 Hitos del Proyecto

- [ ] **Hito 1**: Compilar y ejecutar por primera vez
- [ ] **Hito 2**: Terminal con colores y scroll
- [ ] **Hito 3**: Primera interrupción manejada
- [ ] **Hito 4**: Ver ticks del timer
- [ ] **Hito 5**: Escribir con el teclado
- [ ] **Hito 6**: Hacer primer kmalloc
- [ ] **Hito 7**: Dos procesos ejecutándose
- [ ] **Hito 8**: Leer archivo del disco
- [ ] **Hito 9**: Ejecutar comando en shell
- [ ] **Hito 10**: OS completo funcional

---

## 📞 Ayuda y Soporte

Si te atascas, sigue este orden:

1. **Lee los comentarios en src/kernel.c**
2. **Consulta docs/ejemplos.c**
3. **Busca en docs/QUICK_REFERENCE.md**
4. **Lee docs/ROADMAP.md para tu fase**
5. **Busca en OSDev Wiki**
6. **Pregunta en r/osdev**

---

**¡Éxito con tu desarrollo! 🚀**

---

_Última actualización: 2025-11-28_
