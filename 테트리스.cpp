#include <windows.h>
#include "resource.h"
#include <TCHAR.H>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
HWND hWndMain;
LPCTSTR lpszClass = _T("Tetris");

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	WndClass.style = 0;
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszClass, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	//HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));
	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}

void DrawScreen_t(HDC hdc);
void MakeNewBrick();
int GetAround(int x, int y, int b, int r);
BOOL MoveDown();
void TestFull();
void PrintTile(HDC hdc, int x, int y, int c);
void DrawBitmap_t(HDC hdc, int x, int y, HBITMAP hBit);


#define BW_T 10
#define BH_T 20
#define random(n) (rand()%n)
#define TS 24
struct Point {
	int x, y;
};
Point Shape[][4][4] = {
{ {0,0,1,0,2,0,-1,0}, {0,0,0,1,0,-1,0,-2}, {0,0,1,0,2,0,-1,0}, {0,0,0,1,0,-1,0,-2} },
{ {0,0,1,0,0,1,1,1}, {0,0,1,0,0,1,1,1}, {0,0,1,0,0,1,1,1}, {0,0,1,0,0,1,1,1} },
{ {0,0,-1,0,0,-1,1,-1}, {0,0,0,1,-1,0,-1,-1}, {0,0,-1,0,0,-1,1,-1}, {0,0,0,1,-1,0,-1,-1} },
{ {0,0,-1,-1,0,-1,1,0}, {0,0,-1,0,-1,1,0,-1}, {0,0,-1,-1,0,-1,1,0}, {0,0,-1,0,-1,1,0,-1} },
{ {0,0,-1,0,1,0,-1,-1}, {0,0,0,-1,0,1,-1,1}, {0,0,-1,0,1,0,1,1}, {0,0,0,-1,0,1,1,-1} },
{ {0,0,1,0,-1,0,1,-1}, {0,0,0,1,0,-1,-1,-1}, {0,0,1,0,-1,0,-1,1}, {0,0,0,-1,0,1,1,1} },
{ {0,0,-1,0,1,0,0,1}, {0,0,0,-1,0,1,1,0}, {0,0,-1,0,1,0,0,-1}, {0,0,-1,0,0,-1,0,1} },
{ {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0} },
{ {0,0,0,0,0,-1,1,0},{0,0,0,0,-1,0,0,-1},{0,0,0,0,0,1,-1,0},{0,0,0,0,0,1,1,0} },
};
enum { EMPTY, BRICK, WALL = sizeof(Shape) / sizeof(Shape[0]) + 1 };
int board[BW_T + 2][BH_T + 2];
int nx_t, ny_t;
int brick, rot;
int nbrick;
int score;
int bricknum;
enum tag_Status { GAMEOVER, RUNNING, PAUSE };
tag_Status GameStatus;
int Interval;
HBITMAP hBit_t[11];

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	int d;
	RECT crt;
	int trot;
	HDC hdc;
	PAINTSTRUCT ps;
	int t_x, t_y;

	switch (iMessage) {
	case WM_CREATE:
		hWndMain = hWnd;
		SetRect(&crt, 0, 0, (BW_T + 12)*TS, (BH_T + 2)*TS);
		AdjustWindowRect(&crt, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, TRUE);
		SetWindowPos(hWndMain, NULL, 0, 0, crt.right - crt.left, crt.bottom - crt.top,
			SWP_NOMOVE | SWP_NOZORDER);

		GameStatus = GAMEOVER;
		srand(GetTickCount());
		for (d = 0; d < 11; d++) {
			hBit_t[d] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP6 + d));
		}
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDM_GAME_START:
			if (GameStatus != GAMEOVER) {
				break;
			}
			for (t_x = 0; t_x < BW_T + 2; t_x++) {
				for (t_y = 0; t_y < BH_T + 2; t_y++) {
					board[t_x][t_y] = (t_y == 0 || t_y == BH_T + 1 || t_x == 0 || t_x == BW_T + 1) ? WALL : EMPTY;
				}
			}
			score = 0;
			bricknum = 0;
			GameStatus = RUNNING;
			nbrick = random(sizeof(Shape) / sizeof(Shape[0]));
			MakeNewBrick();
			Interval = 1000;
			SetTimer(hWnd, 1, Interval, NULL);
			break;
		case IDM_GAME_PAUSE:
			if (GameStatus == RUNNING) {
				GameStatus = PAUSE;
				KillTimer(hWnd, 1);
			}
			else if (GameStatus == PAUSE) {
				GameStatus = RUNNING;
				SetTimer(hWnd, 1, Interval, NULL);
			}
			break;
		case IDM_GAME_EXIT:
			DestroyWindow(hWnd);
			break;
		}
		return 0;
	case WM_TIMER:
		if (MoveDown() == TRUE) {
			MakeNewBrick();
		}
		return 0;
	case WM_KEYDOWN:
		if (GameStatus != RUNNING || brick == -1)
			return 0;
		switch (wParam) {
		case VK_LEFT:
			if (GetAround(nx_t - 1, ny_t, brick, rot) == EMPTY) {
				nx_t--;
				InvalidateRect(hWnd, NULL, FALSE);
			}
			break;
		case VK_RIGHT:
			if (GetAround(nx_t + 1, ny_t, brick, rot) == EMPTY) {
				nx_t++;
				InvalidateRect(hWnd, NULL, FALSE);
			}
			break;
		case VK_UP:
			trot = (rot == 3 ? 0 : rot + 1);
			if (GetAround(nx_t, ny_t, brick, trot) == EMPTY) {
				rot = trot;
				InvalidateRect(hWnd, NULL, FALSE);
			}
			break;
		case VK_DOWN:
			if (MoveDown() == TRUE) {
				MakeNewBrick();
			}
			break;
		case VK_SPACE:
			while (MoveDown() == FALSE) { ; }
			MakeNewBrick();
			break;
		}
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		DrawScreen_t(hdc);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		KillTimer(hWndMain, 1);
		for (d = 0; d < 11; d++) {
			DeleteObject(hBit_t[d]);
		}
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void DrawScreen_t(HDC hdc)
{
	int x, y, i;
	TCHAR str[128];

	// 테두리 그림
	for (x = 0; x < BW_T + 1; x++) {
		PrintTile(hdc, x, 0, WALL);
		PrintTile(hdc, x, BH_T + 1, WALL);
	}
	for (y = 0; y < BH_T + 2; y++) {
		PrintTile(hdc, 0, y, WALL);
		PrintTile(hdc, BW_T + 1, y, WALL);
	}

	// 게임판 그림
	for (x = 1; x < BW_T + 1; x++) {
		for (y = 1; y < BH_T + 1; y++) {
			PrintTile(hdc, x, y, board[x][y]);
		}
	}

	// 이동중인 벽돌 그림
	if (GameStatus != GAMEOVER && brick != -1) {
		for (i = 0; i < 4; i++) {
			PrintTile(hdc, nx_t + Shape[brick][rot][i].x, ny_t + Shape[brick][rot][i].y, brick + 1);
		}
	}

	// 다음 벽돌 그림
	for (x = BW_T + 3; x <= BW_T + 11; x++) {
		for (y = BH_T - 5; y <= BH_T + 1; y++) {
			if (x == BW_T + 3 || x == BW_T + 11 || y == BH_T - 5 || y == BH_T + 1) {
				PrintTile(hdc, x, y, WALL);
			}
			else {
				PrintTile(hdc, x, y, 0);
			}
		}
	}
	if (GameStatus != GAMEOVER) {
		for (i = 0; i < 4; i++) {
			PrintTile(hdc, BW_T + 7 + Shape[nbrick][0][i].x, BH_T - 2 + Shape[nbrick][0][i].y, nbrick + 1);
		}
	}

	// 정보 출력
	lstrcpy(str, _T("Tetris Ver 1.2"));
	TextOut(hdc, (BW_T + 4)*TS, 30, str, lstrlen(str));
	wsprintf(str, _T("점수 : %d   "), score);
	TextOut(hdc, (BW_T + 4)*TS, 60, str, lstrlen(str));
	wsprintf(str, _T("벽돌 : %d 개   "), bricknum);
	TextOut(hdc, (BW_T + 4)*TS, 80, str, lstrlen(str));
}

void MakeNewBrick()
{
	bricknum++;
	brick = nbrick;
	nbrick = random(sizeof(Shape) / sizeof(Shape[0]));
	nx_t = BW_T / 2;
	ny_t = 3;
	rot = 0;
	InvalidateRect(hWndMain, NULL, FALSE);
	if (GetAround(nx_t, ny_t, brick, rot) != EMPTY) {
		KillTimer(hWndMain, 1);
		GameStatus = GAMEOVER;
		MessageBox(hWndMain, _T("게임이 끝났습니다. 다시 시작하려면 게임/시작")
			_T(" 항목(S)을 선택해 주십시오."), _T("알림"), MB_OK);
	}
}

int GetAround(int x, int y, int b, int r)
{
	int i, k = EMPTY;

	for (i = 0; i < 4; i++) {
		k = max(k, board[x + Shape[b][r][i].x][y + Shape[b][r][i].y]);
	}
	return k;
}

BOOL MoveDown()
{
	if (GetAround(nx_t, ny_t + 1, brick, rot) != EMPTY) {
		TestFull();
		return TRUE;
	}
	ny_t++;
	// 즉시 그려서 벽돌이 내려가는 모양을 보여 준다.
	InvalidateRect(hWndMain, NULL, FALSE);
	UpdateWindow(hWndMain);
	return FALSE;
}

void TestFull()
{
	int i, x, y, ty;
	int count = 0;
	static int arScoreInc[] = { 0,1,3,8,20 };

	for (i = 0; i < 4; i++) {
		board[nx_t + Shape[brick][rot][i].x][ny_t + Shape[brick][rot][i].y] = brick + 1;
	}
	// 이동중인 벽돌이 잠시 없는 상태. 
	brick = -1;

	for (y = 1; y < BH_T + 1; y++) {
		for (x = 1; x < BW_T + 1; x++) {
			if (board[x][y] == EMPTY) break;
		}
		if (x == BW_T + 1) {
			count++;
			for (ty = y; ty > 1; ty--) {
				for (x = 1; x < BW_T + 1; x++) {
					board[x][ty] = board[x][ty - 1];
				}
			}
			InvalidateRect(hWndMain, NULL, FALSE);
			UpdateWindow(hWndMain);
			Sleep(150);
		}
	}
	score += arScoreInc[count];
	if (bricknum % 10 == 0 && Interval > 200) {
		Interval -= 50;
		SetTimer(hWndMain, 1, Interval, NULL);
	}
}

void DrawBitmap_t(HDC hdc, int x, int y, HBITMAP hBit)
{
	HDC MemDC;
	HBITMAP OldBitmap;
	int bx, by;
	BITMAP bit;

	MemDC = CreateCompatibleDC(hdc);
	OldBitmap = (HBITMAP)SelectObject(MemDC, hBit);

	GetObject(hBit, sizeof(BITMAP), &bit);
	bx = bit.bmWidth;
	by = bit.bmHeight;

	BitBlt(hdc, x, y, bx, by, MemDC, 0, 0, SRCCOPY);

	SelectObject(MemDC, OldBitmap);
	DeleteDC(MemDC);
}

void PrintTile(HDC hdc, int x, int y, int c)
{
	DrawBitmap_t(hdc, x*TS, y*TS, hBit_t[c]);
	return;
}