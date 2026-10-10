#include "StdAfx.h"
#include "DUIHorizontalLayoutCtrl.h"

DuiImplement_CreateControl(CDUIHorizontalLayoutCtrl)
MMImplement_ClassName(CDUIHorizontalLayoutCtrl)

CDUIHorizontalLayoutCtrl::CDUIHorizontalLayoutCtrl(void)
{
}

CDUIHorizontalLayoutCtrl::~CDUIHorizontalLayoutCtrl(void)
{
	if (m_pWndOwner)
	{
		m_pWndOwner->RemoveINotify(m_pAnimateDrag);
	}

	MMSafeDelete(m_pAnimateDrag);

	return;
}

LPVOID CDUIHorizontalLayoutCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIHorizontalLayoutCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIHorizontalLayoutCtrl::GetDescribe() const
{
	return Dui_Ctrl_HorizontalLayout;
}

bool CDUIHorizontalLayoutCtrl::IsAnimateDrag()
{
	return m_AttributeAnimateDrag.GetValue();
}

void CDUIHorizontalLayoutCtrl::SetAnimateDrag(bool bAnimateDrag)
{
	if (bAnimateDrag == IsAnimateDrag()) return;

	m_AttributeAnimateDrag.SetValue(bAnimateDrag);

	if (IsAnimateDrag() && NULL == m_pAnimateDrag)
	{
		m_pAnimateDrag = new CDUIAnimationDrag(this, DragType_H);
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

bool CDUIHorizontalLayoutCtrl::IsAnimateDraging()
{
	return m_pAnimateDrag && m_pAnimateDrag->IsAnimateDraging();
}

void CDUIHorizontalLayoutCtrl::OnDragMoveBegin(CDUIControlBase *pDragCtrl)
{
	if (m_pAnimateDrag) m_pAnimateDrag->OnDragMoveBegin(pDragCtrl);
}

void CDUIHorizontalLayoutCtrl::OnDragMoving(CDUIControlBase *pDragCtrl)
{
	if (m_pAnimateDrag) m_pAnimateDrag->OnDragMoving(pDragCtrl);
}

void CDUIHorizontalLayoutCtrl::OnDragMoveEnd(CDUIControlBase *pDragCtrl)
{
	if (m_pAnimateDrag) m_pAnimateDrag->OnDragMoveEnd(pDragCtrl);
}

int CDUIHorizontalLayoutCtrl::TranslateIndex(CDUIPoint pt)
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

void CDUIHorizontalLayoutCtrl::RefreshView()
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
	int nChildPaddingH = GetChildPaddingH();
	int nFixedWidthTotal = 0;
	VecDuiControlBase vecControlLeft, vecControlRight;
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

		//right
		enDuiHorizAlignType HorizAlignType = pChild->GetHorizAlignType();
		if (HorizAlign_Right == HorizAlignType)
		{
			vecControlRight.insert(vecControlRight.begin(), pChild);
		}
		//left
		else
		{
			vecControlLeft.push_back(pChild);
		}

		//width
		CDUIRect rcPadding = pChild->GetPadding();
		nFixedWidthTotal += rcPadding.left;
		nFixedWidthTotal += rcPadding.right;

		if ((HorizAlign_Right == HorizAlignType && vecControlRight.size() > 1)
			|| (HorizAlign_Right != HorizAlignType && vecControlLeft.size() > 1))
		{
			nFixedWidthTotal += nChildPaddingH;
		}

		int nFixedWidth = pChild->GetFixedWidth();
		nFixedWidth = max(pChild->GetMinWidth(), nFixedWidth);
		nFixedWidth = min(pChild->GetMaxWidth(), nFixedWidth);
		if (nFixedWidth <= 0)
		{
			mapDynamicCtrl[pChild] = 0;
			continue;
		}

		nFixedWidthTotal += nFixedWidth;
	}

	//dynamic width
	int nDynamicWidth = 0;
	int nDynamicSurplus = rcRangeLayout.GetWidth() - nFixedWidthTotal;
	nDynamicSurplus = max(nDynamicSurplus, 0);
	if (mapDynamicCtrl.size() > 0)
	{
		int nDynamicWidthTotal = rcRangeLayout.GetWidth() - nFixedWidthTotal;
		nDynamicWidthTotal = max(nDynamicWidthTotal, 0);
		nDynamicWidth = nDynamicWidthTotal / mapDynamicCtrl.size();
		nDynamicSurplus = nDynamicWidthTotal % mapDynamicCtrl.size();

		if (nDynamicWidth > 0)
		{
			for (auto &DynamicCtrl : mapDynamicCtrl)
			{
				CDUIControlBase *pChild = DynamicCtrl.first;
				if (NULL == pChild) continue;

				DynamicCtrl.second = min(nDynamicWidth, pChild->GetMaxWidth());

				if (nDynamicWidth > pChild->GetMaxWidth())
				{
					nDynamicSurplus += nDynamicWidth - pChild->GetMaxWidth();
				}
			}
			while (nDynamicSurplus > 0)
			{
				int nDynamicSurplusOld = nDynamicSurplus;

				for (auto &DynamicCtrl : mapDynamicCtrl)
				{
					CDUIControlBase *pChild = DynamicCtrl.first;
					if (NULL == pChild || pChild->GetMaxWidth() <= DynamicCtrl.second) continue;

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
	int nDynamicChildPaddingH = 0;
	if (IsAutoCalcChildPaddingH() && vecControlLeft.size() + vecControlRight.size() - 1 > 0)
	{
		nDynamicChildPaddingH = nDynamicSurplus / (vecControlLeft.size() + vecControlRight.size() - 1);
		nDynamicSurplus = nDynamicSurplus % (vecControlLeft.size() + vecControlRight.size() - 1);
	}

	//adjust layout
	if (nFixedWidthTotal > rcRangeLayout.GetWidth())
	{
		rcRangeLayout.right += (nFixedWidthTotal - rcRangeLayout.GetWidth());
	}

	//layout left
	CDUIRect rcModal;
	int nTileX = rcRangeLayout.left;
	for (int nIndex = 0; nIndex < vecControlLeft.size(); nIndex++)
	{
		CDUIControlBase *pChild = vecControlLeft[nIndex];
		if (NULL == pChild) continue;

		if (nIndex > 0)
		{
			nTileX += nChildPaddingH;
		}
		if (IsAutoCalcChildPaddingH() && nTileX != rcRangeLayout.left)
		{
			nTileX += nDynamicChildPaddingH;
		
			if (nDynamicSurplus > 0)
			{
				nTileX++;
				nDynamicSurplus--;
			}
		}

		int nFixedWidth = pChild->GetFixedWidth();
		nFixedWidth = max(pChild->GetMinWidth(), nFixedWidth);
		nFixedWidth = min(pChild->GetMaxWidth(), nFixedWidth);
		rcModal = CDUIRect(nTileX, rcRangeLayout.top, nTileX, rcRangeLayout.bottom);
		nTileX += pChild->GetPadding().left + pChild->GetPadding().right;
		nTileX += nFixedWidth <= 0 ? mapDynamicCtrl[pChild] : nFixedWidth;
		rcModal.right = nTileX;
		
		pChild->OnDuiSize(rcModal);

		CDUIRect rcChild = pChild->GetAbsoluteRect();
		rcMax.left = min(rcMax.left, rcChild.left);
		rcMax.top = min(rcMax.top, rcChild.top);
		rcMax.right = max(rcMax.right, rcChild.right);
		rcMax.bottom = max(rcMax.bottom, rcChild.bottom);
	}

	//layout right
	nTileX = rcRangeLayout.right;
	for (int nIndex = 0; nIndex < vecControlRight.size(); nIndex++)
	{
		CDUIControlBase *pChild = vecControlRight[nIndex];
		if (NULL == pChild) continue;

		if (nIndex > 0)
		{
			nTileX -= nChildPaddingH;
		}
		if (IsAutoCalcChildPaddingH() && nTileX != rcRangeLayout.right)
		{
			nTileX -= nDynamicChildPaddingH;

			if (nDynamicSurplus > 0)
			{
				nTileX--;
				nDynamicSurplus--;
			}
		}

		int nFixedWidth = pChild->GetFixedWidth();
		nFixedWidth = max(pChild->GetMinWidth(), nFixedWidth);
		nFixedWidth = min(pChild->GetMaxWidth(), nFixedWidth);
		rcModal = CDUIRect(nTileX, rcRangeLayout.top, nTileX, rcRangeLayout.bottom);
		nTileX -= pChild->GetPadding().left + pChild->GetPadding().right;
		nTileX -= nFixedWidth <= 0 ? mapDynamicCtrl[pChild] : nFixedWidth;
		rcModal.left = nTileX;

		pChild->OnDuiSize(rcModal);

		CDUIRect rcChild = pChild->GetAbsoluteRect();
		rcMax.left = min(rcMax.left, rcChild.left);
		rcMax.top = min(rcMax.top, rcChild.top);
		rcMax.right = max(rcMax.right, rcChild.right);
		rcMax.bottom = max(rcMax.bottom, rcChild.bottom);
	}

	//scrollbar
	UpdateScrollBar(CDUISize(rcMax.GetWidth() - rcRangeView.GetWidth(), rcMax.GetHeight() - rcRangeView.GetHeight()));

	CDUIControlBase::RefreshView();

	//autowidth
	if (IsAutoCalcWidth()
		&& nFixedWidthTotal != GetFixedWidth()
		&& DuiDpiScaleBackCtrl(nFixedWidthTotal) != DuiDpiScaleBackCtrl(GetFixedWidth()))
	{
		SetFixedWidth(DuiDpiScaleBackCtrl(nFixedWidthTotal));
	}

	return;
}

bool CDUIHorizontalLayoutCtrl::DoPaint(HDC hDC, bool bGenerateBmp /* = false */)
{
	if (false == __super::DoPaint(hDC, bGenerateBmp)) return false;

	//darg topmost
	if (m_pAnimateDrag && m_pAnimateDrag->GetDragingCtrl())
	{
		m_pAnimateDrag->GetDragingCtrl()->OnDraw(hDC, m_rcPaint, bGenerateBmp);
	}

	return true;
}

void CDUIHorizontalLayoutCtrl::InitProperty()
{
	__super::InitProperty();

	//animate
	DuiCreateGroupAttribute(m_AttributeGroupAnimation, _T("Animation"));
	DuiCreateAttribute(m_AttributeAnimateDrag, _T("AnimateDrag"), _T(""), m_AttributeGroupAnimation);

	//pos
	DuiCreateAttribute(m_AttributeAutoCalcWidth, _T("AutoCalcWidth"), _T(""), m_AttributeGroupPosition);

	//child
	DuiCreateAttribute(m_AttributeChildPaddingH, _T("ChildPaddingH"), _T(""), m_AttributeGroupChild);
	DuiCreateAttribute(m_AttributeAutoCalcChildPaddingH, _T("AutoCalcChildPaddingH"), _T("For TileH"), m_AttributeGroupChild);

	return;
}

void CDUIHorizontalLayoutCtrl::InitComplete()
{
	__super::InitComplete();

	DuiInitAttriVisible(m_AttributeAnimateDrag, true);
	DuiInitAttriVisible(m_AttributeGroupChild, true);
	DuiInitAttriVisible(m_AttributeChildPaddingV, false);
	DuiInitAttriVisible(m_AttributeAutoCalcWidth, true);
	DuiInitAttriVisible(m_AttributeAutoCalcChildPaddingV, false);

	if (IsAnimateDrag())
	{
		m_pAnimateDrag = new CDUIAnimationDrag(this, DragType_H);
	}

	return;
}

void CDUIHorizontalLayoutCtrl::OnDuiWndManagerAttach()
{
	__super::OnDuiWndManagerAttach();

	if (m_pWndOwner && m_pAnimateDrag)
	{
		m_pWndOwner->AddINotify(m_pAnimateDrag);
	}

	return;
}

void CDUIHorizontalLayoutCtrl::OnDuiWndManagerDetach()
{
	if (m_pWndOwner)
	{
		m_pWndOwner->RemoveINotify(m_pAnimateDrag);
	}

	return __super::OnDuiWndManagerDetach();
}

void CDUIHorizontalLayoutCtrl::OnDuiTimer(UINT uTimerID, const DuiMessage &Msg)
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

void CDUIHorizontalLayoutCtrl::ScrollChilds(CDUISize szScroll)
{
	return __super::ScrollChilds(szScroll);
}
