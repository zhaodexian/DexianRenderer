//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
Copyright 2009-2016 Zhao Dexian


-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////
#include "PhantomManager.h"
#include "encrypt.h"
#include "MD5.h"
#include "DexianUIBase.h"
#include "MyInclude.h"

using namespace Phantom;

Pixel  g_touchPixels[MAX_TOUCH_COUNT];

#ifdef OGL_V1
extern	"C"
{
	BOOL	UseOpenGLV1(){return true;}
};
#endif
extern	VOID	OnBeforeDeviceInit();

#ifdef IPHONE_OS
extern	"C"
{
#endif
	PHANTOM_API void	SetCharLocale(const char* szCode, BOOL bIsUTF8)//chs[简体中文], en_US.UTF-8[uf8编码]
	{
		setlocale(LC_ALL, szCode);
		g_manager.SetIsUtf8(bIsUTF8);
	}
	PHANTOM_API double  GetAppTime(){
        return g_manager.GetAppTime();
    }
	//void	InitIMEChar(OpenEditBoxProc Proc)
	//{
	//	DexianManager::GetInstance().OnOpenEdit = Proc;
	//}
	//void	UpdateIMEChar(void* Context, const short* Text, int nCursor, int nSelect)
	//{
	//	DexianManager::GetInstance().OnIMEChange(Context, Text, nCursor, nSelect);
	//}
	PHANTOM_API void    SetAppDocumentDir(const char* szDir)
    {
        char buf[512];
        strcpy(buf, szDir);
        strcat(buf, "/");
        Phantom::SetDocumentDir(buf);
		LogInfo("SetDir %s", szDir);
    }
	PHANTOM_API void	AddZipFile(const char* szZipFile, const char* szFirstDir)
	{
		LFileName l;
		l = szZipFile;
		l.setIsFile();
		LFileName dir;
		l.splitpath(&dir, 0, 0);
		SetProjectDir(dir);
		open_zip(szZipFile, szFirstDir);
	}
	PHANTOM_API void	OnAppRestore()
	{
		cputime::s_channelId = cputime::cid_resetdev;
		g_manager.OnResetDevice();
	}
	PHANTOM_API void	OnAppLost()
	{
		g_manager.OnLostDevice();
	}
	PHANTOM_API void	OnAppPause()
	{
		g_manager.OnPause();
	}
	PHANTOM_API void	OnAppResume()
	{
		g_manager.OnResume();
	}
	PHANTOM_API int     OnAppFrameMove()
    {
		GetCameraPtr()->processEye();
		cputime::s_channelId = cputime::cid_framemove;
        return g_manager.AppFrameMove();
    }
	PHANTOM_API int		GetMultiSample()
	{
		return g_manager.Multisample;
	}
	PHANTOM_API int		OnAppCreate(const char* szAppPath, int nDivTexture, int ost, int* authcodes)
	{
		cputime::s_channelId = cputime::cid_create;
		if(authcodes){
            unsigned char guid[512];
            unsigned char auth[512];
            MD5 md5("game.vrbigspace.com", strlen("game.vrbigspace.com"));
            unsigned char* ch = (unsigned char*)md5.digest();
            for(int i=0;i<512;i++){
                guid[i] = authcodes[i]^ch[i%16];
                auth[i] = authcodes[i+512];
            }
//			int guids[128];
//			for(int i=0;i<128;i++)
//				guids[i] = authcodes[i];
			SetPhantomGUID((unsigned int*)guid);
            SetAuthCode((unsigned int*)auth);
			//SetStateGUID(states);
		}
		OnBeforeDeviceInit();
        memset_s(g_touchPixels, 0, sizeof(g_touchPixels));
		if(szAppPath)
        {
            SetProjectDir(szAppPath);
        }
		LogInfo("TextureDiv %d", nDivTexture);
        g_manager.SetTextureDiv(nDivTexture); //set texture divide mode, 2=original size * 1/2, 4 = original size * 1/4
		if(g_manager.AppCreate(ost))
		{
			g_manager.SetupParticles(500);	//最多创建500个粒子
			return true;
		}
		return false;
	}
	PHANTOM_API void	OnAppResize(int ori, int w, int h, int devw, int devh)
	{
		cputime::s_channelId = cputime::cid_resize;
        g_manager.m_nOrientation = ori;
		g_manager.AppResize(w, h, devw, devh);
	}
	PHANTOM_API int		OnAppRender()
	{
		cputime::s_channelId = cputime::cid_render;
		return g_manager.AppRender();
	}
	PHANTOM_API int		OnAppInputEvent(int nEvent, int x, int y, unsigned int touchIndex, bool* bOK)
	{
		cputime::s_channelId = cputime::cid_framemove;
        if(touchIndex >= MAX_TOUCH_COUNT)
            touchIndex = MAX_TOUCH_COUNT - 1;
        *bOK = g_manager.AppInputEvent((Phantom::InputEventID)nEvent, x, y, touchIndex);
		return (int)g_manager.CursorMode;
	}
	PHANTOM_API void	OnAppDestroyEditor(){
		g_manager.AppDestroy();
	}
	PHANTOM_API void	OnAppDestroy()
	{
		//if(g_manager.GetActiveSDK() && g_manager.GetActiveSDK()->GotoBack())
			//return;
		g_manager.AppDestroy();
#ifdef WIN32_OS
		//PostQuitMessage(0);
#else
		//exit(0);
#endif
	}
#ifdef IPHONE_OS
};
#endif
