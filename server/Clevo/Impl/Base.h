#ifndef RAYMAI_CLEVO_IMPL_BASE_H
#define RAYMAI_CLEVO_IMPL_BASE_H

#include "../../Clevo.h"
#include <WbemIdl.h>
#include <WMIUtils.h>

#define WSL_Declare(Name, Val) \
	static LPCWSTR Name(void) { \
		static WCHAR sz[sizeof(Val)]; \
		if (sz[sizeof(Val) - 1] == 0) { \
			WCHAR *p = sz; char const *r = Val; \
			for (; *p = *r, *r; ++p, ++r); \
		} return sz; \
	}

#define UNUSED_PARAM(a)  (a)
#define SafeFree(p, fn)  if (p) { fn(p); (p) = NULL; }
#define SafeFreeCOM(p)  SafeFree((p), (p)->lpVtbl->Release)

static HRESULT New_BSTR(BSTR *pbstr, LPCWSTR psz)
{
	BSTR bstr = SysAllocString(psz);
	if (bstr) { *pbstr = bstr; }
	return bstr ? S_OK : E_OUTOFMEMORY;
}

static void Del_BSTR(BSTR bstr)
{
	SysFreeString(bstr);
}

#endif/* RAYMAI_CLEVO_IMPL_BASE_H */
