#include "EngineAPI.h"
#include "MyInclude.h"
#include "resource.h"

EngineAPI*	g_Api = 0;
PHANTOM_API	void	SetEngineAPI(EngineAPI* Api)
{
	g_Api = Api;
}
EngineAPI::EngineAPI()
{
	SetEngineAPI(this);
}
void OnBeforeDeviceInit() {
	if (g_Api)
		g_Api->OnBefore();
}
void FromLuaMessage(char const *, char const *, char const *)
{

}

namespace Phantom {
	void Global_msgbox(char const *, char const *, char const *, char const *)
	{
	}
	void Global_goto(char const *)
	{
	}
}
extern "C"
{
	extern	void    showAd(int x, int y, int type, int bShow) {}
	extern	void    shopBuy(const char* buyName) {}
	extern	void    shopRestore(const char* buyName) {}
	extern	void	onVibrator(float fSecond) {}
	void	gotoBuyRemoveAD(int) {}
	void	gotoComment() {}
	void	openRank(const char* rank) {}
	void	setRankScore(const char* rank, int value) {}
	void	setAchievement(const char* achievement, int persent) {}
	void	openUrl(const char* url) {}
	void	openEdit(const char* szDefault, int x, int y, int w, int h){}
	void	closeEdit(char* ret){}
	const char*	onAlert(const char* message, const char* title, const char* type) { return ""; }
};
BOOL __cdecl	OnWin32Proc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	if (g_Api)
		return g_Api->OnWin32Proc(hWnd, uiMsg, wParam, lParam);
	return 0;
}
extern void WINAPI DXUTDestroyState();
PHANTOM_API	VOID	DestroyDevice()
{
	DXUTDestroyState();
}
EngineAPI::~EngineAPI()
{
	SetEngineAPI(0);
}
extern	VOID	InitDevice(const RECT& rc);
extern	VOID	SetParentHWND(HWND hWndParent);
extern	VOID	InitWindow();

BOOL		LoadResData(TypeArray<char>& rets, const char* szResID, const char* szResType, HMODULE hModule)
{
	HRSRC hRsrc = ::FindResourceA(hModule, szResID, szResType);
	//获取资源的大小
	DWORD dwSize = SizeofResource(hModule, hRsrc);
	if (0 == dwSize)
		return FALSE;
	//加载资源
	HGLOBAL hResHandle = LoadResource(hModule, hRsrc);
	if (NULL == hResHandle)
		return FALSE;
	LPVOID pBuffer = LockResource(hResHandle);
	if (NULL == pBuffer)
		return FALSE;
	rets.resize(dwSize, false);
	memcpy(rets.GetPtr(), pBuffer, dwSize);
	UnlockResource(hResHandle);
	FreeResource(hResHandle);
	return TRUE;
}
int GetAppID() { return 0; }
int g_guid[128] = { -881829239,531795469,205672448,133909,-279732572,-40749204,-1544058199,1115756337,-639047129,1877516808,-740304721,1521638536,-1207154134,408159893,-1290536934,-628714630,1312163671,-871496481,210487935,378976542,-920389104,1091360994,1824501840,1353990702,2100465822,41953219,1187414059,-484479293,-61857979,1836278236,1731518070,1000663815,-2145327700,-1893731463,-1491181980,1799716396,1382820141,-1567732596 };
extern	PHANTOM_API	VOID	OnIdleRender();
extern	HWND	GetDeviceWindow();

HWND		EngineAPI::GetWindow() {
	return GetDeviceWindow();
}
void		EngineAPI::Render()
{
	OnIdleRender();
}
extern HMODULE g_hModule;
void		EngineAPI::Init(HWND hWnd)
{
	SetParentHWND(hWnd);
	InitWindow();

	TypeArray<char> defaultZip;
	if (!LoadResData(defaultZip, MAKEINTRESOURCE(IDR_IDR_INIT), "IDR_INIT", g_hModule))// AfxGetResourceHandle()))
		return;
	//
	Phantom::open_zip_mem(defaultZip.GetPtr(), defaultZip.size());
	//AddZipFile("..\\android\\PhantomApp\\data.zip", "assets");
	OnAppCreate(0, 1, OSType_Windows, g_guid);
	RECT rc;
	GetClientRect(hWnd, &rc);
	MoveWindow(GetDeviceWindow(), 0, 0, rc.right - rc.left, rc.bottom - rc.top, true);
	InitDevice(rc);
	g_manager.SetActiveSDK(this);
}

