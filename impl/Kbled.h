#ifndef RAYMAI_CLEVO_KBLED_H
#define RAYMAI_CLEVO_KBLED_H
/*
	CLEVO Laptop Keyboard LED Variant 1 (3 LED)
	-------------------------------------------
	Dependencies: RaymaiClevo
*/
#include <Windows.h>
#include "Clevo.h"

typedef struct RaymaiClevoKbled RaymaiClevoKbled;

HRESULT New_RaymaiClevoKbled(RaymaiClevoKbled **ppSelf, RaymaiClevo *pClevo);
void Del_RaymaiClevoKbled(RaymaiClevoKbled *pSelf);

typedef struct RaymaiClevoKbled_API RaymaiClevoKbled_API;
struct RaymaiClevoKbled_API {
	HRESULT(*New_ErrMsg)(LPTSTR *ppsz, HRESULT hrErr);
	void(*Del_ErrMsg)(LPTSTR psz);
	/*
		val: (0x00)->darkest, (0xFF)->brightest
	*/
	HRESULT(*SetBrightness)(RaymaiClevoKbled *pSelf,
		BYTE val
		);
	/*
		i: (0)->left, (1)->mid, (2)->right, (-1)->all
		r,g,b: Red, Green, Blue of RGB color value
	*/
	HRESULT(*SetColor)(RaymaiClevoKbled *pSelf,
		int i, BYTE r, BYTE g, BYTE b
		);
};

void Get_RaymaiClevoKbled_API(RaymaiClevoKbled_API *p);

#endif/* RAYMAI_CLEVO_KBLED_H */
