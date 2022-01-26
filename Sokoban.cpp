#include <windows.h>
#include <TCHAR.H>
#include "resource.h"

#define MAXSTAGE 3 
#define BW 32 
#define BH 32


void DrawScreen(HDC hdc);
BOOL TestEnd();
void Move(int dir);
void InitStage();
void DrawBitmap(HDC hdc, int x, int y, HBITMAP hB﻿it);


char ns[18][21]; //해당 스테이지 배열
int nStage;		//스테이지 넘버
int nx, ny;		//커서 위치
int nMove;		//이동 수
HBITMAP hBit[5];	//5개의 비트맵 핸들

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
//스테이지 구조

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
HWND hWndMain;
LPCTSTR lpszClass = _T("Sokoban");

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
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszClass, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc; 
	PAINTSTRUCT ps; 
	RECT crt;
	int i;
	TCHAR Message[256];

	switch (iMessage) {
	case WM_CREATE:
		hWndMain = hWnd;
		SetRect(&crt, 0, 0, 900, BH * 18);//작업영역 크기조정
		//왜 BH*18이냐면 비트맵(34,34)짜리를 18개 딱 채워 넣을것이기 때문이다.
		AdjustWindowRect(&crt, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);
		//crt구조체를 위에 설정한 작업영역이 나올 수 있도록
		//크기와 스타일을 조정하고 다시 크기를 구조체에 반환시켜줌
		SetWindowPos(hWnd, NULL, 0, 0, crt.right - crt.left, crt.bottom - crt.top,
			SWP_NOMOVE | SWP_NOZORDER);
		//윈도우 위치조정
		for (i = 0; i < 5; i++) {
			hBit[i] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1 + i));
		}//비트맵을 리소스에서 차례대로 읽어오기
		nStage = 0; //처음 만들 때 스테이지는 1번(0으로 표현)
		InitStage();//스테이지 초기화 함수
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		DrawScreen(hdc);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_KEYDOWN:
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
		return 0;
		//단축키설정이다. Q는 종료, R은 스테이지 초기화,N은 다음스테이지), P는 이전스테이지
	case WM_DESTROY:
		for (i = 0; i < 5; i++) {
			DeleteObject(hBit[i]);  //비트맵 5개 메모리 반환!!
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
	DrawBitmap(hdc, nx*BW, ny*BH, hBit[4]);	//마지막으로 캐릭터(hBit[4])를 그린다. nx*BW,ny*BH : 캐릭터 위치

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

void Move(int dir)	//키입력이 들어오면 처리하는 함수
{
	int dx = 0, dy = 0;	//dx와dy는 매 입력마다 초기화 시킨다.

	switch (dir) {
	case VK_LEFT:
		dx = -1;
		break;
	case VK_RIGHT:
		dx = 1;
		break;
	case VK_UP:
		dy = -1;
		break;
	case VK_DOWN:
		dy = 1;
		break;
	}

	if (ns[ny + dy][nx + dx] != '#') {	//이동예상위치가 벽이 아니고
		if (ns[ny + dy][nx + dx] == 'O') {	//이동예상위치가 공이고
			if (ns[ny + dy * 2][nx + dx * 2] == ' ' || ns[ny + dy * 2][nx + dx * 2] == '.') {	//이동예상위치의 한칸앞이 텅빈 곳이거나, 창고며
				if (arStage[nStage][ny + dy][nx + dx] == '.') {	//이동예상위치가 원본스테이지의 창고이면
					ns[ny + dy][nx + dx] = '.';	//게임 내 예상위치는 창고가 된다
				}
				else {
					ns[ny + dy][nx + dx] = ' ';	//원본스테이지의 창고가 아니라면 게임 내 빈공간된다
				}
				ns[ny + dy * 2][nx + dx * 2] = 'O';	//이동예상위치의 한칸앞이 텅빈 곳이거나, 창고면 게임내 공의 위치가 된다.
			}
			else {
				return;//나머지는 전부 무시.
			}
		}
		//[ny+dy][nx+dx]는 현재캐릭터 위치에 방향키를 입력받고 난 후의 이동예상 지점을 나타낸다.
		nx += dx;
		ny += dy;	//캐릭터 현 위치를 갱신시킨다.
		nMove++;	//이동 회수를 증가시킴
		InvalidateRect(hWndMain, NULL, FALSE);	//이동(이 되었을 때!)한 경우는 무조건 WM_PAINT메세지로 스테이지를 전부 다시 그린다.
	}
}

void InitStage()	//스테이지 초기화 함수
{
	int x, y;

	memcpy(ns, arStage[nStage], sizeof(ns));	//원본스테이지를 우리가 쓸 ns배열로 메모리복사한다.
	for (y = 0; y < 18; y++) {
		for (x = 0; x < 20; x++) {
			if (ns[y][x] == '@') {
				nx = x;
				ny = y;	//캐릭터 현재위치정보를 nx,ny에 대입한다.
				ns[y][x] = ' ';	//일단 캐릭터 위치 nx,ny는 DrawScreen으로 보내고 여기서는 그냥 빈공간으로 바꾸어 버린다. 
				//즉 캐릭터 초기 위치만 보내주고 나머지는 스테이지로 바꾸어 버린다
			}
		}
	}
	nMove = 0;
	InvalidateRect(hWndMain, NULL, TRUE);//무효화시켜서 다시 그린다.
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