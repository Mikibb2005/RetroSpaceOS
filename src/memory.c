#include "memory.h"
#include "types.h"

// Simple linked list allocator
struct block_header {
  size_t size;
  bool is_free;
  struct block_header *next;
};

// Usamos una dirección fija segura para el heap (4MB)
#define HEAP_START 0x00400000
#define HEAP_SIZE 0x00400000 // 4MB de heap

static struct block_header *head = NULL;

void kheap_init() {
  head = (struct block_header *)HEAP_START;
  head->size = HEAP_SIZE - sizeof(struct block_header);
  head->is_free = true;
  head->next = NULL;
}

void *kmalloc(size_t size) {
  // Alinear size a 4 bytes
  if (size % 4 != 0)
    size += 4 - (size % 4);

  struct block_header *curr = head;
  while (curr) {
    if (curr->is_free && curr->size >= size) {
      // Split block si sobra suficiente espacio
      if (curr->size > size + sizeof(struct block_header) + 16) {
        struct block_header *new_block =
            (struct block_header *)((uint8_t *)curr +
                                    sizeof(struct block_header) + size);
        new_block->size = curr->size - size - sizeof(struct block_header);
        new_block->is_free = true;
        new_block->next = curr->next;

        curr->size = size;
        curr->next = new_block;
      }
      curr->is_free = false;
      return (void *)((uint8_t *)curr + sizeof(struct block_header));
    }
    curr = curr->next;
  }
  return NULL; // Out of Memory
}

void kfree(void *ptr) {
  if (!ptr)
    return;
  struct block_header *header =
      (struct block_header *)((uint8_t *)ptr - sizeof(struct block_header));
  header->is_free = true;

  // Merge con el siguiente si está libre (Coalescing simple)
  if (header->next && header->next->is_free) {
    header->size += sizeof(struct block_header) + header->next->size;
    header->next = header->next->next;
  }
}

void *kmemset(void *ptr, int value, size_t num) {
  unsigned char *p = ptr;
  while (num--) {
    *p++ = (unsigned char)value;
  }
  return ptr;
}

void *kmemcpy(void *dest, const void *src, size_t n) {
  char *d = dest;
  const char *s = src;
  while (n--) {
    *d++ = *s++;
  }
  return dest;
}
