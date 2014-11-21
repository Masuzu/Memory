#include "custom_malloc.h"

#include <stdio.h>
#include <malloc.h>
#include <Windows.h>

#define SUCCESSFUL_MEMORY_ALLOCATION(p) if (!(p))	{printf("ERROR: Out of memory\n"); return 1;}

int main()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	printf("The page size for this system is %u bytes.\n", si.dwPageSize);

	int *p = (int *)Memory.malloc(sizeof(int) * 100);
	SUCCESSFUL_MEMORY_ALLOCATION(p);
	for (int i = 0; i < 100; ++i)
		p[i] = 0xFFFFFFFF;

	Memory.free(p);
	// Will split the first block allocated into 2 smaller blocks.
	int *p2 = (int *)Memory.malloc(sizeof(int) * 50);
	SUCCESSFUL_MEMORY_ALLOCATION(p2);
	for (int i = 0; i < 50; ++i)
		p2[i] = 0xFFFFFFFF;

	// Will merge the 2 smaller blocks into a large block of size 100*sizeof(int) bytes
	Memory.free(p2);

	// Will allocate a new block of size 101*sizeof(int) bytes
	int *p3 = (int *)Memory.malloc(sizeof(int) * 101);
	SUCCESSFUL_MEMORY_ALLOCATION(p3);
	for (int i = 0; i < 100; ++i)
		p3[i] = 0xFFFFFFFF;

	Memory.free(p3);

	system("pause");

	return 0;
}