#include "StdAfx.h"
#include "DUIContainerCtrl.h"

DuiImplement_CreateControl(CDUIContainerCtrl)
MMImplement_ClassName(CDUIContainerCtrl)

CDUIContainerCtrl::CDUIContainerCtrl(void)
{
	m_pHorizScrollBarCtrl = NULL;
	m_pVertScrollBarCtrl = NULL;

	return;
}

CDUIContainerCtrl::~CDUIContainerCtrl(void)
{
	for (int nIndex = 0; nIndex < GetChildCount(); nIndex++)
	{
		CDUIControlBase *pChild = GetChildAt(nIndex);
		MMSafeDelete(pChild);
	}

	m_vecChilds.clear();
	MMSafeDelete(m_pHorizScrollBarCtrl);
	MMSafeDelete(m_pVertScrollBarCtrl);

	return;
}

bool CDUIContainerCtrl::OnAttributeChange(CDUIAttributeObject *pAttributeObj)
{
	if (__super::OnAttributeChange(pAttributeObj)) return true;

	if (pAttributeObj == &m_AttributeVisible)
	{
		RefreshChildInternVisible();

		return true;
	}
	if (pAttributeObj == &m_AttributeUseHorizScrollBar)
	{
		SetUseHorizScrollBar(IsUseHorizScrollBar());

		return true;
	}
	if (pAttributeObj == &m_AttributeUseVertScrollBar)
	{
		SetUseVertScrollBar(IsUseVertScrollBar());

		return true;
	}

	return false;
}

void CDUIContainerCtrl::OnDpiChanged(int nScalePre)
{
	__super::OnDpiChanged(nScalePre);

	for (int nIndex = 0; nIndex < GetChildCount(); nIndex++)
	{
		CDUIControlBase *pChild = GetChildAt(nIndex);
		if (NULL == pChild) continue;

		pChild->OnDpiChanged(nScalePre);
	}
	if (m_pHorizScrollBarCtrl)
	{
		m_pHorizScrollBarCtrl->OnDpiChanged(nScalePre);
	}
	if (m_pVertScrollBarCtrl)
	{
		m_pVertScrollBarCtrl->OnDpiChanged(nScalePre);
	}

	return;
}

bool CDUIContainerCtrl::RegisterControlListen(IDuiControlListen *pIDuiControlListen)
{
	if (NULL == pIDuiControlListen || false == __super::RegisterControlListen(pIDuiControlListen)) return false;

	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUIControlBase *pChild = GetChildAt(n);
		if (NULL == pChild) continue;

		pChild->RegisterControlListen(pIDuiControlListen);
	}

	//scrollbar
	if (m_pHorizScrollBarCtrl)
	{
		m_pHorizScrollBarCtrl->RegisterControlListen(pIDuiControlListen);
	}
	if (m_pVertScrollBarCtrl)
	{
		m_pVertScrollBarCtrl->RegisterControlListen(pIDuiControlListen);
	}

	return true;
}

bool CDUIContainerCtrl::UnRegisterControlListen(IDuiControlListen *pIDuiControlListen)
{
	if (NULL == pIDuiControlListen || false == __super::UnRegisterControlListen(pIDuiControlListen)) return false;

	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUIControlBase *pChild = GetChildAt(n);
		if (NULL == pChild) continue;

		pChild->UnRegisterControlListen(pIDuiControlListen);
	}

	//scrollbar
	if (m_pHorizScrollBarCtrl)
	{
		m_pHorizScrollBarCtrl->UnRegisterControlListen(pIDuiControlListen);
	}
	if (m_pVertScrollBarCtrl)
	{
		m_pVertScrollBarCtrl->UnRegisterControlListen(pIDuiControlListen);
	}

	return true;
}

LPVOID CDUIContainerCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIContainerCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIContainerCtrl::GetDescribe() const
{
	return Dui_Ctrl_Container;
}

CDUIContainerCtrl * CDUIContainerCtrl::Clone(bool bIncludeChild, bool bRefreshCtrlID)
{
	MMInterfaceHelper(CDUIContainerCtrl, __super::Clone(bIncludeChild, bRefreshCtrlID), pContainerClone);
	if (NULL == pContainerClone) return NULL;

	//scrollbar
	if (m_pHorizScrollBarCtrl)
	{
		pContainerClone->InsertChild(m_pHorizScrollBarCtrl->Clone(bIncludeChild, bRefreshCtrlID));
	}
	if (m_pVertScrollBarCtrl)
	{
		pContainerClone->InsertChild(m_pVertScrollBarCtrl->Clone(bIncludeChild, bRefreshCtrlID));
	}

	//child
	if (false == bIncludeChild)
	{
		pContainerClone->InitComplete();

		return pContainerClone;
	}

	for (int nIndex = 0; nIndex < GetChildCount(); nIndex++)
	{
		CDUIControlBase *pChild = GetChildAt(nIndex);
		if (NULL == pChild) continue;

		pContainerClone->InsertChild(pChild->Clone(bIncludeChild, bRefreshCtrlID));
	}

	pContainerClone->InitComplete();

	return pContainerClone;
}

UINT CDUIContainerCtrl::InitCtrlID()
{
	__super::InitCtrlID();

	for (int nIndex = 0; nIndex < GetChildCount(); nIndex++)
	{
		CDUIControlBase *pChild = GetChildAt(nIndex);
		if (pChild) pChild->InitCtrlID();
	}
	if (m_pHorizScrollBarCtrl)
	{
		m_pHorizScrollBarCtrl->InitCtrlID();
	}
	if (m_pVertScrollBarCtrl)
	{
		m_pVertScrollBarCtrl->InitCtrlID();
	}

	return GetCtrlID();
}

void CDUIContainerCtrl::RefreshCtrlID(bool bSelfSingle)
{
	__super::RefreshCtrlID(bSelfSingle);

	if (bSelfSingle) return;

	for (int nIndex = 0; nIndex < GetChildCount(); nIndex++)
	{
		CDUIControlBase *pChild = GetChildAt(nIndex);
		if (pChild) pChild->RefreshCtrlID();
	}
	if (m_pHorizScrollBarCtrl)
	{
		m_pHorizScrollBarCtrl->RefreshCtrlID();
	}
	if (m_pVertScrollBarCtrl)
	{
		m_pVertScrollBarCtrl->RefreshCtrlID();
	}

	return;
}

bool CDUIContainerCtrl::SetWndOwner(CDUIWnd *pWndOwner)
{
	if (false == __super::SetWndOwner(pWndOwner)) return false;

	for (int nIndex = 0; nIndex < GetChildCount(); nIndex++)
	{
		CDUIControlBase *pChild = GetChildAt(nIndex);
		if (pChild) pChild->SetWndOwner(m_pWndOwner);
	}
	if (m_pHorizScrollBarCtrl)
	{
		m_pHorizScrollBarCtrl->SetWndOwner(m_pWndOwner);
	}
	if (m_pVertScrollBarCtrl)
	{
		m_pVertScrollBarCtrl->SetWndOwner(m_pWndOwner);
	}

	return true;
}

void CDUIContainerCtrl::SetVisible(bool bVisible)
{
	bool bVisibleOld = IsVisible();

	__super::SetVisible(bVisible);

	if (bVisibleOld != IsVisible())
	{
		RefreshChildInternVisible();
	}

	return;
}

void CDUIContainerCtrl::SetOwnerModelCtrl(CDUIControlBase *pOwnerModelCtrl)
{
	__super::SetOwnerModelCtrl(pOwnerModelCtrl);

	for (int nIndex = 0; nIndex < GetChildCount(); nIndex++)
	{
		CDUIControlBase *pChild = GetChildAt(nIndex);
		if (pChild) pChild->SetOwnerModelCtrl(pOwnerModelCtrl);
	}

	return;
}

void CDUIContainerCtrl::MovePosition(SIZE szOffset)
{
	__super::MovePosition(szOffset);

	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUIControlBase *pChild = GetChildAt(n);
		if (NULL == pChild) continue;

		pChild->MovePosition(szOffset);
	}
	if (m_pHorizScrollBarCtrl) m_pHorizScrollBarCtrl->MovePosition(szOffset);
	if (m_pVertScrollBarCtrl) m_pVertScrollBarCtrl->MovePosition(szOffset);

	return;
}

bool CDUIContainerCtrl::IsAutoCalcWidth()
{
	return m_AttributeAutoCalcWidth.GetValue();
}

void CDUIContainerCtrl::SetAutoCalcWidth(bool bAutoCalcWidth)
{
	if (bAutoCalcWidth == IsAutoCalcWidth()) return;

	m_AttributeAutoCalcWidth.SetValue(bAutoCalcWidth);

	NeedParentRefreshView();

	return;
}

bool CDUIContainerCtrl::IsAutoCalcHeight()
{
	return m_AttributeAutoCalcHeight.GetValue();
}

void CDUIContainerCtrl::SetAutoCalcHeight(bool bAutoCalcHeight)
{
	if (bAutoCalcHeight == IsAutoCalcHeight()) return;

	m_AttributeAutoCalcHeight.SetValue(bAutoCalcHeight);

	NeedParentRefreshView();

	return;
}

CDUIRect CDUIContainerCtrl::GetRangeInset()
{
	return m_AttributeRangeInset.GetValue();
}

void CDUIContainerCtrl::SetRangeInset(CDUIRect rcInset)
{
	if (rcInset == GetRangeInset()) return;

	m_AttributeRangeInset.SetValue(rcInset);

	NeedRefreshView();

	return;
}

bool CDUIContainerCtrl::SetTimer(UINT uTimerID, UINT nElapse)
{
	return __super::SetTimer(uTimerID, nElapse);
}

bool CDUIContainerCtrl::KillTimer(UINT uTimerID)
{
	return __super::KillTimer(uTimerID);
}

bool CDUIContainerCtrl::KillTimer()
{
	return __super::KillTimer();
}

bool CDUIContainerCtrl::IsAnimateDrag()
{
	return false;
}

void CDUIContainerCtrl::SetAnimateDrag(bool bAnimateDrag)
{
	return;
}

bool CDUIContainerCtrl::IsAnimateDraging()
{
	return false;
}

void CDUIContainerCtrl::OnDragMoveBegin(CDUIControlBase *pDragCtrl)
{
	return;
}

void CDUIContainerCtrl::OnDragMoving(CDUIControlBase *pDragCtrl)
{
	return;
}

void CDUIContainerCtrl::OnDragMoveEnd(CDUIControlBase *pDragCtrl)
{
	return;
}

bool CDUIContainerCtrl::IsScrollBarSpeedModel()
{
	return m_AttributeScrollBarSpeedModel.GetValue();
}

void CDUIContainerCtrl::SetScrollBarSpeedModel(bool bSpeedModel)
{
	if (bSpeedModel == IsScrollBarSpeedModel()) return;

	m_AttributeScrollBarSpeedModel.SetValue(bSpeedModel);

	return;
}

bool CDUIContainerCtrl::IsUseHorizScrollBar()
{
	return m_AttributeUseHorizScrollBar.GetValue();
}

void CDUIContainerCtrl::SetUseHorizScrollBar(bool bUseHorizScrollBar)
{
	m_AttributeUseHorizScrollBar.SetValue(bUseHorizScrollBar);

	if (false == IsUseHorizScrollBar())
	{
		//control notify
		CDUIGlobal::PerformNotifyChildRemove(this, m_pHorizScrollBarCtrl);

		MMSafeDelete(m_pHorizScrollBarCtrl);

		Invalidate();

		return;
	}

	if (NULL == m_pHorizScrollBarCtrl)
	{
		auto pImageBaseHScrollBk = CDUIGlobal::GetInstance()->GetImageResource(Name_ImageScrollBarHoriz);

		CDUIScrollBarCtrl *pScrollBarCtrl = new CDUIScrollBarCtrl;
		pScrollBarCtrl->Init();
		pScrollBarCtrl->SetProgressType(enDuiProgressType::ProgressType_Horiz);
		pScrollBarCtrl->SetPadding(0, 0, 0, 0);
		pScrollBarCtrl->SetVertAlignType(VertAlign_Bottom);
		pScrollBarCtrl->SetFixedHeight(30);

		if (pImageBaseHScrollBk)
		{
			pScrollBarCtrl->SetFixedHeight(pImageBaseHScrollBk->GetHeight(GetScale()) / 4);
			pScrollBarCtrl->SetUpDownBtnHorizFixedWidth(0);

			{
				tagDuiImageSection ImageSection;
				ImageSection.vecImageResSwitch.push_back(pImageBaseHScrollBk->GetResourceName());
				ImageSection.ImageSourceType = ImageSource_Part;
				ImageSection.bPartVert = true;
				ImageSection.cbPartAll = 4;
				ImageSection.cbPartSel = 1;
				ImageSection.rcCorner = { 5, 0, 5, 0 };
				pScrollBarCtrl->SetImageSectionGrooveHot(ImageSection);
			}
			{
				tagDuiImageSection ImageSection;
				ImageSection.vecImageResSwitch.push_back(pImageBaseHScrollBk->GetResourceName());
				ImageSection.ImageSourceType = ImageSource_Part;
				ImageSection.bPartVert = true;
				ImageSection.cbPartAll = 4;
				ImageSection.cbPartSel = 2;
				ImageSection.rcCorner = { 5, 0, 5, 0 };
				pScrollBarCtrl->SetImageSectionThumbNormal(ImageSection);

				ImageSection.cbPartSel = 2;
				pScrollBarCtrl->SetImageSectionThumbHot(ImageSection);

				ImageSection.cbPartSel = 3;
				pScrollBarCtrl->SetImageSectionThumbPushed(ImageSection);
			}
		}

		if (false == InsertChild(pScrollBarCtrl))
		{
			MMSafeDelete(pScrollBarCtrl);
		}
	}

	Invalidate();

	return;
}

bool CDUIContainerCtrl::IsUseVertScrollBar()
{
	return m_AttributeUseVertScrollBar.GetValue();
}

void CDUIContainerCtrl::SetUseVertScrollBar(bool bUseVertScrollBar)
{
	m_AttributeUseVertScrollBar.SetValue(bUseVertScrollBar);

	if (false == IsUseVertScrollBar())
	{
		//control notify
		CDUIGlobal::PerformNotifyChildRemove(this, m_pVertScrollBarCtrl);

		MMSafeDelete(m_pVertScrollBarCtrl);

		Invalidate();

		return;
	}

	if (NULL == m_pVertScrollBarCtrl)
	{
		auto pImageBaseVScrollBk = CDUIGlobal::GetInstance()->GetImageResource(Name_ImageScrollBarVert);

		CDUIScrollBarCtrl *pScrollBarCtrl = new CDUIScrollBarCtrl;
		pScrollBarCtrl->Init();
		pScrollBarCtrl->SetProgressType(enDuiProgressType::ProgressType_Vert);
		pScrollBarCtrl->SetPadding(0, 0, 0, 0);
		pScrollBarCtrl->SetHorizAlignType(HorizAlign_Right);
		pScrollBarCtrl->SetFixedWidth(30);
		pScrollBarCtrl->SetUpDownBtnVertFixedHeight(30);

		if (pImageBaseVScrollBk)
		{
			pScrollBarCtrl->SetFixedWidth(pImageBaseVScrollBk->GetWidth(GetScale()) / 4);
			pScrollBarCtrl->SetUpDownBtnVertFixedHeight(0);

			{
				tagDuiImageSection ImageSection;
				ImageSection.vecImageResSwitch.push_back(pImageBaseVScrollBk->GetResourceName());
				ImageSection.ImageSourceType = ImageSource_Part;
				ImageSection.bPartVert = false;
				ImageSection.cbPartAll = 4;
				ImageSection.cbPartSel = 1;
				ImageSection.rcCorner = { 0, 5, 0, 5 };
				pScrollBarCtrl->SetImageSectionGrooveHot(ImageSection);
			}
			{
				tagDuiImageSection ImageSection;
				ImageSection.vecImageResSwitch.push_back(pImageBaseVScrollBk->GetResourceName());
				ImageSection.ImageSourceType = ImageSource_Part;
				ImageSection.bPartVert = false;
				ImageSection.cbPartAll = 4;
				ImageSection.cbPartSel = 2;
				ImageSection.rcCorner = { 0, 5, 0, 5 };
				pScrollBarCtrl->SetImageSectionThumbNormal(ImageSection);

				ImageSection.cbPartSel = 3;
				pScrollBarCtrl->SetImageSectionThumbHot(ImageSection);

				ImageSection.cbPartSel = 4;
				pScrollBarCtrl->SetImageSectionThumbPushed(ImageSection);
			}
		}

		InsertChild(pScrollBarCtrl);
	}

	Invalidate();

	return;
}

bool CDUIContainerCtrl::IsHScrollBarPushItemRange()
{
	return m_AttributeHScrollBarPushItemRange.GetValue();
}

void CDUIContainerCtrl::SetHScrollBarPushItemRange(bool bPushItemRange)
{
	if (bPushItemRange == IsHScrollBarPushItemRange()) return;

	m_AttributeHScrollBarPushItemRange.SetValue(bPushItemRange);

	NeedRefreshView();

	return;
}

bool CDUIContainerCtrl::IsVScrollBarPushItemRange()
{
	return m_AttributeVScrollBarPushItemRange.GetValue();
}

void CDUIContainerCtrl::SetVScrollBarPushItemRange(bool bPushItemRange)
{
	if (bPushItemRange == IsVScrollBarPushItemRange()) return;

	m_AttributeVScrollBarPushItemRange.SetValue(bPushItemRange);

	NeedRefreshView();

	return;
}

SIZE CDUIContainerCtrl::GetScrollPos() const
{
	SIZE sz = {};
	if (m_pHorizScrollBarCtrl && m_pHorizScrollBarCtrl->IsVisible())
	{
		sz.cx = m_pHorizScrollBarCtrl->GetCurValue();
	}
	if (m_pVertScrollBarCtrl && m_pVertScrollBarCtrl->IsVisible())
	{
		sz.cy = m_pVertScrollBarCtrl->GetCurValue();
	}

	return sz;
}

SIZE CDUIContainerCtrl::GetScrollRange() const
{
	SIZE sz = {};
	if (m_pHorizScrollBarCtrl && m_pHorizScrollBarCtrl->IsVisible())
	{
		sz.cx = m_pHorizScrollBarCtrl->GetMaxValue();
	}
	if (m_pVertScrollBarCtrl && m_pVertScrollBarCtrl->IsVisible())
	{
		sz.cy = m_pVertScrollBarCtrl->GetMaxValue();
	}

	return sz;
}

void CDUIContainerCtrl::SetScrollPos(SIZE szPos)
{
	int cx = 0;
	int cy = 0;
	if (m_pHorizScrollBarCtrl)
	{
		m_pHorizScrollBarCtrl->SetCurValue(szPos.cx);
	}
	if (m_pVertScrollBarCtrl)
	{
		m_pVertScrollBarCtrl->SetCurValue(szPos.cy);
	}

	return;
}

void CDUIContainerCtrl::LineUp()
{
	if (NULL == m_pVertScrollBarCtrl || false == m_pVertScrollBarCtrl->IsVisible()) return;

	int cyLine = m_pVertScrollBarCtrl->GetScrollSpeed();
	cyLine = max(cyLine, Dui_ScrollSpeed_Normal);

	SIZE sz = GetScrollPos();
	sz.cy -= cyLine;
	sz.cy = max(0, sz.cy);
	SetScrollPos(sz);

	return;
}

void CDUIContainerCtrl::LineDown()
{
	if (NULL == m_pVertScrollBarCtrl || false == m_pVertScrollBarCtrl->IsVisible()) return;

	int cyLine = m_pVertScrollBarCtrl->GetScrollSpeed();
	cyLine = max(cyLine, Dui_ScrollSpeed_Normal);

	SIZE sz = GetScrollPos();
	sz.cy += cyLine;
	sz.cy = min(GetScrollRange().cy, sz.cy);
	SetScrollPos(sz);

	return;
}

void CDUIContainerCtrl::PageUp()
{
	if (NULL == m_pVertScrollBarCtrl || false == m_pVertScrollBarCtrl->IsVisible()) return;

	SIZE sz = GetScrollPos();

	int iOffset = m_rcAbsolute.GetHeight();
	if (m_pHorizScrollBarCtrl && m_pHorizScrollBarCtrl->IsVisible() && IsUseHorizScrollBar())
	{
		iOffset -= m_pHorizScrollBarCtrl->GetHeight();
	}

	sz.cy -= iOffset;
	sz.cy = max(0, sz.cy);
	SetScrollPos(sz);

	return;
}

void CDUIContainerCtrl::PageDown()
{
	if (NULL == m_pVertScrollBarCtrl || false == m_pVertScrollBarCtrl->IsVisible()) return;

	SIZE sz = GetScrollPos();

	int iOffset = m_rcAbsolute.GetHeight();
	if (m_pHorizScrollBarCtrl && m_pHorizScrollBarCtrl->IsVisible() && IsUseHorizScrollBar())
	{
		iOffset -= m_pHorizScrollBarCtrl->GetHeight();
	}

	sz.cy += iOffset;
	sz.cy = min(GetScrollRange().cy, sz.cy);
	SetScrollPos(sz);

	return;
}

void CDUIContainerCtrl::HomeUp()
{
	if (NULL == m_pVertScrollBarCtrl || false == m_pVertScrollBarCtrl->IsVisible()) return;

	SIZE sz = GetScrollPos();
	sz.cy = 0;
	SetScrollPos(sz);

	return;
}

void CDUIContainerCtrl::EndDown()
{
	if (NULL == m_pVertScrollBarCtrl || false == m_pVertScrollBarCtrl->IsVisible()) return;

	SIZE sz = GetScrollPos();
	sz.cy = GetScrollRange().cy;
	SetScrollPos(sz);

	return;
}

void CDUIContainerCtrl::LineLeft()
{
	if (NULL == m_pHorizScrollBarCtrl || false == m_pHorizScrollBarCtrl->IsVisible()) return;

	int cxLine = m_pHorizScrollBarCtrl->GetScrollSpeed();
	cxLine = max(cxLine, Dui_ScrollSpeed_Normal);

	SIZE sz = GetScrollPos();
	sz.cx -= cxLine;
	sz.cx = max(0, sz.cx);
	SetScrollPos(sz);

	return;
}
void CDUIContainerCtrl::LineRight()
{
	if (NULL == m_pHorizScrollBarCtrl || false == m_pHorizScrollBarCtrl->IsVisible()) return;

	int cxLine = m_pHorizScrollBarCtrl->GetScrollSpeed();
	cxLine = max(cxLine, Dui_ScrollSpeed_Normal);

	SIZE sz = GetScrollPos();
	sz.cx += cxLine;
	sz.cx = min(GetScrollRange().cx, sz.cx);
	SetScrollPos(sz);

	return;
}

void CDUIContainerCtrl::PageLeft()
{
	if (NULL == m_pHorizScrollBarCtrl || false == m_pHorizScrollBarCtrl->IsVisible()) return;

	SIZE sz = GetScrollPos();

	int iOffset = m_rcAbsolute.GetWidth();
	if (m_pVertScrollBarCtrl && m_pVertScrollBarCtrl->IsVisible() && IsUseVertScrollBar())
	{
		iOffset -= m_pVertScrollBarCtrl->GetWidth();
	}

	sz.cx -= iOffset;
	sz.cx = max(0, sz.cx);
	SetScrollPos(sz);

	return;
}

void CDUIContainerCtrl::PageRight()
{
	if (NULL == m_pHorizScrollBarCtrl || false == m_pHorizScrollBarCtrl->IsVisible()) return;

	SIZE sz = GetScrollPos();

	int iOffset = m_rcAbsolute.GetWidth();
	if (m_pVertScrollBarCtrl && m_pVertScrollBarCtrl->IsVisible() && IsUseVertScrollBar())
	{
		iOffset -= m_pVertScrollBarCtrl->GetWidth();
	}

	sz.cx += iOffset;
	sz.cx = min(GetScrollRange().cx, sz.cx);
	SetScrollPos(sz);

	return;
}

void CDUIContainerCtrl::HomeLeft()
{
	if (NULL == m_pHorizScrollBarCtrl || false == m_pHorizScrollBarCtrl->IsVisible()) return;

	SIZE sz = GetScrollPos();
	sz.cx = 0;
	SetScrollPos(sz);

	return;
}

void CDUIContainerCtrl::EndRight()
{
	if (NULL == m_pHorizScrollBarCtrl || false == m_pHorizScrollBarCtrl->IsVisible()) return;

	SIZE sz = GetScrollPos();
	sz.cx = GetScrollRange().cx;
	SetScrollPos(sz);

	return;
}

void CDUIContainerCtrl::EnableScrollBar(bool bEnableHorizontal, bool bEnableVertical)
{
	SetUseHorizScrollBar(bEnableHorizontal);
	SetUseVertScrollBar(bEnableVertical);

	return;
}

CDUIScrollBarCtrl * CDUIContainerCtrl::GetHorizScrollBar() const
{
	return m_pHorizScrollBarCtrl;
}

CDUIScrollBarCtrl * CDUIContainerCtrl::GetVertScrollBar() const
{
	return m_pVertScrollBarCtrl;
}

void CDUIContainerCtrl::EnsureVisible(int nIndex, bool bCenter)
{
	CDUIControlBase *pChild = GetChildAt(nIndex);
	if (NULL == pChild) return;

	//target
	CDUIRect rcChild = pChild->GetAbsoluteRect();
	CDUISize szScrollPos = GetScrollPos();
	CDUISize szScrollRange = GetScrollRange();
	CDUIRect rcItemRange = GetLayoutRangeOfItem();
	if (bCenter)
	{
		rcItemRange.top = rcItemRange.top + rcItemRange.GetHeight() / 2 - rcChild.GetHeight() / 2;
		rcItemRange.bottom = rcItemRange.top + rcChild.GetHeight();
	}

	//scroll pos
	if (rcChild.left < rcItemRange.left) szScrollPos.cx -= rcItemRange.left - rcChild.left;
	else if (rcChild.right > rcItemRange.right) szScrollPos.cx -= rcItemRange.right - rcChild.right;
	if (rcChild.top < rcItemRange.top) szScrollPos.cy -= rcItemRange.top - rcChild.top;
	else if (rcChild.bottom > rcItemRange.bottom) szScrollPos.cy -= rcItemRange.bottom - rcChild.bottom;
	SetScrollPos(szScrollPos);

	return;
}

int CDUIContainerCtrl::GetChildCount() const
{
	return m_vecChilds.size();
}

bool CDUIContainerCtrl::VerifyChild(CDUIControlBase *pControl)
{
	if (NULL == pControl) return false;

	if (pControl == m_pHorizScrollBarCtrl || pControl == m_pVertScrollBarCtrl) return true;

	for (int nIndex = 0; nIndex < GetChildCount(); nIndex++)
	{
		CDUIControlBase *pChild = GetChildAt(nIndex);
		if (NULL == pChild) continue;

		if (pControl == pChild) return true;

		MMInterfaceHelper(CDUIContainerCtrl, pChild, pContainer);
		if (NULL == pContainer) continue;

		if (true == pContainer->VerifyChild(pControl)) return true;
	}

	return false;
}

bool CDUIContainerCtrl::InsertChild(CDUIControlBase *pChild, int nPos)
{
	if (NULL == pChild) return false;

	//scroll bar
	if ((NULL == m_pHorizScrollBarCtrl || NULL == m_pVertScrollBarCtrl)
		&& (IsInitComplete() || GetChildCount() <= 0)
		&& MMInterfaceHelper(CDUIScrollBarCtrl, pChild))
	{
		CDUIScrollBarCtrl *pScrollBarCtrl = static_cast<CDUIScrollBarCtrl*>(pChild);
		if (NULL == m_pHorizScrollBarCtrl && ProgressType_Horiz == pScrollBarCtrl->GetProgressType())
		{
			m_pHorizScrollBarCtrl = pScrollBarCtrl;
			m_pHorizScrollBarCtrl->SetOwnerCtrl(this);
		}
		else if (NULL == m_pVertScrollBarCtrl && ProgressType_Vert == pScrollBarCtrl->GetProgressType())
		{
			m_pVertScrollBarCtrl = pScrollBarCtrl;
			m_pVertScrollBarCtrl->SetOwnerCtrl(this);
		}
	}
	//child
	else
	{
		if (find(m_vecChilds.begin(), m_vecChilds.end(), pChild) != m_vecChilds.end()) return true;

		auto InsertIt = -1 == nPos ? m_vecChilds.end() : m_vecChilds.begin() + nPos;
		nPos >= m_vecChilds.size() ? InsertIt = m_vecChilds.end() : InsertIt;
		m_vecChilds.insert(InsertIt, pChild);
	}

	//info
	pChild->SetParent(this);
	pChild->SetOwnerModelCtrl(GetOwnerModelCtrl());

	if (m_pWndOwner)
	{
		m_pWndOwner->InitControls(pChild);
	}
	if (IsVisible())
	{
		NeedRefreshView();
	}
	else if (IsInitComplete())
	{
		pChild->SetInternVisible(false);
	}

	//control notify
	CDUIGlobal::PerformNotifyChildAdd(this, pChild);

	return true;
}

CDUIControlBase * CDUIContainerCtrl::DetachChild(int nIndex)
{
	CDUIControlBase *pChild = GetChildAt(nIndex);
	if (pChild)
	{
		//control notify
		CDUIGlobal::PerformNotifyChildRemove(this, pChild);

		m_vecChilds.erase(m_vecChilds.begin() + nIndex);

		pChild->SetOwnerModelCtrl(NULL);
		pChild->ReapControl();
		pChild->SetWndOwner(NULL);

		NeedRefreshView();

		return pChild;
	}

	return NULL;
}

CDUIControlBase * CDUIContainerCtrl::DetachChild(CDUIControlBase *pControl)
{
	return DetachChild(GetChildIndex(pControl));
}

int CDUIContainerCtrl::GetChildIndex(CDUIControlBase *pChild) const
{
	if (NULL == pChild) return -1;

	for (int nIndex = 0; nIndex < GetChildCount(); nIndex++)
	{
		if (GetChildAt(nIndex) == pChild) return nIndex;
	}

	return -1;
}

bool CDUIContainerCtrl::SetChildIndex(CDUIControlBase *pControl, int nIndex)
{
	int nIndexChild = GetChildIndex(pControl);
	if (-1 == nIndexChild) return false;
	if (nIndexChild == nIndex) return true;

	m_vecChilds.erase(m_vecChilds.begin() + nIndexChild);

	auto InsertIt = -1 == nIndex ? m_vecChilds.end() : m_vecChilds.begin() + nIndex;
	nIndex >= m_vecChilds.size() ? InsertIt = m_vecChilds.end() : InsertIt;
	m_vecChilds.insert(InsertIt, pControl);

	NeedRefreshView();

	return true;
}

bool CDUIContainerCtrl::SwapChild(CDUIControlBase *pFirstCtrl, CDUIControlBase *pNextCtrl, bool bNeedRefreshView)
{
	if (NULL == pFirstCtrl || NULL == pNextCtrl) return false;

	return SwapChild(GetChildIndex(pFirstCtrl), GetChildIndex(pNextCtrl), bNeedRefreshView);
}

bool CDUIContainerCtrl::SwapChild(int nIndexFirst, int nIndexNext, bool bNeedRefreshView)
{
	if (nIndexFirst == nIndexNext) return false;

	CDUIControlBase *pFirstCtrl = GetChildAt(nIndexFirst);
	CDUIControlBase *pNextCtrl = GetChildAt(nIndexNext);
	if (NULL == pFirstCtrl || NULL == pNextCtrl) return false;

	std::swap(m_vecChilds[nIndexFirst], m_vecChilds[nIndexNext]);

	if (bNeedRefreshView)
	{
		NeedRefreshView();
	}

	return true;
}

CDUIControlBase * CDUIContainerCtrl::GetChildAt(int nIndex) const
{
	if (0 <= nIndex && nIndex < m_vecChilds.size()) return m_vecChilds[nIndex];

	return NULL;
}

int CDUIContainerCtrl::GetChildPaddingH()
{
	return m_AttributeChildPaddingH.GetValue();
}

int CDUIContainerCtrl::GetChildPaddingV()
{
	return m_AttributeChildPaddingV.GetValue();
}

CDUISize CDUIContainerCtrl::GetChildPadding()
{
	CDUISize sz = {};
	sz.cx = GetChildPaddingH();
	sz.cy = GetChildPaddingV();

	return sz;
}

void CDUIContainerCtrl::SetChildPadding(int nChildPaddingH, int nChildPaddingV)
{
	if (GetChildPadding() == SIZE{ nChildPaddingH, nChildPaddingV }) return;

	m_AttributeChildPaddingH.SetValue(nChildPaddingH);
	m_AttributeChildPaddingV.SetValue(nChildPaddingV);

	NeedRefreshView();

	return;
}

bool CDUIContainerCtrl::IsAutoCalcChildPaddingH()
{
	return m_AttributeAutoCalcChildPaddingH.GetValue();
}

void CDUIContainerCtrl::SetAutoCalcChildPaddingH(bool bAuto)
{
	if (bAuto == IsAutoCalcChildPaddingH()) return;

	m_AttributeAutoCalcChildPaddingH.SetValue(bAuto);

	NeedRefreshView();

	return;
}

bool CDUIContainerCtrl::IsAutoCalcChildPaddingV()
{
	return m_AttributeAutoCalcChildPaddingV.GetValue();
}

void CDUIContainerCtrl::SetAutoCalcChildPaddingV(bool bAuto)
{
	if (bAuto == IsAutoCalcChildPaddingV()) return;

	m_AttributeAutoCalcChildPaddingV.SetValue(bAuto);

	NeedRefreshView();

	return;
}

bool CDUIContainerCtrl::Remove(CDUIControlBase *pControl)
{
	return CDUIContainerCtrl::RemoveAt(GetChildIndex(pControl));
}

bool CDUIContainerCtrl::RemoveAt(int nIndex)
{
	CDUIControlBase *pControl = GetChildAt(nIndex);
	if (NULL == pControl) return false;

	//control notify
	CDUIGlobal::PerformNotifyChildRemove(this, pControl);

	//delete
	m_vecChilds.erase(m_vecChilds.begin() + nIndex);

	if (m_pWndOwner)
	{
		m_pWndOwner->DelayDelete(pControl);
	}
	else
	{
		MMSafeDelete(pControl);
	}

	NeedRefreshView();

	return true;
}

void CDUIContainerCtrl::RemoveAll()
{
	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUIControlBase *pControl = GetChildAt(n);
		if (NULL == pControl) continue;

		//control notify
		CDUIGlobal::PerformNotifyChildRemove(this, pControl);

		if (m_pWndOwner)
		{
			m_pWndOwner->DelayDelete(pControl);
		}
		else
		{
			MMSafeDelete(pControl);
		}
	}

	m_vecChilds.clear();
	SetScrollPos({});
	NeedRefreshView();

	return;
}

int CDUIContainerCtrl::TranslateIndex(CDUIPoint pt)
{
	int nTargetIndex = -1;
	for (int nIndex = GetChildCount() - 1; nIndex >= 0; nIndex--)
	{
		CDUIControlBase *pChild = GetChildAt(nIndex);
		if (NULL == pChild || pChild->IsFloat()) continue;

		CDUIRect rcChild = pChild->GetAbsoluteRect();
		if (rcChild.PtInRect(pt)) return nIndex;
	}

	return nTargetIndex;
}

CDUIControlBase * CDUIContainerCtrl::FindSubControl(UINT uCtrlID)
{
	return FindControl(CDUIWnd::__FindControlFromID, (LPVOID)&uCtrlID, DuiFind_All);
}

CDUIControlBase * CDUIContainerCtrl::FindSubControlThisView(UINT uCtrlID)
{
	return FindControl(CDUIWnd::__FindControlFromID, (LPVOID)&uCtrlID, DuiFind_All | DuiFind_ThisView);
}

int CDUIContainerCtrl::FindNextIndex(int nIndexCur, bool bPositive, UINT uFlags, int nNextCount) const
{
	if (bPositive)
	{
		for (int nIndex = nIndexCur + 1; nIndex < GetChildCount(); nIndex++)
		{
			CDUIControlBase *pChild = GetChildAt(nIndex);
			if (NULL == pChild
				|| ((DuiFind_Visible & uFlags) && false == pChild->IsVisible())
				|| ((DuiFind_Enabled & uFlags) && false == pChild->IsEnabled())) continue;
			if (--nNextCount <= 0)
			{
				return nIndex;
			}
		}

		return -1;
	}

	for (int nIndex = nIndexCur - 1; nIndex >= 0; nIndex--)
	{
		CDUIControlBase *pChild = GetChildAt(nIndex);
		if (NULL == pChild
			|| ((DuiFind_Visible & uFlags) && false == pChild->IsVisible())
			|| ((DuiFind_Enabled & uFlags) && false == pChild->IsEnabled())) continue;
		if (--nNextCount <= 0)
		{
			return nIndex;
		}
	}

	return -1;
}

void CDUIContainerCtrl::RefreshView()
{
	RefreshScrollBar();

	CDUISize szScrollPos = GetScrollPos();
	CDUIRect rcRange = GetLayoutRangeOfItem();

	rcRange.Offset(-szScrollPos.cx, -szScrollPos.cy);
	CDUIRect rcMax = rcRange;

	//child
	for (int nIndex = 0; nIndex < GetChildCount(); nIndex++)
	{
		CDUIControlBase *pChild = GetChildAt(nIndex);
		if (NULL == pChild || false == pChild->IsVisible()) continue;

		pChild->OnDuiSize(rcRange);

		CDUIRect rcChild = pChild->GetAbsoluteRect();
		rcMax.left = min(rcMax.left, rcChild.left);
		rcMax.top = min(rcMax.top, rcChild.top);
		rcMax.right = max(rcMax.right, rcChild.right);
		rcMax.bottom = max(rcMax.bottom, rcChild.bottom);
	}

	UpdateScrollBar(CDUISize(rcMax.GetWidth() - rcRange.GetWidth(), rcMax.GetHeight() - rcRange.GetHeight()));

	__super::RefreshView();

	return;
}

bool CDUIContainerCtrl::DoPaint(HDC hDC, bool bGenerateBmp)
{
	if (false == __super::DoPaint(hDC, bGenerateBmp)) return false;

	for (int nIndex = 0; nIndex < GetChildCount(); nIndex++)
	{
		CDUIControlBase *pChild = GetChildAt(nIndex);
		if (NULL == pChild || false == pChild->IsVisible()) continue;

		pChild->OnDraw(hDC, m_rcPaint, bGenerateBmp);
	}
	if (m_pHorizScrollBarCtrl && m_pHorizScrollBarCtrl->IsVisible())
	{
		m_pHorizScrollBarCtrl->OnDraw(hDC, m_rcPaint, bGenerateBmp);
	}
	if (m_pVertScrollBarCtrl && m_pVertScrollBarCtrl->IsVisible())
	{
		m_pVertScrollBarCtrl->OnDraw(hDC, m_rcPaint, bGenerateBmp);
	}

	return true;
}

void CDUIContainerCtrl::InitProperty()
{
	__super::InitProperty();

	DuiCreateGroupAttribute(m_AttributeGroupScroll, _T("Scroll"));
	DuiCreateAttribute(m_AttributeScrollBarSpeedModel, _T("ScrollBarSpeedModel"), _T(""), m_AttributeGroupScroll);
	DuiCreateAttribute(m_AttributeUseHorizScrollBar, _T("UseHorizScrollBar"), _T(""), m_AttributeGroupScroll);
	DuiCreateAttribute(m_AttributeUseVertScrollBar, _T("UseVertScrollBar"), _T(""), m_AttributeGroupScroll);
	DuiCreateAttribute(m_AttributeHScrollBarPushItemRange, _T("HScrollBarPushItemRange"), _T(""), m_AttributeGroupScroll);
	DuiCreateAttribute(m_AttributeVScrollBarPushItemRange, _T("VScrollBarPushItemRange"), _T(""), m_AttributeGroupScroll);

	DuiCreateGroupAttribute(m_AttributeGroupChild, _T("ChildPadding"));
	DuiCreateAttribute(m_AttributeRangeInset, _T("RangeInset"), _T(""), m_AttributeGroupChild);

	return;
}

void CDUIContainerCtrl::InitComplete()
{
	__super::InitComplete();

	EnableScrollBar(IsUseHorizScrollBar(), IsUseVertScrollBar());

	return;
}

bool CDUIContainerCtrl::SaveAttribute(tinyxml2::XMLElement *pNode, bool bIncludeChild)
{
	if (NULL == pNode || NULL == pNode->GetDocument()) return false;

	__super::SaveAttribute(pNode);

	if (m_pHorizScrollBarCtrl)
	{
		tinyxml2::XMLElement *pNodeChild = pNode->GetDocument()->NewElement((LPCSTR)CT2CA(m_pHorizScrollBarCtrl->GetClass()));
		if (NULL == pNodeChild) { ASSERT(false); }
		m_pHorizScrollBarCtrl->SaveAttribute(pNodeChild);

		pNode->LinkEndChild(pNodeChild);
	}
	if (m_pVertScrollBarCtrl)
	{
		tinyxml2::XMLElement *pNodeChild = pNode->GetDocument()->NewElement((LPCSTR)CT2CA(m_pVertScrollBarCtrl->GetClass()));
		if (NULL == pNodeChild) { ASSERT(false); }
		m_pVertScrollBarCtrl->SaveAttribute(pNodeChild);

		pNode->LinkEndChild(pNodeChild);
	}

	//child
	if (false == bIncludeChild) return true;

	for (int nIndex = 0; nIndex < GetChildCount(); nIndex++)
	{
		CDUIControlBase *pControl = GetChildAt(nIndex);
		if (NULL == pControl) continue;

		tinyxml2::XMLElement *pNodeChild = pNode->GetDocument()->NewElement((LPCSTR)CT2CA(pControl->GetClass()));
		if (NULL == pNodeChild) { ASSERT(false); continue; }

		pControl->SaveAttribute(pNodeChild);

		pNode->LinkEndChild(pNodeChild);
	}

	return true;
}

void CDUIContainerCtrl::OnDuiSize(CDUIRect rcParentAbs)
{
	__super::OnDuiSize(rcParentAbs);

	return;
}

bool CDUIContainerCtrl::OnDuiMouseWheel(const CDUIPoint &pt, const DuiMessage &Msg)
{
	//direction
	int nWheelDelta = (int)(short)HIWORD(Msg.wParam);
	int nWheelCount = abs(nWheelDelta / WHEEL_DELTA);
	bool bPositive = nWheelDelta > 0;

	if (bPositive
		&& GetVertScrollBar()
		&& GetVertScrollBar()->IsVisible()
		&& GetVertScrollBar()->GetCurValue() > 0)
	{
		DuiMessage MsgCopy = Msg;
		MsgCopy.pMsgCtrl = GetVertScrollBar();
		MsgCopy.pMsgCtrl->OnDuiMouseWheel(pt, Msg);
	}
	else if (false == bPositive
		&& GetVertScrollBar()
		&& GetVertScrollBar()->IsVisible()
		&& GetVertScrollBar()->GetCurValue() < GetVertScrollBar()->GetMaxValue())
	{
		DuiMessage MsgCopy = Msg;
		MsgCopy.pMsgCtrl = GetVertScrollBar();
		MsgCopy.pMsgCtrl->OnDuiMouseWheel(pt, Msg);
	}
	else if (bPositive
		&& GetHorizScrollBar()
		&& GetHorizScrollBar()->IsVisible()
		&& GetHorizScrollBar()->GetCurValue() > 0)
	{
		DuiMessage MsgCopy = Msg;
		MsgCopy.pMsgCtrl = GetHorizScrollBar();
		MsgCopy.pMsgCtrl->OnDuiMouseWheel(pt, Msg);
	}
	else if (false == bPositive
		&& GetHorizScrollBar()
		&& GetHorizScrollBar()->IsVisible()
		&& GetHorizScrollBar()->GetCurValue() < GetHorizScrollBar()->GetMaxValue())
	{
		DuiMessage MsgCopy = Msg;
		MsgCopy.pMsgCtrl = GetHorizScrollBar();
		MsgCopy.pMsgCtrl->OnDuiMouseWheel(pt, Msg);
	}
	else
	{
		return __super::OnDuiMouseWheel(pt, Msg);
	}

	__super::OnDuiMouseWheel(pt, Msg);

	return true;
}

void CDUIContainerCtrl::RefreshScrollBar()
{
	CDUIRect rcRange = GetLayoutRangeOfScrollBar();

	//scroll bar
	if (m_pHorizScrollBarCtrl && m_pHorizScrollBarCtrl->IsVisible())
	{
		if (m_pVertScrollBarCtrl && m_pVertScrollBarCtrl->IsVisible())
		{
			rcRange.right -= m_pVertScrollBarCtrl->GetWidth();
		}

		m_pHorizScrollBarCtrl->OnDuiSize(rcRange);
		rcRange.bottom -= m_pHorizScrollBarCtrl->GetHeight();
	}
	if (m_pVertScrollBarCtrl && m_pVertScrollBarCtrl->IsVisible())
	{
		rcRange.right = m_rcAbsolute.right;

		m_pVertScrollBarCtrl->OnDuiSize(rcRange);
		rcRange.right -= m_pVertScrollBarCtrl->GetWidth();
	}

	return;
}

void CDUIContainerCtrl::RefreshChildInternVisible(bool bTraversal)
{
	for (int nIndex = 0; nIndex < GetChildCount(); nIndex++)
	{
		CDUIControlBase *pControl = GetChildAt(nIndex);
		if (NULL == pControl) continue;

		pControl->SetInternVisible(IsVisible(), bTraversal);
	}
	if (m_pHorizScrollBarCtrl)
	{
		m_pHorizScrollBarCtrl->SetInternVisible(IsVisible(), bTraversal);
	}
	if (m_pVertScrollBarCtrl)
	{
		m_pVertScrollBarCtrl->SetInternVisible(IsVisible(), bTraversal);
	}

	return;
}

CDUIRect CDUIContainerCtrl::GetLayoutRangeOfScrollBar()
{
	return GetAbsoluteRect();
}

CDUIRect CDUIContainerCtrl::GetLayoutRangeOfItem()
{
	CDUIRect rcRange = GetAbsoluteRect();

	//scroll bar
	if (IsHScrollBarPushItemRange() && m_pHorizScrollBarCtrl && m_pHorizScrollBarCtrl->IsVisible())
	{
		rcRange.bottom -= m_pHorizScrollBarCtrl->GetHeight();
	}
	if (IsVScrollBarPushItemRange() && m_pVertScrollBarCtrl && m_pVertScrollBarCtrl->IsVisible())
	{
		rcRange.right -= m_pVertScrollBarCtrl->GetWidth();
	}

	rcRange.left += GetRangeInset().left;
	rcRange.top += GetRangeInset().top;
	rcRange.right -= GetRangeInset().right;
	rcRange.bottom -= GetRangeInset().bottom;
	rcRange.CheckRect();

	return rcRange;
}

void CDUIContainerCtrl::UpdateScrollBar(CDUISize szRange)
{
	if (IsVScrollBarPushItemRange()
		&& m_pVertScrollBarCtrl
		&& m_pVertScrollBarCtrl->IsVisible()
		&& szRange.cx <= m_pVertScrollBarCtrl->GetWidth()
		&& IsHScrollBarPushItemRange()
		&& m_pHorizScrollBarCtrl
		&& m_pHorizScrollBarCtrl->IsVisible()
		&& szRange.cy <= m_pHorizScrollBarCtrl->GetHeight())
	{
		szRange.cx = 0;
		szRange.cy = 0;
	}
	if (IsUseHorizScrollBar() && m_pHorizScrollBarCtrl)
	{
		m_pHorizScrollBarCtrl->SetMaxValue(szRange.cx);
	}
	if (IsUseVertScrollBar() && m_pVertScrollBarCtrl)
	{
		m_pVertScrollBarCtrl->SetMaxValue(szRange.cy);
	}

	return;
}

void CDUIContainerCtrl::ScrollChilds(CDUISize szScroll)
{
	for (int nIndex = 0; nIndex < GetChildCount(); nIndex++)
	{
		CDUIControlBase* pChild = GetChildAt(nIndex);
		if (pChild == NULL || false == pChild->IsVisible()) continue;

		CDUIRect rcModal = pChild->GetModalParentRect();
		rcModal.Offset(CDUISize(-szScroll.cx, -szScroll.cy));
		pChild->OnDuiSize(rcModal);
	}
	if (m_pWndOwner)
	{
		m_pWndOwner->SendNotify(this, DuiNotify_Scroll);
	}

	Invalidate();

	return;
}

void CDUIContainerCtrl::SetInternVisible(bool bVisible, bool bTraversal)
{
	bool bVisibleOld = IsVisible();

	__super::SetInternVisible(bVisible, bTraversal);

	if (bVisibleOld != IsVisible() || bTraversal)
	{
		RefreshChildInternVisible(bTraversal);
	}

	return;
}

CDUIControlBase * CDUIContainerCtrl::FindControl(FindControlProc Proc, LPVOID pData, UINT uFlags)
{
	if ((uFlags & DuiFind_Visible) && false == IsVisible()) return NULL;
	if ((uFlags & DuiFind_Enabled) && false == IsEnabled()) return NULL;
	if ((uFlags & DuiFind_HitTest) && (false == IsEnabled() || false == ::PtInRect(&m_rcAbsolute, *static_cast<LPPOINT>(pData)))) return NULL;

	CDUIControlBase *pResult = NULL;

	//mefirst
	if ((uFlags & DuiFind_MeFirst))
	{
		pResult = __super::FindControl(Proc, pData, uFlags);
	}
	if (pResult) return pResult;

	//scrollbar
	if (m_pVertScrollBarCtrl)
	{
		pResult = m_pVertScrollBarCtrl->FindControl(Proc, pData, uFlags);

		if (pResult && 0 == (uFlags & DuiFind_UpdateTest)) return pResult;
	}
	if (m_pHorizScrollBarCtrl)
	{
		pResult = m_pHorizScrollBarCtrl->FindControl(Proc, pData, uFlags);

		if (pResult && 0 == (uFlags & DuiFind_UpdateTest)) return pResult;
	}

	if ((uFlags & DuiFind_TopFirst))
	{
		for (int it = GetChildCount() - 1; it >= 0; it--)
		{
			CDUIControlBase *pChild = GetChildAt(it);
			if (NULL == pChild) continue;

			if (DuiFind_ThisView & uFlags)
			{
				pResult = pChild->CDUIControlBase::FindControl(Proc, pData, uFlags);
			}
			else
			{
				pResult = pChild->FindControl(Proc, pData, uFlags);
			}
			if (pResult && 0 == (uFlags & DuiFind_UpdateTest))
			{
				return pResult;
			}
		}
	}
	else
	{
		for (int it = 0; it < GetChildCount(); it++)
		{
			CDUIControlBase *pChild = GetChildAt(it);
			if (NULL == pChild) continue;

			if (DuiFind_ThisView & uFlags)
			{
				pResult = pChild->CDUIControlBase::FindControl(Proc, pData, uFlags);
			}
			else
			{
				pResult = pChild->FindControl(Proc, pData, uFlags);
			}
			if (pResult && 0 == (uFlags & DuiFind_UpdateTest))
			{
				return pResult;
			}
		}
	}

	return __super::FindControl(Proc, pData, uFlags);
}

void CDUIContainerCtrl::ReapControl()
{
	__super::ReapControl();

	//child
	for (int nIndex = 0; nIndex < GetChildCount(); nIndex++)
	{
		CDUIControlBase *pChild = GetChildAt(nIndex);
		if (pChild) pChild->ReapControl();
	}
	if (m_pHorizScrollBarCtrl)
	{
		m_pHorizScrollBarCtrl->ReapControl();
	}
	if (m_pVertScrollBarCtrl)
	{
		m_pVertScrollBarCtrl->ReapControl();
	}

	return;
}