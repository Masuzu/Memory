#include "custom_malloc.h"

#include <Windows.h>

typedef struct block
{
	size_t size;
	struct block *next;
	int free;
} block;

static HANDLE heap = NULL;
static block *heap_base = NULL;

// It is often required to align data: 4-byte aligned on 32-bit platforms and 8-bype aligned on 64-bit platforms 
// Given an integer x, the closest integer larger or equal to x and which is a multiple of POINTER_SIZE is:
// ((x-1)/POINTER_SIZE)*POINTER_SIZE+POINTER_SIZE

#ifdef _WIN64
#define POINTER_SIZE	8

// Shifting the bits to the right by i positions is equivalent to the division by 2^i and is faster than the division
#define align(x)	(((((x)-1)>>3)<<3)+POINTER_SIZE)
#else
#define POINTER_SIZE	4
#define align(x)	(((((x)-1)>>2)<<2)+POINTER_SIZE)
#endif

// Aligned block size
#define ALIGNED_BLOCK_SIZE	align(sizeof(block))

// Finds a free chunck large enough for 'size' bytes.
// Returns the block found or NULL or otherwise.
// 'last' will contain the last block visited.
static block *Memory_FindBlock(block **last, size_t size)
{
	block *b = heap_base;
	*last = b;
	while (b && !(b->free && b->size >= size))
	{
		*last = b;
		b = b->next;
	}
	return b;
}

// Allocates a new memory block and link it to 'last_block'
static block *Memory_ExtendHeap(block *last_block, size_t size)
{
	if (!heap)
	{
		heap = HeapCreate(0, 0, 0);
		if (!heap)
			return NULL;
	}

	block *next = HeapAlloc(heap, HEAP_ZERO_MEMORY, ALIGNED_BLOCK_SIZE + align(size));
	if (!next)
		return NULL;

	next->free = 0;
	next->size = size;
	next->next = NULL;
	if (last_block)
		last_block->next = next;
	else
		heap_base = next;
	
	return next;
}

static void *Memory_malloc(size_t size)
{
	block *last_block, *free_block;
	byte *new_block;
	
	free_block = Memory_FindBlock(&last_block, size);
	if (free_block)
	{
		free_block->free = 0;
		return (byte*)free_block + ALIGNED_BLOCK_SIZE;
	}
	new_block = Memory_ExtendHeap(last_block, size);
	if (new_block)
		return new_block + ALIGNED_BLOCK_SIZE;
	return NULL;
}

static void Memory_free(void *memory)
{
	block *block_to_free = ((byte*)memory - ALIGNED_BLOCK_SIZE);
	block_to_free->free = 1;
}

namespace_Memory const Memory = { Memory_malloc, Memory_free };