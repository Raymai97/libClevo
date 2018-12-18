#include "Kbled.h"
#include "_Base.h"
#include "_Clevo.h"
#include "_WbemErr.h"
#include "Clevo_.h"

#define MY_SafeFreeCOM  MODULE__SafeFreeCOM
#define MY_SafeFree  MODULE__SafeFree

struct RaymaiClevoKbled {
	RaymaiClevo *pClevo; /* User provide, don't delete! */
};
typedef RaymaiClevoKbled Self;

HRESULT New_RaymaiClevoKbled(Self **ppSelf, RaymaiClevo *pClevo)
{
	HRESULT hr = 0;
	Self *pSelf = NULL;
	pSelf = MemAllocZero(sizeof(*pSelf));
	if (!pSelf) { hr = E_OUTOFMEMORY; goto eof; }
	pSelf->pClevo = pClevo;
	*ppSelf = pSelf;
eof:
	if (FAILED(hr)) {
		MY_SafeFree(pSelf, Del_RaymaiClevoKbled);
	}
	return hr;
}

void Del_RaymaiClevoKbled(Self *pSelf)
{
	if (pSelf) {
		MemFree(pSelf);
	}
}

static HRESULT My_SetKBLED(
	Self const *pSelf, DWORD dwData)
{
	return RaymaiClevo__SetKBLED(GetRaymaiClevo__(pSelf->pClevo), dwData);
}

static HRESULT MyAPI_SetBrightness(
	Self *pSelf, BYTE val)
{
	return My_SetKBLED(pSelf, (0xF4 << 24) | val);
}

static HRESULT My_SetColor(
	Self const *pSelf, int i, BYTE r, BYTE g, BYTE b)
{
	DWORD dw = ((0xF0 + i) << 24);
	dw += (DWORD)b << 16;
	dw += (DWORD)r << 8;
	dw += (DWORD)g << 0;
	return My_SetKBLED(pSelf, dw);
}

static HRESULT MyAPI_SetColor(
	Self *pSelf, int i, BYTE r, BYTE g, BYTE b)
{
	HRESULT hr = 0;
	if (i >= 0 && i <= 2) {
		hr = My_SetColor(pSelf, i, r, g, b);
	}
	else if (i == -1) {
		for (i = 0; SUCCEEDED(hr) && i < 3; ++i) {
			hr = My_SetColor(pSelf, i, r, g, b);
		}
	}
	else {
		hr = E_INVALIDARG;
	}
	return hr;
}

static HRESULT MyAPI_New_ErrMsg(
	LPTSTR *ppsz, HRESULT hrErr)
{
	return RaymaiClevo__New_WbemErrMsg(ppsz, hrErr);
}

static void MyAPI_Del_ErrMsg(
	LPTSTR psz)
{
	RaymaiClevo__Del_WbemErrMsg(psz);
}

void Get_RaymaiClevoKbled_API(RaymaiClevoKbled_API *p)
{
	p->New_ErrMsg = MyAPI_New_ErrMsg;
	p->Del_ErrMsg = MyAPI_Del_ErrMsg;
	p->SetBrightness = MyAPI_SetBrightness;
	p->SetColor = MyAPI_SetColor;
}
