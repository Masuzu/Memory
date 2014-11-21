#include "custom_malloc.h"

#include <stdio.h>
#include <malloc.h>

#define TEST_SUCCESSFUL_MEMORY_ALLOCATION(p) if (!(p))	{printf("ERROR: Out of memory\n"); return 1;}

void PrintBlockLayout(BlockLayout **block_layout)
{
	size_t num_blocks =	Memory.BlockLayout(block_layout);
	for (int i = 0; i < num_blocks; ++i)
	{
		printf("Block %i has a size of %i bytes and is ", i, (*block_layout)[i].size);
		if ((*block_layout)[i].free)
			printf("free.\n");
		else
			printf("not free.\n");
	}
}

int main()
{
	printf("sizeof(int) = %i bytes\n\n", sizeof(int));

	BlockLayout* block_layout = Memory.malloc(sizeof(BlockLayout) * 10);
	memset(block_layout, 0, sizeof(BlockLayout) * 10);

	int *p = Memory.malloc(sizeof(int) * 100);
	TEST_SUCCESSFUL_MEMORY_ALLOCATION(p);
	for (int i = 0; i < 100; ++i)
		p[i] = 0xFFFFFFFF;
	printf("int *p = Memory.malloc(sizeof(int) * 100)\n");
	PrintBlockLayout(&block_layout);
	printf("\n");

	Memory.free(p);
	printf("Memory.free(p)\n", Memory.NumBlocks());
	PrintBlockLayout(&block_layout);
	printf("\n");

	// Will split the first block allocated into 2 smaller blocks.
	int *p2 = Memory.malloc(sizeof(int) * 50);
	TEST_SUCCESSFUL_MEMORY_ALLOCATION(p2);
	for (int i = 0; i < 50; ++i)
		p2[i] = 0xFFFFFFFF;
	printf("int *p2 = Memory.malloc(sizeof(int) * 50)\n", Memory.NumBlocks());
	PrintBlockLayout(&block_layout);
	printf("\n");

	// Will merge the 2 smaller blocks into a large block of size 100*sizeof(int) bytes
	Memory.free(p2);
	printf("Memory.free(p2)\n", Memory.NumBlocks());
	PrintBlockLayout(&block_layout);
	printf("\n");

	// Will allocate a new block of size 101*sizeof(int) bytes
	int *p3 = Memory.malloc(sizeof(int) * 101);
	TEST_SUCCESSFUL_MEMORY_ALLOCATION(p3);
	for (int i = 0; i < 100; ++i)
		p3[i] = 0xFFFFFFFF;
	printf("int *p3 = Memory.malloc(sizeof(int) * 101)\n", Memory.NumBlocks());
	PrintBlockLayout(&block_layout);
	printf("\n");

	Memory.free(p3);
	printf("Memory.free(p3)\n", Memory.NumBlocks());
	PrintBlockLayout(&block_layout);
	printf("\n");

	system("pause");

	return 0;
}