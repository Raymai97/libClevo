#include "_WbemErr.h"
#include <WbemIdl.h>
#include <WMIUtils.h>

#define MY_SafeFree  MODULE__SafeFree
#define MY_SafeFreeCOM  MODULE__SafeFreeCOM

HRESULT RaymaiClevo__New_WbemErrMsg(LPTSTR *ppsz, HRESULT hrErr)
{
	BOOL coinit = CoInitialize(NULL);
	HRESULT hr = 0;
	IWbemStatusCodeText *pSCT = NULL;
	BSTR bstrErr = NULL;
	LPTSTR pszErr = NULL;
	hr = CoCreateInstance(&CLSID_WbemStatusCodeText, NULL, CLSCTX_INPROC_SERVER,
		&IID_IWbemStatusCodeText, (void**)&pSCT);
	if (FAILED(hr)) goto eof;
	hr = pSCT->lpVtbl->GetErrorCodeText(pSCT, hrErr, 0, 0, &bstrErr);
	if (FAILED(hr)) goto eof;
#if defined(UNICODE)
	{
		UINT cbErr = SysStringByteLen(bstrErr);
		pszErr = MemAllocZero(cbErr + sizeof(WCHAR));
		if (!pszErr) {
			hr = E_OUTOFMEMORY; goto eof;
		}
		CopyMemory(pszErr, bstrErr, cbErr);
	}
#else
	{
		UINT cchErr = SysStringLen(bstrErr);
		int cbErr = WideCharToMultiByte(CP_ACP, 0, bstrErr, cchErr, NULL, 0, NULL, NULL);
		if (cbErr < 1) {
			hr = E_UNEXPECTED; goto eof;
		}
		pszErr = MemAllocZero(cbErr + sizeof(char));
		if (!pszErr) {
			hr = E_OUTOFMEMORY; goto eof;
		}
		WideCharToMultiByte(CP_ACP, 0, bstrErr, cchErr, pszErr, cbErr, NULL, NULL);
	}
#endif
	*ppsz = pszErr;
eof:
	MY_SafeFree(bstrErr, SysFreeString);
	MY_SafeFreeCOM(pSCT);
	if (coinit) { CoUninitialize(); }
	return hr;
}

void RaymaiClevo__Del_WbemErrMsg(LPTSTR psz)
{
	MemFree(psz);
}

