#include "Clevo.h"
#include "_Base.h"
#include "_Clevo.h"
#include "_WbemErr.h"

#define MY_SafeFree  MODULE__SafeFree
#define MY_SafeFreeCOM  MODULE__SafeFreeCOM

struct RaymaiClevo {
	RaymaiClevo__ *pClevo__;
};
typedef RaymaiClevo Self;

HRESULT New_RaymaiClevo(Self **ppSelf)
{
	HRESULT hr = 0;
	Self *pSelf = NULL;
	pSelf = MemAllocZero(sizeof(*pSelf));
	if (!pSelf) { hr = E_OUTOFMEMORY; goto eof; }
	hr = RaymaiClevo__New(&pSelf->pClevo__);
	if (FAILED(hr)) goto eof;
	*ppSelf = pSelf;
eof:
	if (FAILED(hr)) {
		MY_SafeFree(pSelf, Del_RaymaiClevo);
	}
	return hr;
}

void Del_RaymaiClevo(Self *pSelf)
{
	if (pSelf) {
		MY_SafeFree(pSelf->pClevo__, RaymaiClevo__Del);
		MemFree(pSelf);
	}
}

static HRESULT MyAPI_New_ErrMsg(LPTSTR *ppsz, HRESULT hrErr)
{
	return RaymaiClevo__New_WbemErrMsg(ppsz, hrErr);
}

static void MyAPI_Del_ErrMsg(LPTSTR psz)
{
	RaymaiClevo__Del_WbemErrMsg(psz);
}

void Get_RaymaiClevo_API(RaymaiClevo_API *p)
{
	p->New_ErrMsg = MyAPI_New_ErrMsg;
	p->Del_ErrMsg = MyAPI_Del_ErrMsg;
}

#include "Clevo_.h"

RaymaiClevo__ * GetRaymaiClevo__(RaymaiClevo const *pSelf)
{
	return pSelf->pClevo__;
}
