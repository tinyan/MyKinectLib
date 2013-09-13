#if !defined __TINYAN_MYSIMPLEDIRECT3D__
#define __TINYAN_MYSIMPLEDIRECT3D__

#include <d3dx9.h>

class CMySimpleDirect3D
{
public:
	CMySimpleDirect3D(HWND hwnd,int screenSizeX,int screenSizeY);
	virtual ~CMySimpleDirect3D();
	void End(void);

	BOOL LockBuffer(LPVOID* lpBuffer,int* lpPitch);
	BOOL UnlockBuffer(void);
	BOOL Clear(int color,float depth = 0.0f);
	BOOL Present(void);

private:
	LPDIRECT3D9 m_D3D;
	LPDIRECT3DDEVICE9 m_D3DDevice;
	HWND m_hWnd;
	SIZE m_screenSize;

	LPDIRECT3DSURFACE9 m_surface;
	BOOL m_lockFlag;
	LPVOID m_buffer;
	int m_pitch;

	void MyCreate(void);
};


#endif
/*_*/
