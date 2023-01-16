
#ifdef DIRECT_X_9//
#include "DXUT.h"
#include <commdlg.h>
#include "MyInclude.h"
#include <vector>
#ifdef DLLEXPORTS
#define PHANTOM_API __declspec(dllexport)
#else
#define PHANTOM_API __declspec(dllimport)
#endif
extern	BOOL	OnWin32Proc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);
bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext );
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext );
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext );
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext );
void CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );
void CALLBACK OnLostDevice( void* pUserContext );
void CALLBACK OnDestroyDevice( void* pUserContext );
void InitApp();

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
HWND	g_hWndParent	=	0;
PHANTOM_API	HWND	GetDeviceWindow()
{
	return DXUTGetHWND();
}
PHANTOM_API	VOID	SetParentHWND(HWND hWndParent)
{
	g_hWndParent	=	hWndParent;
}

extern	VOID	InitSoundManager(HWND hWnd);

int		g_nWidth = 320;
int		g_nHeight = 480;
wchar_t	g_szTitle2[256];
PHANTOM_API	VOID	InitWindowWH(int w, int h, const wchar_t* szTitle)
{
	if(szTitle)
		wcscpy(g_szTitle2, szTitle);
	else
		wcscpy(g_szTitle2, L"PhantomGameEngine SDK Demos");
	g_nWidth	=	w;
	g_nHeight	=	h;
    DXUTSetCallbackD3D9DeviceAcceptable( IsDeviceAcceptable );
    DXUTSetCallbackD3D9DeviceCreated( OnCreateDevice );
    DXUTSetCallbackD3D9DeviceReset( OnResetDevice );
    DXUTSetCallbackD3D9FrameRender( OnFrameRender );
    DXUTSetCallbackD3D9DeviceLost( OnLostDevice );
    DXUTSetCallbackD3D9DeviceDestroyed( OnDestroyDevice );
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackKeyboard( KeyboardProc );
    DXUTSetCallbackFrameMove( OnFrameMove );
    DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );
    DXUTSetCursorSettings( true, true );
    InitApp();
    DXUTInit( true, true ); // Parse the command line and show msgboxes
    DXUTSetHotkeyHandling( true, true, true );  // handle the defaul hotkeys
    DXUTCreateWindow(g_hWndParent, g_szTitle2 );
	InitSoundManager(DXUTGetHWND());
}
PHANTOM_API	VOID	InitWindow()
{
	InitWindowWH(320, 480, 0);
}
PHANTOM_API	VOID	InitDevice(const RECT& rc)
{
	DXUTCreateDevice( true, rc.right - rc.left, rc.bottom - rc.top );
	MoveWindow(DXUTGetHWND(), 0, 0, rc.right - rc.left, rc.bottom - rc.top, true);
}

PHANTOM_API	VOID	RunDemo()
{
	DXUTCreateDevice( true, g_nWidth, g_nHeight );
	RECT rcWindow, rcClient;
	GetWindowRect(DXUTGetHWND(), &rcWindow);
	GetClientRect(DXUTGetHWND(), &rcClient);
	int bW = (rcWindow.right - rcWindow.left) - (rcClient.right - rcClient.left);
	int bH = (rcWindow.bottom - rcWindow.top) - (rcClient.bottom - rcClient.top);
	MoveWindow(DXUTGetHWND(), 0, 0, g_nWidth + bW, g_nHeight + bH, true);
	DXUTMainLoop();
}

void InitApp()
{
}

IDirect3DDevice9* g_pd3dDevice = 0;
IDirect3DDevice9 * GetDevice9(void)
{
	return g_pd3dDevice;
}
PHANTOM_API	HWND	GetHWND()
{
	return DXUTGetHWND();
}

bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat,
                                  D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    return true;
}

bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
    assert( DXUT_D3D9_DEVICE == pDeviceSettings->ver );

    HRESULT hr;
    IDirect3D9* pD3D = DXUTGetD3D9Object();
    D3DCAPS9 caps;
    V( pD3D->GetDeviceCaps( pDeviceSettings->d3d9.AdapterOrdinal,
                            pDeviceSettings->d3d9.DeviceType,
                            &caps ) );
    if( ( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) == 0 ||
        caps.VertexShaderVersion < D3DVS_VERSION( 1, 1 ) )
    {
        pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }
    return true;
}

//bool					IMEMode = false;
//std::vector<wchar_t>	IMETexts;
//int						IMETextCursor = 0;
//int						IMETextSelect = 0;
//void*					IMEContext = 0;
//HWND					HWNDMain = 0;
//void OpenEditBox(const short* Text, int nCursor, int nSelect, void* Context)
//{
//	int length = wcslen((wchar_t*)Text);
//	nCursor = nSelect = length;
//	IMETexts.resize(length);
//	memcpy(&IMETexts.at(0), Text, sizeof(wchar_t)*length);
//	IMETextCursor = nCursor;
//	IMETextSelect = nSelect;
//	IMEMode = true;
//	IMEContext = Context;
//	SetFocus(HWNDMain);
//}
//void	OnChangeIME() {
//	std::vector<wchar_t>	Temp;
//	int length = IMETexts.size();
//	Temp.resize(length + 1);
//	if (length>0)
//		memcpy(&Temp.at(0), &IMETexts.at(0), sizeof(wchar_t)*length);
//	Temp[length] = 0;
//	UpdateIMEChar(IMEContext, (const short*)&Temp.at(0), IMETextCursor, IMETextSelect);
//}
//
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	g_pd3dDevice	=	pd3dDevice;
	return S_OK;
}
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	g_pd3dDevice	=	pd3dDevice;
	OnAppResize(pBackBufferSurfaceDesc->Width>pBackBufferSurfaceDesc->Height?3:1,pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height, pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height);
	OnAppRestore();
    return S_OK;
}

void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
	//static bool bIME = false;
	//if (!bIME) {
	//	InitIMEChar(&OpenEditBox);
	//	bIME = true;
	//}
	if(!OnAppFrameMove())
	{
		SendMessage(DXUTGetHWND(), WM_CLOSE, 0, 0);
	}
}

void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
	g_pd3dDevice	=	pd3dDevice;
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
		//pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xf000000, 1.0f, 0L );
		if(!OnAppRender())
		{
			
		}
        pd3dDevice->EndScene();
    }
}
BOOL g_bLButtonDown2 = false;
BOOL g_bRButtonDown2 = false;
BOOL g_bMButtonDown2 = false;
void					OnEditBoxMessage(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
void					OnEditBoxSelectNone();
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext )
{
	//HWNDMain = hWnd;
	POINT pos;
	GetCursorPos(&pos);
	ScreenToClient(hWnd, &pos);
	int nArrow = -1;
	bool bIsInput = false;
	switch(uMsg)
	{
	case WM_CREATE:
	{
		break;
	}
	case WM_CHAR:
	{
		//if (IMETextCursor >= IMETexts.size())
		//	IMETexts.push_back(wParam);
		//else
		//	IMETexts.insert(IMETexts.begin() + IMETextCursor, wParam);
		//IMETextCursor++;
		//OnChangeIME();
		break;
	}
	case WM_LBUTTONDBLCLK:
		nArrow = OnAppInputEvent(2, pos.x, pos.y, 0, &bIsInput);
		break;
	case WM_LBUTTONDOWN:
		OnEditBoxSelectNone();
		SetCapture(hWnd);
		SetFocus(hWnd);
		nArrow = OnAppInputEvent(2, pos.x, pos.y, 0, &bIsInput);
		g_bLButtonDown2 = true;
		break;
	case WM_LBUTTONUP:
		ReleaseCapture();
		nArrow = OnAppInputEvent(4, pos.x, pos.y, 0, &bIsInput);
		g_bLButtonDown2 = false;
		break;
	case WM_RBUTTONDOWN:
		SetCapture(hWnd);
		nArrow = OnAppInputEvent(2, pos.x, pos.y, 1, &bIsInput);
		g_bRButtonDown2 = true;
		break;
	case WM_RBUTTONUP:
		ReleaseCapture();
		nArrow = OnAppInputEvent(4, pos.x, pos.y, 1, &bIsInput);
		g_bRButtonDown2 = false;
		break;
	case WM_MBUTTONDOWN:
		SetCapture(hWnd);
		nArrow = OnAppInputEvent(2, pos.x, pos.y, 2, &bIsInput);
		g_bMButtonDown2 = true;
		break;
	case WM_MBUTTONUP:
		ReleaseCapture();
		nArrow = OnAppInputEvent(4, pos.x, pos.y, 2, &bIsInput);
		g_bMButtonDown2 = false;
		break;
	case WM_MOUSEMOVE:
		{
			if(g_bLButtonDown2)
				nArrow = OnAppInputEvent(1, pos.x, pos.y, 0, &bIsInput);
			else if(g_bRButtonDown2)
				nArrow = OnAppInputEvent(1, pos.x, pos.y, 1, &bIsInput);
			else if (g_bMButtonDown2)
				nArrow = OnAppInputEvent(1, pos.x, pos.y, 2, &bIsInput);
			else
				nArrow = OnAppInputEvent(1, pos.x, pos.y, 3, &bIsInput);

	}
		break;
	case WM_MOUSEWHEEL:
		nArrow = OnAppInputEvent(5, (short)HIWORD(wParam), 0, 0, &bIsInput);
		break;
	case WM_CLOSE:
	case WM_DESTROY:
		OnAppDestroy();
		break;
	case WM_SETCURSOR:
	{
		return TRUE;
	}
	case WM_KEYDOWN:
		if(wParam == VK_RETURN)
		{
			nArrow = OnAppInputEvent(6, 0, 0, 0, &bIsInput);
		}
		break;
	case WM_SIZE:
		{
			RECT rc;
			GetClientRect(hWnd, &rc);
			OnAppResize((rc.right-rc.left)>(rc.bottom-rc.top)?3:1,rc.right, rc.bottom, rc.right, rc.bottom);
		}
		break;
	}
	if (nArrow >= 0) {
		//SetClassLong(hWnd, (-12), (LONG)hCurs2);   // ne
		if (nArrow == 0) {
			static HCURSOR hand = ::LoadCursor(NULL, IDC_ARROW);
			SetCursor(hand);
		}
		else if (nArrow == 1) {
			static HCURSOR hand = ::LoadCursor(NULL, IDC_HAND);
			SetCursor(hand);
		}
		else if (nArrow == 2) {
			static HCURSOR hand = ::LoadCursor(NULL, IDC_CROSS);
			SetCursor(hand);
		}
		else if (nArrow == 3) {
			static HCURSOR hand = ::LoadCursor(NULL, IDC_SIZEALL);
			SetCursor(hand);
		}
		else if (nArrow == 4) {
			static HCURSOR hand = ::LoadCursor(NULL, IDC_IBEAM);
			SetCursor(hand);
		}
	}
	//wchar_t W[32];
	//swprintf(W, L"%d", nArrow);
	//OutputDebugString(W);
	if(!bIsInput && OnWin32Proc(hWnd, uMsg, wParam, lParam))
		*pbNoFurtherProcessing = 1;
	OnEditBoxMessage(hWnd, uMsg, wParam, lParam);
	return 0;
}

void CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
}

void CALLBACK OnLostDevice( void* pUserContext )
{
	OnAppLost();
}
void CALLBACK OnDestroyDevice( void* pUserContext )
{
}

#endif
