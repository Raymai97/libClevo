#ifndef RAYMAI_CLEVO_IMPL_KBLED_H
#define RAYMAI_CLEVO_IMPL_KBLED_H

#include "Base.h"
#include "../Kbled.h"

typedef struct SRaymaiClevoKbled SRaymaiClevoKbled;
struct SRaymaiClevoKbled {
	IRaymaiClevo *pClevo;
	BSTR bstrSetKBLED;
};

#endif/* RAYMAI_CLEVO_IMPL_KBLED_H */
