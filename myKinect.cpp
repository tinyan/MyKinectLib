#include <Windows.h>
#include <stdio.h>
//#include <time.h>




#include "myKinect.h"



CMyKinect::CMyKinect()
{
	m_sensor = NULL;
//	m_texture = NULL;
	m_colorHandle = NULL;
	m_depthHandle = NULL;
	m_imageSize = NULL;
	m_depthSize = NULL;

	m_deviceNumber = 0;
	m_type = -1;
	m_useSensor = -1;
	SetWaitTime();

	NuiGetSensorCount(&m_deviceNumber);
	if (m_deviceNumber > 0)
	{
		m_sensor = new INuiSensor*[m_deviceNumber];
//		m_texture = new INuiFrameTexture*[m_deviceNumber];
		m_colorHandle = new HANDLE[m_deviceNumber];
		m_depthHandle = new HANDLE[m_deviceNumber];
		m_imageSize = new SIZE[m_deviceNumber];
		m_depthSize = new SIZE[m_deviceNumber];
	}
	for (int i=0;i<m_deviceNumber;i++)
	{
		m_sensor[i] = NULL;
//		m_texture[i] = NULL;
		m_colorHandle[i] = NULL;
		m_depthHandle[i] = NULL;
		m_imageSize[i].cx = 640;
		m_imageSize[i].cy = 480;
		m_depthSize[i].cx = 320;
		m_depthSize[i].cy = 240;
	}

}

CMyKinect::~CMyKinect()
{
	End();
}

void CMyKinect::End(void)
{
	if (m_sensor != NULL)
	{
		if (m_type != -1)
		{
			UnlockAndFree();
		}

		for (int i=0;i<m_deviceNumber;i++)
		{
			//handle??


			if (m_sensor[i] != NULL)
			{
				m_sensor[i]->NuiShutdown();
				m_sensor[i] = NULL;
			}
		}

		delete [] m_sensor;
		m_sensor = NULL;
	}
}

BOOL CMyKinect::CreateSensor(int color,int depth, BOOL skelton,BOOL nearMode,int n)
{
	//color,depth:NUI_IMAGE_RESOLUTION

	if (n >= m_deviceNumber) return FALSE;
	if (m_sensor[n]) return TRUE;//already exist

	HRESULT hr = NuiCreateSensorByIndex(0, &(m_sensor[n]));
	if (SUCCEEDED(hr))
	{
		if (m_sensor[n] != NULL)
		{
			hr = m_sensor[n]->NuiStatus();
			//

		}
	}

	DWORD flag = 0;
	if (color >= 0)
	{
		flag |= NUI_INITIALIZE_FLAG_USES_COLOR;
		m_imageSize[n] = Res2Size(color);
	}
	if (depth >= 0)
	{
		flag |= NUI_INITIALIZE_FLAG_USES_DEPTH;
		m_depthSize[n] = Res2Size(depth);
	}
	if (skelton)
	{
		flag |= NUI_INITIALIZE_FLAG_USES_SKELETON;
	}

	hr = m_sensor[n]->NuiInitialize(flag);
	if (SUCCEEDED(hr))
	{

		if (color >= 0)
		{
			hr = m_sensor[n]->NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR,(NUI_IMAGE_RESOLUTION)color,0,2,NULL,&(m_colorHandle[n]));
		}

		if (depth >= 0)
		{
			hr = m_sensor[n]->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH,(NUI_IMAGE_RESOLUTION)depth,0,2,NULL,&(m_depthHandle[n]));

			DWORD flag2 = 0;
			if (nearMode)
			{
				flag2 |= NUI_IMAGE_STREAM_FLAG_ENABLE_NEAR_MODE;
				hr = m_sensor[n]->NuiImageStreamSetImageFrameFlags(m_depthHandle[n],flag2);
			}
		}

		return TRUE;
	}



	return FALSE;
}


BOOL CMyKinect::GetAndLockImage(LPVOID* lpBuffer,int* lpPitch,int n)
{
	if ((n<0) || (n>=m_deviceNumber)) return FALSE;
	if (m_sensor[n] == NULL) return FALSE;
	m_type = NUI_IMAGE_TYPE_COLOR;
	if (m_sensor[n]->NuiImageStreamGetNextFrame(m_colorHandle[n],m_waitTime,&m_imageFrame) == S_OK)
	{
		m_useSensor = n;
//		m_texture[n] = m_imageFrame.pFrameTexture;
		INuiFrameTexture* lpTexture = m_imageFrame.pFrameTexture;
		NUI_LOCKED_RECT kr;
		if (lpTexture->LockRect(0,&kr,NULL,0) == S_OK)
		{
			*lpBuffer = kr.pBits;
			*lpPitch = kr.Pitch;
			return TRUE;
		}
	}

	return FALSE;
}


BOOL CMyKinect::GetAndLockDepth(LPVOID* lpBuffer,int* lpPitch,int n)
{
	if ((n<0) || (n>=m_deviceNumber)) return FALSE;
	if (m_sensor[n] == NULL) return FALSE;
	m_type = NUI_IMAGE_TYPE_DEPTH;


	if (m_sensor[n]->NuiImageStreamGetNextFrame(m_depthHandle[n],m_waitTime,&m_imageFrame) == S_OK)
	{
		m_useSensor = n;
		INuiFrameTexture* lpTexture = m_imageFrame.pFrameTexture;
//		m_texture[n] = m_imageFrame.pFrameTexture;
		NUI_LOCKED_RECT kr;
		if (lpTexture->LockRect(0,&kr,NULL,0) == S_OK)
		{
			*lpBuffer = kr.pBits;
			*lpPitch = kr.Pitch;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CMyKinect::GetSkeltonPointer(LPVOID* lpBuffer,int n)
{
//	if (m_type != -1) return FALSE;
	if ((n<0) || (n>=m_deviceNumber)) return FALSE;
	if (m_sensor[n] == NULL) return FALSE;


	//NUI_SKELETON_FRAME nsk;

	if (m_sensor[n]->NuiSkeletonGetNextFrame(50,&m_skelton) == S_OK)
	{
		*lpBuffer = &m_skelton;
		return TRUE;
	}

	return FALSE;
}

BOOL CMyKinect::UnlockAndFree(void)
{
	if ((m_useSensor < 0) || (m_useSensor >= m_deviceNumber)) return FALSE;

	INuiFrameTexture* lpTexture = m_imageFrame.pFrameTexture;
	if (lpTexture != NULL)
	{
		lpTexture->UnlockRect(0);
	}


	if (m_type == NUI_IMAGE_TYPE_COLOR)
	{
		m_sensor[m_useSensor]->NuiImageStreamReleaseFrame(m_colorHandle[m_useSensor],&m_imageFrame);
	}

	if (m_type == NUI_IMAGE_TYPE_DEPTH)
	{
		m_sensor[m_useSensor]->NuiImageStreamReleaseFrame(m_depthHandle[m_useSensor],&m_imageFrame);
	}

	m_useSensor = -1;

	return TRUE;
}

BOOL CMyKinect::GetImageData(LPVOID buffer,int n)
{
	int* lpSrc = NULL;
	int pitch = 0;
	if (GetAndLockImage((LPVOID*)&lpSrc,&pitch,n))
	{
		int sizeX = m_imageSize[n].cx;
		int sizeY = m_imageSize[n].cy;

		int* dst = (int*)buffer;

		for (int j=0;j<sizeY;j++)
		{
			memcpy(dst,lpSrc,sizeX*sizeof(int));
			lpSrc += pitch / sizeof(int);
			dst += sizeX;
		}

		UnlockAndFree();
		return TRUE;
	}
	return FALSE;
}

BOOL CMyKinect::GetDepthData(LPVOID buffer,int n)
{
	short* lpSrc = NULL;
	int pitch = 0;
	if (GetAndLockDepth((LPVOID*)&lpSrc,&pitch,n))
	{
		int sizeX = m_depthSize[n].cx;
		int sizeY = m_depthSize[n].cy;

		short* dst = (short*)buffer;

		for (int j=0;j<sizeY;j++)
		{
			memcpy(dst,lpSrc,sizeX*sizeof(short));
			lpSrc += pitch / sizeof(short);
			dst += sizeX;
		}

		UnlockAndFree();
		return TRUE;
	}
	return FALSE;
}

BOOL CMyKinect::GetSkeltonData(LPVOID buffer,int n)
{
//	if (m_type != -1) return FALSE;
	if ((n<0) || (n>=m_deviceNumber)) return FALSE;
	if (m_sensor[n] == NULL) return FALSE;


	//NUI_SKELETON_FRAME nsk;

	if (m_sensor[n]->NuiSkeletonGetNextFrame(50,&m_skelton) == S_OK)
	{
		memcpy(buffer,&m_skelton,sizeof(NUI_SKELETON_FRAME));
		return TRUE;
	}

	return FALSE;
}

SIZE CMyKinect::Res2Size(int res)
{
	SIZE size;
	size.cx = 640;
	size.cy = 480;

	if (res == NUI_IMAGE_RESOLUTION_1280x960)
	{
		size.cx = 1280;
		size.cy = 960;
	}
	if (res == NUI_IMAGE_RESOLUTION_640x480)
	{
		size.cx = 640;
		size.cy = 480;
	}
	if (res == NUI_IMAGE_RESOLUTION_320x240)
	{
		size.cx = 320;
		size.cy = 240;
	}
	if (res == NUI_IMAGE_RESOLUTION_80x60)
	{
		size.cx = 80;
		size.cy = 60;
	}

	return size;
}
