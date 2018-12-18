#include "_Clevo.h"
#include <WbemIdl.h>
#include <WMIUtils.h>

#define MY_SafeFree  MODULE__SafeFree
#define MY_SafeFreeCOM  MODULE__SafeFreeCOM

static HRESULT MyNewBSTR(BSTR *pbstr, LPCWSTR psz)
{
	BSTR bstr = SysAllocString(psz);
	if (bstr) { *pbstr = bstr; }
	return bstr ? S_OK : E_OUTOFMEMORY;
}

static HRESULT MyWbemNewPath(IWbemPath **ppPath)
{
	return CoCreateInstance(&CLSID_WbemDefPath, NULL, CLSCTX_INPROC_SERVER,
		&IID_IWbemPath, (void**)ppPath);
}

static HRESULT MyWbemPathSetPath(IWbemPath *pPath, LPCWSTR pszPath)
{
	return pPath->lpVtbl->SetText(pPath,
		WBEMPATH_CREATE_ACCEPT_ALL, pszPath);
}

static HRESULT MyWbemPathGetNamespace(IWbemPath *pPath, LPWSTR pszVal)
{
	ULONG cch = MAX_PATH;
	return pPath->lpVtbl->GetText(pPath,
		WBEMPATH_GET_SERVER_AND_NAMESPACE_ONLY, &cch, pszVal);
}

static HRESULT MyWbemNewSvc(IWbemServices **ppSvc, LPCWSTR pszNamesp)
{
	HRESULT hr = 0;
	BSTR bstrNamesp = NULL;
	IWbemLocator *pLocator = NULL;
	hr = MyNewBSTR(&bstrNamesp, pszNamesp);
	if (FAILED(hr)) goto eof;
	hr = CoCreateInstance(&CLSID_WbemLocator, NULL, CLSCTX_INPROC_SERVER,
		&IID_IWbemLocator, (void**)&pLocator);
	if (FAILED(hr)) goto eof;
	hr = pLocator->lpVtbl->ConnectServer(pLocator, bstrNamesp, NULL, NULL, NULL,
		WBEM_FLAG_CONNECT_USE_MAX_WAIT, NULL, NULL, ppSvc);
eof:
	MY_SafeFreeCOM(pLocator);
	MY_SafeFree(bstrNamesp, SysFreeString);
	return hr;
}

static HRESULT MyWbemSvcGetObject(
	IWbemServices *pSvc, LPCWSTR pszPath, IWbemClassObject **ppObj)
{
	HRESULT hr = 0;
	BSTR bstrPath = NULL;
	hr = MyNewBSTR(&bstrPath, pszPath);
	if (FAILED(hr)) goto eof;
	hr = pSvc->lpVtbl->GetObject(pSvc, bstrPath,
		WBEM_FLAG_RETURN_WBEM_COMPLETE, NULL, ppObj, NULL);
eof:
	MY_SafeFree(bstrPath, SysFreeString);
	return hr;
}

static HRESULT MyWbemSvcExecQuery(
	IWbemServices *pSvc, LPCWSTR pszQuery, IEnumWbemClassObject **ppEnum)
{
	HRESULT hr = 0;
	BSTR bstrLang = NULL;
	BSTR bstrQuery = NULL;
	hr = MyNewBSTR(&bstrLang, L"WQL");
	if (FAILED(hr)) goto eof;
	hr = MyNewBSTR(&bstrQuery, pszQuery);
	if (FAILED(hr)) goto eof;
	hr = pSvc->lpVtbl->ExecQuery(pSvc, bstrLang, bstrQuery,
		WBEM_FLAG_FORWARD_ONLY, NULL, ppEnum);
eof:
	MY_SafeFree(bstrQuery, SysFreeString);
	MY_SafeFree(bstrLang, SysFreeString);
	return hr;
}

/*
	Get full path to the class or instance, including server and namespace.
*/
static HRESULT MyWbemObjGetPath(
	IWbemClassObject *pObj, BSTR *pbstrPath)
{
	VARIANT v; CIMTYPE type; LONG flavor;
	HRESULT hr = pObj->lpVtbl->Get(pObj, L"__PATH", 0, &v, &type, &flavor);
	if (SUCCEEDED(hr)) {
		*pbstrPath = v.bstrVal;
	}
	return hr;
}

struct RaymaiClevo__ {
	BOOL coinit;
	IWbemServices *pSvc;
	IWbemClassObject *pClevo;
	BSTR bstrInstPath;
	BSTR bstrSetKBLED;
};
typedef RaymaiClevo__ Self;

HRESULT RaymaiClevo__New(Self **ppSelf)
{
	HRESULT hr = 0;
	Self *pSelf = NULL;
	IWbemPath *pScope = NULL;
	WCHAR szNamsp[MAX_PATH] = { 0 };
	IEnumWbemClassObject *pEnum = NULL;
	IWbemClassObject *pEnumO = NULL;
	ULONG nEnumReturn = 0;

	pSelf = MemAllocZero(sizeof(*pSelf));
	if (!pSelf) { hr = E_OUTOFMEMORY; goto eof; }
	pSelf->coinit = SUCCEEDED(CoInitialize(NULL));

	hr = MyWbemNewPath(&pScope);
	if (FAILED(hr)) goto eof;
	hr = MyWbemPathSetPath(pScope, L"root\\WMI");
	if (FAILED(hr)) goto eof;
	hr = MyWbemPathGetNamespace(pScope, szNamsp);
	if (FAILED(hr)) goto eof;

	hr = MyWbemNewSvc(&pSelf->pSvc, szNamsp);
	if (FAILED(hr)) goto eof;

	hr = CoSetProxyBlanket(((IUnknown*)pSelf->pSvc),
		RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
		RPC_C_AUTHN_LEVEL_CONNECT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
	if (FAILED(hr)) goto eof;

	hr = MyWbemSvcGetObject(pSelf->pSvc, L"CLEVO_GET", &pSelf->pClevo);
	if (FAILED(hr)) goto eof;

	hr = MyWbemSvcExecQuery(pSelf->pSvc, L"SELECT * FROM CLEVO_GET", &pEnum);
	if (FAILED(hr)) goto eof;
	hr = pEnum->lpVtbl->Next(pEnum, WBEM_NO_WAIT, 1, &pEnumO, &nEnumReturn);
	if (FAILED(hr)) goto eof;
	if (nEnumReturn == 0) {
		hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND); goto eof;
	}
	hr = MyWbemObjGetPath(pEnumO, &pSelf->bstrInstPath);
	if (FAILED(hr)) goto eof;
	hr = MyNewBSTR(&pSelf->bstrSetKBLED, L"SetKBLED");
	if (FAILED(hr)) goto eof;

	*ppSelf = pSelf;
eof:
	MY_SafeFreeCOM(pEnumO);
	MY_SafeFreeCOM(pEnum);
	MY_SafeFreeCOM(pScope);
	if (FAILED(hr)) {
		RaymaiClevo__Del(pSelf);
	}
	return hr;
}

void RaymaiClevo__Del(Self *pSelf)
{
	if (pSelf) {
		MY_SafeFree(pSelf->bstrSetKBLED, SysFreeString);
		MY_SafeFree(pSelf->bstrInstPath, SysFreeString);
		MY_SafeFreeCOM(pSelf->pClevo);
		MY_SafeFreeCOM(pSelf->pSvc);
		if (pSelf->coinit) { CoUninitialize(); }
		MemFree(pSelf);
	}
}

HRESULT RaymaiClevo__SetKBLED(Self *pSelf, DWORD dwData)
{
	HRESULT hr = 0;
	VARIANT vt;
	vt.vt = VT_I4;
	vt.lVal = (LONG)dwData;
	hr = pSelf->pClevo->lpVtbl->Put(pSelf->pClevo, L"Data", 0, &vt, CIM_EMPTY);
	if (FAILED(hr)) goto eof;
	hr = pSelf->pSvc->lpVtbl->ExecMethod(pSelf->pSvc, pSelf->bstrInstPath,
		pSelf->bstrSetKBLED, 0, NULL, pSelf->pClevo, NULL, NULL);
eof:
	return hr;
}
