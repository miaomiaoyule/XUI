#include "stdafx.h"
#include "DUIWndSDL.h"
#include "../DUIRender/DUIPaintSceneSDL.h"

#if defined DuiPlatform_SDL

//////////////////////////////////////////////////////////////////////////
CDUIWndSDL::CDUIWndSDL(LPCTSTR lpszName, HWND hWndParent)
	: CDUIWndBase(lpszName, hWndParent)
{

}

CDUIWndSDL::~CDUIWndSDL()
{
	//unsub window
	UnSubWindow();

	ReleasePaintScene();

	//wnd dc
	if (IsWindow(m_hWnd))
	{
		SDL_SetWindowHitTest(m_hWnd, NULL, NULL);
		MMUnregisterWnd(m_hWnd);
		SDL_DestroyWindow(m_hWnd);
		m_hWnd = NULL;
		m_uWndID = 0;
	}

	return;
}

LPVOID CDUIWndSDL::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIWndSDL, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIWndSDL::GetDescribe() const
{
	return Dui_WindowSDL;
}

HDC CDUIWndSDL::GetWndDC()
{
	EnsurePaintScene();
	return m_hDCPaint;
}

UINT CDUIWndSDL::MapWndKeyState()
{
	UINT uState = 0;
	SDL_Keymod mod = SDL_GetModState();
	if (mod & SDL_KMOD_CTRL) uState |= MK_CONTROL;
	if (mod & SDL_KMOD_SHIFT) uState |= MK_SHIFT;
	if (mod & SDL_KMOD_ALT) uState |= MK_ALT;

	float x = 0, y = 0;
	SDL_MouseButtonFlags buttons = SDL_GetGlobalMouseState(&x, &y);
	if (buttons & SDL_BUTTON_LMASK) uState |= MK_LBUTTON;
	if (buttons & SDL_BUTTON_RMASK) uState |= MK_RBUTTON;
	if (buttons & SDL_BUTTON_MMASK) uState |= MK_MBUTTON;

	return uState;
}

HWND CDUIWndSDL::Create(HWND hWndParent, LPCTSTR lpszName, DWORD dwStyle, DWORD dwExStyle, int x, int y, int cx, int cy)
{
	m_hWndParent = hWndParent;
	m_ptCreate = { x, y };

	const bool bSdlPopup = (NULL != hWndParent)
		&& (0 != (dwStyle & WS_POPUP))
		&& (0 == (dwStyle & WS_CHILD))
		&& (0 == (dwStyle & WS_CAPTION));

	std::string strTitle = MMStringToUtf8(NULL == lpszName ? _T("") : lpszName);

	if (bSdlPopup)
	{
		int nPopupCx = (cx > 0) ? cx : 32;
		int nPopupCy = (cy > 0) ? cy : 32;
		int ox = 0;
		int oy = 0;
		if (CW_USEDEFAULT != x && CW_USEDEFAULT != y)
		{
			RECT rcParent = {};
			::GetWindowRect(hWndParent, &rcParent);
			ox = x - rcParent.left;
			oy = y - rcParent.top;
		}

		SDL_WindowFlags uPopupFlags = SDL_WINDOW_POPUP_MENU | SDL_WINDOW_HIDDEN;
		if (dwStyle & WS_VISIBLE)
		{
			uPopupFlags &= ~SDL_WINDOW_HIDDEN;
		}
		if (dwExStyle & WS_EX_NOACTIVATE)
		{
			uPopupFlags |= SDL_WINDOW_NOT_FOCUSABLE;
		}

		m_hWnd = SDL_CreatePopupWindow((SDL_Window *)hWndParent, ox, oy, nPopupCx, nPopupCy, uPopupFlags);
	}
	else
	{
		if (cx <= 0) cx = 800;
		if (cy <= 0) cy = 600;

		SDL_PropertiesID props = SDL_CreateProperties();
		SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, strTitle.c_str());
		SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, cx);
		SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, cy);
		SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_BORDERLESS_BOOLEAN, true);
#if defined(__ANDROID__)
		SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_FULLSCREEN_BOOLEAN, true);
		SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN, true);
#elif defined(__APPLE__)
		SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN, true);
#endif
		if (dwStyle & WS_THICKFRAME)
		{
			SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, true);
		}
		if (0 == (dwStyle & WS_VISIBLE))
		{
			SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_HIDDEN_BOOLEAN, true);
		}
		if (CW_USEDEFAULT != x && CW_USEDEFAULT != y)
		{
			SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, x);
			SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, y);
		}
		if (hWndParent)
		{
			SDL_SetPointerProperty(props, SDL_PROP_WINDOW_CREATE_PARENT_POINTER, hWndParent);
		}

		m_hWnd = SDL_CreateWindowWithProperties(props);
		SDL_DestroyProperties(props);
	}

	if (NULL == m_hWnd)
	{
		MMTRACE(_T("SDL create window failed: %s"), (LPCTSTR)CA2CT(SDL_GetError(), CP_ACP));
		return NULL;
	}

	m_uWndID = SDL_GetWindowID(m_hWnd);
	MMRegisterWnd(m_hWnd, this);
	SDL_SetWindowHitTest(m_hWnd, &SDLEnableHitTest, this);

	OnWndMessage(WM_CREATE, 0, 0);

	return m_hWnd;
}

HWND CDUIWndSDL::SubWindow(HWND hWnd)
{
	m_hWnd = hWnd;
	m_uWndID = SDL_GetWindowID(hWnd);
	m_bSubWindow = true;
	MMRegisterWnd(m_hWnd, this);
	SDL_SetWindowHitTest(m_hWnd, &SDLEnableHitTest, this);
	return m_hWnd;
}

void CDUIWndSDL::UnSubWindow()
{
	if (!::IsWindow(m_hWnd)) return;
	if (!m_bSubWindow) return;
	SDL_SetWindowHitTest(m_hWnd, NULL, NULL);
	MMUnregisterWnd(m_hWnd);
	m_hWnd = nullptr;
	m_uWndID = 0;
	m_bSubWindow = false;
	return;
}

void CDUIWndSDL::ShowWindow(bool bShow /*= true*/, bool bTakeFocus /*= false*/)
{
	if (m_hWnd == nullptr) return;

	if (bShow)
	{
		SDL_ShowWindow(m_hWnd);

		// SDL 没有 SW_SHOWNOACTIVATE 的完全等价语义
		// 需要抢焦点时用 RaiseWindow
		if (bTakeFocus)
		{
			SDL_RaiseWindow(m_hWnd);
		}
	}
	else
	{
		SDL_HideWindow(m_hWnd);
	}

	return;
}

UINT CDUIWndSDL::DoModal()
{
	//create
	if (false == IsWindow(m_hWnd))
	{
		Create(m_hWndParent, _T(""), DUI_WNDSTYLE_DIALOG, DUI_WNDSTYLE_EX_DIALOG);
	}
	if (false == IsWindow(m_hWnd))
	{
		return 0;
	}

	//SDL 下没有与 EnableWindow(parent,false) 完全等价的跨平台通用实现，这里先省略父窗禁用
	UINT nRet = 0;

	//show
	ShowWindow(true, false);

	//message
	SDL_Event e = {};
	while (::IsWindowVisible(m_hWnd))
	{
		if (false == SDL_WaitEvent(&e))
		{
			assert(false);
			MMTRACE(_T("EXCEPTION: SDL_WaitEvent failed\n"));
			break;
		}

		MSG Msg = {};
		if (false == MMSdlEventToMsg(e, Msg) || false == CDUIGlobal::GetInstance()->TranslateMessage(&Msg))
		{
			MMSdlDispatchEvent(e);
		}

		if (SDL_EVENT_QUIT == e.type)
		{
			break;
		}
		if ((SDL_EVENT_WINDOW_CLOSE_REQUESTED == e.type || SDL_EVENT_WINDOW_DESTROYED == e.type)
			&& e.window.windowID == m_uWndID)
		{
			nRet = (m_uCtrlIDClose == 0) ? Dui_CtrlIDInner_BtnCancel : m_uCtrlIDClose;
			break;
		}
	}

	//quit
	if (SDL_EVENT_QUIT == e.type)
	{
		SDL_Event quitEvent = {};
		quitEvent.type = SDL_EVENT_QUIT;
		SDL_PeepEvents(&quitEvent, 1, SDL_ADDEVENT, 0, 0);
	}

	if (0 == nRet)
	{
		nRet = (m_uCtrlIDClose == 0) ? Dui_CtrlIDInner_BtnCancel : m_uCtrlIDClose;
	}

	return nRet;
}

UINT CDUIWndSDL::DoBlock()
{
	//create
	if (false == IsWindow(m_hWnd))
	{
		Create(m_hWndParent, _T(""), DUI_WNDSTYLE_DIALOG, DUI_WNDSTYLE_EX_DIALOG);
	}
	if (false == IsWindow(m_hWnd))
	{
		return 0;
	}

	//SDL 下没有与 EnableWindow(parent,false) 完全等价的跨平台通用实现，这里先省略父窗禁用
	UINT nRet = 0;

	//show
	ShowWindow(true, false);

	//message
	SDL_Event e = {};
	while (::IsWindowVisible(m_hWnd))
	{
		if (false == SDL_WaitEvent(&e))
		{
			assert(false);
			MMTRACE(_T("EXCEPTION: SDL_WaitEvent failed\n"));
			break;
		}

		MSG Msg = {};
		if (false == MMSdlEventToMsg(e, Msg) || false == CDUIGlobal::GetInstance()->TranslateMessage(&Msg))
		{
			MMSdlDispatchEvent(e);
		}

		if (SDL_EVENT_QUIT == e.type)
		{
			break;
		}
		if ((SDL_EVENT_WINDOW_CLOSE_REQUESTED == e.type || SDL_EVENT_WINDOW_DESTROYED == e.type)
			&& e.window.windowID == m_uWndID)
		{
			nRet = (m_uCtrlIDClose == 0) ? Dui_CtrlIDInner_BtnCancel : m_uCtrlIDClose;
			break;
		}
	}

	//quit
	if (SDL_EVENT_QUIT == e.type)
	{
		SDL_Event quitEvent = {};
		quitEvent.type = SDL_EVENT_QUIT;
		SDL_PeepEvents(&quitEvent, 1, SDL_ADDEVENT, 0, 0);
	}

	if (0 == nRet)
	{
		nRet = (m_uCtrlIDClose == 0) ? Dui_CtrlIDInner_BtnCancel : m_uCtrlIDClose;
	}

	return nRet;
}

void CDUIWndSDL::Close(UINT nRet)
{
	__super::Close(nRet);
}

void CDUIWndSDL::CenterWindow()
{
	if (m_hWnd == NULL) return;

	int nDlgWidth = 0;
	int nDlgHeight = 0;
	SDL_GetWindowSize(m_hWnd, &nDlgWidth, &nDlgHeight);
	if (nDlgWidth <= 0 || nDlgHeight <= 0) return;

	// 工作区（优先当前窗口所在显示器）
	SDL_Rect rcArea = { 0, 0, nDlgWidth, nDlgHeight };
	SDL_DisplayID nDisplayID = SDL_GetDisplayForWindow(m_hWnd);
	if (nDisplayID != 0)
	{
		if (false == SDL_GetDisplayUsableBounds(nDisplayID, &rcArea))
		{
			SDL_GetDisplayBounds(nDisplayID, &rcArea);
		}
	}

	// 参考中心区域：优先父窗口，否则用工作区
	SDL_Rect rcCenter = rcArea;
	if (m_hWndParent != NULL)
	{
		int xParent = 0;
		int yParent = 0;
		int cxParent = 0;
		int cyParent = 0;
		SDL_GetWindowPosition(m_hWndParent, &xParent, &yParent);
		SDL_GetWindowSize(m_hWndParent, &cxParent, &cyParent);

		if (cxParent > 0 && cyParent > 0)
		{
			rcCenter.x = xParent;
			rcCenter.y = yParent;
			rcCenter.w = cxParent;
			rcCenter.h = cyParent;
		}
	}

	int xLeft = rcCenter.x + (rcCenter.w - nDlgWidth) / 2;
	int yTop = rcCenter.y + (rcCenter.h - nDlgHeight) / 2;

	// 限制在工作区内
	if (xLeft < rcArea.x) xLeft = rcArea.x;
	else if (xLeft + nDlgWidth > rcArea.x + rcArea.w) xLeft = rcArea.x + rcArea.w - nDlgWidth;

	if (yTop < rcArea.y) yTop = rcArea.y;
	else if (yTop + nDlgHeight > rcArea.y + rcArea.h) yTop = rcArea.y + rcArea.h - nDlgHeight;

	::SetWindowPos(m_hWnd, NULL, xLeft, yTop, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

	return;
}

void CDUIWndSDL::SetIcon(UINT nRes)
{
	if (m_hWnd == NULL) return;

	// Linux 下没有 Win32 的 HICON/WM_SETICON 体系，
	// 这里将 nRes 解释为 DUI 图片资源索引。
	CDUIImageBase *pImage = CDUIGlobal::GetInstance()->GetImageResource((int)nRes);
	if (pImage == NULL) return;

	CMMString strIconFile = pImage->GetImageFileFull();
	if (strIconFile.empty()) return;

	std::string strIconFileUtf8 = MMStringToUtf8(strIconFile);

	// 优先 PNG（SDL3 3.4+），失败再尝试 BMP
	SDL_Surface *pSurfaceIcon = SDL_LoadPNG(strIconFileUtf8.c_str());
	if (pSurfaceIcon == NULL)
	{
		pSurfaceIcon = SDL_LoadBMP(strIconFileUtf8.c_str());
	}
	if (pSurfaceIcon == NULL) return;

	SDL_SetWindowIcon(m_hWnd, pSurfaceIcon);
	SDL_DestroySurface(pSurfaceIcon);

	return;
}

LRESULT CDUIWndSDL::SendMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
	if (false == IsWindow(m_hWnd))
	{
		ASSERT(false);
		return 0;
	}

	return OnWndMessage(uMsg, wParam, lParam);
}

LRESULT CDUIWndSDL::PostMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
	if (false == IsWindow(m_hWnd))
	{
		ASSERT(false);
		return 0;
	}

	const Uint32 uEventType = MMSdlGetAsyncEventType();
	if (uEventType == 0 || uEventType == static_cast<Uint32>(-1))
	{
		ASSERT(false);
		return 0;
	}

	//construct
	tagMMSdlAsyncMsg *pAsyncMsg = new (std::nothrow) tagMMSdlAsyncMsg();
	if (NULL == pAsyncMsg) return 0;

	pAsyncMsg->pWnd = this;
	pAsyncMsg->uMsg = uMsg;
	pAsyncMsg->wParam = wParam;
	pAsyncMsg->lParam = lParam;

	//push
	SDL_Event e = {};
	e.type = uEventType;
	e.user.timestamp = SDL_GetTicksNS();
	e.user.windowID = m_uWndID;
	e.user.code = 0;
	e.user.data1 = pAsyncMsg;
	e.user.data2 = NULL;
	if (SDL_PeepEvents(&e, 1, SDL_ADDEVENT, 0, 0) != 1)
	{
		MMSafeDelete(pAsyncMsg);

		return 0;
	}

	return 1;
}

void CDUIWndSDL::SetCapture()
{
	if (false == IsWindow(m_hWnd)) return;

	SDL_CaptureMouse(true);

	return;
}

void CDUIWndSDL::ReleaseCapture()
{
	SDL_CaptureMouse(false);

	return;
}

bool CDUIWndSDL::IsCaptured()
{
	if (false == IsWindow(m_hWnd)) return false;

	SDL_WindowFlags uFlags = SDL_GetWindowFlags(m_hWnd);
	return (uFlags & SDL_WINDOW_MOUSE_CAPTURE) == SDL_WINDOW_MOUSE_CAPTURE;
}

void CDUIWndSDL::SetFocusControl(CDUIControlBase *pFocusCtrl)
{
	__super::SetFocusControl(pFocusCtrl);

	if (m_pFocusCtrl
		&& SDL_GetKeyboardFocus() != m_hWnd)
	{
		// SDL 没有 Win32 SetFocus 的完全等价接口，这里用 Raise 触发前台/焦点请求
		SDL_RaiseWindow(m_hWnd);
	}

	return;
}

CDUIContainerCtrl * CDUIWndSDL::DetachRootCtrl()
{
	// Linux 侧目前未实现 Win32 tooltip window，直接清空句柄即可
	m_hWndTooltip = NULL;

	return __super::DetachRootCtrl();
}

void CDUIWndSDL::RefreshToolTip(CMMString strToolTip)
{
	return;
}

void CDUIWndSDL::ResizeWnd(int cx /*= -1*/, int cy /*= -1*/)
{
	if (false == IsWindow(m_hWnd)) return;

	int nWidth = 0;
	int nHeight = 0;
	SDL_GetWindowSize(m_hWnd, &nWidth, &nHeight);

	if (cx != -1) nWidth = cx;
	if (cy != -1) nHeight = cy;

	if (nWidth <= 0 || nHeight <= 0) return;

	SDL_SetWindowSize(m_hWnd, nWidth, nHeight);

	return;
}

void CDUIWndSDL::AdjustWndPos()
{
	if (false == IsWindow(m_hWnd)) return;

	int x = 0;
	int y = 0;
	int w = 0;
	int h = 0;
	SDL_GetWindowPosition(m_hWnd, &x, &y);
	SDL_GetWindowSize(m_hWnd, &w, &h);

	SDL_Rect rcWork = { 0, 0, w, h };
	SDL_DisplayID nDisplayID = SDL_GetDisplayForWindow(m_hWnd);
	if (nDisplayID != 0)
	{
		if (false == SDL_GetDisplayUsableBounds(nDisplayID, &rcWork))
		{
			SDL_GetDisplayBounds(nDisplayID, &rcWork);
		}
	}

	if (x < rcWork.x)
	{
		x = rcWork.x;
	}
	if (x + w > rcWork.x + rcWork.w)
	{
		x = rcWork.x + rcWork.w - w;
	}
	if (y < rcWork.y)
	{
		y = rcWork.y;
	}
	if (y + h > rcWork.y + rcWork.h)
	{
		y = rcWork.y + rcWork.h - h;
	}
	
	::SetWindowPos(m_hWnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	
	return;
}

bool CDUIWndSDL::IsMaximized()
{
	if (NULL == m_hWnd) return false;
	
	SDL_WindowFlags uFlags = SDL_GetWindowFlags(m_hWnd);
	return (uFlags & SDL_WINDOW_MAXIMIZED) == SDL_WINDOW_MAXIMIZED;
}

bool CDUIWndSDL::IsMinimized()
{
	if (NULL == m_hWnd) return false;
	
	SDL_WindowFlags uFlags = SDL_GetWindowFlags(m_hWnd);
	return (uFlags & SDL_WINDOW_MINIMIZED) == SDL_WINDOW_MINIMIZED;
}

CMMRect CDUIWndSDL::GetClientRect()
{
	CDUIRect rcClient;
	::GetClientRect(m_hWnd, &rcClient);

	return rcClient;
}

CMMRect CDUIWndSDL::GetWindowRect()
{
	CDUIRect rcWnd;
	::GetWindowRect(m_hWnd, &rcWnd);

	return rcWnd;
}

void CDUIWndSDL::Invalidate()
{
	if (NULL == m_hWnd) return;

	NeedRefreshView();

	// Full-client dirty + coalesced EXPOSED (same path as control InvalidateRect)
	::InvalidateRect(m_hWnd, NULL, TRUE);

	return;
}

bool CDUIWndSDL::CreateCaret(HBITMAP hBmp, int nWidth, int nHeight)
{
	if (false == IsWindow(m_hWnd)) return false;

	return __super::CreateCaret(hBmp, nWidth, nHeight);
}

void CDUIWndSDL::ShowCaret(bool bShow)
{
	return __super::ShowCaret(bShow);
}

void CDUIWndSDL::SetCaretPos(CDUIPoint pt)
{
	// SDL 下没有系统插入光标 API，框架负责绘制插入光标
	// 仅更新内部矩形并触发重绘以便框架在下一次绘制时使用新位置
	m_rcCaret.Offset(pt.x - m_rcCaret.left, pt.y - m_rcCaret.top);
	Invalidate();
	return;
}

LRESULT CDUIWndSDL::OnWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	switch (uMsg)
	{
		case WM_PAINT:
		{
			CDUIRect rcPaint;
			BOOL bUpdate = ::GetUpdateRect(m_hWnd, &rcPaint, true);

#ifdef DUI_DESIGN
			bUpdate = true;
			if (rcPaint.Empty())
			{
				rcPaint = GetClientRect();
			}
#endif

			if (bUpdate)
			{
				lRes = OnPaint(rcPaint);
			}

			//design refresh
			if (g_pIDuiWndNotify)
			{
				g_pIDuiWndNotify->OnDuiWndPaint(this);
			}

			return 0;
		}
		case WM_COMMAND:
		{
			return OnCommand(wParam, lParam);
		}
	}

	return __super::OnWndMessage(uMsg, wParam, lParam);
}

LRESULT CDUIWndSDL::OnOldWndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CDUIWndSDL::OnClose(WPARAM wParam, LPARAM lParam)
{
	m_uCtrlIDClose = (UINT)wParam;
	if (IsWindow(m_hWnd))
	{
		ReleasePaintScene();
		SDL_HideWindow(m_hWnd);
		SDL_SetWindowHitTest(m_hWnd, NULL, NULL);
		MMUnregisterWnd(m_hWnd);
		SDL_DestroyWindow(m_hWnd);
		m_hWnd = NULL;
		m_uWndID = 0;
	}

	OnFinalMessage();

	return 0;
}

LRESULT CDUIWndSDL::OnSysCommand(WPARAM wParam, LPARAM lParam)
{
	if (wParam == SC_CLOSE)
	{
		SendMessage(WM_CLOSE);

		return 0;
	}

	switch (wParam)
	{
		case SC_MINIMIZE:
			SDL_MinimizeWindow(m_hWnd);
			break;
		case SC_MAXIMIZE:
			SDL_MaximizeWindow(m_hWnd);
			break;
		case SC_RESTORE:
			SDL_RestoreWindow(m_hWnd);
			break;
		default:
			break;
	}

	CDUIControlBase *pBtnMax = FindControl(Dui_CtrlIDInner_BtnMax);
	CDUIControlBase *pBtnRestore = FindControl(Dui_CtrlIDInner_BtnRestore);
	if (pBtnMax && pBtnRestore)
	{
		pBtnMax->SetVisible(false == IsZoomed(m_hWnd));
		pBtnRestore->SetVisible(IsZoomed(m_hWnd));
	}

	return 0;
}

LRESULT CDUIWndSDL::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	bool bContextMenu = m_dwMouseDownTick && (::GetTickCount() - m_dwMouseDownTick) >= 1000;
	LRESULT lRes = __super::OnLButtonUp(wParam, lParam);

#if defined(__ANDROID__)
	if (bContextMenu)
	{
		CDUIPoint ptScreen(lParam);
		::ClientToScreen(m_hWnd, &ptScreen);
		OnWndMessage(WM_CONTEXTMENU, (WPARAM)m_hWnd, MAKELPARAM(ptScreen.x, ptScreen.y));
	}
#endif

	return lRes;
}

LRESULT CDUIWndSDL::OnKillFocus(WPARAM wParam, LPARAM lParam)
{
	do
	{
		SDL_Window *pWndFocus = SDL_GetKeyboardFocus();
		if (m_hWnd == pWndFocus
			|| (pWndFocus != NULL && SDL_GetWindowParent(pWndFocus) == m_hWnd))
		{
			break;
		}

		SetFocusControl(NULL);

	} while (false);

	return OnOldWndProc(WM_KILLFOCUS, wParam, lParam);
}

LRESULT CDUIWndSDL::OnPaint(CDUIRect rcPaint)
{
	LRESULT lRes = 0;

	//layout
	RefreshLayout();
	
	//verify
	if (NULL == m_pRootCtrl) return lRes;

	//animation wnd
	if (IsAnimatingWnd()) return lRes;

	//Should we paint?
	CDUIRect rcClient = m_pBmpBackgroundBits ? m_rcBmpBackground : GetClientRect();
	if (false == ::IntersectRect(&rcPaint, &rcPaint, &rcClient)) return lRes;
	if (rcPaint.Empty()) return lRes;

	//scene
	EnsurePaintScene();
	if (NULL == m_pPaintScene || NULL == m_pPaintScene->GetCanvas()) return lRes;

	//layered
	if (IsWndLayered())
	{
		const float fOpacity = max(0.0f, min(1.0f, GetWndAlpha() / 255.0f));
		SDL_SetWindowOpacity(m_hWnd, fOpacity);
	}
	else
	{
		SDL_SetWindowOpacity(m_hWnd, 1.0f);
	}

	//paint
	IDuiCanvas *pCanvas = m_pPaintScene->GetCanvas();
	pCanvas->ClearRect(rcPaint);

	try
	{
		pCanvas->Save();
		m_pRootCtrl->OnDraw(m_pPaintScene->GetCanvasHDC(), rcPaint);
		pCanvas->Restore();
	}
	catch (const std::exception& exception)
	{
		OutputDebugString(CA2CT(exception.what()));
		CDUIGlobal::GetInstance()->SetDuiLastError((LPCTSTR)CA2CT(exception.what()));
	}

	m_pPaintScene->Present(rcPaint);

	return lRes;
}

void CDUIWndSDL::PerformCalcWndMinMaxInfo()
{
	SDL_DisplayID displayID = SDL_GetDisplayForWindow(m_hWnd);
	SDL_Rect sdlMonitor = {};
	SDL_Rect sdlWork = {};
	if (displayID != 0)
	{
		if (false == SDL_GetDisplayBounds(displayID, &sdlMonitor))
		{
			sdlMonitor = { 0, 0, 0, 0 };
		}
		if (false == SDL_GetDisplayUsableBounds(displayID, &sdlWork))
		{
			sdlWork = sdlMonitor;
		}
	}

	CDUIRect rcMonitor(sdlMonitor.x, sdlMonitor.y, sdlMonitor.x + sdlMonitor.w, sdlMonitor.y + sdlMonitor.h);
	CDUIRect rcWork(sdlWork.x, sdlWork.y, sdlWork.x + sdlWork.w, sdlWork.y + sdlWork.h);
	rcWork.Offset(-rcMonitor.left, -rcMonitor.top);

	CDUISize szWndMinSize = GetWndMinSize();
	CDUISize szWndMaxSize = GetWndMaxSize();
	if (szWndMinSize.cx > 0 || szWndMinSize.cy > 0) 
	{
		SDL_SetWindowMinimumSize(m_hWnd, max(1, szWndMinSize.cx), max(1, szWndMinSize.cy));
	} 
	else 
	{
		SDL_SetWindowMinimumSize(m_hWnd, 0, 0);
	}
	if (szWndMaxSize.cx > 0 || szWndMaxSize.cy > 0) 
	{
		SDL_SetWindowMaximumSize(m_hWnd, min(rcWork.GetWidth(), max(1, szWndMaxSize.cx)), min(rcWork.GetHeight(), max(1, szWndMaxSize.cy)));
	} 
	else 
	{
		SDL_SetWindowMaximumSize(m_hWnd, 0, 0);
	}

	return;
}

void CDUIWndSDL::UpdateImeCompositionPos()
{
	POINT pt = {};
	GetCaretPos(&pt);

	SDL_Rect rect = {};
	rect.x = pt.x;
	rect.y = pt.y;
	rect.w = 1;
	rect.h = 1;

	HWND hWnd = GetWndHandle();
	if (NULL == hWnd) return;

	// SDL3: TextInputActive / SetTextInputArea
	if (false == SDL_TextInputActive(hWnd))
	{
		SDL_StartTextInput(hWnd);
	}

	SDL_SetTextInputArea(hWnd, &rect, 0);

	return;
}

void CDUIWndSDL::EnsurePaintScene()
{
	if (false == IsWindow(m_hWnd)) return;

	const CDUIRect rcClient = GetClientRect();
	const int nWidth = max(1, rcClient.GetWidth());
	const int nHeight = max(1, rcClient.GetHeight());
	if (m_pPaintScene && m_pPaintScene->GetWidth() == nWidth && m_pPaintScene->GetHeight() == nHeight)
	{
		m_hDCPaint = m_pPaintScene->GetCanvasHDC();
		m_hMemDcBackground = m_hDCPaint;
		m_pBmpBackgroundBits = m_pPaintScene->GetBits();
		m_rcBmpBackground = { 0,0,m_pPaintScene->GetWidth(),m_pPaintScene->GetHeight() };

		return;
	}

	ReleasePaintScene();
	m_pPaintScene = new CDUIPaintSceneSDL(m_hWnd, nWidth, nHeight);
	m_hDCPaint = m_pPaintScene->GetCanvasHDC();
	m_hMemDcBackground = m_hDCPaint;
	m_pBmpBackgroundBits = m_pPaintScene->GetBits();
	m_rcBmpBackground = { 0,0,m_pPaintScene->GetWidth(),m_pPaintScene->GetHeight() };

	return;
}

void CDUIWndSDL::ReleasePaintScene()
{
	MMSafeDelete(m_pPaintScene);
	m_hDCPaint = NULL;
	m_hMemDcBackground = NULL;
	m_hBmpBackground = NULL;
	m_pBmpBackgroundBits = NULL;

	return;
}

void CDUIWndSDL::OnWndMessage(SDL_Event &e)
{
	SDLEventWatch(this, &e);

	return;
}

void CDUIWndSDL::OnSdlWindowEvent(const SDL_Event &e)
{
	switch (e.type)
	{
		case SDL_EVENT_WINDOW_SHOWN:
		{
			OnWndMessage(WM_SHOWWINDOW, TRUE, 0);
			break;
		}
		case SDL_EVENT_WINDOW_HIDDEN:
		{
			OnWndMessage(WM_SHOWWINDOW, FALSE, 0);
			break;
		}
		case SDL_EVENT_WINDOW_EXPOSED:
		{
			if (m_bWndMoving)
			{
				break;
			}

			OnWndMessage(WM_PAINT, 0, 0);
			break;
		}
		case SDL_EVENT_WINDOW_MOVED:
		{
			OnWndMessage(WM_MOVE, 0, MAKELPARAM(e.window.data1, e.window.data2));
			break;
		}
		case SDL_EVENT_WINDOW_RESIZED:
		case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
		{
			m_bWndMoving = false;
			OnWndMessage(WM_SIZE, SIZE_RESTORED, MAKELPARAM(e.window.data1, e.window.data2));
			break;
		}
		case SDL_EVENT_WINDOW_MINIMIZED:
		{
			OnWndMessage(WM_SIZE, SIZE_MINIMIZED, 0);
			break;
		}
		case SDL_EVENT_WINDOW_MAXIMIZED:
		{
			OnWndMessage(WM_SIZE, SIZE_MAXIMIZED, 0);
			break;
		}
		case SDL_EVENT_WINDOW_RESTORED:
		{
			OnWndMessage(WM_SIZE, SIZE_RESTORED, 0);
			break;
		}
		case SDL_EVENT_WINDOW_FOCUS_GAINED:
		{
			OnWndMessage(WM_SETFOCUS, 0, 0);
			break;
		}
		case SDL_EVENT_WINDOW_FOCUS_LOST:
		{
			OnWndMessage(WM_KILLFOCUS, 0, 0);
			break;
		}
		case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
		{
			OnWndMessage(WM_CLOSE, 0, 0);
			break;
		}
		case SDL_EVENT_WINDOW_DESTROYED:
		{
			OnWndMessage(WM_DESTROY, 0, 0);
			break;
		}
		default:
		{
			break;
		}
	}

	return;
}

void CDUIWndSDL::OnSdlMouseEvent(const SDL_Event &e)
{
	const UINT uKeyState = MapWndKeyState();
	int x = 0;
	int y = 0;

	auto EndWndMoving = [this]()
	{
		if (false == m_bWndMoving) return;
		m_bWndMoving = false;
		Invalidate();
	};

	switch (e.type)
	{
		case SDL_EVENT_MOUSE_MOTION:
		{
			// HITTEST_DRAGGABLE may not deliver BUTTON_UP; clear moving when LMB released.
			if (0 == (e.motion.state & SDL_BUTTON_LMASK)) EndWndMoving();
			x = (int)e.motion.x;
			y = (int)e.motion.y;
			OnWndMessage(WM_MOUSEMOVE, uKeyState, MAKELPARAM(x, y));
			break;
		}
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		{
			x = (int)e.button.x;
			y = (int)e.button.y;
			if (SDL_BUTTON_LEFT == e.button.button)
			{
				OnWndMessage((e.button.clicks >= 2) ? WM_LBUTTONDBLCLK : WM_LBUTTONDOWN, uKeyState, MAKELPARAM(x, y));
			}
			else if (SDL_BUTTON_RIGHT == e.button.button)
			{
				OnWndMessage((e.button.clicks >= 2) ? WM_RBUTTONDBLCLK : WM_RBUTTONDOWN, uKeyState, MAKELPARAM(x, y));
			}
			else if (SDL_BUTTON_MIDDLE == e.button.button)
			{
				OnWndMessage(WM_MBUTTONDOWN, uKeyState, MAKELPARAM(x, y));
			}

			break;
		}
		case SDL_EVENT_MOUSE_BUTTON_UP:
		{
			x = (int)e.button.x;
			y = (int)e.button.y;
			if (SDL_BUTTON_LEFT == e.button.button)
			{
				OnWndMessage(WM_LBUTTONUP, uKeyState, MAKELPARAM(x, y));
				EndWndMoving();
			}
			else if (SDL_BUTTON_RIGHT == e.button.button)
			{
				OnWndMessage(WM_RBUTTONUP, uKeyState, MAKELPARAM(x, y));

				// Win32 DefWindowProc synthesizes WM_CONTEXTMENU after RBUTTONUP (screen coords)
				POINT ptScreen = { x, y };
				ClientToScreen(m_hWnd, &ptScreen);
				OnWndMessage(WM_CONTEXTMENU, (WPARAM)m_hWnd, MAKELPARAM(ptScreen.x, ptScreen.y));
			}
			else if (SDL_BUTTON_MIDDLE == e.button.button)
			{
				OnWndMessage(WM_MBUTTONUP, uKeyState, MAKELPARAM(x, y));
			}

			break;
		}
		case SDL_EVENT_MOUSE_WHEEL:
		{
			float fGlobalX = 0.0f;
			float fGlobalY = 0.0f;
			SDL_GetGlobalMouseState(&fGlobalX, &fGlobalY);
			x = (int)fGlobalX;
			y = (int)fGlobalY;
			const int nDelta = (int)(e.wheel.y * WHEEL_DELTA);
			OnWndMessage(WM_MOUSEWHEEL, MAKEWPARAM(uKeyState, nDelta), MAKELPARAM(x, y));
			break;
		}
		default:
		{
			break;
		}
	}

	return;
}

void CDUIWndSDL::OnSdlKeyEvent(const SDL_Event &e)
{
	const UINT uVK = SdlKeycodeToVK(e.key.key);
	const bool bSys = (0 != (e.key.mod & SDL_KMOD_ALT));

	if (SDL_EVENT_KEY_DOWN == e.type)
	{
		OnWndMessage(bSys ? WM_SYSKEYDOWN : WM_KEYDOWN, uVK, 0);

		const bool bApps = (SDLK_APPLICATION == e.key.key || SDLK_MENU == e.key.key);
		const bool bShiftF10 = (SDLK_F10 == e.key.key && 0 != (e.key.mod & SDL_KMOD_SHIFT));
		if (bApps || bShiftF10)
		{
			POINT ptScreen = {};
			GetCursorPos(&ptScreen);
			OnWndMessage(WM_CONTEXTMENU, (WPARAM)m_hWnd, MAKELPARAM(ptScreen.x, ptScreen.y));
		}
	}
	else if (SDL_EVENT_KEY_UP == e.type)
	{
		OnWndMessage(bSys ? WM_SYSKEYUP : WM_KEYUP, uVK, 0);
	}

	return;
}

UINT CDUIWndSDL::SdlKeycodeToVK(SDL_Keycode key)
{
	switch (key)
	{
		case SDLK_RETURN: return VK_RETURN;
		case SDLK_ESCAPE: return VK_ESCAPE;
		case SDLK_BACKSPACE: return VK_BACK;
		case SDLK_TAB: return VK_TAB;
		case SDLK_SPACE: return VK_SPACE;
		case SDLK_LEFT: return VK_LEFT;
		case SDLK_RIGHT: return VK_RIGHT;
		case SDLK_UP: return VK_UP;
		case SDLK_DOWN: return VK_DOWN;
		case SDLK_HOME: return VK_HOME;
		case SDLK_END: return VK_END;
		case SDLK_PAGEUP: return VK_PRIOR;
		case SDLK_PAGEDOWN: return VK_NEXT;
		case SDLK_DELETE: return VK_DELETE;
		case SDLK_LCTRL:
		case SDLK_RCTRL: return VK_CONTROL;
		case SDLK_LSHIFT:
		case SDLK_RSHIFT: return VK_SHIFT;
		case SDLK_LALT:
		case SDLK_RALT: return VK_MENU;
		default:
		{
			if (key >= SDLK_A && key <= SDLK_Z)
			{
				return (UINT)('A' + (key - SDLK_A));
			}
			if (key >= SDLK_0 && key <= SDLK_9)
			{
				return (UINT)('0' + (key - SDLK_0));
			}
			if (key >= SDLK_F1 && key <= SDLK_F12)
			{
				return (UINT)(VK_F1 + (key - SDLK_F1));
			}

			return (UINT)key;
		}
	}
}

SDL_HitTestResult SDLCALL CDUIWndSDL::SDLEnableHitTest(SDL_Window *win, const SDL_Point *pt, void *userdata)
{
	CDUIWndSDL *pWnd = static_cast<CDUIWndSDL *>(userdata);
	if (!pWnd || !IsWindow(pWnd->m_hWnd)) return SDL_HITTEST_NORMAL;

	int x = pt->x;
	int y = pt->y;
	CDUIPoint cpt(x, y);
	CDUIRect rcClient = pWnd->GetClientRect();
	SDL_HitTestResult nHitTest = SDL_HITTEST_NORMAL;

	if (false == IsZoomed(pWnd->m_hWnd) && rcClient.PtInRect(cpt))
	{
		RECT rcSizeBox = pWnd->GetResizeTrack();
		if (y < rcClient.top + rcSizeBox.top)
		{
			if (x < rcClient.left + rcSizeBox.left) nHitTest = SDL_HITTEST_RESIZE_TOPLEFT;
			else if (x > rcClient.right - rcSizeBox.right) nHitTest = SDL_HITTEST_RESIZE_TOPRIGHT;
			else nHitTest = SDL_HITTEST_RESIZE_TOP;
		}
		else if (y > rcClient.bottom - rcSizeBox.bottom)
		{
			if (x < rcClient.left + rcSizeBox.left) nHitTest = SDL_HITTEST_RESIZE_BOTTOMLEFT;
			else if (x > rcClient.right - rcSizeBox.right) nHitTest = SDL_HITTEST_RESIZE_BOTTOMRIGHT;
			else nHitTest = SDL_HITTEST_RESIZE_BOTTOM;
		}
		else if (x < rcClient.left + rcSizeBox.left) nHitTest = SDL_HITTEST_RESIZE_LEFT;
		else if (x > rcClient.right - rcSizeBox.right) nHitTest = SDL_HITTEST_RESIZE_RIGHT;
	}

	if (SDL_HITTEST_NORMAL == nHitTest)
	{
		CDUIRect rcCaption(0, 0, rcClient.right, pWnd->GetCaptionHeight());
		if (rcCaption.PtInRect(cpt))
		{
			CDUIControlBase *pControl = pWnd->FindControl(cpt);
			if (NULL == pControl
				|| (NULL == MMInterfaceHelper(CDUIButtonCtrl, pControl) && NULL == MMInterfaceHelper(CDUIProgressCtrl, pControl)))
			{
				nHitTest = SDL_HITTEST_DRAGGABLE;
			}
		}
	}

	// Non-NORMAL + LMB: compositor is taking a title-bar drag or border resize.
	// Do not infer this from WINDOW_MOVED — SetWindowPos also emits MOVED.
	if (SDL_HITTEST_NORMAL != nHitTest
		&& 0 != (SDL_GetGlobalMouseState(NULL, NULL) & SDL_BUTTON_LMASK))
	{
		pWnd->m_bWndMoving = true;
	}

	return nHitTest;
}

bool SDLCALL CDUIWndSDL::SDLEventWatch(void *userdata, SDL_Event *e)
{
	CDUIWndSDL *self = static_cast<CDUIWndSDL *>(userdata);
	if (NULL == self || NULL == e) return false;

	const Uint32 uAsyncEvent = MMSdlGetAsyncEventType();
	if (uAsyncEvent != 0 && e->type == uAsyncEvent)
	{
		tagMMSdlAsyncMsg *pAsyncMsg = static_cast<tagMMSdlAsyncMsg *>(e->user.data1);
		if (pAsyncMsg && pAsyncMsg->pWnd == self)
		{
			self->OnWndMessage(pAsyncMsg->uMsg, pAsyncMsg->wParam, pAsyncMsg->lParam);
			MMSafeDelete(pAsyncMsg);
			e->user.data1 = NULL;
		}

		return false;
	}

	SDL_WindowID uWndID = 0;
	if (e->type >= SDL_EVENT_WINDOW_FIRST && e->type <= SDL_EVENT_WINDOW_LAST)
	{
		uWndID = e->window.windowID;
	}
	else if (e->type == SDL_EVENT_MOUSE_MOTION || e->type == SDL_EVENT_MOUSE_BUTTON_DOWN
		|| e->type == SDL_EVENT_MOUSE_BUTTON_UP || e->type == SDL_EVENT_MOUSE_WHEEL)
	{
		uWndID = e->motion.windowID;
	}
	else if (e->type == SDL_EVENT_KEY_DOWN || e->type == SDL_EVENT_KEY_UP)
	{
		uWndID = e->key.windowID;
	}
	else if (e->type == SDL_EVENT_TEXT_EDITING)
	{
		uWndID = e->edit.windowID;
	}
	else if (e->type == SDL_EVENT_TEXT_INPUT)
	{
		uWndID = e->text.windowID;
	}
	else
	{
		return false;
	}

	if (uWndID != self->m_uWndID) return false;

	switch (e->type)
	{
		case SDL_EVENT_WINDOW_SHOWN:
		case SDL_EVENT_WINDOW_HIDDEN:
		case SDL_EVENT_WINDOW_EXPOSED:
		case SDL_EVENT_WINDOW_MOVED:
		case SDL_EVENT_WINDOW_RESIZED:
		case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
		case SDL_EVENT_WINDOW_MINIMIZED:
		case SDL_EVENT_WINDOW_MAXIMIZED:
		case SDL_EVENT_WINDOW_RESTORED:
		case SDL_EVENT_WINDOW_FOCUS_GAINED:
		case SDL_EVENT_WINDOW_FOCUS_LOST:
		case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
		case SDL_EVENT_WINDOW_DESTROYED:
		{
			self->OnSdlWindowEvent(*e);
			break;
		}
		case SDL_EVENT_MOUSE_MOTION:
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		case SDL_EVENT_MOUSE_BUTTON_UP:
		case SDL_EVENT_MOUSE_WHEEL:
		{
			self->OnSdlMouseEvent(*e);
			break;
		}
		case SDL_EVENT_KEY_DOWN:
		case SDL_EVENT_KEY_UP:
		{
			self->OnSdlKeyEvent(*e);
			break;
		}
		case SDL_EVENT_TEXT_EDITING:
		{
			if (self->m_pFocusCtrl && e->edit.text)
			{
				DuiMessage Msg = {};
				Msg.strText = CA2CT(e->edit.text, CP_UTF8);
				self->m_pFocusCtrl->OnDuiTextEditing(Msg);
			}

			break;
		}
		case SDL_EVENT_TEXT_INPUT:
		{
			if (self->m_pFocusCtrl && e->text.text)
			{
				DuiMessage Msg = {};
				Msg.strText = CA2CT(e->text.text, CP_UTF8);
				self->m_pFocusCtrl->OnDuiTextInput(Msg);
			}

			break;
		}
		default:
		{
			break;
		}
	}

	return false;
}

void CDUIWndSDL::ForegroundWindow(HWND hWnd)
{
	// SDL 下 hWnd 实际为 SDL_Window*
	if (hWnd == nullptr) return;

	// 尝试显示并提升窗口：这是 SDL 上最直接的跨平台方法。
	// 注意：许多 Linux 窗口管理器有防止抢焦点的策略，可能无法强制获得键盘焦点。
	SDL_ShowWindow(hWnd);
	SDL_RaiseWindow(hWnd);

	// 有些平台上可以尝试额外手段（例如通过原生 API），但那会引入平台特定代码。
	return;
}

//////////////////////////////////////////////////////////////////////////
#endif