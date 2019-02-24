#ifndef RAYMAI_CLEVO_IMPL_CLEVO_H
#define RAYMAI_CLEVO_IMPL_CLEVO_H

#include "Base.h"
#include "../../Clevo.h"

typedef struct SRaymaiClevo SRaymaiClevo;
struct SRaymaiClevo {
	BOOL coinit;
	IWbemServices *pSvc;
	IWbemClassObject *pClevo;
	BSTR bstrInstance;
};

#endif/* RAYMAI_CLEVO_IMPL_CLEVO_H */
