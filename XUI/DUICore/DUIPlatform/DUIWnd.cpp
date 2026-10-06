#include "stdafx.h"
#include "DUIWnd.h"

//////////////////////////////////////////////////////////////////////////
#if defined(DuiPlatform_SDL)
static Uint32 SDLCALL __DuiSdlTimerProc(void *userdata, SDL_TimerID timerID, Uint32 interval)
{
	CDUIWndBase *pWnd = static_cast<CDUIWndBase *>(userdata);
	if (pWnd == NULL) return 0;

	if (0 == pWnd->PostMessage(WM_TIMER, (WPARAM)timerID, 0))
	{
		return 0;
	}

	return interval;
}
#endif

////////////////////////////////////////////////////////////////////
MMImplement_ClassName(CDUIWnd)

DuiBegin_Message_Map(CDUIWnd, CDUINotifyPump)
	Dui_On_Timer()
	Dui_On_Notify(DuiNotify_Click, OnDuiClick)
	Dui_On_Notify(DuiNotify_WndAnimateFinish, OnDuiWndAnimateFinish)
DuiEnd_Message_Map()

CDUIWnd::CDUIWnd(LPCTSTR lpszDuiName, HWND hWndParent)
#if defined(DuiPlatform_SDL)
	: CDUIWndSDL(lpszDuiName, hWndParent)
#else
	: CDUIWndWin32(lpszDuiName, hWndParent)
#endif
{
	CDUIGlobal::GetInstance()->AddWnd(this);
	CDUIGlobal::GetInstance()->AddPreMessagePtr(this);

	//attribute
	CDUIGlobal::GetInstance()->LoadWnd(GetDuiName(), this);

	return;
}

CDUIWnd::~CDUIWnd()
{
	CDUIGlobal::GetInstance()->RemoveWnd(this);
	CDUIGlobal::GetInstance()->RemovePreMessagePtr(this);

	return;
}

LPVOID CDUIWnd::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIWnd, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIWnd::GetDescribe() const
{
	return Dui_Window;
}

bool CDUIWnd::SetTimer(CDUIPropertyObject *pPropObj, UINT uTimerID, UINT uElapse)
{
	ASSERT(pPropObj != NULL);
	ASSERT(uElapse > 0);
	if (NULL == pPropObj || 0 == uElapse) return false;

	for (int n = 0; n < m_vecTimers.size(); n++)
	{
		auto &TimerInfo = m_vecTimers[n];
		if (TimerInfo.pPropObj == pPropObj
			&& TimerInfo.hWnd == m_hWnd
			&& TimerInfo.nLocalID == uTimerID)
		{
			if (TimerInfo.bKilled == true)
			{
#if defined(DuiPlatform_SDL)
				SDL_TimerID uSdlTimer = SDL_AddTimer(uElapse, __DuiSdlTimerProc, this);
				if (0 != uSdlTimer)
				{
					TimerInfo.uWinTimer = (UINT)uSdlTimer;
					TimerInfo.bKilled = false;
					return true;
				}
#else
				if (::SetTimer(m_hWnd, TimerInfo.uWinTimer, uElapse, NULL))
				{
					TimerInfo.bKilled = false;
					return true;
				}
#endif

				return false;
			}

			return false;
		}
	}

	DuiTimerInfo TimerInfo;
	TimerInfo.hWnd = m_hWnd;
	TimerInfo.pPropObj = pPropObj;
	TimerInfo.pControl = MMInterfaceHelper(CDUIControlBase, pPropObj);
	TimerInfo.nLocalID = uTimerID;
	TimerInfo.bKilled = false;

#if defined(DuiPlatform_SDL)
	SDL_TimerID uSdlTimer = SDL_AddTimer(uElapse, __DuiSdlTimerProc, this);
	if (0 == uSdlTimer) return false;

	TimerInfo.uWinTimer = (UINT)uSdlTimer;
#else
	m_uTimerID = (++m_uTimerID) % 0xF0; //0xf1-0xfe特殊用途
	if (false == ::SetTimer(m_hWnd, m_uTimerID, uElapse, NULL)) return false;

	TimerInfo.uWinTimer = m_uTimerID;
#endif

	m_vecTimers.push_back(TimerInfo);

	return true;
}

bool CDUIWnd::KillTimer(CDUIPropertyObject *pPropObj, UINT uTimerID)
{
	ASSERT(pPropObj != NULL);
	if (NULL == pPropObj) return false;

	for (int n = 0; n < m_vecTimers.size(); n++)
	{
		auto &TimerInfo = m_vecTimers[n];
		if (TimerInfo.pPropObj == pPropObj
			&& TimerInfo.hWnd == m_hWnd
			&& TimerInfo.nLocalID == uTimerID)
		{
			if (TimerInfo.bKilled == false)
			{
#if defined(DuiPlatform_SDL)
				SDL_RemoveTimer((SDL_TimerID)TimerInfo.uWinTimer);
#else
				if (::IsWindow(m_hWnd)) ::KillTimer(TimerInfo.hWnd, TimerInfo.uWinTimer);
#endif

				TimerInfo.bKilled = true;

				return true;
			}
		}
	}

	return false;
}

bool CDUIWnd::KillTimer(CDUIPropertyObject *pPropObj)
{
	ASSERT(pPropObj != NULL);
	if (NULL == pPropObj) return false;

	for (int i = m_vecTimers.size() - 1; i >= 0; i--)
	{
		DuiTimerInfo TimerInfo = (m_vecTimers[i]);

		if (TimerInfo.pPropObj == pPropObj)
		{
			if (false == TimerInfo.bKilled)
			{
#if defined(DuiPlatform_SDL)
				SDL_RemoveTimer((SDL_TimerID)TimerInfo.uWinTimer);
#else
				::KillTimer(TimerInfo.hWnd, TimerInfo.uWinTimer);
#endif
			}

			m_vecTimers.erase(m_vecTimers.begin() + i);
		}
	}

	return false;
}

bool CDUIWnd::RemoveAllTimer()
{
	for (auto &TimerInfo : m_vecTimers)
	{
		if (TimerInfo.bKilled == false)
		{
			if (IsWindow(m_hWnd))
			{
#if defined(DuiPlatform_SDL)
				SDL_RemoveTimer((SDL_TimerID)TimerInfo.uWinTimer);
#else
				::KillTimer(m_hWnd, TimerInfo.uWinTimer);
#endif
			}
		}
	}

	m_vecTimers.clear();
	m_TimerStore = {};

	return true;
}

void CDUIWnd::InitProperty()
{
	DuiCreateGroupAttribute(m_AttributeGroupWndInfo, _T("WndInfo"));
	DuiCreateAttribute(m_AttributeWndCaptionHeight, _T("WndCaptionHeight"), _T(""), m_AttributeGroupWndInfo);
	DuiCreateAttribute(m_AttributeWndInitSize, _T("WndInitSize"), _T(""), m_AttributeGroupWndInfo);
	DuiCreateAttribute(m_AttributeWndMinSize, _T("WndMinSize"), _T(""), m_AttributeGroupWndInfo);
	DuiCreateAttribute(m_AttributeWndMaxSize, _T("WndMaxSize"), _T(""), m_AttributeGroupWndInfo);
	DuiCreateAttribute(m_AttributeWndResizeTrack, _T("WndResizeTrack"), _T(""), m_AttributeGroupWndInfo);

	DuiCreateGroupAttribute(m_AttributeGroupLayered, _T("WndLayered"));
	DuiCreateAttribute(m_AttributeWndLayered, _T("WndLayered"), _T(""), m_AttributeGroupLayered);
	DuiCreateAttribute(m_AttributeWndAlpha, _T("WndAlpha"), _T(""), m_AttributeGroupLayered);

	DuiCreateGroupAttribute(m_AttributeGroupRenderText, _T("RenderText"));
	DuiCreateAttribute(m_AttributeGdiplusRenderText, _T("GdiplusRenderText"), _T(""), m_AttributeGroupRenderText);
	DuiCreateAttribute(m_AttributeGdiplusRenderType, _T("GdiplusRenderType"), _T(""), m_AttributeGroupRenderText);

	DuiCreateGroupAttribute(m_AttributeGroupRenderImage, _T("RenderImage"));
	DuiCreateAttribute(m_AttributeGdiplusRenderImage, _T("GdiplusRenderImage"), _T(""), m_AttributeGroupRenderImage);

	DuiCreateGroupAttribute(m_AttributeGroupAnimation, _T("Animation"));
	DuiCreateAttribute(m_AttributeAnimationType, _T("AnimationType"), _T("please set window layered style, or the animation is not perfect"), m_AttributeGroupAnimation);
	DuiCreateAttribute(m_AttributeAnimationFrame, _T("AnimationFrame"), _T("Window Animation TotalFrame"), m_AttributeGroupAnimation);
	DuiCreateAttribute(m_AttributeAnimationElapse, _T("AnimationElapse"), _T("Window Animation Elapse Of One Frame"), m_AttributeGroupAnimation);

	DuiCreateGroupAttribute(m_AttributeGroupCustom, _T("CustomData"));
	DuiCreateAttribute(m_AttributeUserTag, _T("UserTag"), _T(""), m_AttributeGroupCustom);

	//value
	if (false == m_AttributeGdiplusRenderType.IsModified())
	{
		tagDuiCombox AttriCombox;
#if defined(DuiPlatform_SDL)
		m_AttributeGdiplusRenderType.SetCombox(AttriCombox);
		//m_AttributeGdiplusRenderType.SelectItem(TextRenderingHintSystemDefault);
#else
		AttriCombox.vecItem.push_back({ Gdiplus::TextRenderingHintSystemDefault, _T("HintSystemDefault") });
		AttriCombox.vecItem.push_back({ Gdiplus::TextRenderingHintSingleBitPerPixelGridFit, _T("HintSingleBitPerPixelGridFit") });
		AttriCombox.vecItem.push_back({ Gdiplus::TextRenderingHintSingleBitPerPixel, _T("HintSingleBitPerPixel") });
		AttriCombox.vecItem.push_back({ Gdiplus::TextRenderingHintAntiAliasGridFit, _T("HintAntiAliasGridFit") });
		AttriCombox.vecItem.push_back({ Gdiplus::TextRenderingHintAntiAlias, _T("HintAntiAlias") });
		AttriCombox.vecItem.push_back({ Gdiplus::TextRenderingHintClearTypeGridFit, _T("HintClearTypeGridFit") });
		m_AttributeGdiplusRenderType.SetCombox(AttriCombox);
		m_AttributeGdiplusRenderType.SelectItem(Gdiplus::TextRenderingHintSystemDefault);
#endif
	}
	if (false == m_AttributeAnimationType.IsModified())
	{
		tagDuiCombox AttriCombox;
		AttriCombox.vecItem.push_back({ AnimateWnd_None, _T("None") });
		AttriCombox.vecItem.push_back({ AnimateWnd_Left, _T("Left") });
		AttriCombox.vecItem.push_back({ AnimateWnd_LeftDiffuse, _T("LeftDiffuse") });
		AttriCombox.vecItem.push_back({ AnimateWnd_Right, _T("Right") });
		AttriCombox.vecItem.push_back({ AnimateWnd_RightDiffuse, _T("RightDiffuse") });
		AttriCombox.vecItem.push_back({ AnimateWnd_Top, _T("Top") });
		AttriCombox.vecItem.push_back({ AnimateWnd_TopDiffuse, _T("TopDiffuse") });
		AttriCombox.vecItem.push_back({ AnimateWnd_Bottom, _T("Bottom") });
		AttriCombox.vecItem.push_back({ AnimateWnd_BottomDiffuse, _T("BottomDiffuse") });
		AttriCombox.vecItem.push_back({ AnimateWnd_Size, _T("Size") });
		//m_AttributeAnimationType.InsertItem(AnimateWnd_SizeSpring, _T("SizeSpring"));
		AttriCombox.vecItem.push_back({ AnimateWnd_SizeDiffuse, _T("SizeDiffuse") });
		AttriCombox.vecItem.push_back({ AnimateWnd_CenterDiffuse, _T("CenterDiffuse") });
		AttriCombox.vecItem.push_back({ AnimateWnd_LeftRightDiffuse, _T("LeftRightDiffuse") });
		AttriCombox.vecItem.push_back({ AnimateWnd_LeftRightExpand, _T("LeftRightExpand") });
		//m_AttributeAnimationType.InsertItem(AnimateWnd_LeftRightExpandSpring, _T("LeftRightExpandSpring"));
		AttriCombox.vecItem.push_back({ AnimateWnd_LeftRightCombin, _T("LeftRightCombin") });
		AttriCombox.vecItem.push_back({ AnimateWnd_TopBottomDiffuse, _T("TopBottomDiffuse") });
		AttriCombox.vecItem.push_back({ AnimateWnd_TopBottomExpand, _T("TopBottomExpand") });
		//m_AttributeAnimationType.InsertItem(AnimateWnd_TopBottomExpandSpring, _T("TopBottomExpandSpring"));
		AttriCombox.vecItem.push_back({ AnimateWnd_TopBottomCombin, _T("TopBottomCombin") });
		AttriCombox.vecItem.push_back({ AnimateWnd_3DRotateLeftRight, _T("3DRotateLeftRight") });
		AttriCombox.vecItem.push_back({ AnimateWnd_3DRotateTopBottom, _T("3DRotateTopBottom") });
		AttriCombox.vecItem.push_back({ AnimateWnd_3DRoundLeft, _T("3DRoundLeft") });
		AttriCombox.vecItem.push_back({ AnimateWnd_3DRoundRight, _T("3DRoundRight") });
		AttriCombox.vecItem.push_back({ AnimateWnd_3DRoundTop, _T("3DRoundTop") });
		AttriCombox.vecItem.push_back({ AnimateWnd_3DRoundBottom, _T("3DRoundBottom") });
		AttriCombox.vecItem.push_back({ AnimateWnd_3DRotateReplaseLR, _T("3DRotateReplaseLR") });
		AttriCombox.vecItem.push_back({ AnimateWnd_3DRotateReplaseTB, _T("3DRotateReplaseTB") });
		AttriCombox.vecItem.push_back({ AnimateWnd_3DRoundReplaseLR, _T("3DRoundReplaseLR") });
		AttriCombox.vecItem.push_back({ AnimateWnd_3DRoundReplaseTB, _T("3DRoundReplaseTB") });
		m_AttributeAnimationType.SetCombox(AttriCombox);
		m_AttributeAnimationType.SelectItem(AnimateWnd_None);
	}

	return;
}

LRESULT CDUIWnd::OnPreWndMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled)
{
#if defined(DuiPlatform_SDL)
	if (IsWindow(GetWndHandle())
		&& MMSdlIsPopupWindow(GetWndHandle())
		&& (WM_LBUTTONDOWN == uMsg || WM_RBUTTONDOWN == uMsg || WM_MBUTTONDOWN == uMsg))
	{
		float fX = 0.0f;
		float fY = 0.0f;
		SDL_GetGlobalMouseState(&fX, &fY);

		CDUIRect rcDrop;
		::GetWindowRect(GetWndHandle(), &rcDrop);

		if (false == rcDrop.PtInRect(CDUIPoint((int)fX, (int)fY)))
		{
			Close();
		}
	}
#endif

	return __super::OnPreWndMessage(hWnd, uMsg, wParam, lParam, bHandled);
}

LRESULT CDUIWnd::OnCreate(WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = OnOldWndProc(WM_CREATE, wParam, lParam);

	//root
	CDUIContainerCtrl *pRootCtrl = dynamic_cast<CDUIContainerCtrl*>(CDUIGlobal::GetInstance()->LoadDui(GetDuiName(), IsInitComplete() ? NULL : this));
	if (NULL == pRootCtrl)
	{
		OutputDebugString(CDUIGlobal::GetInstance()->GetDuiLastError());
	}

	//init
#if defined(DuiPlatform_SDL)
#else
	//modify
	LONG lStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	lStyle &= ~WS_CAPTION;
	//lStyle &= ~WS_SYSMENU;
	::SetWindowLong(m_hWnd, GWL_STYLE, lStyle);

	m_hDCPaint = ::GetDC(m_hWnd);
	CMMDragDrop::UnRegister();
#endif
	m_uTimerID = 0x1000;
	m_bMouseTracking = false;
	m_bRefreshToolTipNeeded = false;
	m_bRefreshViewNeeded = false;
	m_bPostedAppMsg = false;
	m_bFirstLayout = true;

	AdjustWndSize();
	AddINotify(this);
	AttachRootCtrl(pRootCtrl);

	//center wnd
	if (CW_USEDEFAULT == m_ptCreate.x
		&& CW_USEDEFAULT == m_ptCreate.y
#if defined(DuiPlatform_SDL)
		&& SDL_GetWindowParent(m_hWnd) == NULL)
#else
		&& 0 == (lStyle & WS_CHILD))
#endif
	{
		CenterWindow();
	}

	//max restore
	CDUIControlBase *pBtnMax = FindControl(Dui_CtrlIDInner_BtnMax);
	CDUIControlBase *pBtnRestore = FindControl(Dui_CtrlIDInner_BtnRestore);
	if (pBtnMax && pBtnRestore)
	{
		pBtnMax->SetVisible(false == IsMaximized());
		pBtnRestore->SetVisible(IsMaximized());
	}

	//init func
	OnFindControl();
	OnInitDialog();

	//call func
	if (g_pIDuiWndNotify)
	{
		g_pIDuiWndNotify->OnDuiWndCreate(this);
	}

	return lRes;
}

LRESULT CDUIWnd::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	do
	{
		CDUIPoint pt(lParam);

		//msg
		DuiMessage DuiMsg = {};
		DuiMsg.wParam = wParam;
		DuiMsg.lParam = lParam;
		DuiMsg.ptMousePre = m_ptMousePosLast;
		DuiMsg.ptMouse = pt;

		m_ptMousePosPre = m_ptMousePosLast;
		m_ptMousePosLast = pt;

		//capture
		if (m_pCaptureCtrl)
		{
			DuiMsg.pMsgCtrl = m_pCaptureCtrl;
			m_pCaptureCtrl->OnDuiMouseMove(pt, DuiMsg);

			//model
			DispatchModelMouseEvent(m_pCaptureCtrl, &CDUIControlBase::OnDuiMouseMove, pt, DuiMsg, false);

			if (NULL == m_pWinDragCtrl
				&& (abs(m_ptMousePosLast.x - m_ptMousePosDown.x) >= 4 || abs(m_ptMousePosLast.y - m_ptMousePosDown.y) >= 4))
			{
				if (m_pCaptureCtrl->IsWinDragEnabled())
				{
					m_pCaptureCtrl->OnWinDragStart(pt, DuiMsg);

					//model
					DispatchModelMouseEvent(m_pCaptureCtrl, &CDUIControlBase::OnWinDragStart, pt, DuiMsg, false);
				}
				else if ((NULL == MMInterfaceHelper(CDUIXEditCtrl, m_pCaptureCtrl) || m_pFocusCtrl != m_pCaptureCtrl)
#ifndef DuiPlatform_SDL
					&& (NULL == MMInterfaceHelper(CDUIEditCtrl, m_pCaptureCtrl) || m_pFocusCtrl != m_pCaptureCtrl)
					&& (NULL == MMInterfaceHelper(CDUIRichEditCtrl, m_pCaptureCtrl) || m_pFocusCtrl != m_pCaptureCtrl)
#endif
					&& m_pCaptureCtrl->GetOwnerModelCtrl()
					&& m_pCaptureCtrl->GetOwnerModelCtrl()->IsWinDragEnabled())
				{
					m_pCaptureCtrl->GetOwnerModelCtrl()->OnWinDragStart(pt, DuiMsg);
				}
			}

			break;
		}

		//tooltip
		if (false == m_bMouseTracking)
		{
#if defined(DuiPlatform_SDL)
#else
			TRACKMOUSEEVENT tme = {};
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.dwFlags = TME_HOVER | TME_LEAVE;
			tme.hwndTrack = m_hWnd;
			tme.dwHoverTime = NULL == m_hWndTooltip ? m_nToolTipHoverTime : (DWORD)::SendMessage(m_hWndTooltip, TTM_GETDELAYTIME, TTDT_INITIAL, 0L);
			BOOL bRes = _TrackMouseEvent(&tme);
			m_bMouseTracking = true;
#endif
		}

		CDUIControlBase *pControl = FindSubControlByPoint(m_pRootCtrl, pt);

		//hover
		if (pControl == m_pHoverCtrl && m_pHoverCtrl)
		{
			DuiMsg.pMsgCtrl = m_pHoverCtrl;
			DuiMsg.pMsgCtrl->OnDuiMouseMove(pt, DuiMsg);

			//model
			DispatchModelMouseEvent(m_pHoverCtrl, &CDUIControlBase::OnDuiMouseMove, pt, DuiMsg, false);

			break;
		}

		//leave
		if (m_hWndTooltip)
		{
#if defined(DuiPlatform_SDL)
#else
			::SendMessage(m_hWndTooltip, TTM_TRACKACTIVATE, false, (LPARAM)&m_ToolTip);
#endif
		}
		auto vecMouseEnterCtrl = m_vecMouseEnterCtrl;
		for (int n = vecMouseEnterCtrl.size() - 1; n >= 0; n--)
		{
			CDUIControlBase *pControl = vecMouseEnterCtrl[n];
			if (NULL == pControl) continue;

			CDUIRect rcCtrl = pControl->GetAbsoluteRect();
			if (false == rcCtrl.PtInRect(pt))
			{
				DuiMessage msgLeave = {};
				msgLeave.wParam = DuiMsg.wParam;
				msgLeave.lParam = DuiMsg.lParam;
				msgLeave.pMsgCtrl = pControl;
				pControl->OnDuiMouseLeave(pt, msgLeave);

				m_vecMouseEnterCtrl.erase(std::remove(m_vecMouseEnterCtrl.begin(), m_vecMouseEnterCtrl.end(), pControl), m_vecMouseEnterCtrl.end());
			}
		}

		//hover
		m_pHoverCtrl = pControl;
		m_bRefreshToolTipNeeded = true;
		if (m_pHoverCtrl && std::find(m_vecMouseEnterCtrl.begin(), m_vecMouseEnterCtrl.end(), m_pHoverCtrl) == m_vecMouseEnterCtrl.end())
		{
			m_vecMouseEnterCtrl.push_back(m_pHoverCtrl);

			//enter
			DuiMsg.pMsgCtrl = m_pHoverCtrl;
			m_pHoverCtrl->OnDuiMouseEnter(pt, DuiMsg);
			DispatchModelMouseEvent(m_pHoverCtrl, &CDUIControlBase::OnDuiMouseEnter, pt, DuiMsg, false);

			//move, verify if enter popup wnd then hoverctrl will null
			if (m_pHoverCtrl)
			{
				DuiMsg.pMsgCtrl = m_pHoverCtrl;
				m_pHoverCtrl->OnDuiMouseMove(pt, DuiMsg);
				DispatchModelMouseEvent(m_pHoverCtrl, &CDUIControlBase::OnDuiMouseMove, pt, DuiMsg, false);
			}
		}

	} while (false);

	return OnOldWndProc(WM_MOUSEMOVE, wParam, lParam);
}

LRESULT CDUIWnd::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	CDUIPoint pt(lParam);
	DuiMessage msgLeave = {};
	msgLeave.wParam = wParam;
	msgLeave.lParam = MAKELPARAM(-1, -1);

	for (int n = m_vecMouseEnterCtrl.size() - 1; n >= 0; n--)
	{
		CDUIControlBase *pControl = m_vecMouseEnterCtrl[n];
		if (NULL == pControl) continue;

		msgLeave.pMsgCtrl = pControl;
		msgLeave.pMsgCtrl->OnDuiMouseLeave(pt, msgLeave);
	}

	m_pHoverCtrl = NULL;
	m_bRefreshToolTipNeeded = true;
	m_vecMouseEnterCtrl.clear();

	//提示窗体
	if (m_hWndTooltip)
	{
#if defined(DuiPlatform_SDL)
#else
		::SendMessage(m_hWndTooltip, TTM_TRACKACTIVATE, false, (LPARAM)&m_ToolTip);
#endif
	}
	if (m_bMouseTracking && m_hWnd) 
	{
		SendMessage(WM_MOUSEMOVE, 0, (LPARAM)-1);
	}

	m_bMouseTracking = false;

	return OnOldWndProc(WM_MOUSELEAVE, wParam, lParam);
}

LRESULT CDUIWnd::OnSetCursor(WPARAM wParam, LPARAM lParam)
{
	do
	{
#ifndef DuiPlatform_SDL
		if (LOWORD(lParam) != HTCLIENT) break;
#endif

		if (m_pCaptureCtrl) return 1;

		POINT pt = { 0 };
		GetCursorPos(&pt);
		ScreenToClient(m_hWnd, &pt);
		CDUIControlBase *pControl = FindControl(pt);
		if (NULL == pControl) break;
		if ((pControl->GetControlFlags() & DUIFLAG_SETCURSOR) == 0) break;

		DuiMessage DuiMsg = {};
		DuiMsg.wParam = wParam;
		DuiMsg.lParam = lParam;
		DuiMsg.wKeyState = MapWndKeyState();
		DuiMsg.dwTimestamp = ::GetTickCount();
		DuiMsg.ptMouse = pt;
		pControl->OnDuiSetCursor(pt, DuiMsg);

		return 1;

	} while (false);

	return OnOldWndProc(WM_SETCURSOR, wParam, lParam);
}

CDUIWnd * CDUIWnd::GetWndOwner()
{
	return this;
}

void CDUIWnd::DispatchModelMouseEvent(CDUIControlBase *pCtrl, MouseEventPtr pFunc, const CDUIPoint &pt, const DuiMessage &Msg, bool bClick)
{
	if (pCtrl
		&& (false == bClick || pCtrl->IsClickTransmit())
		&& pCtrl->GetOwnerModelCtrl()
		&& pCtrl->GetOwnerModelCtrl()->IsEnabled()
		&& false == pCtrl->GetOwnerModelCtrl()->IsMouseThrough()
		&& (NULL == MMInterfaceHelper(CDUIXEditCtrl, pCtrl) || m_pFocusCtrl != pCtrl)
#if !defined(DuiPlatform_SDL)
		&& (NULL == MMInterfaceHelper(CDUIEditCtrl, pCtrl) || m_pFocusCtrl != pCtrl)
		&& (NULL == MMInterfaceHelper(CDUIRichEditCtrl, pCtrl) || m_pFocusCtrl != pCtrl)
#endif
		)
	{
		CDUIControlBase *pOwnerModelCtrl = pCtrl->GetOwnerModelCtrl();
		if (pFunc == &CDUIControlBase::OnDuiMouseEnter)
		{
			if (std::find(m_vecMouseEnterCtrl.begin(), m_vecMouseEnterCtrl.end(), pOwnerModelCtrl) == m_vecMouseEnterCtrl.end())
			{
				m_vecMouseEnterCtrl.push_back(pOwnerModelCtrl);
				(pOwnerModelCtrl->*pFunc)(pt, Msg);
			}
		}
		else
		{
			(pOwnerModelCtrl->*pFunc)(pt, Msg);
		}
	}

	return;
}

void CDUIWnd::DispatchModelKeyboardEvent(CDUIControlBase *pCtrl, KeyboardEventPtr pFunc, const DuiMessage &Msg)
{
	if (pCtrl
		&& pCtrl->GetOwnerModelCtrl()
		&& pCtrl->GetOwnerModelCtrl()->IsEnabled()
		&& (NULL == MMInterfaceHelper(CDUIXEditCtrl, pCtrl) || m_pFocusCtrl != pCtrl)
#if !defined(DuiPlatform_SDL)
		&& (NULL == MMInterfaceHelper(CDUIEditCtrl, pCtrl) || m_pFocusCtrl != pCtrl)
		&& (NULL == MMInterfaceHelper(CDUIRichEditCtrl, pCtrl) || m_pFocusCtrl != pCtrl)
#endif
		)
	{
		CDUIControlBase *pOwnerModelCtrl = pCtrl->GetOwnerModelCtrl();
		(pOwnerModelCtrl->*pFunc)(Msg);
	}

	return;
}

UINT CDUIWnd::MapKeyState()
{
#if defined(DuiPlatform_SDL)
	return CDUIWndSDL().MapWndKeyState();
#else
	return CDUIWndWin32().MapWndKeyState();
#endif
}