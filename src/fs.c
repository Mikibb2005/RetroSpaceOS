#include "fs.h"
#include "memory.h"
#include "string.h"
#include "terminal.h"

static fs_node_t *root = NULL;
static fs_node_t *current_dir = NULL;

// Forward declarations
static void add_child(fs_node_t *parent, fs_node_t *child);

void fs_init() {
  // 1. Crear nodo raíz
  root = (fs_node_t *)kmalloc(sizeof(fs_node_t));
  kstrcpy(root->name, "/");
  root->type = FS_DIR;
  root->parent = NULL;
  root->child = NULL;
  root->next = NULL;
  root->content = NULL;
  root->size = 0;

  current_dir = root;

  // 2. Crear estructura base
  // Necesitamos crear los nodos manualmente o usar una versión interna de
  // create_dir que acepte un padre explícito, pero como current_dir es root,
  // podemos usar fs_create_dir después de inicializar.

  fs_create_dir("home");
  fs_create_dir("sys");
  fs_create_dir("bin");
  fs_create_dir("games");
  fs_create_dir("utility");

  // Configurar /home con carpetas de usuario
  fs_node_t *home = fs_open("home");
  if (home) {
    current_dir = home;
    fs_create_dir("Documents");
    fs_create_dir("Downloads");
    fs_create_dir("Images");
    fs_create_dir("Videos");
    fs_create_dir("Audio");
    // Mantenerse en home como directorio inicial
  }
}

fs_node_t *fs_get_current_dir() { return current_dir; }

static void add_child(fs_node_t *parent, fs_node_t *child) {
  child->parent = parent;
  child->next = NULL;

  if (parent->child == NULL) {
    parent->child = child;
  } else {
    fs_node_t *curr = parent->child;
    while (curr->next) {
      curr = curr->next;
    }
    curr->next = child;
  }
}

// Busca un hijo inmediato por nombre
static fs_node_t *fs_find_child(fs_node_t *parent, const char *name) {
  if (!parent || parent->type != FS_DIR)
    return NULL;
  fs_node_t *curr = parent->child;
  while (curr) {
    if (kstrcmp(curr->name, name) == 0)
      return curr;
    curr = curr->next;
  }
  return NULL;
}

// Resuelve una ruta compleja (ej: "/home/games" o "../sys")
// Devuelve el nodo final o NULL si no existe.
static fs_node_t *fs_resolve_path(const char *path) {
  if (!path || !*path)
    return current_dir;

  fs_node_t *curr;
  char temp_path[128];
  kstrcpy(temp_path, path);

  // Ruta absoluta o relativa
  if (path[0] == '/') {
    curr = root;
    // Avanzar puntero si es solo "/"
    if (path[1] == '\0')
      return root;
  } else {
    curr = current_dir;
  }

  // Tokenizar manualmente
  int i = (path[0] == '/') ? 1 : 0;
  int start = i;

  while (1) {
    if (path[i] == '/' || path[i] == '\0') {
      // Extraer token
      int len = i - start;
      if (len > 0) {
        char token[32];
        for (int k = 0; k < len && k < 31; k++)
          token[k] = path[start + k];
        token[len] = '\0';

        // Procesar token
        if (kstrcmp(token, ".") == 0) {
          // Nada
        } else if (kstrcmp(token, "..") == 0) {
          if (curr->parent)
            curr = curr->parent;
        } else {
          fs_node_t *next = fs_find_child(curr, token);
          if (!next)
            return NULL; // Ruta invalida
          curr = next;
        }
      }

      if (path[i] == '\0')
        break;
      start = i + 1;
    }
    i++;
  }

  return curr;
}

fs_node_t *fs_create_file(const char *name) {
  // Simplificación: Solo crea en directorio actual por ahora
  // Para soportar "touch /home/file" habría que separar ruta de nombre base
  if (fs_find_child(current_dir, name))
    return NULL;

  fs_node_t *node = (fs_node_t *)kmalloc(sizeof(fs_node_t));
  if (!node)
    return NULL;

  kstrcpy(node->name, name);
  node->type = FS_FILE;
  node->child = NULL;

  node->capacity = 1024;
  node->content = (char *)kmalloc(node->capacity);
  if (!node->content) {
    kfree(node);
    return NULL;
  }
  node->content[0] = '\0';
  node->size = 0;

  add_child(current_dir, node);
  return node;
}

fs_node_t *fs_create_dir(const char *name) {
  if (fs_find_child(current_dir, name))
    return NULL;

  fs_node_t *node = (fs_node_t *)kmalloc(sizeof(fs_node_t));
  if (!node)
    return NULL;

  kstrcpy(node->name, name);
  node->type = FS_DIR;
  node->child = NULL;
  node->content = NULL;
  node->size = 0;

  add_child(current_dir, node);
  return node;
}

fs_node_t *fs_open(const char *name) {
  // Usar el resolver para soportar rutas en open (ej: cat /sys/log.txt)
  return fs_resolve_path(name);
}

bool fs_write(fs_node_t *node, const char *data, size_t len) {
  if (!node || node->type != FS_FILE)
    return false;

  if (len >= node->capacity) {
    size_t new_cap = len + 1024;
    char *new_content = (char *)kmalloc(new_cap);
    if (!new_content)
      return false;

    for (size_t i = 0; i < len; i++)
      new_content[i] = data[i];

    kfree(node->content);
    node->content = new_content;
    node->capacity = new_cap;
  } else {
    for (size_t i = 0; i < len; i++)
      node->content[i] = data[i];
  }
  node->size = len;
  node->content[len] = '\0';
  return true;
}

void fs_delete(const char *name) {
  // Solo borra en directorio actual por simplicidad
  if (!current_dir)
    return;

  fs_node_t *curr = current_dir->child;
  fs_node_t *prev = NULL;

  while (curr) {
    if (kstrcmp(curr->name, name) == 0) {
      if (curr->type == FS_DIR && curr->child != NULL) {
        terminal_writestring("Error: Directorio no vacio.\n");
        return;
      }

      if (prev)
        prev->next = curr->next;
      else
        current_dir->child = curr->next;

      if (curr->content)
        kfree(curr->content);
      kfree(curr);
      return;
    }
    prev = curr;
    curr = curr->next;
  }
  terminal_writestring("No encontrado.\n");
}

void fs_list() {
  if (!current_dir)
    return;

  terminal_writestring("Directorio: ");
  fs_pwd();
  terminal_writestring("\n");

  fs_node_t *curr = current_dir->child;
  if (!curr) {
    terminal_writestring("  (vacio)\n");
    return;
  }

  while (curr) {
    terminal_writestring("  ");
    if (curr->type == FS_DIR) {
      terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
      terminal_writestring("[DIR] ");
      terminal_writestring(curr->name);
    } else {
      terminal_setcolor(
          vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
      terminal_writestring(curr->name);
      terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
      terminal_writestring(" (");
      terminal_write_dec(curr->size);
      terminal_writestring(" b)");
    }
    terminal_writestring("\n");
    terminal_setcolor(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    curr = curr->next;
  }
}

void fs_cd(const char *path) {
  fs_node_t *target = fs_resolve_path(path);

  if (target) {
    if (target->type == FS_DIR) {
      current_dir = target;
    } else {
      terminal_writestring("Error: No es un directorio.\n");
    }
  } else {
    terminal_writestring("Ruta no encontrada.\n");
  }
}

static void print_path_recursive(fs_node_t *node) {
  if (node->parent) {
    print_path_recursive(node->parent);
    if (node->parent != root)
      terminal_writestring("/");
  }
  terminal_writestring(node->name);
}

static void get_cwd_recursive(fs_node_t *node, char *buf, int *pos,
                              size_t size) {
  if (node == root) {
    if (*pos < size - 1)
      buf[(*pos)++] = '/';
    return;
  }

  if (node->parent) {
    get_cwd_recursive(node->parent, buf, pos, size);
    // Añadir separador si el padre no es root (porque root ya pone /)
    // Ojo: root pone /, así que /home queda bien.
    // Pero /home/games -> root(/) + home + / + games
    if (node->parent != root) {
      if (*pos < size - 1)
        buf[(*pos)++] = '/';
    }
  }

  int len = kstrlen(node->name);
  for (int i = 0; i < len && *pos < size - 1; i++) {
    buf[(*pos)++] = node->name[i];
  }
}

void fs_get_cwd(char *buf, size_t size) {
  if (!buf || size == 0)
    return;
  int pos = 0;
  get_cwd_recursive(current_dir, buf, &pos, size);
  buf[pos] = '\0';
}

void fs_pwd() {
  if (current_dir == root) {
    terminal_writestring("/");
  } else {
    print_path_recursive(current_dir);
  }
}
