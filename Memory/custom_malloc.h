#ifndef __CUSTOM_MALLOC_H__
#define __CUSTOM_MALLOC_H__

#include <crtdefs.h>

typedef struct {
	// If the function fails, a NULL pointer is returned
	void *(*malloc)(size_t size_in_bytes);
	void(*free)(void *memory);
} namespace_Memory;
extern namespace_Memory const Memory;

#endif
