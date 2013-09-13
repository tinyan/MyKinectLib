#include <Windows.h>
#include <stdio.h>
#include <time.h>

#include <d3dx9.h>

#include "mySimpleDirect3D.h"

CMySimpleDirect3D::CMySimpleDirect3D(HWND hwnd,int screenSizeX,int screenSizeY)
{
	m_D3D = NULL;
	m_D3DDevice = NULL;
	m_surface = NULL;
	m_lockFlag = FALSE;


	m_hWnd = hwnd;
	m_screenSize.cx = screenSizeX;
	m_screenSize.cy = screenSizeY;

	MyCreate();

}

CMySimpleDirect3D::~CMySimpleDirect3D()
{
	End();
}

void CMySimpleDirect3D::End(void)
{
	if (m_D3DDevice != NULL)
	{
		m_D3DDevice->Release();
		m_D3DDevice = NULL;
	}

	if (m_D3D != NULL)
	{
		m_D3D->Release();
		m_D3D = NULL;
	}

}

void CMySimpleDirect3D::MyCreate(void)
{
	m_D3D = Direct3DCreate9(D3D_SDK_VERSION);
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp,sizeof(d3dpp));
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.Windowed = TRUE;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;


	HRESULT hr;
	DWORD vertexProcessing = D3DCREATE_HARDWARE_VERTEXPROCESSING;

	if (m_D3DDevice == NULL)
	{
		hr = m_D3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,m_hWnd,vertexProcessing,&d3dpp,&m_D3DDevice);
		if (FAILED(hr))
		{
			BOOL flg = FALSE;

			for (int i=0;i<100;i++)
			{
				Sleep(100);
				if (hr == D3DERR_DEVICELOST)
				{
					hr = m_D3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,m_hWnd,vertexProcessing,&d3dpp,&m_D3DDevice);
					if (!FAILED(hr))
					{
						flg = TRUE;
						break;
					}
				}
				else
				{
					break;
				}
			}

			if (flg == FALSE)
			{
//				DXTRACE_ERR("CreateDevice",hr);
				OutputDebugString("\nError:CreateDevice\n");
				
//				MessageBox(NULL,DXGetErrorString9(hr),DXGetErrorDescription9(hr),MB_OK);
				return;
			}
		}
	}

//	m_D3DDevice->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);





	D3DVIEWPORT9 vp;
	vp.X = 0;
	vp.Y = 0;
	vp.Width = m_screenSize.cx;
	vp.Height = m_screenSize.cy;

	vp.MinZ = 0.0f;
	vp.MaxZ = 1.0f;

	hr = m_D3DDevice->SetViewport(&vp);
	if (FAILED(hr))
	{
		OutputDebugString("\nError:SetViewPort\n");
		//return;
	}

	m_D3DDevice->Clear(0,NULL,D3DCLEAR_TARGET,D3DCOLOR(rand() % 256),0.0f,0);

}

BOOL CMySimpleDirect3D::LockBuffer(LPVOID* lpBuffer,int* lpPitch)
{
	if (m_surface != NULL) return FALSE;

	if (m_D3DDevice->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO ,&m_surface) == D3D_OK)
	{
		D3DLOCKED_RECT lr;
		if (m_surface->LockRect(&lr,NULL,D3DLOCK_NOSYSLOCK) == D3D_OK)
		{
			m_lockFlag = TRUE;
			m_buffer = lr.pBits;
			m_pitch = lr.Pitch;
			*lpBuffer = m_buffer;
			*lpPitch = m_pitch;

			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

BOOL CMySimpleDirect3D::UnlockBuffer(void)
{
	if (m_surface == NULL) return FALSE;
	if (m_lockFlag)
	{
		m_surface->UnlockRect();
		m_lockFlag = FALSE;
	}
	m_surface->Release();
	m_surface = NULL;
	return TRUE;
}

BOOL CMySimpleDirect3D::Clear(int color,float depth)
{
	if (m_D3DDevice == NULL) return FALSE;

	DWORD sentinel = 0;
	m_D3DDevice->Clear(0,NULL,D3DCLEAR_TARGET,D3DCOLOR(color),depth,sentinel);

	return TRUE;
}

BOOL CMySimpleDirect3D::Present(void)
{
	if (m_D3DDevice == NULL) return FALSE;

	m_D3DDevice->Present(NULL, NULL, NULL, NULL);

	return TRUE;
}
