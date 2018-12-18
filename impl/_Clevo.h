#ifndef RAYMAI_CLEVO__CLEVO_H
#define RAYMAI_CLEVO__CLEVO_H

#include "_Base.h"

typedef struct RaymaiClevo__ RaymaiClevo__;

HRESULT RaymaiClevo__New(RaymaiClevo__ **ppSelf);
void RaymaiClevo__Del(RaymaiClevo__ *pSelf);

HRESULT RaymaiClevo__SetKBLED(RaymaiClevo__ *pSelf, DWORD dwData);

#endif/* RAYMAI_CLEVO__CLEVO_H */
