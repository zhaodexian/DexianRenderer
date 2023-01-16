#pragma once
#define WIN32_OS
#include <WinSock2.h>
#include <Windows.h>
#include "PhantomBase.h"
#include "PhantomManager.h"
#include "DexianUIBase.h"

using namespace Phantom;
class PHANTOM_API EngineAPI: public SDKBase
{
public:
	EngineAPI();
	virtual ~EngineAPI();
public:
	virtual void	Init(HWND hWnd);
	virtual void	OnBefore() {}
	virtual	BOOL	OnWin32Proc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam) { return false; }
public:
	void			Render();
	HWND			GetWindow();
};
PHANTOM_API	void	SetEngineAPI(EngineAPI* Api);

PHANTOM_API	HWND	GetDeviceWindow();
PHANTOM_API	VOID	SetParentHWND(HWND hWndParent);
PHANTOM_API	VOID	InitSoundManager(HWND hWnd);
PHANTOM_API	VOID	InitWindowWH(int w, int h, const wchar_t* szTitle);
PHANTOM_API	VOID	InitWindow();
PHANTOM_API	VOID	InitDevice(const RECT& rc);
PHANTOM_API	VOID	DestroyDevice();
PHANTOM_API	VOID	RunDemo();
PHANTOM_API	HWND	GetHWND();

