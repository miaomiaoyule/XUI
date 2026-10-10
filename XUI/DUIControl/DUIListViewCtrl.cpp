#include "StdAfx.h"
#include "DUIListViewCtrl.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateControl(CDUIListViewCtrl)
MMImplement_ClassName(CDUIListViewCtrl)

CDUIListViewCtrl::CDUIListViewCtrl(void)
{
	//model
	m_AttributeItemModel.SetDuiType(DuiType_ModelListItem);

	//list view type
	static tagDuiCombox AttriCombox;
	if (AttriCombox.vecItem.empty())
	{
		AttriCombox.vecItem.push_back({ ListView_List, _T("List") });
		AttriCombox.vecItem.push_back({ ListView_TileH, _T("TileH") });
		AttriCombox.vecItem.push_back({ ListView_TileV, _T("TileV") });
	}

	m_AttributeListViewType.SetCombox(AttriCombox);
	m_AttributeListViewType.SelectItem(ListView_List);

	return;
}

CDUIListViewCtrl::~CDUIListViewCtrl(void)
{
	//owner
	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUIListItemCtrl *pItem = GetChildAt(n);
		if (NULL == pItem) continue;

		pItem->SetOwner(NULL);
	}

	MMSafeDelete(m_pListHeader)

	return;
}

bool CDUIListViewCtrl::RegisterControlListen(IDuiControlListen *pIDuiControlListen)
{
	if (NULL == pIDuiControlListen || false == __super::RegisterControlListen(pIDuiControlListen)) return false;

	if (m_pListHeader)
	{
		m_pListHeader->RegisterControlListen(pIDuiControlListen);
	}

	return true;
}

bool CDUIListViewCtrl::UnRegisterControlListen(IDuiControlListen *pIDuiControlListen)
{
	if (NULL == pIDuiControlListen || false == __super::UnRegisterControlListen(pIDuiControlListen)) return false;

	if (m_pListHeader)
	{
		m_pListHeader->UnRegisterControlListen(pIDuiControlListen);
	}

	return true;
}

void CDUIListViewCtrl::OnDpiChanged(int nScalePre)
{
	__super::OnDpiChanged(nScalePre);

	if (m_pListHeader)
	{
		m_pListHeader->OnDpiChanged(nScalePre);
	}

	return;
}

bool CDUIListViewCtrl::OnAttributeChange(CDUIAttributeObject *pAttributeObj)
{
	if (__super::OnAttributeChange(pAttributeObj)) return true;

	if (pAttributeObj == &m_AttributeItemModel || pAttributeObj == &m_AttributeItemModelInitCount)
	{
		InitChild();

		return true;
	}
	if (pAttributeObj == &m_AttributeUseListHeader)
	{
		SetUseListHeader(IsUseListHeader() && ListView_List == GetListViewType());

		return true;
	}
	if (pAttributeObj == &m_AttributeListViewType)
	{
		SwitchListViewType(GetListViewType());

		return true;
	}
	if (pAttributeObj == &m_AttributeSelectIconVisible)
	{
		SetSelectIconVisible(IsSelectIconVisible());

		return true;
	}
	if (pAttributeObj == &m_AttributeSelectIconLeftPadding)
	{
		SetSelectIconLeftPadding(GetSelectIconLeftPadding());

		return true;
	}
	if (pAttributeObj == &m_AttributeSelectIconFixedWidth)
	{
		SetSelectIconFixedWidth(GetSelectIconFixedWidth());

		return true;
	}
	if (pAttributeObj == &m_AttributeSelectIconFixedHeight)
	{
		SetSelectIconFixedHeight(GetSelectIconFixedHeight());

		return true;
	}
	if (pAttributeObj == &m_AttributeImageSelectIconNormal)
	{
		SetImageSectionSelIconNormal(GetImageSectionSelIconNormal());

		return true;
	}
	if (pAttributeObj == &m_AttributeImageSelectIconHot)
	{
		SetImageSectionSelIconHot(GetImageSectionSelIconHot());

		return true;
	}
	if (pAttributeObj == &m_AttributeImageSelectIconPushed)
	{
		SetImageSectionSelIconPushed(GetImageSectionSelIconPushed());

		return true;
	}
	if (pAttributeObj == &m_AttributeImageSelectIconSelNormal)
	{
		SetImageSectionSelIconSelNormal(GetImageSectionSelIconSelNormal());

		return true;
	}
	if (pAttributeObj == &m_AttributeImageSelectIconSelHot)
	{
		SetImageSectionSelIconSelHot(GetImageSectionSelIconSelHot());

		return true;
	}
	if (pAttributeObj == &m_AttributeImageSelectIconSelPushed)
	{
		SetImageSectionSelIconSelPushed(GetImageSectionSelIconSelPushed());

		return true;
	}

	return false;
}

LPVOID CDUIListViewCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIListViewCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIListViewCtrl::GetDescribe() const
{
	return Dui_Ctrl_ListView;
}

CDUIListViewCtrl * CDUIListViewCtrl::Clone(bool bIncludeChild, bool bRefreshCtrlID)
{
	MMInterfaceHelper(CDUIListViewCtrl, __super::Clone(bIncludeChild, bRefreshCtrlID), pListViewClone);
	if (NULL == pListViewClone) return NULL;

	MMSafeDelete(pListViewClone->m_pListHeader);

	if (m_pListHeader)
	{
		pListViewClone->InsertChild(m_pListHeader->Clone(true, bRefreshCtrlID));
	}

	return pListViewClone;
}

UINT CDUIListViewCtrl::InitCtrlID()
{
	__super::InitCtrlID();

	if (m_pListHeader)
	{
		m_pListHeader->InitCtrlID();
	}

	return GetCtrlID();
}

void CDUIListViewCtrl::RefreshCtrlID(bool bSelfSingle)
{
	__super::RefreshCtrlID(bSelfSingle);

	if (bSelfSingle) return;

	if (m_pListHeader)
	{
		m_pListHeader->RefreshCtrlID();
	}

	return;
}

bool CDUIListViewCtrl::SetWndOwner(CDUIWnd *pWndOwner)
{
	if (false == __super::SetWndOwner(pWndOwner)) return false;

	if (m_pListHeader)
	{
		m_pListHeader->SetWndOwner(m_pWndOwner);
	}

	return true;
}

void CDUIListViewCtrl::SetVisible(bool bVisible)
{
	__super::SetVisible(bVisible);

	if (m_pListHeader)
	{
		m_pListHeader->SetInternVisible(IsVisible());
	}

	return;
}

void CDUIListViewCtrl::SetAnimateDrag(bool bAnimateDrag)
{
	__super::SetAnimateDrag(bAnimateDrag);

	if (m_pAnimateDrag)
	{
		m_pAnimateDrag->SetDragType(ListView_List == GetListViewType() ? DragType_V : DragType_HV);
	}

	return;
}

void CDUIListViewCtrl::NeedRefreshView()
{
	return __super::NeedRefreshView();
}

void CDUIListViewCtrl::RefreshView()
{
	//animate draging
	if (IsAnimateDraging()) return;

	//refresh
	enDuiListViewType ListViewType = GetListViewType();
	if (ListView_List == ListViewType)
	{
		RefreshViewOnListType();
	}
	else if (ListView_TileH == ListViewType)
	{
		RefreshViewOnTileTypeH();
	}
	else
	{
		RefreshViewOnTileTypeV();
	}

	//auto width height
	if (IsAutoCalcWidth()
		&& GetTotalRange().cx != GetFixedWidth()
		&& DuiDpiScaleBackCtrl(GetTotalRange().cx) != DuiDpiScaleBackCtrl(GetFixedWidth()))
	{
		SetFixedWidth(DuiDpiScaleBackCtrl(GetTotalRange().cx));
	}
	if (IsAutoCalcHeight() 
		&& GetTotalRange().cy != GetFixedHeight()
		&& DuiDpiScaleBackCtrl(GetTotalRange().cy) != DuiDpiScaleBackCtrl(GetFixedHeight()))
	{
		SetFixedHeight(DuiDpiScaleBackCtrl(GetTotalRange().cy));
	}

	CDUIControlBase::RefreshView();

	return;
}

bool CDUIListViewCtrl::DoPaint(HDC hDC, bool bGenerateBmp)
{
	if (false == CDUIControlBase::DoPaint(hDC, bGenerateBmp)) return false;

	//child
	CDUIRect rcItemRange = GetLayoutRangeOfItem();
	IntersectRect(&rcItemRange, &rcItemRange, &m_rcPaint);
	CDUIListItemCtrl *pItemFocus = NULL;
	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUIListItemCtrl *pItem = GetChildAt(n);
		if (NULL == pItem || false == pItem->IsVisible()) continue;
		if (false == pItem->IsFloat() && (n < m_nFirstShowIndex || n > m_nLastShowIndex)) continue;

		//focus
		if (m_pWndOwner && m_pWndOwner->GetFocusControl() && m_pWndOwner->GetFocusControl()->GetOwnerModelCtrl() == pItem)
		{
			pItemFocus = pItem;

			continue;
		}

		pItem->OnDraw(hDC, rcItemRange, bGenerateBmp);
	}

	//header
	if (IsUseListHeader() && m_pListHeader && m_pListHeader->IsVisible())
	{
		m_pListHeader->OnDraw(hDC, m_rcPaint, bGenerateBmp);
	}

	//focus
	if (pItemFocus)
	{
		pItemFocus->OnDraw(hDC, rcItemRange, bGenerateBmp);
	}

	//darg topmost
	if (m_pAnimateDrag && m_pAnimateDrag->GetDragingCtrl())
	{
		m_pAnimateDrag->GetDragingCtrl()->OnDraw(hDC, m_rcPaint, bGenerateBmp);
	}

	//scrollbar
	if (m_pHorizScrollBarCtrl && m_pHorizScrollBarCtrl->IsVisible())
	{
		m_pHorizScrollBarCtrl->OnDraw(hDC, m_rcPaint, bGenerateBmp);
	}
	if (m_pVertScrollBarCtrl && m_pVertScrollBarCtrl->IsVisible())
	{
		m_pVertScrollBarCtrl->OnDraw(hDC, m_rcPaint, bGenerateBmp);
	}

	//sel
	PaintMouseDragSel(hDC);

	return true;
}

tagDuiListInfo CDUIListViewCtrl::GetListInfo()
{
	tagDuiListInfo ListInfo;
	ListInfo.pAttributeItemTextStyleNormal = &m_AttributeItemTextStyleNormal;
	ListInfo.pAttributeItemTextStyleHot = &m_AttributeItemTextStyleHot;
	ListInfo.pAttributeItemTextStyleSelNormal = &m_AttributeItemTextStyleSelNormal;
	ListInfo.pAttributeItemTextStyleSelHot = &m_AttributeItemTextStyleSelHot;
	ListInfo.pAttributeItemTextStyleDisabled = &m_AttributeItemTextStyleDisabled;

	ListInfo.pAttributeItemStatusColorNormal = &m_AttributeColorItemStatusNormal;
	ListInfo.pAttributeItemStatusColorHot = &m_AttributeColorItemStatusHot;
	ListInfo.pAttributeItemStatusColorSelNormal = &m_AttributeColorItemStatusSelNormal;
	ListInfo.pAttributeItemStatusColorSelHot = &m_AttributeColorItemStatusSelHot;
	ListInfo.pAttributeItemStatusColorDisabled = &m_AttributeColorItemStatusDisabled;

	ListInfo.pAttributeItemStatusImageNormal = &m_AttributeImageItemStatusNormal;
	ListInfo.pAttributeItemStatusImageHot = &m_AttributeImageItemStatusHot;
	ListInfo.pAttributeItemStatusImageSelNormal = &m_AttributeImageItemStatusSelNormal;
	ListInfo.pAttributeItemStatusImageSelHot = &m_AttributeImageItemStatusSelHot;
	ListInfo.pAttributeItemStatusImageDisabled = &m_AttributeImageItemStatusDisabled;

	ListInfo.pAttributeColorLine = &m_AttributeColorLine;

	return ListInfo;
}

CDUISize CDUIListViewCtrl::GetTotalRange()
{
	return m_szTotalRange;
}

bool CDUIListViewCtrl::IsUseListHeader()
{
	return m_AttributeUseListHeader.GetValue();
}

void CDUIListViewCtrl::SetUseListHeader(bool bUseListHeader)
{
	if ((bUseListHeader && ListView_List != GetListViewType())) return;

	m_AttributeUseListHeader.SetValue(bUseListHeader);

	if (m_pListHeader)
	{
		m_pListHeader->SetVisible(bUseListHeader);
	}

	NeedRefreshView();

	return;
}

enDuiListViewType CDUIListViewCtrl::GetListViewType()
{
	return (enDuiListViewType)m_AttributeListViewType.GetCurSelItem();
}

void CDUIListViewCtrl::SwitchListViewType(enDuiListViewType ListViewType)
{
	m_AttributeListViewType.SelectItem(ListViewType);
	ListViewType = GetListViewType();
	DuiInitAttriVisible(m_AttributeAutoCalcWidth, ListView_TileV == ListViewType);
	DuiInitAttriVisible(m_AttributeAutoCalcHeight, ListView_List == ListViewType || ListView_TileH == ListViewType);

	//tile
	if (ListView_List != ListViewType)
	{
		SetUseListHeader(false);

		for (int n = 0; n < GetChildCount(); n++)
		{
			CDUIListItemCtrl *pItem = GetChildAt(n);
			if (NULL == pItem) continue;

			pItem->SetPaddingB(0);
			pItem->SetSelectIconVisible(false);
		}
	}

	//list
	if (ListView_List == ListViewType)
	{
		for (int n = 0; n < GetChildCount(); n++)
		{
			CDUIListItemCtrl *pItem = GetChildAt(n);
			if (NULL == pItem) continue;

			pItem->SetSelectIconVisible(IsSelectIconVisible());
		}
	}

	//init
	if (m_pAnimateDrag)
	{
		m_pAnimateDrag->SetDragType(ListView_List == ListViewType ? DragType_V : DragType_HV);
	}

	//notify
	if (m_pWndOwner)
	{
		m_pWndOwner->SendNotify(this, DuiNotify_SwitchListViewType);
	}

	NeedRefreshView();

	return;
}

tagDuiImageSection CDUIListViewCtrl::GetImageSectionMouseDragSel()
{
	return m_AttributeImageMouseDragSel.GetImageSection();
}

void CDUIListViewCtrl::SetImageSectionMouseDragSel(const tagDuiImageSection &ImageSection)
{
	if (ImageSection == GetImageSectionMouseDragSel()) return;

	m_AttributeImageMouseDragSel.SetImageSection(ImageSection);

	return;
}

CDUIListHeaderCtrl * CDUIListViewCtrl::GetListHeader() const
{
	return m_pListHeader;
}

int CDUIListViewCtrl::GetSwitchListItemHeight()
{
	return DuiDpiScaleCtrl(m_AttributeSwitchListItemHeight.GetValue());
}

void CDUIListViewCtrl::SetSwitchListItemHeight(int nHeight)
{
	if (nHeight <= 0 || DuiDpiScaleCtrl(nHeight) == GetSwitchListItemHeight()) return;

	m_AttributeSwitchListItemHeight.SetValue(nHeight);

	return;
}

bool CDUIListViewCtrl::IsSelectIconVisible()
{
	return m_AttributeSelectIconVisible.GetValue();
}

void CDUIListViewCtrl::SetSelectIconVisible(bool bVisible)
{
	if (ListView_List != GetListViewType()) return;

#ifndef DUI_DESIGN
	if (bVisible == IsSelectIconVisible()) return;
#endif

	m_AttributeSelectIconVisible.SetValue(bVisible);

	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUIListItemCtrl *pItem = GetChildAt(n);
		if (NULL == pItem) continue;

		pItem->SetSelectIconVisible(IsSelectIconVisible());
	}

	//header
	if (m_pListHeader)
	{
		m_pListHeader->OnVisibleChangedSelectIcon();
	}

	return;
}

bool CDUIListViewCtrl::IsSelectDbClick()
{
	return m_AttributeSelectDbClick.GetValue();
}

void CDUIListViewCtrl::SetSelectDbClick(bool bDbClick)
{
	if (bDbClick == IsSelectDbClick()) return;

	m_AttributeSelectDbClick.SetValue(bDbClick);

	return;
}

int CDUIListViewCtrl::GetSelectIconLeftPadding()
{
	return m_AttributeSelectIconLeftPadding.GetValue();
}

void CDUIListViewCtrl::SetSelectIconLeftPadding(int nLeft)
{
#ifndef DUI_DESIGN
	if (nLeft == GetSelectIconLeftPadding()) return;
#endif

	m_AttributeSelectIconLeftPadding.SetValue(nLeft);

	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUIListItemCtrl *pItem = GetChildAt(n);
		if (NULL == pItem) continue;

		pItem->SetSelectIconLeftPadding(GetSelectIconLeftPadding());
	}

	return;
}

int CDUIListViewCtrl::GetSelectIconFixedWidth()
{
	return m_AttributeSelectIconFixedWidth.GetValue();
}

void CDUIListViewCtrl::SetSelectIconFixedWidth(int nWidth)
{
#ifndef DUI_DESIGN
	if (nWidth == GetSelectIconFixedWidth()) return;
#endif

	m_AttributeSelectIconFixedWidth.SetValue(nWidth);

	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUIListItemCtrl *pItem = GetChildAt(n);
		if (NULL == pItem) continue;

		pItem->SetSelectIconFixedWidth(GetSelectIconFixedWidth());
	}

	return;
}

int CDUIListViewCtrl::GetSelectIconFixedHeight()
{
	return m_AttributeSelectIconFixedHeight.GetValue();
}

void CDUIListViewCtrl::SetSelectIconFixedHeight(int nHeight)
{
#ifndef DUI_DESIGN
	if (nHeight == GetSelectIconFixedHeight()) return;
#endif

	m_AttributeSelectIconFixedHeight.SetValue(nHeight);

	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUIListItemCtrl *pItem = GetChildAt(n);
		if (NULL == pItem) continue;

		pItem->SetSelectIconFixedHeight(GetSelectIconFixedHeight());
	}

	return;
}

tagDuiImageSection CDUIListViewCtrl::GetImageSectionSelIconNormal()
{
	return m_AttributeImageSelectIconNormal.GetImageSection();
}

tagDuiImageSection CDUIListViewCtrl::GetImageSectionSelIconHot()
{
	return m_AttributeImageSelectIconHot.GetImageSection();
}

tagDuiImageSection CDUIListViewCtrl::GetImageSectionSelIconPushed()
{
	return m_AttributeImageSelectIconPushed.GetImageSection();
}

tagDuiImageSection CDUIListViewCtrl::GetImageSectionSelIconSelNormal()
{
	return m_AttributeImageSelectIconSelNormal.GetImageSection();
}

tagDuiImageSection CDUIListViewCtrl::GetImageSectionSelIconSelHot()
{
	return m_AttributeImageSelectIconSelHot.GetImageSection();
}

tagDuiImageSection CDUIListViewCtrl::GetImageSectionSelIconSelPushed()
{
	return m_AttributeImageSelectIconSelPushed.GetImageSection();
}

void CDUIListViewCtrl::SetImageSectionSelIconNormal(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageSelectIconNormal.SetImageSection(ImageSection);

	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUIListItemCtrl *pItem = GetChildAt(n);
		if (NULL == pItem) continue;

		pItem->SetImageSectionSelIconNormal(GetImageSectionSelIconNormal());
	}

	return;
}

void CDUIListViewCtrl::SetImageSectionSelIconHot(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageSelectIconHot.SetImageSection(ImageSection);

	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUIListItemCtrl *pItem = GetChildAt(n);
		if (NULL == pItem) continue;

		pItem->SetImageSectionSelIconHot(GetImageSectionSelIconHot());
	}

	return;
}

void CDUIListViewCtrl::SetImageSectionSelIconPushed(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageSelectIconPushed.SetImageSection(ImageSection);

	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUIListItemCtrl *pItem = GetChildAt(n);
		if (NULL == pItem) continue;

		pItem->SetImageSectionSelIconPushed(GetImageSectionSelIconPushed());
	}

	return;
}

void CDUIListViewCtrl::SetImageSectionSelIconSelNormal(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageSelectIconSelNormal.SetImageSection(ImageSection);

	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUIListItemCtrl *pItem = GetChildAt(n);
		if (NULL == pItem) continue;

		pItem->SetImageSectionSelIconSelNormal(GetImageSectionSelIconSelNormal());
	}

	return;
}

void CDUIListViewCtrl::SetImageSectionSelIconSelHot(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageSelectIconSelHot.SetImageSection(ImageSection);

	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUIListItemCtrl *pItem = GetChildAt(n);
		if (NULL == pItem) continue;

		pItem->SetImageSectionSelIconSelHot(GetImageSectionSelIconSelHot());
	}

	return;
}

void CDUIListViewCtrl::SetImageSectionSelIconSelPushed(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageSelectIconSelPushed.SetImageSection(ImageSection);

	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUIListItemCtrl *pItem = GetChildAt(n);
		if (NULL == pItem) continue;

		pItem->SetImageSectionSelIconSelPushed(GetImageSectionSelIconSelPushed());
	}

	return;
}

CDUISize CDUIListViewCtrl::GetSwitchTileItemSize()
{
	return DuiDpiScaleCtrl(m_AttributeSwitchTileItemSize.GetValue());
}

void CDUIListViewCtrl::SetSwitchTileItemSize(CDUISize szItem)
{
	if (szItem.cx <= 0 || szItem.cy <= 0 || DuiDpiScaleCtrl(szItem) == GetSwitchTileItemSize()) return;

	m_AttributeSwitchTileItemSize.SetValue(szItem);

	NeedRefreshView();

	return;
}

bool CDUIListViewCtrl::IsAutoCalcItemWidth()
{
	return m_AttributeAutoCalcItemWidth.GetValue();
}

void CDUIListViewCtrl::SetAutoCalcItemWidth(bool bAuto)
{
	if (bAuto == IsAutoCalcItemWidth()) return;

	m_AttributeAutoCalcItemWidth.SetValue(bAuto);

	NeedRefreshView();

	return;
}

bool CDUIListViewCtrl::IsAutoCalcItemHeight()
{
	return m_AttributeAutoCalcItemHeight.GetValue();
}

void CDUIListViewCtrl::SetAutoCalcItemHeight(bool bAuto)
{
	if (bAuto == IsAutoCalcItemHeight()) return;

	m_AttributeAutoCalcItemHeight.SetValue(bAuto);

	NeedRefreshView();

	return;
}

bool CDUIListViewCtrl::IsLeftFromRange()
{
	return m_AttributeLeftFromRange.GetValue();
}

void CDUIListViewCtrl::SetLeftFromRange(bool bLeftFromRange)
{
	if (bLeftFromRange == IsLeftFromRange()) return;

	m_AttributeLeftFromRange.SetValue(bLeftFromRange);

	NeedRefreshView();

	return;
}

bool CDUIListViewCtrl::IsTopFromRange()
{
	return m_AttributeTopFromRange.GetValue();
}

void CDUIListViewCtrl::SetTopFromRange(bool bTopFromRange)
{
	if (bTopFromRange == IsTopFromRange()) return;

	m_AttributeTopFromRange.SetValue(bTopFromRange);

	NeedRefreshView();

	return;
}

bool CDUIListViewCtrl::IsScrollSelect()
{
	return m_AttributeScrollSelect.GetValue();
}

void CDUIListViewCtrl::SetScrollSelect(bool bScrollSelect)
{
	m_AttributeScrollSelect.SetValue(bScrollSelect);

	return;
}

bool CDUIListViewCtrl::IsMultiSelect()
{
	return m_AttributeMultiSel.GetValue();
}

void CDUIListViewCtrl::SetMultiSelect(bool bMultiSel)
{
	if (bMultiSel == IsMultiSelect()) return;

	m_AttributeMultiSel.SetValue(bMultiSel);

	if (false == IsMultiSelect())
	{
		UnSelectAllItems();
	}

	return;
}

int CDUIListViewCtrl::GetSelectItemCount()
{
	return m_vecSelItem.size();
}

int CDUIListViewCtrl::GetCurSel() const
{
	return m_nCurSel;
}

int CDUIListViewCtrl::GetFirstSelect() const
{
	int nIndex = -1;

	for (auto pItem : m_vecSelItem)
	{
		if (NULL == pItem) continue;

		if (-1 == nIndex || pItem->GetIndex() < nIndex)
		{
			nIndex = pItem->GetIndex();
		}
	}

	return nIndex;
}

int CDUIListViewCtrl::GetNextSelItem(int nItem) const
{
	if (m_vecSelItem.size() <= 0) return -1;

	CDUIListItemCtrl *pItem = GetChildAt(nItem);
	auto FindIt = std::find(m_vecSelItem.begin(), m_vecSelItem.end(), pItem);
	if (FindIt == m_vecSelItem.end()) return GetFirstSelect();

	int nIndex = -1;
	for (auto pListItemSel : m_vecSelItem)
	{
		if (NULL == pListItemSel
			|| pListItemSel == pItem
			|| pListItemSel->GetIndex() < pItem->GetIndex()) continue;

		nIndex = -1 == nIndex ? pListItemSel->GetIndex() : min(nIndex, pListItemSel->GetIndex());
	}

	return nIndex;
}

VecDUIListItemCtrl CDUIListViewCtrl::GetSelectItem()
{
	return m_vecSelItem;
}

bool CDUIListViewCtrl::IsSelected(int nIndex)
{
	if (nIndex == m_nCurSel) return true;

	for (auto pItem : m_vecSelItem)
	{
		if (NULL == pItem) continue;

		if (pItem->GetIndex() == nIndex) return true;
	}

	return false;
}

bool CDUIListViewCtrl::SelectItem(int nIndex, bool bTakeFocus)
{
	if (nIndex < 0)
	{
		UnSelectAllItems();

		return true;
	}

	//single
	if (false == IsMultiSelect() 
		&& (0 == (CDUIWnd::MapKeyState() & MK_CONTROL) || (GetKeyState('V') & 0x01)))
	{
		UnSelectItem(nIndex, true);
	}

	CDUIListItemCtrl *pItem = GetChildAt(nIndex);
	if (pItem == NULL || false == pItem->Select(true)) return false;
	if (IsSelected(nIndex)) return true;

	m_nCurSel = nIndex;
	m_vecSelItem.push_back(pItem);
	EnsureVisible(nIndex);

	if (bTakeFocus)
	{
		pItem->SetFocus();
	}

	//head select
	if (IsUseListHeader()
		&& IsSelectIconVisible()
		&& m_pListHeader)
	{
		m_pListHeader->OnSelectChangedListItem();
	}

	return true;
}

bool CDUIListViewCtrl::SelectItem(CDUIListItemCtrl *pItem, bool bTakeFocus)
{
	return SelectItem(GetChildIndex(pItem), bTakeFocus);
}

bool CDUIListViewCtrl::SelectItem(int nFirst, int nLast)
{
	if (false == IsMultiSelect()) return false;

	int nCurSel = GetCurSel();

	//sel all
	if (-1 == nFirst && -1 == nLast)
	{
		SelectAllItems();

		m_nCurSel = -1 != nCurSel ? nCurSel : m_nCurSel;

		return true;
	}

	//first-last
	if (nFirst > nLast) swap(nFirst, nLast);
	for (int nIndex = 0; nIndex < GetChildCount(); nIndex++)
	{
		if (nFirst <= nIndex && nIndex <= nLast)
		{
			SelectItem(nIndex);
		}
		else
		{
			UnSelectItem(nIndex);
		}
	}

	//cursel
	if (IsSelected(nCurSel))
	{
		m_nCurSel = nCurSel;
	}

	return true;
}

bool CDUIListViewCtrl::UnSelectItem(int nIndex, bool bOthers)
{
	//other
	if (bOthers)
	{
		auto vecSelItem = m_vecSelItem;
		for (auto pSelListItem : vecSelItem)
		{
			if (NULL == pSelListItem || pSelListItem->GetIndex() == nIndex) continue;

			pSelListItem->Select(false);
		}
		if (-1 == m_nCurSel)
		{
			m_nCurSel = GetMaxSelItemIndex();
		}

		return true;
	}

	if (nIndex < 0) return false;
	if (false == IsSelected(nIndex)) return true;

	CDUIListItemCtrl *pItem = GetChildAt(nIndex);
	if (NULL == pItem) return false;
	if (false == pItem->Select(false)) return false;

	m_vecSelItem.erase(std::remove(m_vecSelItem.begin(), m_vecSelItem.end(), pItem), m_vecSelItem.end());

	if (m_nCurSel == nIndex)
	{
		m_nCurSel = GetMaxSelItemIndex();
	}

	//head select
	if (IsUseListHeader()
		&& IsSelectIconVisible()
		&& m_pListHeader)
	{
		m_pListHeader->OnSelectChangedListItem();
	}

	return true;
}

void CDUIListViewCtrl::SelectAllItems()
{
	for (int i = 0; i < GetChildCount(); ++i)
	{
		CDUIListItemCtrl *pItem = GetChildAt(i);
		if (NULL == pItem) continue;
		if (false == pItem->Select(true)) continue;
	}

	return;
}

void CDUIListViewCtrl::UnSelectAllItems()
{
	auto vecSelItem = m_vecSelItem;
	for (auto pSelListItem : vecSelItem)
	{
		if (NULL == pSelListItem) continue;
		if (false == pSelListItem->Select(false)) continue;
	}

	m_vecSelItem.clear();
	m_nCurSel = -1;

	return;
}

int CDUIListViewCtrl::FindSelectable(WORD wKey)
{
	enDuiListViewType ListViewType = GetListViewType();
	if (ListView_List == ListViewType)
	{
		if (VK_UP == wKey)
		{
			return FindSelectableOnListType(GetCurSel(), false);
		}
		if (VK_DOWN == wKey)
		{
			return FindSelectableOnListType(GetCurSel(), true);
		}

		return -1;
	}
	if (ListView_TileH == ListViewType)
	{
		return FindSelectableOnTileTypeH(wKey);
	}
	if (ListView_TileV == ListViewType)
	{
		return FindSelectableOnTileTypeV(wKey);
	}

	return -1;
}

CMMString CDUIListViewCtrl::GetItemModel()
{
	return m_AttributeItemModel.GetCurSelDescribe();
}

bool CDUIListViewCtrl::SetItemModel(CMMString strModel)
{
	if (strModel == m_AttributeItemModel.GetCurSelDescribe()) return false;

	m_AttributeItemModel.SelectItem(strModel);

	return true;
}

int CDUIListViewCtrl::GetItemInitCount()
{
	return m_AttributeItemModelInitCount.GetValue();
}

void CDUIListViewCtrl::SetItemInitCount(int nCount) 
{
	if (nCount == GetItemInitCount()) return;

	m_AttributeItemModelInitCount.SetValue(nCount);

	return;
}

void CDUIListViewCtrl::InitChild()
{
	int nCount = m_AttributeItemModelInitCount.GetValue();
	if (nCount < 0) return;

	RemoveAll();
	InsertChild(nCount);

	return;
}

bool CDUIListViewCtrl::InsertChild(CDUIControlBase *pChild, int nPos)
{
	if (NULL == pChild) return false;

	//header
	if (NULL == m_pListHeader && MMInterfaceHelper(CDUIListHeaderCtrl, pChild))
	{
		m_pListHeader = static_cast<CDUIListHeaderCtrl*>(pChild);
		m_pListHeader->SetParent(this);
		m_pListHeader->SetOwner(this);

		if (m_pWndOwner)
		{
			m_pWndOwner->InitControls(pChild);
		}
		if (IsVisible())
		{
			NeedRefreshView();
		}

		//control notify
		CDUIGlobal::PerformNotifyChildAdd(this, pChild);

		return true;
	}

	//list item
	MMInterfaceHelper(CDUIListItemCtrl, pChild, pItem);
	if (pItem)
	{
		if (false == __super::InsertChild(pChild, nPos)) return false;

		//sub insert
		enDuiListViewType ListViewType = GetListViewType();
		if (ListView_List == ListViewType)
		{
			InsertChildOnListType(pItem);
		}
		else if (ListView_TileH == ListViewType)
		{
			InsertChildOnTileTypeH(pItem);
		}
		else
		{
			InsertChildOnTileTypeV(pItem);
		}
		do
		{
			if (false == IsInitComplete()) break;

			pItem->SetSelectIconVisible(ListView_List == ListViewType && IsSelectIconVisible());
			pItem->SetSelectIconLeftPadding(GetSelectIconLeftPadding());
			pItem->SetSelectIconFixedWidth(GetSelectIconFixedWidth());
			pItem->SetSelectIconFixedHeight(GetSelectIconFixedHeight());
			pItem->SetImageSectionSelIconNormal(GetImageSectionSelIconNormal());
			pItem->SetImageSectionSelIconHot(GetImageSectionSelIconHot());
			pItem->SetImageSectionSelIconPushed(GetImageSectionSelIconPushed());
			pItem->SetImageSectionSelIconSelNormal(GetImageSectionSelIconSelNormal());
			pItem->SetImageSectionSelIconSelHot(GetImageSectionSelIconSelHot());
			pItem->SetImageSectionSelIconSelPushed(GetImageSectionSelIconSelPushed());

		} while (false);

		//index
		if (nPos == -1 || nPos >= GetChildCount() - 1)
		{
			pItem->SetIndex(GetChildCount() - 1);
		}
		else
		{
			pItem->SetIndex(nPos);

			for (int i = nPos + 1; i < GetChildCount(); ++i)
			{
				CDUIListItemCtrl *pItem = GetChildAt(i);
				if (NULL == pItem) continue;

				pItem->SetIndex(i);
			}

			if (m_nCurSel >= nPos) m_nCurSel += 1;
		}

		//select
		if (pItem->IsSelected())
		{
			m_vecSelItem.push_back(pItem);

			if (-1 == m_nCurSel) m_nCurSel = pItem->GetIndex();
		}

		pItem->SetOwner(this);

		return true;
	}

	//header item
	if (m_pListHeader && m_pListHeader->InsertChild(pChild, nPos))
	{
		return true;
	}

	//scroll bar
	if ((NULL == m_pHorizScrollBarCtrl || NULL == m_pVertScrollBarCtrl)
		&& MMInterfaceHelper(CDUIScrollBarCtrl, pChild))
	{
		return __super::InsertChild(pChild, nPos);
	}

	return false;
}

bool CDUIListViewCtrl::InsertChild(int nItemModelCount, int nPos)
{
	if (nItemModelCount <= 0) return false;

	for (int n = 0; n < nItemModelCount; n++)
	{
		CDUIControlBase *pItem = CDUIGlobal::GetInstance()->LoadDui(GetItemModel(), NULL);
		NULL == pItem ? pItem = ProductItemDefault() : NULL;
		if (NULL == pItem)
		{
			assert(false);
			return false;
		}

		if (false == InsertChild(pItem, nPos))
		{
			assert(false);
			return false;
		}
	}

	return true;
}

int CDUIListViewCtrl::GetChildIndex(CDUIControlBase *pChild) const
{
	return __super::GetChildIndex(pChild);
}

int CDUIListViewCtrl::GetHeaderChildIndex(CDUIControlBase *pChild) const
{
	if (NULL == m_pListHeader) return -1;

	return m_pListHeader->GetChildIndex(pChild);
}

bool CDUIListViewCtrl::SwapChild(int nIndexFirst, int nIndexNext, bool bNeedRefreshView)
{
	CDUIListItemCtrl *pFirstItem = GetChildAt(nIndexFirst);
	CDUIListItemCtrl *pNextItem = GetChildAt(nIndexNext);

	if (NULL == pFirstItem || NULL == pNextItem) return false;
	if (false == __super::SwapChild(nIndexFirst, nIndexNext, bNeedRefreshView)) return false;

	pFirstItem->SetIndex(nIndexNext);
	pNextItem->SetIndex(nIndexFirst);
	m_nCurSel == nIndexFirst ? m_nCurSel = nIndexNext : (m_nCurSel == nIndexNext ? m_nCurSel = nIndexFirst : 0);

	return true;
}

CDUIListItemCtrl * CDUIListViewCtrl::GetChildAt(int nIndex) const
{
	return static_cast<CDUIListItemCtrl*>(CDUIContainerCtrl::GetChildAt(nIndex));
}

CDUIListHeaderItemCtrl * CDUIListViewCtrl::GetHeaderChildAt(int nIndex) const
{
	if (NULL == m_pListHeader) return NULL;

	return m_pListHeader->GetChildAt(nIndex);
}

bool CDUIListViewCtrl::SetChildIndex(CDUIControlBase *pControl, int nIndex)
{
	MMInterfaceHelper(CDUIListItemCtrl, pControl, pItem);
	if (NULL == pItem) return false;

	int nOrginIndex = GetChildIndex(pItem);
	if (nOrginIndex < 0) return false;
	if (nOrginIndex == nIndex) return true;
	if (false == __super::SetChildIndex(pItem, nIndex)) return false;

	//刷新Index
	int iMinIndex = min(nOrginIndex, nIndex);
	int iMaxIndex = max(nOrginIndex, nIndex);
	for (int i = iMinIndex; i < iMaxIndex + 1; ++i)
	{
		CDUIListItemCtrl *pItem = GetChildAt(i);
		if (pItem) pItem->SetIndex(i);
	}

	if (m_nCurSel == nOrginIndex && pItem->IsSelected()) m_nCurSel = pItem->GetIndex();

	return true;
}

bool CDUIListViewCtrl::SetHeaderChildIndex(CDUIControlBase *pControl, int nIndex)
{
	if (NULL == m_pListHeader) return false;

	return m_pListHeader->SetChildIndex(pControl, nIndex);
}

int CDUIListViewCtrl::GetChildCount() const
{
	return __super::GetChildCount();
}

bool CDUIListViewCtrl::VerifyChild(CDUIControlBase *pControl)
{
	if (NULL == pControl) return false;

	if (m_pListHeader && (m_pListHeader == pControl || m_pListHeader->VerifyChild(pControl))) return true;

	return __super::VerifyChild(pControl);
}

int CDUIListViewCtrl::GetHeaderChildCount() const
{
	if (NULL == m_pListHeader) return 0;

	return m_pListHeader->GetChildCount();
}

int CDUIListViewCtrl::GetRowCount()
{
	return ListView_List == GetListViewType() ? GetChildCount() : m_nRow;
}

int CDUIListViewCtrl::GetColumnCount()
{
	if (ListView_List != GetListViewType()) return m_nColumn;

	if (false == IsUseListHeader() || NULL == m_pListHeader || false == m_pListHeader->IsVisible()) return 0;

	return m_pListHeader->GetColumnCount();
}

CDUIRect CDUIListViewCtrl::GetColumnRect(int nIndex) const
{
	if (NULL == m_pListHeader) return CDUIRect();

	return m_pListHeader->GetColumnRect(nIndex);
}

int CDUIListViewCtrl::GetChildPaddingH()
{
	return __super::GetChildPaddingH();
}

int CDUIListViewCtrl::GetChildPaddingV()
{
	return __super::GetChildPaddingV();
}

CDUISize CDUIListViewCtrl::GetChildPadding()
{
	return __super::GetChildPadding();
}

void CDUIListViewCtrl::SetChildPadding(int nChildPaddingH, int nChildPaddingV)
{
	return __super::SetChildPadding(nChildPaddingH, nChildPaddingV);
}

CDUISize CDUIListViewCtrl::GetShowIndex()
{
	return { m_nFirstShowIndex, m_nLastShowIndex };
}

bool CDUIListViewCtrl::Remove(CDUIControlBase *pControl)
{
	if (NULL == pControl) return false;

	//header item
	if (MMInterfaceHelper(CDUIListHeaderItemCtrl, pControl))
	{
		if (m_pListHeader) return m_pListHeader->Remove(pControl);

		return false;
	}

	//listitem
	int nIndex = GetChildIndex(pControl);
	return RemoveAt(nIndex);
}

bool CDUIListViewCtrl::RemoveAt(int nIndex)
{
	CDUIListItemCtrl *pItem = GetChildAt(nIndex);
	if (NULL == pItem) return false;

	pItem->SetOwner(NULL);

	if (false == __super::RemoveAt(nIndex)) return false;

	//index
	for (int i = nIndex; i < GetChildCount(); ++i)
	{
		CDUIListItemCtrl *pItem = GetChildAt(i);
		if (pItem) pItem->SetIndex(i);
	}

	//select index
	m_vecSelItem.erase(std::remove(m_vecSelItem.begin(), m_vecSelItem.end(), pItem), m_vecSelItem.end());
	if (nIndex == m_nCurSel)
	{
		m_nCurSel = GetMaxSelItemIndex();
	}
	else if (m_nCurSel > nIndex)
	{
		m_nCurSel--;
	}

	return true;
}

void CDUIListViewCtrl::RemoveAll()
{
	//owner
	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUIListItemCtrl *pItem = GetChildAt(n);
		if (NULL == pItem) continue;

		pItem->SetOwner(NULL);
	}

	//remove
	__super::RemoveAll();

	m_szTotalRange.cx = m_szTotalRange.cy = 0;
	m_nFirstShowIndex = m_nLastShowIndex = -1;

	m_nVisibleCount = 0;
	m_nCurSel = -1;
	m_vecSelItem.clear();

	return;
}

int CDUIListViewCtrl::TranslateIndex(CDUIPoint pt)
{
	return __super::TranslateIndex(pt);
}

void CDUIListViewCtrl::EnsureVisible(int nIndex, bool bCenter)
{
	CDUIListItemCtrl *pItem = GetChildAt(nIndex);
	if (NULL == pItem) return;

	//target
	CDUISize szScrollPos = GetScrollPos();
	CDUISize szScrollRange = GetScrollRange();
	CDUIRect rcItemRange = GetLayoutRangeOfItem();
	enDuiListViewType ListViewType = GetListViewType();
	if (bCenter)
	{
		switch (ListViewType)
		{
			case ListView_List:
			{
				rcItemRange.top = rcItemRange.top + rcItemRange.GetHeight() / 2 - GetSwitchListItemHeight() / 2;
				rcItemRange.bottom = rcItemRange.top + GetSwitchListItemHeight();

				break;
			}
			case ListView_TileH:
			{
				rcItemRange.top = rcItemRange.top + rcItemRange.GetHeight() / 2 - GetSwitchTileItemSize().cy / 2;
				rcItemRange.bottom = rcItemRange.top + GetSwitchTileItemSize().cy;

				break;
			}
			case ListView_TileV:
			{
				rcItemRange.left = rcItemRange.left + rcItemRange.GetWidth() / 2 - GetSwitchTileItemSize().cx / 2;
				rcItemRange.right = rcItemRange.left + GetSwitchTileItemSize().cx;

				break;
			}
			default:
				break;
		}
	}

	//scroll pos
	if (m_nFirstShowIndex <= nIndex && nIndex <= m_nLastShowIndex)
	{
		RECT rcItem = pItem->GetAbsoluteRect();

		if (rcItemRange.RcInRect(rcItem)) return;

		if (rcItem.left < rcItemRange.left) szScrollPos.cx -= rcItemRange.left - rcItem.left;
		else if (rcItem.right > rcItemRange.right) szScrollPos.cx -= rcItemRange.right - rcItem.right;
		if (rcItem.top < rcItemRange.top) szScrollPos.cy -= rcItemRange.top - rcItem.top;
		else if (rcItem.bottom > rcItemRange.bottom) szScrollPos.cy -= rcItemRange.bottom - rcItem.bottom;
		SetScrollPos(szScrollPos);

		return;
	}

	szScrollPos.cx = 0;
	szScrollPos.cy = 0;

	//list
	if (ListView_List == ListViewType)
	{
		for (int n = 0; n < nIndex; n++)
		{
			CDUIListItemCtrl *pItem = GetChildAt(n);
			if (NULL == pItem || false == pItem->IsVisible()) continue;

			int nFixedHeight = pItem->GetFixedHeight();
			nFixedHeight = (nFixedHeight <= 0 ? GetSwitchListItemHeight() : nFixedHeight);
			szScrollPos.cy += nFixedHeight;
			szScrollPos.cy += GetChildPaddingV();
		}
	}
	//tileh
	else if (ListView_TileH == ListViewType && GetColumnCount() > 0)
	{
		int nRow = (nIndex + GetColumnCount() - 1) / GetColumnCount();
		for (int n = 0; n < nRow; n++)
		{
			szScrollPos.cy += GetSwitchTileItemSize().cy;
			szScrollPos.cy += GetChildPaddingV();
		}
	}
	if (ListView_List == ListViewType || ListView_TileH == ListViewType)
	{
		szScrollPos.cy = max(0, szScrollPos.cy - rcItemRange.GetHeight());
		szScrollPos.cy = bCenter ? min(szScrollRange.cy, szScrollPos.cy + rcItemRange.GetHeight() / 2) : szScrollPos.cy;
		SetScrollPos(szScrollPos);

		return;
	}

	//tilev
	if (GetRowCount() > 0)
	{
		int nColumn = (nIndex + GetRowCount() - 1) / GetRowCount();
		for (int n = 0; n < nColumn; n++)
		{
			szScrollPos.cx += GetSwitchTileItemSize().cx;
			szScrollPos.cx += GetChildPaddingH();
		}
	}

	szScrollPos.cx = max(0, szScrollPos.cx - rcItemRange.GetWidth());
	szScrollPos.cx = bCenter ? min(szScrollRange.cx, szScrollPos.cx + rcItemRange.GetWidth() / 2) : szScrollPos.cx;
	SetScrollPos(szScrollPos);

	return;
}

CDUIPoint CDUIListViewCtrl::TranslateRowColumn(int nIndex)
{
	CDUIPoint ptRowColumn(-1, -1);

	CDUIListItemCtrl *pItem = GetChildAt(nIndex);
	if (NULL == pItem || false == pItem->IsVisible()) return ptRowColumn;

	CDUIRect rcItem = pItem->GetAbsoluteRect();
	rcItem.Offset(-GetScrollPos().cx, -GetScrollPos().cy);
	rcItem.Offset(-GetAbsoluteRect().left, -GetAbsoluteRect().top);
	if (m_nHeightRow <= 0 || m_nWidthColumn <= 0) return ptRowColumn;

	ptRowColumn.x = (rcItem.bottom + m_nHeightRow - 1) / m_nHeightRow;
	ptRowColumn.y = (rcItem.right + m_nWidthColumn - 1) / m_nWidthColumn;
	ptRowColumn.x--;
	ptRowColumn.y--;

	return ptRowColumn;
}

RECT CDUIListViewCtrl::GetItemTextPadding()
{
	return DuiDpiScaleCtrl(m_AttributeItemTextPadding.GetValue());
}

Gdiplus::ARGB CDUIListViewCtrl::GetItemTextColorNormal()
{
	return m_AttributeItemTextStyleNormal.GetTextColor();
}

Gdiplus::ARGB CDUIListViewCtrl::GetItemTextColorHot()
{
	return m_AttributeItemTextStyleHot.GetTextColor();
}

Gdiplus::ARGB CDUIListViewCtrl::GetItemTextColorSelNormal()
{
	return m_AttributeItemTextStyleSelNormal.GetTextColor();
}

Gdiplus::ARGB CDUIListViewCtrl::GetItemTextColorSelHot()
{
	return m_AttributeItemTextStyleSelHot.GetTextColor();
}

Gdiplus::ARGB CDUIListViewCtrl::GetItemTextColorDisabled()
{
	return m_AttributeItemTextStyleDisabled.GetTextColor();
}

tagDuiTextStyle CDUIListViewCtrl::GetItemTextStyleNormal()
{
	return m_AttributeItemTextStyleNormal.GetTextStyle();
}

tagDuiTextStyle CDUIListViewCtrl::GetItemTextStyleHot()
{
	return m_AttributeItemTextStyleHot.GetTextStyle();
}

tagDuiTextStyle CDUIListViewCtrl::GetItemTextStyleSelNormal()
{
	return m_AttributeItemTextStyleSelNormal.GetTextStyle();
}

tagDuiTextStyle CDUIListViewCtrl::GetItemTextStyleSelHot()
{
	return m_AttributeItemTextStyleSelHot.GetTextStyle();
}

tagDuiTextStyle CDUIListViewCtrl::GetItemTextStyleDisabled()
{
	return m_AttributeItemTextStyleDisabled.GetTextStyle();
}

Gdiplus::ARGB CDUIListViewCtrl::GetItemStatusColorNormal()
{
	return m_AttributeColorItemStatusNormal.GetColorValue();
}

Gdiplus::ARGB CDUIListViewCtrl::GetItemStatusColorHot()
{
	return m_AttributeColorItemStatusHot.GetColorValue();
}

Gdiplus::ARGB CDUIListViewCtrl::GetItemStatusColorSelNormal()
{
	return m_AttributeColorItemStatusSelNormal.GetColorValue();
}

Gdiplus::ARGB CDUIListViewCtrl::GetItemStatusColorSelHot()
{
	return m_AttributeColorItemStatusSelHot.GetColorValue();
}

Gdiplus::ARGB CDUIListViewCtrl::GetItemStatusColorDisabled()
{
	return m_AttributeColorItemStatusDisabled.GetColorValue();
}

vector<CMMString> CDUIListViewCtrl::GetItemStatusColorResSwitchNormal()
{
	return m_AttributeColorItemStatusNormal.GetColorResSwitch();
}

vector<CMMString> CDUIListViewCtrl::GetItemStatusColorResSwitchHot()
{
	return m_AttributeColorItemStatusHot.GetColorResSwitch();
}

vector<CMMString> CDUIListViewCtrl::GetItemStatusColorResSwitchSelNormal()
{
	return m_AttributeColorItemStatusSelNormal.GetColorResSwitch();
}

vector<CMMString> CDUIListViewCtrl::GetItemStatusColorResSwitchSelHot()
{
	return m_AttributeColorItemStatusSelHot.GetColorResSwitch();
}

vector<CMMString> CDUIListViewCtrl::GetItemStatusColorResSwitchDisabled()
{
	return m_AttributeColorItemStatusDisabled.GetColorResSwitch();
}

tagDuiImageSection CDUIListViewCtrl::GetItemStatusImageSectionNormal()
{
	return m_AttributeImageItemStatusNormal.GetImageSection();
}

tagDuiImageSection CDUIListViewCtrl::GetItemStatusImageSectionHot()
{
	return m_AttributeImageItemStatusHot.GetImageSection();
}

tagDuiImageSection CDUIListViewCtrl::GetItemStatusImageSectionSelNormal()
{
	return m_AttributeImageItemStatusSelNormal.GetImageSection();
}

tagDuiImageSection CDUIListViewCtrl::GetItemStatusImageSectionSelHot()
{
	return m_AttributeImageItemStatusSelHot.GetImageSection();
}

tagDuiImageSection CDUIListViewCtrl::GetItemStatusImageSectionDisabled()
{
	return m_AttributeImageItemStatusDisabled.GetImageSection();
}

void CDUIListViewCtrl::SetItemTextPadding(RECT rcPadding)
{
	if (rcPadding == m_AttributeItemTextPadding.GetValue()) return;

	m_AttributeItemTextPadding.SetValue(rcPadding);

	Invalidate();

	return;
}

void CDUIListViewCtrl::SetItemTextStyleAll(const tagDuiTextStyle &TextStyle)
{
	m_AttributeItemTextStyleNormal.SetTextStyle(TextStyle);
	m_AttributeItemTextStyleHot.SetTextStyle(TextStyle);
	m_AttributeItemTextStyleSelNormal.SetTextStyle(TextStyle);
	m_AttributeItemTextStyleSelHot.SetTextStyle(TextStyle);
	m_AttributeItemTextStyleDisabled.SetTextStyle(TextStyle);

	Invalidate();

	return;
}

void CDUIListViewCtrl::SetItemTextStyleNormal(const tagDuiTextStyle &TextStyle)
{
	m_AttributeItemTextStyleNormal.SetTextStyle(TextStyle);

	Invalidate();

	return;
}

void CDUIListViewCtrl::SetItemTextStyleHot(const tagDuiTextStyle &TextStyle)
{
	m_AttributeItemTextStyleHot.SetTextStyle(TextStyle);

	Invalidate();

	return;
}

void CDUIListViewCtrl::SetItemTextStyleSelNormal(const tagDuiTextStyle &TextStyle)
{
	m_AttributeItemTextStyleSelNormal.SetTextStyle(TextStyle);

	Invalidate();

	return;
}

void CDUIListViewCtrl::SetItemTextStyleSelHot(const tagDuiTextStyle &TextStyle)
{
	m_AttributeItemTextStyleSelHot.SetTextStyle(TextStyle);

	Invalidate();

	return;
}

void CDUIListViewCtrl::SetItemTextStyleDisabled(const tagDuiTextStyle &TextStyle)
{
	m_AttributeItemTextStyleDisabled.SetTextStyle(TextStyle);

	Invalidate();

	return;
}

void CDUIListViewCtrl::SetItemStatusColorResSwitchNormal(const vector<CMMString> &vecResSwitch)
{
	m_AttributeColorItemStatusNormal.SetColorResSwitch(vecResSwitch);

	Invalidate();

	return;
}

void CDUIListViewCtrl::SetItemStatusColorResSwitchHot(const vector<CMMString> &vecResSwitch)
{
	m_AttributeColorItemStatusHot.SetColorResSwitch(vecResSwitch);

	Invalidate();

	return;
}

void CDUIListViewCtrl::SetItemStatusColorResSwitchSelNormal(const vector<CMMString> &vecResSwitch)
{
	m_AttributeColorItemStatusSelNormal.SetColorResSwitch(vecResSwitch);

	Invalidate();

	return;
}

void CDUIListViewCtrl::SetItemStatusColorResSwitchSelHot(const vector<CMMString> &vecResSwitch)
{
	m_AttributeColorItemStatusSelHot.SetColorResSwitch(vecResSwitch);

	Invalidate();

	return;
}

void CDUIListViewCtrl::SetItemStatusColorResSwitchDisabled(const vector<CMMString> &vecResSwitch)
{
	m_AttributeColorItemStatusDisabled.SetColorResSwitch(vecResSwitch);

	Invalidate();

	return;
}

void CDUIListViewCtrl::SetItemStatusImageSectionNormal(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageItemStatusNormal.SetImageSection(ImageSection);

	Invalidate();

	return;
}

void CDUIListViewCtrl::SetItemStatusImageSectionHot(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageItemStatusHot.SetImageSection(ImageSection);

	Invalidate();

	return;
}

void CDUIListViewCtrl::SetItemStatusImageSectionSelNormal(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageItemStatusSelNormal.SetImageSection(ImageSection);

	Invalidate();

	return;
}

void CDUIListViewCtrl::SetItemStatusImageSectionSelHot(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageItemStatusSelHot.SetImageSection(ImageSection);

	Invalidate();

	return;
}

void CDUIListViewCtrl::SetItemStatusImageSectionDisabled(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageItemStatusDisabled.SetImageSection(ImageSection);

	Invalidate();

	return;
}

Gdiplus::ARGB CDUIListViewCtrl::GetLineColor()
{
	return m_AttributeColorLine.GetColorValue();
}

void CDUIListViewCtrl::SetLineColorResSwitch(const vector<CMMString> &vecResSwitch)
{
	m_AttributeColorLine.SetColorResSwitch(vecResSwitch);

	Invalidate();

	return;
}

bool CDUIListViewCtrl::IsShowRowLine()
{
	return m_AttributeShowRowLine.GetValue();
}

void CDUIListViewCtrl::SetShowRowLine(bool bShowLine)
{
	if (bShowLine == IsShowRowLine()) return;

	m_AttributeShowRowLine.SetValue(bShowLine);

	Invalidate();

	return;
}

bool CDUIListViewCtrl::IsShowColumnLine()
{
	return m_AttributeShowColumnLine.GetValue();
}

void CDUIListViewCtrl::SetShowColumnLine(bool bShowLine)
{
	if (bShowLine == IsShowColumnLine()) return;

	m_AttributeShowColumnLine.SetValue(bShowLine);

	Invalidate();

	return;
}

bool CDUIListViewCtrl::SortItems(PLVCompareFunc pfnCompare, UINT_PTR dwData)
{
	if (NULL == pfnCompare) return false;

	//cursel
	CDUIListItemCtrl *pItemCur = GetChildAt(GetCurSel());

	m_pCompareFunc = pfnCompare;
	m_CompareData = dwData;

	if (m_vecChilds.size() > 0)
	{
		CDUIControlBase **pData = m_vecChilds.data();
		qsort_s(m_vecChilds.data(), GetChildCount(), sizeof(CDUIControlBase*), ItemComareFunc, this);
	}

	//index
	for (int i = 0; i < GetChildCount(); ++i)
	{
		CDUIListItemCtrl *pItem = GetChildAt(i);
		if (NULL == pItem) continue;

		pItem->SetIndex(i);
	}

	//cursel
	if (pItemCur)
	{
		m_nCurSel = pItemCur->GetIndex();
	}

	NeedRefreshView();

	return true;
}

bool CDUIListViewCtrl::OnDuiMouseWheel(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (IsScrollSelect() && false == IsMultiSelect())
	{
		//direction
		int nWheelDelta = (int)(short)HIWORD(Msg.wParam);
		int nWheelCount = abs(nWheelDelta / WHEEL_DELTA);
		bool bPositive = nWheelDelta > 0;

		if (bPositive)
		{
			SelectItem(FindSelectable(VK_UP));
		}
		else
		{
			SelectItem(FindSelectable(VK_DOWN));
		}

		CDUIControlBase::OnDuiMouseWheel(pt, Msg);

		return true;
	}

	return __super::OnDuiMouseWheel(pt, Msg);
}

bool CDUIListViewCtrl::OnDuiMouseMove(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (false == m_AttributeImageMouseDragSel.IsEmpty()
		&& this == m_pWndOwner->GetCaptureControl())
	{
		PerformMouseDragSel();
	}

	return __super::OnDuiMouseMove(pt, Msg);
}

bool CDUIListViewCtrl::OnDuiLButtonDown(const CDUIPoint &pt, const DuiMessage &Msg)
{
	__super::OnDuiLButtonDown(pt, Msg);

	if (this == Msg.pMsgCtrl)
	{
		if (0 == (CDUIWnd::MapKeyState() & MK_CONTROL) && 0 == (CDUIWnd::MapKeyState() & MK_SHIFT))
		{
			UnSelectAllItems();
		}
		if (this == m_pWndOwner->GetCaptureControl())
		{
			m_ptMouseDown = pt;
			m_rcMouseDragSel.Clear();
			m_vecSelItemMouseDragPre = m_vecSelItem;
		}
	}

	return 0;
}

bool CDUIListViewCtrl::OnDuiLButtonUp(const CDUIPoint &pt, const DuiMessage &Msg)
{
	m_vecSelItemMouseDragPre.clear();

	return __super::OnDuiLButtonUp(pt, Msg);
}

bool CDUIListViewCtrl::OnDuiLButtonDlk(const CDUIPoint &pt, const DuiMessage &Msg)
{
	return __super::OnDuiLButtonDlk(pt, Msg);
}

bool CDUIListViewCtrl::OnDuiRButtonDown(const CDUIPoint &pt, const DuiMessage &Msg)
{
	if (this == Msg.pMsgCtrl)
	{
		if (0 == (CDUIWnd::MapKeyState() & MK_CONTROL) && 0 == (CDUIWnd::MapKeyState() & MK_SHIFT))
		{
			UnSelectAllItems();
		}
		if (this == m_pWndOwner->GetCaptureControl())
		{
			m_ptMouseDown = pt;
			m_rcMouseDragSel.Clear();
			m_vecSelItemMouseDragPre = m_vecSelItem;
		}
	}

	return __super::OnDuiRButtonDown(pt, Msg);
}

bool CDUIListViewCtrl::OnDuiRButtonUp(const CDUIPoint &pt, const DuiMessage &Msg)
{
	m_vecSelItemMouseDragPre.clear();

	return __super::OnDuiRButtonUp(pt, Msg);
}

bool CDUIListViewCtrl::OnDuiRButtonDlk(const CDUIPoint &pt, const DuiMessage &Msg)
{
	return __super::OnDuiRButtonDlk(pt, Msg);
}

LRESULT CDUIListViewCtrl::OnDuiKeyDown(const DuiMessage &Msg)
{
	switch (Msg.chKey)
	{
		case VK_UP:
		case VK_DOWN:
		case VK_LEFT:
		case VK_RIGHT:
		{
			int nIndex = FindSelectable(Msg.chKey);
			
			if (-1 != nIndex)
			{
				if (0 == (Msg.wKeyState & MK_CONTROL))
				{
					UnSelectAllItems();
				}

				SelectItem(nIndex);
			}

			return 0;
		}
		case VK_PRIOR:
		{
			PageUp();

			return 0;
		}
		case VK_NEXT:
		{
			PageDown();

			return 0;
		}
		case VK_HOME:
		{
			SelectItem(FindNextIndex(-1, true));

			return 0;
		}
		case VK_END:
		{
			SelectItem(FindNextIndex(GetChildCount() - 1, false));

			return 0;
		}
		case VK_RETURN:
		{
			if (m_nCurSel >= 0)
			{
				CDUIListItemCtrl *pItem = GetChildAt(m_nCurSel);
				if (pItem == NULL) break;

				pItem->Active();
			}

			return 0;
		}
		case 0x41:// Ctrl+A
		{
			if (GetKeyState(VK_CONTROL) & 0x8000)
			{
				SelectAllItems();
			}

			return 0;
		}

		default:
			break;
	}

	return __super::OnDuiKeyDown(Msg);
}

LRESULT CDUIListViewCtrl::OnDuiKeyUp(const DuiMessage &Msg)
{
	return __super::OnDuiKeyUp(Msg);
}

LRESULT	CDUIListViewCtrl::OnDuiContextMenu(const DuiMessage &Msg)
{
	return __super::OnDuiContextMenu(Msg);
}

void CDUIListViewCtrl::OnItemVisible(CDUIListItemCtrl *pItem)
{
	if (NULL == pItem) return;

	if (pItem->IsVisible())
	{
		return;
	}

	UnSelectItem(pItem->GetIndex());

	return;
}

void CDUIListViewCtrl::OnItemFixedWidth(int nWidthOld, int nWidthNew)
{
	return;
}

void CDUIListViewCtrl::OnItemFixedHeight(int nHeightOld, int nHeightNew)
{
	return;
}

void CDUIListViewCtrl::InitProperty()
{
	__super::InitProperty();

	//pos
	DuiCreateAttribute(m_AttributeAutoCalcWidth, _T("AutoCalcWidth"), _T(""), m_AttributeGroupPosition);

	//child
	DuiCreateAttribute(m_AttributeChildPaddingH, _T("ChildPaddingH"), _T(""), m_AttributeGroupChild);
	DuiCreateAttribute(m_AttributeAutoCalcChildPaddingH, _T("AutoCalcChildPaddingH"), _T("For TileH"), m_AttributeGroupChild);

	DuiCreateGroupAttribute(m_AttributeGroupListView, _T("ListView"));
	DuiCreateAttribute(m_AttributeUseListHeader, _T("UseListHeader"), _T(""), m_AttributeGroupListView);
	DuiCreateAttribute(m_AttributeListViewType, _T("ListViewType"), _T(""), m_AttributeGroupListView);
	DuiCreateAttribute(m_AttributeMultiSel, _T("MultiSelect"), _T(""), m_AttributeGroupListView);
	DuiCreateAttribute(m_AttributeScrollSelect, _T("ScrollSelect"), _T("For SingleSelect"), m_AttributeGroupListView);
	DuiCreateAttribute(m_AttributeItemModel, _T("ItemModel"), _T(""), m_AttributeGroupListView);
	DuiCreateAttribute(m_AttributeItemModelInitCount, _T("ItemModelInitCount"), _T(""), m_AttributeGroupListView);
	DuiCreateAttribute(m_AttributeImageMouseDragSel, _T("ImageMouseDragSel"), _T(""), m_AttributeGroupListView);

	DuiCreateGroupAttribute(m_AttributeGroupListType, _T("ListType"));
	DuiCreateAttribute(m_AttributeSwitchListItemHeight, _T("SwitchListItemHeight"), _T(""), m_AttributeGroupListType);
	DuiCreateAttribute(m_AttributeSelectIconVisible, _T("SelectIconVisible"), _T(""), m_AttributeGroupListType);
	DuiCreateAttribute(m_AttributeSelectDbClick, _T("SelectDbClick"), _T(""), m_AttributeGroupListType);
	DuiCreateAttribute(m_AttributeSelectIconLeftPadding, _T("SelectIconLeftPadding"), _T(""), m_AttributeGroupListType);
	DuiCreateAttribute(m_AttributeSelectIconFixedWidth, _T("SelectIconFixedWidth"), _T(""), m_AttributeGroupListType);
	DuiCreateAttribute(m_AttributeSelectIconFixedHeight, _T("SelectIconFixedHeight"), _T(""), m_AttributeGroupListType);
	DuiCreateAttribute(m_AttributeImageSelectIconNormal, _T("SelectIconNormal"), _T(""), m_AttributeGroupListType);
	DuiCreateAttribute(m_AttributeImageSelectIconHot, _T("SelectIconHot"), _T(""), m_AttributeGroupListType);
	DuiCreateAttribute(m_AttributeImageSelectIconPushed, _T("SelectIconPushed"), _T(""), m_AttributeGroupListType);
	DuiCreateAttribute(m_AttributeImageSelectIconSelNormal, _T("SelectIconSelNormal"), _T(""), m_AttributeGroupListType);
	DuiCreateAttribute(m_AttributeImageSelectIconSelHot, _T("SelectIconSelHot"), _T(""), m_AttributeGroupListType);
	DuiCreateAttribute(m_AttributeImageSelectIconSelPushed, _T("SelectIconSelPushed"), _T(""), m_AttributeGroupListType);

	//tile type
	DuiCreateGroupAttribute(m_AttributeGroupTileType, _T("TileType"));
	DuiCreateAttribute(m_AttributeAutoCalcItemWidth, _T("AutoCalcItemWidth"), _T("For TileH, Item Width >= SwitchTileItemSize"), m_AttributeGroupTileType);
	DuiCreateAttribute(m_AttributeAutoCalcItemHeight, _T("AutoCalcItemHeight"), _T("For TileV, Item Height >= SwitchTileItemSize"), m_AttributeGroupTileType);
	DuiCreateAttribute(m_AttributeLeftFromRange, _T("LeftFromRange"), _T(""), m_AttributeGroupTileType);
	DuiCreateAttribute(m_AttributeTopFromRange, _T("TopFromRange"), _T(""), m_AttributeGroupTileType);
	DuiCreateAttribute(m_AttributeSwitchTileItemSize, _T("SwitchTileItemSize"), _T(""), m_AttributeGroupTileType);

	DuiCreateGroupAttribute(m_AttributeGroupItemText, _T("ItemText"));
	DuiCreateAttribute(m_AttributeItemTextStyleNormal, _T("ItemTextStyleNormal"), _T("Item Text1-32 Style"), m_AttributeGroupItemText);
	DuiCreateAttribute(m_AttributeItemTextStyleHot, _T("ItemTextStyleHot"), _T("Item Text1-32 Style"), m_AttributeGroupItemText);
	DuiCreateAttribute(m_AttributeItemTextStyleSelNormal, _T("ItemTextStyleSelNormal"), _T("Item Text1-32 Style"), m_AttributeGroupItemText);
	DuiCreateAttribute(m_AttributeItemTextStyleSelHot, _T("ItemTextStyleSelHot"), _T("Item Text1-32 Style"), m_AttributeGroupItemText);
	DuiCreateAttribute(m_AttributeItemTextStyleDisabled, _T("ItemTextStyleDisabled"), _T("Item Text1-32 Style"), m_AttributeGroupItemText);
	DuiCreateAttribute(m_AttributeItemTextPadding, _T("ItemTextPadding"), _T("Item Text1-32 Padding"), m_AttributeGroupItemText);

	DuiCreateGroupAttribute(m_AttributeGroupItemStatus, _T("ItemStatus"));
	DuiCreateAttribute(m_AttributeColorItemStatusNormal, _T("ItemColorNormal"), _T(""), m_AttributeGroupItemStatus);
	DuiCreateAttribute(m_AttributeColorItemStatusHot, _T("ItemColorHot"), _T(""), m_AttributeGroupItemStatus);
	DuiCreateAttribute(m_AttributeColorItemStatusSelNormal, _T("ItemColorSelNormal"), _T(""), m_AttributeGroupItemStatus);
	DuiCreateAttribute(m_AttributeColorItemStatusSelHot, _T("ItemColorSelHot"), _T(""), m_AttributeGroupItemStatus);
	DuiCreateAttribute(m_AttributeColorItemStatusDisabled, _T("ItemColorDisabled"), _T(""), m_AttributeGroupItemStatus);
	DuiCreateAttribute(m_AttributeImageItemStatusNormal, _T("ItemImageNormal"), _T(""), m_AttributeGroupItemStatus);
	DuiCreateAttribute(m_AttributeImageItemStatusHot, _T("ItemImageHot"), _T(""), m_AttributeGroupItemStatus);
	DuiCreateAttribute(m_AttributeImageItemStatusSelNormal, _T("ItemImageSelNormal"), _T(""), m_AttributeGroupItemStatus);
	DuiCreateAttribute(m_AttributeImageItemStatusSelHot, _T("ItemImageSelHot"), _T(""), m_AttributeGroupItemStatus);
	DuiCreateAttribute(m_AttributeImageItemStatusDisabled, _T("ItemImageDisabled"), _T(""), m_AttributeGroupItemStatus);

	DuiCreateGroupAttribute(m_AttributeGroupLine, _T("Line"));
	DuiCreateAttribute(m_AttributeColorLine, _T("ColorLine"), _T(""), m_AttributeGroupLine);
	DuiCreateAttribute(m_AttributeShowRowLine, _T("ShowRowLine"), _T(""), m_AttributeGroupLine);
	DuiCreateAttribute(m_AttributeShowColumnLine, _T("ShowColumnLine"), _T(""), m_AttributeGroupLine);
	
	//swap index
	DuiSwapAttriIndex(m_AttributeChildPaddingH, m_AttributeChildPaddingV);
	DuiSwapAttriIndex(m_AttributeAutoCalcChildPaddingH, m_AttributeAutoCalcChildPaddingV);

	//value
	DuiInitAttriValue(m_AttributeUseHorizScrollBar, true);
	DuiInitAttriValue(m_AttributeUseVertScrollBar, true);

	return;
}

void CDUIListViewCtrl::InitComplete()
{
	__super::InitComplete();

	DuiInitAttriVisible(m_AttributeChildPaddingH, true);
	DuiInitAttriVisible(m_AttributeAutoCalcChildPaddingH, true);
	DuiInitAttriVisible(m_AttributeAutoCalcChildPaddingV, true);
	
	//header
	if (NULL == m_pListHeader && 0 != lstrcmp(GetClass(), CDUIRotateMenuCtrl().GetClass()))
	{
		CDUIListHeaderCtrl *pListHeader = new CDUIListHeaderCtrl();
		pListHeader->Init();
		pListHeader->SetPadding(0, 0, 0, 0);
		pListHeader->SetFixedHeight(30);

		if (false == InsertChild(pListHeader))
		{
			MMSafeDelete(pListHeader);
		}
	}
	if (m_pAnimateDrag)
	{
		m_pAnimateDrag->SetDragType(ListView_List == GetListViewType() ? DragType_V : DragType_HV);
	}

	SetUseListHeader(IsUseListHeader());
	SwitchListViewType(GetListViewType());

	return;
}

bool CDUIListViewCtrl::SaveAttribute(tinyxml2::XMLElement *pNode, bool bIncludeChild /*= true*/)
{
	if (NULL == pNode || NULL == pNode->GetDocument()) return false;

	__super::SaveAttribute(pNode, bIncludeChild);

	if (m_pListHeader)
	{
		tinyxml2::XMLElement *pNodeChild = pNode->GetDocument()->NewElement((LPCSTR)CT2CA(m_pListHeader->GetClass()));
		if (NULL == pNodeChild) { ASSERT(false); }

		m_pListHeader->SaveAttribute(pNodeChild);

		pNode->LinkEndChild(pNodeChild);
	}

	return true;
}

void CDUIListViewCtrl::PaintMouseDragSel(HDC hDC)
{
	if (NULL == m_pWndOwner
		|| m_AttributeImageMouseDragSel.IsEmpty()
		|| this != m_pWndOwner->GetCaptureControl()) return;

	m_AttributeImageMouseDragSel.Draw(hDC, m_rcMouseDragSel, m_rcPaint);

	return;
}

CDUIRect CDUIListViewCtrl::GetLayoutRangeOfScrollBar()
{
	CDUIRect rcRange = GetAbsoluteRect();

	if (IsUseListHeader() && m_pListHeader && m_pListHeader->IsVisible())
	{
		rcRange.top = m_pListHeader->GetAbsoluteRect().bottom;
	}

	return rcRange;
}

CDUIRect CDUIListViewCtrl::GetLayoutRangeOfItem()
{
	CDUIRect rcItemRange = GetAbsoluteRect();

	//header
	if (IsUseListHeader() && m_pListHeader && m_pListHeader->IsVisible())
	{
		rcItemRange.top = m_pListHeader->GetAbsoluteRect().bottom;
		rcItemRange.right += max(0, m_pListHeader->GetWidth() - GetWidth());
	}

	//scroll bar
	if (IsHScrollBarPushItemRange() && m_pHorizScrollBarCtrl && m_pHorizScrollBarCtrl->IsVisible())
	{
		rcItemRange.bottom -= m_pHorizScrollBarCtrl->GetHeight();
	}
	if (IsVScrollBarPushItemRange() && m_pVertScrollBarCtrl && m_pVertScrollBarCtrl->IsVisible())
	{
		rcItemRange.right -= m_pVertScrollBarCtrl->GetWidth();
	}

	rcItemRange.left += GetRangeInset().left;
	rcItemRange.top += GetRangeInset().top;
	rcItemRange.right -= GetRangeInset().right;
	rcItemRange.bottom -= GetRangeInset().bottom;

	return rcItemRange;
}

void CDUIListViewCtrl::ScrollChilds(CDUISize szScroll)
{
	//header
	if (0 != szScroll.cx)
	{
		if (IsUseListHeader() && m_pListHeader && m_pListHeader->IsVisible())
		{
			CDUIRect rcModal = m_pListHeader->GetModalParentRect();
			rcModal.Offset(CDUISize(-szScroll.cx, 0));
			m_pListHeader->OnDuiSize(rcModal);
		}
	}

	//child
	switch (GetListViewType())
	{
		case ListView_List:
		{
			if (szScroll.cy > 0 || 0 != szScroll.cx)
			{
				ScrollChildsUpOnListType(szScroll);

				break;
			}
			if (szScroll.cy < 0)
			{
				ScrollChildsDownOnListType(szScroll);
			}

			break;
		}
		case ListView_TileH:
		{
			if (szScroll.cy > 0)
			{
				ScrollChildsUpOnTileTypeH(szScroll);

				break;
			}
			if (szScroll.cy < 0)
			{
				ScrollChildsDownOnTileTypeH(szScroll);
			}

			break;
		}
		case ListView_TileV:
		{
			if (szScroll.cx > 0)
			{
				ScrollChildsUpOnTileTypeV(szScroll);

				break;
			}
			if (szScroll.cx < 0)
			{
				ScrollChildsDownOnTileTypeV(szScroll);
			}

			break;
		}
	}

	//mouse down
	m_ptMouseDown.x -= szScroll.cx;
	m_ptMouseDown.y -= szScroll.cy;

	PerformMouseDragSel();

	Invalidate();

	return;
}

int CDUIListViewCtrl::GetMinSelItemIndex()
{
	if (m_vecSelItem.size() <= 0) return -1;

	int nIndex = -1;
	for (auto pItem : m_vecSelItem)
	{
		if (NULL == pItem) continue;

		if (-1 == nIndex) nIndex = pItem->GetIndex();

		nIndex = min(nIndex, pItem->GetIndex());
	}

	return nIndex;
}

int CDUIListViewCtrl::GetMaxSelItemIndex()
{
	if (m_vecSelItem.size() <= 0) return -1;

	int nIndex = -1;
	for (auto pItem : m_vecSelItem)
	{
		if (NULL == pItem) continue;

		if (-1 == nIndex) nIndex = pItem->GetIndex();

		nIndex = max(nIndex, pItem->GetIndex());
	}

	return nIndex;
}

CDUIListItemCtrl * CDUIListViewCtrl::ProductItemDefault()
{
	CDUIListItemCtrl *pItem = new CDUIListItemCtrl();
	pItem->Init();

	return pItem;
}

void CDUIListViewCtrl::SetInternVisible(bool bVisible, bool bTraversal)
{
	__super::SetInternVisible(bVisible, bTraversal);

	if (m_pListHeader)
	{
		m_pListHeader->SetInternVisible(IsVisible(), bTraversal);
	}

	return;
}

CDUIControlBase * CDUIListViewCtrl::FindControl(FindControlProc Proc, LPVOID pData, UINT uFlags)
{
	//header
	if (m_pListHeader)
	{
		CDUIControlBase *pResult = m_pListHeader->FindControl(Proc, pData, uFlags);

		if (pResult && 0 == (uFlags & DuiFind_UpdateTest)) return pResult;
	}

	//hittest
	if (uFlags & DuiFind_HitTest)
	{
		if (false == IsEnabled()) return NULL;
		if ((uFlags & DuiFind_Visible) && false == IsVisible()) return NULL;
		if (false == ::PtInRect(&m_rcAbsolute, *static_cast<LPPOINT>(pData))) return NULL;

		CDUIControlBase *pResult = NULL;

		//mefirst
		if ((uFlags & DuiFind_MeFirst) != 0)
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

		//focus
		CDUIControlBase *pFocusCtrl = m_pWndOwner->GetFocusControl();
		if (pFocusCtrl && pFocusCtrl->GetOwnerModelCtrl())
		{
			for (int n = 0; n < GetChildCount(); n++)
			{
				CDUIControlBase *pChild = GetChildAt(n);
				if (NULL == pChild || pChild != pFocusCtrl->GetOwnerModelCtrl()) continue;

				pResult = pChild->FindControl(Proc, pData, uFlags);
				if (pResult) return pResult;

				break;
			}
		}

		//child
		if ((uFlags & DuiFind_TopFirst) != 0)
		{
			for (int n = GetChildCount() - 1; n >= 0; n--)
			{
				CDUIControlBase *pChild = GetChildAt(n);
				if (NULL == pChild) continue;
				if (false == pChild->IsFloat() && (n < m_nFirstShowIndex || n > m_nLastShowIndex)) continue;

				pResult = pChild->FindControl(Proc, pData, uFlags);
				if (pResult) return pResult;
			}
		}
		else
		{
			for (int n = 0; n < GetChildCount(); n++)
			{
				CDUIControlBase *pChild = GetChildAt(n);
				if (NULL == pChild) continue;
				if (false == pChild->IsFloat() && (n < m_nFirstShowIndex || n > m_nLastShowIndex)) continue;

				pResult = pChild->FindControl(Proc, pData, uFlags);
				if (pResult) return pResult;
			}
		}

		return __super::FindControl(Proc, pData, uFlags);
	}

	//other
	return __super::FindControl(Proc, pData, uFlags);
}

void CDUIListViewCtrl::ReapControl()
{
	__super::ReapControl();

	if (m_pListHeader)
	{
		m_pListHeader->ReapControl();
	}

	return;
}

void CDUIListViewCtrl::PerformMouseDragSel()
{
	if (NULL == m_pWndOwner
		|| m_AttributeImageMouseDragSel.IsEmpty()
		|| this != m_pWndOwner->GetCaptureControl()) return;

	//range
	CDUIPoint ptMouse = {};
	GetCursorPos(&ptMouse);
	ScreenToClient(m_pWndOwner->GetWndHandle(), &ptMouse);
	m_rcMouseDragSel = CDUIRect
	(
		min(ptMouse.x, m_ptMouseDown.x),
		min(ptMouse.y, m_ptMouseDown.y),
		max(ptMouse.x, m_ptMouseDown.x),
		max(ptMouse.y, m_ptMouseDown.y)
	);

	//select child
	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUIListItemCtrl *pItem = GetChildAt(n);
		if (NULL == pItem || false == pItem->IsVisible()) continue;

		CDUIRect rcTemp = {};
		CDUIRect rcItem = pItem->GetAbsoluteRect();
		if (::GetKeyState(VK_CONTROL) & 0x8000 || ::GetKeyState(VK_SHIFT) & 0x8000)
		{
			auto FindIt = find(m_vecSelItemMouseDragPre.begin(), m_vecSelItemMouseDragPre.end(), pItem);

			if (::IntersectRect(&rcTemp, &rcItem, &m_rcMouseDragSel))
			{
				pItem->Select(FindIt == m_vecSelItemMouseDragPre.end());

				continue;
			}

			pItem->Select(FindIt != m_vecSelItemMouseDragPre.end());

			continue;
		}

		pItem->Select(::IntersectRect(&rcTemp, &rcItem, &m_rcMouseDragSel));
	}

	Invalidate();

	return;
}

int __cdecl CDUIListViewCtrl::ItemComareFunc(void *pVlocale, const void *pItemFirst, const void *pItemNext)
{
	CDUIListViewCtrl *pThis = (CDUIListViewCtrl*)pVlocale;
	if (!pThis || !pItemFirst || !pItemNext) return 0;

	return pThis->ItemComareFunc(pItemFirst, pItemNext);
}

int __cdecl CDUIListViewCtrl::ItemComareFunc(const void *pItemFirst, const void *pItemNext)
{
	CDUIControlBase *pControl1 = *(CDUIControlBase**)pItemFirst;
	CDUIControlBase *pControl2 = *(CDUIControlBase**)pItemNext;

	return m_pCompareFunc(pControl1, pControl2, m_CompareData);
}

void CDUIListViewCtrl::InsertChildOnListType(CDUIListItemCtrl *pItem)
{
	if (NULL == pItem) return;

	return;
}

void CDUIListViewCtrl::RefreshViewOnListType()
{
	CDUISize szScrollPos = GetScrollPos();

	//header
	if (IsUseListHeader() && m_pListHeader && m_pListHeader->IsVisible())
	{
		CDUIRect rcModal = GetAbsoluteRect();
		rcModal.Offset(CDUISize(-szScrollPos.cx, 0));
		m_pListHeader->OnDuiSize(rcModal);
	}

	//scroll bar
	RefreshScrollBar();

	CalcShowItemOnListType();

	return;
}

void CDUIListViewCtrl::CalcShowItemOnListType()
{
	CDUISize szScrollPos = GetScrollPos();
	CDUIRect rcItemRange = GetLayoutRangeOfItem();

	//init variant
	m_nFirstShowIndex = m_nLastShowIndex = -1;
	m_szTotalRange = { rcItemRange.GetWidth(), 0 };
	int nYTop = -szScrollPos.cy;
	bool bFindFirst = true;
	bool bFindLast = true;

	//first index
	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUIControlBase *pItem = GetChildAt(n);
		if (NULL == pItem || false == pItem->IsVisible()) continue;

		//float
		if (pItem->IsFloat())
		{
			CDUIRect rcModal = rcItemRange;
			pItem->OnDuiSize(rcModal);

			continue;
		}

		//height
		int nFixedHeight = pItem->GetFixedHeight();
		nFixedHeight = (nFixedHeight <= 0 ? GetSwitchListItemHeight() : nFixedHeight);
		m_szTotalRange.cx = max(m_szTotalRange.cx, pItem->GetFixedWidth());
		m_szTotalRange.cy += m_szTotalRange.cy > 0 ? GetChildPaddingV() : 0;
		m_szTotalRange.cy += nFixedHeight;

		//item pos
		CDUIRect rcModal = rcItemRange;
		rcModal.Offset(CDUISize(-szScrollPos.cx, 0));
		rcModal.top += nYTop;
		rcModal.bottom = rcModal.top + nFixedHeight;
		pItem->OnDuiSize(rcModal);

		CDUIRect rcItem = pItem->GetAbsoluteRect();
		if (bFindFirst)
		{
			m_nFirstShowIndex = n;

			if (rcItem.bottom >= rcItemRange.top)
			{
				bFindFirst = false;
			}
		}
		if (bFindLast)
		{
			m_nLastShowIndex = n;

			if (rcItem.top >= rcItemRange.bottom)
			{
				bFindLast = false;
			}
		}

		nYTop += rcItem.GetHeight();
		nYTop += GetChildPaddingV();
	}

	//update scrollbar
	int nHorizOver = 0;
	if (IsUseListHeader() && m_pListHeader && m_pListHeader->IsVisible())
	{
		nHorizOver = max(0, m_pListHeader->GetWidth() - GetWidth());
	}

	CDUISize szCurRange(max(nHorizOver, m_szTotalRange.cx - rcItemRange.GetWidth()), m_szTotalRange.cy - rcItemRange.GetHeight());
	szCurRange.cx = max(0, szCurRange.cx);
	szCurRange.cy = max(0, szCurRange.cy);

	UpdateScrollBar(szCurRange);

	return;
}

void CDUIListViewCtrl::ScrollChildsUpOnListType(CDUISize szScroll)
{
	CDUIRect rcItemRange = GetLayoutRangeOfItem();
	CDUISize szScrollPos = GetScrollPos();

	//no float
	bool bFindFirst = true;
	CDUIListItemCtrl *pItemLast = NULL;
	for (int n = m_nFirstShowIndex; n < GetChildCount(); n++)
	{
		CDUIListItemCtrl *pItem = GetChildAt(n);
		if (NULL == pItem || false == pItem->IsVisible()) continue;

		if (n == m_nFirstShowIndex)
		{
			pItemLast = pItem;

			CDUIRect rcModal = pItem->GetModalParentRect();
			rcModal.Offset(CDUISize(-szScroll.cx, -szScroll.cy));
			pItem->OnDuiSize(rcModal);
		}
		else if (pItemLast)
		{
			int nFixedHeight = pItem->GetFixedHeight();
			nFixedHeight = (nFixedHeight <= 0 ? GetSwitchListItemHeight() : nFixedHeight);

			CDUIRect rcModal = pItemLast->GetModalParentRect();
			rcModal.Offset(CDUISize(rcItemRange.left - szScrollPos.cx - rcModal.left, GetChildPaddingV()));
			rcModal.top = rcModal.bottom;
			rcModal.bottom = rcModal.top + nFixedHeight;

			pItem->OnDuiSize(rcModal);
			pItemLast = pItem;
		}

		CDUIRect rcItem = pItem->GetAbsoluteRect();
		if (bFindFirst)
		{
			if (rcItem.bottom >= rcItemRange.top)
			{
				m_nFirstShowIndex = n;
				m_nLastShowIndex = m_nFirstShowIndex;
				bFindFirst = false;
			}

			continue;
		}

		m_nLastShowIndex = n;

		if (rcItem.bottom >= rcItemRange.bottom) break;
	}

	return;
}

void CDUIListViewCtrl::ScrollChildsDownOnListType(CDUISize szScroll)
{
	CDUIRect rcItemRange = GetLayoutRangeOfItem();
	CDUISize szScrollPos = GetScrollPos();

	//no float
	bool bFindLast = true;
	CDUIListItemCtrl *pItemLast = NULL;
	for (int n = m_nLastShowIndex; n >= 0; n--)
	{
		CDUIListItemCtrl *pItem = GetChildAt(n);
		if (NULL == pItem || false == pItem->IsVisible()) continue;

		if (n == m_nLastShowIndex)
		{
			pItemLast = pItem;

			CDUIRect rcModal = pItem->GetModalParentRect();
			rcModal.Offset(CDUISize(-szScroll.cx, -szScroll.cy));
			pItem->OnDuiSize(rcModal);
		}
		else if (pItemLast)
		{
			int nFixedHeight = pItem->GetFixedHeight();
			nFixedHeight = (nFixedHeight <= 0 ? GetSwitchListItemHeight() : nFixedHeight);

			CDUIRect rcModal = pItemLast->GetModalParentRect();
			rcModal.Offset(CDUISize(rcItemRange.left - szScrollPos.cx - rcModal.left, -GetChildPaddingV()));
			rcModal.bottom = rcModal.top;
			rcModal.top = rcModal.bottom - nFixedHeight;

			pItem->OnDuiSize(rcModal);
			pItemLast = pItem;
		}

		CDUIRect rcItem = pItem->GetAbsoluteRect();
		if (bFindLast)
		{
			if (rcItem.top <= rcItemRange.bottom)
			{
				m_nLastShowIndex = n;
				m_nFirstShowIndex = m_nLastShowIndex;
				bFindLast = false;
			}

			continue;
		}

		m_nFirstShowIndex = n;

		if (rcItem.top <= rcItemRange.top) break;
	}

	return;
}

int CDUIListViewCtrl::FindSelectableOnListType(int nIndexCur, bool bPositive) const
{
	if (GetChildCount() <= 0) return -1;

	if (bPositive)
	{
		for (int i = nIndexCur + 1; i < GetChildCount(); i++)
		{
			CDUIListItemCtrl *pItem = GetChildAt(i);
			if (pItem
				&& pItem->IsVisible()
				&& pItem->IsEnabled()) return i;
		}

		return -1;
	}
	else
	{
		for (int i = nIndexCur - 1; i >= 0; --i)
		{
			CDUIListItemCtrl *pItem = GetChildAt(i);
			if (pItem
				&& pItem->IsVisible()
				&& pItem->IsEnabled()) return i;
		}
	}

	return -1;
}

void CDUIListViewCtrl::InsertChildOnTileTypeH(CDUIListItemCtrl *pItem)
{
	if (NULL == pItem) return;

	return;
}

void CDUIListViewCtrl::InsertChildOnTileTypeV(CDUIListItemCtrl *pItem)
{
	if (NULL == pItem) return;

	return;
}

void CDUIListViewCtrl::RefreshViewOnTileTypeH()
{
	//scroll bar
	RefreshScrollBar();

	CalcShowItemOnTileTypeH();

	return;
}

void CDUIListViewCtrl::RefreshViewOnTileTypeV()
{
	//scroll bar
	RefreshScrollBar();

	CalcShowItemOnTileTypeV();

	return;
}

void CDUIListViewCtrl::CalcShowItemOnTileTypeH()
{
	CDUISize szScrollPos = GetScrollPos();
	CDUIRect rcItemRange = GetLayoutRangeOfItem();
	SIZE szItem = GetSwitchTileItemSize();
	CDUIRect rcRangeScroll(rcItemRange.left, rcItemRange.top - rcItemRange.GetHeight(), rcItemRange.right, rcItemRange.bottom + rcItemRange.GetHeight());

	//column
	CalcVisibleCount();
	CalcTileTypeInfo();

	bool bFindFirst = true;
	bool bFindLast = true;
	int nCurScrollRangeY = 0;
	m_nFirstShowIndex = m_nLastShowIndex = -1;
	for (int n = 0, m = 0; n < GetChildCount(); n++)
	{
		CDUIListItemCtrl *pItem = GetChildAt(n);
		if (NULL == pItem || false == pItem->IsVisible()) continue;

		//float
		if (pItem->IsFloat())
		{
			CDUIRect rcModal = rcItemRange;
			rcModal.Offset(GetScrollPos());
			pItem->OnDuiSize(rcModal);

			continue;
		}

		//item pos
		int nColumn = m % m_nColumn;
		int nRow = m / m_nColumn;
		int nTopOffset = (IsTopFromRange() ? 0 : GetChildPaddingV() / 2);
		int nTop = rcItemRange.top + nRow * m_nHeightRow + nTopOffset - szScrollPos.cy;
		int nBottom = nTop + (pItem->GetFixedHeight() <= 0 ? m_nHeightRow - GetChildPaddingV() : pItem->GetFixedHeight());	
		nCurScrollRangeY = max(nCurScrollRangeY, nBottom + szScrollPos.cy + GetChildPaddingV() / 2 - rcItemRange.top);
		m++;

		CDUIRect rcModal = rcItemRange;
		int nLeftOffset = (IsLeftFromRange() ? 0 : GetChildPaddingH() / 2);
		rcModal.left += nColumn * m_nWidthColumn + nLeftOffset;
		rcModal.right = rcModal.left + m_nWidthColumn - GetChildPaddingH();
		rcModal.top = nTop;
		rcModal.bottom = nBottom;
		pItem->OnDuiSize(rcModal);

		CDUIRect rcItem = pItem->GetAbsoluteRect();
		if (bFindFirst)
		{
			m_nFirstShowIndex = n;

			if (rcItem.bottom >= rcRangeScroll.top)
			{
				bFindFirst = false;
			}
		}
		if (bFindLast)
		{
			m_nLastShowIndex = n;

			if (rcItem.top >= rcRangeScroll.bottom)
			{
				bFindLast = false;
			}
		}
	}

	m_szTotalRange = { rcItemRange.GetWidth(), nCurScrollRangeY };
	UpdateScrollBar({ 0, max(0, nCurScrollRangeY - rcItemRange.GetHeight()) });

	return;
}

void CDUIListViewCtrl::CalcShowItemOnTileTypeV()
{
	CDUISize szScrollPos = GetScrollPos();
	CDUIRect rcItemRange = GetLayoutRangeOfItem();
	SIZE szItem = GetSwitchTileItemSize();
	CDUIRect rcRangeScroll(rcItemRange.left - rcItemRange.GetWidth(), rcItemRange.top, rcItemRange.right + rcItemRange.GetWidth(), rcItemRange.bottom);

	//column
	CalcVisibleCount();
	CalcTileTypeInfo();

	bool bFindFirst = true;
	bool bFindLast = true;
	int nCurScrollRangeX = 0;
	m_nFirstShowIndex = m_nLastShowIndex = -1;
	for (int n = 0, m = 0; n < GetChildCount(); n++)
	{
		CDUIListItemCtrl *pItem = GetChildAt(n);
		if (NULL == pItem || false == pItem->IsVisible()) continue;

		//float
		if (pItem->IsFloat())
		{
			CDUIRect rcModal = rcItemRange;
			rcModal.Offset(GetScrollPos());
			pItem->OnDuiSize(rcModal);

			continue;
		}

		//item pos
		int nRow = m % m_nRow;
		int nColumn = m / m_nRow;
		int nLeftOffset = (IsLeftFromRange() ? 0 : GetChildPaddingH() / 2);
		int nLeft = rcItemRange.left + nColumn * m_nWidthColumn + nLeftOffset - szScrollPos.cx;
		int nRight = nLeft + (pItem->GetFixedWidth() <= 0 ? m_nWidthColumn - GetChildPaddingH() : pItem->GetFixedWidth());
		nCurScrollRangeX = max(nCurScrollRangeX, nRight + szScrollPos.cx + GetChildPaddingH() / 2 - rcItemRange.left);
		m++;

		CDUIRect rcModal = rcItemRange;
		int nTopOffset = IsTopFromRange() ? 0 : GetChildPaddingV() / 2;
		rcModal.left = nLeft;
		rcModal.right = nRight;
		rcModal.top += nRow * m_nHeightRow + nTopOffset;
		rcModal.bottom = rcModal.top + m_nHeightRow - GetChildPaddingV();
		pItem->OnDuiSize(rcModal);

		CDUIRect rcItem = pItem->GetAbsoluteRect();
		if (bFindFirst)
		{
			m_nFirstShowIndex = n;

			if (rcItem.right >= rcRangeScroll.left)
			{
				bFindFirst = false;
			}
		}
		if (bFindLast)
		{
			m_nLastShowIndex = n;

			if (rcItem.left >= rcRangeScroll.right)
			{
				bFindLast = false;
			}
		}
	}

	m_szTotalRange = { nCurScrollRangeX, rcItemRange.GetHeight() };
	UpdateScrollBar({ max(0, nCurScrollRangeX - rcItemRange.GetWidth()), 0 });

	return;
}

void CDUIListViewCtrl::CalcVisibleCount()
{
	m_nVisibleCount = 0;

	for (int n = 0; n < GetChildCount(); n++)
	{
		CDUIListItemCtrl *pItem = GetChildAt(n);
		if (NULL == pItem || false == pItem->IsVisible()) continue;

		m_nVisibleCount++;
	}

	return;
}

void CDUIListViewCtrl::CalcTileTypeInfo()
{
	CDUIRect rcItemRange = GetLayoutRangeOfItem();
	SIZE szItem = GetSwitchTileItemSize();
	enDuiListViewType ListViewType = GetListViewType();

	int nChildPaddingH = IsAutoCalcChildPaddingH() ? 1 : GetChildPaddingH();
	int nChildPaddingV = IsAutoCalcChildPaddingV() ? 1 : GetChildPaddingV();
	m_nWidthColumn = szItem.cx + nChildPaddingH;
	m_nWidthColumn = max(1, m_nWidthColumn);
	m_nHeightRow = szItem.cy + nChildPaddingV;
	m_nHeightRow = max(1, m_nHeightRow);

	//row column
	if (ListView_TileH == ListViewType)
	{
		m_nColumn = rcItemRange.GetWidth() / m_nWidthColumn;
		m_nColumn = max(1, m_nColumn);
		m_nRow = (m_nVisibleCount + m_nColumn - 1) / m_nColumn;
		m_nRow = max(1, m_nRow);
	}
	if (ListView_TileV == ListViewType)
	{
		m_nRow = rcItemRange.GetHeight() / m_nHeightRow;
		m_nRow = max(1, m_nRow);
		m_nColumn = (m_nVisibleCount + m_nRow - 1) / m_nRow;
		m_nColumn = max(1, m_nColumn);
	}

	if (IsAutoCalcItemWidth())
	{
		int nWidthSurplus = rcItemRange.GetWidth() % m_nWidthColumn;
		m_nWidthColumn += nWidthSurplus / m_nColumn;
	}
	if (IsAutoCalcChildPaddingH())
	{
		int nWidthSurplus = rcItemRange.GetWidth() % m_nWidthColumn;
		m_nWidthColumn -= nChildPaddingH;
		nChildPaddingH += nWidthSurplus / m_nColumn;
		m_nWidthColumn += nChildPaddingH;

		SetChildPadding(nChildPaddingH, GetChildPaddingV());
	}

	if (IsAutoCalcItemHeight())
	{
		int nHeightSurplus = rcItemRange.GetHeight() % m_nHeightRow;
		m_nHeightRow += nHeightSurplus / m_nRow;
	}
	if (IsAutoCalcChildPaddingV())
	{
		int nHeightSurplus = rcItemRange.GetHeight() % m_nHeightRow;
		m_nHeightRow -= nChildPaddingV;
		nChildPaddingV += nHeightSurplus / m_nRow;
		m_nHeightRow += nChildPaddingV;

		SetChildPadding(GetChildPaddingH(), nChildPaddingV);
	}

	return;
}

void CDUIListViewCtrl::ScrollChildsUpOnTileTypeH(CDUISize szScroll)
{
	CDUIRect rcItemRange = GetLayoutRangeOfItem();
	CDUISize szScrollPos = GetScrollPos();
	CDUISize szItem = GetSwitchTileItemSize();
	CDUIRect rcRangeScroll(rcItemRange.left, rcItemRange.top - rcItemRange.GetHeight(), rcItemRange.right, rcItemRange.bottom + rcItemRange.GetHeight());

	//no float
	bool bFindFirst = true;
	CDUIListItemCtrl *pItemLast = NULL;
	for (int n = m_nFirstShowIndex; n < GetChildCount(); n++)
	{
		CDUIListItemCtrl *pItem = GetChildAt(n);
		if (NULL == pItem || false == pItem->IsVisible()) continue;

		if (n == m_nFirstShowIndex)
		{
			pItemLast = pItem;

			CDUIRect rcModal = pItem->GetModalParentRect();
			rcModal.Offset(CDUISize(-szScroll.cx, -szScroll.cy));
			pItem->OnDuiSize(rcModal);
		}
		else if (pItemLast)
		{
			CDUIRect rcModal = pItemLast->GetModalParentRect();
			rcModal.Offset(m_nWidthColumn, 0);
			if (rcModal.right > rcItemRange.right)
			{
				rcModal.Offset(0, m_nHeightRow);
				rcModal.left = rcItemRange.left + GetChildPaddingH() / 2;
				rcModal.right = rcModal.left + m_nWidthColumn - GetChildPaddingH();
			}

			pItem->OnDuiSize(rcModal);
			pItemLast = pItem;
		}

		CDUIRect rcItem = pItem->GetAbsoluteRect();
		if (bFindFirst)
		{
			if (rcItem.bottom >= rcRangeScroll.top)
			{
				m_nFirstShowIndex = n;
				m_nLastShowIndex = m_nFirstShowIndex;
				bFindFirst = false;
			}

			continue;
		}

		m_nLastShowIndex = n;

		if (rcItem.bottom >= rcRangeScroll.bottom) break;
	}

	return;
}

void CDUIListViewCtrl::ScrollChildsDownOnTileTypeH(CDUISize szScroll)
{
	CDUIRect rcItemRange = GetLayoutRangeOfItem();
	CDUISize szScrollPos = GetScrollPos();
	CDUISize szItem = GetSwitchTileItemSize();
	CDUIRect rcRangeScroll(rcItemRange.left, rcItemRange.top - rcItemRange.GetHeight(), rcItemRange.right, rcItemRange.bottom + rcItemRange.GetHeight());

	//no float
	bool bFindLast = true;
	CDUIListItemCtrl *pItemLast = NULL;
	for (int n = m_nLastShowIndex; n >= 0; n--)
	{
		CDUIListItemCtrl *pItem = GetChildAt(n);
		if (NULL == pItem || false == pItem->IsVisible()) continue;

		if (n == m_nLastShowIndex)
		{
			pItemLast = pItem;

			CDUIRect rcModal = pItem->GetModalParentRect();
			rcModal.Offset(CDUISize(-szScroll.cx, -szScroll.cy));
			pItem->OnDuiSize(rcModal);
		}
		else if (pItemLast)
		{
			CDUIRect rcModal = pItemLast->GetModalParentRect();
			rcModal.Offset(-m_nWidthColumn, 0);
			if (rcModal.left < rcItemRange.left)
			{
				rcModal.Offset(0, -m_nHeightRow);
				rcModal.left = rcItemRange.left + m_nWidthColumn * (m_nColumn - 1) + GetChildPaddingH() / 2;
				rcModal.right = rcModal.left + m_nWidthColumn - GetChildPaddingH();
			}

			pItem->OnDuiSize(rcModal);
			pItemLast = pItem;
		}

		CDUIRect rcItem = pItem->GetAbsoluteRect();
		if (bFindLast)
		{
			if (rcItem.top <= rcRangeScroll.bottom)
			{
				m_nLastShowIndex = n;
				m_nFirstShowIndex = m_nLastShowIndex;
				bFindLast = false;
			}

			continue;
		}

		m_nFirstShowIndex = n;

		if (rcItem.top <= rcRangeScroll.top) break;
	}

	return;
}

void CDUIListViewCtrl::ScrollChildsUpOnTileTypeV(CDUISize szScroll)
{
	CDUIRect rcItemRange = GetLayoutRangeOfItem();
	CDUISize szScrollPos = GetScrollPos();
	CDUISize szItem = GetSwitchTileItemSize();
	CDUIRect rcRangeScroll(rcItemRange.left - rcItemRange.GetWidth(), rcItemRange.top, rcItemRange.right + rcItemRange.GetWidth(), rcItemRange.bottom);

	//no float
	bool bFindFirst = true;
	CDUIListItemCtrl *pItemLast = NULL;
	for (int n = m_nFirstShowIndex; n < GetChildCount(); n++)
	{
		CDUIListItemCtrl *pItem = GetChildAt(n);
		if (NULL == pItem || false == pItem->IsVisible()) continue;

		if (n == m_nFirstShowIndex)
		{
			pItemLast = pItem;

			CDUIRect rcModal = pItem->GetModalParentRect();
			rcModal.Offset(CDUISize(-szScroll.cx, -szScroll.cy));
			pItem->OnDuiSize(rcModal);
		}
		else if (pItemLast)
		{
			CDUIRect rcModal = pItemLast->GetModalParentRect();
			rcModal.Offset(0, m_nHeightRow);
			if (rcModal.bottom > rcItemRange.bottom)
			{
				rcModal.Offset(m_nWidthColumn, 0);
				rcModal.top = rcItemRange.top + GetChildPaddingV() / 2;
				rcModal.bottom = rcModal.top + m_nHeightRow - GetChildPaddingV();
			}

			pItem->OnDuiSize(rcModal);
			pItemLast = pItem;
		}

		CDUIRect rcItem = pItem->GetAbsoluteRect();
		if (bFindFirst)
		{
			if (rcItem.right >= rcRangeScroll.left)
			{
				m_nFirstShowIndex = n;
				m_nLastShowIndex = m_nFirstShowIndex;
				bFindFirst = false;
			}

			continue;
		}

		m_nLastShowIndex = n;

		if (rcItem.right >= rcRangeScroll.right) break;
	}

	return;
}

void CDUIListViewCtrl::ScrollChildsDownOnTileTypeV(CDUISize szScroll)
{
	CDUIRect rcItemRange = GetLayoutRangeOfItem();
	CDUISize szScrollPos = GetScrollPos();
	CDUISize szItem = GetSwitchTileItemSize();
	CDUIRect rcRangeScroll(rcItemRange.left - rcItemRange.GetWidth(), rcItemRange.top, rcItemRange.right + rcItemRange.GetWidth(), rcItemRange.bottom);

	//no float
	bool bFindLast = true;
	CDUIListItemCtrl *pItemLast = NULL;
	for (int n = m_nLastShowIndex; n >= 0; n--)
	{
		CDUIListItemCtrl *pItem = GetChildAt(n);
		if (NULL == pItem || false == pItem->IsVisible()) continue;

		if (n == m_nLastShowIndex)
		{
			pItemLast = pItem;

			CDUIRect rcModal = pItem->GetModalParentRect();
			rcModal.Offset(CDUISize(-szScroll.cx, -szScroll.cy));
			pItem->OnDuiSize(rcModal);
		}
		else if (pItemLast)
		{
			CDUIRect rcModal = pItemLast->GetModalParentRect();
			rcModal.Offset(0, -m_nHeightRow);
			if (rcModal.top < rcItemRange.top)
			{
				rcModal.Offset(-m_nWidthColumn, 0);
				rcModal.top = rcItemRange.top + m_nHeightRow * (m_nRow - 1) + GetChildPaddingV() / 2;
				rcModal.bottom = rcModal.top + m_nHeightRow - GetChildPaddingV();
			}

			pItem->OnDuiSize(rcModal);
			pItemLast = pItem;
		}

		CDUIRect rcItem = pItem->GetAbsoluteRect();
		if (bFindLast)
		{
			if (rcItem.left <= rcRangeScroll.right)
			{
				m_nLastShowIndex = n;
				m_nFirstShowIndex = m_nLastShowIndex;
				bFindLast = false;
			}

			continue;
		}

		m_nFirstShowIndex = n;

		if (rcItem.left <= rcRangeScroll.left) break;
	}

	return;
}

int CDUIListViewCtrl::FindSelectableOnTileTypeH(WORD wKey)
{
	if (-1 == GetCurSel())
	{
		return FindNextIndex(-1, true);
	}
	if (VK_LEFT == wKey || VK_RIGHT == wKey)
	{
		CDUIListItemCtrl *pItemCur = GetChildAt(GetCurSel());
		if (NULL == pItemCur) return -1;

		int nIndex = FindNextIndex(GetCurSel(), VK_RIGHT == wKey ? true : false);
		CDUIListItemCtrl *pItemNext = GetChildAt(nIndex);
		if (NULL == pItemNext) return -1;

		if (pItemCur->GetModalParentRect().top != pItemNext->GetModalParentRect().top) return -1;

		return nIndex;
	}
	if (VK_UP == wKey || VK_DOWN == wKey)
	{
		int nCount = m_nColumn, nIndex = GetCurSel();
		while (nCount-- > 0)
		{
			nIndex = FindNextIndex(nIndex, VK_DOWN == wKey ? true : false);
			CDUIListItemCtrl *pItem = GetChildAt(nIndex);
			if (NULL == pItem) return -1;
		}

		return nIndex;
	}

	return -1;
}

int CDUIListViewCtrl::FindSelectableOnTileTypeV(WORD wKey)
{
	int nCurSel = GetCurSel();
	CDUIPoint ptRowColumn = TranslateRowColumn(nCurSel);

	if (-1 == nCurSel)
	{
		return FindNextIndex(-1, true);
	}
	if (VK_UP == wKey || VK_DOWN == wKey)
	{
		if (VK_UP == wKey && 0 == ptRowColumn.x) return -1;
		if (VK_DOWN == wKey && GetRowCount() - 1 == ptRowColumn.x) return -1;

		int nFindCount = 3;
		while (nFindCount > 0)
		{
			bool bPositive = VK_DOWN == wKey ? true : false;
			1 == nFindCount ? bPositive = !bPositive : bPositive;
			int nIndex = FindNextIndex(nCurSel, bPositive, DuiFind_Visible | DuiFind_Enabled, 3 == nFindCount ? 1 : GetRowCount());
			if (-1 == nIndex)
			{
				nFindCount--;

				continue;
			}
			if (3 != nFindCount)
			{
				nIndex = FindNextIndex(nIndex, VK_DOWN == wKey ? true : false);
			}

			CDUIListItemCtrl *pItemNext = GetChildAt(nIndex);
			if (pItemNext) return nIndex;

			nFindCount--;
		}

		return -1;
	}
	if (VK_LEFT == wKey || VK_RIGHT == wKey)
	{
		int nIndex = FindNextIndex(nCurSel, VK_RIGHT == wKey ? true : false, DuiFind_Visible | DuiFind_Enabled, GetRowCount());
		CDUIListItemCtrl *pItemNext = GetChildAt(nIndex);
		if (pItemNext) return nIndex;

		return -1;
	}

	return -1;
}
