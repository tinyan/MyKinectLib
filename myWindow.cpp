#include <Windows.h>
#include <stdio.h>
#include <time.h>

#include "myWindow.h"


char CMyWindow::m_defaultTitle[] = "title";
char CMyWindow::m_defaultClassName[] = "testkinectclass2";

CMyWindow::CMyWindow()
{
	m_hWnd = NULL;
}

CMyWindow::CMyWindow(HINSTANCE hInstance,int sizeX,int sizeY,LPSTR title,LPSTR className ,LPVOID wndProc)
{
	CreateMyWindow(hInstance,sizeX,sizeY,title,className,wndProc);
}

CMyWindow::~CMyWindow()
{
	End();
}

void CMyWindow::End(void)
{
}

HWND CMyWindow::CreateMyWindow(HINSTANCE hInstance,int sizeX,int sizeY,LPSTR title,LPSTR className,LPVOID wndProc)
{
	WNDCLASS wc;
	if (title == NULL) title = m_defaultTitle;
	if (className == NULL) className = m_defaultClassName;
	if (wndProc == NULL) wndProc = CMyWindow::MyMainWndProc;


	ZeroMemory(&wc,sizeof(wc));
	wc.hInstance = hInstance;
	wc.lpfnWndProc = (WNDPROC)wndProc;
	wc.lpszClassName = className;
	RegisterClass(&wc);

	int windowX = 0;
	int windowY = 0;

	int frameSizeX = sizeX + 2*GetSystemMetrics(SM_CXFIXEDFRAME);
	int frameSizeY = sizeY + 2*GetSystemMetrics(SM_CYFIXEDFRAME) + GetSystemMetrics(SM_CYCAPTION);

	DWORD dwExStyle = 0;

	m_hWnd = CreateWindowEx(dwExStyle,className,title,
									(
										WS_OVERLAPPED |
										WS_CAPTION |
										WS_SYSMENU |
										WS_MINIMIZEBOX |
										WS_MAXIMIZEBOX |
										WS_POPUP |
										WS_VISIBLE |
										0
									),
									windowX,windowY,frameSizeX,frameSizeY,
									NULL,NULL,hInstance,NULL
								);

	return m_hWnd;
}

int CMyWindow::Loop(int(*myCallBack)(int) , BOOL show, BOOL precisionMax)
{
	if (precisionMax)
	{
		SetTimerPrecisionMax();
	}

	if (show)
	{
		ShowWindow(m_hWnd,SW_SHOW);
	}

	int oldTime = (int)timeGetTime();
	int oldTime0 = oldTime;

	MSG msg;
	while (1)
	{
		if (PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
		{
			if (!GetMessage(&msg,NULL,0,0))
			{
				break;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			int tm = (int)timeGetTime();
			if ((tm - oldTime) > 20)
			{
				//m_sample->MainLoop(tm-oldTime-20);
				myCallBack(tm-oldTime-20);
				oldTime += 20;
			}

			Sleep(1);
		}
	}

	return 0;
}


void CMyWindow::SetTimerPrecisionMax(void)
{
	TIMECAPS	tc ;
	timeGetDevCaps( &tc , sizeof(TIMECAPS) );

	char mes5[256];
	wsprintf(mes5,"\nGame2::\ntimer精度=%d\n\n",tc.wPeriodMin);
	OutputDebugString(mes5);


	/* マルチメディアタイマーのサービス精度を最大に */
	timeBeginPeriod( tc.wPeriodMin );
}


LRESULT CALLBACK CMyWindow::MyMainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_DESTROY)
	{
		ShowWindow(hWnd,SW_HIDE);
		PostQuitMessage(0);
		return 0;
	}

	if (message == WM_CREATE)
	{
	}

	return DefWindowProc(hWnd,message,wParam,lParam);
}

/*_*/

