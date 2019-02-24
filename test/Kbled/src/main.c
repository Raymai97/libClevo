#include <raymai/Clevo/Kbled.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SAFEFREE(p, fn) \
	if (p) { fn(p); (p) = NULL; }

static void * MemAllocZero(SIZE_T cb)
{
	return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, cb);
}

static void MemFree(void *ptr)
{
	HeapFree(GetProcessHeap(), 0, ptr);
}

enum { MaxInput = 99 };
static char s_szInput[MaxInput] = { 0 };

/*
	On success, return strlen of szInput.
	On error, return -1 for various kind of error.
*/
static int MyGetInput(void)
{
	char *pszInput = s_szInput;
	if (fgets(pszInput, MaxInput, stdin)) {
		char *pNL = strchr(pszInput, '\n');
		if (pNL) {
			*pNL = 0; return (int)(pNL - pszInput);
		}
		if (fgetc(stdin) == '\n') {
			return MaxInput - 1;
		}
		while (fgetc(stdin) != '\n');
	}
	return -1;
}

/*
	On success, return zero.
	On error, return -1 if MyGetInput return < 1,
		return -2 if contain invalid char,
		return -3 if value overflow,
		return -4 if value underflow.
*/
static int MyGetInputParseInt(int *pVal, int minVal, int maxVal)
{
	if (MyGetInput() > 0) {
		char *pBad = NULL;
		long val = strtol(s_szInput, &pBad, 0);
		if (pBad && *pBad) { return -2; }
		if (val < minVal) { return -3; }
		if (val > maxVal) { return -4; }
		*pVal = (int)val;
		return 0;
	}
	return -1;
}

static void MyMsgErr(HRESULT hrCode);

int main(void)
{
	HRESULT hr = 0;
	IRaymaiClevo *pClevo = NULL;
	IRaymaiClevoKbled *pKbled = NULL;
	hr = New_RaymaiClevo(&pClevo);
	if (FAILED(hr)) {
		fprintf(stderr, "Failed to create instance of RaymaiClevo.\n"
			"Make sure you run as administrator.\n");
		MyMsgErr(hr);
		goto eof;
	}
	hr = New_RaymaiClevoKbled(&pKbled, pClevo);
	if (FAILED(hr)) {
		MyMsgErr(hr);
		goto eof;
	}
	for (;;) {
		printf("\n"
			"--------------------------------------------------\n"
			"   CLEVO Laptop Keyboard LED Control Module Test  \n"
			"                - variant 1 [ 3 LED ] -           \n"
			"--------------------------------------------------\n"
			"[0] Set Brightness \n"
			"[1] Set Left Color \n"
			"[2] Set Middle Color \n"
			"[3] Set Right Color \n"
			"[4] Set All Color \n"
			"[Q] Quit \n"
			"\nEnter: "
		);
		if (MyGetInput() == 1) switch (s_szInput[0]) {
		case '0':
		{
			int val = 0;
			printf(">> Set LED Brightness \n");
			printf("Enter 0 ~ 255: ");
			if (MyGetInputParseInt(&val, 0x00, 0xFF) != 0) { continue; }
			hr = pKbled->SetBrightness(pKbled, (BYTE)val);
			if (FAILED(hr)) {
				MyMsgErr(hr);
			}
		}
		break;
		case '1':
		case '2':
		case '3':
		case '4':
		{
			int i = s_szInput[0] - '1', r = 0, g = 0, b = 0;
			if (i == 3) { i = -1; }
			printf(">> Set %s LED Color \n",
				i == 0 ? "Left" :
				i == 1 ? "Middle" :
				i == 2 ? "Right": "All");
			printf("Enter R of RGB (0 ~ 255): ");
			if (MyGetInputParseInt(&r, 0x00, 0xFF) != 0) { continue; }
			printf("Enter G of RGB (0 ~ 255): ");
			if (MyGetInputParseInt(&g, 0x00, 0xFF) != 0) { continue; }
			printf("Enter B of RGB (0 ~ 255): ");
			if (MyGetInputParseInt(&b, 0x00, 0xFF) != 0) { continue; }
			hr = pKbled->SetColor(pKbled, i, RGB(r, g, b));
			if (FAILED(hr)) {
				MyMsgErr(hr);
			}
		}
		break;
		case 'Q': case 'q': goto eof;
		}
	}
eof:
	SAFEFREE(pKbled, Del_RaymaiClevoKbled);
	SAFEFREE(pClevo, Del_RaymaiClevo);
	return FAILED(hr) ? 1 : 0;
}


/*
	Note that HRESULT returned by RaymaiClevo is Wbem HRESULT, so instead of
	FormatMessage(), use IWbemStatusCodeText to get error message.
*/
#include <WbemIdl.h>

static HRESULT My_New_DescWbem(LPTSTR *ppsz, HRESULT hrCode)
{
	BOOL coinit = CoInitialize(NULL);
	HRESULT hr = 0;
	IWbemStatusCodeText *pSCT = NULL;
	BSTR bstrErr = NULL;
	UINT cchErr = 0;
	LPTSTR pszErr = NULL;
	hr = CoCreateInstance(&CLSID_WbemStatusCodeText, NULL, CLSCTX_INPROC_SERVER,
		&IID_IWbemStatusCodeText, (void**)&pSCT);
	if (FAILED(hr)) goto eof;
	hr = pSCT->lpVtbl->GetErrorCodeText(pSCT, hrCode, 0, 0, &bstrErr);
	if (FAILED(hr)) goto eof;
	cchErr = SysStringLen(bstrErr);
	if (cchErr >= 2 && bstrErr[cchErr - 2] == '\r') {
		bstrErr[cchErr - 2] = 0;
	}
#if defined(UNICODE)
	pszErr = MemAllocZero((cchErr + 1) * sizeof(WCHAR));
	if (!pszErr) {
		hr = E_OUTOFMEMORY; goto eof;
	}
	CopyMemory(pszErr, bstrErr, cchErr * sizeof(WCHAR));
#else
	{
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
	SAFEFREE(bstrErr, SysFreeString);
	SAFEFREE(pSCT, pSCT->lpVtbl->Release);
	if (coinit) { CoUninitialize(); }
	return hr;
}

static void MyMsgErr(HRESULT hrCode)
{
	HRESULT hr = 0;
	LPTSTR pszDesc = NULL;
	hr = My_New_DescWbem(&pszDesc, hrCode);
	if (FAILED(hr)) {
		fprintf(stderr, "Error 0x%.8X on get error message.\n", hr);
		goto eof;
	}
	MessageBox(GetConsoleWindow(), pszDesc, NULL, MB_ICONERROR | MB_SYSTEMMODAL);
eof:
	SAFEFREE(pszDesc, MemFree);
}
