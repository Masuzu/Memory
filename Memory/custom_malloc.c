#include "custom_malloc.h"

#include <Windows.h>

typedef struct Block
{
	size_t size;
	struct Block *previous;
	struct Block *next;
	size_t free;
	byte data[1];
} Block;

typedef struct
{
	size_t size;
	Block *previous;
	Block *next;
	size_t free;
} BlockMetadata;

static HANDLE heap = NULL;
static Block *heap_base = NULL;

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
#define BLOCK_METADATA_SIZE	align(sizeof(BlockMetadata))

// Finds a free chunck large enough for 'size' bytes.
// Returns the block found or NULL or otherwise.
// 'last' will contain the last block visited.
// Note that when 'heap_base' is returned, 'last' equals NULL.
static Block *MemoryFindBlock(Block **last, size_t size)
{
	Block *b = heap_base;
	*last = NULL;
	while (b && !(b->free && b->size >= size))
	{
		*last = b;
		b = b->next;
	}
	return b;
}

// Allocates a new memory block and link it to 'last_block'.
// The block is initialized to zero and is aligned.
// Returns the allocated block.
static Block *MemoryExtendHeap(Block *last_block, size_t size)
{
	if (!heap)
	{
		heap = HeapCreate(0, 0, 0);
		if (!heap)
			return NULL;
	}

	Block *next = HeapAlloc(heap, HEAP_ZERO_MEMORY, BLOCK_METADATA_SIZE + align(size));
	if (!next)
		return NULL;

	next->free = 0;
	next->next = NULL;
	next->previous = last_block;
	next->size = size;
	if (last_block)
		last_block->next = next;
	else
		heap_base = next;
	
	return next;
}

// Splits the block passed as argument into 2 smaller blocks.
// Does nothing if 'size + BLOCK_METADATA_SIZE + POINTER_SIZE' is greater than block_to_split's size.
// Indeed, allocating a block with a size of 0 byte makes no sense.
static void MemorySplitBlock(Block *block_to_split, size_t size)
{
	Block *new_block;
	if (block_to_split->size >= size + BLOCK_METADATA_SIZE + POINTER_SIZE)
	{
		new_block = (block_to_split->data + block_to_split->size);
		new_block->free = 1;
		new_block->previous = block_to_split;
		new_block->next = block_to_split->next;
		new_block->size = block_to_split->size - size - BLOCK_METADATA_SIZE;
		block_to_split->next = new_block;
		block_to_split->size = size;
	}
}

static void MemoryMergeBlocks(Block *block_to_merge)
{
	Block *merged_block = block_to_merge;
	Block *previous = block_to_merge->previous;
	Block *next = block_to_merge->next;
	// 1. previous is free but next is not
	// 2. previous is not free while next is
	// 3. both previous and next are free
	// 4. both previous and next are not free
	if (previous)
	{
		if (previous->free)
		{
			previous->size += (block_to_merge->size + BLOCK_METADATA_SIZE);
			previous->next = next;
			if (next)
				next->previous = previous;
			merged_block = previous;
		}
	}
	if (next)
	{
		if (next->free)
		{
			merged_block->size += (next->size + BLOCK_METADATA_SIZE);
			merged_block->next = next->next;
			if (next->next)
				next->next->previous = merged_block;
		}
	}
}

static void *MemoryMalloc(size_t size)
{
	Block *last_block, *new_block;

	if (!size)
		return NULL;

	new_block = MemoryFindBlock(&last_block, size);
	if (new_block)	// Resize the free block previously allocated
	{
		MemorySplitBlock(new_block, size);
		new_block->free = 0;
	}
	else
	{
		// Allocate a new memory block
		new_block = MemoryExtendHeap(last_block, size);
	}
	if (!new_block)
		return NULL;
	return new_block->data;
}

static void* MemoryCalloc(size_t num, size_t size)
{
	return MemoryMalloc(num*size);
}

static void MemoryFree(void *memory)
{
	Block *block_to_free;
	if (!memory)
		return;
	block_to_free = ((byte*)memory - BLOCK_METADATA_SIZE);
	MemoryMergeBlocks(block_to_free);
	block_to_free->free = 1;
}

static size_t MemoryNumBlocks(void)
{
	size_t count = 0;
	Block *b = heap_base;
	while (b)
	{
		b = b->next;
		++count;
	}
	return count;
}

static size_t MemoryBlockLayout(BlockLayout** block_layout)
{
	size_t count = 0;
	Block *b = heap_base;
	while (b)
	{
		(*block_layout)[count].free = b->free;
		(*block_layout)[count].size = b->size;
		b = b->next;
		++count;
	}
	return count;
}

const NamespaceMemory Memory = { MemoryMalloc, MemoryFree, MemoryCalloc, MemoryNumBlocks, MemoryBlockLayout };