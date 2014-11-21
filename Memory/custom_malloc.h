#ifndef __CUSTOM_MALLOC_H__
#define __CUSTOM_MALLOC_H__

#include <stddef.h>

typedef struct BlockLayout
{
	size_t size;
	int free;
} BlockLayout;

typedef struct {
	// If the function fails, a NULL pointer is returned
	void *(*malloc)(size_t size_in_bytes);
	void(*free)(void *memory);
	void* (*calloc)(size_t num, size_t size);
	size_t (*NumBlocks)(void);
	size_t (*BlockLayout)(BlockLayout**);
} NamespaceMemory;
extern const NamespaceMemory Memory;

#endif
