#include "StdAfx.h"
#include "DUIAnimation.h"

//////////////////////////////////////////////////////////////////////////
#define Timer_Spring_ID1				(1990)
#define Timer_Spring_ID2				(1991)
#define Timer_Spring_Frame1				(10)
#define Timer_Spring_Frame2				(10)
#define Timer_Spring_Elapse1			(10)
#define Timer_Spring_Elapse2			(10)

#define Offset_Spring_Total				(30)
#define Min_Spring_Offset				(5)

//////////////////////////////////////////////////////////////////////////
CDUIAnimation::CDUIAnimation()
{
}

CDUIAnimation:: ~CDUIAnimation()
{
}

bool CDUIAnimation::StartAnimation(int nAnimationID, int nElapse, int nTotalFrame, bool bLoop/* = FALSE*/)
{
	tagAnimationData *pData = GetAnimationDataByID(nAnimationID);
	if (pData || nElapse <= 0 || nTotalFrame <= 0)
	{
		return FALSE;
	}

	tagAnimationData Animation = { nAnimationID, nTotalFrame, nElapse, bLoop };

	if (SetTimer(nAnimationID, nElapse))
	{
		m_vecAnimationData.push_back(Animation);

		return true;
	}

	return false;
}

bool CDUIAnimation::OnAnimationElapse(int nAnimationID)
{
	tagAnimationData *pData = GetAnimationDataByID(nAnimationID);
	if (NULL == pData) return false;

	//start
	if (0 == pData->nCurFrame)
	{
		OnAnimationStart(nAnimationID, pData->bFirstLoop);
		pData->bFirstLoop = FALSE;
	}

	//step
	++(pData->nCurFrame);
	OnAnimationStep(nAnimationID, pData->nTotalFrame, pData->nCurFrame);

	//round
	if (pData->nCurFrame >= pData->nTotalFrame)
	{
		OnAnimationStop(nAnimationID);

		if (pData->bLoop)
		{
			pData->nCurFrame = 0;

			return true;
		}

		StopAnimation(nAnimationID);
	}

	return true;
}

void CDUIAnimation::StopAnimation(int nAnimationID /*= -1*/)
{
	if (-1 != nAnimationID)
	{
		if (false == RemoveAnimationData(nAnimationID)) return;

		KillTimer(nAnimationID);

		return;
	}
	
	for (int i = 0; i < m_vecAnimationData.size(); ++i)
	{
		tagAnimationData &AnimationData = m_vecAnimationData[i];
		KillTimer(AnimationData.nAnimationID);
	}

	m_vecAnimationData.clear();

	return;
}

bool CDUIAnimation::IsAnimationRunning(int nAnimationID)
{
	tagAnimationData *pData = GetAnimationDataByID(nAnimationID);

	return NULL != pData;
}

int CDUIAnimation::GetCurrentFrame(int nAnimationID/* = 0*/)
{
	tagAnimationData *pData = GetAnimationDataByID(nAnimationID);
	if (NULL == pData)
	{
		ASSERT(FALSE);
		return -1;
	}

	return pData->nCurFrame;
}

bool CDUIAnimation::SetCurrentFrame(int nAnimationID, int nFrame)
{
	tagAnimationData *pData = GetAnimationDataByID(nAnimationID);
	if (NULL == pData)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	if (nFrame >= 0 && nFrame <= pData->nTotalFrame)
	{
		pData->nCurFrame = nFrame;
		return TRUE;
	}
	else
	{
		ASSERT(FALSE);
	}

	return FALSE;
}

tagAnimationData* CDUIAnimation::GetAnimationDataByID(int nAnimationID)
{
	for (int nIndex = 0; nIndex < m_vecAnimationData.size(); nIndex++)
	{
		if (m_vecAnimationData[nIndex].nAnimationID == nAnimationID)
		{
			return &m_vecAnimationData[nIndex];
		}
	}

	return NULL;
}

bool CDUIAnimation::RemoveAnimationData(int nAnimationID)
{
	for (int nIndex = 0; nIndex < m_vecAnimationData.size(); nIndex++)
	{
		if (m_vecAnimationData[nIndex].nAnimationID == nAnimationID)
		{
			m_vecAnimationData.erase(m_vecAnimationData.begin() + nIndex);

			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
DuiBegin_Message_Map(CDUIAnimationDrag, CDUINotifyPump)
	Dui_On_Notify(DuiNotify_WinDragEnter, OnDuiWinDragEnter)
	Dui_On_Notify(DuiNotify_WinDragLeave, OnDuiWinDragLeave)
	Dui_On_Notify(DuiNotify_WinDragOver, OnDuiWinDragOver)
	Dui_On_Notify(DuiNotify_WinDrop, OnDuiWinDrop)
	Dui_On_Notify(DuiNotify_WinDragFinish, OnDuiWinDragFinish)
DuiEnd_Message_Map()

CDUIAnimationDrag::CDUIAnimationDrag(CDUIContainerCtrl *pBindContainerCtrl, enDuiAnimateDragType DragStyle)
	: m_pBindContainerCtrl(pBindContainerCtrl)
	, m_DragType(DragStyle)
{
	return;
}

CDUIAnimationDrag::~CDUIAnimationDrag()
{
}

bool CDUIAnimationDrag::SetTimer(UINT uTimerID, UINT nElapse)
{
	return m_pBindContainerCtrl ? m_pBindContainerCtrl->SetTimer(uTimerID, nElapse) : false;
}

bool CDUIAnimationDrag::KillTimer(UINT uTimerID)
{
	return m_pBindContainerCtrl ? m_pBindContainerCtrl->KillTimer(uTimerID) : false;
}

bool CDUIAnimationDrag::KillTimer()
{
	return m_pBindContainerCtrl ? m_pBindContainerCtrl->KillTimer() : 0;
}

enDuiAnimateDragType CDUIAnimationDrag::GetDragType()
{
	return m_DragType;
}

void CDUIAnimationDrag::SetDragType(enDuiAnimateDragType DragType)
{
	m_DragType = DragType;

	return;
}

bool CDUIAnimationDrag::IsAnimateDraging()
{
	return (NULL != m_pDragingCtrl);
}

CDUIControlBase * CDUIAnimationDrag::GetDragingCtrl()
{
	return m_pDragingCtrl;
}

void CDUIAnimationDrag::OnDragMoveBegin(CDUIControlBase *pDragCtrl)
{
	if (NULL == pDragCtrl 
		|| NULL == pDragCtrl->GetWndOwner()
		|| NULL == m_pBindContainerCtrl
		|| NULL == m_pBindContainerCtrl->GetWndOwner()
		|| pDragCtrl->GetParent() != m_pBindContainerCtrl)
	{
		ASSERT(false); return;
	}

	CDUIWndBase *pWndManager = m_pBindContainerCtrl->GetWndOwner();
	if (NULL == pWndManager) return;
	
	//prepare
	m_vecDragCtrl.clear();
	m_vecDragPre.clear();
	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUIControlBase *pChild = m_pBindContainerCtrl->GetChildAt(n);
		if (NULL == pChild) continue;

		pChild->OnDragMovePrepar();
		m_vecDragPre.push_back(pChild->GetAbsoluteRect());

		MMInterfaceHelper(CDUIListItemCtrl, pChild, pItem);
		if(NULL == pItem || false == pItem->IsSelected()) continue;

		m_vecDragCtrl.push_back({ pItem, n });
	}
	if (m_vecDragCtrl.empty())
	{
		m_vecDragCtrl.push_back({ pDragCtrl, GetChildIndex(pDragCtrl) });
	}

	m_pDragingCtrl = pDragCtrl;
	m_ptMouseLast = m_pDragingCtrl->GetWndOwner()->GetMousePosLast();

	//start
	StartAnimation(Dui_TimerAnimate_ID, Dui_TimerAnimate_Elapse, ANIMATE_FRAME, true);

	pWndManager->SendNotify(m_pBindContainerCtrl, DuiNotify_DragAnimateStart);

	return;
}

void CDUIAnimationDrag::OnDragMoving(CDUIControlBase *pDragCtrl)
{
	if (NULL == m_pBindContainerCtrl || NULL == pDragCtrl || NULL == pDragCtrl->GetWndOwner()) return;
	if (m_pDragingCtrl != pDragCtrl || pDragCtrl->GetParent() != m_pBindContainerCtrl) return;

	CDUIWndBase *pWndManager = m_pDragingCtrl->GetWndOwner();
	SIZE szOffset = {};
	POINT ptMouse = pWndManager->GetMousePosLast();
	CDUIRect rcDragCtrl = m_pDragingCtrl->GetAbsoluteRect();
	CDUIRect rcDragRange = m_pBindContainerCtrl->GetAbsoluteRect();

	//range
	if (DragType_H == m_DragType && (ptMouse.x < rcDragRange.left || ptMouse.x > rcDragRange.right)) return;
	if (DragType_V == m_DragType && (ptMouse.y < rcDragRange.top || ptMouse.y > rcDragRange.bottom)) return;
	if (DragType_HV == m_DragType && (ptMouse.x < rcDragRange.left || ptMouse.x > rcDragRange.right)) return;

	int nOffsetX = ptMouse.x - m_ptMouseLast.x;
	int nOffsetY = ptMouse.y - m_ptMouseLast.y;
	m_ptMouseLast = ptMouse;

	//offset
	if (DragType_H == m_DragType || DragType_HV == m_DragType)
	{
		if (false == m_pDragingCtrl->IsWinDragEnabled())
		{
			if (rcDragCtrl.left + nOffsetX < rcDragRange.left) nOffsetX = rcDragRange.left - rcDragCtrl.left;
			else if (rcDragCtrl.right + nOffsetX > rcDragRange.right) nOffsetX = rcDragRange.right - rcDragCtrl.right;
		}

		szOffset.cx = nOffsetX;
	}
	if (DragType_V == m_DragType || DragType_HV == m_DragType)
	{
		if (false == m_pDragingCtrl->IsWinDragEnabled())
		{
			if (ptMouse.y < rcDragRange.top || ptMouse.y > rcDragRange.bottom) nOffsetY = 0;
			else if (rcDragCtrl.top + nOffsetY < rcDragRange.top) nOffsetY = rcDragRange.top - rcDragCtrl.top;
			else if (rcDragCtrl.bottom + nOffsetY > rcDragRange.bottom) nOffsetY = rcDragRange.bottom - rcDragCtrl.bottom;
		}

		szOffset.cy = nOffsetY;
	}

	for (auto &DragCtrl : m_vecDragCtrl)
	{
		CDUIControlBase *pDragCtrl = DragCtrl.pControl;
		if (NULL == pDragCtrl) continue;

		pDragCtrl->MovePosition(szOffset);

		rcDragCtrl = pDragCtrl->GetAbsoluteRect();

		//hit test
		if (DragType_H == m_DragType) FindMoveCtrlStyleH(szOffset, DragCtrl.nIndex, rcDragCtrl);
		else if (DragType_V == m_DragType) FindMoveCtrlStyleV(szOffset, DragCtrl.nIndex, rcDragCtrl);
		else if (DragType_HV == m_DragType) FindMoveCtrlStyleHV(szOffset, DragCtrl.nIndex, rcDragCtrl);
	}

	pWndManager->SendNotify(m_pBindContainerCtrl, DuiNotify_DragAnimateMoving);

	return;
}

void CDUIAnimationDrag::OnDragMoveEnd(CDUIControlBase *pDragCtrl)
{
	if (NULL == m_pBindContainerCtrl || NULL == m_pDragingCtrl || (m_pDragingCtrl != pDragCtrl && m_pBindContainerCtrl != pDragCtrl->GetParent())) return;
	
	CDUIWndBase *pWndManager = m_pBindContainerCtrl->GetWndOwner();
	if (NULL == pWndManager) return;

	//finish
	StopAnimation(Dui_TimerAnimate_ID);

	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUIControlBase *pChild = m_pBindContainerCtrl->GetChildAt(n);
		if (NULL == pChild) continue;

		pChild->OnDragMoveFinish();
	}

	//通知
	pWndManager->SendNotify(m_pBindContainerCtrl, DuiNotify_DragAnimateFinish);

	//初始化
	m_vecPosDest.clear();
	m_pDragingCtrl = NULL;
	m_vecDragCtrl.clear();

	m_pBindContainerCtrl->NeedRefreshView();

	return;
}

void CDUIAnimationDrag::OnAnimationStep(int nAnimationID, int nTotalFrame, int nCurFrame)
{
	if (NULL == m_pBindContainerCtrl || NULL == m_pBindContainerCtrl->GetWndOwner()) return;

	for (int nIndex = m_vecPosDest.size() - 1; nIndex >= 0; nIndex--)
	{
		tagDragMove &PosMoveDest = m_vecPosDest[nIndex];
		CDUIControlBase *pMoveCtrl = PosMoveDest.pControl;
		if (NULL == pMoveCtrl) continue;

		CDUIRect rcItem = PosMoveDest.pControl->GetAbsoluteRect();
		CDUIRect rcDest = PosMoveDest.rcDest;
		SIZE szOffsetMove = PosMoveDest.szOffsetMove;

		int nXOffset = szOffsetMove.cx, nYOffset = szOffsetMove.cy;
		SIZE szOffset = { };
		if (rcDest.left > rcItem.left)
		{
			szOffset.cx = min(nXOffset, rcDest.left - rcItem.left);
		}
		else
		{
			szOffset.cx = -min(nXOffset, rcItem.left - rcDest.left);
		}

		if (rcDest.top > rcItem.top)
		{
			szOffset.cy = min(nYOffset, rcDest.top - rcItem.top);
		}
		else
		{
			szOffset.cy = -min(nYOffset, rcItem.top - rcDest.top);
		}

		pMoveCtrl->MovePosition(szOffset);

		//达到目标
		rcItem = PosMoveDest.pControl->GetAbsoluteRect();
		if (rcItem.left == rcDest.left && rcItem.top == rcDest.top)
		{
			m_vecPosDest.erase(m_vecPosDest.begin() + nIndex);
		}
	}

	m_pBindContainerCtrl->Invalidate();
	UpdateWindow(m_pBindContainerCtrl->GetWndOwner()->GetWndHandle());

	return;
}

void CDUIAnimationDrag::OnDuiWinDragEnter(const DuiNotify &Notify)
{
	if (NULL == m_pBindContainerCtrl
		|| NULL == m_pBindContainerCtrl->GetWndOwner()
		|| Notify.pNotifyCtrl != m_pBindContainerCtrl
		|| false == m_pBindContainerCtrl->IsAnimateDrag()
		|| m_pDragingCtrl) return;

	CDUIWndBase *pWndManager = m_pBindContainerCtrl->GetWndOwner();
	CDUIRect rcBindCtrl = m_pBindContainerCtrl->GetAbsoluteRect();
	if (false == ::PtInRect(&rcBindCtrl, pWndManager->GetMousePosLast())) return;

	//first visible
	CDUIControlBase *pChildFirst = NULL;
	for (int n = 0; n < m_pBindContainerCtrl->GetChildCount(); n++)
	{
		CDUIControlBase *pChild = m_pBindContainerCtrl->GetChildAt(n);
		if (NULL == pChild || false == pChild->IsVisible()) continue;

		pChildFirst = pChild;

		break;
	}

	//temp
	if (NULL == pChildFirst) return;
	CDUIControlBase *pDragCtrl = pChildFirst->Clone();
	pDragCtrl->SetCtrlID(Dui_CtrlIDInner_DragTmp);
	pDragCtrl->SetFixedWidth(pChildFirst->GetWidth());
	pDragCtrl->SetFixedHeight(pChildFirst->GetHeight());
	pDragCtrl->SetWinDragEnabled(true);
	m_pBindContainerCtrl->InsertChild(pDragCtrl);
	m_pBindContainerCtrl->RefreshView();

	//begin
	OnDragMoveBegin(pDragCtrl);

	//adjust
	SIZE szOffset = {};
	szOffset.cx = pWndManager->GetMousePosLast().x - (pDragCtrl->GetAbsoluteRect().left + pDragCtrl->GetWidth() / 2);
	szOffset.cy = pWndManager->GetMousePosLast().y - (pDragCtrl->GetAbsoluteRect().top + pDragCtrl->GetHeight() / 2);
	pDragCtrl->MovePosition(szOffset);

	//moving
	OnDragMoving(pDragCtrl);

	return;
}

void CDUIAnimationDrag::OnDuiWinDragLeave(const DuiNotify &Notify)
{
	if (NULL == m_pBindContainerCtrl
		|| NULL == m_pBindContainerCtrl->GetWndOwner()
		|| NULL == m_pDragingCtrl
		|| Notify.pNotifyCtrl != m_pBindContainerCtrl) return;

	if (Dui_CtrlIDInner_DragTmp != m_pDragingCtrl->GetCtrlID()) return;

	OnDragMoveEnd(m_pDragingCtrl);

	for (int n = m_pBindContainerCtrl->GetChildCount() - 1; n >= 0; n--)
	{
		CDUIControlBase *pChild = m_pBindContainerCtrl->GetChildAt(n);
		if (NULL == pChild || Dui_CtrlIDInner_DragTmp != pChild->GetCtrlID()) continue;

		m_pBindContainerCtrl->RemoveAt(n);
	}

	return;
}

void CDUIAnimationDrag::OnDuiWinDragOver(const DuiNotify &Notify)
{
	if (NULL == m_pBindContainerCtrl
		|| NULL == m_pBindContainerCtrl->GetWndOwner()
		|| NULL == m_pDragingCtrl
		|| Notify.pNotifyCtrl != m_pBindContainerCtrl) return;

	CDUIRect rcBindCtrl = m_pBindContainerCtrl->GetAbsoluteRect();
	if (false == ::PtInRect(&rcBindCtrl, m_pBindContainerCtrl->GetWndOwner()->GetMousePosLast())) return;

	OnDragMoving(m_pDragingCtrl);

	return;
}

void CDUIAnimationDrag::OnDuiWinDrop(const DuiNotify &Notify)
{
	if (IsAnimateDraging()
		&& m_pDragingCtrl
		&& m_pDragingCtrl->GetWndOwner())
	{
		m_pDragingCtrl->GetWndOwner()->PostNotify(m_pDragingCtrl->GetWndOwner()->GetRootCtrl(), DuiNotify_WinDragFinish, 0, 0);
	}

	return;
}

void CDUIAnimationDrag::OnDuiWinDragFinish(const DuiNotify &Notify)
{
	if (NULL == m_pBindContainerCtrl
		|| NULL == m_pBindContainerCtrl->GetWndOwner()
		|| NULL == m_pDragingCtrl) return;

	OnDragMoveEnd(m_pDragingCtrl);

	for (int n = m_pBindContainerCtrl->GetChildCount() - 1; n >= 0; n--)
	{
		CDUIControlBase *pChild = m_pBindContainerCtrl->GetChildAt(n);
		if (NULL == pChild || Dui_CtrlIDInner_DragTmp != pChild->GetCtrlID()) continue;

		m_pBindContainerCtrl->RemoveAt(n);
	}

	return;
}

void CDUIAnimationDrag::FindMoveCtrlStyleH(SIZE szOffset, int &nIndexDrag, CDUIRect rcDragCtrl)
{
	CDUIListViewCtrl *pListViewCtrl = MMDynamicPtr(CDUIListViewCtrl, this);
	int nIndexFirst = 0, nIndexLast = GetChildCount() - 1;
	pListViewCtrl ? nIndexFirst = pListViewCtrl->GetShowIndex().cx : 0;
	pListViewCtrl ? nIndexLast = pListViewCtrl->GetShowIndex().cy : 0;

	if (szOffset.cx > 0)
	{
		for (int n = nIndexDrag; n <= nIndexLast; n++)
		{
			CDUIControlBase *pChildCtrl = GetChildAt(n);
			if (NULL == pChildCtrl || false == pChildCtrl->IsVisible() || IsDragingCtrl(pChildCtrl)) continue;

			CDUIRect rcPos = pChildCtrl->GetAbsoluteRect();
			POINT ptHitRight = { rcDragCtrl.right, rcDragCtrl.top };
			POINT ptHitCritical = { };
			ptHitCritical.x = rcPos.left + rcPos.GetWidth() / 2;
			ptHitCritical.y = rcPos.top;
			if (ptHitRight.x > ptHitCritical.x)
			{
				PerformSwapCtrl(nIndexDrag, n);

				break;
			}
		}
	}
	else if (szOffset.cx < 0)
	{
		for (int n = nIndexDrag - 1; n >= nIndexFirst; n--)
		{
			CDUIControlBase *pChildCtrl = GetChildAt(n);
			if (NULL == pChildCtrl || false == pChildCtrl->IsVisible() || IsDragingCtrl(pChildCtrl)) continue;

			CDUIRect rcPos = pChildCtrl->GetAbsoluteRect();
			POINT ptHitLeft = { rcDragCtrl.left, rcDragCtrl.top };
			POINT ptHitCritical = { };
			ptHitCritical.x = rcPos.left + rcPos.GetWidth() / 2;
			ptHitCritical.y = rcPos.top;
			if (ptHitLeft.x < ptHitCritical.x)
			{
				PerformSwapCtrl(nIndexDrag, n);

				break;
			}
		}
	}

	return;
}

void CDUIAnimationDrag::FindMoveCtrlStyleV(SIZE szOffset, int &nIndexDrag, CDUIRect rcDragCtrl)
{
	CDUIListViewCtrl *pListViewCtrl = MMDynamicPtr(CDUIListViewCtrl, this);
	int nIndexFirst = 0, nIndexLast = GetChildCount() - 1;
	pListViewCtrl ? nIndexFirst = pListViewCtrl->GetShowIndex().cx : 0;
	pListViewCtrl ? nIndexLast = pListViewCtrl->GetShowIndex().cy : 0;

	if (szOffset.cy > 0)
	{
		for (int n = nIndexDrag; n <= nIndexLast; n++)
		{
			CDUIControlBase *pChildCtrl = GetChildAt(n);
			if (NULL == pChildCtrl || false == pChildCtrl->IsVisible() || IsDragingCtrl(pChildCtrl)) continue;

			CDUIRect rcPos = pChildCtrl->GetAbsoluteRect();
			POINT ptHitBottom = { rcDragCtrl.left, rcDragCtrl.bottom };
			POINT ptHitCritical = { };
			ptHitCritical.x = rcPos.left;
			ptHitCritical.y = rcPos.top + rcPos.GetHeight() / 2;
			if (ptHitBottom.y > ptHitCritical.y)
			{
				PerformSwapCtrl(nIndexDrag, n);

				break;
			}
		}
	}
	else if (szOffset.cy < 0)
	{
		for (int n = nIndexDrag - 1; n >= nIndexFirst; n--)
		{
			CDUIControlBase *pChildCtrl = GetChildAt(n);
			if (NULL == pChildCtrl || false == pChildCtrl->IsVisible() || IsDragingCtrl(pChildCtrl)) continue;

			CDUIRect rcPos = pChildCtrl->GetAbsoluteRect();
			POINT ptHitTop = { rcDragCtrl.left, rcDragCtrl.top };
			POINT ptHitCritical = { };
			ptHitCritical.x = rcPos.left;
			ptHitCritical.y = rcPos.top + rcPos.GetHeight() / 2;
			if (ptHitTop.y < ptHitCritical.y)
			{
				PerformSwapCtrl(nIndexDrag, n);

				break;
			}
		}
	}

	return;
}

void CDUIAnimationDrag::FindMoveCtrlStyleHV(SIZE szOffset, int &nIndexDrag, CDUIRect rcDragCtrl)
{
	CDUIListViewCtrl *pListViewCtrl = MMDynamicPtr(CDUIListViewCtrl, this);
	int nIndexFirst = 0, nIndexLast = GetChildCount() - 1;
	pListViewCtrl ? nIndexFirst = pListViewCtrl->GetShowIndex().cx : 0;
	pListViewCtrl ? nIndexLast = pListViewCtrl->GetShowIndex().cy : 0;

	if (szOffset.cy > 0 || (0 == szOffset.cy && szOffset.cx > 0))
	{
		//碰撞检测
		int nIndexImpact = -1;
		for (int n = nIndexDrag; n <= nIndexLast; n++)
		{
			CDUIControlBase *pChildCtrl = GetChildAt(n);
			if (NULL == pChildCtrl || false == pChildCtrl->IsVisible() || IsDragingCtrl(pChildCtrl)) continue;

			CDUIRect rcIntersect;
			CDUIRect rcPos = pChildCtrl->GetAbsoluteRect();
			POINT ptHit = { rcDragCtrl.right, rcDragCtrl.bottom };
			POINT ptHitCritical = { };
			ptHitCritical.x = rcPos.left + rcPos.GetWidth() / 2;
			ptHitCritical.y = rcPos.top + rcPos.GetHeight() / 2;
			if (::IntersectRect(&rcIntersect, &rcPos, &rcDragCtrl) && (ptHit.x > ptHitCritical.x) && (ptHit.y > ptHitCritical.y))
			{
				nIndexImpact = n;

				break;
			}
		}

		if (-1 == nIndexImpact) return;

		PerformSwapCtrl(nIndexDrag, nIndexImpact);
	}
	else
	{
		//碰撞检测
		int nIndexImpact = -1;
		for (int n = nIndexDrag; n >= nIndexFirst; n--)
		{
			CDUIControlBase *pChildCtrl = GetChildAt(n);
			if (NULL == pChildCtrl || false == pChildCtrl->IsVisible() || IsDragingCtrl(pChildCtrl)) continue;

			CDUIRect rcIntersect;
			CDUIRect rcPos = pChildCtrl->GetAbsoluteRect();
			POINT ptHit = { rcDragCtrl.left, rcDragCtrl.top };
			POINT ptHitCritical = { };
			ptHitCritical.x = rcPos.left + rcPos.GetWidth() / 2;
			ptHitCritical.y = rcPos.top + rcPos.GetHeight() / 2;
			if (::IntersectRect(&rcIntersect, &rcPos, &rcDragCtrl) && (ptHit.x < ptHitCritical.x) && (ptHit.y < ptHitCritical.y))
			{
				nIndexImpact = n;

				break;
			}
		}

		if (-1 == nIndexImpact) return;

		PerformSwapCtrl(nIndexDrag, nIndexImpact);
	}

	return;
}

void CDUIAnimationDrag::PerformSwapCtrl(int &nIndexDrag, int nIndexStop)
{
	if (-1 == nIndexStop || -1 == nIndexDrag) return;
	if (NULL == m_pBindContainerCtrl) return;

	if (nIndexStop > nIndexDrag)
	{
		for (int nIndex = nIndexDrag; nIndex <= nIndexStop; nIndex++)
		{
			CDUIControlBase *pChildCtrl = GetChildAt(nIndex);
			if (NULL == pChildCtrl || false == pChildCtrl->IsVisible() || IsDragingCtrl(pChildCtrl)) continue;

			//移动控件
			m_pBindContainerCtrl->SwapChild(nIndex, nIndexDrag, false);
			SetPosDest(pChildCtrl, m_vecDragPre[nIndexDrag]);
			nIndexDrag = nIndex;
		}
	}
	else if (nIndexStop < nIndexDrag)
	{
		for (int nIndex = nIndexDrag; nIndex >= nIndexStop; nIndex--)
		{
			CDUIControlBase *pChildCtrl = GetChildAt(nIndex);
			if (NULL == pChildCtrl || false == pChildCtrl->IsVisible() || IsDragingCtrl(pChildCtrl)) continue;

			//移动控件
			m_pBindContainerCtrl->SwapChild(nIndex, nIndexDrag, false);
			SetPosDest(pChildCtrl, m_vecDragPre[nIndexDrag]);
			nIndexDrag = nIndex;
		}
	}

	return;
}

void CDUIAnimationDrag::SetPosDest(CDUIControlBase *pControl, CDUIRect rcPosDest)
{
	if (NULL == pControl) return;

	CDUIRect rcItem = pControl->GetAbsoluteRect();
	if (rcItem == rcPosDest) return;

	tagDragMove PosMoveDest = {};
	PosMoveDest.pControl = pControl;
	PosMoveDest.rcDest = rcPosDest;

	int nWidth = abs(rcPosDest.left - rcItem.left);
	int nHeight = abs(rcPosDest.top - rcItem.top);
	PosMoveDest.szOffsetMove.cx = nWidth < ANIMATE_FRAME && nWidth > 0 ? 1 : nWidth / ANIMATE_FRAME;
	PosMoveDest.szOffsetMove.cy = nHeight < ANIMATE_FRAME && nHeight > 0 ? 1 : nHeight / ANIMATE_FRAME;

	for (int nIndex = 0; nIndex < m_vecPosDest.size(); nIndex++)
	{
		if (m_vecPosDest[nIndex].pControl == pControl)
		{
			m_vecPosDest[nIndex] = PosMoveDest;

			return;
		}
	}

	m_vecPosDest.push_back(PosMoveDest);

	return;
}

bool CDUIAnimationDrag::IsDragingCtrl(CDUIControlBase *pControl)
{
	if (NULL == pControl) return false;

	if (find_if(m_vecDragCtrl.begin(), m_vecDragCtrl.end(), [&](tagDuiDragCtrl &DragCtrl)
	{
		return DragCtrl.pControl == pControl;
	}) != m_vecDragCtrl.end()) return true;

	return false;
}

int CDUIAnimationDrag::GetChildCount() const
{
	return m_pBindContainerCtrl ? m_pBindContainerCtrl->GetChildCount() : 0;
}

int CDUIAnimationDrag::GetChildIndex(CDUIControlBase *pChild) const
{
	return m_pBindContainerCtrl ? m_pBindContainerCtrl->GetChildIndex(pChild) : -1;
}

CDUIControlBase * CDUIAnimationDrag::GetChildAt(int nIndex) const
{
	return m_pBindContainerCtrl ? m_pBindContainerCtrl->GetChildAt(nIndex) : NULL;
}

//////////////////////////////////////////////////////////////////////////
CDUIAnimationWnd::CDUIAnimationWnd()
{

}

CDUIAnimationWnd::~CDUIAnimationWnd()
{
	UnInit();

	return;
}

bool CDUIAnimationWnd::StartAnimation(int nAnimationID, int nElapse, int nTotalFrame, bool bLoop /* = FALSE */)
{
	if (false == IsWindow(GetWndHandle()) || AnimateWnd_None == GetAnimateWndType()) return false;

	return __super::StartAnimation(nAnimationID, nElapse, nTotalFrame, bLoop);
}

void CDUIAnimationWnd::OnAnimationStart(int nAnimationID, bool bFirstLoop)
{
	return;
}

void CDUIAnimationWnd::OnAnimationStep(int nAnimationID, int nTotalFrame, int nCurFrame)
{
	//clear
	memset(m_pBmpAniBits, 0, m_rcWndAnimation.GetWidth() * m_rcWndAnimation.GetHeight() * sizeof(Gdiplus::ARGB));

	switch (GetAnimateWndType())
	{
		case AnimateWnd_Left:
		{
			OnAnimateWndLeft(nAnimationID, nTotalFrame, nCurFrame);
			break;
		}
		case AnimateWnd_LeftDiffuse:
		{
			OnAnimateWndLeftDiffuse(nAnimationID, nTotalFrame, nCurFrame);
			break;
		}
		case AnimateWnd_Right:
		{
			OnAnimateWndRight(nAnimationID, nTotalFrame, nCurFrame);
			break;
		}
		case AnimateWnd_RightDiffuse:
		{
			OnAnimateWndRightDiffuse(nAnimationID, nTotalFrame, nCurFrame);
			break;
		}
		case AnimateWnd_Top:
		{
			OnAnimateWndTop(nAnimationID, nTotalFrame, nCurFrame);
			break;
		}
		case AnimateWnd_TopDiffuse:
		{
			OnAnimateWndTopDiffuse(nAnimationID, nTotalFrame, nCurFrame);
			break;
		}
		case AnimateWnd_Bottom:
		{
			OnAnimateWndBottom(nAnimationID, nTotalFrame, nCurFrame);
			break;
		}
		case AnimateWnd_BottomDiffuse:
		{
			OnAnimateWndBottomDiffuse(nAnimationID, nTotalFrame, nCurFrame);
			break;
		}
		case AnimateWnd_Size:
		{
			OnAnimateWndSize(nAnimationID, nTotalFrame, nCurFrame);
			break;
		}
		case AnimateWnd_SizeSpring:
		{
			OnAnimateWndSizeSpring(nAnimationID, nTotalFrame, nCurFrame);
			break;
		}
		case AnimateWnd_SizeDiffuse:
		{
			OnAnimateWndSizeDiffuse(nAnimationID, nTotalFrame, nCurFrame);
			break;
		}
		case AnimateWnd_CenterDiffuse:
		{
			OnAnimateWndCenterDiffuse(nAnimationID, nTotalFrame, nCurFrame);
			break;
		}
		case AnimateWnd_LeftRightDiffuse:
		{
			OnAnimateWndLeftRightDiffuse(nAnimationID, nTotalFrame, nCurFrame);
			break;
		}
		case AnimateWnd_LeftRightExpand:
		{
			OnAnimateWndLeftRightExpand(nAnimationID, nTotalFrame, nCurFrame);
			break;
		}
		case AnimateWnd_LeftRightExpandSpring:
		{
			OnAnimateWndLeftRightExpandSpring(nAnimationID, nTotalFrame, nCurFrame);
			break;
		}
		case AnimateWnd_LeftRightCombin:
		{
			OnAnimateWndLeftRightCombin(nAnimationID, nTotalFrame, nCurFrame);
			break;
		}
		case AnimateWnd_TopBottomDiffuse:
		{
			OnAnimateWndTopBottomDiffuse(nAnimationID, nTotalFrame, nCurFrame);
			break;
		}
		case AnimateWnd_TopBottomExpand:
		{
			OnAnimateWndTopBottomExpand(nAnimationID, nTotalFrame, nCurFrame);
			break;
		}
		case AnimateWnd_TopBottomExpandSpring:
		{
			OnAnimateWndTopBottomExpandSpring(nAnimationID, nTotalFrame, nCurFrame);
			break;
		}
		case AnimateWnd_TopBottomCombin:
		{
			OnAnimateWndTopBottomCombin(nAnimationID, nTotalFrame, nCurFrame);
			break;
		}
		case AnimateWnd_3DRotateLeftRight:
		{
			OnAnimateWnd3DRotateLeftRight(nAnimationID, nTotalFrame, nCurFrame);
			break;
		}
		case AnimateWnd_3DRotateTopBottom:
		{
			OnAnimateWnd3DRotateTopBottom(nAnimationID, nTotalFrame, nCurFrame);
			break;
		}
		case AnimateWnd_3DRoundLeft:
		{
			OnAnimateWnd3DRoundLeft(nAnimationID, nTotalFrame, nCurFrame);
			break;
		}
		case AnimateWnd_3DRoundRight:
		{
			OnAnimateWnd3DRoundRight(nAnimationID, nTotalFrame, nCurFrame);
			break;
		}
		case AnimateWnd_3DRoundTop:
		{
			OnAnimateWnd3DRoundTop(nAnimationID, nTotalFrame, nCurFrame);
			break;
		}
		case AnimateWnd_3DRoundBottom:
		{
			OnAnimateWnd3DRoundBottom(nAnimationID, nTotalFrame, nCurFrame);
			break;
		}
		case AnimateWnd_3DRotateReplaseLR:
		{
			OnAnimateWnd3DRotateReplaseLR(nAnimationID, nTotalFrame, nCurFrame);
			break;
		}
		case AnimateWnd_3DRotateReplaseTB:
		{
			OnAnimateWnd3DRotateReplaseTB(nAnimationID, nTotalFrame, nCurFrame);
			break;
		}
		case AnimateWnd_3DRoundReplaseLR:
		{
			OnAnimateWnd3DRoundReplaseLR(nAnimationID, nTotalFrame, nCurFrame);
			break;
		}
		case AnimateWnd_3DRoundReplaseTB:
		{
			OnAnimateWnd3DRoundReplaseTB(nAnimationID, nTotalFrame, nCurFrame);
			break;
		}
	}

	OnDrawAnimationWnd();

	return;
}

void CDUIAnimationWnd::OnAnimationStop(int nAnimationID)
{
	switch (GetAnimateWndType())
	{
		case AnimateWnd_SizeSpring:
		case AnimateWnd_LeftRightExpandSpring:
		case AnimateWnd_TopBottomExpandSpring:
		{
			if (Timer_Spring_ID2 == nAnimationID)
			{
				StopAnimationWnd();
			}

			break;
		}
		default:
		{
			if (Dui_TimerAnimate_ID == nAnimationID)
			{
				StopAnimationWnd();
			}

			break;
		}
	}

	return;
}

void CDUIAnimationWnd::OnDrawAnimationWnd()
{
	if (false == IsAnimatingWnd()) return;

	//update
	SIZE szDest = { m_rcWndAnimation.GetWidth(), m_rcWndAnimation.GetHeight() };
	POINT ptDest = { m_rcWndAnimation.left , m_rcWndAnimation.top };
	POINT ptSrc = { 0, 0 };
	BLENDFUNCTION Blend = {};
	Blend.AlphaFormat = AC_SRC_ALPHA;
	Blend.SourceConstantAlpha = GetWndAlpha();
	UpdateLayeredWindow(GetWndHandle(), GetWndDC(), &ptDest, &szDest, m_hMemDCAni, &ptSrc, 0, &Blend, ULW_ALPHA);

	return;
}

void CDUIAnimationWnd::StartAnimationWnd(bool bShow/* = true*/)
{
	UnInit();

	CDUIContainerCtrl *pRootCtrl = GetRootCtrl();
	if (NULL == pRootCtrl) return;

	CDUIWndBase *pWndManager = pRootCtrl->GetWndOwner();
	if (NULL == pWndManager) return;

	m_bShow = bShow;

	::GetWindowRect(GetWndHandle(), &m_rcWndSrc);
	m_rcWndAnimation = m_rcWndSrc;

	m_bWndLayered = IsWndLayered();
	SetWndLayered(true);

	CDUIRect rcBitmap = pRootCtrl->GetAbsoluteRect();
	m_hMemDCAni = CreateCompatibleDC(GetWndDC());
	m_hMemDCSrc = CreateCompatibleDC(m_hMemDCAni);
	m_hBitmapSrc = CDUIRenderEngine::GenerateBitmap(pRootCtrl, rcBitmap);
	GetObject(m_hBitmapSrc, sizeof(m_BmpInfoSrc), &m_BmpInfoSrc);
	CDUIRenderEngine::RestorePixelAlpha((LPBYTE)m_BmpInfoSrc.bmBits, rcBitmap.GetWidth(), rcBitmap);
	m_hBitmapSrcOld = (HBITMAP)SelectObject(m_hMemDCSrc, m_hBitmapSrc);
	m_pBitmapSrc = CDUIRenderEngine::GetAlphaBitmap(m_hBitmapSrc);

	switch (GetAnimateWndType())
	{
		case AnimateWnd_3DRotateLeftRight:
		{
			m_szRotateRadius.cx = m_rcWndAnimation.GetWidth() / 2;
			m_szRotateRadius.cy = min(100, m_rcWndSrc.GetHeight() / 3);
			m_rcWndAnimation.top -= m_szRotateRadius.cy;
			m_rcWndAnimation.bottom += m_szRotateRadius.cy;
			m_hBitmapAni = CDUIRenderEngine::CreateARGB32Bitmap(m_hMemDCSrc, m_rcWndAnimation.GetWidth(), m_rcWndAnimation.GetHeight(), &m_pBmpAniBits);
			::SetWindowPos(GetWndHandle(), NULL, m_rcWndAnimation.left, m_rcWndAnimation.top, m_rcWndAnimation.GetWidth(), m_rcWndAnimation.GetHeight(), SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW);
			break;
		}
		case AnimateWnd_3DRotateTopBottom:
		{
			m_szRotateRadius.cx = min(100, m_rcWndSrc.GetWidth() / 3);
			m_szRotateRadius.cy = m_rcWndAnimation.GetHeight() / 2;
			m_rcWndAnimation.left -= m_szRotateRadius.cx;
			m_rcWndAnimation.right += m_szRotateRadius.cx;
			m_hBitmapAni = CDUIRenderEngine::CreateARGB32Bitmap(m_hMemDCSrc, m_rcWndAnimation.GetWidth(), m_rcWndAnimation.GetHeight(), &m_pBmpAniBits);
			::SetWindowPos(GetWndHandle(), NULL, m_rcWndAnimation.left, m_rcWndAnimation.top, m_rcWndAnimation.GetWidth(), m_rcWndAnimation.GetHeight(), SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW);
			break;
		}
		case AnimateWnd_3DRotateReplaseLR:
		case AnimateWnd_3DRotateReplaseTB:
		case AnimateWnd_3DRoundReplaseLR:
		case AnimateWnd_3DRoundReplaseTB:
		{
			CDUITabLayoutCtrl *pTabRoot = dynamic_cast<CDUITabLayoutCtrl*>(GetRootCtrl());
			if (NULL == pTabRoot || pTabRoot->GetChildCount() < 2)
			{
				MessageBox(GetWndHandle(), _T("This Animatewnd Need RootCtrl Is TablayoutCtrl, And Children Has Two or More Limit"), NULL, NULL);
				return;
			}

			int nNextSel = pTabRoot->GetCurSel() + 1 < pTabRoot->GetChildCount() ? pTabRoot->GetCurSel() + 1 : 0;
			pTabRoot->Select(nNextSel);
			pTabRoot->RefreshView();
			
			BITMAP BmpInfo = {};
			m_hBitmapReplase = CDUIRenderEngine::GenerateBitmap(pRootCtrl, rcBitmap);
			GetObject(m_hBitmapReplase, sizeof(BmpInfo), &BmpInfo);
			CDUIRenderEngine::RestorePixelAlpha((LPBYTE)BmpInfo.bmBits, rcBitmap.GetWidth(), rcBitmap);
			m_pBitmapReplase = CDUIRenderEngine::GetAlphaBitmap(m_hBitmapReplase);
			m_hBitmapAni = CDUIRenderEngine::CreateARGB32Bitmap(m_hMemDCSrc, m_rcWndAnimation.GetWidth(), m_rcWndAnimation.GetHeight(), &m_pBmpAniBits);

			break;
		}
		default:
		{
			m_hBitmapAni = CDUIRenderEngine::CreateARGB32Bitmap(m_hMemDCSrc, m_rcWndAnimation.GetWidth(), m_rcWndAnimation.GetHeight(), &m_pBmpAniBits);
			break;
		}
	}

	m_hBitmapAniOld = (HBITMAP)SelectObject(m_hMemDCAni, m_hBitmapAni);
	m_bAnimating = true;

	StartAnimation(Dui_TimerAnimate_ID, GetAnimateWndElapse(), GetAnimateWndFrame());

	//notify
	DuiNotify Notify;
	Notify.NotifyType = DuiNotify_WndAnimateStart;
	Notify.pNotifyCtrl = pRootCtrl;
	Notify.DuiNotifyExtend.Type = tagDuiNotify::DuiNotifyExtend_WndAnimate;
	Notify.DuiNotifyExtend.WndAnimate.bShow = m_bShow;
	pWndManager->SendNotify(Notify);

	return;
}

void CDUIAnimationWnd::StopAnimationWnd()
{
	CDUIContainerCtrl *pRootCtrl = GetRootCtrl();
	if (NULL == pRootCtrl) return;

	CDUIWndBase *pWndManager = pRootCtrl->GetWndOwner();
	if (NULL == pWndManager) return;

	m_bAnimating = false;

	StopAnimation(Dui_TimerAnimate_ID);
	StopAnimation(Timer_Spring_ID1);
	StopAnimation(Timer_Spring_ID2);

	//refresh
	{
		if (m_hBitmapReplase)
		{
			SelectObject(m_hMemDCSrc, m_hBitmapReplase);
		}

		SIZE szDest = { m_rcWndSrc.GetWidth(), m_rcWndSrc.GetHeight() };
		POINT ptDest = { m_rcWndSrc.left , m_rcWndSrc.top };
		POINT ptSrc = { 0, 0 };
		BLENDFUNCTION Blend = {};
		Blend.AlphaFormat = AC_SRC_ALPHA;
		Blend.SourceConstantAlpha = GetWndAlpha();
		UpdateLayeredWindow(GetWndHandle(), GetWndDC(), &ptDest, &szDest, m_hMemDCSrc, &ptSrc, 0, &Blend, ULW_ALPHA);
	}

	SetWndLayered(m_bWndLayered);
	::SetWindowPos(GetWndHandle(), NULL, m_rcWndSrc.left, m_rcWndSrc.top, m_rcWndSrc.GetWidth(), m_rcWndSrc.GetHeight(), SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW);
	pWndManager->Invalidate();

	//notify
	DuiNotify Notify;
	Notify.NotifyType = DuiNotify_WndAnimateFinish;
	Notify.pNotifyCtrl = GetRootCtrl();
	Notify.DuiNotifyExtend.Type = tagDuiNotify::DuiNotifyExtend_WndAnimate;
	Notify.DuiNotifyExtend.WndAnimate.bShow = m_bShow;
	pWndManager->SendNotify(Notify);

	return;
}

bool CDUIAnimationWnd::IsAnimatingWnd()
{
	return m_bAnimating;
}

void CDUIAnimationWnd::UnInit()
{
	m_bAnimating = false;
	StopAnimation(Dui_TimerAnimate_ID);
	StopAnimation(Timer_Spring_ID1);
	StopAnimation(Timer_Spring_ID2);

	if (m_hBitmapSrcOld)
	{
		SelectObject(m_hMemDCSrc, m_hBitmapSrcOld);
	}
	if (m_hBitmapAniOld)
	{
		SelectObject(m_hMemDCAni, m_hBitmapAniOld);
	}

	MMSafeDeleteObject(m_hBitmapSrc);
	MMSafeDeleteObject(m_hBitmapAni);
	MMSafeDeleteObject(m_hBitmapReplase);
	MMSafeDeleteDC(m_hMemDCSrc);
	MMSafeDeleteDC(m_hMemDCAni);
	MMSafeDelete(m_pBitmapSrc);
	MMSafeDelete(m_pBitmapReplase);
	m_szRoundRadius = {};
	m_szRotateRadius = {};

	return;
}

void CDUIAnimationWnd::OnAnimateWndLeft(int nAnimationID, int nTotalFrame, int nCurFrame)
{
	if (NULL == m_hBitmapSrc || NULL == m_hBitmapAni || NULL == m_pBmpAniBits || NULL == m_BmpInfoSrc.bmBits) return;

	int nWidthItem = m_rcWndAnimation.GetWidth() / nTotalFrame;
	int nWidthSurplus = m_rcWndAnimation.GetWidth() % nTotalFrame;
	int nWidthOffset = nCurFrame == nTotalFrame ? 0 : m_rcWndAnimation.GetWidth() - nWidthItem * nCurFrame;
	false == m_bShow ? nWidthOffset = m_rcWndAnimation.GetWidth() - nWidthOffset : nWidthOffset;

	for (int nTop = 0; nTop < m_rcWndAnimation.GetHeight(); nTop++)
	{
		Gdiplus::ARGB *pSrc = (Gdiplus::ARGB*)m_BmpInfoSrc.bmBits + m_rcWndAnimation.GetWidth() * nTop + nWidthOffset;
		Gdiplus::ARGB *pDest = (Gdiplus::ARGB*)m_pBmpAniBits + m_rcWndAnimation.GetWidth() * nTop;
		memcpy(pDest, pSrc, (m_rcWndAnimation.GetWidth() - nWidthOffset) * sizeof(Gdiplus::ARGB));
	}

	return;
}

void CDUIAnimationWnd::OnAnimateWndLeftDiffuse(int nAnimationID, int nTotalFrame, int nCurFrame)
{
	if (NULL == m_hBitmapSrc || NULL == m_hBitmapAni || NULL == m_pBmpAniBits || NULL == m_BmpInfoSrc.bmBits) return;

	int nWidthItem = m_rcWndAnimation.GetWidth() / nTotalFrame;
	int nWidthSurplus = m_rcWndAnimation.GetWidth() % nTotalFrame;
	int nWidthOffset = nCurFrame == nTotalFrame ? m_rcWndAnimation.GetWidth() : nWidthItem * nCurFrame;
	false == m_bShow ? nWidthOffset = m_rcWndAnimation.GetWidth() - nWidthOffset : nWidthOffset;

	for (int nTop = 0; nTop < m_rcWndAnimation.GetHeight(); nTop++)
	{
		Gdiplus::ARGB *pSrc = (Gdiplus::ARGB*)m_BmpInfoSrc.bmBits + m_rcWndAnimation.GetWidth() * nTop;
		Gdiplus::ARGB *pDest = (Gdiplus::ARGB*)m_pBmpAniBits + m_rcWndAnimation.GetWidth() * nTop;
		memcpy(pDest, pSrc, (nWidthOffset) * sizeof(Gdiplus::ARGB));
	}

	return;
}

void CDUIAnimationWnd::OnAnimateWndRight(int nAnimationID, int nTotalFrame, int nCurFrame)
{
	if (NULL == m_hBitmapSrc || NULL == m_hBitmapAni || NULL == m_pBmpAniBits || NULL == m_BmpInfoSrc.bmBits) return;

	int nWidthItem = m_rcWndAnimation.GetWidth() / nTotalFrame;
	int nWidthSurplus = m_rcWndAnimation.GetWidth() % nTotalFrame;
	int nWidthOffset = nCurFrame == nTotalFrame ? 0 : m_rcWndAnimation.GetWidth() - nWidthItem * nCurFrame;
	false == m_bShow ? nWidthOffset = m_rcWndAnimation.GetWidth() - nWidthOffset : nWidthOffset;

	for (int nTop = 0; nTop < m_rcWndAnimation.GetHeight(); nTop++)
	{
		Gdiplus::ARGB *pSrc = (Gdiplus::ARGB*)m_BmpInfoSrc.bmBits + m_rcWndAnimation.GetWidth() * nTop;
		Gdiplus::ARGB *pDest = (Gdiplus::ARGB*)m_pBmpAniBits + m_rcWndAnimation.GetWidth() * nTop + nWidthOffset;
		memcpy(pDest, pSrc, (m_rcWndAnimation.GetWidth() - nWidthOffset) * sizeof(Gdiplus::ARGB));
	}

	return;
}

void CDUIAnimationWnd::OnAnimateWndRightDiffuse(int nAnimationID, int nTotalFrame, int nCurFrame)
{
	if (NULL == m_hBitmapSrc || NULL == m_hBitmapAni || NULL == m_pBmpAniBits || NULL == m_BmpInfoSrc.bmBits) return;

	int nWidthItem = m_rcWndAnimation.GetWidth() / nTotalFrame;
	int nWidthSurplus = m_rcWndAnimation.GetWidth() % nTotalFrame;
	int nWidthOffset = nCurFrame == nTotalFrame ? 0 : m_rcWndAnimation.GetWidth() - nWidthItem * nCurFrame;
	false == m_bShow ? nWidthOffset = m_rcWndAnimation.GetWidth() - nWidthOffset : nWidthOffset;

	for (int nTop = 0; nTop < m_rcWndAnimation.GetHeight(); nTop++)
	{
		Gdiplus::ARGB *pSrc = (Gdiplus::ARGB*)m_BmpInfoSrc.bmBits + m_rcWndAnimation.GetWidth() * nTop + nWidthOffset;
		Gdiplus::ARGB *pDest = (Gdiplus::ARGB*)m_pBmpAniBits + m_rcWndAnimation.GetWidth() * nTop + nWidthOffset;
		memcpy(pDest, pSrc, (m_rcWndAnimation.GetWidth() - nWidthOffset) * sizeof(Gdiplus::ARGB));
	}

	return;
}

void CDUIAnimationWnd::OnAnimateWndTop(int nAnimationID, int nTotalFrame, int nCurFrame)
{
	if (NULL == m_hBitmapSrc || NULL == m_hBitmapAni || NULL == m_pBmpAniBits || NULL == m_BmpInfoSrc.bmBits) return;

	int nHeightItem = m_rcWndAnimation.GetHeight() / nTotalFrame;
	int nHeightSurplus = m_rcWndAnimation.GetHeight() % nTotalFrame;
	int nHeightOffset = nCurFrame == nTotalFrame ? 0 : m_rcWndAnimation.GetHeight() - nHeightItem * nCurFrame;
	false == m_bShow ? nHeightOffset = m_rcWndAnimation.GetHeight() - nHeightOffset : nHeightOffset;

	for (int nTop = nHeightOffset; nTop < m_rcWndAnimation.GetHeight(); nTop++)
	{
		Gdiplus::ARGB *pSrc = (Gdiplus::ARGB*)m_BmpInfoSrc.bmBits + m_rcWndAnimation.GetWidth() * (nTop - nHeightOffset);
		Gdiplus::ARGB *pDest = (Gdiplus::ARGB*)m_pBmpAniBits + m_rcWndAnimation.GetWidth() * nTop;
		memcpy(pDest, pSrc, (m_rcWndAnimation.GetWidth()) * sizeof(Gdiplus::ARGB));
	}

	return;
}

void CDUIAnimationWnd::OnAnimateWndTopDiffuse(int nAnimationID, int nTotalFrame, int nCurFrame)
{
	if (NULL == m_hBitmapSrc || NULL == m_hBitmapAni || NULL == m_pBmpAniBits || NULL == m_BmpInfoSrc.bmBits) return;

	int nHeightItem = m_rcWndAnimation.GetHeight() / nTotalFrame;
	int nHeightSurplus = m_rcWndAnimation.GetHeight() % nTotalFrame;
	int nHeightOffset = nCurFrame == nTotalFrame ? 0 : m_rcWndAnimation.GetHeight() - nHeightItem * nCurFrame;
	false == m_bShow ? nHeightOffset = m_rcWndAnimation.GetHeight() - nHeightOffset : nHeightOffset;

	for (int nTop = nHeightOffset; nTop < m_rcWndAnimation.GetHeight(); nTop++)
	{
		Gdiplus::ARGB *pSrc = (Gdiplus::ARGB*)m_BmpInfoSrc.bmBits + m_rcWndAnimation.GetWidth() * nTop;
		Gdiplus::ARGB *pDest = (Gdiplus::ARGB*)m_pBmpAniBits + m_rcWndAnimation.GetWidth() * nTop;
		memcpy(pDest, pSrc, (m_rcWndAnimation.GetWidth()) * sizeof(Gdiplus::ARGB));
	}

	return;
}

void CDUIAnimationWnd::OnAnimateWndBottom(int nAnimationID, int nTotalFrame, int nCurFrame)
{
	if (NULL == m_hBitmapSrc || NULL == m_hBitmapAni || NULL == m_pBmpAniBits || NULL == m_BmpInfoSrc.bmBits) return;

	int nHeightItem = m_rcWndAnimation.GetHeight() / nTotalFrame;
	int nHeightSurplus = m_rcWndAnimation.GetHeight() % nTotalFrame;
	int nHeightOffset = nCurFrame == nTotalFrame ? m_rcWndAnimation.GetHeight() : nHeightItem * nCurFrame;
	false == m_bShow ? nHeightOffset = m_rcWndAnimation.GetHeight() - nHeightOffset : nHeightOffset;

	for (int nTop = 0; nTop < nHeightOffset; nTop++)
	{
		Gdiplus::ARGB *pSrc = (Gdiplus::ARGB*)m_BmpInfoSrc.bmBits + m_rcWndAnimation.GetWidth() * (m_rcWndAnimation.GetHeight() - nHeightOffset + nTop);
		Gdiplus::ARGB *pDest = (Gdiplus::ARGB*)m_pBmpAniBits + m_rcWndAnimation.GetWidth() * nTop;
		memcpy(pDest, pSrc, (m_rcWndAnimation.GetWidth()) * sizeof(Gdiplus::ARGB));
	}

	return;
}

void CDUIAnimationWnd::OnAnimateWndBottomDiffuse(int nAnimationID, int nTotalFrame, int nCurFrame)
{
	if (NULL == m_hBitmapSrc || NULL == m_hBitmapAni || NULL == m_pBmpAniBits || NULL == m_BmpInfoSrc.bmBits) return;

	int nHeightItem = m_rcWndAnimation.GetHeight() / nTotalFrame;
	int nHeightSurplus = m_rcWndAnimation.GetHeight() % nTotalFrame;
	int nHeightOffset = nCurFrame == nTotalFrame ? m_rcWndAnimation.GetHeight() : nHeightItem * nCurFrame;
	false == m_bShow ? nHeightOffset = m_rcWndAnimation.GetHeight() - nHeightOffset : nHeightOffset;

	for (int nTop = 0; nTop < nHeightOffset; nTop++)
	{
		Gdiplus::ARGB *pSrc = (Gdiplus::ARGB*)m_BmpInfoSrc.bmBits + m_rcWndAnimation.GetWidth() * nTop;
		Gdiplus::ARGB *pDest = (Gdiplus::ARGB*)m_pBmpAniBits + m_rcWndAnimation.GetWidth() * nTop;
		memcpy(pDest, pSrc, (m_rcWndAnimation.GetWidth()) * sizeof(Gdiplus::ARGB));
	}

	return;
}

void CDUIAnimationWnd::OnAnimateWndSize(int nAnimationID, int nTotalFrame, int nCurFrame)
{
	if (NULL == m_hBitmapSrc || NULL == m_hBitmapAni || NULL == m_pBmpAniBits || NULL == m_BmpInfoSrc.bmBits) return;

	int nWidthItem = m_rcWndAnimation.GetWidth() / nTotalFrame;
	int nWidthSurplus = m_rcWndAnimation.GetWidth() % nTotalFrame;
	int nWidthOffset = nCurFrame == nTotalFrame ? m_rcWndAnimation.GetWidth() : nWidthItem * nCurFrame;
	false == m_bShow ? nWidthOffset = m_rcWndAnimation.GetWidth() - nWidthOffset : nWidthOffset;

	int nHeightItem = m_rcWndAnimation.GetHeight() / nTotalFrame;
	int nHeightSurplus = m_rcWndAnimation.GetHeight() % nTotalFrame;
	int nHeightOffset = nCurFrame == nTotalFrame ? m_rcWndAnimation.GetHeight() : nHeightItem * nCurFrame;
	false == m_bShow ? nHeightOffset = m_rcWndAnimation.GetHeight() - nHeightOffset : nHeightOffset;

	int nLeftStart = m_rcWndAnimation.GetWidth() / 2 - nWidthOffset / 2;
	int nTopStart = m_rcWndAnimation.GetHeight() / 2 - nHeightOffset / 2;
	StretchBlt(m_hMemDCAni, nLeftStart, nTopStart, nWidthOffset, nHeightOffset, 
		m_hMemDCSrc, 0, 0, m_rcWndAnimation.GetWidth(), m_rcWndAnimation.GetHeight(), SRCCOPY);

	return;
}

void CDUIAnimationWnd::OnAnimateWndSizeSpring(int nAnimationID, int nTotalFrame, int nCurFrame)
{
	if (Dui_TimerAnimate_ID == nAnimationID)
	{
		OnAnimateWndSize(nAnimationID, nTotalFrame, nCurFrame);

		if (nCurFrame == nTotalFrame)
		{
			m_szSpringValue.cx = -min(Offset_Spring_Total, m_rcWndAnimation.GetWidth());
			m_szSpringValue.cy = -min(Offset_Spring_Total, m_rcWndAnimation.GetHeight());
			int nTotalFrame = min(abs(m_szSpringValue.cx) / Min_Spring_Offset, abs(m_szSpringValue.cy) / Min_Spring_Offset);
			if (nTotalFrame <= 0)
			{
				StopAnimationWnd();

				return;
			}

			StartAnimation(Timer_Spring_ID1, m_szSpringValue.cx < 0 ? 30 : 10, nTotalFrame);
		}

		return;
	}
	if (Timer_Spring_ID1 == nAnimationID)
	{
		int nWidthOffsetTotal = abs(m_szSpringValue.cx);
		int nHeightOffsetTotal = abs(m_szSpringValue.cy);
		
		int nWidthItem = nWidthOffsetTotal / nTotalFrame;
		int nWidthSurplus = nWidthOffsetTotal % nTotalFrame;
		int nWidthOffset = nCurFrame == nTotalFrame ? nWidthOffsetTotal : nWidthItem * nCurFrame;
		int nHeightItem = nHeightOffsetTotal / nTotalFrame;
		int nHeightSurplus = nHeightOffsetTotal % nTotalFrame;
		int nHeightOffset = nCurFrame == nTotalFrame ? nHeightOffsetTotal : nHeightItem * nCurFrame;

		int nLeftStart = m_szSpringValue.cx < 0 ? nWidthOffset / 2 : nWidthOffsetTotal / 2 - nWidthOffset / 2;
		int nTopStart = m_szSpringValue.cy < 0 ? nHeightOffset / 2 : nHeightOffsetTotal / 2 - nHeightOffset / 2;
		StretchBlt(m_hMemDCAni, nLeftStart, nTopStart, m_rcWndAnimation.GetWidth() - nWidthOffset, m_rcWndAnimation.GetHeight() - nHeightOffset,
			m_hMemDCSrc, 0, 0, m_rcWndAnimation.GetWidth(), m_rcWndAnimation.GetHeight(), SRCCOPY);

		if (nCurFrame == nTotalFrame)
		{
			m_szSpringValue.cx = -(m_szSpringValue.cx / 2);
			m_szSpringValue.cy = -(m_szSpringValue.cy / 2);
			int nTotalFrame = min(abs(m_szSpringValue.cx) / Min_Spring_Offset, abs(m_szSpringValue.cy) / Min_Spring_Offset);
			if (nTotalFrame <= 0)
			{
				StopAnimation(Timer_Spring_ID1);
			}
			else
			{
				StartAnimation(Timer_Spring_ID1, m_szSpringValue.cx < 0 ? 30 : 10, nTotalFrame);
			}
		}

		return;
	}
	if (Timer_Spring_ID2 == nAnimationID)
	{
		int nWidthOffsetTotal = min(Offset_Spring_Total, m_rcWndAnimation.GetWidth());
		int nHeightOffsetTotal = min(Offset_Spring_Total, m_rcWndAnimation.GetHeight());

		int nWidthItem = nWidthOffsetTotal / nTotalFrame;
		int nWidthSurplus = nWidthOffsetTotal % nTotalFrame;
		int nWidthOffset = nCurFrame == nTotalFrame ? nWidthOffsetTotal : nWidthItem * nCurFrame;
		int nHeightItem = nHeightOffsetTotal / nTotalFrame;
		int nHeightSurplus = nHeightOffsetTotal % nTotalFrame;
		int nHeightOffset = nCurFrame == nTotalFrame ? nHeightOffsetTotal : nHeightItem * nCurFrame;

		return;
	}

	return;
}

void CDUIAnimationWnd::OnAnimateWndSizeDiffuse(int nAnimationID, int nTotalFrame, int nCurFrame)
{
	if (NULL == m_hBitmapSrc || NULL == m_hBitmapAni || NULL == m_pBmpAniBits || NULL == m_BmpInfoSrc.bmBits) return;

	int nWidthItem = m_rcWndAnimation.GetWidth() / nTotalFrame;
	int nWidthSurplus = m_rcWndAnimation.GetWidth() % nTotalFrame;
	int nWidthOffset = nCurFrame == nTotalFrame ? m_rcWndAnimation.GetWidth() : nWidthItem * nCurFrame;
	false == m_bShow ? nWidthOffset = m_rcWndAnimation.GetWidth() - nWidthOffset : nWidthOffset;

	int nHeightItem = m_rcWndAnimation.GetHeight() / nTotalFrame;
	int nHeightSurplus = m_rcWndAnimation.GetHeight() % nTotalFrame;
	int nHeightOffset = nCurFrame == nTotalFrame ? m_rcWndAnimation.GetHeight() : nHeightItem * nCurFrame;
	false == m_bShow ? nHeightOffset = m_rcWndAnimation.GetHeight() - nHeightOffset : nHeightOffset;

	int nTopStart = m_rcWndAnimation.GetHeight() / 2 - nHeightOffset / 2;
	int nLeftStart = m_rcWndAnimation.GetWidth() / 2 - nWidthOffset / 2;
	for (int nTop = nTopStart; nTop < nTopStart + nHeightOffset; nTop++)
	{
		Gdiplus::ARGB *pSrc = (Gdiplus::ARGB*)m_BmpInfoSrc.bmBits + m_rcWndAnimation.GetWidth() * nTop + nLeftStart;
		Gdiplus::ARGB *pDest = (Gdiplus::ARGB*)m_pBmpAniBits + m_rcWndAnimation.GetWidth() * nTop + nLeftStart;
		memcpy(pDest, pSrc, (nWidthOffset) * sizeof(Gdiplus::ARGB));
	}

	return;
}

void CDUIAnimationWnd::OnAnimateWndCenterDiffuse(int nAnimationID, int nTotalFrame, int nCurFrame)
{
	if (NULL == m_hBitmapSrc || NULL == m_hBitmapAni || NULL == m_pBmpAniBits || NULL == m_BmpInfoSrc.bmBits) return;

	int nWidthItem = m_rcWndAnimation.GetWidth() / nTotalFrame;
	int nWidthSurplus = m_rcWndAnimation.GetWidth() % nTotalFrame;
	int nWidthOffset = nCurFrame == nTotalFrame ? m_rcWndAnimation.GetWidth() : nWidthItem * nCurFrame;
	false == m_bShow ? nWidthOffset = m_rcWndAnimation.GetWidth() - nWidthOffset : nWidthOffset;

	int nLeftStart = m_rcWndAnimation.GetWidth() / 2 - nWidthOffset / 2;
	for (int nTop = 0; nTop < m_rcWndAnimation.GetHeight(); nTop++)
	{
		Gdiplus::ARGB *pSrc = (Gdiplus::ARGB*)m_BmpInfoSrc.bmBits + m_rcWndAnimation.GetWidth() * nTop + nLeftStart;
		Gdiplus::ARGB *pDest = (Gdiplus::ARGB*)m_pBmpAniBits + m_rcWndAnimation.GetWidth() * nTop + nLeftStart;
		memcpy(pDest, pSrc, (nWidthOffset) * sizeof(Gdiplus::ARGB));
	}

	return;
}

void CDUIAnimationWnd::OnAnimateWndLeftRightDiffuse(int nAnimationID, int nTotalFrame, int nCurFrame)
{
	if (NULL == m_hBitmapSrc || NULL == m_hBitmapAni || NULL == m_pBmpAniBits || NULL == m_BmpInfoSrc.bmBits) return;

	int nWidthItem = m_rcWndAnimation.GetWidth() / nTotalFrame;
	int nWidthSurplus = m_rcWndAnimation.GetWidth() % nTotalFrame;
	int nWidthOffset = nCurFrame == nTotalFrame ? m_rcWndAnimation.GetWidth() : nWidthItem * nCurFrame;
	false == m_bShow ? nWidthOffset = m_rcWndAnimation.GetWidth() - nWidthOffset : nWidthOffset;

	int nLeftStart1 = 0;
	int nLeftStart2 = m_rcWndAnimation.GetWidth() - nWidthOffset / 2;
	for (int nTop = 0; nTop < m_rcWndAnimation.GetHeight(); nTop++)
	{
		Gdiplus::ARGB *pSrc1 = (Gdiplus::ARGB*)m_BmpInfoSrc.bmBits + m_rcWndAnimation.GetWidth() * nTop + nLeftStart1;
		Gdiplus::ARGB *pDest1 = (Gdiplus::ARGB*)m_pBmpAniBits + m_rcWndAnimation.GetWidth() * nTop + nLeftStart1;
		memcpy(pDest1, pSrc1, (nWidthOffset / 2) * sizeof(Gdiplus::ARGB));

		Gdiplus::ARGB *pSrc2 = (Gdiplus::ARGB*)m_BmpInfoSrc.bmBits + m_rcWndAnimation.GetWidth() * nTop + nLeftStart2;
		Gdiplus::ARGB *pDest2 = (Gdiplus::ARGB*)m_pBmpAniBits + m_rcWndAnimation.GetWidth() * nTop + nLeftStart2;
		memcpy(pDest2, pSrc2, (nWidthOffset / 2) * sizeof(Gdiplus::ARGB));
	}

	return;
}

void CDUIAnimationWnd::OnAnimateWndLeftRightExpand(int nAnimationID, int nTotalFrame, int nCurFrame)
{
	if (NULL == m_hBitmapSrc || NULL == m_hBitmapAni || NULL == m_pBmpAniBits || NULL == m_BmpInfoSrc.bmBits) return;

	int nWidthItem = m_rcWndAnimation.GetWidth() / nTotalFrame;
	int nWidthSurplus = m_rcWndAnimation.GetWidth() % nTotalFrame;
	int nWidthOffset = nCurFrame == nTotalFrame ? m_rcWndAnimation.GetWidth() : nWidthItem * nCurFrame;
	false == m_bShow ? nWidthOffset = m_rcWndAnimation.GetWidth() - nWidthOffset : nWidthOffset;

	int nLeftStart = m_rcWndAnimation.GetWidth() / 2 - nWidthOffset / 2;
	StretchBlt(m_hMemDCAni, nLeftStart, 0, nWidthOffset, m_rcWndAnimation.GetHeight(),
		m_hMemDCSrc, 0, 0, m_rcWndAnimation.GetWidth(), m_rcWndAnimation.GetHeight(), SRCCOPY);

	return;
}

void CDUIAnimationWnd::OnAnimateWndLeftRightExpandSpring(int nAnimationID, int nTotalFrame, int nCurFrame)
{
	if (Dui_TimerAnimate_ID == nAnimationID)
	{
		OnAnimateWndLeftRightExpand(nAnimationID, nTotalFrame, nCurFrame);

		if (nCurFrame == nTotalFrame)
		{
			StartAnimation(Timer_Spring_ID1, Timer_Spring_Elapse1, Timer_Spring_Frame1);
		}

		return;
	}
	if (Timer_Spring_ID1 == nAnimationID)
	{
		int nWidthOffsetTotal = min(Offset_Spring_Total, m_rcWndAnimation.GetWidth());

		int nWidthItem = nWidthOffsetTotal / nTotalFrame;
		int nWidthSurplus = nWidthOffsetTotal % nTotalFrame;
		int nWidthOffset = nCurFrame == nTotalFrame ? nWidthOffsetTotal : nWidthItem * nCurFrame;

		int nLeftStart = nWidthOffset / 2;
		StretchBlt(m_hMemDCAni, nLeftStart, 0, m_rcWndAnimation.GetWidth() - nWidthOffset, m_rcWndAnimation.GetHeight(),
			m_hMemDCSrc, 0, 0, m_rcWndAnimation.GetWidth(), m_rcWndAnimation.GetHeight(), SRCCOPY);

		if (nCurFrame == nTotalFrame)
		{
			StartAnimation(Timer_Spring_ID2, Timer_Spring_Elapse2, Timer_Spring_Frame2);
		}

		return;
	}
	if (Timer_Spring_ID2 == nAnimationID)
	{
		int nWidthOffsetTotal = min(Offset_Spring_Total, m_rcWndAnimation.GetWidth());

		int nWidthItem = nWidthOffsetTotal / nTotalFrame;
		int nWidthSurplus = nWidthOffsetTotal % nTotalFrame;
		int nWidthOffset = nCurFrame == nTotalFrame ? nWidthOffsetTotal : nWidthItem * nCurFrame;

		int nLeftStart = nWidthOffsetTotal / 2 - nWidthOffset / 2;
		StretchBlt(m_hMemDCAni, nLeftStart, 0, m_rcWndAnimation.GetWidth() - nWidthOffset, m_rcWndAnimation.GetHeight(),
			m_hMemDCSrc, 0, 0, m_rcWndAnimation.GetWidth(), m_rcWndAnimation.GetHeight(), SRCCOPY);

		return;
	}

	return;
}

void CDUIAnimationWnd::OnAnimateWndLeftRightCombin(int nAnimationID, int nTotalFrame, int nCurFrame)
{
	if (NULL == m_hBitmapSrc || NULL == m_hBitmapAni || NULL == m_pBmpAniBits || NULL == m_BmpInfoSrc.bmBits) return;

	int nWidthItem = m_rcWndAnimation.GetWidth() / nTotalFrame;
	int nWidthSurplus = m_rcWndAnimation.GetWidth() % nTotalFrame;
	int nWidthOffset = nCurFrame == nTotalFrame ? m_rcWndAnimation.GetWidth() : nWidthItem * nCurFrame;
	false == m_bShow ? nWidthOffset = m_rcWndAnimation.GetWidth() - nWidthOffset : nWidthOffset;

	int nLeftStartSrc1 = m_rcWndAnimation.GetWidth() / 2 - nWidthOffset / 2;
	int nLeftStartSrc2 = m_rcWndAnimation.GetWidth() / 2;
	int nLeftStartDest1 = 0;
	int nLeftStartDest2 = m_rcWndAnimation.GetWidth() - nWidthOffset / 2;
	for (int nTop = 0; nTop < m_rcWndAnimation.GetHeight(); nTop++)
	{
		Gdiplus::ARGB *pSrc1 = (Gdiplus::ARGB*)m_BmpInfoSrc.bmBits + m_rcWndAnimation.GetWidth() * nTop + nLeftStartSrc1;
		Gdiplus::ARGB *pDest1 = (Gdiplus::ARGB*)m_pBmpAniBits + m_rcWndAnimation.GetWidth() * nTop + nLeftStartDest1;
		memcpy(pDest1, pSrc1, (nWidthOffset / 2) * sizeof(Gdiplus::ARGB));

		Gdiplus::ARGB *pSrc2 = (Gdiplus::ARGB*)m_BmpInfoSrc.bmBits + m_rcWndAnimation.GetWidth() * nTop + nLeftStartSrc2;
		Gdiplus::ARGB *pDest2 = (Gdiplus::ARGB*)m_pBmpAniBits + m_rcWndAnimation.GetWidth() * nTop + nLeftStartDest2;
		memcpy(pDest2, pSrc2, (nWidthOffset / 2) * sizeof(Gdiplus::ARGB));
	}

	return;
}

void CDUIAnimationWnd::OnAnimateWndTopBottomDiffuse(int nAnimationID, int nTotalFrame, int nCurFrame)
{
	if (NULL == m_hBitmapSrc || NULL == m_hBitmapAni || NULL == m_pBmpAniBits || NULL == m_BmpInfoSrc.bmBits) return;

	int nHeightItem = m_rcWndAnimation.GetHeight() / nTotalFrame;
	int nHeightSurplus = m_rcWndAnimation.GetHeight() % nTotalFrame;
	int nHeightOffset = nCurFrame == nTotalFrame ? m_rcWndAnimation.GetHeight() : nHeightItem * nCurFrame;
	false == m_bShow ? nHeightOffset = m_rcWndAnimation.GetHeight() - nHeightOffset : nHeightOffset;

	int nTopStart1 = 0;
	int nTopStart2 = m_rcWndAnimation.GetHeight() - nHeightOffset / 2;
	for (int nTop = nTopStart1; nTop < nTopStart1 + nHeightOffset / 2; nTop++)
	{
		Gdiplus::ARGB *pSrc = (Gdiplus::ARGB*)m_BmpInfoSrc.bmBits + m_rcWndAnimation.GetWidth() * nTop;
		Gdiplus::ARGB *pDest = (Gdiplus::ARGB*)m_pBmpAniBits + m_rcWndAnimation.GetWidth() * nTop;
		memcpy(pDest, pSrc, (m_rcWndAnimation.GetWidth()) * sizeof(Gdiplus::ARGB));
	}
	for (int nTop = nTopStart2; nTop < nTopStart2 + nHeightOffset / 2; nTop++)
	{
		Gdiplus::ARGB *pSrc = (Gdiplus::ARGB*)m_BmpInfoSrc.bmBits + m_rcWndAnimation.GetWidth() * nTop;
		Gdiplus::ARGB *pDest = (Gdiplus::ARGB*)m_pBmpAniBits + m_rcWndAnimation.GetWidth() * nTop;
		memcpy(pDest, pSrc, (m_rcWndAnimation.GetWidth()) * sizeof(Gdiplus::ARGB));
	}

	return;
}

void CDUIAnimationWnd::OnAnimateWndTopBottomExpand(int nAnimationID, int nTotalFrame, int nCurFrame)
{
	if (NULL == m_hBitmapSrc || NULL == m_hBitmapAni || NULL == m_pBmpAniBits || NULL == m_BmpInfoSrc.bmBits) return;

	int nHeightItem = m_rcWndAnimation.GetHeight() / nTotalFrame;
	int nHeightSurplus = m_rcWndAnimation.GetHeight() % nTotalFrame;
	int nHeightOffset = nCurFrame == nTotalFrame ? m_rcWndAnimation.GetHeight() : nHeightItem * nCurFrame;
	false == m_bShow ? nHeightOffset = m_rcWndAnimation.GetHeight() - nHeightOffset : nHeightOffset;

	int nTopStart = m_rcWndAnimation.GetHeight() / 2 - nHeightOffset / 2;
	StretchBlt(m_hMemDCAni, 0, nTopStart, m_rcWndAnimation.GetWidth(), nHeightOffset,
		m_hMemDCSrc, 0, 0, m_rcWndAnimation.GetWidth(), m_rcWndAnimation.GetHeight(), SRCCOPY);

	return;
}

void CDUIAnimationWnd::OnAnimateWndTopBottomExpandSpring(int nAnimationID, int nTotalFrame, int nCurFrame)
{
	if (Dui_TimerAnimate_ID == nAnimationID)
	{
		OnAnimateWndTopBottomExpand(nAnimationID, nTotalFrame, nCurFrame);

		if (nCurFrame == nTotalFrame)
		{
			StartAnimation(Timer_Spring_ID1, Timer_Spring_Elapse1, Timer_Spring_Frame1);
		}

		return;
	}
	if (Timer_Spring_ID1 == nAnimationID)
	{
		int nHeightOffsetTotal = min(Offset_Spring_Total, m_rcWndAnimation.GetHeight());

		int nHeightItem = nHeightOffsetTotal / nTotalFrame;
		int nHeightSurplus = nHeightOffsetTotal % nTotalFrame;
		int nHeightOffset = nCurFrame == nTotalFrame ? nHeightOffsetTotal : nHeightItem * nCurFrame;

		int nTopStart = nHeightOffset / 2;
		StretchBlt(m_hMemDCAni, 0, nTopStart, m_rcWndAnimation.GetWidth(), m_rcWndAnimation.GetHeight() - nHeightOffset,
			m_hMemDCSrc, 0, 0, m_rcWndAnimation.GetWidth(), m_rcWndAnimation.GetHeight(), SRCCOPY);

		if (nCurFrame == nTotalFrame)
		{
			StartAnimation(Timer_Spring_ID2, Timer_Spring_Elapse2, Timer_Spring_Frame2);
		}

		return;
	}
	if (Timer_Spring_ID2 == nAnimationID)
	{
		int nHeightOffsetTotal = min(Offset_Spring_Total, m_rcWndAnimation.GetHeight());

		int nHeightItem = nHeightOffsetTotal / nTotalFrame;
		int nHeightSurplus = nHeightOffsetTotal % nTotalFrame;
		int nHeightOffset = nCurFrame == nTotalFrame ? nHeightOffsetTotal : nHeightItem * nCurFrame;

		int nTopStart = nHeightOffsetTotal / 2 - nHeightOffset / 2;
		StretchBlt(m_hMemDCAni, 0, nTopStart, m_rcWndAnimation.GetWidth(), m_rcWndAnimation.GetHeight() - nHeightOffset,
			m_hMemDCSrc, 0, 0, m_rcWndAnimation.GetWidth(), m_rcWndAnimation.GetHeight(), SRCCOPY);

		return;
	}

	return;
}

void CDUIAnimationWnd::OnAnimateWndTopBottomCombin(int nAnimationID, int nTotalFrame, int nCurFrame)
{
	if (NULL == m_hBitmapSrc || NULL == m_hBitmapAni || NULL == m_pBmpAniBits || NULL == m_BmpInfoSrc.bmBits) return;

	int nHeightItem = m_rcWndAnimation.GetHeight() / nTotalFrame;
	int nHeightSurplus = m_rcWndAnimation.GetHeight() % nTotalFrame;
	int nHeightOffset = nCurFrame == nTotalFrame ? m_rcWndAnimation.GetHeight() : nHeightItem * nCurFrame;
	false == m_bShow ? nHeightOffset = m_rcWndAnimation.GetHeight() - nHeightOffset : nHeightOffset;

	int nTopStartSrc1 = m_rcWndAnimation.GetHeight() / 2 - nHeightOffset / 2;
	int nTopStartSrc2 = m_rcWndAnimation.GetHeight() / 2;
	int nTopStartDest1 = 0;
	int nTopStartDest2 = m_rcWndAnimation.GetHeight() - nHeightOffset / 2;
	for (int nTop = nTopStartDest1; nTop < nTopStartDest1 + nHeightOffset / 2; nTop++)
	{
		Gdiplus::ARGB *pSrc1 = (Gdiplus::ARGB*)m_BmpInfoSrc.bmBits + m_rcWndAnimation.GetWidth() * nTopStartSrc1++;
		Gdiplus::ARGB *pDest1 = (Gdiplus::ARGB*)m_pBmpAniBits + m_rcWndAnimation.GetWidth() * nTop;
		memcpy(pDest1, pSrc1, (m_rcWndAnimation.GetWidth()) * sizeof(Gdiplus::ARGB));
	}
	for (int nTop = nTopStartDest2; nTop < nTopStartDest2 + nHeightOffset / 2; nTop++)
	{
		Gdiplus::ARGB *pSrc1 = (Gdiplus::ARGB*)m_BmpInfoSrc.bmBits + m_rcWndAnimation.GetWidth() * nTopStartSrc2++;
		Gdiplus::ARGB *pDest1 = (Gdiplus::ARGB*)m_pBmpAniBits + m_rcWndAnimation.GetWidth() * nTop;
		memcpy(pDest1, pSrc1, (m_rcWndAnimation.GetWidth()) * sizeof(Gdiplus::ARGB));
	}

	return;
}

void CDUIAnimationWnd::OnAnimateWnd3DRotateLeftRight(int nAnimationID, int nTotalFrame, int nCurFrame)
{
	if (NULL == m_hBitmapSrc || NULL == m_hBitmapAni || NULL == m_pBmpAniBits || NULL == m_BmpInfoSrc.bmBits || NULL == m_pBitmapSrc) return;

	Gdiplus::Graphics Graphics(m_hMemDCAni);

	//angle
	CDUIPoint ptCenter(m_rcWndAnimation.GetWidth() / 2, m_szRotateRadius.cy);
	int nAngleItem = 90 / nTotalFrame;
	float fAngle = nCurFrame == nTotalFrame ? 0.0f : 270.0f + nAngleItem * nCurFrame;
	false == m_bShow ? fAngle = 360.0f - fAngle : fAngle;

	float fArcLeft = (fAngle + 180.0) / 180.0f * PI, fArcRight = fAngle / 180.0 * PI;
	Gdiplus::PointF pts[] = { Gdiplus::PointF(), Gdiplus::PointF(), Gdiplus::PointF() };
	pts[0].X = cos(fArcLeft) * m_szRotateRadius.cx + ptCenter.x;
	pts[0].Y = sin(fArcLeft) * m_szRotateRadius.cy + ptCenter.y;
	pts[1].X = cos(fArcRight) * m_szRotateRadius.cx + ptCenter.x;
	pts[1].Y = sin(fArcRight) * m_szRotateRadius.cy + ptCenter.y;
	pts[2].X = pts[0].X;
	pts[2].Y = pts[0].Y + m_rcWndSrc.GetHeight();
	Graphics.DrawImage(m_pBitmapSrc, pts, sizeof(pts) / sizeof(Gdiplus::PointF));

	return;
}

void CDUIAnimationWnd::OnAnimateWnd3DRotateTopBottom(int nAnimationID, int nTotalFrame, int nCurFrame)
{
	if (NULL == m_hBitmapSrc || NULL == m_hBitmapAni || NULL == m_pBmpAniBits || NULL == m_BmpInfoSrc.bmBits || NULL == m_pBitmapSrc) return;

	Gdiplus::Graphics Graphics(m_hMemDCAni);

	//angle
	CDUIPoint ptCenter(m_szRotateRadius.cx, m_rcWndAnimation.GetHeight() / 2);
	int nAngleItem = 90 / nTotalFrame;
	float fAngle = nCurFrame == nTotalFrame ? 90.0f : 180.0f - nAngleItem * nCurFrame;
	false == m_bShow ? fAngle = 90.0f + fAngle : fAngle;

	float fArcTop = (fAngle + 180.0) / 180.0f * PI, fArcBottom = fAngle / 180.0 * PI;
	Gdiplus::PointF pts[] = { Gdiplus::PointF(), Gdiplus::PointF(), Gdiplus::PointF() };
	pts[0].X = cos(fArcTop) * m_szRotateRadius.cx + ptCenter.x;
	pts[0].Y = sin(fArcTop) * m_szRotateRadius.cy + ptCenter.y;
	pts[1].X = pts[0].X + m_rcWndSrc.GetWidth();
	pts[1].Y = pts[0].Y; 
	pts[2].X = cos(fArcBottom) * m_szRotateRadius.cx + ptCenter.x;
	pts[2].Y = sin(fArcBottom) * m_szRotateRadius.cy + ptCenter.y;
	Graphics.DrawImage(m_pBitmapSrc, pts, sizeof(pts) / sizeof(Gdiplus::PointF));

	return;
}

void CDUIAnimationWnd::OnAnimateWnd3DRoundLeft(int nAnimationID, int nTotalFrame, int nCurFrame)
{
	if (NULL == m_hBitmapSrc || NULL == m_hBitmapAni || NULL == m_pBmpAniBits || NULL == m_BmpInfoSrc.bmBits || NULL == m_pBitmapSrc) return;

	Gdiplus::Graphics Graphics(m_hMemDCAni);

	//width height
	int nWidthMin = min(100, m_rcWndSrc.GetWidth() / 2 / 3);
	int nWidthItem = (m_rcWndSrc.GetWidth() / 2 - nWidthMin) / nTotalFrame;	
	m_szRotateRadius.cx = nCurFrame == nTotalFrame ? m_rcWndSrc.GetWidth() / 2 : nWidthMin + nWidthItem * nCurFrame;
	false == m_bShow ? m_szRotateRadius.cx = m_rcWndSrc.GetWidth() / 2 - m_szRotateRadius.cx : m_szRotateRadius.cx;

	int nHeightMin = min(100, m_rcWndSrc.GetHeight() / 3);
	int nHeightItem = (m_rcWndSrc.GetHeight() - nHeightMin) / nTotalFrame;
	int nHeightCur = nCurFrame == nTotalFrame ? m_rcWndSrc.GetHeight() : nHeightMin + nHeightItem * nCurFrame;
	false == m_bShow ? nHeightCur = m_rcWndSrc.GetHeight() - nHeightCur : nHeightCur;

	//angle
	int nAngleTotal = 180, nAngleRotateTotal = 180;
	int nAngleItem = nAngleTotal / nTotalFrame, nAngleRotateItem = nAngleRotateTotal / nTotalFrame;
	float fAngle = nCurFrame == nTotalFrame ? 270.0f - nAngleTotal : 270.0f - nAngleItem * nCurFrame;
	false == m_bShow ? fAngle = 270.f - nAngleTotal + fAngle : fAngle;

	float fAngleRotate = nCurFrame == nTotalFrame ? 0.0f : 360.0f - nAngleRotateTotal + nAngleRotateItem * nCurFrame;
	false == m_bShow ? fAngleRotate = 360.0f - fAngleRotate : fAngleRotate;
	float fArcRotateLeft = (fAngleRotate + 180.0) / 180.0f * PI, fArcRotateRight = fAngleRotate / 180.0 * PI;
	
	//center
	m_szRoundRadius.cx = m_rcWndSrc.GetWidth() / 4;
	CDUIPoint ptCenter(m_rcWndAnimation.GetWidth() / 2, m_rcWndAnimation.GetHeight() / 2);
	ptCenter.x += cos(fAngle / 180.0 * PI) * m_szRoundRadius.cx;
	ptCenter.y += sin(fAngle / 180.0 * PI) * m_szRoundRadius.cy - nHeightCur / 2;

	Gdiplus::PointF pts[] = { Gdiplus::PointF(), Gdiplus::PointF(), Gdiplus::PointF() };
	pts[0].X = cos(fArcRotateLeft) * m_szRotateRadius.cx + ptCenter.x;
	pts[0].Y = sin(fArcRotateLeft) * m_szRotateRadius.cy + ptCenter.y;
	pts[1].X = cos(fArcRotateRight) * m_szRotateRadius.cx + ptCenter.x;
	pts[1].Y = sin(fArcRotateRight) * m_szRotateRadius.cy + ptCenter.y;
	pts[2].X = pts[0].X;
	pts[2].Y = pts[0].Y + nHeightCur;
	Graphics.DrawImage(m_pBitmapSrc, pts, sizeof(pts) / sizeof(Gdiplus::PointF));

	return;
}

void CDUIAnimationWnd::OnAnimateWnd3DRoundRight(int nAnimationID, int nTotalFrame, int nCurFrame)
{
	if (NULL == m_hBitmapSrc || NULL == m_hBitmapAni || NULL == m_pBmpAniBits || NULL == m_BmpInfoSrc.bmBits || NULL == m_pBitmapSrc) return;

	Gdiplus::Graphics Graphics(m_hMemDCAni);

	//width height
	int nWidthMin = min(100, m_rcWndSrc.GetWidth() / 2 / 3);
	int nWidthItem = (m_rcWndSrc.GetWidth() / 2 - nWidthMin) / nTotalFrame;
	m_szRotateRadius.cx = nCurFrame == nTotalFrame ? m_rcWndSrc.GetWidth() / 2 : nWidthMin + nWidthItem * nCurFrame;
	false == m_bShow ? m_szRotateRadius.cx = m_rcWndSrc.GetWidth() / 2 - m_szRotateRadius.cx : m_szRotateRadius.cx;

	int nHeightMin = min(100, m_rcWndSrc.GetHeight() / 3);
	int nHeightItem = (m_rcWndSrc.GetHeight() - nHeightMin) / nTotalFrame;
	int nHeightCur = nCurFrame == nTotalFrame ? m_rcWndSrc.GetHeight() : nHeightMin + nHeightItem * nCurFrame;
	false == m_bShow ? nHeightCur = m_rcWndSrc.GetHeight() - nHeightCur : nHeightCur;

	//angle
	int nAngleTotal = 180, nAngleRotateTotal = 180;
	int nAngleItem = nAngleTotal / nTotalFrame, nAngleRotateItem = nAngleRotateTotal / nTotalFrame;
	float fAngle = nCurFrame == nTotalFrame ? 270.0f + nAngleTotal : 270.0f + nAngleItem * nCurFrame;
	false == m_bShow ? fAngle = 270.0f + nAngleTotal - fAngle : fAngle;
	
	float fAngleRotate = nCurFrame == nTotalFrame ? 0.0f : 360.0f - nAngleRotateTotal + nAngleRotateItem * nCurFrame;
	false == m_bShow ? fAngleRotate = 360.0f - fAngleRotate : fAngleRotate;
	float fArcRotateLeft = (fAngleRotate + 180.0) / 180.0f * PI, fArcRotateRight = fAngleRotate / 180.0 * PI;

	//center
	m_szRoundRadius.cx = m_rcWndSrc.GetWidth() / 4;
	CDUIPoint ptCenter(m_rcWndAnimation.GetWidth() / 2, m_rcWndAnimation.GetHeight() / 2);
	ptCenter.x += cos(fAngle / 180.0 * PI) * m_szRoundRadius.cx;
	ptCenter.y += sin(fAngle / 180.0 * PI) * m_szRoundRadius.cy - nHeightCur / 2;

	Gdiplus::PointF pts[] = { Gdiplus::PointF(), Gdiplus::PointF(), Gdiplus::PointF() };
	pts[0].X = cos(fArcRotateLeft) * m_szRotateRadius.cx + ptCenter.x;
	pts[0].Y = sin(fArcRotateLeft) * m_szRotateRadius.cy + ptCenter.y;
	pts[1].X = cos(fArcRotateRight) * m_szRotateRadius.cx + ptCenter.x;
	pts[1].Y = sin(fArcRotateRight) * m_szRotateRadius.cy + ptCenter.y;
	pts[2].X = pts[0].X;
	pts[2].Y = pts[0].Y + nHeightCur;
	Graphics.DrawImage(m_pBitmapSrc, pts, sizeof(pts) / sizeof(Gdiplus::PointF));

	return;
}

void CDUIAnimationWnd::OnAnimateWnd3DRoundTop(int nAnimationID, int nTotalFrame, int nCurFrame)
{
	if (NULL == m_hBitmapSrc || NULL == m_hBitmapAni || NULL == m_pBmpAniBits || NULL == m_BmpInfoSrc.bmBits || NULL == m_pBitmapSrc) return;

	Gdiplus::Graphics Graphics(m_hMemDCAni);

	//width height
	int nWidthMin = min(100, m_rcWndSrc.GetWidth() / 3);
	int nWidthItem = (m_rcWndSrc.GetWidth() - nWidthMin) / nTotalFrame;
	int nWidthCur = nCurFrame == nTotalFrame ? m_rcWndSrc.GetWidth() : nWidthMin + nWidthItem * nCurFrame;
	false == m_bShow ? nWidthCur = m_rcWndSrc.GetWidth() - nWidthCur : nWidthCur;

	int nHeightMin = min(100, m_rcWndSrc.GetHeight() / 2 / 3);
	int nHeightItem = (m_rcWndSrc.GetHeight() / 2 - nHeightMin) / nTotalFrame;
	m_szRotateRadius.cy = nCurFrame == nTotalFrame ? m_rcWndSrc.GetHeight() / 2 : nHeightMin + nHeightItem * nCurFrame;
	false == m_bShow ? m_szRotateRadius.cy = m_rcWndSrc.GetHeight() / 2 - m_szRotateRadius.cy : m_szRotateRadius.cy;

	//angle
	int nAngleTotal = 180, nAngleRotateTotal = 180;
	int nAngleItem = nAngleTotal / nTotalFrame, nAngleRotateItem = nAngleRotateTotal / nTotalFrame;
	float fAngle = nCurFrame == nTotalFrame ? 270.0f - nAngleTotal : 270.0f - nAngleItem * nCurFrame;
	false == m_bShow ? fAngle = 270.0f - fAngle : fAngle;
	
	float fAngleRotate = nCurFrame == nTotalFrame ? 0.0f : 360.0f - nAngleRotateTotal + nAngleRotateItem * nCurFrame;
	false == m_bShow ? fAngleRotate = 360.0f - fAngleRotate : fAngleRotate;
	float fArcRotateTop = (fAngleRotate + 180.0) / 180.0f * PI, fArcRotateBottom = fAngleRotate / 180.0 * PI;

	//center
	m_szRoundRadius.cy = m_rcWndSrc.GetHeight() / 4;
	CDUIPoint ptCenter(m_rcWndAnimation.GetWidth() / 2, m_rcWndAnimation.GetHeight() / 2);
	ptCenter.x += sin(fAngle / 180.0 * PI) * m_szRoundRadius.cx - nWidthCur / 2;
	ptCenter.y += cos(fAngle / 180.0 * PI) * m_szRoundRadius.cy;

	Gdiplus::PointF pts[] = { Gdiplus::PointF(), Gdiplus::PointF(), Gdiplus::PointF() };
	pts[0].X = sin(fArcRotateTop) * m_szRotateRadius.cx + ptCenter.x;
	pts[0].Y = cos(fArcRotateTop) * m_szRotateRadius.cy + ptCenter.y;
	pts[1].X = pts[0].X + nWidthCur;
	pts[1].Y = pts[0].Y;
	pts[2].X = sin(fArcRotateBottom) * m_szRotateRadius.cx + ptCenter.x;
	pts[2].Y = cos(fArcRotateBottom) * m_szRotateRadius.cy + ptCenter.y;
	Graphics.DrawImage(m_pBitmapSrc, pts, sizeof(pts) / sizeof(Gdiplus::PointF));

	return;
}

void CDUIAnimationWnd::OnAnimateWnd3DRoundBottom(int nAnimationID, int nTotalFrame, int nCurFrame)
{
	if (NULL == m_hBitmapSrc || NULL == m_hBitmapAni || NULL == m_pBmpAniBits || NULL == m_BmpInfoSrc.bmBits || NULL == m_pBitmapSrc) return;

	Gdiplus::Graphics Graphics(m_hMemDCAni);

	//width height
	int nWidthMin = min(100, m_rcWndSrc.GetWidth() / 3);
	int nWidthItem = (m_rcWndSrc.GetWidth() - nWidthMin) / nTotalFrame;
	int nWidthCur = nCurFrame == nTotalFrame ? m_rcWndSrc.GetWidth() : nWidthMin + nWidthItem * nCurFrame;
	false == m_bShow ? nWidthCur = m_rcWndSrc.GetWidth() - nWidthCur : nWidthCur;

	int nHeightMin = min(100, m_rcWndSrc.GetHeight() / 2 / 3);
	int nHeightItem = (m_rcWndSrc.GetHeight() / 2 - nHeightMin) / nTotalFrame;
	m_szRotateRadius.cy = nCurFrame == nTotalFrame ? m_rcWndSrc.GetHeight() / 2 : nHeightMin + nHeightItem * nCurFrame;
	false == m_bShow ? m_szRotateRadius.cy = m_rcWndSrc.GetHeight() / 2 - m_szRotateRadius.cy : m_szRotateRadius.cy;

	//angle
	int nAngleTotal = 180, nAngleRotateTotal = 180;
	int nAngleItem = nAngleTotal / nTotalFrame, nAngleRotateItem = nAngleRotateTotal / nTotalFrame;
	float fAngle = nCurFrame == nTotalFrame ? 270.0f + nAngleTotal : 270.0f + nAngleItem * nCurFrame;
	false == m_bShow ? fAngle = 270.0f - fAngle : fAngle;
	
	float fAngleRotate = nCurFrame == nTotalFrame ? 0.0f : 360.0f - nAngleRotateTotal + nAngleRotateItem * nCurFrame;
	false == m_bShow ? fAngleRotate = 360.0f - fAngleRotate : fAngleRotate;
	float fArcRotateTop = (fAngleRotate + 180.0) / 180.0f * PI, fArcRotateBottom = fAngleRotate / 180.0 * PI;

	//center
	m_szRoundRadius.cy = m_rcWndSrc.GetHeight() / 4;
	CDUIPoint ptCenter(m_rcWndAnimation.GetWidth() / 2, m_rcWndAnimation.GetHeight() / 2);
	ptCenter.x += sin(fAngle / 180.0 * PI) * m_szRoundRadius.cx - nWidthCur / 2;
	ptCenter.y += cos(fAngle / 180.0 * PI) * m_szRoundRadius.cy;

	Gdiplus::PointF pts[] = { Gdiplus::PointF(), Gdiplus::PointF(), Gdiplus::PointF() };
	pts[0].X = sin(fArcRotateTop) * m_szRotateRadius.cx + ptCenter.x;
	pts[0].Y = cos(fArcRotateTop) * m_szRotateRadius.cy + ptCenter.y;
	pts[1].X = pts[0].X + nWidthCur;
	pts[1].Y = pts[0].Y;
	pts[2].X = sin(fArcRotateBottom) * m_szRotateRadius.cx + ptCenter.x;
	pts[2].Y = cos(fArcRotateBottom) * m_szRotateRadius.cy + ptCenter.y;
	Graphics.DrawImage(m_pBitmapSrc, pts, sizeof(pts) / sizeof(Gdiplus::PointF));

	return;
}

void CDUIAnimationWnd::OnAnimateWnd3DRotateReplaseLR(int nAnimationID, int nTotalFrame, int nCurFrame)
{
	if (NULL == m_hBitmapSrc || NULL == m_hBitmapAni || NULL == m_pBmpAniBits || NULL == m_BmpInfoSrc.bmBits || NULL == m_pBitmapSrc || NULL == m_pBitmapReplase) return;

	Gdiplus::Graphics Graphics(m_hMemDCAni);

	//frame
	int nTotalFrameSeparate = nTotalFrame / 2;
	int nAnimateFrame = nCurFrame <= nTotalFrameSeparate ? nCurFrame : nCurFrame - nTotalFrameSeparate;
	bool bFirstAnimate = nCurFrame <= nTotalFrameSeparate;

	//width height
	int nWidthMin = m_rcWndSrc.GetWidth() / 3 * 2;
	int nWidthItem = (m_rcWndSrc.GetWidth() - nWidthMin) / (nTotalFrameSeparate);
	int nWidthCur = bFirstAnimate ? m_rcWndSrc.GetWidth() - nWidthItem * nAnimateFrame : nWidthMin + nWidthItem * nAnimateFrame;
	nWidthCur = nCurFrame == nTotalFrame ? m_rcWndSrc.GetWidth() : nWidthCur;
	m_szRotateRadius.cx = nWidthCur / 2;

	int nHeightMin = m_rcWndSrc.GetHeight() / 3 * 2;
	int nHeightItem = (m_rcWndSrc.GetHeight() - nHeightMin) / (nTotalFrameSeparate);
	int nHeightCur = bFirstAnimate ? m_rcWndSrc.GetHeight() - nHeightItem * nAnimateFrame : nHeightMin + nHeightItem * nAnimateFrame;
	nHeightCur = nCurFrame == nTotalFrame ? m_rcWndSrc.GetHeight() : nHeightCur;
	m_szRotateRadius.cy = 10;

	//angle
	CDUIPoint ptCenter(m_rcWndAnimation.GetWidth() / 2, m_rcWndAnimation.GetHeight() / 2 - nHeightCur / 2);
	int nAngleItem = 180 / nTotalFrame;
	float fAngle = bFirstAnimate ? nAngleItem * nAnimateFrame : 270.0f + nAngleItem * nAnimateFrame;
	fAngle = nCurFrame == nTotalFrame ? 0 : fAngle;
	float fArcLeft = (fAngle + 180.0) / 180.0f * PI, fArcRight = fAngle / 180.0 * PI;
	Gdiplus::Bitmap *pBitmap = fAngle <= 90.0f ? m_pBitmapSrc : m_pBitmapReplase;
	Gdiplus::PointF pts[] = { Gdiplus::PointF(), Gdiplus::PointF(), Gdiplus::PointF() };
	pts[0].X = cos(fArcLeft) * m_szRotateRadius.cx + ptCenter.x;
	pts[0].Y = sin(fArcLeft) * m_szRotateRadius.cy + ptCenter.y;
	pts[1].X = cos(fArcRight) * m_szRotateRadius.cx + ptCenter.x;
	pts[1].Y = sin(fArcRight) * m_szRotateRadius.cy + ptCenter.y;
	pts[2].X = pts[0].X;
	pts[2].Y = pts[0].Y + nHeightCur;
	Graphics.DrawImage(pBitmap, pts, sizeof(pts) / sizeof(Gdiplus::PointF));

	return;
}

void CDUIAnimationWnd::OnAnimateWnd3DRotateReplaseTB(int nAnimationID, int nTotalFrame, int nCurFrame)
{
	if (NULL == m_hBitmapSrc || NULL == m_hBitmapAni || NULL == m_pBmpAniBits || NULL == m_BmpInfoSrc.bmBits || NULL == m_pBitmapSrc || NULL == m_pBitmapReplase) return;

	Gdiplus::Graphics Graphics(m_hMemDCAni);

	//frame
	int nTotalFrameSeparate = nTotalFrame / 2;
	int nAnimateFrame = nCurFrame <= nTotalFrameSeparate ? nCurFrame : nCurFrame - nTotalFrameSeparate;
	bool bFirstAnimate = nCurFrame <= nTotalFrameSeparate;

	//width height
	int nWidthMin = m_rcWndSrc.GetWidth() / 3 * 2;
	int nWidthItem = (m_rcWndSrc.GetWidth() - nWidthMin) / (nTotalFrameSeparate);
	int nWidthCur = bFirstAnimate ? m_rcWndSrc.GetWidth() - nWidthItem * nAnimateFrame : nWidthMin + nWidthItem * nAnimateFrame;
	nWidthCur = nCurFrame == nTotalFrame ? m_rcWndSrc.GetWidth() : nWidthCur;
	m_szRotateRadius.cx = 10;

	int nHeightMin = m_rcWndSrc.GetHeight() / 3 * 2;
	int nHeightItem = (m_rcWndSrc.GetHeight() - nHeightMin) / (nTotalFrameSeparate);
	int nHeightCur = bFirstAnimate ? m_rcWndSrc.GetHeight() - nHeightItem * nAnimateFrame : nHeightMin + nHeightItem * nAnimateFrame;
	nHeightCur = nCurFrame == nTotalFrame ? m_rcWndSrc.GetHeight() : nHeightCur;
	m_szRotateRadius.cy = nHeightCur / 2;

	//angle
	CDUIPoint ptCenter(m_rcWndAnimation.GetWidth() / 2 - nWidthCur / 2, m_rcWndAnimation.GetHeight() / 2);
	int nAngleItem = 180 / nTotalFrame;
	float fAngle = bFirstAnimate ? nAngleItem * nAnimateFrame : 270.0f + nAngleItem * nAnimateFrame;
	fAngle = nCurFrame == nTotalFrame ? 0 : fAngle;
	float fArcTop = (fAngle + 180.0) / 180.0f * PI, fArcBottom = fAngle / 180.0 * PI;
	Gdiplus::Bitmap *pBitmap = fAngle <= 90.0f ? m_pBitmapSrc : m_pBitmapReplase;
	Gdiplus::PointF pts[] = { Gdiplus::PointF(), Gdiplus::PointF(), Gdiplus::PointF() };
	pts[0].X = sin(fArcTop) * m_szRotateRadius.cx + ptCenter.x;
	pts[0].Y = cos(fArcTop) * m_szRotateRadius.cy + ptCenter.y;
	pts[1].X = pts[0].X + nWidthCur;
	pts[1].Y = pts[0].Y;
	pts[2].X = sin(fArcBottom) * m_szRotateRadius.cx + ptCenter.x;
	pts[2].Y = cos(fArcBottom) * m_szRotateRadius.cy + ptCenter.y;
	Graphics.DrawImage(pBitmap, pts, sizeof(pts) / sizeof(Gdiplus::PointF));

	return;
}

void CDUIAnimationWnd::OnAnimateWnd3DRoundReplaseLR(int nAnimationID, int nTotalFrame, int nCurFrame)
{
	if (NULL == m_hBitmapSrc || NULL == m_hBitmapAni || NULL == m_pBmpAniBits || NULL == m_BmpInfoSrc.bmBits || NULL == m_pBitmapSrc) return;

	Gdiplus::Graphics Graphics(m_hMemDCAni);

	m_szRoundRadius.cx = m_rcWndSrc.GetWidth() / 4;
	Gdiplus::PointF PtLeft[] = { Gdiplus::PointF(), Gdiplus::PointF(), Gdiplus::PointF() };
	Gdiplus::PointF PtRight[] = { Gdiplus::PointF(), Gdiplus::PointF(), Gdiplus::PointF() };

	//left
	{
		//width height
		int nWidthMin = min(100, m_rcWndSrc.GetWidth() / 2 / 3);
		int nWidthItem = (m_rcWndSrc.GetWidth() / 2 - nWidthMin) / nTotalFrame;
		m_szRotateRadius.cx = nCurFrame == nTotalFrame ? m_rcWndSrc.GetWidth() / 2 : nWidthMin + nWidthItem * nCurFrame;

		int nHeightMin = min(100, m_rcWndSrc.GetHeight() / 3);
		int nHeightItem = (m_rcWndSrc.GetHeight() - nHeightMin) / nTotalFrame;
		int nHeightCur = nCurFrame == nTotalFrame ? m_rcWndSrc.GetHeight() : nHeightMin + nHeightItem * nCurFrame;
		m_szRotateRadius.cy = 10;

		//angle
		int nAngleTotal = 180, nAngleRotateTotal = 180;
		int nAngleItem = nAngleTotal / nTotalFrame, nAngleRotateItem = nAngleRotateTotal / nTotalFrame;
		float fAngle = nCurFrame == nTotalFrame ? 270.0f - nAngleTotal : 270.0f - nAngleItem * nCurFrame;
		float fAngleRotate = nCurFrame == nTotalFrame ? 0.0f : 360.0f - nAngleRotateTotal + nAngleRotateItem * nCurFrame;
		float fArcRotateLeft = (fAngleRotate + 180.0) / 180.0f * PI, fArcRotateRight = fAngleRotate / 180.0 * PI;

		//center
		CDUIPoint ptCenter(m_rcWndAnimation.GetWidth() / 2, m_rcWndAnimation.GetHeight() / 2);
		ptCenter.x += cos(fAngle / 180.0 * PI) * m_szRoundRadius.cx;
		ptCenter.y += sin(fAngle / 180.0 * PI) * m_szRoundRadius.cy - nHeightCur / 2;

		PtLeft[0].X = cos(fArcRotateLeft) * m_szRotateRadius.cx + ptCenter.x;
		PtLeft[0].Y = sin(fArcRotateLeft) * m_szRotateRadius.cy + ptCenter.y;
		PtLeft[1].X = cos(fArcRotateRight) * m_szRotateRadius.cx + ptCenter.x;
		PtLeft[1].Y = sin(fArcRotateRight) * m_szRotateRadius.cy + ptCenter.y;
		PtLeft[2].X = PtLeft[0].X;
		PtLeft[2].Y = PtLeft[0].Y + nHeightCur;
	}

	//right
	{
		//width height
		int nWidthMin = min(100, m_rcWndSrc.GetWidth() / 2 / 3);
		int nWidthItem = (m_rcWndSrc.GetWidth() / 2 - nWidthMin) / nTotalFrame;
		m_szRotateRadius.cx = nCurFrame == nTotalFrame ? nWidthMin : m_rcWndSrc.GetWidth() / 2 - nWidthItem * nCurFrame;

		int nHeightMin = min(100, m_rcWndSrc.GetHeight() / 3);
		int nHeightItem = (m_rcWndSrc.GetHeight() - nHeightMin) / nTotalFrame;
		int nHeightCur = nCurFrame == nTotalFrame ? nHeightMin : m_rcWndSrc.GetHeight() - nHeightItem * nCurFrame;
		m_szRotateRadius.cy = 10;

		//angle
		int nAngleTotal = 180, nAngleRotateTotal = 180;
		int nAngleItem = nAngleTotal / nTotalFrame, nAngleRotateItem = nAngleRotateTotal / nTotalFrame;
		float fAngle = nCurFrame == nTotalFrame ? 180 : nAngleItem * nCurFrame;
		float fAngleRotate = nCurFrame == nTotalFrame ? 180.0f : nAngleRotateItem * nCurFrame;
		float fArcRotateLeft = (fAngleRotate + 180.0) / 180.0f * PI, fArcRotateRight = fAngleRotate / 180.0 * PI;

		//center
		CDUIPoint ptCenter(m_rcWndAnimation.GetWidth() / 2, m_rcWndAnimation.GetHeight() / 2);
		ptCenter.x += cos(fAngle / 180.0 * PI) * m_szRoundRadius.cx;
		ptCenter.y += sin(fAngle / 180.0 * PI) * m_szRoundRadius.cy - nHeightCur / 2;

		PtRight[0].X = cos(fArcRotateLeft) * m_szRotateRadius.cx + ptCenter.x;
		PtRight[0].Y = sin(fArcRotateLeft) * m_szRotateRadius.cy + ptCenter.y;
		PtRight[1].X = cos(fArcRotateRight) * m_szRotateRadius.cx + ptCenter.x;
		PtRight[1].Y = sin(fArcRotateRight) * m_szRotateRadius.cy + ptCenter.y;
		PtRight[2].X = PtRight[0].X;
		PtRight[2].Y = PtRight[0].Y + nHeightCur;
	}

	//first next
	if (nCurFrame <= nTotalFrame / 2)
	{
		Graphics.DrawImage(m_pBitmapReplase, PtLeft, sizeof(PtLeft) / sizeof(Gdiplus::PointF));
		Graphics.DrawImage(m_pBitmapSrc, PtRight, sizeof(PtRight) / sizeof(Gdiplus::PointF));
	}
	else
	{
		Graphics.DrawImage(m_pBitmapSrc, PtRight, sizeof(PtRight) / sizeof(Gdiplus::PointF));
		Graphics.DrawImage(m_pBitmapReplase, PtLeft, sizeof(PtLeft) / sizeof(Gdiplus::PointF));
	}

	return;
}

void CDUIAnimationWnd::OnAnimateWnd3DRoundReplaseTB(int nAnimationID, int nTotalFrame, int nCurFrame)
{
	if (NULL == m_hBitmapSrc || NULL == m_hBitmapAni || NULL == m_pBmpAniBits || NULL == m_BmpInfoSrc.bmBits || NULL == m_pBitmapSrc) return;

	Gdiplus::Graphics Graphics(m_hMemDCAni);

	m_szRoundRadius.cy = m_rcWndSrc.GetHeight() / 4;
	Gdiplus::PointF PtTop[] = { Gdiplus::PointF(), Gdiplus::PointF(), Gdiplus::PointF() };
	Gdiplus::PointF PtBottom[] = { Gdiplus::PointF(), Gdiplus::PointF(), Gdiplus::PointF() };

	//top
	{
		//width height
		int nWidthMin = min(100, m_rcWndSrc.GetWidth() / 3);
		int nWidthItem = (m_rcWndSrc.GetWidth() - nWidthMin) / nTotalFrame;
		int nWidthCur = nCurFrame == nTotalFrame ? m_rcWndSrc.GetWidth() : nWidthMin + nWidthItem * nCurFrame;
		m_szRotateRadius.cx = 10;

		int nHeightMin = min(100, m_rcWndSrc.GetHeight() / 2 / 3);
		int nHeightItem = (m_rcWndSrc.GetHeight() / 2 - nHeightMin) / nTotalFrame;
		m_szRotateRadius.cy = nCurFrame == nTotalFrame ? m_rcWndSrc.GetHeight() / 2 : nHeightMin + nHeightItem * nCurFrame;

		//angle
		int nAngleTotal = 180, nAngleRotateTotal = 180;
		int nAngleItem = nAngleTotal / nTotalFrame, nAngleRotateItem = nAngleRotateTotal / nTotalFrame;
		float fAngle = nCurFrame == nTotalFrame ? 270.0f - nAngleTotal : 270.0f - nAngleItem * nCurFrame;
		float fAngleRotate = nCurFrame == nTotalFrame ? 0.0f : 360.0f - nAngleRotateTotal + nAngleRotateItem * nCurFrame;
		float fArcRotateTop = (fAngleRotate + 180.0) / 180.0f * PI, fArcRotateBottom = fAngleRotate / 180.0 * PI;

		//center
		CDUIPoint ptCenter(m_rcWndAnimation.GetWidth() / 2, m_rcWndAnimation.GetHeight() / 2);
		ptCenter.x += sin(fAngle / 180.0 * PI) * m_szRoundRadius.cx - nWidthCur / 2;
		ptCenter.y += cos(fAngle / 180.0 * PI) * m_szRoundRadius.cy;

		PtTop[0].X = sin(fArcRotateTop) * m_szRotateRadius.cx + ptCenter.x;
		PtTop[0].Y = cos(fArcRotateTop) * m_szRotateRadius.cy + ptCenter.y;
		PtTop[1].X = PtTop[0].X + nWidthCur;
		PtTop[1].Y = PtTop[0].Y;
		PtTop[2].X = sin(fArcRotateBottom) * m_szRotateRadius.cx + ptCenter.x;
		PtTop[2].Y = cos(fArcRotateBottom) * m_szRotateRadius.cy + ptCenter.y;
	}

	//bottom
	{
		//width height
		int nWidthMin = min(100, m_rcWndSrc.GetWidth() / 3);
		int nWidthItem = (m_rcWndSrc.GetWidth() - nWidthMin) / nTotalFrame;
		int nWidthCur = nCurFrame == nTotalFrame ? nWidthMin : m_rcWndSrc.GetWidth() - nWidthItem * nCurFrame;
		m_szRotateRadius.cy = 10;

		int nHeightMin = min(100, m_rcWndSrc.GetHeight() / 2 / 3);
		int nHeightItem = (m_rcWndSrc.GetHeight() / 2 - nHeightMin) / nTotalFrame;
		m_szRotateRadius.cy = nCurFrame == nTotalFrame ? nHeightMin : m_rcWndSrc.GetHeight() / 2 - nHeightItem * nCurFrame;

		//angle
		int nAngleTotal = 180, nAngleRotateTotal = 180;
		int nAngleItem = nAngleTotal / nTotalFrame, nAngleRotateItem = nAngleRotateTotal / nTotalFrame;
		float fAngle = nCurFrame == nTotalFrame ? 180 : nAngleItem * nCurFrame;
		float fAngleRotate = nCurFrame == nTotalFrame ? 180.0f : nAngleRotateItem * nCurFrame;
		float fArcRotateTop = (fAngleRotate + 180.0) / 180.0f * PI, fArcRotateBottom = fAngleRotate / 180.0 * PI;

		//center
		CDUIPoint ptCenter(m_rcWndAnimation.GetWidth() / 2, m_rcWndAnimation.GetHeight() / 2);
		ptCenter.x += sin(fAngle / 180.0 * PI) * m_szRoundRadius.cx - nWidthCur / 2;
		ptCenter.y += cos(fAngle / 180.0 * PI) * m_szRoundRadius.cy;

		PtBottom[0].X = sin(fArcRotateTop) * m_szRotateRadius.cx + ptCenter.x;
		PtBottom[0].Y = cos(fArcRotateTop) * m_szRotateRadius.cy + ptCenter.y;
		PtBottom[1].X = PtBottom[0].X + nWidthCur;
		PtBottom[1].Y = PtBottom[0].Y;
		PtBottom[2].X = sin(fArcRotateBottom) * m_szRotateRadius.cx + ptCenter.x;
		PtBottom[2].Y = cos(fArcRotateBottom) * m_szRotateRadius.cy + ptCenter.y;
	}

	//first next
	if (nCurFrame <= nTotalFrame / 2)
	{
		Graphics.DrawImage(m_pBitmapReplase, PtTop, sizeof(PtTop) / sizeof(Gdiplus::PointF));
		Graphics.DrawImage(m_pBitmapSrc, PtBottom, sizeof(PtBottom) / sizeof(Gdiplus::PointF));
	}
	else
	{
		Graphics.DrawImage(m_pBitmapSrc, PtBottom, sizeof(PtBottom) / sizeof(Gdiplus::PointF));
		Graphics.DrawImage(m_pBitmapReplase, PtTop, sizeof(PtTop) / sizeof(Gdiplus::PointF));
	}

	return;
}



