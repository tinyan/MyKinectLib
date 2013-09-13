#if !defined __TINYAN_MYKINECT__
#define __TINYAN_MYKINECT__

#include <NuiApi.h>
class CMyKinect
{
public:
	CMyKinect();
	virtual ~CMyKinect();
	void End(void);

	BOOL CreateSensor(int color,int depth = -1, BOOL skelton = FALSE,BOOL nearMode = FALSE,int n = 0);

	BOOL GetAndLockImage(LPVOID* lpBuffer,int* lpPitch,int n = 0);
	BOOL GetAndLockDepth(LPVOID* lpBuffer,int* lpPitch,int n = 0);
	BOOL UnlockAndFree(void);
	BOOL GetImageData(LPVOID buffer,int n=0);
	BOOL GetDepthData(LPVOID buffer,int n=0);

	BOOL GetSkeltonPointer(LPVOID* lpBuffer,int n = 0);
	BOOL GetSkeltonData(LPVOID buffer,int n = 0);
//	BOOL UnlockSkelton(int n=0);
	void SetWaitTime(int millSecond = 100){m_waitTime = millSecond;}
private:
	INuiSensor** m_sensor;
//	INuiFrameTexture** m_texture;
	HANDLE* m_colorHandle;
	HANDLE* m_depthHandle;
	SIZE* m_imageSize;
	SIZE* m_depthSize;

	int m_deviceNumber;
	int m_useSensor;
	int m_type;

	NUI_SKELETON_FRAME m_skelton;
	NUI_IMAGE_FRAME m_imageFrame;
	int m_waitTime;
	BOOL m_nearMode;

	SIZE Res2Size(int res);
};

#endif
/*_*/
