#include "stdafx.h"
#include "DUIWndBase.h"

//////////////////////////////////////////////////////////////////////////
MMImplement_ClassName(CDUIWndBase)

DuiBegin_Message_Map(CDUIWndBase, CDUINotifyPump)
	Dui_On_Timer()
	Dui_On_Notify(DuiNotify_Click, OnDuiClick)
	Dui_On_Notify(DuiNotify_WndAnimateFinish, OnDuiWndAnimateFinish)
DuiEnd_Message_Map()

CDUIWndBase::CDUIWndBase(LPCTSTR lpszDuiName, HWND hWndParent)
	: m_strDuiName(lpszDuiName)
	, m_hWndParent(hWndParent)
	, m_hWnd(NULL)
{
	SetDpi(CDUIGlobal::GetInstance()->GetDpi());

	return;
}

CDUIWndBase::~CDUIWndBase()
{
	CDUIAnimationWnd::UnInit();

	OnDuiDelayDelete();
	ReleaseRootCtrl();
	OnFinalMessage();

	return;
}

bool CDUIWndBase::RegisterControlListen(IDuiControlListen *pIDuiControlListen)
{
	if (false == __super::RegisterControlListen(pIDuiControlListen)) return false;

	CDUIContainerCtrl *pRootCtrl = GetRootCtrl();
	if (NULL == pRootCtrl) return true;

	return pRootCtrl->RegisterControlListen(pIDuiControlListen);
}

bool CDUIWndBase::UnRegisterControlListen(IDuiControlListen *pIDuiControlListen)
{
	if (false == __super::UnRegisterControlListen(pIDuiControlListen)) return false;

	CDUIContainerCtrl *pRootCtrl = GetRootCtrl();
	if (NULL == pRootCtrl) return false;

	return pRootCtrl->UnRegisterControlListen(pIDuiControlListen);
}

bool CDUIWndBase::OnAttributeChange(CDUIAttributeObject *pAttributeObj)
{
	if (NULL == pAttributeObj) return true;

	__super::OnAttributeChange(pAttributeObj);

	if (m_hWnd && m_pRootCtrl)
	{
		AdjustWndSize();

		CDUIRect rcClient = GetClientRect();
		m_pRootCtrl->OnDuiSize(rcClient);

		Invalidate();
	}

	return true;
}

LPVOID CDUIWndBase::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIWndBase, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIWndBase::GetDescribe() const
{
	return Dui_WindowBase;
}

bool CDUIWndBase::IsDesigned()
{
	return m_bDesigned;
}

void CDUIWndBase::SetDesigned(bool bDesigned)
{
	if (bDesigned == IsDesigned()) return;

	m_bDesigned = bDesigned;

	return;
}

CMMString CDUIWndBase::GetDuiName() const
{
	return m_strDuiName;
}

UINT CDUIWndBase::GetClassStyle() const
{
	return CS_DBLCLKS;
}

UINT CDUIWndBase::MapWndKeyState()
{
	return 0;
}

LPCTSTR CDUIWndBase::GetSuperClassName() const
{
	return NULL;
}

HWND CDUIWndBase::Create(HWND hWndParent, LPCTSTR lpszName, DWORD dwStyle, DWORD dwExStyle, const RECT rc)
{
	return Create(hWndParent, lpszName, dwStyle, dwExStyle, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
}

HWND CDUIWndBase::Create(HWND hWndParent, LPCTSTR lpszName, DWORD dwStyle, DWORD dwExStyle, int x, int y, int cx, int cy)
{
	return NULL;
}

HWND CDUIWndBase::SubWindow(HWND hWnd)
{
	return NULL;
}

void CDUIWndBase::UnSubWindow()
{
	return;
}

void CDUIWndBase::ShowWindow(bool bShow /*= true*/, bool bTakeFocus /*= false*/)
{
	return;
}

void CDUIWndBase::ShowWindow(int nCmdShow)
{
	return;
}

UINT CDUIWndBase::DoModal()
{
	return 0;
}

UINT CDUIWndBase::DoBlock()
{
	return 0;
}

void CDUIWndBase::Close(UINT nRet)
{
	if (false == IsWindow(m_hWnd)) return;

	PostMessage(WM_CLOSE, (WPARAM)nRet, 0L);

	return;
}

void CDUIWndBase::CenterWindow()
{
	return;
}

void CDUIWndBase::SetIcon(UINT nRes)
{
	return;
}

LRESULT CDUIWndBase::SendMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
	return 0;
}

LRESULT CDUIWndBase::PostMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
	return 0;
}

void CDUIWndBase::SetCapture()
{
	return;
}

void CDUIWndBase::ReleaseCapture()
{
	return;
}

bool CDUIWndBase::IsCaptured()
{
	return false;
}

CDUIControlBase * CDUIWndBase::GetCaptureControl()
{
	return m_pCaptureCtrl;
}

CDUIControlBase * CDUIWndBase::GetFocusControl()
{
	return m_pFocusCtrl;
}

void CDUIWndBase::SetFocusControl(CDUIControlBase *pFocusCtrl)
{
	if (m_pFocusCtrl == pFocusCtrl) return;

	//kill focus
	if (m_pFocusCtrl)
	{
		CDUIControlBase *pControl = m_pFocusCtrl;
		m_pFocusCtrl = NULL;
		pControl->OnDuiKillFocus();
	}

	//set focus
	m_pFocusCtrl = pFocusCtrl;
	if (m_pFocusCtrl)
	{
		m_pFocusCtrl->OnDuiSetFocus();
	}

	return;
}

CDUIControlBase * CDUIWndBase::GetHoverCtrl()
{
	return m_pHoverCtrl;
}

bool CDUIWndBase::AttachRootCtrl(CDUIContainerCtrl* pControl)
{
	if (NULL == pControl) return false;

	// Reset any previous attachment
	ReleaseRootCtrl();

	// Set the dialog root element
	m_pRootCtrl = pControl;

	//m_shadow.Create(this);
	m_mapControl.clear();

	// Initiate all control
	InitControls(pControl);

	//active tabpag
	for (auto GroupItem : m_mapRadioBoxGroup)
	{
		auto &vecRadioBox = GroupItem.second;

		for (int i = 0; i < vecRadioBox.size(); i++)
		{
			CDUIRadioBoxCtrl *pRadioBoxCtrl = vecRadioBox[i];
			if (NULL == pRadioBoxCtrl) continue;

			if (pRadioBoxCtrl->IsSelected())
			{
				pRadioBoxCtrl->PerformUnSelOther(false);
				pRadioBoxCtrl->PerformActiveBindTabPage();

				break;
			}
		}
	}

	CDUIGlobal::PerformNotifyChildAdd(this, m_pRootCtrl);

	//refresh view
	if (m_pRootCtrl)
	{
		m_pRootCtrl->NeedRefreshView();
	}

	Invalidate();

	return true;
}

bool CDUIWndBase::ReleaseRootCtrl()
{
	CDUIContainerCtrl *pRootCtrl = DetachRootCtrl();

	MMSafeDelete(pRootCtrl);

	return true;
}

CDUIContainerCtrl * CDUIWndBase::GetRootCtrl()
{
	return m_pRootCtrl;
}

CDUIContainerCtrl * CDUIWndBase::DetachRootCtrl()
{
	CDUIGlobal::PerformNotifyChildRemove(this, m_pRootCtrl);

	CDUIContainerCtrl *pRootCtrl = m_pRootCtrl;
	if (pRootCtrl)
	{
		pRootCtrl->SetWndOwner(NULL);
	}

	m_pRootCtrl = NULL;
	m_pCaptureCtrl = NULL;
	m_pFocusCtrl = NULL;
	m_pHoverCtrl = NULL;
	m_pEventCtrl = NULL;
	m_pWinDragCtrl = NULL;
	m_pWinDragEnterCtrl = NULL;

	m_ptMousePosLast = {};

	m_vecMouseEnterCtrl.clear();
	m_mapControl.clear();
	m_vecAsynNotify.clear();
	RemoveAllRadioBoxGroup();
	RemoveAllTimer();

	return pRootCtrl;
}

bool CDUIWndBase::InitControls(CDUIControlBase *pControl)
{
	ASSERT(pControl);
	if (NULL == pControl) return false;
	pControl->SetWndOwner(MMInterfaceHelper(CDUIWnd, this));
	pControl->FindControl(__FindControlFromIDHash, this, DuiFind_All);

	return true;
}

void CDUIWndBase::InitControlIDHash(CDUIControlBase *pControl)
{
	__FindControlFromIDHash(pControl, this);

	return;
}

void CDUIWndBase::UnInitControlIDHash(CDUIControlBase *pControl)
{
	if (NULL == pControl) return;

	m_mapControl.erase(pControl->GetCtrlID());

	return;
}

void CDUIWndBase::ReapControl(CDUIControlBase *pControl)
{
	if (NULL == pControl) return;
	if (pControl == m_pCaptureCtrl) m_pCaptureCtrl = NULL;
	if (pControl == m_pFocusCtrl) m_pFocusCtrl = NULL;
	if (pControl == m_pHoverCtrl) m_pHoverCtrl = NULL;
	if (pControl == m_pEventCtrl) m_pEventCtrl = NULL;
	if (pControl == m_pWinDragCtrl) m_pWinDragCtrl = NULL;
	if (pControl == m_pWinDragEnterCtrl) m_pWinDragEnterCtrl = NULL;

	m_vecMouseEnterCtrl.erase(std::remove(m_vecMouseEnterCtrl.begin(), m_vecMouseEnterCtrl.end(), pControl), m_vecMouseEnterCtrl.end());
	m_mapControl.erase(pControl->GetCtrlID());

	KillTimer(pControl);

	for (int i = m_vecAsynNotify.size() - 1; i >= 0; i--)
	{
		DuiNotify &Notify = m_vecAsynNotify[i];
		if (Notify.pNotifyCtrl == pControl
			|| Notify.DuiNotifyExtend.TreeView.pTreeNode == pControl
			|| Notify.DuiNotifyExtend.TreeView.pRootNode == pControl
			|| Notify.DuiNotifyExtend.TreeView.pRootView == pControl)
		{
			m_vecAsynNotify.erase(m_vecAsynNotify.begin() + i);
		}
	}

	return;
}

bool CDUIWndBase::VerifyControl(CDUIControlBase *pControl)
{
	if (NULL == pControl || NULL == m_pRootCtrl) return false;

	if (pControl == m_pRootCtrl) return true;

	return m_pRootCtrl->VerifyChild(pControl);
}

UINT_PTR CDUIWndBase::GetTag()
{
	return m_AttributeUserTag.GetValue();
}

void CDUIWndBase::SetTag(UINT_PTR pTag)
{
	if (pTag == m_AttributeUserTag.GetValue()) return;

	m_AttributeUserTag.SetValue(pTag);

	return;
}

bool CDUIWndBase::SetTimer(CDUIPropertyObject *pPropObj, UINT uTimerID, UINT uElapse)
{
	return false;
}

bool CDUIWndBase::FindTimer(CDUIPropertyObject *pPropObj, UINT uTimerID)
{
	ASSERT(pPropObj != NULL);
	if (NULL == pPropObj) return false;

	auto FindIt = find_if(m_vecTimers.begin(), m_vecTimers.end(), [&](DuiTimerInfo &TimerInfo)
	{
		return TimerInfo.pPropObj == pPropObj
			&& TimerInfo.hWnd == m_hWnd
			&& TimerInfo.nLocalID == uTimerID
			&& false == TimerInfo.bKilled;
	});

	return FindIt != m_vecTimers.end();
}

bool CDUIWndBase::KillTimer(CDUIPropertyObject *pPropObj, UINT uTimerID)
{
	return false;
}

bool CDUIWndBase::KillTimer(CDUIPropertyObject *pPropObj)
{
	return false;
}

bool CDUIWndBase::RemoveAllTimer()
{
	return false;
}

bool CDUIWndBase::AsyncTask(std::function<void()> pFunc)
{
	if (false == (bool)pFunc) return false;

	std::function<void()> *pTask = new (std::nothrow) std::function<void()>(std::move(pFunc));
	if (NULL == pTask) return false;

	if (0 == PostMessage(WM_DUIASYNC, (WPARAM)pTask, 0))
	{
		delete pTask;
		return false;
	}

	return true;
}

UINT_PTR CDUIWndBase::TimerTask(unsigned int ms, bool bRepeat, std::function<void()> pFunc)
{
	if (false == (bool)pFunc || 0 == ms) return 0;

	const UINT uTimerID = m_TimerStore.uNextID++;
	if (false == SetTimer(this, uTimerID, ms))
	{
		return 0;
	}

	DuiTimerStore::Item &Item = m_TimerStore.mapTimer[uTimerID];
	Item.pFunc = std::move(pFunc);
	Item.bRepeat = bRepeat;

	return uTimerID;
}

bool CDUIWndBase::StopTimer(UINT_PTR timerId)
{
	m_TimerStore.mapTimer.erase(timerId);

	return KillTimer(this, (UINT)timerId);
}

bool CDUIWndBase::AddRadioBoxToGroup(CDUIRadioBoxCtrl *pControl)
{
	if (pControl == NULL) return false;

	auto &vecRadioBox = m_mapRadioBoxGroup[pControl->GetGroupName()];
	for (int i = 0; i < vecRadioBox.size(); i++)
	{
		if (vecRadioBox[i] == pControl)
		{
			return false;
		}
	}

	vecRadioBox.push_back(pControl);

	return true;
}

VecDuiRadioBoxCtrl CDUIWndBase::GetRadioBoxGroup(LPCTSTR lpszGroup)
{
	auto FindIt = m_mapRadioBoxGroup.find(lpszGroup);

	return FindIt != m_mapRadioBoxGroup.end() ? FindIt->second : VecDuiRadioBoxCtrl();
}

bool CDUIWndBase::RemoveRadioBoxFromGroup(CDUIRadioBoxCtrl *pControl)
{
	if (NULL == pControl) return false;

	auto FindIt = m_mapRadioBoxGroup.find(pControl->GetGroupName());
	if (FindIt == m_mapRadioBoxGroup.end()) return false;

	auto &vecRadioBox = FindIt->second;

	for (int i = 0; i < (int)vecRadioBox.size(); i++)
	{
		if (vecRadioBox[i] == pControl)
		{
			vecRadioBox.erase(vecRadioBox.begin() + i);

			return true;
		}
	}

	return false;
}

void CDUIWndBase::RemoveAllRadioBoxGroup()
{
	m_mapRadioBoxGroup.clear();

	return;
}

CDUIRadioBoxCtrl* CDUIWndBase::GetRadioBoxOfBindTabCtrl(CDUITabLayoutCtrl *pTabCtrl, long lBindIndex)
{
	if (pTabCtrl == NULL || lBindIndex < 0) return NULL;

	for (auto GroupItem : m_mapRadioBoxGroup)
	{
		VecDuiRadioBoxCtrl &vecRadioBox = GroupItem.second;
		if (vecRadioBox.empty()) continue;

		CDUIRadioBoxCtrl *pRadioBoxCtrl = vecRadioBox[0];
		if (NULL == pRadioBoxCtrl) continue;

		if (pRadioBoxCtrl->GetBindTabCtrl() != pTabCtrl) continue;

		for (int i = 0; i < vecRadioBox.size(); i++)
		{
			pRadioBoxCtrl = vecRadioBox[i];
			if (NULL == pRadioBoxCtrl) continue;

			if (pRadioBoxCtrl->GetBindTabCtrlIndex() == lBindIndex) return pRadioBoxCtrl;
		}
	}

	return NULL;
}

HWND CDUIWndBase::GetToolTipWindow() const
{
	return m_hWndTooltip;
}

int CDUIWndBase::GetToolTipHoverTime() const
{
	return m_nToolTipHoverTime;
}

void CDUIWndBase::SetToolTipHoverTime(int nTime)
{
	m_nToolTipHoverTime = nTime;
}

void CDUIWndBase::RefreshToolTip(CMMString strToolTip)
{
	return;
}

void CDUIWndBase::RefreshLayout()
{
	CDUIRect rcClient = GetClientRect();
	if (false == IsRefreshViewNeeded() || NULL == m_pRootCtrl || IsMinimized()) return;

	//layout
	bool bNeedLayoutMsg = false;
	m_bRefreshViewNeeded = false;
	if (m_pRootCtrl && m_pRootCtrl->IsRefreshViewNeeded())
	{
		m_pRootCtrl->OnDuiSize(rcClient);
		bNeedLayoutMsg = true;
	}
	else
	{
		m_vecFoundControls.clear();
		m_pRootCtrl->FindControl(__FindControlsFromUpdate, nullptr, DuiFind_Visible | DuiFind_MeFirst | DuiFind_UpdateTest);
		for (auto pControl : m_vecFoundControls)
		{
			pControl->OnDuiSize(pControl->GetModalParentRect());
		}
	}

	//notify
	if (m_bFirstLayout)
	{
		SendNotify(m_pRootCtrl, DuiNotify_WndInited, 0, 0);
	}
	if (bNeedLayoutMsg)
	{
		SendNotify(m_pRootCtrl, DuiNotify_WndLayout, 0, 0);
	}

	//animate wnd
	if (m_bFirstLayout)
	{
		//animation wnd
#ifndef DUI_DESIGN
		if (AnimateWnd_None != GetAnimateWndType())
		{
			StartAnimationWnd();
		}
#endif
	}

	m_bFirstLayout = false;

	return;
}

void CDUIWndBase::Invalidate()
{
	return;
}

bool CDUIWndBase::IsRefreshViewNeeded() const
{
	return m_bRefreshViewNeeded;
}

void CDUIWndBase::NeedRefreshView()
{
	m_bRefreshViewNeeded = true;

	return;
}

POINT CDUIWndBase::GetMousePosLast() const
{
	return m_ptMousePosLast;
}

POINT CDUIWndBase::GetMousePosDown() const
{
	return m_ptMousePosDown;
}

CDUIWndBase::operator HWND() const
{
	return m_hWnd;
}

HWND CDUIWndBase::GetWndHandle()
{
	return m_hWnd;
}

HWND CDUIWndBase::GetParent()
{
	return m_hWndParent;
}

UINT CDUIWndBase::GetWndID() 
{
	return m_uWndID; 
}

HDC CDUIWndBase::GetWndDC()
{
	return m_hDCPaint;
}

void CDUIWndBase::ResizeWnd(int cx /*= -1*/, int cy /*= -1*/)
{
	return;
}

void CDUIWndBase::AdjustWndPos()
{
	return;
}

bool CDUIWndBase::IsMaximized()
{
	return false;
}

bool CDUIWndBase::IsMinimized()
{
	return false;
}

bool CDUIWndBase::IsVirtualWnd() 
{ 
	return false; 
}

bool CDUIWndBase::IsWindowVisible() 
{
	return ::IsWindowVisible(m_hWnd);
}

CMMRect CDUIWndBase::GetWindowRect()
{
	return {};
}

CMMRect CDUIWndBase::GetClientRect()
{
	return {};
}

SIZE CDUIWndBase::GetWndInitSize()
{
	return GetDpiObj().Scale(m_AttributeWndInitSize.GetValue());
}

void CDUIWndBase::SetWndInitSize(int cx, int cy)
{
	if (GetWndInitSize() == DuiDpiScaleWnd(CDUISize(cx, cy))) return;

	m_AttributeWndInitSize.SetValue(cx, cy);

	AdjustWndSize();

	return;
}

SIZE CDUIWndBase::GetWndMinSize()
{
	return GetDpiObj().Scale(m_AttributeWndMinSize.GetValue());
}

void CDUIWndBase::SetWndMinSize(int cx, int cy)
{
	ASSERT(cx >= 0 && cy >= 0);
	if (GetWndMinSize() == DuiDpiScaleWnd(CDUISize(cx, cy))) return;

	m_AttributeWndMinSize.SetValue(cx, cy);

	AdjustWndSize();

	return;
}

SIZE CDUIWndBase::GetWndMaxSize()
{
	return GetDpiObj().Scale(m_AttributeWndMaxSize.GetValue());
}

void CDUIWndBase::SetWndMaxSize(int cx, int cy)
{
	ASSERT(cx >= 0 && cy >= 0);
	if (GetWndMaxSize() == DuiDpiScaleWnd(CDUISize(cx, cy))) return;

	m_AttributeWndMaxSize.SetValue(cx, cy);

	AdjustWndSize();

	return;
}

RECT CDUIWndBase::GetResizeTrack()
{
	return m_AttributeWndResizeTrack.GetValue();
}

void CDUIWndBase::SetResizeTrack(RECT &rcResizeTrack)
{
	if (GetResizeTrack() == rcResizeTrack) return;

	m_AttributeWndResizeTrack.SetValue(rcResizeTrack);

	return;
}

int CDUIWndBase::GetCaptionHeight()
{
	return GetDpiObj().Scale(m_AttributeWndCaptionHeight.GetValue());
}

void CDUIWndBase::SetCaptionHeight(int nHeight)
{
	if (DuiDpiScaleWnd(nHeight) == GetCaptionHeight()) return;

	m_AttributeWndCaptionHeight.SetValue(nHeight);

	return;
}

const CMMDpi & CDUIWndBase::GetDpiObj()
{
	return m_DpiInfo;
}

int CDUIWndBase::GetDpi()
{
	return GetDpiObj().GetDpi();
}

bool CDUIWndBase::SetDpi(int nDpi)
{
	if (nDpi == GetDpi()) return true;

	int nDpiPre = GetDpi();
	m_DpiInfo.SetDpi(nDpi);

	AdjustWndSize();
	AdjustWndPos();

	if (m_pRootCtrl)
	{
		m_pRootCtrl->OnDpiChanged(nDpiPre);
	}

	return true;
}

int CDUIWndBase::GetScale()
{
	return GetDpiObj().GetScale();
}

bool CDUIWndBase::SetScale(int nScale)
{
	if (nScale < 100) return false;

	SetDpi(MulDiv(nScale, 96, 100));

	return true;
}

HBITMAP CDUIWndBase::GetBackgroundBmp()
{
	return m_hBmpBackground;
}

LPBYTE CDUIWndBase::GetBackgroundBits()
{
	return m_pBmpBackgroundBits;
}

void CDUIWndBase::SetWindowPos(HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
	::SetWindowPos(m_hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);

	return;
}

bool CDUIWndBase::CreateCaret(HBITMAP hBmp, int nWidth, int nHeight)
{
	if (false == IsWindow(m_hWnd)) return false;

	m_bCaretActive = false;
	m_rcCaret.right = m_rcCaret.left + nWidth;
	m_rcCaret.bottom = m_rcCaret.top + nHeight;

	return true;
}

bool CDUIWndBase::IsShowCaret()
{
	return m_bCaretActive;
}

void CDUIWndBase::ShowCaret(bool bShow)
{
	if (bShow == IsShowCaret()) return;

	m_bCaretActive = bShow;

	Invalidate();

	return;
}

void CDUIWndBase::SetCaretPos(CDUIPoint pt)
{
	return;
}

BYTE CDUIWndBase::GetWndAlpha()
{
	return m_AttributeWndAlpha.GetValue();
}

void CDUIWndBase::SetWndAlpha(BYTE cbWndAlpha)
{
	if (cbWndAlpha == GetWndAlpha()) return;

	cbWndAlpha = min(255, max(0, cbWndAlpha));
	m_AttributeWndAlpha.SetValue(cbWndAlpha);

	SetWndLayered(true);

	Invalidate();

	return;
}

bool CDUIWndBase::IsWndLayered()
{
	return m_AttributeWndLayered.GetValue();
}

void CDUIWndBase::SetWndLayered(bool bLayered)
{
	m_AttributeWndLayered.SetValue(bLayered);

	return;
}

bool CDUIWndBase::IsGdiplusRenderText()
{
	return m_AttributeGdiplusRenderText.GetValue();
}

void CDUIWndBase::SetGdiplusRenderText(bool bGdiplus)
{
	if (bGdiplus == IsGdiplusRenderText()) return;

	m_AttributeGdiplusRenderText.SetValue(bGdiplus);

	Invalidate();

	return;
}

Gdiplus::TextRenderingHint CDUIWndBase::GetGdiplusRenderTextType()
{
	return (Gdiplus::TextRenderingHint)m_AttributeGdiplusRenderType.GetCurSelItem();
}

void CDUIWndBase::SetGdiplusRenderTextType(Gdiplus::TextRenderingHint RenderType)
{
	if (RenderType == GetGdiplusRenderTextType()) return;

	m_AttributeGdiplusRenderType.SelectItem(RenderType);

	Invalidate();

	return;
}

bool CDUIWndBase::IsGdiplusRenderImage()
{
	return m_AttributeGdiplusRenderImage.GetValue();
}

void CDUIWndBase::SetGdiplusRenderImage(bool bGdiplus)
{
	if (bGdiplus == IsGdiplusRenderImage()) return;

	m_AttributeGdiplusRenderImage.SetValue(bGdiplus);

	Invalidate();

	return;
}

enDuiAnimateWndType CDUIWndBase::GetAnimateWndType()
{
	return (enDuiAnimateWndType)m_AttributeAnimationType.GetCurSelItem();
}

void CDUIWndBase::SetAnimateWndType(enDuiAnimateWndType AnimateWndType)
{
	if (AnimateWndType == GetAnimateWndType()) return;

	m_AttributeAnimationType.SelectItem(AnimateWndType);

	return;
}

int CDUIWndBase::GetAnimateWndFrame()
{
	return m_AttributeAnimationFrame.GetValue();
}

void CDUIWndBase::SetAnimateWndFrame(int nFrame)
{
	if (nFrame == GetAnimateWndFrame()) return;

	m_AttributeAnimationFrame.SetValue(nFrame);

	return;
}

int CDUIWndBase::GetAnimateWndElapse()
{
	return m_AttributeAnimationElapse.GetValue();
}

void CDUIWndBase::SetAnimateWndElapse(int nElapse)
{
	if (nElapse == GetAnimateWndElapse()) return;

	m_AttributeAnimationElapse.SetValue(nElapse);

	return;
}

bool CDUIWndBase::AddINotify(IDuiNotify *pINotify)
{
	if (NULL == pINotify) return false;
	if (find(m_vecINotify.begin(), m_vecINotify.end(), pINotify) != m_vecINotify.end()) return true;

	m_vecINotify.push_back(pINotify);

	return true;
}

bool CDUIWndBase::RemoveINotify(IDuiNotify *pINotify)
{
	m_vecINotify.erase(std::remove(m_vecINotify.begin(), m_vecINotify.end(), pINotify), m_vecINotify.end());

	return true;
}

void CDUIWndBase::SendNotify(DuiNotify &Notify)
{
	if (NULL == Notify.pNotifyCtrl) return;

	CDUIWndBase *pWnd = Notify.pNotifyCtrl->GetWndOwner();
	if (NULL == pWnd) return;

	Notify.uCtrlID = Notify.pNotifyCtrl->GetCtrlID();
	Notify.ptMouse = m_ptMousePosLast;
	Notify.ptMousePre = m_ptMousePosPre;
	Notify.wKeyState = MapWndKeyState();
	Notify.dwTimestamp = ::GetTickCount();

	int nCount = Notify.pNotifyCtrl->GetControlCallBackCount();
	for (int n = 0; n < nCount; n++)
	{
		if (false == pWnd->VerifyControl(Notify.pNotifyCtrl)) return;

		IDuiControlCallBack *pICallBack = Notify.pNotifyCtrl->GetControlCallBack(n);
		if (NULL == pICallBack) continue;

		pICallBack->OnNotify(Notify.pNotifyCtrl, Notify);
	}
	for (int n = 0; n < m_vecINotify.size(); n++)
	{
		IDuiNotify *pINotify = m_vecINotify[n];
		if (NULL == pINotify) continue;

		pINotify->OnNotify(Notify);
	}

	return;
}

void CDUIWndBase::SendNotify(CDUIControlBase *pControl, enDuiNotifyType NotifyType, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
	if (NULL == pControl) return;

	DuiNotify Notify;
	Notify.NotifyType = NotifyType;
	Notify.pNotifyCtrl = pControl;
	Notify.wParam = wParam;
	Notify.lParam = lParam;
	SendNotify(Notify);

	return;
}

void CDUIWndBase::PostNotify(DuiNotify &Notify)
{
	if (NULL == Notify.pNotifyCtrl || this != Notify.pNotifyCtrl->GetWndOwner()) return;

	Notify.ptMouse = m_ptMousePosLast;
	Notify.ptMousePre = m_ptMousePosPre;
	Notify.wKeyState = MapWndKeyState();
	Notify.dwTimestamp = ::GetTickCount();
	m_vecAsynNotify.push_back(Notify);

	PostAppMsg();

	return;
}

void CDUIWndBase::PostNotify(CDUIControlBase *pControl, enDuiNotifyType NotifyType, WPARAM wParam/* = 0*/, LPARAM lParam/* = 0*/)
{
	if (NULL == pControl) return;

	DuiNotify Notify;
	Notify.NotifyType = NotifyType;
	Notify.pNotifyCtrl = pControl;
	Notify.wParam = wParam;
	Notify.lParam = lParam;
	PostNotify(Notify);

	return;
}

CDUIControlBase * CDUIWndBase::FindControl(POINT pt) const
{
	if (NULL == m_pRootCtrl) return NULL;

	return m_pRootCtrl->FindControl(__FindControlFromPoint, &pt, DuiFind_Visible | DuiFind_HitTest | DuiFind_TopFirst);
}

CDUIControlBase * CDUIWndBase::FindControl(UINT uCtrlID) const
{
	auto FindIt = m_mapControl.find(uCtrlID);
	return FindIt != m_mapControl.end() ? static_cast<CDUIControlBase*>(FindIt->second) : NULL;
}

CDUIControlBase * CDUIWndBase::FindControlByShortCut(TCHAR chChar) const
{
	if (NULL == m_pRootCtrl) return NULL;

	DuiFindShortCut FindShortCut = {};
	FindShortCut.chChar = toupper((int)chChar);

	UINT uFlag = DuiFind_Enabled | DuiFind_MeFirst | DuiFind_TopFirst | DuiFind_Visible;
	return m_pRootCtrl->FindControl(__FindControlFromShortcut, &FindShortCut, uFlag);
}

CDUIControlBase * CDUIWndBase::FindControlByDrop(POINT pt) const
{
	if (NULL == m_pRootCtrl) return NULL;

	return m_pRootCtrl->FindControl(__FindControlFromDrop, &pt, DuiFind_Visible | DuiFind_HitTest | DuiFind_TopFirst);
}

CDUIControlBase * CDUIWndBase::FindSubControlByPoint(CDUIContainerCtrl *pParent, POINT pt)
{
	if (NULL == pParent) pParent = GetRootCtrl();
	if (NULL == pParent) return NULL;

	return pParent->FindControl(__FindControlFromPoint, &pt, DuiFind_Visible | DuiFind_HitTest | DuiFind_TopFirst);
}

CDUIControlBase * CDUIWndBase::FindSubControlByID(CDUIContainerCtrl *pParent, UINT uCtrlID)
{
	if (NULL == pParent) pParent = GetRootCtrl();
	if (NULL == pParent) return NULL;

	return pParent->FindControl(__FindControlFromID, (LPVOID)uCtrlID, DuiFind_All);
}

VecDuiControlBase CDUIWndBase::FindSubControlsByClass(CDUIContainerCtrl *pParent, LPCTSTR pstrClass)
{
	if (NULL == pParent) pParent = GetRootCtrl();
	if (NULL == pParent) return {};

	m_vecFoundControls.clear();
	pParent->FindControl(__FindControlsFromClass, (LPVOID)pstrClass, DuiFind_All);

	return m_vecFoundControls;
}

bool CDUIWndBase::SetTimer(UINT uTimerID, UINT nElapse)
{
	return SetTimer(this, uTimerID, nElapse);
}

bool CDUIWndBase::KillTimer(UINT uTimerID)
{
	return KillTimer(this, uTimerID);
}

bool CDUIWndBase::KillTimer()
{
	return KillTimer(this);
}

void CDUIWndBase::InitProperty()
{
	return;
}

void CDUIWndBase::InitComplete()
{
	__super::InitComplete();

	return;
}

bool CDUIWndBase::SaveAttribute(tinyxml2::XMLElement *pNode, bool bIncludeChild)
{
	if (NULL == pNode || NULL == pNode->GetDocument()) return false;

	if (false == __super::SaveAttribute(pNode)) return false;

	//控件
	if (m_pRootCtrl)
	{
		tinyxml2::XMLElement *pNodeCtrl = pNode->GetDocument()->NewElement((LPCSTR)CT2CA(m_pRootCtrl->GetClass()));
		if (!pNodeCtrl) return true;

		m_pRootCtrl->SaveAttribute(pNodeCtrl);

		pNode->LinkEndChild(pNodeCtrl);
	}

	return true;
}

LRESULT CDUIWndBase::OnPreWndMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	return 0;
}

LRESULT CDUIWndBase::OnWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	switch (uMsg)
	{
		case WM_DUIAPP:
		{
			//notify
			DispatchNotifyAsyn();

			//delaydelete
			OnDuiDelayDelete();

			m_bPostedAppMsg = false;

			return 0;
		}
		case WM_DUIASYNC:
		{
			std::function<void()> *pFunc = reinterpret_cast<std::function<void()>*>(wParam);
			if (pFunc)
			{
				try
				{
					(*pFunc)();
				}
				catch (...)
				{
				}

				delete pFunc;
			}

			return 0;
		}
		case WM_CREATE:
		{
			return OnCreate(wParam, lParam);
		}
		case WM_CLOSE:
		{
			return OnClose(wParam, lParam);
		}
		case WM_DESTROY:
		{
			return OnDestroy(wParam, lParam);
		}
#if defined(WIN32) && !defined(UNDER_CE)
		case WM_NCACTIVATE:
		{
			return OnNcActivate(wParam, lParam);
		}
		case WM_NCCALCSIZE:
		{
			return OnNcCalcSize(wParam, lParam);
		}
		case WM_NCPAINT:
		{
			return OnNcPaint(wParam, lParam);
		}
		case WM_NCHITTEST:
		{
			return OnNcHitTest(wParam, lParam);
		}
		case WM_GETMINMAXINFO:
		{
			return OnGetMinMaxInfo(wParam, lParam);
		}
#endif
		case WM_EXITSIZEMOVE:
		{
			return OnExitSizeMove(wParam, lParam);
		}
		case WM_SYSCOMMAND:
		{
			return OnSysCommand(wParam, lParam);
		}
		case WM_LBUTTONDOWN:
		{
			return OnLButtonDown(wParam, lParam);
		}
		case WM_LBUTTONUP:
		{
			return OnLButtonUp(wParam, lParam);
		}
		case WM_LBUTTONDBLCLK:
		{
			return OnLButtonDlk(wParam, lParam);
		}
		case WM_RBUTTONDOWN:
		{
			return OnRButtonDown(wParam, lParam);
		}
		case WM_RBUTTONUP:
		{
			return OnRButtonUp(wParam, lParam);
		}
		case WM_RBUTTONDBLCLK:
		{
			return OnRButtonDlk(wParam, lParam);
		}
		case WM_MOUSEMOVE:
		{
			return OnMouseMove(wParam, lParam);
		}
		case WM_MOUSEHOVER:
		{
			return OnMouseHover(wParam, lParam);
		}
		case WM_MOUSELEAVE:
		{
			return OnMouseLeave(wParam, lParam);
		}
		case WM_MOUSEWHEEL:
		{
			return OnMouseWheel(wParam, lParam);
		}
		case WM_TIMER:
		{
			return OnTimer(wParam, lParam);
		}
		case WM_MOVING:
		{
			return OnMoving(wParam, lParam);
		}
		case WM_MOVE:
		{
			return OnMove(wParam, lParam);
		}
		case WM_SIZING:
		{
			return OnSizing(wParam, lParam);
		}
		case WM_SIZE:
		{
			return OnSize(wParam, lParam);
		}
		case WM_WINDOWPOSCHANGING:
		{
			return OnWindowPosChanging(wParam, lParam);
		}
		case WM_WINDOWPOSCHANGED:
		{
			return OnWindowPosChanged(wParam, lParam);
		}
		case WM_KEYDOWN:
		{
			return OnKeyDown(wParam, lParam);
		}
		case WM_KEYUP:
		{
			return OnKeyUp(wParam, lParam);
		}
		case WM_CHAR:
		{
			return OnChar(wParam, lParam);
		}
		case WM_SYSKEYDOWN:
		{
			return OnSysKeyDown(wParam, lParam);
		}
		case WM_SYSKEYUP:
		{
			return OnSysKeyUp(wParam, lParam);
		}
		case WM_SYSCHAR:
		{
			return OnSysChar(wParam, lParam);
		}
		case WM_ERASEBKGND:
		{
			// We'll do the painting here...
			return 1;
		}
		case WM_CONTEXTMENU:
		{
			return OnContextMenu(wParam, lParam);
		}
		case WM_SETFOCUS:
		{
			return OnSetFocus(wParam, lParam);
		}
		case WM_KILLFOCUS:
		{
			return OnKillFocus(wParam, lParam);
		}
		case WM_SETCURSOR:
		{
			return OnSetCursor(wParam, lParam);
		}
		case WM_DPICHANGED:
		{
			return OnDpiChanged(wParam, lParam);
		}
	}

	return OnOldWndProc(uMsg, wParam, lParam);
}

LRESULT CDUIWndBase::OnOldWndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CDUIWndBase::OnCreate(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CDUIWndBase::OnClose(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CDUIWndBase::OnDestroy(WPARAM wParam, LPARAM lParam)
{
	if (g_pIDuiWndNotify)
	{
		g_pIDuiWndNotify->OnDuiWndDestroy(this);
	}

	return OnOldWndProc(WM_DESTROY, wParam, lParam);
}

LRESULT CDUIWndBase::OnNcActivate(WPARAM wParam, LPARAM lParam)
{
	if (false == IsMinimized()) return (wParam == 0) ? TRUE : FALSE;

	return OnOldWndProc(WM_NCACTIVATE, wParam, lParam);
}

LRESULT CDUIWndBase::OnNcCalcSize(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CDUIWndBase::OnNcPaint(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CDUIWndBase::OnNcHitTest(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CDUIWndBase::OnExitSizeMove(WPARAM wParam, LPARAM lParam)
{
	return OnOldWndProc(WM_EXITSIZEMOVE, wParam, lParam);
}

LRESULT CDUIWndBase::OnSysCommand(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CDUIWndBase::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	CDUIPoint pt(lParam);

	SetCapture();

	//msg
	DuiMessage DuiMsg = {};
	DuiMsg.wParam = wParam;
	DuiMsg.lParam = lParam;
	DuiMsg.ptMousePre = m_ptMousePosLast;
	DuiMsg.ptMouse = pt;

	m_ptMousePosPre = m_ptMousePosLast;
	m_ptMousePosLast = pt;
	m_ptMousePosDown = pt;
	m_dwMouseDownTick = ::GetTickCount();

	//find
	m_pCaptureCtrl = FindSubControlByPoint(m_pRootCtrl, pt);
	if (m_pCaptureCtrl)
	{
		SetFocusControl(m_pCaptureCtrl);

		DuiMsg.pMsgCtrl = m_pCaptureCtrl;
		DuiMsg.pMsgCtrl->OnDuiLButtonDown(pt, DuiMsg);

		//model
		DispatchModelMouseEvent(m_pCaptureCtrl, &CDUIControlBase::OnDuiLButtonDown, pt, DuiMsg, true);
	}

	return OnOldWndProc(WM_LBUTTONDOWN, wParam, lParam);
}

LRESULT CDUIWndBase::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	CDUIPoint pt(lParam);

	ReleaseCapture();

	//msg
	DuiMessage DuiMsg = {};
	DuiMsg.wParam = wParam;
	DuiMsg.lParam = lParam;
	DuiMsg.ptMousePre = m_ptMousePosLast;
	DuiMsg.ptMouse = pt;

	m_ptMousePosPre = m_ptMousePosLast;
	m_ptMousePosLast = pt;
	m_dwMouseDownTick = 0;

	m_pCaptureCtrl = m_pCaptureCtrl ? m_pCaptureCtrl : FindSubControlByPoint(m_pRootCtrl, pt);
	if (m_pCaptureCtrl)
	{
		DuiMsg.pMsgCtrl = m_pCaptureCtrl;
		DuiMsg.pMsgCtrl->OnDuiLButtonUp(pt, DuiMsg);

		//model
		DispatchModelMouseEvent(m_pCaptureCtrl, &CDUIControlBase::OnDuiLButtonUp, pt, DuiMsg, true);
	}

	m_pCaptureCtrl = NULL;

	return OnOldWndProc(WM_LBUTTONUP, wParam, lParam);
}

LRESULT CDUIWndBase::OnLButtonDlk(WPARAM wParam, LPARAM lParam)
{
	CDUIPoint pt(lParam);

	SetCapture();

	//msg
	DuiMessage DuiMsg = {};
	DuiMsg.wParam = wParam;
	DuiMsg.lParam = lParam;
	DuiMsg.ptMousePre = m_ptMousePosLast;
	DuiMsg.ptMouse = pt;

	m_ptMousePosPre = m_ptMousePosLast;
	m_ptMousePosLast = pt;

	//find
	m_pCaptureCtrl = FindSubControlByPoint(m_pRootCtrl, pt);
	if (m_pCaptureCtrl)
	{
		SetFocusControl(m_pCaptureCtrl);

		DuiMsg.pMsgCtrl = m_pCaptureCtrl;
		DuiMsg.pMsgCtrl->OnDuiLButtonDlk(pt, DuiMsg);

		//model
		DispatchModelMouseEvent(m_pCaptureCtrl, &CDUIControlBase::OnDuiLButtonDlk, pt, DuiMsg, true);
	}

	return OnOldWndProc(WM_LBUTTONDBLCLK, wParam, lParam);
}

LRESULT CDUIWndBase::OnRButtonDown(WPARAM wParam, LPARAM lParam)
{
	CDUIPoint pt(lParam);

	SetCapture();

	//msg
	DuiMessage DuiMsg = {};
	DuiMsg.wParam = wParam;
	DuiMsg.lParam = lParam;
	DuiMsg.ptMousePre = m_ptMousePosLast;
	DuiMsg.ptMouse = pt;

	m_ptMousePosPre = m_ptMousePosLast;
	m_ptMousePosLast = pt;
	m_ptMousePosDown = pt;

	//find
	m_pCaptureCtrl = FindSubControlByPoint(m_pRootCtrl, pt);
	if (m_pCaptureCtrl)
	{
		SetFocusControl(m_pCaptureCtrl);

		DuiMsg.pMsgCtrl = m_pCaptureCtrl;
		DuiMsg.pMsgCtrl->OnDuiRButtonDown(pt, DuiMsg);

		//model
		DispatchModelMouseEvent(m_pCaptureCtrl, &CDUIControlBase::OnDuiRButtonDown, pt, DuiMsg, true);
	}

	return OnOldWndProc(WM_RBUTTONDOWN, wParam, lParam);
}

LRESULT CDUIWndBase::OnRButtonUp(WPARAM wParam, LPARAM lParam)
{
	CDUIPoint pt(lParam);

	ReleaseCapture();

	//msg
	DuiMessage DuiMsg = {};
	DuiMsg.wParam = wParam;
	DuiMsg.lParam = lParam;
	DuiMsg.ptMousePre = m_ptMousePosLast;
	DuiMsg.ptMouse = pt;

	m_ptMousePosPre = m_ptMousePosLast;
	m_ptMousePosLast = pt;

	m_pCaptureCtrl = m_pCaptureCtrl ? m_pCaptureCtrl : FindSubControlByPoint(m_pRootCtrl, pt);
	if (m_pCaptureCtrl)
	{
		DuiMsg.pMsgCtrl = m_pCaptureCtrl;
		DuiMsg.pMsgCtrl->OnDuiRButtonUp(pt, DuiMsg);

		//model
		DispatchModelMouseEvent(m_pCaptureCtrl, &CDUIControlBase::OnDuiRButtonUp, pt, DuiMsg, true);
	}

	m_pCaptureCtrl = NULL;

	return OnOldWndProc(WM_RBUTTONUP, wParam, lParam);
}

LRESULT CDUIWndBase::OnRButtonDlk(WPARAM wParam, LPARAM lParam)
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

	//查找
	m_pCaptureCtrl = FindSubControlByPoint(m_pRootCtrl, pt);
	if (m_pCaptureCtrl)
	{
		SetFocusControl(m_pCaptureCtrl);

		DuiMsg.pMsgCtrl = m_pCaptureCtrl;
		DuiMsg.pMsgCtrl->OnDuiRButtonDlk(pt, DuiMsg);

		//model
		DispatchModelMouseEvent(m_pCaptureCtrl, &CDUIControlBase::OnDuiRButtonDlk, pt, DuiMsg, true);
	}

	return OnOldWndProc(WM_RBUTTONDBLCLK, wParam, lParam);
}

LRESULT CDUIWndBase::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CDUIWndBase::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	CDUIPoint pt(lParam);
	m_bMouseTracking = false;

	if (NULL == m_pHoverCtrl) return lRes;

	//msg
	DuiMessage DuiMsg = {};
	DuiMsg.wParam = wParam;
	DuiMsg.lParam = lParam;
	DuiMsg.ptMouse = pt;

	DuiMsg.pMsgCtrl = m_pHoverCtrl;
	m_pHoverCtrl->OnDuiMouseHover(pt, DuiMsg);

	//track modify
	CMMString strToolTip = m_pHoverCtrl->GetToolTip();
	if (strToolTip.empty()) return lRes;
	if (false == m_bRefreshToolTipNeeded && m_strToolTip == strToolTip) return lRes;

	RefreshToolTip(strToolTip);

	return lRes;
}

LRESULT CDUIWndBase::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CDUIWndBase::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
	//variant
	POINT pt = {};
	pt.x = (INT)((SHORT)(LOWORD(lParam)));
	pt.y = (INT)((SHORT)(HIWORD(lParam)));
	ScreenToClient(m_hWnd, &pt);
	m_ptMousePosLast = pt;

	//msg
	DuiMessage DuiMsg = {};
	DuiMsg.lParam = lParam;
	DuiMsg.ptMouse = pt;
	DuiMsg.wParam = wParam;

	//find
	CDUIControlBase *pControl = m_pCaptureCtrl;
	NULL == pControl ? pControl = FindControl(pt) : NULL;

	if (pControl)
	{
		m_pEventCtrl = pControl;
		DuiMsg.pMsgCtrl = m_pEventCtrl;
		bool bRes = DuiMsg.pMsgCtrl->OnDuiMouseWheel(pt, DuiMsg);

		if (false == bRes)
		{
			while (m_pEventCtrl = m_pEventCtrl->GetParent())
			{
				if (m_pEventCtrl
					&& m_pEventCtrl->IsEnabled()
					&& false == m_pEventCtrl->IsMouseThrough()
					&& ((m_pEventCtrl->GetHorizScrollBar() && m_pEventCtrl->GetHorizScrollBar()->IsVisible())
						|| (m_pEventCtrl->GetVertScrollBar() && m_pEventCtrl->GetVertScrollBar()->IsVisible())))
				{
					DuiMsg.pMsgCtrl = m_pEventCtrl;
					DuiMsg.pMsgCtrl->OnDuiMouseWheel(pt, DuiMsg);

					break;
				}
			}
		}
	}

	// Let's make sure that the scroll item below the cursor is the same as before...
	SendMessage(WM_MOUSEMOVE, 0, (LPARAM)MAKELPARAM(m_ptMousePosLast.x, m_ptMousePosLast.y));

	return OnOldWndProc(WM_MOUSEWHEEL, wParam, lParam);
}

LRESULT CDUIWndBase::OnTimer(WPARAM wParam, LPARAM lParam)
{
	DuiMessage DuiMsg = {};

	for (int n = 0; n < m_vecTimers.size(); n++)
	{
		auto TimerInfo = m_vecTimers[n];
		if (NULL == TimerInfo.pPropObj) continue;

		if (TimerInfo.hWnd == m_hWnd
			&& TimerInfo.uWinTimer == wParam
			&& TimerInfo.bKilled == false)
		{
			DuiMsg.pMsgCtrl = TimerInfo.pControl;
			DuiMsg.wParam = TimerInfo.nLocalID;
			DuiMsg.ptMouse = m_ptMousePosLast;
			if (DuiMsg.pMsgCtrl)
			{
				DuiMsg.pMsgCtrl->OnDuiTimer(TimerInfo.nLocalID, DuiMsg);
			}
			else
			{
				auto it = m_TimerStore.mapTimer.find(TimerInfo.nLocalID);
				if (it != m_TimerStore.mapTimer.end())
				{
					std::function<void()> pFunc = it->second.pFunc;
					const bool bRepeat = it->second.bRepeat;
					if (false == bRepeat)
					{
						StopTimer(TimerInfo.nLocalID);
					}
					if (pFunc)
					{
						try
						{
							pFunc();
						}
						catch (...)
						{
						}
					}
				}
				else
				{
					CDUIAnimationWnd::OnAnimationElapse(TimerInfo.nLocalID);
				}
			}

			break;
		}
	}

	return OnOldWndProc(WM_TIMER, wParam, lParam);
}

LRESULT CDUIWndBase::OnMoving(WPARAM wParam, LPARAM lParam)
{
	return OnOldWndProc(WM_MOVING, wParam, lParam);
}

LRESULT CDUIWndBase::OnMove(WPARAM wParam, LPARAM lParam)
{
	return OnOldWndProc(WM_MOVE, wParam, lParam);
}

LRESULT CDUIWndBase::OnSizing(WPARAM wParam, LPARAM lParam)
{
	return OnOldWndProc(WM_SIZING, wParam, lParam);
}

LRESULT CDUIWndBase::OnSize(WPARAM wParam, LPARAM lParam)
{
	ReleasePaintScene();

	if (m_pRootCtrl)
	{
		m_pRootCtrl->NeedRefreshView();
	}

	//design refresh
	if (g_pIDuiWndNotify)
	{
		g_pIDuiWndNotify->OnDuiWndSize(this);
	}

	return 0;
}

LRESULT CDUIWndBase::OnWindowPosChanging(WPARAM wParam, LPARAM lParam)
{
	return OnOldWndProc(WM_WINDOWPOSCHANGING, wParam, lParam);
}

LRESULT CDUIWndBase::OnWindowPosChanged(WPARAM wParam, LPARAM lParam)
{
	return OnOldWndProc(WM_WINDOWPOSCHANGED, wParam, lParam);
}

LRESULT CDUIWndBase::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	do
	{
		//close
		if (VK_ESCAPE == wParam)
		{
			Close(Dui_CtrlIDInner_BtnCancel);
		}

		if (NULL == m_pFocusCtrl && NULL == m_pCaptureCtrl) break;

		m_pEventCtrl = m_pCaptureCtrl ? m_pCaptureCtrl : m_pFocusCtrl;

		DuiMessage DuiMsg = {};
		DuiMsg.wParam = wParam;
		DuiMsg.lParam = lParam;
		DuiMsg.pMsgCtrl = m_pEventCtrl;
		DuiMsg.chKey = (TCHAR)DuiMsg.wParam;
		DuiMsg.ptMouse = m_ptMousePosLast;
		DuiMsg.wKeyState = MapWndKeyState();
		DuiMsg.dwTimestamp = ::GetTickCount();

		if (DuiMsg.pMsgCtrl->IsEnabled())
		{
			DuiMsg.pMsgCtrl->OnDuiKeyDown(DuiMsg);
		}

		//model
		DispatchModelKeyboardEvent(m_pEventCtrl, &CDUIControlBase::OnDuiKeyDown, DuiMsg);

	} while (false);

	return OnOldWndProc(WM_KEYDOWN, wParam, lParam);
}

LRESULT CDUIWndBase::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
	do
	{
		if (NULL == m_pFocusCtrl && NULL == m_pCaptureCtrl) break;

		m_pEventCtrl = m_pCaptureCtrl ? m_pCaptureCtrl : m_pFocusCtrl;

		DuiMessage DuiMsg = {};
		DuiMsg.wParam = wParam;
		DuiMsg.lParam = lParam;
		DuiMsg.pMsgCtrl = m_pEventCtrl;
		DuiMsg.chKey = (TCHAR)DuiMsg.wParam;
		DuiMsg.ptMouse = m_ptMousePosLast;
		DuiMsg.wKeyState = MapWndKeyState();
		DuiMsg.dwTimestamp = ::GetTickCount();

		if (DuiMsg.pMsgCtrl->IsEnabled())
		{
			DuiMsg.pMsgCtrl->OnDuiKeyUp(DuiMsg);
		}

		//model
		DispatchModelKeyboardEvent(m_pEventCtrl, &CDUIControlBase::OnDuiKeyUp, DuiMsg);

	} while (false);

	return OnOldWndProc(WM_KEYUP, wParam, lParam);
}

LRESULT CDUIWndBase::OnChar(WPARAM wParam, LPARAM lParam)
{
	do
	{
		if (NULL == m_pFocusCtrl && NULL == m_pCaptureCtrl) break;

		m_pEventCtrl = m_pCaptureCtrl ? m_pCaptureCtrl : m_pFocusCtrl;

		DuiMessage DuiMsg = {};
		DuiMsg.wParam = wParam;
		DuiMsg.lParam = lParam;
		DuiMsg.pMsgCtrl = m_pEventCtrl;
		DuiMsg.chKey = (TCHAR)DuiMsg.wParam;
		DuiMsg.ptMouse = m_ptMousePosLast;
		DuiMsg.wKeyState = MapWndKeyState();
		DuiMsg.dwTimestamp = ::GetTickCount();

		if (DuiMsg.pMsgCtrl->IsEnabled())
		{
			DuiMsg.pMsgCtrl->OnDuiChar(DuiMsg);
		}

		//model
		DispatchModelKeyboardEvent(m_pEventCtrl, &CDUIControlBase::OnDuiChar, DuiMsg);

	} while (false);

	return OnOldWndProc(WM_CHAR, wParam, lParam);
}

LRESULT CDUIWndBase::OnSysKeyDown(WPARAM wParam, LPARAM lParam)
{
	do
	{
		if (NULL == m_pFocusCtrl && NULL == m_pCaptureCtrl) break;

		m_pEventCtrl = m_pCaptureCtrl ? m_pCaptureCtrl : m_pFocusCtrl;

		DuiMessage DuiMsg = {};
		DuiMsg.wParam = wParam;
		DuiMsg.lParam = lParam;
		DuiMsg.pMsgCtrl = m_pEventCtrl;
		DuiMsg.chKey = (TCHAR)DuiMsg.wParam;
		DuiMsg.ptMouse = m_ptMousePosLast;
		DuiMsg.wKeyState = MapWndKeyState();
		DuiMsg.dwTimestamp = ::GetTickCount();

		if (DuiMsg.pMsgCtrl->IsEnabled())
		{
			DuiMsg.pMsgCtrl->OnDuiSysKeyDown(DuiMsg);
		}

		//model
		DispatchModelKeyboardEvent(m_pEventCtrl, &CDUIControlBase::OnDuiSysKeyDown, DuiMsg);

	} while (false);

	return OnOldWndProc(WM_SYSKEYDOWN, wParam, lParam);
}

LRESULT CDUIWndBase::OnSysKeyUp(WPARAM wParam, LPARAM lParam)
{
	do
	{
		if (NULL == m_pFocusCtrl && NULL == m_pCaptureCtrl) break;

		m_pEventCtrl = m_pCaptureCtrl ? m_pCaptureCtrl : m_pFocusCtrl;

		DuiMessage DuiMsg = {};
		DuiMsg.wParam = wParam;
		DuiMsg.lParam = lParam;
		DuiMsg.pMsgCtrl = m_pEventCtrl;
		DuiMsg.chKey = (TCHAR)DuiMsg.wParam;
		DuiMsg.ptMouse = m_ptMousePosLast;
		DuiMsg.wKeyState = MapWndKeyState();
		DuiMsg.dwTimestamp = ::GetTickCount();

		if (DuiMsg.pMsgCtrl->IsEnabled())
		{
			DuiMsg.pMsgCtrl->OnDuiSysKeyUp(DuiMsg);
		}

		//model
		DispatchModelKeyboardEvent(m_pEventCtrl, &CDUIControlBase::OnDuiSysKeyUp, DuiMsg);

	} while (false);

	return OnOldWndProc(WM_SYSKEYUP, wParam, lParam);
}

LRESULT CDUIWndBase::OnSysChar(WPARAM wParam, LPARAM lParam)
{
	do
	{
		m_pEventCtrl = m_pCaptureCtrl ? m_pCaptureCtrl : m_pFocusCtrl;
		if (NULL == m_pEventCtrl)
		{
			m_pEventCtrl = FindControlByShortCut(wParam);
		}
		if (NULL == m_pEventCtrl)
		{
			break;
		}

		DuiMessage DuiMsg = {};
		DuiMsg.wParam = wParam;
		DuiMsg.lParam = lParam;
		DuiMsg.pMsgCtrl = m_pEventCtrl;
		DuiMsg.chKey = (TCHAR)DuiMsg.wParam;
		DuiMsg.ptMouse = m_ptMousePosLast;
		DuiMsg.wKeyState = MapWndKeyState();
		DuiMsg.dwTimestamp = ::GetTickCount();

		if (DuiMsg.pMsgCtrl->IsEnabled())
		{
			DuiMsg.pMsgCtrl->OnDuiSysChar(DuiMsg);

			//shortcut
			if (NULL == m_pCaptureCtrl 
				&& NULL == m_pFocusCtrl
				&& m_pEventCtrl)
			{
				m_pEventCtrl->SetFocus();
				m_pEventCtrl->Active();
			}

		}

		//model
		DispatchModelKeyboardEvent(m_pEventCtrl, &CDUIControlBase::OnDuiSysChar, DuiMsg);

	} while (false);

	return OnOldWndProc(WM_SYSCHAR, wParam, lParam);
}

LRESULT CDUIWndBase::OnPaint(CDUIRect rcPaint)
{
	return 0;
}

LRESULT CDUIWndBase::OnGetMinMaxInfo(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	return 0;
}

LRESULT CDUIWndBase::OnImeComPosition(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	return 0;
}

LRESULT CDUIWndBase::OnContextMenu(WPARAM wParam, LPARAM lParam)
{
	do
	{
		//pt
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		ScreenToClient(m_hWnd, &pt);

		//menu ctrl
		m_pEventCtrl = m_pCaptureCtrl ? m_pCaptureCtrl : m_pFocusCtrl;
		while (m_pEventCtrl && false == m_pEventCtrl->IsContextMenu())
		{
			m_pEventCtrl = m_pEventCtrl->GetParent();
		}

		if (NULL == m_pEventCtrl) break;

		DuiMessage DuiMsg = {};
		DuiMsg.wParam = wParam;
		DuiMsg.lParam = lParam;
		DuiMsg.pMsgCtrl = m_pEventCtrl;
		DuiMsg.ptMouse = pt;
		DuiMsg.wKeyState = (WORD)DuiMsg.wParam;
		DuiMsg.dwTimestamp = ::GetTickCount();

		if (DuiMsg.pMsgCtrl->IsContextMenu())
		{
			DuiMsg.pMsgCtrl->OnDuiContextMenu(DuiMsg);
		}

		//model
		DispatchModelKeyboardEvent(m_pEventCtrl, &CDUIControlBase::OnDuiContextMenu, DuiMsg);

	} while (false);

	return OnOldWndProc(WM_CONTEXTMENU, wParam, lParam);
}

LRESULT CDUIWndBase::OnSetFocus(WPARAM wParam, LPARAM lParam)
{
	return OnOldWndProc(WM_SETFOCUS, wParam, lParam);
}

LRESULT CDUIWndBase::OnKillFocus(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CDUIWndBase::OnSetCursor(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CDUIWndBase::OnDpiChanged(WPARAM wParam, LPARAM lParam)
{
	return OnOldWndProc(WM_DPICHANGED, wParam, lParam);
}

LRESULT CDUIWndBase::OnCommand(WPARAM wParam, LPARAM lParam)
{
	do
	{
		if (NULL == m_pFocusCtrl && NULL == m_pCaptureCtrl) break;

		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		ScreenToClient(m_hWnd, &pt);

		m_pEventCtrl = m_pCaptureCtrl ? m_pCaptureCtrl : m_pFocusCtrl;

		DuiMessage DuiMsg = {};
		DuiMsg.wParam = wParam;
		DuiMsg.lParam = lParam;
		DuiMsg.pMsgCtrl = m_pEventCtrl;
		DuiMsg.ptMouse = pt;
		DuiMsg.wKeyState = MapWndKeyState();
		DuiMsg.dwTimestamp = ::GetTickCount();

		DuiMsg.pMsgCtrl->OnDuiCommand(DuiMsg);

		//model
		DispatchModelEvent(m_pEventCtrl, &CDUIControlBase::OnDuiCommand, DuiMsg);

	} while (false);

	return OnOldWndProc(WM_COMMAND, wParam, lParam);
}

void CDUIWndBase::OnFinalMessage()
{
	RemoveINotify(this);

	if (GetRootCtrl())
	{
		GetRootCtrl()->ReapControl();
	}

	m_hWnd = NULL;
	m_bSubWindow = false;

	if (g_pIDuiWndNotify)
	{
		g_pIDuiWndNotify->OnDuiWndDestroy(this);
	}

	return;
}

void CDUIWndBase::OnDuiTimer(const DuiNotify &Notify)
{
	return;
}

void CDUIWndBase::OnDuiClick(const DuiNotify &Notify)
{
	if (Dui_CtrlIDInner_BtnClose == Notify.uCtrlID
		|| Dui_CtrlIDInner_BtnCancel == Notify.uCtrlID
		|| Dui_CtrlIDInner_BtnOk == Notify.uCtrlID)
	{
		m_uCtrlIDClose = Notify.uCtrlID;

		if (AnimateWnd_None != GetAnimateWndType())
		{
			StartAnimationWnd(false);

			return;
		}

		Close(Dui_CtrlIDInner_BtnOk == Notify.uCtrlID ? Notify.uCtrlID : Dui_CtrlIDInner_BtnCancel);

		return;
	}
	else if (Dui_CtrlIDInner_BtnMin == Notify.uCtrlID)
	{
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);

		return;
	}
	else if (Dui_CtrlIDInner_BtnMax == Notify.uCtrlID)
	{
		SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

		return;
	}
	else if (Dui_CtrlIDInner_BtnRestore == Notify.uCtrlID)
	{
		SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);

		return;
	}

	return;
}

void CDUIWndBase::OnDuiWndAnimateFinish(const DuiNotify &Notify)
{
	if (false == Notify.DuiNotifyExtend.WndAnimate.bShow)
	{
		ShowWindow(false);

		Close(Dui_CtrlIDInner_BtnOk == m_uCtrlIDClose ? m_uCtrlIDClose : Dui_CtrlIDInner_BtnCancel);
	}

	return;
}

void CDUIWndBase::OnDuiDelayDelete()
{
	while (false == m_queDelayDelete.empty())
	{
		auto pControl = m_queDelayDelete.front();
		m_queDelayDelete.pop_front();

		MMSafeDelete(pControl);
	}

	return;
}

void CDUIWndBase::OnDpiChanged(int nScalePre)
{
	nScalePre = max(100, nScalePre);
	SetDpi(CDUIGlobal::GetInstance()->GetDpi());

	//wndsize
	if (false == IsMaximized())
	{
		CDUIRect rcWnd = GetWindowRect();
		rcWnd.right = rcWnd.left + (rcWnd.GetWidth()) * (GetScale() * 1.0f / nScalePre);
		rcWnd.bottom = rcWnd.top + (rcWnd.GetHeight()) * (GetScale() * 1.0f / nScalePre);
		::SetWindowPos(GetWndHandle(), NULL, rcWnd.left, rcWnd.top, rcWnd.GetWidth(), rcWnd.GetHeight(), SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOREDRAW);
	}

	//refresh
	if (GetRootCtrl())
	{
		GetRootCtrl()->OnDpiChanged(nScalePre);
		GetRootCtrl()->NeedRefreshView();
	}

	SendNotify(GetRootCtrl(), DuiNotify_DpiChanged);

	return;
}

void CDUIWndBase::AdjustWndSize()
{
	if (false == IsWindow(m_hWnd)) return;

	CDUISize szWndInit = GetWndInitSize();
	if (szWndInit.cx > 0 && szWndInit.cy > 0)
	{
		CDUISize szWndMax = GetWndMaxSize();
		CDUISize szWndMin = GetWndMinSize();
		szWndInit.cx = szWndMax.cx > 0 ? min(szWndInit.cx, szWndMax.cx) : szWndInit.cx;
		szWndInit.cy = szWndMax.cy > 0 ? min(szWndInit.cy, szWndMax.cy) : szWndInit.cy;
		szWndInit.cx = max(szWndInit.cx, szWndMin.cx);
		szWndInit.cy = max(szWndInit.cy, szWndMin.cy);
		::SetWindowPos(m_hWnd, NULL, 0, 0, szWndInit.cx, szWndInit.cy, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
	}

	return;
}

void CDUIWndBase::DelayDelete(CDUIControlBase *pControl)
{
	if (NULL == pControl) return;

	pControl->ReapControl();
	pControl->SetWndOwner(NULL);
	pControl->SetParent(NULL);
	m_queDelayDelete.push_back(pControl);
	PostAppMsg();

	return;
}

void CDUIWndBase::DispatchNotifyAsyn()
{
	VecDuiNotify vecAsynNotify = m_vecAsynNotify;
	m_vecAsynNotify.clear();

	for (auto &Notify : vecAsynNotify)
	{
		if (NULL == Notify.pNotifyCtrl) continue;

		CDUIWndBase *pWnd = Notify.pNotifyCtrl->GetWndOwner();
		if (NULL == pWnd) continue;

		int nCount = Notify.pNotifyCtrl->GetControlCallBackCount();
		for (int n = 0; n < nCount; n++)
		{
			if (false == pWnd->VerifyControl(Notify.pNotifyCtrl)) break;

			IDuiControlCallBack *pICallBack = Notify.pNotifyCtrl->GetControlCallBack(n);
			if (NULL == pICallBack) continue;

			pICallBack->OnNotify(Notify.pNotifyCtrl, Notify);
		}
		for (int n = 0; n < m_vecINotify.size(); n++)
		{
			IDuiNotify *pINotify = m_vecINotify[n];
			if (NULL == pINotify) continue;

			pINotify->OnNotify(Notify);
		}
	}

	return;
}

void CDUIWndBase::PostAppMsg()
{
	if (false == m_bPostedAppMsg)
	{
		PostMessage(WM_DUIAPP, 0, 0);

		m_bPostedAppMsg = true;
	}

	return;
}

void CDUIWndBase::EnsurePaintScene()
{
	return;
}

void CDUIWndBase::ReleasePaintScene()
{
	return;
}

void CDUIWndBase::DispatchModelMouseEvent(CDUIControlBase *pCtrl, MouseEventPtr pFunc, const CDUIPoint &pt, const DuiMessage &Msg, bool bClick)
{
	return;
}

void CDUIWndBase::DispatchModelKeyboardEvent(CDUIControlBase *pCtrl, KeyboardEventPtr pFunc, const DuiMessage &Msg)
{
	return;
}

void CDUIWndBase::DispatchModelEvent(CDUIControlBase* pCtrl, ModelEventPtr pFunc, const DuiMessage &Msg)
{
	if (pCtrl && pCtrl->GetOwnerModelCtrl())
	{
		CDUIControlBase* pOwnerModelCtrl = pCtrl->GetOwnerModelCtrl();
		(pOwnerModelCtrl->*pFunc)(Msg);
	}

	return;
}

VecDuiControlBase & CDUIWndBase::GetFoundControls()
{
	return m_vecFoundControls;
}

CDUIControlBase * CALLBACK CDUIWndBase::__FindControlFromIDHash(CDUIControlBase *pThis, LPVOID pData)
{
	CDUIWndBase *pManager = static_cast<CDUIWndBase*>(pData);
	UINT uCtrlID = pThis->GetCtrlID();
	if (0 == uCtrlID) return NULL;
	// Add this control to the hash list
	pManager->m_mapControl[uCtrlID] = pThis;
	return NULL; // Attempt to add all controls
}

CDUIControlBase * CALLBACK CDUIWndBase::__FindControlFromCount(CDUIControlBase* /*pThis*/, LPVOID pData)
{
	int* pnCount = static_cast<int*>(pData);
	(*pnCount)++;
	return NULL;  // Count all controls
}

CDUIControlBase * CALLBACK CDUIWndBase::__FindControlFromPoint(CDUIControlBase *pThis, LPVOID pData)
{
	LPPOINT pPoint = static_cast<LPPOINT>(pData);
	CDUIRect rcCtrl = pThis->GetAbsoluteRect();
	return ::PtInRect(&rcCtrl, *pPoint) ? pThis : NULL;
}

CDUIControlBase * CALLBACK CDUIWndBase::__FindControlFromShortcut(CDUIControlBase *pThis, LPVOID pData)
{
	if (false == pThis->IsVisible()) return NULL;

	DuiFindShortCut *pFindShortCut = static_cast<DuiFindShortCut*>(pData);
	if (pFindShortCut->chChar == toupper(pThis->GetShortcut())) return pThis;

	return NULL;
}

CDUIControlBase * CALLBACK CDUIWndBase::__FindControlFromDrop(CDUIControlBase *pThis, LPVOID pData)
{
	LPPOINT pPoint = static_cast<LPPOINT>(pData);
	CDUIRect rcCtrl = pThis->GetAbsoluteRect();
	return ::PtInRect(&rcCtrl, *pPoint) && pThis->IsWinDropEnabled() ? pThis : nullptr;
}

CDUIControlBase * CALLBACK CDUIWndBase::__FindControlFromID(CDUIControlBase *pThis, LPVOID pData)
{
	UINT uFindID = *(static_cast<UINT*>(pData));
	UINT uCtrlID = pThis->GetCtrlID();

	return (uFindID == uCtrlID) ? pThis : NULL;
}

CDUIControlBase * CALLBACK CDUIWndBase::__FindControlsFromClass(CDUIControlBase *pThis, LPVOID pData)
{
	LPCTSTR pstrType = static_cast<LPCTSTR>(pData);
	LPCTSTR pType = pThis->GetClass();
	if (_tcscmp(pstrType, _T("*")) == 0 || _tcscmp(pstrType, pType) == 0)
	{
		if (pThis->GetWndOwner())
		{
			pThis->GetWndOwner()->GetFoundControls().push_back(pThis);
		}
	}

	return NULL;
}

CDUIControlBase * CALLBACK CDUIWndBase::__FindControlsFromUpdate(CDUIControlBase *pThis, LPVOID pData)
{
	if (NULL == pThis) return NULL;

	if (pThis->IsRefreshViewNeeded() && pThis->GetWndOwner())
	{
		pThis->GetWndOwner()->GetFoundControls().push_back(pThis);

		return pThis;
	}

	return nullptr;
}