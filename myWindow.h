#if !defined __TINYAN_MYWINDOW__
#define __TINYAN_MYWINDOW__

class CMyWindow
{
public:
	CMyWindow();
	CMyWindow(HINSTANCE hInstance,int sizeX,int sizeY,LPSTR title = NULL,LPSTR className = NULL,LPVOID wndProc = NULL);
	virtual ~CMyWindow();
	void End(void);

	HWND CreateMyWindow(HINSTANCE hInstance,int sizeX,int sizeY,LPSTR title = NULL,LPSTR className = NULL,LPVOID wndProc = NULL);
	int Loop(int(*myCallBack)(int),BOOL show = TRUE,BOOL precisionMax = TRUE);

	static char m_defaultTitle[];
	static char m_defaultClassName[];

	static LRESULT CALLBACK MyMainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	HWND GetHWND(void){return m_hWnd;}
private:
	void SetTimerPrecisionMax(void);
	HWND m_hWnd;
};

#endif
/*_*/
