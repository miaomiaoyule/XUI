#include "StdAfx.h"
#include "DUIDockBarCtrl.h"

#include <math.h>

//////////////////////////////////////////////////////////////////////////
//scale lerp speed
#define DockBar_Scale_Lerp_Mouse				(0.35f)

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateControl(CDUIDockBarCtrl)
MMImplement_ClassName(CDUIDockBarCtrl)

CDUIDockBarCtrl::CDUIDockBarCtrl(void)
{
	//dock effect type
	static tagDuiCombox AttriCombox;
	if (AttriCombox.vecItem.empty())
	{
		AttriCombox.vecItem.push_back({ DockBarEffect_None, _T("None") });
		AttriCombox.vecItem.push_back({ DockBarEffect_MouseMove, _T("MouseMove") });
	}

	m_AttributeEffectType.SetCombox(AttriCombox);
	m_AttributeEffectType.SelectItem(DockBarEffect_MouseMove);

	return;
}

CDUIDockBarCtrl::~CDUIDockBarCtrl(void)
{
	StopEffect();

	return;
}

bool CDUIDockBarCtrl::OnAttributeChange(CDUIAttributeObject *pAttributeObj)
{
	if (__super::OnAttributeChange(pAttributeObj)) return true;

	if (pAttributeObj == &m_AttributeEffectType)
	{
		OnDockAttributeChanged(true);

		return true;
	}
	if (pAttributeObj == &m_AttributeAlignBottom
		|| pAttributeObj == &m_AttributeScaleMax
		|| pAttributeObj == &m_AttributeEffectRange)
	{
		bool bResetScale = (pAttributeObj != &m_AttributeAlignBottom);
		OnDockAttributeChanged(bResetScale);

		return true;
	}
	if (pAttributeObj == &m_AttributeDockImageBk
		|| pAttributeObj == &m_AttributeReflectionRatio)
	{
		Invalidate();

		return true;
	}

	return false;
}

void CDUIDockBarCtrl::OnDpiChanged(int nScalePre)
{
	__super::OnDpiChanged(nScalePre);

	StopEffect();
	ResetItemScale();
	ResetItemSet();

	//the item fixed size is rescaled, the base size must be captured again
	m_vecItemBaseSize.clear();

	return;
}

void CDUIDockBarCtrl::OnNotify(CDUIControlBase *pControl, const DuiNotify &Notify)
{
	if (pControl == this)
	{
		if (DuiNotify_ItemMouseEnter == Notify.NotifyType)
		{
			OnItemMouseEnter(pControl, Notify);
		}
		else if (DuiNotify_ItemMouseMove == Notify.NotifyType)
		{
			OnItemMouseMove(pControl, Notify);
		}
		else if (DuiNotify_ItemMouseLeave == Notify.NotifyType)
		{
			OnItemMouseLeave(pControl, Notify);
		}
		else if (DuiNotify_VisibleChanged == Notify.NotifyType)
		{
			if (false == IsVisible())
			{
				StopEffect();
				ResetItemScale();
			}
		}

		return;
	}

	return IDuiControlCallBack::OnNotify(pControl, Notify);
}

void CDUIDockBarCtrl::OnRelease(CDUIControlBase *pControl)
{
	if (NULL == pControl) return;

	return IDuiControlCallBack::OnRelease(pControl);
}

LPVOID CDUIDockBarCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIDockBarCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIDockBarCtrl::GetDescribe() const
{
	return Dui_Ctrl_DockBar;
}

void CDUIDockBarCtrl::RefreshView()
{
	//item size is calculated by dock bar, then the list view layout the items by the size
	ApplyItemLayout();

	__super::RefreshView();

	return;
}

CDUISize CDUIDockBarCtrl::GetSwitchTileItemSize()
{
	//width 0: the list view layout every item by the item width(accumulate),
	//so the dock bar width can be calculated by the total item width
	//height: the max base item height, for the row height of tile layout
	int nMaxHeight = 0;
	int nCount = GetChildCount();
	for (int n = 0; n < nCount; n++)
	{
		nMaxHeight = max(nMaxHeight, (int)GetBaseItemSize(n).cy);
	}

	CDUISize szItem(0, nMaxHeight);

	return szItem;
}

bool CDUIDockBarCtrl::IsAutoCalcItemWidth()
{
	//the item width is calculated by dock bar(base size * scale), not by list view
	return false;
}

bool CDUIDockBarCtrl::IsAutoCalcItemHeight()
{
	return false;
}

void CDUIDockBarCtrl::OnDragMoveBegin(CDUIControlBase *pDragCtrl)
{
	//the item is dragged, stop the magnify effect and restore the item size.
	//the restore must be done before __super::OnDragMoveBegin, because the
	//list view stops its refresh while the drag animation is running, then
	//the item size and position can never be restored during the drag
	StopEffect();
	ResetItemScale();

	RefreshView();

	return __super::OnDragMoveBegin(pDragCtrl);
}

bool CDUIDockBarCtrl::InsertChild(CDUIControlBase *pChild, int nPos)
{
	if (false == __super::InsertChild(pChild, nPos)) return false;

	ResetItemScale();
	NeedRefreshView();

	return true;
}

bool CDUIDockBarCtrl::IsAutoCalcChildPaddingH()
{
	//the item space is set by ChildPaddingH attribute, not calculated by list view
	return false;
}

bool CDUIDockBarCtrl::IsAutoCalcChildPaddingV()
{
	return false;
}

bool CDUIDockBarCtrl::Remove(CDUIControlBase *pControl)
{
	bool bRes = __super::Remove(pControl);

	ResetItemScale();
	NeedRefreshView();

	return bRes;
}

bool CDUIDockBarCtrl::RemoveAt(int nIndex)
{
	bool bRes = __super::RemoveAt(nIndex);

	ResetItemScale();
	NeedRefreshView();

	return bRes;
}

void CDUIDockBarCtrl::RemoveAll()
{
	__super::RemoveAll();

	ResetItemScale();
	NeedRefreshView();

	return;
}

enDuiDockBarEffectType CDUIDockBarCtrl::GetEffectType()
{
	return (enDuiDockBarEffectType)m_AttributeEffectType.GetCurSelItem();
}

void CDUIDockBarCtrl::SetEffectType(enDuiDockBarEffectType EffectType)
{
	if (EffectType == GetEffectType()) return;

	m_AttributeEffectType.SelectItem(EffectType);

	OnDockAttributeChanged(true);

	return;
}

tagDuiImageSection CDUIDockBarCtrl::GetDockImageBk()
{
	return m_AttributeDockImageBk.GetImageSection();
}

void CDUIDockBarCtrl::SetDockImageBk(const tagDuiImageSection &ImageSection)
{
	if (ImageSection == GetDockImageBk()) return;

	m_AttributeDockImageBk.SetImageSection(ImageSection);

	Invalidate();

	return;
}

CDUIRect CDUIDockBarCtrl::GetDockImageBkRect()
{
	//the item background image is icon + reflection in one image, the reflection
	//is generated by CDUIRenderEngine::GenerateReflectionBitmap with the ratio
	//ReflectionRatio(reflection height / icon height), so the item height is
	//icon * (1 + ratio): the icon part is the top of the item, the reflection
	//is the bottom part, the reflection area must be out of the background area.
	//the area is the bounding rect of the icon part of all visible items, every
	//item is clamped to its base size(scale == 1), so the magnified part is out
	//of the area too. the rect is absolute(window coordinate), for the acrylic
	//region of the window it can be converted to screen coordinate by the caller
	float fRatio = GetReflectionRatio();
	float fIconRatio = 1.0f / (1.0f + fRatio);

	CDUIRect rcBk;
	bool bFind = false;
	bool bAlignBottom = IsAlignBottom();

	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUIListItemCtrl *pItem = GetChildAt(n);
		if (NULL == pItem || false == pItem->IsVisible()) continue;

		CDUIRect rcItem = pItem->GetAbsoluteRect();
		SIZE szBase = GetBaseItemSize(n);

		//the icon part height of the item height
		int nIconH = (int)(szBase.cy * fIconRatio + 0.5f);
		if (nIconH < 1) nIconH = 1;
		int nReflectH = max(0, (int)szBase.cy - nIconH);

		CDUIRect rcBase = rcItem;
		rcBase.right = rcBase.left + min(szBase.cx, rcBase.GetWidth());
		if (bAlignBottom)
		{
			//the item grows up from the fixed bottom, the reflection hangs at
			//the bottom of the item, the icon part is above the reflection
			rcBase.bottom = rcBase.bottom - min(nReflectH, rcBase.GetHeight());
			rcBase.top = max(rcBase.top, rcBase.bottom - nIconH);
		}
		else
		{
			//the item grows down from the fixed top, the icon part is at the top
			rcBase.top = rcItem.top;
			rcBase.bottom = min(rcBase.bottom, rcBase.top + nIconH);
		}

		if (false == bFind)
		{
			rcBk = rcBase;
			bFind = true;

			continue;
		}

		rcBk.left = min(rcBk.left, rcBase.left);
		rcBk.top = min(rcBk.top, rcBase.top);
		rcBk.right = max(rcBk.right, rcBase.right);
		rcBk.bottom = max(rcBk.bottom, rcBase.bottom);
	}

	return rcBk;
}

float CDUIDockBarCtrl::GetReflectionRatio()
{
	float fRatio = m_AttributeReflectionRatio.GetValue();

	//the reflection is generated by CDUIRenderEngine::GenerateReflectionBitmap,
	//its height is the icon height * ratio, the item height = icon * (1 + ratio)
	return max(0.0f, fRatio);
}

void CDUIDockBarCtrl::SetReflectionRatio(float fRatio)
{
	if (fRatio < 0.0f) fRatio = 0.0f;
	if (fRatio == GetReflectionRatio()) return;

	m_AttributeReflectionRatio.SetValue(fRatio);

	Invalidate();

	return;
}

bool CDUIDockBarCtrl::IsAlignBottom()
{
	return m_AttributeAlignBottom.GetValue();
}

void CDUIDockBarCtrl::SetAlignBottom(bool bBottom)
{
	if (bBottom == IsAlignBottom()) return;

	m_AttributeAlignBottom.SetValue(bBottom);

	OnDockAttributeChanged(false);

	return;
}

float CDUIDockBarCtrl::GetScaleMax()
{
	float fScaleMax = m_AttributeScaleMax.GetValue();

	return max(1.0f, fScaleMax);
}

void CDUIDockBarCtrl::SetScaleMax(float fScaleMax)
{
	if (fScaleMax <= 0 || fScaleMax == m_AttributeScaleMax.GetValue()) return;

	m_AttributeScaleMax.SetValue(fScaleMax);

	OnDockAttributeChanged(true);

	return;
}

int CDUIDockBarCtrl::GetEffectRange()
{
	return m_AttributeEffectRange.GetValue();
}

void CDUIDockBarCtrl::SetEffectRange(int nRange)
{
	if (nRange == GetEffectRange()) return;

	m_AttributeEffectRange.SetValue(nRange);

	OnDockAttributeChanged(true);

	return;
}

float CDUIDockBarCtrl::GetItemScale(int nIndex)
{
	if (nIndex < 0 || nIndex >= (int)m_vecItemScale.size()) return 1.0f;

	return m_vecItemScale[nIndex];
}

bool CDUIDockBarCtrl::OnDuiMouseMove(const CDUIPoint &pt, const DuiMessage &Msg)
{
	bool bRes = __super::OnDuiMouseMove(pt, Msg);

	//mouse is in the gap between items
	if (DockBarEffect_MouseMove == GetEffectType())
	{
		m_bMouseInDock = true;

		UpdateMousePosX(pt.x);

		StartEffectTimer();
	}

	return bRes;
}

void CDUIDockBarCtrl::OnDuiMouseLeave(const CDUIPoint &pt, const DuiMessage &Msg)
{
	__super::OnDuiMouseLeave(pt, Msg);

	//mouse may move from dock bar into its item, not leave the dock bar
	if (false == GetAbsoluteRect().PtInRect(pt))
	{
		m_bMouseInDock = false;
	}

	if (DockBarEffect_MouseMove == GetEffectType())
	{
		StartEffectTimer();
	}

	return;
}

void CDUIDockBarCtrl::OnDuiWndManagerDetach()
{
	StopEffect();

	__super::OnDuiWndManagerDetach();

	return;
}

void CDUIDockBarCtrl::InitProperty()
{
	__super::InitProperty();

	//dock bar
	DuiCreateGroupAttribute(m_AttributeGroupDockBar, _T("DockBar"));
	DuiCreateAttribute(m_AttributeEffectType, _T("EffectType"), _T(""), m_AttributeGroupDockBar);
	DuiCreateAttribute(m_AttributeAlignBottom, _T("AlignBottom"), _T(""), m_AttributeGroupDockBar);
	DuiCreateAttribute(m_AttributeScaleMax, _T("ScaleMax"), _T(""), m_AttributeGroupDockBar);
	DuiCreateAttribute(m_AttributeEffectRange, _T("EffectRange"), _T(""), m_AttributeGroupDockBar);
	DuiCreateAttribute(m_AttributeReflectionRatio, _T("ReflectionRatio"), _T(""), m_AttributeGroupDockBar);
	DuiCreateAttribute(m_AttributeDockImageBk, _T("DockImageBk"), _T(""), m_AttributeGroupDockBar);

	return;
}

void CDUIDockBarCtrl::InitComplete()
{
	__super::InitComplete();

	DuiInitAttriVisible(m_AttributeGroupListType, false);

	//dock bar only support the horizontal tile layout
	SwitchListViewType(ListView_TileH);

	//dock bar never use select icon
	SetSelectIconVisible(false);

	//item notify is send by list view self(pNotifyCtrl == this), so dock bar must listen self
	RegisterControlCallBack(this);

	SyncItemScale();

	return;
}

void CDUIDockBarCtrl::PaintBkImage(HDC hDC)
{
	__super::PaintBkImage(hDC);

	//the background is painted on the base icon area only, the magnified
	//part of the icon and the reflection are out of the area, so the icon
	//pops out of the background when it is magnified
	if (false == m_AttributeDockImageBk.IsEmpty())
	{
		m_AttributeDockImageBk.Draw(hDC, GetDockImageBkRect(), m_rcPaint);
	}

	return;
}

void CDUIDockBarCtrl::OnItemMouseMove(CDUIControlBase *pControl, const DuiNotify &Notify)
{
	if (DockBarEffect_MouseMove != GetEffectType()) return;

	m_bMouseInDock = true;

	UpdateMousePosX(Notify.ptMouse.x);

	StartEffectTimer();

	return;
}

void CDUIDockBarCtrl::OnItemMouseEnter(CDUIControlBase *pControl, const DuiNotify &Notify)
{
	if (DockBarEffect_None == GetEffectType()) return;

	m_bMouseInDock = true;

	if (DockBarEffect_MouseMove == GetEffectType())
	{
		OnItemMouseMove(pControl, Notify);
	}

	return;
}

void CDUIDockBarCtrl::OnItemMouseLeave(CDUIControlBase *pControl, const DuiNotify &Notify)
{
	//move between items: the leave of old item is earlier than enter of new item,
	//so judge by mouse position, avoid effect break when mouse across item gap
	if (false == GetAbsoluteRect().PtInRect(Notify.ptMouse))
	{
		m_bMouseInDock = false;
	}

	if (DockBarEffect_MouseMove == GetEffectType())
	{
		StartEffectTimer();
	}

	return;
}

void CDUIDockBarCtrl::SyncItemScale()
{
	int nCount = GetChildCount();
	if ((int)m_vecItemScale.size() == nCount
		&& (int)m_vecItemSet.size() == nCount
		&& (int)m_vecItemBaseSize.size() == nCount) return;

	m_vecItemScale.assign(nCount, 1.0f);
	m_vecItemSet.assign(nCount, tagDuiDockItemSet());

	//keep the base size of the old items, the new item base is captured by its fixed size
	std::vector<CDUISize> vecBaseOld = m_vecItemBaseSize;
	m_vecItemBaseSize.assign(nCount, CDUISize());
	int nCopy = min(nCount, (int)vecBaseOld.size());
	for (int n = 0; n < nCopy; n++)
	{
		if (vecBaseOld[n].cx > 0 && vecBaseOld[n].cy > 0) m_vecItemBaseSize[n] = vecBaseOld[n];
	}

	return;
}

void CDUIDockBarCtrl::ResetItemScale()
{
	int nCount = GetChildCount();

	m_vecItemScale.assign(nCount, 1.0f);
	m_vecItemSet.assign(nCount, tagDuiDockItemSet());

	return;
}

void CDUIDockBarCtrl::ResetItemSet()
{
	m_vecItemSet.assign(GetChildCount(), tagDuiDockItemSet());

	return;
}

void CDUIDockBarCtrl::ApplyItemLayout()
{
	SyncItemScale();

	int nCount = GetChildCount();
	if (nCount <= 0) return;

	CDUIRect rcRange = GetLayoutRangeOfItem();

	//item top is same as the list view tile layout
	SIZE szScrollPos = GetScrollPos();
	int nItemTop = rcRange.top + (IsTopFromRange() ? 0 : GetChildPaddingV() / 2) - szScrollPos.cy;
	bool bAlignBottom = IsAlignBottom();

	for (int n = 0; n < nCount; n++)
	{
		CDUIListItemCtrl *pItem = GetChildAt(n);
		if (NULL == pItem) continue;

		//the item size is the base size(the user set fixed size) * scale
		SIZE szBase = GetBaseItemSize(n);
		float fScale = m_vecItemScale[n];
		int nWidth = max(1, (int)(szBase.cx * fScale + 0.5f));
		int nHeight = max(1, (int)(szBase.cy * fScale + 0.5f));

		//icon anchored at bottom, so the item grows up from the range bottom
		int nPaddingTop = 0;
		if (bAlignBottom && rcRange.GetHeight() > 0)
		{
			nPaddingTop = rcRange.bottom - nHeight - nItemTop;
		}

		//item value is logical, it is scaled by dpi when used
		tagDuiDockItemSet ItemSet;
		ItemSet.nWidth = DuiDpiScaleBackCtrl(nWidth);
		ItemSet.nHeight = DuiDpiScaleBackCtrl(nHeight);
		ItemSet.nPaddingTop = DuiDpiScaleBackCtrl(nPaddingTop);

		if (ItemSet.nWidth == m_vecItemSet[n].nWidth
			&& ItemSet.nHeight == m_vecItemSet[n].nHeight
			&& ItemSet.nPaddingTop == m_vecItemSet[n].nPaddingTop)
		{
			continue;
		}

		//item is layout by list view, so the left padding must be reset
		pItem->SetPaddingL(0);
		pItem->SetPaddingT(ItemSet.nPaddingTop);
		pItem->SetFixedWidth(ItemSet.nWidth);
		pItem->SetFixedHeight(ItemSet.nHeight);

		m_vecItemSet[n] = ItemSet;
	}

	return;
}

SIZE CDUIDockBarCtrl::GetBaseItemSize(int nIndex)
{
	CDUIListItemCtrl *pItem = (nIndex >= 0 && nIndex < GetChildCount()) ? GetChildAt(nIndex) : NULL;
	if (NULL == pItem) return SIZE{ 0, 0 };

	bool bStored = (nIndex >= 0 && nIndex < (int)m_vecItemBaseSize.size());
	SIZE szBase = bStored ? (SIZE)m_vecItemBaseSize[nIndex] : SIZE{ 0, 0 };
	if (szBase.cx > 0 && szBase.cy > 0) return szBase;

	//the base size is the item fixed size which the user set,
	//it is captured once at first layout, the value is pixel(scaled)
	szBase.cx = pItem->GetFixedWidth();
	szBase.cy = pItem->GetFixedHeight();
	if (szBase.cx <= 0 && szBase.cy > 0) szBase.cx = szBase.cy;
	if (szBase.cy <= 0 && szBase.cx > 0) szBase.cy = szBase.cx;
	if (szBase.cx <= 0) szBase.cx = DuiDpiScaleCtrl((int)DockBar_IconSize_Default);
	if (szBase.cy <= 0) szBase.cy = szBase.cx;

	if (bStored) m_vecItemBaseSize[nIndex] = szBase;

	return szBase;
}

int CDUIDockBarCtrl::GetBaseItemCenterX(int nIndex)
{
	//base center is calculated by scale 1 accumulate layout, so the mouse hit
	//is stable when item is magnified
	int nSpace = GetChildPaddingH();
	int nLeftFrom = (IsLeftFromRange() ? 0 : nSpace / 2);
	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUIListItemCtrl *pItem = GetChildAt(n);
		if (NULL == pItem || false == pItem->IsVisible()) continue;

		SIZE szBase = GetBaseItemSize(n);
		if (n == nIndex) return nLeftFrom + (int)szBase.cx / 2;

		nLeftFrom += (int)szBase.cx + nSpace;
	}

	return 0;
}

int CDUIDockBarCtrl::GetEffectRangeValue()
{
	//the range is the count of the icons affected on each side of the mouse,
	//it is converted to pixel by the slot width(the widest icon + the padding),
	//so the falloff always covers exactly the count of icons, 0 means auto
	int nCount = GetEffectRange();
	if (nCount <= 0) nCount = DockBar_EffectRange_Icon_Default;

	int nMaxWidth = 0;
	for (int n = 0; n < GetChildCount(); n++)
	{
		nMaxWidth = max(nMaxWidth, (int)GetBaseItemSize(n).cx);
	}

	return max(1, nCount) * (nMaxWidth + GetChildPaddingH());
}

void CDUIDockBarCtrl::UpdateMousePosX(int nAbsoluteX)
{
	//the mouse x is measured against the item range left captured at rest.
	//if it is measured against the current left edge, it feeds back: the
	//effect changes the item width, the total width changes, the dock bar
	//resized by AutoCalcWidth is placed again, the item enter or leave fires
	//by the shift, then the mouse x relative to the shifted edge changes,
	//the scales change again and the total width flips back, the whole bar
	//jitters 1 pixel left and right even the mouse does not move.
	//the left edge is stable while the effect timer is stopped, so capture
	//it only then, the mouse x is never moved by the effect itself
	CDUIRect rcRange = GetLayoutRangeOfItem();
	if (0 == m_uEffectTimer)
	{
		m_nMouseRefPosX = rcRange.left;
	}

	m_nMousePosX = nAbsoluteX - m_nMouseRefPosX;

	return;
}

void CDUIDockBarCtrl::StartEffectTimer()
{
	//the item is dragging, the effect is resumed after the drag is finished
	if (IsAnimateDraging()) return;

	if (m_uEffectTimer || NULL == m_pWndOwner) return;

	m_uEffectTimer = m_pWndOwner->TimerTask(DockBar_Effect_Elapse, true, [this]()
	{
		OnEffectElapse();
	});

	return;
}

void CDUIDockBarCtrl::StopEffect()
{
	if (m_uEffectTimer)
	{
		if (m_pWndOwner)
		{
			m_pWndOwner->StopTimer(m_uEffectTimer);
		}

		m_uEffectTimer = 0;
	}

	return;
}

void CDUIDockBarCtrl::OnEffectElapse()
{
	bool bContinue = false;
	if (DockBarEffect_MouseMove == GetEffectType())
	{
		bContinue = UpdateMouseMoveEffect();
	}

	if (false == bContinue)
	{
		StopEffect();
	}

	//apply the last frame, the item size is restored when effect is finished
	NeedRefreshView();

	return;
}

bool CDUIDockBarCtrl::UpdateMouseMoveEffect()
{
	SyncItemScale();

	int nCount = GetChildCount();
	if (nCount <= 0) return false;

	float fScaleMax = GetScaleMax();
	int nRange = GetEffectRangeValue();
	bool bChanged = false;

	for (int n = 0; n < nCount; n++)
	{
		CDUIListItemCtrl *pItem = GetChildAt(n);
		if (NULL == pItem || false == pItem->IsVisible())
		{
			m_vecItemScale[n] = 1.0f;

			continue;
		}

		float fTarget = 1.0f;
		if (m_bMouseInDock && nRange > 0)
		{
			int nCenter = GetBaseItemCenterX(n);
			int nDistance = abs(m_nMousePosX - nCenter);
			if (nDistance < nRange)
			{
				//cosine falloff, max scale at mouse center
				float fRatio = (float)nDistance / (float)nRange;
				fTarget = 1.0f + (fScaleMax - 1.0f) * 0.5f * (1.0f + cosf(PI * fRatio));
			}
		}

		float fScaleOld = m_vecItemScale[n];
		float fScale = fScaleOld + (fTarget - fScaleOld) * DockBar_Scale_Lerp_Mouse;
		if (fTarget == 1.0f && fabs(fScale - 1.0f) <= DockBar_Scale_Epsilon)
		{
			fScale = 1.0f;
		}
		m_vecItemScale[n] = fScale;

		if (fabs(fScale - fScaleOld) > DockBar_Scale_Epsilon) bChanged = true;
	}

	return bChanged;
}

void CDUIDockBarCtrl::OnDockAttributeChanged(bool bResetScale)
{
	if (bResetScale)
	{
		StopEffect();
		ResetItemScale();
	}
	else
	{
		//only the padding is changed, the item size is still valid
		ResetItemSet();
	}

	NeedRefreshView();
	Invalidate();

	return;
}
