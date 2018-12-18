#ifndef RAYMAI_CLEVO_H
#define RAYMAI_CLEVO_H
/*
	CLEVO Laptop Control Center Module
	----------------------------------
	Dependencies: wbemuuid.lib
*/
#include <Windows.h>

typedef struct RaymaiClevo RaymaiClevo;

HRESULT New_RaymaiClevo(RaymaiClevo **ppSelf);
void Del_RaymaiClevo(RaymaiClevo *pSelf);

typedef struct RaymaiClevo_API RaymaiClevo_API;
struct RaymaiClevo_API {
	HRESULT(*New_ErrMsg)(LPTSTR *ppsz, HRESULT hrErr);
	void(*Del_ErrMsg)(LPTSTR psz);
};

void Get_RaymaiClevo_API(RaymaiClevo_API *p);

#endif/* RAYMAI_CLEVO_H */
