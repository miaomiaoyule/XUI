#include "StdAfx.h"
#include "DUIListItemCtrl.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateControl(CDUIListItemCtrl)
MMImplement_ClassName(CDUIListItemCtrl)

CDUIListItemCtrl::CDUIListItemCtrl(void)
{
}

CDUIListItemCtrl::~CDUIListItemCtrl(void)
{
	if (m_pCheckSelectCtrl)
	{
		m_pCheckSelectCtrl->UnRegisterControlCallBack(this);
	}
	if (m_pEditTextCtrl)
	{
		m_pEditTextCtrl->UnRegisterControlCallBack(this);
	}

	m_pCheckSelectCtrl = NULL;
	m_pEditTextCtrl = NULL;
	m_pOwner = NULL;

	return;
}

void CDUIListItemCtrl::OnNotify(CDUIControlBase *pControl, const DuiNotify &Notify)
{
	if (pControl == m_pEditTextCtrl)
	{
		switch (Notify.NotifyType)
		{
			case DuiNotify_KillFocus:
			{
				m_pEditTextCtrl->SetVisible(false);

				break;
			}
			case DuiNotify_Edited:
			{
				CMMString strTextOld = GetText();

				SetText(m_pEditTextCtrl->GetText());
				m_pEditTextCtrl->SetVisible(false);

				SendNotify(DuiNotify_ItemEditFinish, 0, 0, strTextOld);

				break;
			}
		}

		return;
	}

	return;
}

void CDUIListItemCtrl::OnRelease(CDUIControlBase *pControl)
{
	if (pControl == m_pCheckSelectCtrl)
	{
		m_pCheckSelectCtrl->UnRegisterControlCallBack(this);
		m_pCheckSelectCtrl = NULL;

		return;
	}
	if (pControl == m_pEditTextCtrl)
	{
		m_pEditTextCtrl->UnRegisterControlCallBack(this);
		m_pEditTextCtrl = NULL;

		return;
	}

	return;
}

void CDUIListItemCtrl::OnSize(CDUIControlBase *pControl)
{
	if (pControl == m_pCheckSelectCtrl)
	{
		m_pCheckSelectCtrl->SetFloat(false);
	}

	return;
}

int CDUIListItemCtrl::GetChildCount() const
{
	return __super::GetChildCount();
}

bool CDUIListItemCtrl::InsertChild(CDUIControlBase *pChild, int nPos)
{
	if (false == __super::InsertChild(pChild, nPos)) return false;

	if (pChild)
	{
		pChild->SetOwnerModelCtrl(this);
	}

	return true;
}

CDUIControlBase * CDUIListItemCtrl::GetChildAt(int nIndex) const
{
	return __super::GetChildAt(nIndex);
}

bool CDUIListItemCtrl::RemoveAt(int nIndex)
{
	if (nIndex < 2) return false;

	return __super::RemoveAt(nIndex);
}

LPVOID CDUIListItemCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIListItemCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIListItemCtrl::GetDescribe() const
{
	return Dui_Ctrl_ListItem;
}

UINT CDUIListItemCtrl::GetControlFlags()
{
	return __super::GetControlFlags();
}

bool CDUIListItemCtrl::Active()
{
	if (false == __super::Active()) return false;

	Select();

	SendNotify(DuiNotify_ItemActive);

	return true;
}

void CDUIListItemCtrl::EnsureVisible(bool bCenter)
{
	if (NULL == m_pOwner) return;

	m_pOwner->EnsureVisible(GetIndex(), bCenter);

	return;
}

int CDUIListItemCtrl::GetIndex() const
{
	return m_nIndex;
}

void CDUIListItemCtrl::SetIndex(int nIndex)
{
	m_nIndex = nIndex;
}

CDUIListViewCtrl * CDUIListItemCtrl::GetOwner()
{
	return m_pOwner;
}

void CDUIListItemCtrl::SetOwner(CDUIListViewCtrl *pOwner)
{
	m_pOwner = pOwner;

	return;
}

bool CDUIListItemCtrl::IsSelected() const
{
	return m_pCheckSelectCtrl && m_pCheckSelectCtrl->IsSelected();
}

bool CDUIListItemCtrl::Select(bool bSelect)
{
	if (false == IsEnabled()) return false;
	if (bSelect == IsSelected()) return true;

	m_pCheckSelectCtrl->Select(bSelect);

	if (m_pOwner)
	{
		if (IsSelected())
		{
			m_pOwner->SelectItem(m_nIndex);
		}
		else
		{
			m_pOwner->UnSelectItem(m_nIndex);
		}
	}

	SendNotify(IsSelected() ? DuiNotify_ItemSelected : DuiNotify_ItemUnSelected);

	Invalidate();

	return true;
}

CDUICheckBoxCtrl * CDUIListItemCtrl::GetSelectIconCtrl()
{
	return m_pCheckSelectCtrl;
}

bool CDUIListItemCtrl::IsSelectIconVisible()
{
	return m_pCheckSelectCtrl && m_pCheckSelectCtrl->IsVisible();
}

void CDUIListItemCtrl::SetSelectIconVisible(bool bVisible)
{
	if (NULL == m_pCheckSelectCtrl) return;

	m_pCheckSelectCtrl->SetVisible(bVisible);

	return;
}

int CDUIListItemCtrl::GetSelectIconLeftPadding()
{
	return m_pCheckSelectCtrl ? m_pCheckSelectCtrl->GetPadding().left : 0;
}

void CDUIListItemCtrl::SetSelectIconLeftPadding(int nLeft)
{
	if (NULL == m_pCheckSelectCtrl) return;

	m_pCheckSelectCtrl->SetPaddingL(nLeft);

	return;
}

int CDUIListItemCtrl::GetSelectIconFixedWidth()
{
	return m_pCheckSelectCtrl ? m_pCheckSelectCtrl->GetFixedWidth() : 0;
}

void CDUIListItemCtrl::SetSelectIconFixedWidth(int nWidth)
{
	if (NULL == m_pCheckSelectCtrl) return;

	m_pCheckSelectCtrl->SetFixedWidth(nWidth);

	return;
}

int CDUIListItemCtrl::GetSelectIconFixedHeight()
{
	return m_pCheckSelectCtrl ? m_pCheckSelectCtrl->GetFixedHeight() : 0;
}

void CDUIListItemCtrl::SetSelectIconFixedHeight(int nHeight)
{
	if (NULL == m_pCheckSelectCtrl) return;

	m_pCheckSelectCtrl->SetFixedHeight(nHeight);

	return;
}

tagDuiImageSection CDUIListItemCtrl::GetImageSectionSelIconNormal()
{
	return m_pCheckSelectCtrl ? m_pCheckSelectCtrl->GetImageSectionNormal() : tagDuiImageSection();
}

tagDuiImageSection CDUIListItemCtrl::GetImageSectionSelIconHot()
{
	return m_pCheckSelectCtrl ? m_pCheckSelectCtrl->GetImageSectionHot() : tagDuiImageSection();
}

tagDuiImageSection CDUIListItemCtrl::GetImageSectionSelIconPushed()
{
	return m_pCheckSelectCtrl ? m_pCheckSelectCtrl->GetImageSectionPushed() : tagDuiImageSection();
}

tagDuiImageSection CDUIListItemCtrl::GetImageSectionSelIconSelNormal()
{
	return m_pCheckSelectCtrl ? m_pCheckSelectCtrl->GetImageSectionSelNormal() : tagDuiImageSection();
}

tagDuiImageSection CDUIListItemCtrl::GetImageSectionSelIconSelHot()
{
	return m_pCheckSelectCtrl ? m_pCheckSelectCtrl->GetImageSectionSelHot() : tagDuiImageSection();
}

tagDuiImageSection CDUIListItemCtrl::GetImageSectionSelIconSelPushed()
{
	return m_pCheckSelectCtrl ? m_pCheckSelectCtrl->GetImageSectionSelPushed() : tagDuiImageSection();
}

void CDUIListItemCtrl::SetImageSectionSelIconNormal(const tagDuiImageSection &ImageSection)
{
	if (NULL == m_pCheckSelectCtrl) return;

	m_pCheckSelectCtrl->SetImageSectionNormal(ImageSection);

	return;
}

void CDUIListItemCtrl::SetImageSectionSelIconHot(const tagDuiImageSection &ImageSection)
{
	if (NULL == m_pCheckSelectCtrl) return;

	m_pCheckSelectCtrl->SetImageSectionHot(ImageSection);

	return;
}

void CDUIListItemCtrl::SetImageSectionSelIconPushed(const tagDuiImageSection &ImageSection)
{
	if (NULL == m_pCheckSelectCtrl) return;

	m_pCheckSelectCtrl->SetImageSectionPushed(ImageSection);

	return;
}

void CDUIListItemCtrl::SetImageSectionSelIconSelNormal(const tagDuiImageSection &ImageSection)
{
	if (NULL == m_pCheckSelectCtrl) return;

	m_pCheckSelectCtrl->SetImageSectionSelNormal(ImageSection);

	return;
}

void CDUIListItemCtrl::SetImageSectionSelIconSelHot(const tagDuiImageSection &ImageSection)
{
	if (NULL == m_pCheckSelectCtrl) return;

	m_pCheckSelectCtrl->SetImageSectionSelHot(ImageSection);

	return;
}

void CDUIListItemCtrl::SetImageSectionSelIconSelPushed(const tagDuiImageSection &ImageSection)
{
	if (NULL == m_pCheckSelectCtrl) return;

	m_pCheckSelectCtrl->SetImageSectionSelPushed(ImageSection);

	return;
}

void CDUIListItemCtrl::SetFixedWidth(int nWidth)
{
	int nWidthOld = GetFixedWidth();

	__super::SetFixedWidth(nWidth);

	if (m_pOwner)
	{
		if (IsVisible())
		{
			m_pOwner->OnItemFixedWidth(nWidthOld, GetFixedWidth());
		}
	}

	return;
}

void CDUIListItemCtrl::SetFixedHeight(int nHeight)
{
	int nHeightOld = GetFixedHeight();

	__super::SetFixedHeight(nHeight);

	if (m_pOwner)
	{
		if (IsVisible())
		{
			m_pOwner->OnItemFixedHeight(nHeightOld, GetFixedHeight());
		}
	}

	return;
}

void CDUIListItemCtrl::SetVisible(bool bVisible)
{
	bool bVisibleOld = IsVisible();

	__super::SetVisible(bVisible);

	if (bVisibleOld != IsVisible())
	{
		if (m_pOwner)
		{
			m_pOwner->OnItemVisible(this);
		}
	}

	return;
}

void CDUIListItemCtrl::SetEnabled(bool bEnabled)
{
	return __super::SetEnabled(bEnabled);
}

void CDUIListItemCtrl::SetOwnerModelCtrl(CDUIControlBase *pOwnerModelCtrl)
{
	return CDUIControlBase::SetOwnerModelCtrl(pOwnerModelCtrl);
}

bool CDUIListItemCtrl::Remove(CDUIControlBase *pControl)
{
	if (pControl == m_pCheckSelectCtrl || pControl == m_pEditTextCtrl) return false;

	return __super::Remove(pControl);
}

void CDUIListItemCtrl::RemoveAll()
{
	for (int n = GetChildCount() - 1; n >= 0; n--)
	{
		CDUIControlBase *pControl = GetChildAt(n);
		if (pControl == m_pCheckSelectCtrl || pControl == m_pEditTextCtrl) continue;

		RemoveAt(n);
	}

	return;
}

bool CDUIListItemCtrl::InsertChildAtUser(CDUIControlBase *pChild, int nPos)
{
	return InsertChild(pChild, nPos + 2);
}

bool CDUIListItemCtrl::RemoveAtUser(int nIndex)
{
	return RemoveAt(nIndex + 2);
}

int CDUIListItemCtrl::GetChildCountUser() const
{
	return max(GetChildCount() - 2, 0);
}

CDUIControlBase * CDUIListItemCtrl::GetChildAtUser(int nIndex) const
{
	return GetChildAt(nIndex + 2);
}

void CDUIListItemCtrl::RefreshView()
{
	RefreshSubItem();

	return __super::RefreshView();
}

void CDUIListItemCtrl::RefreshSubItem()
{
	if (NULL == m_pOwner
		|| NULL == m_pCheckSelectCtrl
		|| false == m_pOwner->IsUseListHeader()) return;

	//header
	CDUIListHeaderCtrl *pListHeader = m_pOwner->GetListHeader();
	if (NULL == pListHeader) return;

	//headeritem sel
	CDUIListHeaderItemCtrl *pHeaderItemSelectCtrl = pListHeader->GetHeaderItemSelect();
	if (NULL == pHeaderItemSelectCtrl) return;

	//column width
	m_pCheckSelectCtrl->SetPaddingL(m_pOwner->GetSelectIconLeftPadding());
	m_pCheckSelectCtrl->SetFixedWidth(m_pOwner->GetSelectIconFixedWidth());
	for (int nColumn = pHeaderItemSelectCtrl->IsVisible() ? 1 : 0, j = 0; nColumn < m_pOwner->GetColumnCount(); nColumn++)
	{
		CDUIControlBase *pSubItemCtrl = NULL;
		for (; j < GetChildCount(); )
		{
			CDUIControlBase *pControl = GetChildAt(j++);
			if (NULL == pControl 
				|| false == pControl->IsVisible() 
				|| pControl == m_pCheckSelectCtrl) continue;

			pSubItemCtrl = pControl;

			break;
		}

		if (NULL == pSubItemCtrl) return;

		CDUIRect rcColumn = m_pOwner->GetColumnRect(nColumn);
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

bool CDUIListItemCtrl::DoPaint(HDC hDC, bool bGenerateBmp)
{
	PaintItemColor(hDC);
	PaintItemImage(hDC);

	if (false == __super::DoPaint(hDC, bGenerateBmp)) return false;

	PaintLine(hDC);

	return true;
}

CMMString CDUIListItemCtrl::GetText()
{
	return m_AttributeText.GetValue();
}

bool CDUIListItemCtrl::SetText(LPCTSTR lpszText)
{
	if (lpszText == GetText()) return false;

	m_AttributeText.SetValue(lpszText);

	SendNotify(DuiNotify_ItemTextChange);

	Invalidate();

	return true;
}

HFONT CDUIListItemCtrl::GetFont()
{
	return m_AttributeTextStyle.GetFont();
}

Gdiplus::ARGB CDUIListItemCtrl::GetTextColor()
{
	return m_AttributeTextStyle.GetTextColor();
}

tagDuiTextStyle CDUIListItemCtrl::GetTextStyle()
{
	return m_AttributeTextStyle.GetTextStyle();
}

tagDuiTextStyle CDUIListItemCtrl::GetTextStyleActive()
{
	return GetTextStyle();
}

void CDUIListItemCtrl::SetTextStyle(const tagDuiTextStyle &TextStyle)
{
	m_AttributeTextStyle.SetTextStyle(TextStyle);

	Invalidate();

	return;
}

RECT CDUIListItemCtrl::GetTextPadding()
{
	return DuiDpiScaleCtrl(m_AttributeTextPadding.GetValue());
}

void CDUIListItemCtrl::SetTextPadding(RECT rcPadding)
{
	if (DuiDpiScaleCtrl(rcPadding) == GetTextPadding()) return;

	m_AttributeTextPadding.SetValue(rcPadding);

	Invalidate();

	return;
}

void CDUIListItemCtrl::PerformEditText()
{
	if (NULL == m_pEditTextCtrl) return;

	//edit
	CDUIAttributeTextStyle *pAttribute = GetAttributeTextStyleCur();
	if (NULL == pAttribute) return;

	tagDuiTextStyle TextStyle = pAttribute->GetTextStyle();
	TextStyle.vecColorResSwitch = { Name_ColorBlack };
	m_pEditTextCtrl->SetText(GetText());
	m_pEditTextCtrl->SetTextStyle(TextStyle);
	m_pEditTextCtrl->SetFloat(true);
	m_pEditTextCtrl->SetPadding(GetTextPadding());
	m_pEditTextCtrl->SetVisible(true);

	RefreshView();

	m_pEditTextCtrl->SetFocus();

	return;
}

CDUIXEditCtrl * CDUIListItemCtrl::GetEditTextCtrl()
{
	return m_pEditTextCtrl;
}

bool CDUIListItemCtrl::OnDuiLButtonDown(const CDUIPoint &pt, const DuiMessage &Msg)
{
	__super::OnDuiLButtonDown(pt, Msg);

	SendNotify(DuiNotify_ItemButtonDown, Msg.wParam, Msg.lParam);

	PerformItemMouseDown(true, pt);

	return true;
}

bool CDUIListItemCtrl::OnDuiLButtonUp(const CDUIPoint &pt, const DuiMessage &Msg)
{
	bool bClick = (m_cbControlStatus & ControlStatus_Pushed);
	bool bDragMoving = m_pOwner && m_pOwner->IsAnimateDraging();

	__super::OnDuiLButtonUp(pt, Msg);

	if (m_pWndOwner)
	{
		SendNotify(DuiNotify_ItemButtonUp, Msg.wParam, Msg.lParam);

		if (bClick)
		{
			SendNotify(DuiNotify_ItemClick, Msg.wParam, Msg.lParam);
		}
	}

	return true;
}

bool CDUIListItemCtrl::OnDuiLButtonDlk(const CDUIPoint &pt, const DuiMessage &Msg)
{
	__super::OnDuiLButtonDlk(pt, Msg);

	//if((GetKeyState(VK_CONTROL) & 0x8000)) CTRL键
	Select();

	SendNotify(DuiNotify_ItemDbClick, Msg.wParam, Msg.lParam);

	Active();

	//edit
	//PerformEditText();

	return true;
}

bool CDUIListItemCtrl::OnDuiRButtonDown(const CDUIPoint &pt, const DuiMessage &Msg)
{
	__super::OnDuiRButtonDown(pt, Msg);

	SendNotify(DuiNotify_ItemRButtonDown, Msg.wParam, Msg.lParam);

	PerformItemMouseDown(false, pt);

	if (m_pOwner)
	{
		m_pOwner->OnDuiRButtonDown(pt, Msg);
	}

	return true;
}

bool CDUIListItemCtrl::OnDuiRButtonUp(const CDUIPoint &pt, const DuiMessage &Msg)
{
	bool bClick = (m_cbControlStatus & ControlStatus_Pushed);

	__super::OnDuiRButtonUp(pt, Msg);

	SendNotify(DuiNotify_ItemRButtonUp, Msg.wParam, Msg.lParam);

	if (m_pOwner)
	{
		m_pOwner->OnDuiRButtonUp(pt, Msg);
	}

	return true;
}

bool CDUIListItemCtrl::OnDuiRButtonDlk(const CDUIPoint &pt, const DuiMessage &Msg)
{
	__super::OnDuiRButtonDlk(pt, Msg);

	Select();

	SendNotify(DuiNotify_ItemRDbClick, Msg.wParam, Msg.lParam);

	if (m_pOwner)
	{
		m_pOwner->OnDuiRButtonDlk(pt, Msg);
	}

	return true;
}

bool CDUIListItemCtrl::OnDuiMouseEnter(const CDUIPoint &pt, const DuiMessage &Msg)
{
	__super::OnDuiMouseEnter(pt, Msg);

	SendNotify(DuiNotify_ItemMouseEnter, Msg.wParam, Msg.lParam);

	return true;
}

bool CDUIListItemCtrl::OnDuiMouseMove(const CDUIPoint &pt, const DuiMessage &Msg)
{
	__super::OnDuiMouseMove(pt, Msg);

	SendNotify(DuiNotify_ItemMouseMove, Msg.wParam, Msg.lParam);

	return true;
}

void CDUIListItemCtrl::OnDuiMouseLeave(const CDUIPoint &pt, const DuiMessage &Msg)
{
	__super::OnDuiMouseLeave(pt, Msg);

	SendNotify(DuiNotify_ItemMouseLeave, Msg.wParam, Msg.lParam);

	return;
}

bool CDUIListItemCtrl::OnDuiMouseWheel(const CDUIPoint &pt, const DuiMessage &Msg)
{
	bool bRes = __super::OnDuiMouseWheel(pt, Msg);

	if (m_pOwner)
	{
		return m_pOwner->OnDuiMouseWheel(pt, Msg);
	}

	return bRes;
}

bool CDUIListItemCtrl::OnDuiSetFocus()
{
	return __super::OnDuiSetFocus();
}

bool CDUIListItemCtrl::OnDuiKillFocus()
{
	return __super::OnDuiKillFocus();
}

void CDUIListItemCtrl::OnDuiTimer(UINT uTimerID, const DuiMessage &Msg)
{
	return __super::OnDuiTimer(uTimerID, Msg);
}

void CDUIListItemCtrl::OnDuiSize(CDUIRect rcParentAbs)
{
	CDUIRect rcThis = GetAbsoluteRect();

	__super::OnDuiSize(rcParentAbs);

	return;
}

LRESULT CDUIListItemCtrl::OnDuiKeyDown(const DuiMessage &Msg)
{
	__super::OnDuiKeyDown(Msg);

	SendNotify(DuiNotify_ItemKeyDown, Msg.wParam, Msg.lParam);

	if (Msg.chKey == VK_RETURN)
	{
		Active();

		return 0;
	}

	if (m_pOwner)
	{
		m_pOwner->OnDuiKeyDown(Msg);
	}

	return 0;
}

LRESULT CDUIListItemCtrl::OnDuiKeyUp(const DuiMessage &Msg)
{
	__super::OnDuiKeyUp(Msg);

	SendNotify(DuiNotify_ItemKeyUp, Msg.wParam, Msg.lParam);

	if (m_pOwner)
	{
		m_pOwner->OnDuiKeyUp(Msg);
	}

	return 0;
}

LRESULT CDUIListItemCtrl::OnDuiContextMenu(const DuiMessage &Msg)
{
	return __super::OnDuiContextMenu(Msg);
}

void CDUIListItemCtrl::InitProperty()
{
	__super::InitProperty();

	DuiCreateGroupAttribute(m_AttributeGroupText, _T("Text"));
	DuiCreateAttribute(m_AttributeText, _T("Text"), _T(""), m_AttributeGroupText);
	DuiCreateAttribute(m_AttributeTextStyle, _T("TextStyle"), _T(""), m_AttributeGroupText);
	DuiCreateAttribute(m_AttributeTextPadding, _T("TextPadding"), _T(""), m_AttributeGroupText);

	return;
}

void CDUIListItemCtrl::InitComplete()
{
	__super::InitComplete();

	if (NULL == m_pCheckSelectCtrl)
	{
		do
		{
			m_pCheckSelectCtrl = MMInterfaceHelper(CDUICheckBoxCtrl, FindSubControlThisView(Dui_CtrlIDInner_ListItemSelect));
			if (m_pCheckSelectCtrl) break;

			m_pCheckSelectCtrl = new CDUICheckBoxCtrl;
			m_pCheckSelectCtrl->Init();
			m_pCheckSelectCtrl->SetCtrlID(Dui_CtrlIDInner_ListItemSelect);
			m_pCheckSelectCtrl->SetFixedWidth(20);
			m_pCheckSelectCtrl->SetFixedHeight(20);
			m_pCheckSelectCtrl->SetPaddingVertCenter();
			m_pCheckSelectCtrl->SetVisible(GetOwner() && GetOwner()->IsSelectIconVisible());
			InsertChild(m_pCheckSelectCtrl, 0);

		} while (false);
	}
	if (NULL == m_pEditTextCtrl)
	{
		do
		{
			m_pEditTextCtrl = MMInterfaceHelper(CDUIXEditCtrl, FindSubControlThisView(Dui_CtrlIDInner_ListItemEdit));
			if (m_pEditTextCtrl) break;

			m_pEditTextCtrl = new CDUIXEditCtrl;
			m_pEditTextCtrl->Init();
			m_pEditTextCtrl->SetCtrlID(Dui_CtrlIDInner_ListItemEdit);
			m_pEditTextCtrl->SetVisible(false);
			m_pEditTextCtrl->SetBorderLine({ 1,1,1,1 });
			m_pEditTextCtrl->SetBkColor({ Name_ColorWhite });
			m_pEditTextCtrl->SetAutoSelAll(true);
			InsertChild(m_pEditTextCtrl, 1);

		} while (false);
	}
	if (m_pCheckSelectCtrl)
	{
		m_pCheckSelectCtrl->SetPaddingVertCenter();
		m_pCheckSelectCtrl->SetMouseThrough(true);
		m_pCheckSelectCtrl->RegisterControlCallBack(this);
	}
	if (m_pEditTextCtrl)
	{
		m_pEditTextCtrl->RegisterControlCallBack(this);
	}

	return;
}

void CDUIListItemCtrl::PaintText(HDC hDC)
{
	if (NULL == m_pWndOwner) return;

	//text
	CMMString strText = GetText();
	if (strText.empty()) return;

	//textstyle
	CDUIAttributeTextStyle *pAttribute = GetAttributeTextStyleCur();
	if (NULL == pAttribute) return;

	CDUIRect rcRange = GetTextRange();

	pAttribute->Draw(hDC, rcRange, strText, m_pWndOwner->IsGdiplusRenderText(), m_pWndOwner->GetGdiplusRenderTextType(), false);

	return;
}

void CDUIListItemCtrl::PaintItemColor(HDC hDC)
{
	auto ListInfo = GetItemStyleInfo();
	CDUIAttributeColorSwitch *pAttribute = NULL;

	if (false == IsEnabled())
	{
		pAttribute = ListInfo.pAttributeItemStatusColorDisabled;
	}
	else if (IsSelected())
	{
		if (m_cbControlStatus & ControlStatus_Hot)
		{
			pAttribute = ListInfo.pAttributeItemStatusColorSelHot;
		}
		else
		{
			pAttribute = ListInfo.pAttributeItemStatusColorSelNormal;
		}
	}
	else
	{
		if (m_cbControlStatus & ControlStatus_Hot)
		{
			pAttribute = ListInfo.pAttributeItemStatusColorHot;
		}
		else
		{
			pAttribute = ListInfo.pAttributeItemStatusColorNormal;
		}
	}
	if ((NULL == pAttribute || pAttribute->IsEmpty())
		&& true == IsSelected()
		&& ListInfo.pAttributeItemStatusColorSelNormal
		&& false == ListInfo.pAttributeItemStatusColorSelNormal->IsEmpty())
	{
		pAttribute = ListInfo.pAttributeItemStatusColorSelNormal;
	}
	if ((NULL == pAttribute || pAttribute->IsEmpty())
		&& ListInfo.pAttributeItemStatusColorNormal
		&& false == ListInfo.pAttributeItemStatusColorNormal->IsEmpty())
	{
		pAttribute = ListInfo.pAttributeItemStatusColorNormal;
	}
	if (NULL == pAttribute)
	{
		return;
	}

	CDUIRect rcBorderRound = GetRoundCorner();
	if (rcBorderRound.left > 0
		|| rcBorderRound.top > 0
		|| rcBorderRound.right > 0
		|| rcBorderRound.bottom > 0)
	{
		CDUIRect rcBorder = GetBorderLine();
		int nSize = max(rcBorder.left, rcBorder.top);
		nSize = max(nSize, rcBorder.right);
		nSize = max(nSize, rcBorder.bottom);

		pAttribute->FillRoundRect(hDC, GetBackRange(), nSize, rcBorderRound);

		return;
	}

	pAttribute->FillRect(hDC, GetBackRange());

	return;
}

void CDUIListItemCtrl::PaintItemImage(HDC hDC)
{
	auto ListInfo = GetItemStyleInfo();
	CDUIAttriImageSection *pAttribute = NULL;

	if (false == IsEnabled())
	{
		pAttribute = ListInfo.pAttributeItemStatusImageDisabled;
	}
	else if (IsSelected())
	{
		if (m_cbControlStatus & ControlStatus_Hot)
		{
			pAttribute = ListInfo.pAttributeItemStatusImageSelHot;
		}
		else
		{
			pAttribute = ListInfo.pAttributeItemStatusImageSelNormal;
		}
	}
	else
	{
		if (m_cbControlStatus & ControlStatus_Hot)
		{
			pAttribute = ListInfo.pAttributeItemStatusImageHot;
		}
		else
		{
			pAttribute = ListInfo.pAttributeItemStatusImageNormal;
		}
	}
	if ((NULL == pAttribute || pAttribute->IsEmpty())
		&& true == IsSelected()
		&& ListInfo.pAttributeItemStatusImageSelNormal
		&& false == ListInfo.pAttributeItemStatusImageSelNormal->IsEmpty())
	{
		pAttribute = ListInfo.pAttributeItemStatusImageSelNormal;
	}
	if ((NULL == pAttribute || pAttribute->IsEmpty())
		&& ListInfo.pAttributeItemStatusImageNormal
		&& false == ListInfo.pAttributeItemStatusImageNormal->IsEmpty())
	{
		pAttribute = ListInfo.pAttributeItemStatusImageNormal;
	}
	if (NULL == pAttribute) return;

	pAttribute->Draw(hDC, GetBackRange(), m_rcPaint);

	return;
}

void CDUIListItemCtrl::PaintLine(HDC hDC)
{
	if (NULL == m_pOwner) return;

	if (ListView_List == m_pOwner->GetListViewType())
	{
		return PaintLineOnListType(hDC);
	}

	return PaintLineOnTileType(hDC);
}

void CDUIListItemCtrl::PaintLineOnListType(HDC hDC)
{
	if (NULL == m_pOwner) return;

	tagDuiListInfo ListInfo = GetItemStyleInfo();
	if (NULL == ListInfo.pAttributeColorLine) return;

	//row line
	if (m_pOwner->IsShowRowLine())
	{
		CDUIRect rcRowLine = GetAbsoluteRect();
		rcRowLine.top = rcRowLine.bottom;
		rcRowLine.Offset(0, -1);
		ListInfo.pAttributeColorLine->DrawLine(hDC, rcRowLine, 1);
	}

	//column line
	if (m_pOwner->IsShowColumnLine())
	{
		for (int nColumn = 0; nColumn < m_pOwner->GetColumnCount() - 1; nColumn++)
		{
			CDUIRect rcColumn = m_pOwner->GetColumnRect(nColumn);
			CDUIRect rcColumnLine = GetAbsoluteRect();
			rcColumnLine.left = rcColumn.right;
			rcColumnLine.right = rcColumnLine.left;
			rcColumnLine.Offset(-1, 0);
			if (rcColumnLine.bottom <= rcColumnLine.top) break;

			ListInfo.pAttributeColorLine->DrawLine(hDC, rcColumnLine, 1);
		}
	}

	return;
}

void CDUIListItemCtrl::PaintLineOnTileType(HDC hDC)
{
	if (NULL == m_pOwner) return;

	tagDuiListInfo ListInfo = GetItemStyleInfo();
	if (NULL == ListInfo.pAttributeColorLine) return;
	if (false == m_pOwner->IsShowRowLine() && false == m_pOwner->IsShowColumnLine()) return;

	CDUIRect rcItem = GetAbsoluteRect();
	CDUIRect rcLine = rcItem;
	rcLine.top = rcLine.bottom;
	rcLine.Offset(0, -1);

	CDUIRect rcColumn = rcItem;
	rcColumn.left = rcColumn.right;
	rcColumn.Offset(-1, 0);

	ListInfo.pAttributeColorLine->DrawLine(hDC, rcLine, 1);
	ListInfo.pAttributeColorLine->DrawLine(hDC, rcColumn, 1);

	return;
}

void CDUIListItemCtrl::SendNotify(enDuiNotifyType NotifyType, WPARAM wParam, LPARAM lParam, CMMString strTextOld)
{
	if (NULL == m_pWndOwner) return;

	DuiNotify Notify;
	Notify.NotifyType = NotifyType;
	Notify.pNotifyCtrl = m_pOwner;
	Notify.wParam = wParam;
	Notify.lParam = lParam;
	Notify.DuiNotifyExtend.Type = tagDuiNotify::DuiNotifyExtend_ListView;
	Notify.DuiNotifyExtend.ListView.nIndexItem = GetIndex();
	Notify.DuiNotifyExtend.ListView.strTextOld = strTextOld;

	m_pWndOwner->SendNotify(Notify);

	return;
}

void CDUIListItemCtrl::PerformItemMouseDown(bool bLeft, const CDUIPoint &pt)
{
	do
	{
		if (m_pOwner && m_pOwner->IsSelectDbClick()) break;

		//ctrl
		if (GetKeyState(VK_CONTROL) & 0x8000)
		{
			Select(!IsSelected());

			break;
		}

		//shift
		if (GetKeyState(VK_SHIFT) & 0x8000)
		{
			if (m_pOwner)
			{
				int nIndexFirst = -1 == m_pOwner->GetCurSel() ? GetIndex() : m_pOwner->GetCurSel();
				int nIndexLast = GetIndex();
				m_pOwner->SelectItem(nIndexFirst, nIndexLast);
			}

			break;
		}

		//check
		if (IsSelectIconVisible())
		{
			Select(bLeft ? !IsSelected() : true);

			break;
		}

		//single
		if (m_pOwner)
		{
			if (bLeft || false == IsSelected())
			{
				m_pOwner->UnSelectItem(GetIndex(), true);
			}

			Select();
		}

		break;

	} while (false);

	return;
}

CDUIRect CDUIListItemCtrl::GetTextRange()
{
	CDUIRect rcRange = GetAbsoluteRect();
	CDUIRect rcTextPadding = GetTextPadding();

	rcRange.left += rcTextPadding.left;
	rcRange.right -= rcTextPadding.right;
	rcRange.top += rcTextPadding.top;
	rcRange.bottom -= rcTextPadding.bottom;

	//model owner is null
	if (m_pOwner)
	{
		rcTextPadding = m_pOwner->GetItemTextPadding();
		rcRange.left += rcTextPadding.left;
		rcRange.right -= rcTextPadding.right;
		rcRange.top += rcTextPadding.top;
		rcRange.bottom -= rcTextPadding.bottom;
	}

	rcRange.CheckRect();

	return rcRange;
}

CDUIRect CDUIListItemCtrl::GetBackRange()
{
	return m_rcAbsolute;
}

tagDuiListInfo CDUIListItemCtrl::GetItemStyleInfo()
{
	return m_pOwner ? m_pOwner->GetListInfo() : tagDuiListInfo();
}

CDUIAttributeTextStyle * CDUIListItemCtrl::GetAttributeTextStyleCur()
{
	CDUIAttributeTextStyle *pAttribute = NULL;
	auto ListInfo = GetItemStyleInfo();

	if (false == m_AttributeTextStyle.IsEmpty())
	{
		pAttribute = &m_AttributeTextStyle;
	}
	else if (false == IsEnabled())
	{
		pAttribute = ListInfo.pAttributeItemTextStyleDisabled;
	}
	else if (IsSelected())
	{
		if (m_cbControlStatus & ControlStatus_Hot)
		{
			pAttribute = ListInfo.pAttributeItemTextStyleSelHot;
		}
		else
		{
			pAttribute = ListInfo.pAttributeItemTextStyleSelNormal;
		}
	}
	else
	{
		if (m_cbControlStatus & ControlStatus_Hot)
		{
			pAttribute = ListInfo.pAttributeItemTextStyleHot;
		}
		else
		{
			pAttribute = ListInfo.pAttributeItemTextStyleNormal;
		}
	}
	if ((NULL == pAttribute || pAttribute->IsEmpty())
		&& true == IsSelected()
		&& ListInfo.pAttributeItemTextStyleSelNormal
		&& false == ListInfo.pAttributeItemTextStyleSelNormal->IsEmpty())
	{
		pAttribute = ListInfo.pAttributeItemTextStyleSelNormal;
	}
	if ((NULL == pAttribute || pAttribute->IsEmpty())
		&& ListInfo.pAttributeItemTextStyleNormal
		&& false == ListInfo.pAttributeItemTextStyleNormal->IsEmpty())
	{
		pAttribute = ListInfo.pAttributeItemTextStyleNormal;
	}

	return pAttribute;
}