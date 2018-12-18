#include <raymai/Clevo/Kbled.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SAFEFREE(p, fn) \
	if (p) { fn(p); (p) = NULL; }

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

typedef HRESULT(*INewErrMsg)(LPTSTR *, HRESULT);
typedef void(*IDelErrMsg)(LPTSTR);

static void MyMsgErr(HRESULT hrErr, INewErrMsg NewErrMsg, IDelErrMsg DelErrMsg)
{
	HWND const hwnd = GetConsoleWindow();
	LPCTSTR pszMsg = TEXT("FATAL: Failed to alloc error message!");
	LPTSTR pszErr = NULL;
	fprintf(stderr, "HRESULT: 0x%.8lX \n", hrErr);
	NewErrMsg(&pszErr, hrErr);
	if (pszErr) {
		pszMsg = pszErr;
	}
	MessageBox(hwnd, pszMsg, TEXT("ClevoKbled Error!"), MB_ICONERROR);
	SAFEFREE(pszErr, DelErrMsg);
}


int main(void)
{
	HRESULT hr = 0;
	RaymaiClevo_API aClevo = { 0 };
	RaymaiClevo *pClevo = NULL;
	RaymaiClevoKbled_API aKbled = { 0 };
	RaymaiClevoKbled *pKbled = NULL;
	Get_RaymaiClevo_API(&aClevo);
	Get_RaymaiClevoKbled_API(&aKbled);
	hr = New_RaymaiClevo(&pClevo);
	if (FAILED(hr)) {
		fprintf(stderr, "Failed to create instance of RaymaiClevo.\n"
			"Make sure you run as administrator.\n");
		MyMsgErr(hr, aClevo.New_ErrMsg, aClevo.Del_ErrMsg);
		goto eof;
	}
	hr = New_RaymaiClevoKbled(&pKbled, pClevo);
	if (FAILED(hr)) {
		MyMsgErr(hr, aKbled.New_ErrMsg, aKbled.Del_ErrMsg);
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
			hr = aKbled.SetBrightness(pKbled, (BYTE)val);
			if (FAILED(hr)) {
				MyMsgErr(hr, aKbled.New_ErrMsg, aKbled.Del_ErrMsg);
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
			hr = aKbled.SetColor(pKbled, i, (BYTE)r, (BYTE)g, (BYTE)b);
			if (FAILED(hr)) {
				MyMsgErr(hr, aKbled.New_ErrMsg, aKbled.Del_ErrMsg);
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
