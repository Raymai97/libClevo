#include "Kbled.h"

typedef struct MySelf MySelf;
struct MySelf {
	IRaymaiClevoKbled I;
	SRaymaiClevoKbled S;
};

#define MY_0A  IRaymaiClevoKbled const * const pIKbled
#define MY_0B  IRaymaiClevoKbled * const pIKbled
#define MY_0AdSelf  MySelf const * const pSelf = (MySelf*)pIKbled
#define MY_0BdSelf  MySelf * const pSelf = (MySelf*)pIKbled

static HRESULT MyAPI_SetBrightness(MY_0B, BYTE val);
static HRESULT MyAPI_SetColor(MY_0B, int i, COLORREF cr);

static IRaymaiClevoKbled const s_IRaymaiClevoKbled = {
	MyAPI_SetBrightness,
	MyAPI_SetColor
};

static HRESULT My_SetKBLED(MySelf *pSelf, DWORD dwData)
{
	IRaymaiClevo * const pClevo = pSelf->S.pClevo;
	BSTR const bstrSetKBLED = pSelf->S.bstrSetKBLED;
	return pClevo->Exec1(pClevo, bstrSetKBLED, dwData);
}

static HRESULT MyAPI_SetBrightness(MY_0B, BYTE val)
{
	MY_0BdSelf;
	DWORD dwData = ((DWORD)0xF4) << 24;
	dwData |= val;
	return My_SetKBLED(pSelf, dwData);
}

static HRESULT My_SetColor(MySelf *pSelf, int i, COLORREF cr)
{
	BYTE const r = (cr >> 0) & 0xFF;
	BYTE const g = (cr >> 8) & 0xFF;
	BYTE const b = (cr >> 16) & 0xFF;
	DWORD dwData = ((DWORD)0xF0 + i) << 24;
	dwData |= (DWORD)b << 16;
	dwData |= (DWORD)r << 8;
	dwData |= (DWORD)g << 0;
	return My_SetKBLED(pSelf, dwData);
}

static HRESULT MyAPI_SetColor(MY_0B, int i, COLORREF cr)
{
	MY_0BdSelf;
	HRESULT hr = 0;
	if (i >= 0 && i <= 2) {
		hr = My_SetColor(pSelf, i, cr);
	}
	else if (i == -1) {
		for (i = 0; i < 3; ++i) {
			hr = My_SetColor(pSelf, i, cr);
			if (FAILED(hr)) { break; }
		}
	}
	else {
		hr = E_INVALIDARG;
	}
	return hr;
}

WSL_Declare(Mywsl_SetKBLED, "SetKBLED");

HRESULT New_RaymaiClevoKbled(
	IRaymaiClevoKbled **ppKbled,
	IRaymaiClevo *pClevo)
{
	HRESULT hr = 0;
	MySelf *pSelf = RAYMAI_CLEVO_MALLOC0(sizeof(*pSelf));
	if (!pSelf) {
		hr = E_OUTOFMEMORY; goto eof;
	}
	hr = New_BSTR(&pSelf->S.bstrSetKBLED, Mywsl_SetKBLED());
	if (FAILED(hr)) goto eof;
	pSelf->S.pClevo = pClevo;
	pSelf->I = s_IRaymaiClevoKbled;
	*ppKbled = &pSelf->I;
eof:
	if (FAILED(hr)) {
		Del_RaymaiClevoKbled(&pSelf->I);
	}
	return hr;
}

void Del_RaymaiClevoKbled(
	IRaymaiClevoKbled *pIKbled)
{
	MY_0BdSelf;
	SafeFree(pSelf->S.bstrSetKBLED, Del_BSTR);
	/* Do not free */ (pSelf->S.pClevo);
	RAYMAI_CLEVO_MEMFREE(pSelf);
}
