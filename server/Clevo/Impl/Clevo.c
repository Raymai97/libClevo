#include "Base.h"
#include "Clevo.h"

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
	hr = New_BSTR(&bstrNamesp, pszNamesp);
	if (FAILED(hr)) goto eof;
	hr = CoCreateInstance(&CLSID_WbemLocator, NULL, CLSCTX_INPROC_SERVER,
		&IID_IWbemLocator, (void**)&pLocator);
	if (FAILED(hr)) goto eof;
	hr = pLocator->lpVtbl->ConnectServer(pLocator, bstrNamesp, NULL, NULL, NULL,
		WBEM_FLAG_CONNECT_USE_MAX_WAIT, NULL, NULL, ppSvc);
eof:
	SafeFreeCOM(pLocator);
	SafeFree(bstrNamesp, Del_BSTR);
	return hr;
}

static HRESULT MyWbemSvcGetObject(
	IWbemServices *pSvc, LPCWSTR pszPath, IWbemClassObject **ppObj)
{
	HRESULT hr = 0;
	BSTR bstrPath = NULL;
	hr = New_BSTR(&bstrPath, pszPath);
	if (FAILED(hr)) goto eof;
	hr = pSvc->lpVtbl->GetObject(pSvc, bstrPath,
		WBEM_FLAG_RETURN_WBEM_COMPLETE, NULL, ppObj, NULL);
eof:
	SafeFree(bstrPath, Del_BSTR);
	return hr;
}

WSL_Declare(Mywsl_WQL, "WQL");

static HRESULT MyWbemSvcExecQuery(
	IWbemServices *pSvc, LPCWSTR pszQuery, IEnumWbemClassObject **ppEnum)
{
	HRESULT hr = 0;
	BSTR bstrLang = NULL;
	BSTR bstrQuery = NULL;
	hr = New_BSTR(&bstrLang, Mywsl_WQL());
	if (FAILED(hr)) goto eof;
	hr = New_BSTR(&bstrQuery, pszQuery);
	if (FAILED(hr)) goto eof;
	hr = pSvc->lpVtbl->ExecQuery(pSvc, bstrLang, bstrQuery,
		WBEM_FLAG_FORWARD_ONLY, NULL, ppEnum);
eof:
	SafeFree(bstrQuery, Del_BSTR);
	SafeFree(bstrLang, Del_BSTR);
	return hr;
}

WSL_Declare(Mywsl___PATH, "__PATH");

static HRESULT MyWbemObjGetPath(
	IWbemClassObject *pObj, BSTR *pbstrPath)
{
	VARIANT v; CIMTYPE type; LONG flavor;
	HRESULT hr = pObj->lpVtbl->Get(pObj, Mywsl___PATH(), 0, &v, &type, &flavor);
	if (SUCCEEDED(hr)) {
		*pbstrPath = v.bstrVal;
	}
	return hr;
}


typedef struct MySelf MySelf;
struct MySelf {
	IRaymaiClevo I;
	SRaymaiClevo S;
};

#define MY_0A  IRaymaiClevo const * const pIClevo
#define MY_0B  IRaymaiClevo * const pIClevo
#define MY_0AdSelf  MySelf const * const pSelf = (MySelf*)pIClevo
#define MY_0BdSelf  MySelf * const pSelf = (MySelf*)pIClevo

static UINT MyAPI_GetVersion(MY_0A);
static HRESULT MyAPI_Exec1(MY_0A, BSTR bstrMethod, DWORD dwData);

static IRaymaiClevo const s_IRaymaiClevo = {
	MyAPI_GetVersion,
	MyAPI_Exec1
};

static UINT MyAPI_GetVersion(MY_0A)
{
	UNUSED_PARAM(pIClevo);
	return 1;
}

WSL_Declare(Mywsl_Data, "Data");

static HRESULT MyAPI_Exec1(MY_0A, BSTR bstrMethod, DWORD dwData)
{
	MY_0AdSelf;
	IWbemClassObject * const pClevo = pSelf->S.pClevo;
	IWbemServices * const pSvc = pSelf->S.pSvc;
	BSTR const bstrInstance = pSelf->S.bstrInstance;
	HRESULT hr = 0;
	VARIANT vt;
	vt.vt = VT_I4;
	vt.lVal = (LONG)dwData;
	hr = pClevo->lpVtbl->Put(pClevo, Mywsl_Data(), 0, &vt, CIM_EMPTY);
	if (FAILED(hr)) goto eof;
	hr = pSvc->lpVtbl->ExecMethod(pSvc, bstrInstance,
		bstrMethod, 0L, NULL, pClevo, NULL, NULL);
eof:
	return hr;
}

WSL_Declare(Mywsl_root_WMI, "root\\WMI");
WSL_Declare(Mywsl_CLEVO_GET, "CLEVO_GET");
WSL_Declare(Mywsl_SELECT_ALL_CLEVO_GET, "SELECT * FROM CLEVO_GET");

HRESULT New_RaymaiClevo(
	IRaymaiClevo **ppClevo)
{
	HRESULT hr = 0;
	MySelf *pSelf = NULL;
	IWbemPath *pScope = NULL;
	WCHAR szNamsp[MAX_PATH] = { 0 };
	IEnumWbemClassObject *pEnum = NULL;
	IWbemClassObject *pEnumO = NULL;
	ULONG nEnumReturn = 0;

	pSelf = RAYMAI_CLEVO_MALLOC0(sizeof(*pSelf));
	if (!pSelf) { hr = E_OUTOFMEMORY; goto eof; }
	pSelf->S.coinit = SUCCEEDED(CoInitialize(NULL));

	hr = MyWbemNewPath(&pScope);
	if (FAILED(hr)) goto eof;
	hr = MyWbemPathSetPath(pScope, Mywsl_root_WMI());
	if (FAILED(hr)) goto eof;
	hr = MyWbemPathGetNamespace(pScope, szNamsp);
	if (FAILED(hr)) goto eof;

	hr = MyWbemNewSvc(&pSelf->S.pSvc, szNamsp);
	if (FAILED(hr)) goto eof;

	hr = CoSetProxyBlanket((IUnknown*)(pSelf->S.pSvc),
		RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
		RPC_C_AUTHN_LEVEL_CONNECT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
	if (FAILED(hr)) goto eof;

	hr = MyWbemSvcGetObject(pSelf->S.pSvc, Mywsl_CLEVO_GET(), &pSelf->S.pClevo);
	if (FAILED(hr)) goto eof;

	hr = MyWbemSvcExecQuery(pSelf->S.pSvc, Mywsl_SELECT_ALL_CLEVO_GET(), &pEnum);
	if (FAILED(hr)) goto eof;
	hr = pEnum->lpVtbl->Next(pEnum, WBEM_NO_WAIT, 1, &pEnumO, &nEnumReturn);
	if (FAILED(hr)) goto eof;
	if (nEnumReturn == 0) {
		hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND); goto eof;
	}
	hr = MyWbemObjGetPath(pEnumO, &pSelf->S.bstrInstance);
	if (FAILED(hr)) goto eof;

	pSelf->I = s_IRaymaiClevo;
	*ppClevo = &pSelf->I;
eof:
	return hr;
}

void Del_RaymaiClevo(
	IRaymaiClevo *pIClevo)
{
	MY_0BdSelf;
	SafeFree(pSelf->S.bstrInstance, SysFreeString);
	SafeFreeCOM(pSelf->S.pClevo);
	SafeFreeCOM(pSelf->S.pSvc);
	if (pSelf->S.coinit) { CoUninitialize(); }
	RAYMAI_CLEVO_MEMFREE(pSelf);
}
