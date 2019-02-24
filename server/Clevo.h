#ifndef RAYMAI_CLEVO_H
#define RAYMAI_CLEVO_H

#include <Windows.h>
#include <objbase.h>
#if defined(RAYMAI_CLEVO_USRINCLUDE)
#include RAYMAI_CLEVO_USRINCLUDE
#endif

#ifndef RAYMAI_CLEVO_MALLOC0
#define RAYMAI_CLEVO_MALLOC0(Cb)  HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (Cb))
#endif
#ifndef RAYMAI_CLEVO_MEMFREE
#define RAYMAI_CLEVO_MEMFREE(p)  HeapFree(GetProcessHeap(), 0, (p))
#endif

typedef struct IRaymaiClevo IRaymaiClevo;
struct IRaymaiClevo {
#define H__A  IRaymaiClevo const * const pClevo
#define H__B  IRaymaiClevo * const pClevo
	UINT(*GetVersion)(H__A);
	HRESULT(*Exec1)(H__A, BSTR bstrMethod, DWORD dwData);
#undef H__A
#undef H__B
};

HRESULT New_RaymaiClevo(
	IRaymaiClevo **ppClevo
);
void Del_RaymaiClevo(
	IRaymaiClevo *pClevo
);

#endif/* RAYMAI_CLEVO_H */
