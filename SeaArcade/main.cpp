#include <windows.h>
#include <tchar.h>
#include <ctime>
#include "resource.h"
#include "SeaArcadeWinApi.h"
#include "SeaArcade.h"

int WINAPI _tWinMain (HINSTANCE hInstance, HINSTANCE prev_hInst, PWSTR cmd, int mode)
{
	srand(time(0));
	MSG msg;
	hInst = hInstance;
	if (CreateClassAndWindow(NULL, WndProc, _T("Main Class"), hWndMain, _T("Sea Arcade"), WS_DLGFRAME, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, HWND_DESKTOP) != 0) return 1;
	if (CreateClassAndWindow(NULL, FldProc, _T("FieldClass"), hWndField, _T("Field"), WS_CHILD, 0, 0, FIELDWIDTH, FIELDHEIGHT, hWndMain)) return 1;
	if (CreateClassAndWindow(NULL, StatProc, _T("StatClass"), hWndStat, _T("Stat"), WS_CHILD, 0, FIELDHEIGHT, FIELDWIDTH, STATHEIGHT, hWndMain)) return 1;
	ShowWindow (hWndMain, mode); //создается три окна: главное и два дочерних (игровое поле и строка статистики), главное окно сразу отображается
	TreatMessages(msg);
	return 0;
}