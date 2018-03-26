#include <stddef.h>
#include <unistd.h>

typedef struct Block {
  void *ptr;
  size_t size;
  struct Block *next;
  bool available;
} Block;

unsigned int g_avl_mem = 0, g_max_avl = 0;
struct Block *g_ptr_blk = NULL, *g_ptr_blk_end = NULL;
const unsigned int g_blk_sz = sizeof(struct Block);

inline Block *_new_block(Block *ptr, size_t size) {
    Block block;
    block.ptr = (void *)((char *) ptr + g_blk_sz);
    block.size = size;
    block.next = NULL;
    block.available = false;
    *ptr = block;
    return ptr;
}

void *malloc(size_t size) {
    if (size == 0) return NULL;
    if (!g_ptr_blk) {
        void *ptr = sbrk(size + g_blk_sz);
        if (ptr == (void *) -1)
            return (void *) -1;
        g_ptr_blk = _new_block((Block *) ptr, size);
        g_ptr_blk_end = g_ptr_blk;

        return (void *) g_ptr_blk_end->ptr;

    } else {
        if (g_max_avl < size) {
            void *ptr = sbrk(size + g_blk_sz);
            if (ptr == (void *) -1)
                return (void *) -1;
            g_ptr_blk_end->next = _new_block((Block *) ptr, size);
            g_ptr_blk_end = g_ptr_blk_end->next;

            return (void *) g_ptr_blk_end->ptr;
        } else {
            for (Block *at = g_ptr_blk; at; at = at->next) {
                if (at->available) {
                    if (at->size > size) {
                        void *ptr_new_mem = sbrk(g_blk_sz);
                        if (ptr_new_mem == (void *) -1)
                            return (void *) -1;
                        Block *ptr_new_block = _new_block(
                            (Block *) ptr_new_mem, at->size - size);
                        ptr_new_block->ptr = (void *)((char *) at->ptr + size);
                        ptr_new_block->available = true;

                        ptr_new_block->next = at->next;
                        at->next = ptr_new_block;
                        at->available = false;

                        return (void *) at->ptr;
                    } else if (at->size == size) {
                        at->available = false;
                        return (void *) at->ptr;
                    }
                }
            }

        }
        return (void *) -1;
    }
}

void free(void *ptr) {
    if (!g_ptr_blk) return;
    else {
        for (Block *at = g_ptr_blk; at; at = at->next) {
            if (at->ptr == ptr) {
                at->available = true;
                g_max_avl = (at->size > g_max_avl) ? at->size : g_max_avl;

                return;
            }
        }
    }
}
