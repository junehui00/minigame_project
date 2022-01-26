#include <windows.h>
#include <TCHAR.H>
#include "resource.h"
#include <stdio.h>
#include <MMSystem.h>

#define MAXSTAGE 3 
#define BW 32 
#define BH 32

//소코반 
void DrawScreen(HDC hdc);
BOOL TestEnd();
void Move(int dir);
void InitStage();
void DrawBitmap(HDC hdc, int x, int y, HBITMAP hB﻿it);

char ns[18][21]; //해당 스테이지 배열
int nStage;		//스테이지 넘버
int nx, ny;		//커서 위치
int nMove;		//이동 수
HBITMAP hBit[8];	//8개의 비트맵 핸들
int ManBit;

char arStage[MAXSTAGE][18][21] = {
{
"####################",
"####################",
"####################",
"#####   ############",
"#####O  ############",
"#####  O############",
"###  O O ###########",
"### # ## ###########",
"#   # ## #####  ..##",
"# O  O   @      ..##",
"##### ### # ##  ..##",
"#####     ##########",
"####################",
"####################",
"####################",
"####################",
"####################",
"####################"
},
{
"####################",
"####################",
"####################",
"####################",
"####..  #     ######",
"####..  # O  O  ####",
"####..  #O####  ####",
"####..    @ ##  ####",
"####..  # #  O #####",
"######### ##O O ####",
"###### O  O O O ####",
"######    #     ####",
"####################",
"####################",
"####################",
"####################",
"####################",
"####################"
},
{
"####################",
"####################",
"####################",
"####################",
"##########     @####",
"########## O#O #####",
"########## O  O#####",
"###########O O #####",
"########## O # #####",
"##....  ## O  O  ###",
"###...    O  O   ###",
"##....  ############",
"####################",
"####################",
"####################",
"####################",
"####################",
"####################"
},
};
//소코반 스테이지 구조

BOOL CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
HWND hWndMain;
LPCTSTR lpszClass = _T("Mini Game");

int temp = 1;

// 함수 원형, 테트리스
void DrawScreen_t(HDC hdc);
void MakeNewBrick();
int GetAround(int x, int y, int b, int r);
BOOL MoveDown();
void TestFull();
void PrintTile(HDC hdc, int x, int y, int c);
void DrawBitmap_t(HDC hdc, int x, int y, HBITMAP hBit);

// 매크로 및 전역 변수들
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
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		300, 100, 1000, 700,
		NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	hWndMain = hWnd; // hWnd 정보도 전역변수에 저장!

	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}

BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	switch (iMessage) {
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			if (IsDlgButtonChecked(hDlg, IDC_RADIO1) == BST_CHECKED)
				temp = 1;
			if (IsDlgButtonChecked(hDlg, IDC_RADIO2) == BST_CHECKED)
				temp = 2;
			if (IsDlgButtonChecked(hDlg, IDC_RADIO3) == BST_CHECKED)
				temp = 3;
			EndDialog(hDlg, IDOK);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			DestroyWindow(hWndMain);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage,
	WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT crt;
	int i;
	TCHAR Message[256];
	int d;
	int trot;
	int t_x, t_y;
	int Answer;
	int exScore = 0;


	switch (iMessage) {
	case WM_CREATE:
		hWndMain = hWnd;
		if (temp == 1) {
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, AboutDlgProc);
			SetRect(&crt, 0, 0, 900, BH * 18);
			AdjustWindowRect(&crt, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);
			SetWindowPos(hWnd, NULL, 0, 0, crt.right - crt.left, crt.bottom - crt.top,
				SWP_NOMOVE | SWP_NOZORDER);
			for (i = 0; i < 8; i++) {
				hBit[i] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1 + i));
			}
			nStage = 0;
			InitStage();
			sndPlaySound(_T("선수입장.wav"), SND_ASYNC);
		}
		if (temp == 3) {
			SetRect(&crt, 0, 0, (BW_T + 12)*TS, (BH_T + 2)*TS);
			AdjustWindowRect(&crt, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, TRUE);
			SetWindowPos(hWndMain, NULL, 0, 0, crt.right - crt.left, crt.bottom - crt.top,
				SWP_NOMOVE | SWP_NOZORDER);

			GameStatus = GAMEOVER;
			srand(GetTickCount());
			for (d = 0; d < 11; d++) {
				hBit_t[d] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP21 + d));
			}
		}
		return 0;
	case WM_TIMER:
		if (MoveDown() == TRUE) {
			MakeNewBrick();
		}
		return 0;
	case WM_LBUTTONDOWN:
		if (temp == 2) {
			Answer = MessageBox(hWnd, _T("달팽이도 이빨이 있다."), _T("1번째 질문"), MB_YESNO);
			if (Answer == IDYES) {
				exScore++;
				Answer = MessageBox(hWnd, _T("고양이는 눈썹도 속눈썹도 없다."), _T("2번째 질문"), MB_YESNO);
				if (Answer == IDNO) {
					exScore++;
					Answer = MessageBox(hWnd, _T("손톱은 뼈가 아니고 피부가 변해서 된 것이다."), _T("3번째 질문"), MB_YESNO);
					if (Answer == IDYES) {
						exScore++;
						Answer = MessageBox(hWnd, _T("사슴은 쓸개가 없다."), _T("4번째 질문"), MB_YESNO);
						if (Answer == IDYES) {
							exScore++;
							Answer = MessageBox(hWnd, _T("금붕어는 뒤로 헤엄칠 수 있다."), _T("5번째 질문"), MB_YESNO);
							if (Answer == IDNO) {
								exScore++;
								sndPlaySound(_T("박수.wav"), SND_ASYNC);
								Answer = MessageBox(hWnd, _T("모든 문제를 맞췄습니다. (총 5점)"), _T("축하합니다!"), MB_OK);
								
							}
							else {
								sndPlaySound(_T("배경음악2.wav"), SND_ASYNC);
								MessageBox(hWnd, _T("탈락입니다. (총 4점)"), _T("아쉽네요!"), MB_OK);
								DestroyWindow(hWnd);
							}
						}
						else {
							sndPlaySound(_T("배경음악2.wav"), SND_ASYNC);
							MessageBox(hWnd, _T("탈락입니다. (총 3점)"), _T("아쉽네요!"), MB_OK);
							DestroyWindow(hWnd);
						}
					}
					else {
						sndPlaySound(_T("배경음악2.wav"), SND_ASYNC);
						MessageBox(hWnd, _T("탈락입니다. (총 2점)"), _T("아쉽네요!"), MB_OK);
						DestroyWindow(hWnd);
					}
				}
				else {
					sndPlaySound(_T("배경음악2.wav"), SND_ASYNC);
					MessageBox(hWnd, _T("탈락입니다. (총 1점)"), _T("아쉽네요!"), MB_OK);
					DestroyWindow(hWnd);
				}
			}
			else {
				sndPlaySound(_T("배경음악2.wav"), SND_ASYNC);
				MessageBox(hWnd, _T("탈락입니다. (총 0점)"), _T("아쉽네요!"), MB_OK);
				DestroyWindow(hWnd);
			}
		}
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		if (temp == 1) {
			DrawScreen(hdc);
		}
		else if (temp == 2) {
			TextOut(hdc, 40, 40, _T("왼쪽 버튼을 누르세요!"), 21);
		}
		else if (temp == 3) {
			DrawScreen_t(hdc);
		}
		EndPaint(hWnd, &ps);
		return 0;
	case WM_KEYDOWN:
		if (temp == 1) {
			switch (wParam) {
			case VK_LEFT:
			case VK_RIGHT:
			case VK_UP:
			case VK_DOWN:
				Move(wParam); //방향키 입력이 들어오면 move함수로 처리.
				if (TestEnd()) {
					wsprintf(Message, _T("%d 스테이지를 풀었습니다. ")
						_T("다음 스테이지로 이동합니다"), nStage + 1);
					MessageBox(hWnd, Message, _T("알림"), MB_OK);
					if (nStage < MAXSTAGE - 1) {
						nStage++;
					}
					InitStage();
				}
				sndPlaySound(_T("효과음6.wav"), SND_ASYNC);
				break;
			case 'Q':
				DestroyWindow(hWnd);
				break;
			case 'R':
				InitStage();
				break;
			case 'N':
				if (nStage < MAXSTAGE - 1) {
					nStage++;
					InitStage();
				}
				break;
			case 'P':
				if (nStage > 0) {
					nStage--;
					InitStage();
				}
				break;
			}
		}
		if (temp == 3) {
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
				sndPlaySound(_T("효과음5.wav"), SND_ASYNC);
				MakeNewBrick();
				break;
			}
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
			SetTimer(hWnd, 2, Interval, NULL);
			break;
		case IDM_GAME_PAUSE:
			if (GameStatus == RUNNING) {
				GameStatus = PAUSE;
				KillTimer(hWnd, 2);
			}
			else if (GameStatus == PAUSE) {
				GameStatus = RUNNING;
				SetTimer(hWnd, 2, Interval, NULL);
			}
			break;
		case IDM_GAME_EXIT:
			DestroyWindow(hWnd);
			break;
		}
		return 0;
	case WM_DESTROY:
		if (temp == 1) {
			for (i = 0; i < 8; i++) {
				DeleteObject(hBit[i]);  //비트맵 5개 메모리 반환!!
			}
		}
		if (temp == 3) {
			KillTimer(hWndMain, 2);
			for (d = 0; d < 11; d++) {
				DeleteObject(hBit[d]);
			}
		}
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}


void DrawScreen(HDC hdc)	//스크린을 그리는 함수
{
	int x, y;
	int iBit;
	TCHAR Message[256];

	for (y = 0; y < 18; y++) {
		for (x = 0; x < 20; x++) {
			switch (ns[y][x]) {
			case '#':
				iBit = 0;
				break;
			case 'O':
				iBit = 1;
				break;
			case '.':
				iBit = 2;
				break;
			case ' ':
				iBit = 3;
				break;
			}
			DrawBitmap(hdc, x*BW, y*BH, hBit[iBit]);	//스테이지 전체 배열을 돌아가며 문자를 비트맵으로 치환하는 작업이다.
		}
	}
	DrawBitmap(hdc, nx*BW, ny*BH, hBit[ManBit]);	//마지막으로 캐릭터(hBit[4])를 그린다. nx*BW,ny*BH : 캐릭터 위치

	wsprintf(Message, _T("SOKOBAN"));
	TextOut(hdc, 700, 10, Message, lstrlen(Message));
	wsprintf(Message, _T("Q:종료, R:다시 시작"));
	TextOut(hdc, 700, 30, Message, lstrlen(Message));
	wsprintf(Message, _T("N:다음, P:이전"));
	TextOut(hdc, 700, 50, Message, lstrlen(Message));
	wsprintf(Message, _T("스테이지 : %d"), nStage + 1);
	TextOut(hdc, 700, 70, Message, lstrlen(Message));
	wsprintf(Message, _T("이동 회수 : %d"), nMove);
	TextOut(hdc, 700, 90, Message, lstrlen(Message));	//안내문구
}

BOOL TestEnd()//클리어 조건을 참,거짓으로 반환한다.
{
	int x, y;
	//원본스테이지 창고 부분과, 조작한 스테이지의 선물이 하나라도 일치하지 않는다면 FALSE반환
	for (y = 0; y < 18; y++) {
		for (x = 0; x < 20; x++) {
			if (arStage[nStage][y][x] == '.' && ns[y][x] != 'O') {
				return FALSE;
			}
		}
	}
	return TRUE;	//만족하면 참을 반환
}

void Move(int dir) {
	int dx = 0, dy = 0;
	switch (dir) {
	case VK_LEFT:
		ManBit = 4;
		dx = -1;
		break;
	case VK_RIGHT:
		ManBit = 5;
		dx = 1;
		break;
	case VK_UP:
		ManBit = 6;
		dy = -1;
		break;
	case VK_DOWN:
		ManBit = 7;
		dy = 1;
		break;
	}

	if (ns[ny + dy][nx + dx] != '#') {	//이동예상위치가 벽이 아니고
		if (ns[ny + dy][nx + dx] == 'O') {	//이동예상위치가 상자이고
			if (ns[ny + dy * 2][nx + dx * 2] == ' ' || ns[ny + dy * 2][nx + dx * 2] == '.') {
				//이동예상위치의 한칸앞이 빈곳이거나, 창고이며
				if (arStage[nStage][ny + dy][nx + dx] == '.') {	//이동예상위치가 원본스테이지의 창고이면
					ns[ny + dy][nx + dx] = '.';	//게임 내 예상위치는 창고가 된다
				}
				else {
					ns[ny + dy][nx + dx] = ' ';	//원본스테이지의 창고가 아니라면 게임 내 빈공간된다
				}
				ns[ny + dy * 2][nx + dx * 2] = 'O';	//이동예상위치의 한칸앞이 빈곳이거나, 창고면 게임내 상자의 위치가 된다.
			}
			else {
				return;//나머지는 전부 무시.
			}
		}
		nx += dx;
		ny += dy;	//캐릭터 위치를 갱신
		nMove++;	//이동 횟수 증가
		InvalidateRect(hWndMain, NULL, FALSE);	//이동한 경우, 스테이지 다시 그림
	}
}
//스테이지 초기화 함수
void InitStage(){
	int x, y;
	ManBit = 4;
	memcpy(ns, arStage[nStage], sizeof(ns));	//원본스테이지를 ns배열로 복사
	for (y = 0; y < 18; y++) {
		for (x = 0; x < 20; x++) {
			if (ns[y][x] == '@') {
				nx = x;
				ny = y;	//캐릭터 현재위치정보를 nx,ny에 대입한다.
				ns[y][x] = ' ';	//캐릭터 위치 nx,ny는 DrawScreen으로 보내고  빈공간으로 바꾸어 버림
								//즉, 캐릭터 초기 위치만 보내주고 나머지는 스테이지로 바꾸어 버림
			}
		}
	}
	nMove = 0;
	InvalidateRect(hWndMain, NULL, TRUE);//무효화시켜서 다시 그림
}

void DrawBitmap(HDC hdc, int x, int y, HBITMAP hBit)	//비트맵치환 함수
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
//테트리스 함수들
void DrawScreen_t(HDC hdc) {
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
	if (GetAround(nx_t, ny_t, brick, rot) != EMPTY) { //맨위에 닿으면.
		KillTimer(hWndMain, 2);
		GameStatus = GAMEOVER;
		MessageBox(hWndMain, _T("게임이 끝났습니다. 다시 시작하려면 게임/시작")
			_T(" 을 선택해 주십시오."), _T("알림"), MB_OK);
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
//바닥이 다 찼는지 확인하는 함수
void TestFull() {
	int i, x, y, ty;
	int count = 0;
	static int arScoreInc[] = { 0,1,3,8,20 };
	for (i = 0; i < 4; i++) {
		board[nx_t + Shape[brick][rot][i].x][ny_t + Shape[brick][rot][i].y] = brick + 1;
	} 
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
	sndPlaySound(_T("효과음2.wav"), SND_ASYNC);
	if (bricknum % 10 == 0 && Interval > 200) {
		Interval -= 50;
		SetTimer(hWndMain, 2, Interval, NULL);
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
