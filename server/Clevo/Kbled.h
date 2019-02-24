#ifndef RAYMAI_CLEVO_KBLED_H
#define RAYMAI_CLEVO_KBLED_H

#include "../Clevo.h"

typedef struct IRaymaiClevoKbled IRaymaiClevoKbled;
struct IRaymaiClevoKbled {
#define H__A  IRaymaiClevoKbled const * const pKbled
#define H__B  IRaymaiClevoKbled * const pKbled
	HRESULT(*SetBrightness)(H__B, BYTE val);
	HRESULT(*SetColor)(H__B, int i, COLORREF cr);
#undef H__A
#undef H__B
};

HRESULT New_RaymaiClevoKbled(
	IRaymaiClevoKbled **ppKbled,
	IRaymaiClevo *pClevo
);
void Del_RaymaiClevoKbled(
	IRaymaiClevoKbled *pKbled
);

#endif/* RAYMAI_CLEVO_KBLED_H */
