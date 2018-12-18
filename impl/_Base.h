#ifndef RAYMAI_CLEVO__BASE_H
#define RAYMAI_CLEVO__BASE_H

#include <Windows.h>

#define MODULE__SafeFreeCOM(p)  \
	if (p) { (p)->lpVtbl->Release(p); (p) = NULL; }

#define MODULE__SafeFree(p,fn) \
	if (p) { fn(p); (p) = NULL; }

static void * MemAllocZero(SIZE_T cb)
{
	return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, cb);
}

static void MemFree(void *ptr)
{
	HeapFree(GetProcessHeap(), 0, ptr);
}

#endif/* RAYMAI_CLEVO__BASE_H */
