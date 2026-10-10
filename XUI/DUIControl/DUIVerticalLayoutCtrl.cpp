#include "StdAfx.h"
#include "DUIVerticalLayoutCtrl.h"

DuiImplement_CreateControl(CDUIVerticalLayoutCtrl)
MMImplement_ClassName(CDUIVerticalLayoutCtrl)

CDUIVerticalLayoutCtrl::CDUIVerticalLayoutCtrl(void)
{
}

CDUIVerticalLayoutCtrl::~CDUIVerticalLayoutCtrl(void)
{
	if (m_pWndOwner)
	{
		m_pWndOwner->RemoveINotify(m_pAnimateDrag);
	}

	MMSafeDelete(m_pAnimateDrag);

	return;
}

LPVOID CDUIVerticalLayoutCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIVerticalLayoutCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIVerticalLayoutCtrl::GetDescribe() const
{
	return Dui_Ctrl_VerticalLayout;
}

UINT CDUIVerticalLayoutCtrl::GetControlFlags()
{
	return __super::GetControlFlags();
}

bool CDUIVerticalLayoutCtrl::IsAnimateDrag()
{
	return m_AttributeAnimateDrag.GetValue();
}

void CDUIVerticalLayoutCtrl::SetAnimateDrag(bool bAnimateDrag)
{
	if (bAnimateDrag == IsAnimateDrag()) return;

	m_AttributeAnimateDrag.SetValue(bAnimateDrag);

	if (IsAnimateDrag() && NULL == m_pAnimateDrag)
	{
		m_pAnimateDrag = new CDUIAnimationDrag(this, DragType_V);
	}
	if (m_pWndOwner)
	{
		if (IsAnimateDrag())
		{
			m_pWndOwner->AddINotify(m_pAnimateDrag);
		}
		else
		{
			m_pWndOwner->RemoveINotify(m_pAnimateDrag);
		}
	}

	return;
}

bool CDUIVerticalLayoutCtrl::IsAnimateDraging()
{
	return m_pAnimateDrag && m_pAnimateDrag->IsAnimateDraging();
}

void CDUIVerticalLayoutCtrl::OnDragMoveBegin(CDUIControlBase *pDragCtrl)
{
	if (m_pAnimateDrag) 
	{
		m_pAnimateDrag->OnDragMoveBegin(pDragCtrl);
	}

	return;
}

void CDUIVerticalLayoutCtrl::OnDragMoving(CDUIControlBase *pDragCtrl)
{
	if (m_pAnimateDrag)
	{
		m_pAnimateDrag->OnDragMoving(pDragCtrl);
	}

	return;
}

void CDUIVerticalLayoutCtrl::OnDragMoveEnd(CDUIControlBase *pDragCtrl)
{
	if (m_pAnimateDrag)
	{
		m_pAnimateDrag->OnDragMoveEnd(pDragCtrl);
	}

	return;
}

int CDUIVerticalLayoutCtrl::TranslateIndex(CDUIPoint pt)
{
	int nTargetIndex = -1;
	for (int nIndex = 0; nIndex < GetChildCount(); nIndex++)
	{
		CDUIControlBase *pChild = GetChildAt(nIndex);
		if (NULL == pChild || pChild->IsFloat()) continue;

		CDUIRect rcChild = pChild->GetAbsoluteRect();
		if (pt.x <= rcChild.right && pt.y <= rcChild.bottom) return nIndex;
	}

	return nTargetIndex;
}

void CDUIVerticalLayoutCtrl::RefreshView()
{
	//animate draging
	if (IsAnimateDraging()) return;

	//refresh
	RefreshScrollBar();

	CDUISize szScrollPos = GetScrollPos();
	CDUIRect rcRangeLayout = GetLayoutRangeOfItem();
	CDUIRect rcRangeView = rcRangeLayout;
	
	rcRangeLayout.Offset(-szScrollPos.cx, -szScrollPos.cy);
	CDUIRect rcMax = rcRangeLayout;

	//variant
	int nChildPaddingV = GetChildPaddingV();
	int nFixedHeightTotal = 0;
	VecDuiControlBase vecControlTop, vecControlBottom;
	std::unordered_map<CDUIControlBase*, int> mapDynamicCtrl;

	//adjust
	for (int nIndex = 0; nIndex < GetChildCount(); nIndex++)
	{
		CDUIControlBase *pChild = GetChildAt(nIndex);
		if (NULL == pChild || false == pChild->IsVisible()) continue;

		//float
		if (pChild->IsFloat())
		{
			pChild->OnDuiSize(rcRangeLayout);

			continue;
		}

		//bottom
		enDuiVertAlignType VertAlignType = pChild->GetVertAlignType();
		if (VertAlign_Bottom == VertAlignType)
		{
			vecControlBottom.insert(vecControlBottom.begin(), pChild);
		}
		//top
		else
		{
			vecControlTop.push_back(pChild);
		}

		CDUIRect rcPadding = pChild->GetPadding();
		nFixedHeightTotal += rcPadding.top;
		nFixedHeightTotal += rcPadding.bottom;

		if ((VertAlign_Bottom == VertAlignType && vecControlBottom.size() > 1)
			|| (VertAlign_Bottom != VertAlignType && vecControlTop.size() > 1))
		{
			nFixedHeightTotal += nChildPaddingV;
		}

		int nFixedHeight = pChild->GetFixedHeight();
		nFixedHeight = max(pChild->GetMinHeight(), nFixedHeight);
		nFixedHeight = min(pChild->GetMaxHeight(), nFixedHeight);
		if (nFixedHeight <= 0)
		{
			mapDynamicCtrl[pChild] = 0;
			continue;
		}

		nFixedHeightTotal += nFixedHeight;
	}

	//dynamicheight
	int nDynamicHeight = 0;
	int nDynamicSurplus = rcRangeLayout.GetHeight() - nFixedHeightTotal;
	nDynamicSurplus = max(nDynamicSurplus, 0);
	if (mapDynamicCtrl.size() > 0)
	{
		int nDynamicHeightTotal = rcRangeLayout.GetHeight() - nFixedHeightTotal;
		nDynamicHeightTotal = max(nDynamicHeightTotal, 0);
		nDynamicHeight = nDynamicHeightTotal / mapDynamicCtrl.size();
		nDynamicSurplus = nDynamicHeightTotal % mapDynamicCtrl.size();

		if (nDynamicHeight > 0)
		{
			for (auto &DynamicCtrl : mapDynamicCtrl)
			{
				CDUIControlBase *pChild = DynamicCtrl.first;
				if (NULL == pChild) continue;

				DynamicCtrl.second = min(nDynamicHeight, pChild->GetMaxHeight());

				if (nDynamicHeight > pChild->GetMaxHeight())
				{
					nDynamicSurplus += nDynamicHeight - pChild->GetMaxHeight();
				}
			}
			while (nDynamicSurplus > 0)
			{
				int nDynamicSurplusOld = nDynamicSurplus;

				for (auto &DynamicCtrl : mapDynamicCtrl)
				{
					CDUIControlBase *pChild = DynamicCtrl.first;
					if (NULL == pChild || pChild->GetMaxHeight() <= DynamicCtrl.second) continue;

					DynamicCtrl.second++;
					nDynamicSurplus--;

					if (nDynamicSurplus <= 0) break;
				}
				if (nDynamicSurplusOld == nDynamicSurplus)
				{
					break;
				}
			}
		}
	}

	//auto child padding
	int nDynamicChildPaddingV = 0;
	if (IsAutoCalcChildPaddingV() && vecControlTop.size() + vecControlBottom.size() - 1 > 0)
	{
		nDynamicChildPaddingV = nDynamicSurplus / (vecControlTop.size() + vecControlBottom.size() - 1);
		nDynamicSurplus = nDynamicSurplus % (vecControlTop.size() + vecControlBottom.size() - 1);
	}

	//adjust layout
	if (nFixedHeightTotal > rcRangeLayout.GetHeight())
	{
		rcRangeLayout.bottom += (nFixedHeightTotal - rcRangeLayout.GetHeight());
	}

	//layout top
	CDUIRect rcModal;
	int nTileY = rcRangeLayout.top;
	for (int nIndex = 0; nIndex < vecControlTop.size(); nIndex++)
	{
		CDUIControlBase *pChild = vecControlTop[nIndex];
		if (NULL == pChild) continue;

		if (nIndex > 0)
		{
			nTileY += nChildPaddingV;
		}
		if (IsAutoCalcChildPaddingV() && nTileY != rcRangeLayout.top)
		{
			nTileY += nDynamicChildPaddingV;

			if (nDynamicSurplus > 0)
			{
				nTileY++;
				nDynamicSurplus--;
			}
		}

		int nFixedHeight = pChild->GetFixedHeight();
		nFixedHeight = max(pChild->GetMinHeight(), nFixedHeight);
		nFixedHeight = min(pChild->GetMaxHeight(), nFixedHeight);
		rcModal = CDUIRect(rcRangeLayout.left, nTileY, rcRangeLayout.right, nTileY);
		nTileY += pChild->GetPadding().top + pChild->GetPadding().bottom;
		nTileY += nFixedHeight <= 0 ? mapDynamicCtrl[pChild] : nFixedHeight;
		rcModal.bottom = nTileY;

		pChild->OnDuiSize(rcModal);

		CDUIRect rcChild = pChild->GetAbsoluteRect();
		rcMax.left = min(rcMax.left, rcChild.left);
		rcMax.top = min(rcMax.top, rcChild.top);
		rcMax.right = max(rcMax.right, rcChild.right);
		rcMax.bottom = max(rcMax.bottom, rcChild.bottom);
	}

	//layout bottom
	nTileY = rcRangeLayout.bottom;
	for (int nIndex = 0; nIndex < vecControlBottom.size(); nIndex++)
	{
		CDUIControlBase *pChild = vecControlBottom[nIndex];
		if (NULL == pChild) continue;

		if (nIndex > 0)
		{
			nTileY -= nChildPaddingV;
		}
		if (IsAutoCalcChildPaddingV() && nTileY != rcRangeLayout.bottom)
		{
			nTileY -= nDynamicChildPaddingV;

			if (nDynamicSurplus > 0)
			{
				nTileY--;
				nDynamicSurplus--;
			}
		}

		int nFixedHeight = pChild->GetFixedHeight();
		nFixedHeight = max(pChild->GetMinHeight(), nFixedHeight);
		nFixedHeight = min(pChild->GetMaxHeight(), nFixedHeight);
		rcModal = CDUIRect(rcRangeLayout.left, nTileY, rcRangeLayout.right, nTileY);
		nTileY -= pChild->GetPadding().top + pChild->GetPadding().bottom;
		nTileY -= nFixedHeight <= 0 ? mapDynamicCtrl[pChild] : nFixedHeight;
		rcModal.top = nTileY;

		pChild->OnDuiSize(rcModal);

		CDUIRect rcChild = pChild->GetAbsoluteRect();
		rcMax.left = min(rcMax.left, rcChild.left);
		rcMax.top = min(rcMax.top, rcChild.top);
		rcMax.right = max(rcMax.right, rcChild.right);
		rcMax.bottom = max(rcMax.bottom, rcChild.bottom);
	}

	UpdateScrollBar(CDUISize(rcMax.GetWidth() - rcRangeView.GetWidth(), rcMax.GetHeight() - rcRangeView.GetHeight()));

	CDUIControlBase::RefreshView();

	//autoheight
	if (IsAutoCalcHeight()
		&& nFixedHeightTotal != GetFixedHeight()
		&& DuiDpiScaleBackCtrl(nFixedHeightTotal) != DuiDpiScaleBackCtrl(GetFixedHeight()))
	{
		SetFixedHeight(DuiDpiScaleBackCtrl(nFixedHeightTotal));
	}

	return;
}

bool CDUIVerticalLayoutCtrl::DoPaint(HDC hDC, bool bGenerateBmp /* = false */)
{
	if (false == __super::DoPaint(hDC, bGenerateBmp)) return false;

	//darg topmost
	if (m_pAnimateDrag && m_pAnimateDrag->GetDragingCtrl())
	{
		m_pAnimateDrag->GetDragingCtrl()->OnDraw(hDC, m_rcPaint, bGenerateBmp);
	}

	return true;
}

void CDUIVerticalLayoutCtrl::InitProperty()
{
	__super::InitProperty();

	//animate
	DuiCreateGroupAttribute(m_AttributeGroupAnimation, _T("Animation"));
	DuiCreateAttribute(m_AttributeAnimateDrag, _T("AnimateDrag"), _T(""), m_AttributeGroupAnimation);

	//pos
	DuiCreateAttribute(m_AttributeAutoCalcHeight, _T("AutoCalcHeight"), _T(""), m_AttributeGroupPosition);

	//child
	DuiCreateAttribute(m_AttributeChildPaddingV, _T("ChildPaddingV"), _T(""), m_AttributeGroupChild);
	DuiCreateAttribute(m_AttributeAutoCalcChildPaddingV, _T("AutoCalcChildPaddingV"), _T("For TileV"), m_AttributeGroupChild);

	return;
}

void CDUIVerticalLayoutCtrl::InitComplete()
{
	__super::InitComplete();

	DuiInitAttriVisible(m_AttributeAnimateDrag, true);
	DuiInitAttriVisible(m_AttributeGroupChild, true);
	DuiInitAttriVisible(m_AttributeChildPaddingH, false);
	DuiInitAttriVisible(m_AttributeAutoCalcHeight, true);
	DuiInitAttriVisible(m_AttributeAutoCalcChildPaddingH, false);

	if (IsAnimateDrag())
	{
		m_pAnimateDrag = new CDUIAnimationDrag(this, DragType_V);
	}

	return;
}

void CDUIVerticalLayoutCtrl::OnDuiWndManagerAttach()
{
	__super::OnDuiWndManagerAttach();

	if (m_pWndOwner && m_pAnimateDrag)
	{
		m_pWndOwner->AddINotify(m_pAnimateDrag);
	}
	
	return;
}

void CDUIVerticalLayoutCtrl::OnDuiWndManagerDetach()
{
	if (m_pWndOwner)
	{
		m_pWndOwner->RemoveINotify(m_pAnimateDrag);
	}

	return __super::OnDuiWndManagerDetach();
}

void CDUIVerticalLayoutCtrl::OnDuiTimer(UINT uTimerID, const DuiMessage &Msg)
{
	if (Dui_TimerAnimate_ID == uTimerID)
	{
		if (m_pAnimateDrag)
		{
			m_pAnimateDrag->OnAnimationElapse(uTimerID);
		}

		return;
	}

	return __super::OnDuiTimer(uTimerID, Msg);
}

void CDUIVerticalLayoutCtrl::ScrollChilds(CDUISize szScroll)
{
	return __super::ScrollChilds(szScroll);
}
