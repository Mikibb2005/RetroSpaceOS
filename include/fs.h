#ifndef FS_H
#define FS_H

#include "types.h"

#define FS_FILE 0
#define FS_DIR 1

typedef struct fs_node {
  char name[32];
  uint8_t type;           // FS_FILE o FS_DIR
  struct fs_node *parent; // Directorio padre
  struct fs_node *child;  // Primer hijo (si es DIR)
  struct fs_node *next;   // Siguiente hermano

  // Solo para archivos
  char *content;
  size_t size;
  size_t capacity;
} fs_node_t;

// Alias para compatibilidad
typedef fs_node_t file_t;

void fs_init();
fs_node_t *fs_create_file(const char *name);
fs_node_t *fs_create_dir(const char *name);
fs_node_t *fs_open(const char *name); // Busca en directorio actual
bool fs_write(fs_node_t *node, const char *data, size_t len);
void fs_delete(const char *name);
void fs_list(); // Lista directorio actual

// Navegación
void fs_cd(const char *path);
void fs_pwd();
void fs_get_cwd(char *buf, size_t size);
fs_node_t *fs_get_current_dir();

#endif
