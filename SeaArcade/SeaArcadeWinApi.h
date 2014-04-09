#ifndef SEAARCADEWINAPI
#define SEAARCADEWINAPI

#include <windows.h>
#include "SeaArcade.h"

HINSTANCE hInst;
int CreateClassAndWindow (HICON, WNDPROC, LPCWSTR, HWND &, LPCWSTR, DWORD, int, int, int, int, const HWND &);
void GetDCAndCreateCompatible(HWND &, HDC &, HDC &, HDC &);
void PaintFromMemory(HWND &, HDC &, HDC &, int, int);

void TreatMessages(MSG & msg)
{
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

LRESULT CALLBACK WndProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)	//функция главного окна
{
	
	HDC hdc;
	static HBITMAP hBkgrnd;
	static HDC memBit;
	static HDC hDCBit;
	switch (msg)
	{
	case WM_CREATE:
		hBkgrnd = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BKGRND));
		GetDCAndCreateCompatible(hWnd, hdc, memBit, hDCBit);
		//связываем HBITMAP с битмапами из ресурсов, создаем HDC и несколько совместимых с ним (для буферизации)
		RECT wndRect;
		RECT clRect;
		SelectObject (memBit, hBkgrnd);
		GetWindowRect(hWnd, &wndRect);
		GetClientRect(hWnd, &clRect);
		SetWindowPos(hWnd, NULL, wndRect.left, wndRect.top, wndRect.right-wndRect.left-clRect.right+clRect.left+FIELDWIDTH, wndRect.bottom-wndRect.top-clRect.bottom+clRect.top+FIELDHEIGHT+STATHEIGHT, NULL);
		//узнаем размеры созданного окна и клиентской части и затем задаем необходимые размеры для того, чтобы клиентская часть отображалась полностью
		break;
	case WM_CHAR:
		switch(wParam)
		{ 
		case VK_RETURN:
			StartGame(); //нажатие Enter начинает новую игру
			break;
		case VK_SPACE:
			gamePaused = !gamePaused; InvalidateRect(hWndStat, NULL, 0); //нажатие пробела ставит паузу/снимает с паузы
			break;
		case VK_ESCAPE: DestroyWindow(hWnd); //нажатие Esc закрывает окно 
		}
		break;
	case WM_PAINT:
		PaintFromMemory(hWnd, hdc, memBit, FIELDWIDTH, FIELDHEIGHT+STATHEIGHT);
		break;
	case WM_DESTROY:
		DeleteDC(memBit);
		PostQuitMessage(0);
		break;
	default: return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK FldProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	static HDC memBit;
	static HDC hDCBit;
	static HBITMAP hGameField;
	switch (msg)
	{
	case WM_CREATE:
		hHero =  LoadBitmap(hInst, MAKEINTRESOURCE(IDB_HERO));
		hMHero = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_MHERO));
		hXp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_XP));
		hMXp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_MXP));
		hEnemyL = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_ENEMYL));
		hMEnemyL = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_MENEMYL));
		hEnemyU = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_ENEMYU));
		hMEnemyU = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_MENEMYU));
		hEnemyR = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_ENEMYR));
		hMEnemyR = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_MENEMYR));
		hEnemyD = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_ENEMYD));
		hMEnemyD = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_MENEMYD));
		hLifeBonus = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_LIFEBONUS));
		hMLifeBonus = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_MLIFEBONUS));
		hXpBonus = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_XPBONUS));
		hMXpBonus = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_MXPBONUS));
		hDoubleEnemBonus = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_DOUBLEENEMBONUS));
		hMDoubleEnemBonus = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_MDOUBLEENEMBONUS));
		hGameField = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_FIELD));
		hClearField = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_FIELD));
		hGameOver = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_GAMEOVER));
		GetDCAndCreateCompatible(hWnd, hdc, memBit, hDCBit);
		//связываем HBITMAP с битмапами из ресурсов, создаем HDC и несколько совместимых с ним (для буферизации)
		SelectObject(memBit, hGameField);
		break;
	case WM_TIMER:
		GameTick (memBit, hDCBit);
		break;
	case WM_PAINT:
		PaintFromMemory(hWnd, hdc, memBit, FIELDWIDTH, FIELDHEIGHT);
		break;
	case WM_DESTROY:
		GameOver();
		DeleteDC(hDCBit);
		PostQuitMessage(0);
		break;
	default: return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK StatProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	static HBITMAP hStats;
	static HDC memBit;
	static HDC hDCBit;
	static HFONT hFont;
	switch (msg)
	{
	case WM_CREATE:
		hStats = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_STATS));
		hClearStatLine = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_STATS));
		hPause = LoadBitmap (hInst, MAKEINTRESOURCE(IDB_PAUSE));
		h1Hp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_1HP));
		h2Hp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_2HP));
		h3Hp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_3HP));
		h4Hp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_4HP));
		hFont = CreateFont(48, 0, 0, 0, 700, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_MODERN, _T("Fixedsys") );
		GetDCAndCreateCompatible(hWnd, hdc, memBit, hDCBit);
		//связываем HBITMAP с битмапами из ресурсов, создаем шрифт, создаем HDC и несколько совместимых с ним (для буферизации)
		SelectObject (memBit, hStats);
		break;
	case WM_PAINT:
		RefreshStat(memBit, hDCBit, hFont);
		PaintFromMemory(hWnd, hdc, memBit, FIELDWIDTH, STATHEIGHT);
		break;
	case WM_DESTROY:
		DeleteObject(hFont);
		DeleteDC(memBit);
		PostQuitMessage(0);
		break;
	default: return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int CreateClassAndWindow (HICON hIcon, WNDPROC wndProc, LPCWSTR className, HWND &hWindow, LPCWSTR windowName, DWORD windowStyle, int x, int y, int width, int height, const HWND &hWndParent)
{
	WNDCLASSEX wndCl;
	memset(&wndCl, 0, sizeof(wndCl));
	wndCl.cbClsExtra = 0;
	wndCl.cbSize = sizeof(WNDCLASSEX);
	wndCl.cbWndExtra = 0;
	wndCl.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);
	wndCl.hCursor = LoadCursor (NULL, IDC_ARROW);
	wndCl.hIconSm = hIcon;
	wndCl.hInstance = hInst;
	wndCl.lpfnWndProc = wndProc;
	wndCl.lpszClassName = className;
	wndCl.lpszMenuName = NULL;
	wndCl.style = CS_HREDRAW|CS_VREDRAW;
	if (!RegisterClassEx(&wndCl))
	{
		MessageBox(0, _T("Window Class was not registered!"), _T("Critical Error!"), MB_ICONSTOP|MB_OK);
		return 1;
	}
	hWindow = CreateWindowEx(NULL, className, windowName, windowStyle, x, y, width, height, hWndParent, NULL, hInst, NULL);
	return 0;
}

void GetDCAndCreateCompatible(HWND & hWnd, HDC & hdc, HDC & mem1, HDC & mem2)
{
	hdc = GetDC(hWnd);
	mem1 = CreateCompatibleDC(hdc);
	mem2 = CreateCompatibleDC(hdc);
	ReleaseDC(hWnd, hdc);
}

void PaintFromMemory(HWND & hWnd, HDC & hdc, HDC & memBit, int width, int height)
{
	PAINTSTRUCT ps;
	hdc = BeginPaint(hWnd, &ps);
	BitBlt(hdc,0,0, width, height, memBit, 0, 0, SRCCOPY);
	EndPaint(hWnd, &ps);
}

#endif