#include "StdAfx.h"
#include "DUIListHeaderCtrl.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateControl(CDUIListHeaderCtrl)
MMImplement_ClassName(CDUIListHeaderCtrl)

CDUIListHeaderCtrl::CDUIListHeaderCtrl()
{
	return;
}

CDUIListHeaderCtrl::~CDUIListHeaderCtrl(void)
{
	if (m_pHeaderItemSelectCtrl)
	{
		m_pHeaderItemSelectCtrl->UnRegisterControlCallBack(this);
	}
	if (m_pCheckSelectCtrl)
	{
		m_pCheckSelectCtrl->UnRegisterControlCallBack(this);
	}

	m_pHeaderItemSelectCtrl = NULL;
	m_pCheckSelectCtrl = NULL;
	m_pOwner = NULL;

	return;
}

bool CDUIListHeaderCtrl::OnAttributeChange(CDUIAttributeObject *pAttributeObj)
{
	if (__super::OnAttributeChange(pAttributeObj)) return true;

	if (pAttributeObj == &m_AttributeItemInitCount)
	{
		InitChild();

		return true;
	}

	return false;
}

void CDUIListHeaderCtrl::OnNotify(CDUIControlBase *pControl, const DuiNotify &Notify)
{
	if (pControl == m_pHeaderItemSelectCtrl)
	{
		switch (Notify.NotifyType)
		{
			case DuiNotify_CtrlIDChanged:
			{
				if (m_pHeaderItemSelectCtrl->GetCtrlID() != Dui_CtrlIDInner_HeaderItemSelect)
				{
					m_pHeaderItemSelectCtrl->SetCtrlID(Dui_CtrlIDInner_HeaderItemSelect);
				}

				break;
			}
			default:
			{
				break;
			}
		}

		return;
	}
	if (pControl == m_pCheckSelectCtrl)
	{
		switch (Notify.NotifyType)
		{
			case DuiNotify_SelectChanged:
			{
				if (NULL == GetOwner()) break;
				
				if (m_pCheckSelectCtrl->IsSelected())
				{
					GetOwner()->SelectAllItems();

					break;
				}

				GetOwner()->UnSelectAllItems();

				break;
			}
			default:
			{
				break;
			}
		}

		return;
	}

	return;
}

void CDUIListHeaderCtrl::OnSize(CDUIControlBase *pControl)
{
	if (NULL == m_pHeaderItemSelectCtrl || NULL == GetOwner()) return;

	if (pControl == m_pHeaderItemSelectCtrl)
	{
		int nWidth = GetOwner()->GetSelectIconLeftPadding() * 2 + GetOwner()->GetSelectIconFixedWidth();
		m_pHeaderItemSelectCtrl->SetFixedWidth(nWidth);

		return;
	}

	return;
}

void CDUIListHeaderCtrl::OnVisibleChanged(CDUIControlBase *pControl)
{
	if (NULL == m_pHeaderItemSelectCtrl || NULL == GetOwner()) return;

	if (pControl == m_pHeaderItemSelectCtrl)
	{
		return;
	}

	return;
}

int CDUIListHeaderCtrl::GetChildCount() const
{
	return __super::GetChildCount();
}

bool CDUIListHeaderCtrl::InsertChild(CDUIControlBase *pChild, int nPos)
{
	MMInterfaceHelper(CDUIListHeaderItemCtrl, pChild, pHeaderItem);
	if (NULL == pHeaderItem) return false;

	if (false == __super::InsertChild(pChild, nPos)) return false;

	if (pHeaderItem)
	{
		pHeaderItem->SetOwner(this);
	}

	return true;
}

CDUIListHeaderItemCtrl * CDUIListHeaderCtrl::GetChildAt(int nIndex) const
{
	return static_cast<CDUIListHeaderItemCtrl*>(CDUIContainerCtrl::GetChildAt(nIndex));
}

bool CDUIListHeaderCtrl::RemoveAt(int nIndex)
{
	if (0 == nIndex) return false;

	return __super::RemoveAt(nIndex);
}

LPVOID CDUIListHeaderCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIListHeaderCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIListHeaderCtrl::GetDescribe() const
{
	return Dui_Ctrl_ListHeader;
}

CDUIListViewCtrl * CDUIListHeaderCtrl::GetOwner()
{
	return m_pOwner;
}

void CDUIListHeaderCtrl::SetOwner(CDUIListViewCtrl *pOwner)
{
	m_pOwner = MMInterfaceHelper(CDUIListViewCtrl, pOwner);

	if (NULL == GetOwner() || NULL == m_pHeaderItemSelectCtrl) return;

	int nWidth = GetOwner()->GetSelectIconLeftPadding() * 2 + GetOwner()->GetSelectIconFixedWidth();
	m_pHeaderItemSelectCtrl->SetFixedWidth(nWidth);

	return;
}

void CDUIListHeaderCtrl::SetScaleHeader(bool bScale)
{
	if (bScale == m_AttributeScaleItem.GetValue()) return;

	m_AttributeScaleItem.SetValue(bScale);

	if (IsScaleHeader())
	{
		int nHeight = GetHeight();
		SetPadding(0, 0, 0, 0);
		SetFixedHeight(nHeight);
	}

	NeedRefreshView();

	return;
}

bool CDUIListHeaderCtrl::IsScaleHeader()
{
	return m_AttributeScaleItem.GetValue();
}

float CDUIListHeaderCtrl::GetScaleValue(int nWidth)
{
	if (false == IsScaleHeader()) return 0;

	CDUIRect rcRange = GetLayoutRangeOfItem();
	if (rcRange.GetWidth() <= 0) return 0;

	return nWidth * 1.0 / rcRange.GetWidth();
}

int CDUIListHeaderCtrl::GetColumnCount()
{
	return m_nColumn;
}

CDUIRect CDUIListHeaderCtrl::GetColumnRect(int nIndex)
{
	if (nIndex < 0 || nIndex >= MMCountArray(m_rcColumn)) return CDUIRect();

	return m_rcColumn[nIndex];
}

int CDUIListHeaderCtrl::GetItemInitCount()
{
	return m_AttributeItemInitCount.GetValue();
}

void CDUIListHeaderCtrl::SetIteminitCount(int nCount)
{
	if (nCount == GetItemInitCount()) return;

	m_AttributeItemInitCount.SetValue(nCount);

	return;
}

void CDUIListHeaderCtrl::InitChild()
{
	int nCount = m_AttributeItemInitCount.GetValue();
	if (nCount < 0) return;

	RemoveAll();
	InsertChild(nCount);

	return;
}

bool CDUIListHeaderCtrl::InsertChild(int nItemCount, int nPos)
{
	if (nItemCount <= 0) return false;

	for (int n = 0; n < nItemCount; n++)
	{
		CDUIListHeaderItemCtrl *pItem = new CDUIListHeaderItemCtrl();
		if (NULL == pItem)
		{
			assert(false);
			return false;
		}

		pItem->Init();

		if (false == InsertChildAtUser(pItem, nPos))
		{
			assert(false);
			return false;
		}
	}

	return true;
}

bool CDUIListHeaderCtrl::InsertChildAtUser(CDUIControlBase *pChild, int nPos)
{
	return InsertChild(pChild, -1 != nPos ? nPos + 1 : nPos);
}

int CDUIListHeaderCtrl::GetChildCountAtUser()
{
	return max(GetChildCount() - 1, 0);
}

CDUIListHeaderItemCtrl * CDUIListHeaderCtrl::GetChildAtUser(int nIndex)
{
	return GetChildAt(nIndex + 1);
}

CDUIListHeaderItemCtrl * CDUIListHeaderCtrl::GetHeaderItemSelect()
{
	return m_pHeaderItemSelectCtrl;
}

bool CDUIListHeaderCtrl::RemoveAtUser(int nIndex)
{
	return RemoveAt(nIndex + 1);
}

bool CDUIListHeaderCtrl::Remove(CDUIControlBase *pControl)
{
	if (pControl == m_pHeaderItemSelectCtrl) return false;

	return __super::Remove(pControl);
}

void CDUIListHeaderCtrl::RemoveAll()
{
	for (int n = GetChildCount() - 1; n >= 0; n--)
	{
		CDUIControlBase *pControl = GetChildAt(n);
		if (pControl == m_pHeaderItemSelectCtrl) continue;

		RemoveAt(n);
	}

	return;
}

void CDUIListHeaderCtrl::RefreshView()
{
	CDUIRect rcRange = GetLayoutRangeOfItem();

	//scale
	if (IsScaleHeader())
	{
		for (int nIndex = 0; nIndex < GetChildCount(); nIndex++)
		{
			CDUIListHeaderItemCtrl *pChild = GetChildAt(nIndex);
			if (NULL == pChild) continue;

			int nWidth = int(rcRange.GetWidth() * pChild->GetScale());
			pChild->SetFixedWidth(nWidth);
		}
	}

	//refresh
	__super::RefreshView();

	//column
	m_nColumn = 0;
	for (int i = 0; i < GetChildCount() && i < Dui_Max_ListColumn; i++)
	{
		CDUIListHeaderItemCtrl *pChild = GetChildAt(i);
		if (NULL == pChild || false == pChild->IsVisible() || pChild->IsFloat()) continue;

		m_rcColumn[m_nColumn++] = pChild->GetAbsoluteRect();
	}

	return;
}

void CDUIListHeaderCtrl::OnVisibleChangedSelectIcon()
{
	if (NULL == GetOwner() || NULL == m_pHeaderItemSelectCtrl) return;

	m_pHeaderItemSelectCtrl->SetVisible(GetOwner()->IsSelectIconVisible());

	return;
}

void CDUIListHeaderCtrl::OnSelectChangedListItem()
{
	if (NULL == GetOwner() || NULL == m_pCheckSelectCtrl) return;

	m_pCheckSelectCtrl->Select(GetOwner()->GetSelectItemCount() == GetOwner()->GetChildCount(), false);

	return;
}

void CDUIListHeaderCtrl::InitProperty()
{
	__super::InitProperty();

	DuiCreateGroupAttribute(m_AttributeGroupHeader, _T("Header"));
	DuiCreateAttribute(m_AttributeScaleItem, _T("UseScaleItem"), _T("item width is use scal of header's width"), m_AttributeGroupHeader);
	DuiCreateAttribute(m_AttributeItemInitCount, _T("ItemInitCount"), _T("init item count"), m_AttributeGroupHeader);

	//value
	DuiInitAttriValue(m_AttributeAutoCalcChildPaddingH, true);

	return;
}

void CDUIListHeaderCtrl::InitComplete()
{
	__super::InitComplete();

	DuiInitAttriVisible(m_AttributeGroupScroll, false);

	if (NULL == m_pHeaderItemSelectCtrl)
	{
		do
		{
			m_pHeaderItemSelectCtrl = MMInterfaceHelper(CDUIListHeaderItemCtrl, FindSubControlThisView(Dui_CtrlIDInner_HeaderItemSelect));
			if (NULL == m_pHeaderItemSelectCtrl)
			{
				m_pHeaderItemSelectCtrl = new CDUIListHeaderItemCtrl;
				m_pHeaderItemSelectCtrl->Init();
				m_pHeaderItemSelectCtrl->SetCtrlID(Dui_CtrlIDInner_HeaderItemSelect);
				m_pHeaderItemSelectCtrl->SetVisible(GetOwner() && GetOwner()->IsSelectIconVisible());
				m_pHeaderItemSelectCtrl->SetSepWidth(0);
				InsertChild(m_pHeaderItemSelectCtrl, 0);
			}

			m_pCheckSelectCtrl = MMInterfaceHelper(CDUICheckBoxCtrl, m_pHeaderItemSelectCtrl->FindSubControlThisView(Dui_CtrlIDInner_HeaderItemCheck));
			if (NULL == m_pCheckSelectCtrl)
			{
				m_pCheckSelectCtrl = new CDUICheckBoxCtrl();
				m_pCheckSelectCtrl->Init();
				m_pCheckSelectCtrl->SetCtrlID(Dui_CtrlIDInner_HeaderItemCheck);
				m_pCheckSelectCtrl->SetFloat(true);
				m_pCheckSelectCtrl->SetHorizAlignType(enDuiHorizAlignType::HorizAlign_Center);
				m_pCheckSelectCtrl->SetVertAlignType(enDuiVertAlignType::VertAlign_Middle);

				tagDuiImageSection ImageSection = m_pCheckSelectCtrl->GetImageSectionNormal();
				if (ImageSection.vecImageResSwitch.empty()) break;

				auto pImageBaseCheckBoxUnSelect = CDUIGlobal::GetInstance()->GetImageResource(ImageSection.vecImageResSwitch[0]);
				if (NULL == pImageBaseCheckBoxUnSelect) break;

				m_pCheckSelectCtrl->SetFixedWidth(pImageBaseCheckBoxUnSelect->GetWidth(GetScale()) / ImageSection.cbPartAll);
				m_pCheckSelectCtrl->SetFixedHeight(pImageBaseCheckBoxUnSelect->GetHeight(GetScale()));
				m_pHeaderItemSelectCtrl->InsertChild(m_pCheckSelectCtrl);
			}

		} while (false);
	}
	if (m_pHeaderItemSelectCtrl)
	{
		m_pHeaderItemSelectCtrl->RegisterControlCallBack(this);
	}
	if (m_pCheckSelectCtrl)
	{
		m_pCheckSelectCtrl->RegisterControlCallBack(this);
	}

	//no scrollbar
	EnableScrollBar(false, false);

	return;
}