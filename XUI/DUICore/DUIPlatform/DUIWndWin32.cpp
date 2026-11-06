#include "stdafx.h"
#include "DUIWndWin32.h"

#ifndef DuiPlatform_SDL

//////////////////////////////////////////////////////////////////////////
#define GetPIDLFolder(pida)				(LPCITEMIDLIST)(((LPBYTE)pida)+(pida)->aoffset[0])
#define GetPIDLItem(pida, i)			(LPCITEMIDLIST)(((LPBYTE)pida)+(pida)->aoffset[i+1])
#define Dui_ControlDrag					_T("DUIThinkControlDrag")

//////////////////////////////////////////////////////////////////////////
CDUIWndWin32::CDUIWndWin32(LPCTSTR lpszDuiName, HWND hWndParent)
	: CDUIWndBase(lpszDuiName, hWndParent)
	, m_OldWndProc(::DefWindowProc)
{
}

CDUIWndWin32::~CDUIWndWin32()
{
	ReleasePaintScene();

	//release dc
	if (IsWindow(m_hWnd))
	{
		if (m_hDCPaint)
		{
			::ReleaseDC(m_hWnd, m_hDCPaint);
			m_hDCPaint = NULL;
		}
	}

	//unsub window
	UnSubWindow();

	//close window
	if (IsWindow(m_hWnd))
	{
		Close(Dui_CtrlIDInner_BtnCancel);
		::SetWindowLongPtr(m_hWnd, GWLP_USERDATA, NULL);
	}

	return;
}

#if (NTDDI_VERSION >= NTDDI_VISTA)
void CDUIWndWin32::OnWinDragEnter(IDataObject *pIDataObject, DWORD dwKeyState, POINTL pt, DWORD *pdwEffect)
{
	m_DropData.vecDropFiles.clear();
	m_DropData.vecFileNameMap.clear();
	m_DropData.bHasGUID = false;
	for (auto &it : m_DropData.vecPIDL)
	{
		::CoTaskMemFree((LPVOID)it);
	}

	m_DropData.vecPIDL.clear();

	// parse IDataObject
	CDUIPoint ptMouse = { pt.x, pt.y };
	ScreenToClient(m_hWnd, &ptMouse);
	m_ptMousePosLast = ptMouse;
	m_DropData.ptMouse = ptMouse;
	m_DropData.pIDataObject = pIDataObject;
	m_DropData.dwKeyState = dwKeyState;

	FORMATETC ftetc = { 0 };
	ftetc.lindex = -1;
	ftetc.dwAspect = DVASPECT_CONTENT;

	//CFSTR_PREFERREDDROPEFFECT
	ftetc.cfFormat = CMMDragDrop::RegisterFormat(CFSTR_PREFERREDDROPEFFECT);
	ftetc.tymed = TYMED_HGLOBAL;

	//parse dropfiles
	ftetc.cfFormat = CF_HDROP;
	ftetc.tymed = TYMED_HGLOBAL;
	STGMEDIUM medium = { 0 };
	if (S_OK == pIDataObject->GetData(&ftetc, &medium))
	{
		HDROP hDrop = (HDROP)GlobalLock(medium.hGlobal);
		if (hDrop)
		{
			UINT cFiles = DragQueryFile(hDrop, 0xFFFFFFFF, nullptr, 0);
			if (cFiles > 0)
			{
				for (int i = 0; i < cFiles; i++)
				{
					TCHAR szFileName[MAX_PATH] = { 0 };
					DragQueryFile(hDrop, i, szFileName, sizeof(szFileName));
					m_DropData.vecDropFiles.push_back(szFileName);
				}
			}
		}

		GlobalUnlock(medium.hGlobal);
		ReleaseStgMedium(&medium);
	}

	//parse FileNameMap
	ftetc.cfFormat = (CLIPFORMAT)RegisterClipboardFormat(CFSTR_FILENAMEMAP);
	ftetc.tymed = TYMED_HGLOBAL;
	if (S_OK == pIDataObject->GetData(&ftetc, &medium))
	{
		HDROP hDrop = (HDROP)GlobalLock(medium.hGlobal);
		if (hDrop != NULL)
		{
			LPCWSTR s = reinterpret_cast<LPCWSTR>(hDrop);
			while (*s)
			{
				CMMString strFileName = CMMStrHelp::Format(_T("%ls"), s);
				m_DropData.vecFileNameMap.push_back(strFileName);
				s = 1 + wcschr(s, L'\0');
			}

			GlobalUnlock(medium.hGlobal);
			ReleaseStgMedium(&medium);

			if (m_DropData.vecFileNameMap.size() > 0)
			{
				if (m_DropData.vecFileNameMap.size() != m_DropData.vecDropFiles.size())
				{
					m_DropData.vecFileNameMap.clear();
				}
			}
		}
	}

	// parse pidl
	ftetc.cfFormat = CMMDragDrop::RegisterFormat(CFSTR_SHELLIDLIST);
	ftetc.tymed = TYMED_HGLOBAL;
	STGMEDIUM mediumPIDL = { 0 };
	if (S_OK == pIDataObject->GetData(&ftetc, &mediumPIDL))
	{
		LPIDA pida = (LPIDA)GlobalLock(mediumPIDL.hGlobal);
		PCIDLIST_ABSOLUTE pidl_parent = GetPIDLFolder(pida);
		for (int i = 0; i < pida->cidl; i++)
		{
			PCIDLIST_ABSOLUTE pcid = ILCombine(pidl_parent, GetPIDLItem(pida, i));
			m_DropData.vecPIDL.push_back(pcid);

			TCHAR szPath[MAX_PATH] = { 0 };
			SHGetPathFromIDList(pcid, szPath);
			if (lstrlen(szPath) == 0)
			{
				SHDESCRIPTIONID shdescid = { 0 };
				CComPtr<IShellFolder> psfParent = nullptr;
				PCUITEMID_CHILD lpidRelative = nullptr;
				HRESULT hr = SHBindToParent(pcid, IID_IShellFolder, (void**)&psfParent, &lpidRelative);
				if (hr != S_OK)
					continue;

				hr = SHGetDataFromIDList(psfParent, lpidRelative, SHGDFIL_DESCRIPTIONID, &shdescid, sizeof(SHDESCRIPTIONID));

				if (hr != S_OK)
					continue;

				LPOLESTR pszStr = NULL;
				hr = ::StringFromCLSID(shdescid.clsid, &pszStr);
				if (hr != S_OK)
					continue;

				CMMString strPath = pszStr;
				auto itor = std::find_if(m_DropData.vecDropFiles.begin(), m_DropData.vecDropFiles.end(), [&](CMMString& strTemp) {
					return strTemp.CompareNoCase(strPath) == 0;
				});
				if (!m_DropData.bHasGUID)
				{
					m_DropData.bHasGUID = true;
				}
				if (itor == m_DropData.vecDropFiles.end())
					m_DropData.vecDropFiles.push_back(strPath);
			}
		}

		GlobalUnlock(mediumPIDL.hGlobal);
		ReleaseStgMedium(&mediumPIDL);
	}

	// parse text
	ftetc.cfFormat = CMMDragDrop::RegisterFormat(Dui_ControlDrag);
	ftetc.tymed = TYMED_HGLOBAL;
	STGMEDIUM mediumText = { 0 };
	if (S_OK == pIDataObject->GetData(&ftetc, &mediumText))
	{
		PWSTR pzStr = (PWSTR)GlobalLock(mediumText.hGlobal);

		GlobalUnlock(mediumText.hGlobal);
		ReleaseStgMedium(&mediumText);
	}

	//notify
	m_pWinDragEnterCtrl = FindControlByDrop(ptMouse);
	if (m_pWinDragEnterCtrl)
	{
		m_pWinDragEnterCtrl->OnWinDragEnter(&m_DropData, &m_DropData.dwEffect);
	}

	return;
}

void CDUIWndWin32::OnWinDragOver(DWORD dwKeyState, POINTL pt, DWORD *pdwEffect)
{
	if (NULL == m_pRootCtrl) return;

	POINT ptMouse = { pt.x, pt.y };
	ScreenToClient(m_hWnd, &ptMouse);
	if (m_DropData.ptMouse == ptMouse
		&& m_DropData.dwKeyState == dwKeyState)
	{
		*pdwEffect = m_DropData.dwEffect;

		return;
	}

	m_DropData.dwKeyState = dwKeyState;
	m_DropData.ptMouse = ptMouse;
	m_ptMousePosLast = ptMouse;

	CDUIControlBase *pOverControl = FindControlByDrop(ptMouse);

	//leave
	if (pOverControl != m_pWinDragEnterCtrl)
	{
		if (m_pWinDragEnterCtrl
			&& m_pRootCtrl->VerifyChild(m_pWinDragEnterCtrl))
		{
			m_pWinDragEnterCtrl->OnWinDragLeave(&m_DropData);
		}

		m_pWinDragEnterCtrl = NULL;
	}
	//enter
	if (pOverControl != m_pWinDragEnterCtrl && pOverControl)
	{
		m_pWinDragEnterCtrl = pOverControl;
		pOverControl->OnWinDragEnter(&m_DropData, &m_DropData.dwEffect);
	}
	//over
	if (m_pWinDragEnterCtrl)
	{
		m_pWinDragEnterCtrl->OnWinDragOver(&m_DropData, m_DropData.dwEffect);
	}

	*pdwEffect = m_DropData.dwEffect;

	return;
}

void CDUIWndWin32::OnWinDragLeave()
{
	if (NULL == m_pRootCtrl) return;

	m_DropData.ptMouse = m_ptMousePosLast;
	if (m_pWinDragEnterCtrl && m_pRootCtrl->VerifyChild(m_pWinDragEnterCtrl))
	{
		m_pWinDragEnterCtrl->OnWinDragLeave(&m_DropData);
	}
	for (auto& it : m_DropData.vecPIDL)
	{
		::CoTaskMemFree((LPVOID)it);
	}

	m_pWinDragEnterCtrl = NULL;
	m_DropData = {};

	return;
}

void CDUIWndWin32::OnWinDrop(IDataObject *pIDataObject, POINTL pt, DWORD *pdwEffect)
{
	if (NULL == m_pRootCtrl) return;

	POINT ptMouse = { pt.x, pt.y };
	ScreenToClient(m_hWnd, &ptMouse);
	m_ptMousePosLast = ptMouse;
	m_DropData.pIDataObject = pIDataObject;
	m_DropData.ptMouse = ptMouse;

	//从外部拖拽时查询数据对象是否支持异步操作
	if (m_DropData.pDragDropFrom != this)
	{
		//0x602 win7 
#if (_WIN32_WINNT >= 0x602)
		IDataObjectAsyncCapability *pac = NULL;
#else
		IAsyncOperation *pac = NULL;
#endif

		BOOL bAsyncSupported = FALSE;
		/* Does the drop source support asynchronous copy */
		HRESULT hr = pIDataObject->QueryInterface(IID_PPV_ARGS(&pac));
		if (hr == S_OK && pac != NULL)
		{
			pac->GetAsyncMode(&bAsyncSupported);
			pac->Release();
			pac = NULL;
			m_DropData.bAsyncMode = (bAsyncSupported == TRUE);
		}
		else
		{
			m_DropData.bAsyncMode = false;
		}
	}

	//兼容从压缩包中直接拖拽文件，再parse下 
	m_DropData.vecDropFiles.clear();
	FORMATETC ftetc = { 0 };
	ftetc.lindex = -1;
	ftetc.dwAspect = DVASPECT_CONTENT;
	ftetc.cfFormat = CF_HDROP;
	ftetc.tymed = TYMED_HGLOBAL;
	STGMEDIUM medium = { 0 };
	if (S_OK == pIDataObject->GetData(&ftetc, &medium))
	{
		HDROP hDrop = (HDROP)GlobalLock(medium.hGlobal);
		if (hDrop)
		{
			UINT cFiles = DragQueryFile(hDrop, 0xFFFFFFFF, nullptr, 0);
			if (cFiles > 0)
			{
				for (int i = 0; i < cFiles; i++)
				{
					TCHAR szFileName[MAX_PATH] = { 0 };
					DragQueryFile(hDrop, i, szFileName, sizeof(szFileName));
					m_DropData.vecDropFiles.push_back(szFileName);
				}
			}
		}

		GlobalUnlock(medium.hGlobal);
		ReleaseStgMedium(&medium);
	}

	//notify
	if (m_pWinDragEnterCtrl && m_pRootCtrl->VerifyChild(m_pWinDragEnterCtrl))
	{
		m_pWinDragEnterCtrl->OnWinDrop(&m_DropData, pdwEffect);
	}

	m_pWinDragEnterCtrl = NULL;

	return;
}
#endif

LPVOID CDUIWndWin32::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIWndWin32, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIWndWin32::GetDescribe() const
{
	return Dui_WindowBase;
}

UINT CDUIWndWin32::MapWndKeyState()
{
	UINT uState = 0;
	if (::GetKeyState(VK_CONTROL) < 0) uState |= MK_CONTROL;
	if (::GetKeyState(VK_RBUTTON) < 0) uState |= MK_RBUTTON;
	if (::GetKeyState(VK_LBUTTON) < 0) uState |= MK_LBUTTON;
	if (::GetKeyState(VK_MBUTTON) < 0) uState |= MK_MBUTTON;
	if (::GetKeyState(VK_SHIFT) < 0) uState |= MK_SHIFT;
	if (::GetKeyState(VK_MENU) < 0) uState |= MK_ALT;

	return uState;
}

HWND CDUIWndWin32::Create(HWND hWndParent, LPCTSTR lpszName, DWORD dwStyle, DWORD dwExStyle, int x, int y, int cx, int cy)
{
	if (GetSuperClassName() != NULL && !RegisterSuperclass()) return NULL;
	if (GetSuperClassName() == NULL && !RegisterWindowClass()) return NULL;
	HINSTANCE hInstance = CDUIGlobal::GetInstance()->GetInstanceHandle();
	m_hWndParent = hWndParent;
	m_ptCreate = { x, y };
	m_hWnd = ::CreateWindowEx(dwExStyle, GetClass(), lpszName, dwStyle, x, y, cx, cy, hWndParent, NULL, hInstance, MMStaticPtr(CDUIWndBase, this));
	ASSERT(m_hWnd != NULL);
	m_uWndID = (UINT)m_hWnd;
	return m_hWnd;
}

HWND CDUIWndWin32::SubWindow(HWND hWnd)
{
	ASSERT(::IsWindow(hWnd));
	ASSERT(m_hWnd == NULL);
	m_OldWndProc = SubclassWindow(hWnd, __WndProc);
	if (m_OldWndProc == NULL) return NULL;
	m_bSubWindow = true;
	m_hWnd = hWnd;
	::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(this));
	return m_hWnd;
}

void CDUIWndWin32::UnSubWindow()
{
	if (!::IsWindow(m_hWnd)) return;
	if (!m_bSubWindow) return;
	SubclassWindow(m_hWnd, m_OldWndProc);
	m_OldWndProc = ::DefWindowProc;
	m_bSubWindow = false;
	return;
}

void CDUIWndWin32::ShowWindow(bool bShow /*= true*/, bool bTakeFocus /*= false*/)
{
	ASSERT(::IsWindow(m_hWnd));
	if (!::IsWindow(m_hWnd)) return;
	::ShowWindow(m_hWnd, bShow ? (bTakeFocus ? SW_SHOW : SW_SHOWNA) : SW_HIDE);

	return;
}

UINT CDUIWndWin32::DoModal()
{
	//create
	if (false == IsWindow(m_hWnd))
	{
		Create(m_hWndParent, _T(""), DUI_WNDSTYLE_DIALOG, DUI_WNDSTYLE_EX_DIALOG);
	}

	//show
	ShowWindow(true, false);

	//disable parent
	UINT nRet = 0;
	HWND hWndParent = GetWindowOwner(m_hWnd);
	bool bWndParentEnable = IsWindowEnabled(hWndParent);
	::EnableWindow(hWndParent, false);

	//message
	BOOL bRet = 0;
	MSG Msg = {};
	while (::IsWindowVisible(m_hWnd) && (bRet = GetMessage(&Msg, NULL, 0, 0)) != 0)
	{
		if (-1 == bRet)
		{
			assert(false);
			MMTRACE(_T("EXCEPTION: %s(%d)\n"), __FILET__, __LINE__);

			break;
		}
		if (Msg.message == WM_CLOSE && Msg.hwnd == m_hWnd)
		{
			nRet = Msg.wParam;
			::EnableWindow(hWndParent, bWndParentEnable);
			::SetFocus(hWndParent);
		}
		if (false == CDUIGlobal::GetInstance()->TranslateMessage(&Msg))
		{
			::TranslateMessage(&Msg);
			::DispatchMessage(&Msg);
		}
		if (Msg.message == WM_QUIT) break;
	}

	::EnableWindow(hWndParent, bWndParentEnable);
	::SetFocus(hWndParent);
	if (Msg.message == WM_QUIT) ::PostQuitMessage(Msg.wParam);

	return nRet;
}

UINT CDUIWndWin32::DoBlock()
{
	//create
	if (false == IsWindow(m_hWnd))
	{
		Create(m_hWndParent, _T(""), DUI_WNDSTYLE_DIALOG, DUI_WNDSTYLE_EX_DIALOG);
	}

	//show
	ShowWindow(true, false);

	//message
	UINT nRet = 0;
	BOOL bRet = 0;
	MSG Msg = {};
	while (::IsWindowVisible(m_hWnd) && (bRet = GetMessage(&Msg, NULL, 0, 0)) != 0)
	{
		if (-1 == bRet)
		{
			assert(false);
			MMTRACE(_T("EXCEPTION: %s(%d)\n"), __FILET__, __LINE__);

			break;
		}
		if (Msg.message == WM_CLOSE && Msg.hwnd == m_hWnd)
		{
			nRet = Msg.wParam;
		}
		if (false == CDUIGlobal::GetInstance()->TranslateMessage(&Msg))
		{
			::TranslateMessage(&Msg);
			::DispatchMessage(&Msg);
		}
		if (Msg.message == WM_QUIT) break;
	}
	if (Msg.message == WM_QUIT) ::PostQuitMessage(Msg.wParam);

	return nRet;
}

void CDUIWndWin32::CenterWindow()
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT((GetWindowStyle(m_hWnd) & WS_CHILD) == 0);
	RECT rcDlg = {};
	::GetWindowRect(m_hWnd, &rcDlg);
	RECT rcArea = {};
	RECT rcCenter = {};
	HWND hWndCenter = ::GetWindowOwner(m_hWnd);

	// 处理多显示器模式下屏幕居中
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(hWndCenter, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	rcArea = oMonitor.rcWork;

	if (hWndCenter == NULL || ::IsIconic(hWndCenter))
		rcCenter = rcArea;
	else
		::GetWindowRect(hWndCenter, &rcCenter);

	int DlgWidth = rcDlg.right - rcDlg.left;
	int DlgHeight = rcDlg.bottom - rcDlg.top;

	// Find dialog's upper left based on rcCenter
	int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
	int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;

	// The dialog is outside the screen, move it inside
	if (xLeft < rcArea.left) xLeft = rcArea.left;
	else if (xLeft + DlgWidth > rcArea.right) xLeft = rcArea.right - DlgWidth;
	if (yTop < rcArea.top) yTop = rcArea.top;
	else if (yTop + DlgHeight > rcArea.bottom) yTop = rcArea.bottom - DlgHeight;
	::SetWindowPos(m_hWnd, NULL, xLeft, yTop, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);

	return;
}

void CDUIWndWin32::SetIcon(UINT nRes)
{
	HINSTANCE hInstance = CDUIGlobal::GetInstance()->GetInstanceHandle();
	HICON hIcon = (HICON)::LoadImage(hInstance, MAKEINTRESOURCE(nRes), IMAGE_ICON,
		(::GetSystemMetrics(SM_CXICON) + 15) & ~15, (::GetSystemMetrics(SM_CYICON) + 15) & ~15,	// 防止高DPI下图标模糊
		LR_DEFAULTCOLOR);
	ASSERT(hIcon);
	::SendMessage(m_hWnd, WM_SETICON, (WPARAM)TRUE, (LPARAM)hIcon);

	hIcon = (HICON)::LoadImage(hInstance, MAKEINTRESOURCE(nRes), IMAGE_ICON,
		(::GetSystemMetrics(SM_CXICON) + 15) & ~15, (::GetSystemMetrics(SM_CYICON) + 15) & ~15,	// 防止高DPI下图标模糊
		LR_DEFAULTCOLOR);
	ASSERT(hIcon);
	::SendMessage(m_hWnd, WM_SETICON, (WPARAM)false, (LPARAM)hIcon);

	return;
}

LRESULT CDUIWndWin32::SendMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
	ASSERT(::IsWindow(m_hWnd));
	return ::SendMessage(m_hWnd, uMsg, wParam, lParam);
}

LRESULT CDUIWndWin32::PostMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
	ASSERT(::IsWindow(m_hWnd));
	return ::PostMessage(m_hWnd, uMsg, wParam, lParam);
}

void CDUIWndWin32::SetCapture()
{
	::SetCapture(m_hWnd);
}

void CDUIWndWin32::ReleaseCapture()
{
	::ReleaseCapture();
}

bool CDUIWndWin32::IsCaptured()
{
	return ::GetCapture() == m_hWnd;
}

void CDUIWndWin32::SetFocusControl(CDUIControlBase *pFocusCtrl)
{
	__super::SetFocusControl(pFocusCtrl);

	//focus child wnd
	MMInterfaceHelper(CDUIEditCtrl, m_pFocusCtrl, pEditFocus);
	MMInterfaceHelper(CDUIHotKeyCtrl, m_pFocusCtrl, pHotKeyFocus);

	if (m_pFocusCtrl
		&& GetFocus() != m_hWnd
		&& NULL == pEditFocus
		&& NULL == pHotKeyFocus)
	{
		::SetFocus(m_hWnd);
	}

	return;
}

CDUIContainerCtrl * CDUIWndWin32::DetachRootCtrl()
{
	if (IsWindow(m_hWndTooltip))
	{
		DestroyWindow(m_hWndTooltip);
	}

	m_hWndTooltip = NULL;

	return __super::DetachRootCtrl();
}

void CDUIWndWin32::RefreshToolTip(CMMString strToolTip)
{
	m_bRefreshToolTipNeeded = false;
	m_strToolTip = strToolTip;

	HINSTANCE hInst = CDUIGlobal::GetInstance()->GetInstanceHandle();
	::ZeroMemory(&m_ToolTip, sizeof(m_ToolTip));
	m_ToolTip.cbSize = sizeof(m_ToolTip);
	m_ToolTip.uFlags = TTF_IDISHWND;
	m_ToolTip.hwnd = m_hWnd;
	m_ToolTip.uId = (UINT_PTR)m_hWnd;
	m_ToolTip.hinst = hInst;
	m_ToolTip.lpszText = const_cast<LPTSTR>((LPCTSTR)strToolTip);
	m_ToolTip.rect = m_pHoverCtrl->GetAbsoluteRect();

	if (NULL == m_hWndTooltip)
	{
		m_hWndTooltip = ::CreateWindowEx(0, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, m_hWnd, NULL, hInst, NULL);
		::SendMessage(m_hWndTooltip, TTM_ADDTOOL, 0, (LPARAM)&m_ToolTip);
	}
	::SendMessage(m_hWndTooltip, TTM_SETMAXTIPWIDTH, 0, m_pHoverCtrl->GetToolTipWidth());
	::SendMessage(m_hWndTooltip, TTM_SETTIPBKCOLOR, RGB(DUIARGBGetR(m_pHoverCtrl->GetToolTipBkColor()), DUIARGBGetG(m_pHoverCtrl->GetToolTipBkColor()), DUIARGBGetB(m_pHoverCtrl->GetToolTipBkColor())), 0);
	::SendMessage(m_hWndTooltip, TTM_SETTIPTEXTCOLOR, RGB(DUIARGBGetR(m_pHoverCtrl->GetToolTipTextColor()), DUIARGBGetG(m_pHoverCtrl->GetToolTipTextColor()), DUIARGBGetB(m_pHoverCtrl->GetToolTipTextColor())), 0);
	::SendMessage(m_hWndTooltip, TTM_SETTOOLINFO, 0, (LPARAM)&m_ToolTip);
	::SendMessage(m_hWndTooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&m_ToolTip);

	return;
}

void CDUIWndWin32::ResizeWnd(int cx /*= -1*/, int cy /*= -1*/)
{
	ASSERT(::IsWindow(m_hWnd));

	CDUIRect rcWnd;
	if (!::GetWindowRect(m_hWnd, &rcWnd)) return;
	if (cx != -1) rcWnd.right = rcWnd.left + cx;
	if (cy != -1) rcWnd.bottom = rcWnd.top + cy;
	if (!::AdjustWindowRectEx(&rcWnd, GetWindowStyle(m_hWnd), (!(GetWindowStyle(m_hWnd) & WS_CHILD) && (::GetMenu(m_hWnd) != NULL)), GetWindowExStyle(m_hWnd))) return;
	::SetWindowPos(m_hWnd, NULL, 0, 0, rcWnd.GetWidth(), rcWnd.GetHeight(), SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOACTIVATE);

	return;
}

void CDUIWndWin32::AdjustWndPos()
{
	if (false == IsWindow(m_hWnd)) return;

	CDUIRect rcWnd = GetWindowRect();
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	CDUIRect rcWork = oMonitor.rcWork;
	if (rcWnd.left < rcWork.left)
	{
		rcWnd.Offset(rcWork.left - rcWnd.left, 0);
	}
	if (rcWnd.right > rcWork.right)
	{
		rcWnd.Offset(rcWork.right - rcWnd.right, 0);
	}
	if (rcWnd.top < rcWork.top)
	{
		rcWnd.Offset(0, rcWork.top - rcWnd.top);
	}
	if (rcWnd.bottom > rcWork.bottom)
	{
		rcWnd.Offset(0, rcWork.bottom - rcWnd.bottom);
	}

	MoveWindow(m_hWnd, rcWnd.left, rcWnd.top, rcWnd.GetWidth(), rcWnd.GetHeight(), TRUE);

	return;
}

bool CDUIWndWin32::IsMaximized()
{
	return ::IsZoomed(m_hWnd);
}

bool CDUIWndWin32::IsMinimized()
{
	return ::IsIconic(m_hWnd);
}

CMMRect CDUIWndWin32::GetClientRect()
{
	RECT rcClient = {};
	::GetClientRect(m_hWnd, &rcClient);

	return rcClient;
}

CMMRect CDUIWndWin32::GetWindowRect()
{
	CDUIRect rcWnd;
	::GetWindowRect(m_hWnd, &rcWnd);

	return rcWnd;
}

void CDUIWndWin32::Invalidate()
{
	if (NULL == m_hWnd) return;

	InvalidateRect(m_hWnd, NULL, TRUE);

	return;
}

bool CDUIWndWin32::CreateCaret(HBITMAP hBmp, int nWidth, int nHeight)
{
	if (false == IsWindow(m_hWnd)) return false;
	if (false == ::CreateCaret(m_hWnd, hBmp, nWidth, nHeight)) return false;

	return __super::CreateCaret(hBmp, nWidth, nHeight);
}

void CDUIWndWin32::ShowCaret(bool bShow)
{
	if (bShow == IsShowCaret()) return;

	m_bCaretActive = bShow;

	if (IsShowCaret())
	{
		::ShowCaret(GetWndHandle());

		Invalidate();

		return;
	}

	::HideCaret(GetWndHandle());

	Invalidate();

	return;
}

void CDUIWndWin32::SetCaretPos(CDUIPoint pt)
{
	if (false == ::SetCaretPos(pt.x, pt.y)) return;

	m_rcCaret.Offset(pt.x - m_rcCaret.left, pt.y - m_rcCaret.top);

	return;
}

void CDUIWndWin32::Register(HWND hWnd)
{
	CMMDragDrop::Register(hWnd);

	return;
}

bool CDUIWndWin32::BeginDragDrop(CDUIControlBase *pControl, WPARAM wParam, LPARAM lParam, int nFlag)
{
	if (NULL == pControl) return false;

	m_pWinDragCtrl = pControl;
	m_DropData.wParam = wParam;
	m_DropData.lParam = lParam;
	m_DropData.pDragDropFrom = this;

	CDUIRect rcCtrl = m_pWinDragCtrl->GetAbsoluteRect();

	//1、先获取DataObject
	CComPtr<IDataObject> pIDataObject = nullptr;
	CComPtr<IDropSource> pIDropSource = nullptr;
	SendNotify(m_pWinDragCtrl, DuiNotify_WinDragCreateObj, (WPARAM)&pIDataObject);
	if (NULL == pIDataObject)
	{
		CMMDropSource *pDropSource = new CMMDropSource;
		if (NULL == pDropSource) return false;

		pIDataObject = new CMMDataObject((CMMDropSource*)pDropSource);
		if (NULL == pIDataObject) return false;

		pIDropSource = pDropSource;
	}

	//bitmap
	HBITMAP hBitmapDrag = NULL;
	CDUIPoint ptOffset = {};
	SendNotify(m_pWinDragCtrl, DuiNotify_WinDragCreateBmp, (WPARAM)&hBitmapDrag, (LPARAM)&ptOffset);
	if (NULL == hBitmapDrag)
	{
		hBitmapDrag = CDUIRenderEngine::GenerateBitmap(m_pWinDragCtrl, rcCtrl);
		ptOffset = { rcCtrl.GetWidth() / 2, rcCtrl.GetHeight() / 2 };
	}

	CComPtr<IDragSourceHelper2> pIDragSourceHelper2 = nullptr;
	if (SUCCEEDED(GetDragDropHelper(IID_PPV_ARGS(&pIDragSourceHelper2))))
	{
		BITMAP bmap;
		GetObject(hBitmapDrag, sizeof(BITMAP), &bmap);

		SHDRAGIMAGE di = {};
		di.hbmpDragImage = hBitmapDrag;
		di.sizeDragImage.cx = bmap.bmWidth;
		di.sizeDragImage.cy = bmap.bmHeight;
		di.ptOffset.x = ptOffset.x;
		di.ptOffset.y = ptOffset.y;
		pIDragSourceHelper2->SetFlags(DSH_ALLOWDROPDESCRIPTIONTEXT);
		pIDragSourceHelper2->InitializeFromBitmap(&di, pIDataObject);
	}

	DWORD dwEffectResult;
	HRESULT hRes = ::SHDoDragDrop(NULL, pIDataObject, NULL, nFlag, &dwEffectResult);
	EndDragDrop();

	MMSafeDeleteObject(hBitmapDrag);

	return true;
}

void CDUIWndWin32::EndDragDrop()
{
	SendNotify(m_pRootCtrl, DuiNotify_WinDragFinish);

	for (auto &it : m_DropData.vecPIDL)
	{
		::CoTaskMemFree((LPVOID)it);
	}

	::ReleaseCapture();

	m_DropData = {};
	m_pWinDragCtrl = NULL;
	m_pCaptureCtrl = NULL;

	return;
}

#if (NTDDI_VERSION >= NTDDI_VISTA)
void CDUIWndWin32::SetDropDescription(DROPIMAGETYPE dwDropType, PCWSTR pszMsg, PCWSTR pszInsert)
{
	SetDropTip(m_DropData.pIDataObject, dwDropType, pszMsg, pszInsert);
}
#endif

void CDUIWndWin32::ClearDescription()
{
	SetDropTip(m_DropData.pIDataObject, DROPIMAGE_INVALID, L"", NULL);
}

const tagDuiDropData * CDUIWndWin32::GetDropData()
{
	return &m_DropData;
}

HBITMAP CDUIWndWin32::ProductBitmap(IN std::vector<CDUIControlBase*> vecCtrl, IN CDUIControlBase* pFromCtrl, OUT CDUIRect &rcBitmap, OUT CDUIPoint &ptFromOffset)
{
	for (auto pCtrl : vecCtrl)
	{
		if (NULL == pCtrl) continue;

		CDUIRect rcItem = pCtrl->GetAbsoluteRect();
		::UnionRect(&rcBitmap, &rcItem, &rcBitmap);
	}

	HDC hPaintDC = ::CreateCompatibleDC(GetWndDC());
	HBITMAP hPaintBitmap = ::CreateCompatibleBitmap(GetWndDC(), rcBitmap.GetWidth(), rcBitmap.GetHeight());
	ASSERT(hPaintDC);
	ASSERT(hPaintBitmap);
	HBITMAP hPaintBitmapOld = (HBITMAP)::SelectObject(hPaintDC, hPaintBitmap);

	for (auto pCtrl : vecCtrl)
	{
		if (NULL == pCtrl) continue;

		HBITMAP hBitmap = CDUIRenderEngine::GenerateBitmap(pCtrl, pCtrl->GetAbsoluteRect());
		if (NULL == hBitmap) continue;

		CDUIRect rcPos = pCtrl->GetAbsoluteRect();
		rcPos.Offset(-rcBitmap.left, -rcBitmap.top);

		Gdiplus::Bitmap *pBmp = CDUIRenderEngine::GetAlphaBitmap(hBitmap);
		CDUIRenderEngine::DrawImage(hPaintDC, pBmp, rcPos);
		DeleteObject(hBitmap);
		MMSafeDelete(pBmp);

		if (pCtrl == pFromCtrl)
		{
			ptFromOffset = { rcPos.left + rcPos.GetWidth() / 2, rcPos.top + rcPos.GetHeight() / 2 };
		}
	}

	SelectObject(hPaintDC, hPaintBitmapOld);
	DeleteDC(hPaintDC);

	return hPaintBitmap;
}

LRESULT CDUIWndWin32::OnWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	switch (uMsg)
	{
		case WM_PAINT:
		{
			CDUIRect rcPaint;
			bool bUpdate = ::GetUpdateRect(m_hWnd, &rcPaint, FALSE);

#ifdef DUI_DESIGN
			bUpdate = true;
			if (rcPaint.Empty())
			{
				rcPaint = GetClientRect();
			}
#endif

			if (bUpdate)
			{
				PAINTSTRUCT ps = {};
				::BeginPaint(m_hWnd, &ps);

				lRes = OnPaint(rcPaint);

				::EndPaint(m_hWnd, &ps);
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
			if (lParam == 0)
			{
				return OnCommand(wParam, lParam);
			}

			HWND hWndChild = (HWND)lParam;
			lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);

			return lRes;
		}
		case WM_NOTIFY:
		{
			LPNMHDR lpNMHDR = (LPNMHDR)lParam;
			if (lpNMHDR != NULL) lRes = ::SendMessage(lpNMHDR->hwndFrom, OCM__BASE + uMsg, wParam, lParam);

			return lRes;
		}
		case WM_IME_COMPOSITION:
		{
			return OnImeComPosition(wParam, lParam);
		}
		case WM_CTLCOLOREDIT:
		case WM_CTLCOLORSTATIC:
		{
			// Refer To: http://msdn.microsoft.com/en-us/library/bb761691(v=vs.85).aspx
			// Read-only or disabled edit controls do not send the WM_CTLCOLOREDIT message; instead, they send the WM_CTLCOLORSTATIC message.
			if (lParam == 0) break;

			HWND hWndChild = (HWND)lParam;
			lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
			return lRes;
		}
	}

	return __super::OnWndMessage(uMsg, wParam, lParam);
}

LRESULT CDUIWndWin32::OnOldWndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_OldWndProc) return ::CallWindowProc(m_OldWndProc, m_hWnd, uMsg, wParam, lParam);

	return ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
}

LRESULT CDUIWndWin32::OnClose(WPARAM wParam, LPARAM lParam)
{
	return OnOldWndProc(WM_CLOSE, wParam, lParam);
}

LRESULT CDUIWndWin32::OnNcHitTest(WPARAM wParam, LPARAM lParam)
{
	POINT pt = {};
	GetCursorPos(&pt);
	ScreenToClient(m_hWnd, &pt);
	CDUIRect rcClient = GetClientRect();

	if (false == IsMaximized() && rcClient.PtInRect(pt))
	{
		RECT rcSizeBox = GetResizeTrack();
		if (pt.y < rcClient.top + rcSizeBox.top)
		{
			if (pt.x < rcClient.left + rcSizeBox.left) return HTTOPLEFT;
			if (pt.x > rcClient.right - rcSizeBox.right) return HTTOPRIGHT;
			return HTTOP;
		}
		else if (pt.y > rcClient.bottom - rcSizeBox.bottom)
		{
			if (pt.x < rcClient.left + rcSizeBox.left) return HTBOTTOMLEFT;
			if (pt.x > rcClient.right - rcSizeBox.right) return HTBOTTOMRIGHT;
			return HTBOTTOM;
		}

		if (pt.x < rcClient.left + rcSizeBox.left) return HTLEFT;
		if (pt.x > rcClient.right - rcSizeBox.right) return HTRIGHT;
	}

	CDUIRect rcCaption = { 0, 0, rcClient.right, GetCaptionHeight() };
	if (rcCaption.PtInRect(pt))
	{
		CDUIControlBase *pControl = FindControl(pt);
		if (NULL == pControl
			|| (NULL == MMInterfaceHelper(CDUIButtonCtrl, pControl) && NULL == MMInterfaceHelper(CDUIProgressCtrl, pControl)))
			return HTCAPTION;
	}

	return HTCLIENT;
}

LRESULT CDUIWndWin32::OnGetMinMaxInfo(WPARAM wParam, LPARAM lParam)
{
	LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST), &oMonitor);
	CDUIRect rcWork = oMonitor.rcWork;
	CDUIRect rcMonitor = oMonitor.rcMonitor;
	rcWork.Offset(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);

	// 计算最大化时，正确的原点坐标
	lpMMI->ptMaxPosition.x = rcWork.left;
	lpMMI->ptMaxPosition.y = rcWork.top;

	lpMMI->ptMaxSize.x = rcWork.GetWidth();
	lpMMI->ptMaxSize.y = rcWork.GetHeight();

	lpMMI->ptMaxTrackSize.x = GetWndMaxSize().cx == 0 ? rcWork.GetWidth() : GetWndMaxSize().cx;
	lpMMI->ptMaxTrackSize.y = GetWndMaxSize().cy == 0 ? rcWork.GetHeight() : GetWndMaxSize().cy;

	lpMMI->ptMinTrackSize.x = GetWndMinSize().cx;
	lpMMI->ptMinTrackSize.y = GetWndMinSize().cy;

	return 0;
}

LRESULT CDUIWndWin32::OnSysCommand(WPARAM wParam, LPARAM lParam)
{
	if (wParam == SC_CLOSE)
	{
		SendMessage(WM_CLOSE);

		return 0;
	}

#if defined(WIN32) && !defined(UNDER_CE)
	BOOL bZoomed = IsMaximized();
	LRESULT lRes = OnOldWndProc(WM_SYSCOMMAND, wParam, lParam);
	if (IsMaximized() != bZoomed && false == IsMinimized())
	{
		CDUIControlBase *pBtnMax = FindControl(Dui_CtrlIDInner_BtnMax);
		CDUIControlBase *pBtnRestore = FindControl(Dui_CtrlIDInner_BtnRestore);

		if (pBtnMax && pBtnRestore)
		{
			pBtnMax->SetVisible(false == IsMaximized());
			pBtnRestore->SetVisible(IsMaximized());
		}
	}

	return lRes;
#else
	return OnOldWndProc(WM_SYSCOMMAND, wParam, lParam);
#endif
}

LRESULT CDUIWndWin32::OnKillFocus(WPARAM wParam, LPARAM lParam)
{
	do
	{
		HWND hWndFocus = GetFocus();
		if (m_hWnd == hWndFocus
			|| (m_hWnd == ::GetParent(hWndFocus) && (GetWindowLong(hWndFocus, GWL_STYLE) & WS_CHILD))
			|| (m_hWnd == ::GetWindowOwner(hWndFocus) && (GetWindowLong(hWndFocus, GWL_STYLE) & WS_CHILD)))
		{
			break;
		}

		SetFocusControl(NULL);

	} while (false);

	return OnOldWndProc(WM_KILLFOCUS, wParam, lParam);
}

LRESULT CDUIWndWin32::OnPaint(CDUIRect rcPaint)
{
	LRESULT lRes = 0;

	//layout
	RefreshLayout();

	//verify
	if (NULL == m_pRootCtrl) return lRes;

	//animation wnd
	if (IsAnimatingWnd()) return lRes;

	//layered
	DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
	DWORD dwNewStyle = IsWndLayered() ? (dwStyle | WS_EX_LAYERED) : (dwStyle & ~WS_EX_LAYERED);
	if (dwStyle != dwNewStyle) ::SetWindowLong(m_hWnd, GWL_EXSTYLE, dwNewStyle);
	dwStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);

	//Should we paint?
	CDUIRect rcClient = GetClientRect();
	if (false == ::IntersectRect(&rcPaint, &rcPaint, &rcClient)) return lRes;
	if (NULL == m_pRootCtrl || rcPaint.Empty()) return lRes;

	//object
	if (NULL == m_hBmpBackground || NULL == m_hMemDcBackground)
	{
		//release
		ReleasePaintScene();

		//create
		EnsurePaintScene();
	}
	if (NULL == m_hMemDcBackground || NULL == m_hBmpBackground || NULL == m_pBmpBackgroundBits)
	{
		Invalidate();

		return lRes;
	}

	//clear pixel
	CDUIRenderEngine::ClearPixel(m_pBmpBackgroundBits, rcClient.GetWidth(), rcPaint);

	//paint
	try
	{
		int iSaveDC = ::SaveDC(m_hMemDcBackground);
		m_pRootCtrl->OnDraw(m_hMemDcBackground, rcPaint);
		::RestoreDC(m_hMemDcBackground, iSaveDC);
	}
	catch (const std::exception& exception)
	{
		OutputDebugString(CA2CT(exception.what()));
		CDUIGlobal::GetInstance()->SetDuiLastError((LPCTSTR)CA2CT(exception.what()));
	}

	//update
	if (dwStyle & WS_EX_LAYERED)
	{
		CDUIRect rcWnd = GetWindowRect();
		POINT pt = { rcWnd.left, rcWnd.top };
		SIZE szWindow = { rcWnd.GetWidth(), rcWnd.GetHeight() };
		POINT ptSrc = { 0, 0 };
		BLENDFUNCTION Blend = { AC_SRC_OVER, 0, GetWndAlpha(), AC_SRC_ALPHA };
		::UpdateLayeredWindow(m_hWnd, m_hDCPaint, &pt, &szWindow, m_hMemDcBackground,
			&ptSrc, 0, &Blend, ULW_ALPHA);

		return lRes;
	}

	::BitBlt(m_hDCPaint, rcPaint.left, rcPaint.top, rcPaint.GetWidth(), rcPaint.GetHeight(),
		m_hMemDcBackground, rcPaint.left, rcPaint.top, SRCCOPY);

	return lRes;
}

LRESULT CDUIWndWin32::OnImeComPosition(WPARAM wParam, LPARAM lParam)
{
	do
	{
		if (NULL == m_pFocusCtrl && NULL == m_pCaptureCtrl) break;

		POINT pt = {};
		GetCursorPos(&pt);
		ScreenToClient(m_hWnd, &pt);

		DuiMessage DuiMsg = {};
		DuiMsg.wParam = wParam;
		DuiMsg.lParam = lParam;
		DuiMsg.pMsgCtrl = m_pCaptureCtrl ? m_pCaptureCtrl : m_pFocusCtrl;
		DuiMsg.ptMouse = pt;
		DuiMsg.wKeyState = MapWndKeyState();
		DuiMsg.dwTimestamp = ::GetTickCount();
		LRESULT lRes = DuiMsg.pMsgCtrl->OnDuiImeComPosition(DuiMsg);

		if (lRes > 0)
		{
			return lRes;
		}

	} while (false);

	return OnOldWndProc(WM_IME_COMPOSITION, wParam, lParam);
}

void CDUIWndWin32::OnFinalMessage()
{
	m_OldWndProc = NULL;

	return __super::OnFinalMessage();
}

void CDUIWndWin32::UpdateImeCompositionPos()
{
	HIMC hIMC = ImmGetContext(GetWndHandle());
	if (hIMC)
	{
		//Set composition window position near caret position
		POINT point;
		GetCaretPos(&point);

		COMPOSITIONFORM Composition;
		Composition.dwStyle = CFS_POINT;
		Composition.ptCurrentPos.x = point.x;
		Composition.ptCurrentPos.y = point.y;
		ImmSetCompositionWindow(hIMC, &Composition);

		ImmReleaseContext(GetWndHandle(), hIMC);
	}

	return;
}

void CDUIWndWin32::EnsurePaintScene()
{
	if (m_hMemDcBackground || NULL == m_hDCPaint) return;

	CDUIRect rcClient = GetClientRect();
	m_hMemDcBackground = ::CreateCompatibleDC(m_hDCPaint);
	m_hBmpBackground = CDUIRenderEngine::CreateARGB32Bitmap(m_hDCPaint, rcClient.GetWidth(), rcClient.GetHeight(), &m_pBmpBackgroundBits);

	if (m_hMemDcBackground && m_hBmpBackground && m_pBmpBackgroundBits)
	{
		m_hBmpBackgroundOld = (HBITMAP)SelectObject(m_hMemDcBackground, m_hBmpBackground);
	}

	return;
}

void CDUIWndWin32::ReleasePaintScene()
{
	if (m_hMemDcBackground)
	{
		SelectObject(m_hMemDcBackground, m_hBmpBackgroundOld);
	}

	MMSafeDeleteObject(m_hBmpBackground);
	MMSafeDeleteDC(m_hMemDcBackground);
	m_pBmpBackgroundBits = NULL;
	m_hBmpBackgroundOld = NULL;

	return;
}

bool CDUIWndWin32::RegisterSuperclass()
{
	// Get the class information from an existing
	// window so we can subclass it later on...
	HINSTANCE hInstance = CDUIGlobal::GetInstance()->GetInstanceHandle();

	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	if (!::GetClassInfoEx(NULL, GetSuperClassName(), &wc))
	{
		if (!::GetClassInfoEx(hInstance, GetSuperClassName(), &wc))
		{
			ASSERT(!"Unable to locate window class");
			return NULL;
		}
	}

	m_OldWndProc = wc.lpfnWndProc;
	wc.lpfnWndProc = CDUIWndWin32::__ControlProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = GetClass();
	ATOM ret = ::RegisterClassEx(&wc);
	ASSERT(ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS);
	return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

bool CDUIWndWin32::RegisterWindowClass()
{
	HINSTANCE hInstance = CDUIGlobal::GetInstance()->GetInstanceHandle();

	WNDCLASS wc = {};
	wc.style = GetClassStyle();
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = NULL;
	wc.lpfnWndProc = CDUIWndWin32::__WndProc;
	wc.hInstance = hInstance;
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = GetClass();
	ATOM ret = ::RegisterClass(&wc);
	ASSERT(ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS);
	return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

LRESULT CALLBACK CDUIWndWin32::__WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CDUIWnd* pThis = NULL;
	if (uMsg == WM_NCCREATE)
	{
		LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
		pThis = static_cast<CDUIWnd*>(lpcs->lpCreateParams);
		pThis->m_hWnd = hWnd;
		::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(pThis));
	}
	else
	{
		pThis = reinterpret_cast<CDUIWnd*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
		if (uMsg == WM_NCDESTROY && pThis != NULL)
		{
			LRESULT lRes = pThis->OnOldWndProc(uMsg, wParam, lParam);
			::SetWindowLongPtr(pThis->m_hWnd, GWLP_USERDATA, 0L);
			if (pThis->m_bSubWindow) pThis->UnSubWindow();
			pThis->m_hWnd = NULL;
			pThis->OnFinalMessage();
			return lRes;
		}
	}

	if (pThis != NULL)
	{
		return pThis->OnWndMessage(uMsg, wParam, lParam);
	}
	else
	{
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

LRESULT CALLBACK CDUIWndWin32::__ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CDUIWnd* pThis = NULL;
	if (uMsg == WM_NCCREATE)
	{
		LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
		pThis = static_cast<CDUIWnd*>(lpcs->lpCreateParams);
		::SetProp(hWnd, _T("WndX"), (HANDLE)pThis);
		pThis->m_hWnd = hWnd;
	}
	else
	{
		pThis = reinterpret_cast<CDUIWnd*>(::GetProp(hWnd, _T("WndX")));
		if (uMsg == WM_NCDESTROY && pThis != NULL)
		{
			LRESULT lRes = pThis->OnOldWndProc(uMsg, wParam, lParam);
			if (pThis->m_bSubWindow) pThis->UnSubWindow();
			::SetProp(hWnd, _T("WndX"), NULL);
			pThis->m_hWnd = NULL;
			pThis->OnFinalMessage();
			return lRes;
		}
	}

	if (pThis != NULL)
	{
		return pThis->OnWndMessage(uMsg, wParam, lParam);
	}
	else
	{
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

void CDUIWndWin32::ForegroundWindow(HWND hWnd)
{
	HWND hWndForground = GetForegroundWindow();
	DWORD dwThreadIDForground = ::GetWindowThreadProcessId(hWndForground, NULL);
	DWORD dwThreadIDCur = ::GetCurrentThreadId();
	if (dwThreadIDForground == dwThreadIDCur || hWndForground == hWnd) return;

	::AttachThreadInput(dwThreadIDCur, dwThreadIDForground, TRUE);
	::SetActiveWindow(hWnd);
	::BringWindowToTop(hWnd);
	::SetForegroundWindow(hWnd);
	::AttachThreadInput(dwThreadIDCur, dwThreadIDForground, FALSE);

	return;
}

#endif