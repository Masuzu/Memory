#include "custom_malloc.h"

#include <stdio.h>
#include <malloc.h>
#include <Windows.h>

int main()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	printf("The page size for this system is %u bytes.\n", si.dwPageSize);

	int *p = (int *)Memory.malloc(sizeof(int) * 8);
	if (!p)
	{
		printf("ERROR: Out of memory\n");
		return 1;
	}

	for (int i = 0; i < 8; ++i)
	{
		p[i] = i;
		printf("%i ", p[i]);
	}
	printf("\n");

	Memory.free(p);
	int *p2 = (int *)Memory.malloc(sizeof(int) * 8);
	if (!p2)
	{
		printf("ERROR: Out of memory\n");
		return 1;
	}

	for (int i = 0; i < 8; ++i)
	{
		p2[i] = 8-i;
		printf("%i ", p2[i]);
	}
	printf("\n");
	for (int i = 0; i < 8; ++i)
		printf("%i ", p[i]);
	printf("\n");

	int *p3 = (int *)Memory.malloc(sizeof(int) * 8);
	if (!p3)
	{
		printf("ERROR: Out of memory\n");
		return 1;
	}

	for (int i = 0; i < 8; ++i)
	{
		p3[i] = i+8;
		printf("%i ", p3[i]);
	}
	printf("\n");
	for (int i = 0; i < 8; ++i)
		printf("%i ", p[i]);
	printf("\n");
	for (int i = 0; i < 8; ++i)
		printf("%i ", p2[i]);
	printf("\n");

	system("pause");

	return 0;
}