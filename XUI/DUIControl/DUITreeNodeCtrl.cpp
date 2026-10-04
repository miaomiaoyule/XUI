#include "StdAfx.h"
#include "DUITreeNodeCtrl.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateControl(CDUITreeNodeCtrl)
MMImplement_ClassName(CDUITreeNodeCtrl)

CDUITreeNodeCtrl::CDUITreeNodeCtrl(void)
{
}

CDUITreeNodeCtrl::~CDUITreeNodeCtrl(void)
{
	if (m_pCheckExpandCtrl)
	{
		m_pCheckExpandCtrl->UnRegisterControlCallBack(this);
	}
	if (m_pHorizContainerCtrl)
	{
		m_pHorizContainerCtrl->UnRegisterControlCallBack(this);
	}

	m_pCheckExpandCtrl = NULL;
	m_pHorizContainerCtrl = NULL;
	m_pTreeViewCtrl = NULL;

	return;
}

void CDUITreeNodeCtrl::OnNotify(CDUIControlBase *pControl, const DuiNotify &Notify)
{
	//expand
	if (pControl == m_pCheckExpandCtrl)
	{
		switch (Notify.NotifyType)
		{
			case DuiNotify_SelectChanged:
			{
				OnSelectChangedExpand();

				break;
			}
		}

		return;
	}

	//select
	if (pControl == m_pCheckSelectCtrl)
	{
		switch (Notify.NotifyType)
		{
			case DuiNotify_SelectChanged:
			{
				OnSelectChangedSelect();

				break;
			}
		}

		return;
	}

	return __super::OnNotify(pControl, Notify);
}

void CDUITreeNodeCtrl::OnRelease(CDUIControlBase *pControl)
{
	if (pControl == m_pCheckExpandCtrl)
	{
		m_pCheckExpandCtrl->UnRegisterControlCallBack(this);
		m_pCheckExpandCtrl = NULL;
	}
	if (pControl == m_pHorizContainerCtrl)
	{
		m_pHorizContainerCtrl->UnRegisterControlCallBack(this);
		m_pHorizContainerCtrl = NULL;
	}

	return __super::OnRelease(pControl);
}

void CDUITreeNodeCtrl::OnPaint(CDUIControlBase *pControl, HDC hDC)
{
	return __super::OnPaint(pControl, hDC);
}

void CDUITreeNodeCtrl::OnSize(CDUIControlBase *pControl)
{
	if (pControl == m_pHorizContainerCtrl)
	{
		RefreshSubItem();
	}

	return __super::OnSize(pControl);
}

void CDUITreeNodeCtrl::OnVisibleChanged(CDUIControlBase *pControl)
{
	if (NULL == m_pCheckExpandCtrl || NULL == m_pCheckSelectCtrl) return;

	if (pControl == m_pCheckExpandCtrl)
	{
		OnVisibleChangedExpand();

		return;
	}

	OnVisibleChangedSelect();

	return;
}

void CDUITreeNodeCtrl::OnSelectChangedExpand()
{
	if (NULL == m_pWndOwner
		|| NULL == m_pHorizContainerCtrl
		|| NULL == m_pCheckExpandCtrl) return;

	SendNotify(m_pCheckExpandCtrl->IsSelected() ? DuiNotify_ItemExpand : DuiNotify_ItemUnExpand);

	CalcNodeHeight();

	return;
}

void CDUITreeNodeCtrl::OnSelectChangedSelect()
{
	if (NULL == m_pCheckSelectCtrl) return;

	//curnode
	if (false == IsSelected())
	{
		CDUITreeViewCtrl *pRootView = GetRootView();
		if (this == pRootView->GetCurSelNode())
		{
			pRootView->SetCurSelNode(NULL);
		}
	}

	//child node
	if (m_bSelectChild && false == IsSelectSingle())
	{
		SelectChildNode(IsSelected());
	}

	//parent sel
	CDUITreeNodeCtrl *pParentNode = GetParentNode();
	if (pParentNode
		&& false == IsSelectSingle()
		&& pParentNode->m_bSelectParent)
	{
		pParentNode->OnSelectChangedNode(IsSelected());
	}

	return;
}

void CDUITreeNodeCtrl::OnVisibleChangedExpand()
{
	if (NULL == m_pCheckExpandCtrl || NULL == m_pTreeViewCtrl) return;

	m_pTreeViewCtrl->SetExpandIconVisible(m_pCheckExpandCtrl->IsVisible());

	return;
}

void CDUITreeNodeCtrl::OnVisibleChangedSelect()
{
	if (NULL == m_pTreeViewCtrl) return;

	m_pTreeViewCtrl->SetSelectIconVisible(IsSelectIconVisible());

	return;
}

void CDUITreeNodeCtrl::SetOwner(CDUIListViewCtrl *pOwner)
{
	if (m_pOwner)
	{
		m_pOwner->UnRegisterControlCallBack(this);
	}

	__super::SetOwner(pOwner);

	if (NULL == m_pOwner) return;

	m_pOwner->RegisterControlCallBack(this);

	//tree view
#ifdef DUI_DESIGN
	if (NULL == m_pTreeViewCtrl)
	{
		ConstructTreeView();
	}
#endif

	return;
}

bool CDUITreeNodeCtrl::RemoveAt(int nIndex)
{
	if (nIndex < 3) return false;

	return __super::RemoveAt(nIndex);
}

LPVOID CDUITreeNodeCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUITreeNodeCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUITreeNodeCtrl::GetDescribe() const
{
	return Dui_Ctrl_TreeNode;
}

void CDUITreeNodeCtrl::NeedParentRefreshView()
{
	CDUITreeNodeCtrl *pParentNode = GetParentNode();
	if (pParentNode)
	{
		pParentNode->NeedRefreshView();

		return;
	}

	return __super::NeedParentRefreshView();
}

void CDUITreeNodeCtrl::RefreshView()
{
	__super::RefreshView();

	CalcNodeHeight();

	return;
}

void CDUITreeNodeCtrl::RefreshSubItem()
{
	CDUITreeViewCtrl *pRootView = GetRootView();

	if (NULL == pRootView
		|| NULL == m_pHorizContainerCtrl
		|| NULL == m_pCheckSelectCtrl
		|| false == pRootView->IsUseListHeader()) return;

	//header
	CDUIListHeaderCtrl *pListHeader = pRootView->GetListHeader();
	if (NULL == pListHeader) return;

	//headeritem sel
	CDUIListHeaderItemCtrl *pHeaderItemSelectCtrl = pListHeader->GetHeaderItemSelect();
	if (NULL == pHeaderItemSelectCtrl) return;

	//column width
	m_pCheckSelectCtrl->SetPaddingL(pRootView->GetSelectIconLeftPadding());
	m_pCheckSelectCtrl->SetFixedWidth(pRootView->GetSelectIconFixedWidth());
	for (int nColumn = pHeaderItemSelectCtrl->IsVisible() ? 1 : 0, j = 0; nColumn < pRootView->GetColumnCount(); nColumn++)
	{
		CDUIControlBase *pSubItemCtrl = NULL;
		for (; j < m_pHorizContainerCtrl->GetChildCount(); )
		{
			CDUIControlBase *pControl = m_pHorizContainerCtrl->GetChildAt(j++);
			if (NULL == pControl
				|| false == pControl->IsVisible()
				|| pControl == m_pCheckSelectCtrl
				|| pControl == m_pCheckExpandCtrl) continue;

			pSubItemCtrl = pControl;

			break;
		}

		if (NULL == pSubItemCtrl) return;

		CDUIRect rcColumn = pRootView->GetColumnRect(nColumn);
		CDUIRect rcItem = pSubItemCtrl->GetAbsoluteRect();
		int nWidth = rcColumn.right - rcItem.left;
		if (nWidth != pSubItemCtrl->GetFixedWidth()
			&& DuiDpiScaleBackCtrl(nWidth) != DuiDpiScaleBackCtrl(pSubItemCtrl->GetFixedWidth()))
		{
			pSubItemCtrl->SetFixedWidth(DuiDpiScaleBackCtrl(nWidth));
		}
	}

	return;
}

CDUITreeNodeCtrl * CDUITreeNodeCtrl::GetPrevSiblingCtrl()
{
	CDUITreeNodeCtrl *pParentNode = GetParentNode();
	if (NULL == pParentNode) return NULL;

	CDUITreeNodeCtrl *pTreeNode = pParentNode->GetChildNode(pParentNode->GetChildNodeIndex(this) - 1);
	return pTreeNode ? pTreeNode : pParentNode;
}

CDUITreeNodeCtrl * CDUITreeNodeCtrl::GetNextSiblingCtrl()
{
	CDUITreeNodeCtrl *pParentNode = GetParentNode();
	if (NULL == pParentNode) return NULL;

	CDUITreeNodeCtrl *pTreeNode = pParentNode->GetChildNode(pParentNode->GetChildNodeIndex(this) + 1);
	return pTreeNode ? pTreeNode : pParentNode->GetNextSiblingCtrl();
}

bool CDUITreeNodeCtrl::InsertChild(CDUIControlBase *pChild, int nPos /*= -1*/)
{
	//vert container
	if (NULL == m_pVertContainerCtrl
		&& pChild->GetCtrlID() == Dui_CtrlIDInner_TreeNodeVertContainer)
	{
		if (false == __super::InsertChild(pChild, nPos)) return false;

		m_pVertContainerCtrl = MMInterfaceHelper(CDUIVerticalLayoutCtrl, pChild);

		return true;
	}

	if (NULL == m_pVertContainerCtrl) return false;

	//tree view
	MMInterfaceHelper(CDUITreeNodeCtrl, pChild, pTreeNode);
	if (pTreeNode)
	{
		return InsertChildNode(pTreeNode, nPos);
	}

	//insert
	if (NULL == m_pHorizContainerCtrl) return false;

	return m_pHorizContainerCtrl->InsertChild(pChild, nPos);
}

bool CDUITreeNodeCtrl::Remove(CDUIControlBase *pControl)
{
	if (NULL == m_pVertContainerCtrl 
		|| pControl == m_pCheckExpandCtrl 
		|| pControl == m_pCheckSelectCtrl
		|| pControl == m_pEditTextCtrl) return false;

	if (m_pTreeViewCtrl && m_pTreeViewCtrl->Remove(pControl))
	{
		if (m_pTreeViewCtrl->GetChildCount() <= 0)
		{
			m_pVertContainerCtrl->Remove(m_pTreeViewCtrl);
		}

		return true;
	}

	if (NULL == m_pHorizContainerCtrl) return false;

	return m_pHorizContainerCtrl->Remove(pControl);
}

void CDUITreeNodeCtrl::RemoveAll()
{
	if (m_pHorizContainerCtrl)
	{
		for (int n = m_pHorizContainerCtrl->GetChildCount() - 1; n >= 0; n--)
		{
			CDUIControlBase *pControl = m_pHorizContainerCtrl->GetChildAt(n);
			if (pControl == m_pCheckExpandCtrl 
				|| pControl == m_pCheckSelectCtrl
				|| pControl == m_pEditTextCtrl) continue;

			m_pHorizContainerCtrl->RemoveAt(n);
		}
	}
	if (m_pTreeViewCtrl)
	{
		m_pTreeViewCtrl->RemoveAll();
	}

	return;
}

bool CDUITreeNodeCtrl::RemoveAtUser(int nIndex)
{
	if (NULL == m_pHorizContainerCtrl) return false;

	return m_pHorizContainerCtrl->RemoveAt(nIndex + 3);
}

int CDUITreeNodeCtrl::GetChildCountUser() const
{
	if (NULL == m_pHorizContainerCtrl) return 0;

	return max(m_pHorizContainerCtrl->GetChildCount() - 3, 0);
}

CDUIControlBase * CDUITreeNodeCtrl::GetChildAtUser(int nIndex) const
{
	if (NULL == m_pHorizContainerCtrl) return NULL;

	return m_pHorizContainerCtrl->GetChildAt(nIndex + 3);
}

void CDUITreeNodeCtrl::EnsureVisible(bool bCenter)
{
	CDUITreeViewCtrl *pRootView = GetRootView();
	if (NULL == pRootView) return;

	pRootView->EnsureVisible(this, bCenter);

	return;
}

bool CDUITreeNodeCtrl::IsSelected() const
{
	return __super::IsSelected();
}

bool CDUITreeNodeCtrl::Select(bool bSelect/* = true*/)
{
	return __super::Select(bSelect);
}

void CDUITreeNodeCtrl::SetSelectIconVisible(bool bVisible)
{
	__super::SetSelectIconVisible(bVisible);

	return;
}

void CDUITreeNodeCtrl::SetSelectIconLeftPadding(int nLeft)
{
	__super::SetSelectIconLeftPadding(nLeft);

	if (NULL == m_pTreeViewCtrl) return;

	m_pTreeViewCtrl->SetSelectIconLeftPadding(GetSelectIconLeftPadding());

	return;
}

void CDUITreeNodeCtrl::SetSelectIconFixedWidth(int nWidth)
{
	__super::SetSelectIconFixedWidth(nWidth);

	if (NULL == m_pTreeViewCtrl) return;

	m_pTreeViewCtrl->SetSelectIconFixedWidth(GetSelectIconFixedWidth());

	return;
}

void CDUITreeNodeCtrl::SetSelectIconFixedHeight(int nHeight)
{
	__super::SetSelectIconFixedHeight(nHeight);

	if (NULL == m_pTreeViewCtrl) return;

	m_pTreeViewCtrl->SetSelectIconFixedHeight(GetSelectIconFixedHeight());

	return;
}

void CDUITreeNodeCtrl::SetImageSectionSelIconNormal(const tagDuiImageSection &ImageSection)
{
	__super::SetImageSectionSelIconNormal(ImageSection);

	if (NULL == m_pTreeViewCtrl) return;

	m_pTreeViewCtrl->SetImageSectionSelIconNormal(GetImageSectionSelIconNormal());

	return;
}

void CDUITreeNodeCtrl::SetImageSectionSelIconHot(const tagDuiImageSection &ImageSection)
{
	__super::SetImageSectionSelIconHot(ImageSection);

	if (NULL == m_pTreeViewCtrl) return;

	m_pTreeViewCtrl->SetImageSectionSelIconHot(GetImageSectionSelIconHot());

	return;
}

void CDUITreeNodeCtrl::SetImageSectionSelIconPushed(const tagDuiImageSection &ImageSection)
{
	__super::SetImageSectionSelIconPushed(ImageSection);

	if (NULL == m_pTreeViewCtrl) return;

	m_pTreeViewCtrl->SetImageSectionSelIconPushed(GetImageSectionSelIconPushed());

	return;
}

void CDUITreeNodeCtrl::SetImageSectionSelIconSelNormal(const tagDuiImageSection &ImageSection)
{
	__super::SetImageSectionSelIconSelNormal(ImageSection);

	if (NULL == m_pTreeViewCtrl) return;

	m_pTreeViewCtrl->SetImageSectionSelIconSelNormal(GetImageSectionSelIconSelNormal());

	return;
}

void CDUITreeNodeCtrl::SetImageSectionSelIconSelHot(const tagDuiImageSection &ImageSection)
{
	__super::SetImageSectionSelIconSelHot(ImageSection);

	if (NULL == m_pTreeViewCtrl) return;

	m_pTreeViewCtrl->SetImageSectionSelIconSelHot(GetImageSectionSelIconSelHot());

	return;
}

void CDUITreeNodeCtrl::SetImageSectionSelIconSelPushed(const tagDuiImageSection &ImageSection)
{
	__super::SetImageSectionSelIconSelPushed(ImageSection);

	if (NULL == m_pTreeViewCtrl) return;

	m_pTreeViewCtrl->SetImageSectionSelIconSelPushed(GetImageSectionSelIconSelPushed());

	return;
}

bool CDUITreeNodeCtrl::IsExpandEnable()
{
	return m_pCheckExpandCtrl && m_pCheckExpandCtrl->IsEnabled();
}

void CDUITreeNodeCtrl::SetExpandEnable(bool bEnable)
{
	if (NULL == m_pCheckExpandCtrl || bEnable == IsExpandEnable()) return;

	m_pCheckExpandCtrl->SetEnabled(bEnable);

	if (NULL == m_pTreeViewCtrl) return;

	m_pTreeViewCtrl->SetExpandEnable(IsExpandEnable());

	return;
}

bool CDUITreeNodeCtrl::IsExpandDbClick()
{
	return GetRootView() && GetRootView()->IsExpandDbClick();
}

void CDUITreeNodeCtrl::SetExpandDbClick(bool bDbClick)
{
	if (NULL == GetRootView() || bDbClick == IsExpandDbClick()) return;

	GetRootView()->SetExpandDbClick(bDbClick);

	return;
}

bool CDUITreeNodeCtrl::IsExpandIconVisible()
{
	CDUITreeViewCtrl *pRootView = GetRootView();

	return pRootView ? pRootView->IsExpandIconVisible() : false;
}

void CDUITreeNodeCtrl::SetExpandIconVisible(bool bVisible)
{
	if (NULL == m_pCheckExpandCtrl) return;

	m_pCheckExpandCtrl->SetVisible(bVisible && GetChildNodeCount() > 0);

	if (m_pTreeViewCtrl)
	{
		m_pTreeViewCtrl->SetExpandIconVisible(bVisible);
	}

	return;
}

bool CDUITreeNodeCtrl::IsSelectSingle()
{
	return GetRootView() && GetRootView()->IsSelectSingle();
}

void CDUITreeNodeCtrl::SetSelectSingle(bool bSingle)
{
	if (NULL == GetRootView() || bSingle == IsSelectSingle()) return;

	GetRootView()->SetSelectSingle(bSingle);

	return;
}

int CDUITreeNodeCtrl::GetExpandIconLeftPadding()
{
	return m_pCheckExpandCtrl ? m_pCheckExpandCtrl->GetPadding().left : 0;
}

void CDUITreeNodeCtrl::SetExpandIconLeftPadding(int nLeft)
{
	if (NULL == m_pCheckExpandCtrl || nLeft == GetExpandIconLeftPadding()) return;

	m_pCheckExpandCtrl->SetPaddingL(nLeft);

	if (NULL == m_pTreeViewCtrl) return;

	m_pTreeViewCtrl->SetExpandIconLeftPadding(GetExpandIconLeftPadding());

	return;
}

int CDUITreeNodeCtrl::GetExpandIconFixedWidth()
{
	return m_pCheckExpandCtrl ? m_pCheckExpandCtrl->GetFixedWidth() : 0;
}

void CDUITreeNodeCtrl::SetExpandIconFixedWidth(int nWidth)
{
	if (NULL == m_pCheckExpandCtrl || nWidth == GetExpandIconFixedWidth()) return;

	m_pCheckExpandCtrl->SetFixedWidth(nWidth);

	if (NULL == m_pTreeViewCtrl) return;

	m_pTreeViewCtrl->SetExpandIconFixedWidth(GetExpandIconFixedWidth());

	return;
}

int CDUITreeNodeCtrl::GetExpandIconFixedHeight()
{
	return m_pCheckExpandCtrl ? m_pCheckExpandCtrl->GetFixedHeight() : 0;
}

void CDUITreeNodeCtrl::SetExpandIconFixedHeight(int nHeight)
{
	if (NULL == m_pCheckExpandCtrl || nHeight == GetExpandIconFixedHeight()) return;

	m_pCheckExpandCtrl->SetFixedHeight(nHeight);

	if (NULL == m_pTreeViewCtrl) return;

	m_pTreeViewCtrl->SetExpandIconFixedHeight(GetExpandIconFixedHeight());

	return;
}

int CDUITreeNodeCtrl::GetChildIndent()
{
	CDUITreeViewCtrl *pRootView = GetRootView();

	return pRootView ? pRootView->GetChildIndent() : 0;
}

void CDUITreeNodeCtrl::SetChildIndent(int nIndent)
{
	if (GetParentNode())
	{
		SetPaddingL(nIndent);
	}
	if (m_pTreeViewCtrl)
	{
		m_pTreeViewCtrl->SetChildIndent(nIndent);
	}

	return;
}

tagDuiImageSection CDUITreeNodeCtrl::GetImageSectionExpandIconNormal()
{
	return m_pCheckExpandCtrl ? m_pCheckExpandCtrl->GetImageSectionNormal() : tagDuiImageSection();
}

tagDuiImageSection CDUITreeNodeCtrl::GetImageSectionExpandIconHot()
{
	return m_pCheckExpandCtrl ? m_pCheckExpandCtrl->GetImageSectionHot() : tagDuiImageSection();
}

tagDuiImageSection CDUITreeNodeCtrl::GetImageSectionExpandIconPushed()
{
	return m_pCheckExpandCtrl ? m_pCheckExpandCtrl->GetImageSectionPushed() : tagDuiImageSection();
}

tagDuiImageSection CDUITreeNodeCtrl::GetImageSectionExpandIconSelNormal()
{
	return m_pCheckExpandCtrl ? m_pCheckExpandCtrl->GetImageSectionSelNormal() : tagDuiImageSection();
}

tagDuiImageSection CDUITreeNodeCtrl::GetImageSectionExpandIconSelHot()
{
	return m_pCheckExpandCtrl ? m_pCheckExpandCtrl->GetImageSectionSelHot() : tagDuiImageSection();
}

tagDuiImageSection CDUITreeNodeCtrl::GetImageSectionExpandIconSelPushed()
{
	return m_pCheckExpandCtrl ? m_pCheckExpandCtrl->GetImageSectionSelPushed() : tagDuiImageSection();
}

void CDUITreeNodeCtrl::SetImageSectionExpandIconNormal(const tagDuiImageSection &ImageSection)
{
	if (NULL == m_pCheckExpandCtrl) return;

	m_pCheckExpandCtrl->SetImageSectionNormal(ImageSection);

	if (NULL == m_pTreeViewCtrl) return;

	m_pTreeViewCtrl->SetImageSectionExpandIconNormal(GetImageSectionExpandIconNormal());

	return;
}

void CDUITreeNodeCtrl::SetImageSectionExpandIconHot(const tagDuiImageSection &ImageSection)
{
	if (NULL == m_pCheckExpandCtrl) return;

	m_pCheckExpandCtrl->SetImageSectionHot(ImageSection);

	if (NULL == m_pTreeViewCtrl) return;

	m_pTreeViewCtrl->SetImageSectionExpandIconHot(GetImageSectionExpandIconHot());

	return;
}

void CDUITreeNodeCtrl::SetImageSectionExpandIconPushed(const tagDuiImageSection &ImageSection)
{
	if (NULL == m_pCheckExpandCtrl) return;

	m_pCheckExpandCtrl->SetImageSectionPushed(ImageSection);

	if (NULL == m_pTreeViewCtrl) return;

	m_pTreeViewCtrl->SetImageSectionExpandIconPushed(GetImageSectionExpandIconPushed());

	return;
}

void CDUITreeNodeCtrl::SetImageSectionExpandIconSelNormal(const tagDuiImageSection &ImageSection)
{
	if (NULL == m_pCheckExpandCtrl) return;

	m_pCheckExpandCtrl->SetImageSectionSelNormal(ImageSection);

	if (NULL == m_pTreeViewCtrl) return;

	m_pTreeViewCtrl->SetImageSectionExpandIconSelNormal(GetImageSectionExpandIconSelNormal());

	return;
}

void CDUITreeNodeCtrl::SetImageSectionExpandIconSelHot(const tagDuiImageSection &ImageSection)
{
	if (NULL == m_pCheckExpandCtrl) return;

	m_pCheckExpandCtrl->SetImageSectionSelHot(ImageSection);

	if (NULL == m_pTreeViewCtrl) return;

	m_pTreeViewCtrl->SetImageSectionExpandIconSelHot(GetImageSectionExpandIconSelHot());

	return;
}

void CDUITreeNodeCtrl::SetImageSectionExpandIconSelPushed(const tagDuiImageSection &ImageSection)
{
	if (NULL == m_pCheckExpandCtrl) return;

	m_pCheckExpandCtrl->SetImageSectionSelPushed(ImageSection);

	if (NULL == m_pTreeViewCtrl) return;

	m_pTreeViewCtrl->SetImageSectionExpandIconSelPushed(GetImageSectionExpandIconSelPushed());

	return;
}

int CDUITreeNodeCtrl::GetLevel()
{
	int nLevel = 0;

	CDUITreeNodeCtrl *pParentNode = GetParentNode();
	while (pParentNode)
	{
		nLevel++;

		pParentNode = pParentNode->GetParentNode();
	}

	return nLevel;
}

bool CDUITreeNodeCtrl::IsExpanded() const
{
	return m_pCheckExpandCtrl && m_pCheckExpandCtrl->IsSelected();
}

bool CDUITreeNodeCtrl::Expand(bool bExpand /*= true*/)
{
	if (NULL == m_pCheckExpandCtrl) return false;

	m_pCheckExpandCtrl->Select(bExpand);

	return true;
}

CDUICheckBoxCtrl * CDUITreeNodeCtrl::GetExpandIconCtrl()
{
	return m_pCheckExpandCtrl;
}

int CDUITreeNodeCtrl::GetChildNodeIndex(CDUITreeNodeCtrl *pTreeNode)
{
	return m_pTreeViewCtrl ? m_pTreeViewCtrl->GetChildIndex(pTreeNode) : -1;
}

int CDUITreeNodeCtrl::GetChildNodeCount()
{
	return m_pTreeViewCtrl ? m_pTreeViewCtrl->GetChildCount() : 0;
}

int CDUITreeNodeCtrl::GetSelectNodeCount()
{
	return m_pTreeViewCtrl ? m_pTreeViewCtrl->GetSelectNodeCount() : 0;
}

bool CDUITreeNodeCtrl::InsertChildNode(int nItemModelCount, int nPos)
{
	if (NULL == m_pVertContainerCtrl) return false;

	if (NULL == m_pTreeViewCtrl)
	{
		ConstructTreeView();
	}

	return m_pTreeViewCtrl ? m_pTreeViewCtrl->InsertChild(nItemModelCount, nPos) : false;
}

bool CDUITreeNodeCtrl::InsertChildNode(CDUIControlBase *pTreeNode, int nPos)
{
	if (NULL == m_pVertContainerCtrl) return false;

	if (NULL == m_pTreeViewCtrl)
	{
		ConstructTreeView();
	}

	return m_pTreeViewCtrl ? m_pTreeViewCtrl->InsertChild(pTreeNode, nPos) : false;
}

bool CDUITreeNodeCtrl::RemoveChildNode(CDUIControlBase *pTreeNode)
{
	return m_pTreeViewCtrl ? m_pTreeViewCtrl->Remove(pTreeNode) : false;
}

bool CDUITreeNodeCtrl::RemoveChildNodeAt(int nIndex)
{
	return m_pTreeViewCtrl ? m_pTreeViewCtrl->RemoveAt(nIndex) : false;
}

void CDUITreeNodeCtrl::RemoveChildNodeAll()
{
	if (m_pTreeViewCtrl)
	{
		m_pTreeViewCtrl->RemoveAll();
	}

	return;
}

CDUITreeViewCtrl * CDUITreeNodeCtrl::GetOwnerView()
{
	return MMStaticPtr(CDUITreeViewCtrl, m_pOwner);
}

CDUITreeViewCtrl * CDUITreeNodeCtrl::GetTreeView()
{
	return m_pTreeViewCtrl;
}

CDUITreeViewCtrl * CDUITreeNodeCtrl::GetRootView()
{
	CDUITreeNodeCtrl *pRootNode = GetRootNode();

	return pRootNode ? pRootNode->GetOwnerView() : NULL;
}

CDUITreeNodeCtrl * CDUITreeNodeCtrl::GetRootNode()
{
	CDUITreeNodeCtrl *pParentNode = GetParentNode();

	return pParentNode ? pParentNode->GetRootNode() : this;
}

CDUITreeNodeCtrl * CDUITreeNodeCtrl::GetParentNode()
{
	CDUITreeViewCtrl *pOwnerView = GetOwnerView();

	return pOwnerView ? pOwnerView->GetOwnerNode() : NULL;
}

CDUITreeNodeCtrl * CDUITreeNodeCtrl::GetOwnerNode(int nLevel)
{
	CDUITreeNodeCtrl *pParentNode = GetParentNode();
	if (NULL == pParentNode) return NULL;

	int nLevelParent = pParentNode->GetLevel();
	if (nLevelParent > nLevel) return pParentNode->GetOwnerNode(nLevel);

	if (nLevel == nLevelParent) return pParentNode;

	return NULL;
}

CDUITreeNodeCtrl * CDUITreeNodeCtrl::GetChildNode(int nIndex)
{
	return m_pTreeViewCtrl ? m_pTreeViewCtrl->GetChildAt(nIndex) : NULL;
}

CDUITreeNodeCtrl * CDUITreeNodeCtrl::GetLastNode()
{
	return GetChildNode(GetChildNodeCount() - 1);
}

CDUITreeNodeCtrl * CDUITreeNodeCtrl::GetCurSelNode()
{
	CDUITreeViewCtrl *pRootView = GetRootView();

	return pRootView ? pRootView->GetCurSelNode() : NULL;
}

void CDUITreeNodeCtrl::SelectChildNode(bool bSelect)
{
	if (NULL == m_pTreeViewCtrl) return;

	bool bSelectParent = m_bSelectParent;
	m_bSelectParent = false;
	m_pTreeViewCtrl->SelectTreeNodeAll(bSelect);
	m_bSelectParent = bSelectParent;

	//this
	if (false == IsSelectSingle())
	{
		OnSelectChangedNode(bSelect);
	}

	return;
}

bool CDUITreeNodeCtrl::OnDuiLButtonDlk(const CDUIPoint &pt, const DuiMessage &Msg)
{
	CDUITreeViewCtrl *pOwnerView = GetOwnerView();
	if (NULL == pOwnerView
		|| NULL == m_pHorizContainerCtrl
		|| false == m_pHorizContainerCtrl->GetAbsoluteRect().PtInRect(pt)) return true;

	return __super::OnDuiLButtonDlk(pt, Msg);
}

void CDUITreeNodeCtrl::InitComplete()
{
	//find
	if (NULL == m_pHorizContainerCtrl && m_pVertContainerCtrl)
	{
		m_pHorizContainerCtrl = MMInterfaceHelper(CDUIHorizontalLayoutCtrl, m_pVertContainerCtrl->FindSubControlThisView(Dui_CtrlIDInner_TreeNodeHorizContainer));
	}
	if (NULL == m_pTreeViewCtrl && m_pVertContainerCtrl)
	{
		m_pTreeViewCtrl = MMInterfaceHelper(CDUITreeViewCtrl, m_pVertContainerCtrl->FindSubControlThisView(Dui_CtrlIDInner_TreeNodeView));
	}
	if (NULL == m_pCheckExpandCtrl && m_pHorizContainerCtrl)
	{
		m_pCheckExpandCtrl = MMInterfaceHelper(CDUICheckBoxCtrl, m_pHorizContainerCtrl->FindSubControlThisView(Dui_CtrlIDInner_TreeNodeExpand));
	}
	if (NULL == m_pCheckSelectCtrl && m_pHorizContainerCtrl)
	{
		m_pCheckSelectCtrl = MMInterfaceHelper(CDUICheckBoxCtrl, m_pHorizContainerCtrl->FindSubControlThisView(Dui_CtrlIDInner_ListItemSelect));
	}
	if (NULL == m_pEditTextCtrl && m_pHorizContainerCtrl)
	{
		m_pEditTextCtrl = MMInterfaceHelper(CDUIXEditCtrl, m_pHorizContainerCtrl->FindSubControlThisView(Dui_CtrlIDInner_ListItemEdit));
	}

	//create
	if (NULL == m_pVertContainerCtrl)
	{
		CDUIVerticalLayoutCtrl *pVertContainerCtrl = new CDUIVerticalLayoutCtrl;
		pVertContainerCtrl->Init();
		pVertContainerCtrl->SetCtrlID(Dui_CtrlIDInner_TreeNodeVertContainer);
		InsertChild(pVertContainerCtrl);
	}
	if (NULL == m_pHorizContainerCtrl && m_pVertContainerCtrl)
	{
		m_pHorizContainerCtrl = new CDUIHorizontalLayoutCtrl;
		m_pHorizContainerCtrl->Init();
		m_pHorizContainerCtrl->SetCtrlID(Dui_CtrlIDInner_TreeNodeHorizContainer);
		m_pVertContainerCtrl->InsertChild(m_pHorizContainerCtrl, 0);
	}
	if (NULL == m_pCheckExpandCtrl && m_pHorizContainerCtrl)
	{
		do
		{
			//same parent
			CDUITreeNodeCtrl *pParentNode = GetParentNode();
			if (pParentNode)
			{
				CDUICheckBoxCtrl *pCheckExpandCtrl = pParentNode->GetExpandIconCtrl();
				if (NULL == pCheckExpandCtrl) break;

				m_pCheckExpandCtrl = MMInterfaceHelper(CDUICheckBoxCtrl, pCheckExpandCtrl->Clone());
				if (NULL == m_pCheckExpandCtrl) break;

				m_pHorizContainerCtrl->InsertChild(m_pCheckExpandCtrl);

				break;
			}

			//new
			m_pCheckExpandCtrl = new CDUICheckBoxCtrl;
			m_pCheckExpandCtrl->Init();
			m_pCheckExpandCtrl->SetCtrlID(Dui_CtrlIDInner_TreeNodeExpand);
			m_pHorizContainerCtrl->InsertChild(m_pCheckExpandCtrl);

		} while (false);
	}
	if (NULL == m_pCheckSelectCtrl && m_pHorizContainerCtrl)
	{
		do
		{
			//same parent
			CDUITreeNodeCtrl *pParentNode = GetParentNode();
			if (pParentNode)
			{
				CDUICheckBoxCtrl *pCheckSelectCtrl = pParentNode->GetSelectIconCtrl();
				if (NULL == pCheckSelectCtrl) break;

				m_pCheckSelectCtrl = MMInterfaceHelper(CDUICheckBoxCtrl, pCheckSelectCtrl->Clone());
				if (NULL == m_pCheckSelectCtrl) break;

				m_pHorizContainerCtrl->InsertChild(m_pCheckSelectCtrl);

				break;
			}

			//new
			m_pCheckSelectCtrl = new CDUICheckBoxCtrl;
			m_pCheckSelectCtrl->Init();
			m_pCheckSelectCtrl->SetCtrlID(Dui_CtrlIDInner_ListItemSelect);
			m_pHorizContainerCtrl->InsertChild(m_pCheckSelectCtrl);

		} while (false);
	}
	if (NULL == m_pEditTextCtrl && m_pHorizContainerCtrl)
	{
		do
		{
			//same parent
			CDUITreeNodeCtrl *pParentNode = GetParentNode();
			if (pParentNode)
			{
				CDUIXEditCtrl *pEditTextCtrl = pParentNode->GetEditTextCtrl();
				if (NULL == pEditTextCtrl) break;

				m_pEditTextCtrl = MMInterfaceHelper(CDUIXEditCtrl, pEditTextCtrl->Clone());
				if (NULL == m_pEditTextCtrl) break;

				m_pHorizContainerCtrl->InsertChild(m_pEditTextCtrl);

				break;
			}

			//new
			m_pEditTextCtrl = new CDUIXEditCtrl;
			m_pEditTextCtrl->Init();
			m_pEditTextCtrl->SetCtrlID(Dui_CtrlIDInner_ListItemEdit);
			m_pEditTextCtrl->SetVisible(false);
			m_pEditTextCtrl->SetBorderLine({ 1,1,1,1 });
			m_pEditTextCtrl->SetBkColor({ Name_ColorWhite });
			m_pEditTextCtrl->SetAutoSelAll(true);
			m_pHorizContainerCtrl->InsertChild(m_pEditTextCtrl);

		} while (false);
	}

	__super::InitComplete();

	if (m_pCheckExpandCtrl)
	{
		m_pCheckExpandCtrl->SetPaddingVertCenter();
		m_pCheckExpandCtrl->SetClickTransmit(false);
		m_pCheckExpandCtrl->RegisterControlCallBack(this);
	}
	if (m_pCheckSelectCtrl)
	{
		m_pCheckSelectCtrl->SetClickTransmit(false);
		m_pCheckSelectCtrl->RegisterControlCallBack(this);
	}
	if (m_pEditTextCtrl)
	{
		m_pEditTextCtrl->RegisterControlCallBack(this);
	}
	if (m_pHorizContainerCtrl)
	{
		m_pHorizContainerCtrl->RegisterControlCallBack(this);
	}
	if (m_pTreeViewCtrl)
	{
		m_pTreeViewCtrl->SetOwnerNode(this);
	}

	return;
}

bool CDUITreeNodeCtrl::SaveAttribute(tinyxml2::XMLElement *pNode, bool bIncludeChild/* = true*/)
{
	if (m_pVertContainerCtrl
		&& m_pTreeViewCtrl
		&& m_pTreeViewCtrl->GetChildCount() <= 0)
	{
		m_pVertContainerCtrl->DetachChild(m_pTreeViewCtrl);
	}

	bool bRes = __super::SaveAttribute(pNode, bIncludeChild);

	if (m_pVertContainerCtrl
		&& m_pTreeViewCtrl
		&& m_pTreeViewCtrl->GetChildCount() <= 0
		&& false == m_pVertContainerCtrl->VerifyChild(m_pTreeViewCtrl))
	{
		m_pVertContainerCtrl->InsertChild(m_pTreeViewCtrl);
	}

	return bRes;
}

void CDUITreeNodeCtrl::PaintLine(HDC hDC)
{
	return;
}

void CDUITreeNodeCtrl::PaintLineOnListType(HDC hDC)
{
	if (NULL == m_pHorizContainerCtrl) return;

	CDUITreeViewCtrl *pRootView = GetRootView();
	if (NULL == pRootView) return;

	CDUIRect rcLayoutRootView = pRootView->GetLayoutRangeOfItem();
	tagDuiListInfo ListInfo = pRootView->GetListInfo();
	if (NULL == ListInfo.pAttributeColorLine) return;

	//row line
	if (pRootView->IsShowRowLine())
	{
		CDUIRect rcRowLine = pRootView->GetAbsoluteRect();
		CDUIRect rcNodeLine = m_pHorizContainerCtrl->GetAbsoluteRect();
		rcRowLine.top = rcNodeLine.top;
		rcRowLine.bottom = rcNodeLine.top;
		rcRowLine.Offset(0, -1);
		ListInfo.pAttributeColorLine->DrawLine(hDC, rcRowLine, 1);

		rcRowLine.top = rcNodeLine.bottom;
		rcRowLine.bottom = rcNodeLine.bottom;
		rcRowLine.Offset(0, -1);
		ListInfo.pAttributeColorLine->DrawLine(hDC, rcRowLine, 1);
	}

	//column line
	if (pRootView->IsShowColumnLine())
	{
		for (int nColumn = 0; nColumn < pRootView->GetColumnCount() - 1; nColumn++)
		{
			CDUIRect rcColumn = pRootView->GetColumnRect(nColumn);
			CDUIRect rcColumnLine = m_pHorizContainerCtrl->GetAbsoluteRect();
			rcColumnLine.left = rcColumn.right;
			rcColumnLine.right = rcColumnLine.left;
			rcColumnLine.Offset(-1, 0);
			if (rcColumnLine.bottom <= rcColumnLine.top) break;

			ListInfo.pAttributeColorLine->DrawLine(hDC, rcColumnLine, 1);
		}
	}

	//child
	CDUIRect rcTemp;
	for (int n = 0; n < GetChildNodeCount(); n++)
	{
		CDUITreeNodeCtrl *pTreeNode = GetChildNode(n);
		if (NULL == pTreeNode || false == pTreeNode->IsVisible()) continue;

		CDUIRect rcNode = pTreeNode->GetAbsoluteRect();
		if (false == IntersectRect(&rcTemp, &rcNode, &rcLayoutRootView)) continue;

		pTreeNode->PaintLineOnListType(hDC);
	}

	return;
}

void CDUITreeNodeCtrl::ReapControl()
{
	//cur sel
	CDUITreeViewCtrl *pRootView = GetRootView();
	if (pRootView && this == pRootView->m_pCurNodeCtrl)
	{
		pRootView->SetCurSelNode(NULL);
	}

	return __super::ReapControl();
}

CDUIRect CDUITreeNodeCtrl::GetTextRange()
{
	if (NULL == m_pHorizContainerCtrl) return __super::GetTextRange();

	CDUIRect rcRange = m_pHorizContainerCtrl->GetAbsoluteRect();
	if (m_pCheckExpandCtrl && m_pCheckExpandCtrl->IsVisible())
	{
		rcRange.left = m_pCheckExpandCtrl->GetAbsoluteRect().right;
	}
	if (m_pCheckSelectCtrl && m_pCheckSelectCtrl->IsVisible())
	{
		rcRange.left = m_pCheckSelectCtrl->GetAbsoluteRect().right;
	}

	CDUIRect rcTextPadding = GetTextPadding();
	rcRange.left += rcTextPadding.left;
	rcRange.right -= rcTextPadding.right;
	rcRange.top += rcTextPadding.top;
	rcRange.bottom -= rcTextPadding.bottom;
	rcRange.CheckRect();

	return rcRange;
}

CDUIRect CDUITreeNodeCtrl::GetBackRange()
{
	if (NULL == m_pHorizContainerCtrl) return __super::GetBackRange();

	return m_pHorizContainerCtrl->GetAbsoluteRect();
}

tagDuiListInfo CDUITreeNodeCtrl::GetItemStyleInfo()
{
	CDUITreeViewCtrl *pRootView = GetRootView();
	if (NULL == pRootView) return __super::GetItemStyleInfo();

	return pRootView->GetListInfo();
}

CDUIControlBase * CDUITreeNodeCtrl::FindControl(FindControlProc Proc, LPVOID pData, UINT uFlags)
{
	if (uFlags & DuiFind_ThisView)
	{
		CDUIControlBase *pControl = m_pHorizContainerCtrl ? m_pHorizContainerCtrl->FindControl(Proc, pData, uFlags) : NULL;

		NULL == pControl && m_pTreeViewCtrl ? pControl = m_pTreeViewCtrl->FindControl(Proc, pData, uFlags) : NULL;
		NULL == pControl && m_pVertContainerCtrl ? pControl = m_pVertContainerCtrl->FindControl(Proc, pData, uFlags) : NULL;

		return pControl;
	}

	return __super::FindControl(Proc, pData, uFlags);
}

void CDUITreeNodeCtrl::SendNotify(enDuiNotifyType NotifyType, WPARAM wParam, LPARAM lParam, CMMString strTextOld)
{
	if (NULL == m_pWndOwner) return;

	CDUITreeNodeCtrl *pRootNode = GetRootNode();
	CDUITreeViewCtrl *pRootView = MMStaticPtr(CDUITreeViewCtrl, pRootNode->GetOwnerView());

	DuiNotify Notify;
	Notify.NotifyType = NotifyType;
	Notify.pNotifyCtrl = pRootView;
	Notify.wParam = wParam;
	Notify.lParam = lParam;
	Notify.DuiNotifyExtend.Type = tagDuiNotify::DuiNotifyExtend_TreeView;
	Notify.DuiNotifyExtend.TreeView.pTreeNode = this;
	Notify.DuiNotifyExtend.TreeView.pRootNode = pRootNode;
	Notify.DuiNotifyExtend.TreeView.pRootView = pRootView;
	Notify.DuiNotifyExtend.TreeView.strTextOld = strTextOld;

	m_pWndOwner->SendNotify(Notify);

	return;
}

void CDUITreeNodeCtrl::PerformItemMouseDown(bool bLeft, const CDUIPoint &pt)
{
	do
	{
		CDUITreeViewCtrl *pOwnerView = GetOwnerView();
		if (NULL == pOwnerView
			|| NULL == m_pHorizContainerCtrl
			|| pOwnerView->IsSelectDbClick()
			|| false == m_pHorizContainerCtrl->GetAbsoluteRect().PtInRect(pt)) break;

		auto pRootView = GetRootView();
		if (NULL == pRootView) break;

		//shift
		if (GetKeyState(VK_SHIFT) & 0x8000)
		{
			CDUITreeNodeCtrl *pTreeNode = GetCurSelNode();
			pRootView->SelectTreeNodeAll(false);
			pRootView->SelectTreeNode(pTreeNode, this);

			break;
		}

		//ctrl or select icon visible
		if ((GetKeyState(VK_CONTROL) & 0x8000) || IsSelectIconVisible())
		{
			Select(bLeft ? !IsSelected() : true);

			if (IsSelected())
			{
				pRootView->SetCurSelNode(this);
			}

			break;
		}

		//single
		{
			if (bLeft || false == IsSelected())
			{
				pRootView->SelectTreeNodeAll(false);
			}

			Select();

			pRootView->SetCurSelNode(this);
		}

	} while (false);

	return;
}

bool CDUITreeNodeCtrl::SelectTreeNode(CDUITreeNodeCtrl *pFromNode, CDUITreeNodeCtrl *pStopNode, bool &bFindFrom)
{
	if (bFindFrom || pFromNode == this)
	{
		bFindFrom = true;

		Select(true);

		if (pStopNode == this) return false;
	}

	for (int n = 0; n < GetChildNodeCount(); n++)
	{
		CDUITreeNodeCtrl *pChildNode = GetChildNode(n);
		if (NULL == pChildNode) continue;

		if (false == pChildNode->SelectTreeNode(pFromNode, pStopNode, bFindFrom)) return false;
	}

	return true;
}

void CDUITreeNodeCtrl::SelectTreeNodeAll(bool bSelect)
{
	bool bSelectChild = m_bSelectChild;
	bool bSelectParent = m_bSelectParent;
	m_bSelectChild = false;
	m_bSelectParent = false;

	Select(bSelect);
	SelectChildNode(bSelect);

	m_bSelectChild = bSelectChild;
	m_bSelectParent = bSelectParent;

	return;
}

void CDUITreeNodeCtrl::OnSelectChangedNode(bool bSelect)
{
	if (bSelect)
	{
		Select(GetChildNodeCount() == GetSelectNodeCount());

		return;
	}

	m_bSelectChild = false;
	Select(false);
	m_bSelectChild = true;

	return;
}

void CDUITreeNodeCtrl::CalcNodeHeight()
{
	if (NULL == m_pOwner
		|| NULL == m_pHorizContainerCtrl
		|| NULL == m_pCheckExpandCtrl) return;

	int nHeight = m_pOwner->GetSwitchListItemHeight();
	nHeight = min(nHeight, GetMaxHeight());
	nHeight = max(nHeight, GetMinHeight());
	m_pHorizContainerCtrl->SetFixedHeight(DuiDpiScaleVerifyCtrl(nHeight, DuiDpiScaleBackCtrl(nHeight)));

	if (m_pTreeViewCtrl && m_pCheckExpandCtrl->IsSelected())
	{
		nHeight += m_pTreeViewCtrl->GetTotalRange().cy;
		SetFixedHeight(DuiDpiScaleVerifyCtrl(nHeight, DuiDpiScaleBackCtrl(nHeight)));

		return;
	}

	SetFixedHeight(DuiDpiScaleVerifyCtrl(nHeight, DuiDpiScaleBackCtrl(nHeight)));

	return;
}

void CDUITreeNodeCtrl::ConstructTreeView()
{
	if (NULL == m_pVertContainerCtrl) return;

	CDUITreeViewCtrl *pOwnerView = GetOwnerView();
	m_pTreeViewCtrl = pOwnerView ? (CDUITreeViewCtrl*)MMInterfaceHelper(CDUITreeViewCtrl, pOwnerView->Clone(false, false)) : NULL;

	if (NULL == m_pTreeViewCtrl) return;

	m_pTreeViewCtrl->SetFloat(false);
	m_pTreeViewCtrl->SetPadding(0, 0, 0, 0);
	m_pTreeViewCtrl->SetBorderLine({ 0,0,0,0 });
	m_pTreeViewCtrl->SetUseListHeader(false);
	m_pTreeViewCtrl->SetCtrlID(Dui_CtrlIDInner_TreeNodeView);
	m_pTreeViewCtrl->SetOwnerNode(this);
	m_pTreeViewCtrl->EnableScrollBar(false, false);
	m_pVertContainerCtrl->InsertChild(m_pTreeViewCtrl);

	return;
}

void CDUITreeNodeCtrl::OnChildNodeChanged(bool bAdd)
{
	CDUITreeViewCtrl *pRootView = GetRootView();
	if (NULL == pRootView || false == pRootView->IsExpandIconVisible()) return;

	SetExpandIconVisible(GetChildCount() > 0);

	return;
}