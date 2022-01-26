//출처:https://m.blog.naver.com/PostView.nhn?blogId=lot231&logNo=140190352033&proxyReferer=https:%2F%2Fwww.google.com%2F
#include <windows.h>
#include <TCHAR.H>
#include <stdio.h>
#include <time.h>
#include <cmath>
#include <MMSystem.h>

//아이템의 랜덤화를 위한 랜덤 함수
int random(int r)
{
	srand((int)time(NULL));
	r = rand() % 3 + 1;
	return r;
}

//아이템을 떨어트릴때 하나씩 떨어지게 하기위한 구조체
struct ItemStruct
{
	bool itemstart;
	int itemx, itemy;
}Item[30];

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LPCTSTR lpszClass = _T("Block Game");
HINSTANCE g_hInst;
HWND hWndMain;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	HWND hwnd;
	MSG msg;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = lpszClass;
	RegisterClass(&WndClass);

	hwnd = CreateWindow(lpszClass,
		lpszClass,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		350,
		500,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}



LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	//dst(충돌체크), move[0]=바, move[1]=공, block(블럭의 RECT), item(아이템의렉트)
	static RECT dst, move[2], block, item;
	static RECT barL, barR;//왼쪽바,오른쪽바 변수
	static COLORREF barColor;
	//벽돌 배열 (0은 안그려줌, 1은 보통 벽돌)
	static int BackGround[4][11] = {
	0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0,
	0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0,
	0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0,
	0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0
	};
	static int i, j, r, z, count = 0;//for문에서 쓰이는 변수, 랜덤변수,랜덤대입변수, 카운트
	static int ballspeedx = 7, ballspeedy = 7;
	static int barx = 150, bary = 440;//바의 좌표 값
	static int ballx = 165, bally = 425;//볼의 좌표
	static double ball_x = -1, ball_y = -1;//볼의 토글 값
	static bool ballmove = false;//스페이스바 눌렸을때 스타트
	static TCHAR jumsu[100];
	static int sum;
	HPEN hPen, hOldpen;
	HBRUSH hBrush, hOldbrush;

	switch (message){
	case WM_CREATE:
		sndPlaySound(_T("선수입장.wav"), SND_ASYNC);
		barColor = RGB(255, 0, 0);
		SetTimer(hwnd, 1, 50, NULL);
		for (i = 0; i <= 30; i++)//처음엔 전부다 false로 잡아서 안떨어지게
		{
			Item[i].itemstart = false;
		}	
		return 0;
	case WM_TIMER:
		for (i = 0; i <= 30; i++) {
			if (Item[i].itemstart == true) //아이템 떨어지는거
			{
				Item[i].itemy += 5;
				InvalidateRect(hwnd, NULL, true);
			}
		}

		//볼 이동
		if (ballmove == true) {
			//공이동
			ballx -= ballspeedx * ball_x;
			bally += ballspeedy * ball_y;

			//벽 튕기는 부분
			if (ballx < 0)//왼쪽 벽
			{
				if (ball_x == 2)
					ball_y = -0.7;
				ball_x = -1;
			}

			if (ballx > 320)//오른쪽 벽
			{
				if (ball_x == 2)
				{
					ball_x = +0.7;
				}
				ball_x = +1;
			}
			if (bally < 0)//윗면
			{
				if (ball_y == 2)
				{
					ball_x = -0.7;
				}
				ball_y = +1;
			}
			//죽었을 때
			if (bally > 470) {
				SendMessage(hwnd, WM_DESTROY, NULL, NULL);
			}
			InvalidateRect(hwnd, NULL, true);
		}
		return 0;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_LEFT:
			//스페이스 바 누르기 전의 공이동
			if (ballmove == false) {
				ballx -= 5;
				if (ballx <= 14)
					ballx = 14;
			}
			//바 이동
			barx -= 5;
			if (barx <= 0)
				barx = 1;
			InvalidateRect(hwnd, NULL, true);
			return 0;
		case VK_RIGHT:
			//스페이스 바 누르기 전의 공이동
			if (ballmove == false){
				ballx += 5;
				if (ballx >= 310)
					ballx = 310;
			}
			//바 이동
			barx += 5;
			if (barx >= 300)
				barx = 295;
			InvalidateRect(hwnd, NULL, true);
			return 0;
			//SPACE눌렀을때 공 스타트
		case VK_SPACE:
			sndPlaySound(_T("띠디딩.wav"), SND_ASYNC);
			ballmove = true;
			return 0;
			//ESC키 눌렀을때 빠져나감
		case VK_ESCAPE:
			SendMessage(hwnd, WM_DESTROY, NULL, NULL);
			return 0;
		}
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		_stprintf_s(jumsu, "%3d", sum);
		TextOut(hdc, 0, 0, jumsu, _tcslen(jumsu));

		hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		hOldpen = (HPEN)SelectObject(hdc, hPen);
		hBrush = CreateSolidBrush(RGB(150,0,150));
		hOldbrush = (HBRUSH)SelectObject(hdc, hBrush);

		//블럭 셋 부분
		for (i = 0; i < 4; i++){
			for (j = 0; j < 11; j++){
				if (BackGround[i][j] != 0){
					SetRect(&block, j * 30, i * 20, j * 30 + 30, i * 20 + 20);
					//공과 블럭의 충돌체크
					if (IntersectRect(&dst, &move[1], &block) != 0) {
						sndPlaySound(_T("띠디딩.wav"), SND_ASYNC);
						sum += 10;
						r = random(z);
						//item충돌 체크
						if (r == 1 || r == 2 || r == 3) {
							count++;
							Item[count].itemstart = true;

							if (Item[count].itemstart != false) {
								Item[count].itemx = ballx;
								Item[count].itemy = bally;
							}
							BackGround[i][j] = 0;
							ball_y = +1;
						}
						BackGround[i][j] = 0;
						ball_y = +1;
					}
				}
			}
		}
		//블럭 그리는 부분
		for (i = 0; i < 4; i++){
			for (j = 0; j < 11; j++){
				if (BackGround[i][j] != 0){
					Rectangle(hdc, j * 30, i * 20, j * 30 + 30, i * 20 + 20);
				}
			}
		}
		//왼쪽바 셋
		SetRect(&barL, barx, bary, barx + 5, bary + 10);
		if (IntersectRect(&dst, &barL, &move[1]) != 0){
			ball_x = 2;
			ball_y = -1;
			ballspeedx = 10;
			ballspeedy = 10;
		}

		//오른쪽바 셋
		SetRect(&barR, barx + 45, bary, barx + 50, bary + 10);
		if (IntersectRect(&dst, &barR, &move[1]) != 0){
			ball_x = -2;
			ball_y = -1;
			ballspeedx = 10;
			ballspeedy = 10;
		}

		hPen = CreatePen(PS_SOLID, 1, RGB(0,0,0));
		hOldpen = (HPEN)SelectObject(hdc, hPen);
		hBrush = CreateSolidBrush(RGB(0,200,200));
		hOldbrush = (HBRUSH)SelectObject(hdc, hBrush);

		//바 셋
		SetRect(&move[0], barx + 5, bary, barx + 45, bary + 10);
		Rectangle(hdc, barx, bary, barx + 50, bary + 10);

		hPen = CreatePen(PS_SOLID, 1, RGB(0,0,0));
		hOldpen = (HPEN)SelectObject(hdc, hPen);
		hBrush = CreateSolidBrush(RGB(200,200,0));
		hOldbrush = (HBRUSH)SelectObject(hdc, hBrush);
		//공 셋
		SetRect(&move[1], ballx, bally, ballx + 15, bally + 15);
		Ellipse(hdc, ballx, bally, ballx + 15, bally + 15);

		//공과 바의 충돌체크
		if (IntersectRect(&dst, &move[0], &move[1]) != 0){
			ballspeedx = 7;
			ballspeedy = 7;
			ball_y = -1;
		}

		//item그리기
		for (i = 0; i <= 30; i++){
			if (Item[i].itemstart == true)
			{
				Rectangle(hdc, Item[i].itemx, Item[i].itemy, Item[i].itemx + 5, Item[i].itemy + 5);
			}
		}
		//바와 아이템과의 충돌체크
		for (i = 0; i <= 30; i++){
			if (Item[i].itemstart == true){
				SetRect(&item, Item[i].itemx, Item[i].itemy, Item[i].itemx + 5, Item[i].itemy + 5);
				if (IntersectRect(&dst, &move[0], &item) != 0){
					Item[i].itemstart = false;
					sum += 100;
				}
				if (IntersectRect(&dst, &barL, &item) != 0){
					Item[i].itemstart = false;
					sum += 100;
				}
				if (IntersectRect(&dst, &barR, &item) != 0){
					Item[i].itemstart = false;
					sum += 100;
				}
			}
		}
		SelectObject(hdc, hOldpen);
		SelectObject(hdc, hOldbrush);
		DeleteObject(hPen);
		DeleteObject(hBrush);

		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		KillTimer(hwnd, 1);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}