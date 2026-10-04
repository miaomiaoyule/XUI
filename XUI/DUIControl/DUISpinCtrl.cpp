#include "stdafx.h"
#include "DUISpinCtrl.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateControl(CDUISpinCtrl)
MMImplement_ClassName(CDUISpinCtrl)

CDUISpinCtrl::CDUISpinCtrl(void)
{
	static tagDuiCombox AttriCombox;
	if (AttriCombox.vecItem.empty())
	{
		AttriCombox.vecItem.push_back({ EditText_None, _T("None") });
		AttriCombox.vecItem.push_back({ EditText_Number, _T("Number") });
		AttriCombox.vecItem.push_back({ EditText_NumberInt, _T("NumberInt") });
		AttriCombox.vecItem.push_back({ EditText_NumberDouble, _T("NumberDouble") });
	}

	m_AttributeEditTextType.SetCombox(AttriCombox);
	m_AttributeEditTextType.SelectItem(EditText_None);

	return;
}

CDUISpinCtrl::~CDUISpinCtrl(void)
{
	if (m_pBtnUpCtrl)
	{
		m_pBtnUpCtrl->UnRegisterControlCallBack(this);
	}
	if (m_pBtnDownCtrl)
	{
		m_pBtnDownCtrl->UnRegisterControlCallBack(this);
	}
	if (m_pEditValueCtrl)
	{
		m_pEditValueCtrl->UnRegisterControlCallBack(this);
	}

	return;
}

bool CDUISpinCtrl::OnAttributeChange(CDUIAttributeObject *pAttributeObj)
{
	if (__super::OnAttributeChange(pAttributeObj)) return true;

	if (pAttributeObj == &m_AttributeEditTextType)
	{
		SetEditTextType(GetEditTextType());

		return true;
	}
	if (pAttributeObj == &m_AttributeEditTextPadding)
	{
		SetEditTextPadding(GetEditTextPadding());

		return true;
	}
	if (pAttributeObj == &m_AttributeTextStyleNormal)
	{
		SetTextStyleNormal(GetTextStyleNormal());

		return true;
	}
	if (pAttributeObj == &m_AttributeTextStyleHot)
	{
		SetTextStyleHot(GetTextStyleHot());

		return true;
	}
	if (pAttributeObj == &m_AttributeTextStylePushed)
	{
		SetTextStylePushed(GetTextStylePushed());

		return true;
	}
	if (pAttributeObj == &m_AttributeTextStyleFocus)
	{
		SetTextStyleFocus(GetTextStyleFocus());

		return true;
	}
	if (pAttributeObj == &m_AttributeUpDownBtnRightPadding)
	{
		SetUpDownBtnRightPadding(GetUpDownBtnRightPadding());

		return true;
	}
	if (pAttributeObj == &m_AttributeUpDownBtnFixedWidth)
	{
		SetUpDownBtnFixedWidth(GetUpDownBtnFixedWidth());

		return true;
	}
	if (pAttributeObj == &m_AttributeImageUpBtnNormal)
	{
		SetImageSectionUpBtnNormal(GetImageSectionUpBtnNormal());

		return true;
	}
	if (pAttributeObj == &m_AttributeImageUpBtnHot)
	{
		SetImageSectionUpBtnHot(GetImageSectionUpBtnHot());

		return true;
	}
	if (pAttributeObj == &m_AttributeImageUpBtnPushed)
	{
		SetImageSectionUpBtnPushed(GetImageSectionUpBtnPushed());

		return true;
	}
	if (pAttributeObj == &m_AttributeImageDownBtnNormal)
	{
		SetImageSectionDownBtnNormal(GetImageSectionDownBtnNormal());

		return true;
	}
	if (pAttributeObj == &m_AttributeImageDownBtnHot)
	{
		SetImageSectionDownBtnHot(GetImageSectionDownBtnHot());

		return true;
	}
	if (pAttributeObj == &m_AttributeImageDownBtnPushed)
	{
		SetImageSectionDownBtnPushed(GetImageSectionDownBtnPushed());

		return true;
	}

	return false;
}

void CDUISpinCtrl::OnNotify(CDUIControlBase *pControl, const DuiNotify &Notify)
{
	if (pControl == m_pBtnUpCtrl)
	{
		switch (Notify.NotifyType)
		{
			case DuiNotify_Click:
			{
				OnDuiClickBtnUp(Notify);

				return;
			}
		}

		return;
	}
	if (pControl == m_pBtnDownCtrl)
	{
		switch (Notify.NotifyType)
		{
			case DuiNotify_Click:
			{
				OnDuiClickBtnDown(Notify);

				return;
			}
		}

		return;
	}
	if (pControl == m_pEditValueCtrl)
	{
		switch (Notify.NotifyType)
		{
			case DuiNotify_TextChanged:
			{
				OnDuiTextChangeValue(Notify);

				break;
			}
		}

		return;
	}

	return IDuiControlCallBack::OnNotify(pControl, Notify);
}

void CDUISpinCtrl::OnRelease(CDUIControlBase *pControl)
{
	if (NULL == pControl) return;

	pControl->UnRegisterControlCallBack(this);

	if (pControl == m_pBtnUpCtrl)
	{
		m_pBtnUpCtrl = NULL;
	}
	if (pControl == m_pBtnDownCtrl)
	{
		m_pBtnDownCtrl = NULL;
	}

	return IDuiControlCallBack::OnRelease(pControl);
}

void CDUISpinCtrl::OnDuiClickBtnUp(const DuiNotify &Notify)
{
	if (NULL == m_pBtnUpCtrl
		|| NULL == m_pEditValueCtrl
		|| Notify.pNotifyCtrl != m_pBtnUpCtrl) return;

	double lfNumSrc = _tcstod(m_pEditValueCtrl->GetText(), NULL) + 1;

	PerformAdjustValue(lfNumSrc);

	return;
}

void CDUISpinCtrl::OnDuiClickBtnDown(const DuiNotify &Notify)
{
	if (NULL == m_pBtnDownCtrl
		|| NULL == m_pEditValueCtrl
		|| Notify.pNotifyCtrl != m_pBtnDownCtrl) return;

	double lfNumSrc = _tcstod(m_pEditValueCtrl->GetText(), NULL) - 1;

	PerformAdjustValue(lfNumSrc);

	return;
}

void CDUISpinCtrl::OnDuiTextChangeValue(const DuiNotify &Notify)
{
	if (NULL == m_pEditValueCtrl
		|| NULL == m_pWndOwner
		|| Notify.pNotifyCtrl != m_pEditValueCtrl) return;

	DuiNotify NotifyTemp = Notify;
	NotifyTemp.pNotifyCtrl = this;
	m_pWndOwner->SendNotify(NotifyTemp);

	return;
}

LPVOID CDUISpinCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUISpinCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUISpinCtrl::GetDescribe() const
{
	return Dui_Ctrl_Spin;
}

bool CDUISpinCtrl::Remove(CDUIControlBase *pControl)
{
	if (pControl == m_pEditValueCtrl || pControl == m_pVertRightCtrl) return false;

	return __super::Remove(pControl);
}

bool CDUISpinCtrl::RemoveAt(int nIndex)
{
	CDUIControlBase *pControl = GetChildAt(nIndex);
	if (pControl == m_pEditValueCtrl || pControl == m_pVertRightCtrl) return false;

	return __super::RemoveAt(nIndex);
}

void CDUISpinCtrl::RemoveAll()
{
	for (int n = GetChildCount() - 1; n >= 0; n--)
	{
		RemoveAt(n);
	}

	return;
}

CMMString CDUISpinCtrl::GetText()
{
	return m_pEditValueCtrl ? m_pEditValueCtrl->GetText() : CMMString();
}

bool CDUISpinCtrl::SetText(LPCTSTR lpszText)
{
	if (NULL == m_pEditValueCtrl) return false;

	return m_pEditValueCtrl->SetText(lpszText);
}

enDuiEditTextType CDUISpinCtrl::GetEditTextType()
{
	return (enDuiEditTextType)m_AttributeEditTextType.GetCurSelItem();
}

void CDUISpinCtrl::SetEditTextType(enDuiEditTextType TextType)
{
#ifndef DUI_DESIGN
	if (TextType == GetEditTextType()) return;
#endif

	m_AttributeEditTextType.SelectItem(TextType);

	if (NULL == m_pEditValueCtrl) return;

	m_pEditValueCtrl->SetEditTextType(GetEditTextType());

	return;
}

CDUIRect CDUISpinCtrl::GetEditTextPadding()
{
	return m_AttributeEditTextPadding.GetValue();
}

void CDUISpinCtrl::SetEditTextPadding(CDUIRect rcPadding)
{
#ifndef DUI_DESIGN
	if (rcPadding == GetEditTextPadding()) return;
#endif

	m_AttributeEditTextPadding.SetValue(rcPadding);

	if (NULL == m_pEditValueCtrl) return;

	m_pEditValueCtrl->SetPadding(rcPadding);

	return;
}

tagDuiTextStyle CDUISpinCtrl::GetTextStyleNormal()
{
	return m_AttributeTextStyleNormal.GetTextStyle();
}

tagDuiTextStyle CDUISpinCtrl::GetTextStyleHot()
{
	return m_AttributeTextStyleHot.GetTextStyle();
}

tagDuiTextStyle CDUISpinCtrl::GetTextStylePushed()
{
	return m_AttributeTextStylePushed.GetTextStyle();
}

tagDuiTextStyle CDUISpinCtrl::GetTextStyleFocus()
{
	return m_AttributeTextStyleFocus.GetTextStyle();
}

void CDUISpinCtrl::SetTextStyleNormal(const tagDuiTextStyle &TextStyle)
{
	m_AttributeTextStyleNormal.SetTextStyle(TextStyle);

	if (NULL == m_pEditValueCtrl) return;

	m_pEditValueCtrl->SetTextStyleNormal(TextStyle);

	return;
}

void CDUISpinCtrl::SetTextStyleHot(const tagDuiTextStyle &TextStyle)
{
	m_AttributeTextStyleHot.SetTextStyle(TextStyle);

	if (NULL == m_pEditValueCtrl) return;

	m_pEditValueCtrl->SetTextStyleHot(TextStyle);

	return;
}

void CDUISpinCtrl::SetTextStylePushed(const tagDuiTextStyle &TextStyle)
{
	m_AttributeTextStylePushed.SetTextStyle(TextStyle);

	if (NULL == m_pEditValueCtrl) return;

	m_pEditValueCtrl->SetTextStylePushed(TextStyle);

	return;
}

void CDUISpinCtrl::SetTextStyleFocus(const tagDuiTextStyle &TextStyle)
{
	m_AttributeTextStyleFocus.SetTextStyle(TextStyle);

	if (NULL == m_pEditValueCtrl) return;

	m_pEditValueCtrl->SetTextStyleFocus(TextStyle);

	return;
}

int CDUISpinCtrl::GetUpDownBtnRightPadding()
{
	return m_AttributeUpDownBtnRightPadding.GetValue();
}

void CDUISpinCtrl::SetUpDownBtnRightPadding(int nPadding)
{
#ifndef DUI_DESIGN
	if (nPadding == GetUpDownBtnRightPadding()) return;
#endif

	m_AttributeUpDownBtnRightPadding.SetValue(nPadding);

	if (NULL == m_pVertRightCtrl) return;

	m_pVertRightCtrl->SetPaddingR(GetUpDownBtnRightPadding());

	return;
}

int CDUISpinCtrl::GetUpDownBtnFixedWidth()
{
	return m_AttributeUpDownBtnFixedWidth.GetValue();
}

void CDUISpinCtrl::SetUpDownBtnFixedWidth(int nWidth)
{
#ifndef DUI_DESIGN
	if (nWidth == GetUpDownBtnFixedWidth()) return;
#endif

	m_AttributeUpDownBtnFixedWidth.SetValue(nWidth);

	if (NULL == m_pVertRightCtrl) return;

	m_pVertRightCtrl->SetFixedWidth(nWidth);

	return;
}

tagDuiImageSection CDUISpinCtrl::GetImageSectionUpBtnNormal()
{
	return m_AttributeImageUpBtnNormal.GetImageSection();
}

tagDuiImageSection CDUISpinCtrl::GetImageSectionUpBtnHot()
{
	return m_AttributeImageUpBtnHot.GetImageSection();
}

tagDuiImageSection CDUISpinCtrl::GetImageSectionUpBtnPushed()
{
	return m_AttributeImageUpBtnPushed.GetImageSection();
}

tagDuiImageSection CDUISpinCtrl::GetImageSectionDownBtnNormal()
{
	return m_AttributeImageDownBtnNormal.GetImageSection();
}

tagDuiImageSection CDUISpinCtrl::GetImageSectionDownBtnHot()
{
	return m_AttributeImageDownBtnHot.GetImageSection();
}

tagDuiImageSection CDUISpinCtrl::GetImageSectionDownBtnPushed()
{
	return m_AttributeImageDownBtnPushed.GetImageSection();
}

void CDUISpinCtrl::SetImageSectionUpBtnNormal(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageUpBtnNormal.SetImageSection(ImageSection);

	if (NULL == m_pBtnUpCtrl) return;

	m_pBtnUpCtrl->SetImageSectionNormal(ImageSection);

	return;
}

void CDUISpinCtrl::SetImageSectionUpBtnHot(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageUpBtnHot.SetImageSection(ImageSection);

	if (NULL == m_pBtnUpCtrl) return;

	m_pBtnUpCtrl->SetImageSectionHot(ImageSection);

	return;
}

void CDUISpinCtrl::SetImageSectionUpBtnPushed(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageUpBtnPushed.SetImageSection(ImageSection);

	if (NULL == m_pBtnUpCtrl) return;

	m_pBtnUpCtrl->SetImageSectionPushed(ImageSection);

	return;
}

void CDUISpinCtrl::SetImageSectionDownBtnNormal(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageDownBtnNormal.SetImageSection(ImageSection);

	if (NULL == m_pBtnDownCtrl) return;

	m_pBtnDownCtrl->SetImageSectionNormal(ImageSection);

	return;
}

void CDUISpinCtrl::SetImageSectionDownBtnHot(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageDownBtnHot.SetImageSection(ImageSection);

	if (NULL == m_pBtnDownCtrl) return;

	m_pBtnDownCtrl->SetImageSectionHot(ImageSection);

	return;
}

void CDUISpinCtrl::SetImageSectionDownBtnPushed(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageDownBtnPushed.SetImageSection(ImageSection);

	if (NULL == m_pBtnDownCtrl) return;

	m_pBtnDownCtrl->SetImageSectionPushed(ImageSection);

	return;
}

void CDUISpinCtrl::InitProperty()
{
	__super::InitProperty();

	DuiCreateGroupAttribute(m_AttributeGroupSpin, _T("Spin"));
	DuiCreateAttribute(m_AttributeEditTextType, _T("EditTextType"), _T(""), m_AttributeGroupSpin);
	DuiCreateAttribute(m_AttributeEditTextPadding, _T("EditTextPadding"), _T(""), m_AttributeGroupSpin);
	DuiCreateAttribute(m_AttributeTextStyleNormal, _T("TextStyleNormal"), _T(""), m_AttributeGroupSpin);
	DuiCreateAttribute(m_AttributeTextStyleHot, _T("TextStyleHot"), _T(""), m_AttributeGroupSpin);
	DuiCreateAttribute(m_AttributeTextStylePushed, _T("TextStylePushed"), _T(""), m_AttributeGroupSpin);
	DuiCreateAttribute(m_AttributeTextStyleFocus, _T("TextStyleFocus"), _T(""), m_AttributeGroupSpin);
	DuiCreateAttribute(m_AttributeUpDownBtnRightPadding, _T("UpDownBtnRightPadding"), _T(""), m_AttributeGroupSpin);
	DuiCreateAttribute(m_AttributeUpDownBtnFixedWidth, _T("UpDownBtnFixedWidth"), _T(""), m_AttributeGroupSpin);
	DuiCreateAttribute(m_AttributeImageUpBtnNormal, _T("ImageUpBtnNormal"), _T(""), m_AttributeGroupSpin);
	DuiCreateAttribute(m_AttributeImageUpBtnHot, _T("ImageUpBtnHot"), _T(""), m_AttributeGroupSpin);
	DuiCreateAttribute(m_AttributeImageUpBtnPushed, _T("ImageUpBtnPushed"), _T(""), m_AttributeGroupSpin);
	DuiCreateAttribute(m_AttributeImageDownBtnNormal, _T("ImageDownBtnNormal"), _T(""), m_AttributeGroupSpin);
	DuiCreateAttribute(m_AttributeImageDownBtnHot, _T("ImageDownBtnHot"), _T(""), m_AttributeGroupSpin);
	DuiCreateAttribute(m_AttributeImageDownBtnPushed, _T("ImageDownBtnPushed"), _T(""), m_AttributeGroupSpin);

	return;
}

void CDUISpinCtrl::InitComplete()
{
	__super::InitComplete();

	//find
	if (NULL == m_pEditValueCtrl)
	{
		m_pEditValueCtrl = MMInterfaceHelper(CDUIXEditCtrl, FindSubControlThisView(Dui_CtrlIDInner_SpinEditValue));
	}
	if (NULL == m_pVertRightCtrl)
	{
		m_pVertRightCtrl = MMInterfaceHelper(CDUIVerticalLayoutCtrl, FindSubControlThisView(Dui_CtrlIDInner_SpinVertRight));
	}
	if (NULL == m_pBtnUpCtrl && m_pVertRightCtrl)
	{
		m_pBtnUpCtrl = MMInterfaceHelper(CDUIButtonCtrl, m_pVertRightCtrl->FindSubControlThisView(Dui_CtrlIDInner_SpinBtnUp));
	}
	if (NULL == m_pBtnDownCtrl && m_pVertRightCtrl)
	{
		m_pBtnDownCtrl = MMInterfaceHelper(CDUIButtonCtrl, m_pVertRightCtrl->FindSubControlThisView(Dui_CtrlIDInner_SpinBtnDown));
	}

	//create
	if (NULL == m_pEditValueCtrl)
	{
		m_pEditValueCtrl = new CDUIXEditCtrl;
		m_pEditValueCtrl->Init();
		m_pEditValueCtrl->SetCtrlID(Dui_CtrlIDInner_SpinEditValue);
		m_pEditValueCtrl->SetEditTextType(EditText_Number);
		m_pEditValueCtrl->SetAutoSelAll(true);
		InsertChild(m_pEditValueCtrl);
	}
	if (NULL == m_pVertRightCtrl)
	{
		m_pVertRightCtrl = new CDUIVerticalLayoutCtrl;
		m_pVertRightCtrl->Init();
		m_pVertRightCtrl->SetCtrlID(Dui_CtrlIDInner_SpinVertRight);
		m_pVertRightCtrl->SetHorizAlignType(HorizAlign_Right);
		m_pVertRightCtrl->SetPaddingR(GetUpDownBtnRightPadding());
		m_pVertRightCtrl->SetFixedWidth(GetUpDownBtnFixedWidth());
		InsertChild(m_pVertRightCtrl);
	}
	if (NULL == m_pBtnUpCtrl && m_pVertRightCtrl)
	{
		m_pBtnUpCtrl = new CDUIButtonCtrl;
		m_pBtnUpCtrl->Init();
		m_pBtnUpCtrl->SetCtrlID(Dui_CtrlIDInner_SpinBtnUp);
		m_pVertRightCtrl->InsertChild(m_pBtnUpCtrl);
	}
	if (NULL == m_pBtnDownCtrl && m_pVertRightCtrl)
	{
		m_pBtnDownCtrl = new CDUIButtonCtrl;
		m_pBtnDownCtrl->Init();
		m_pBtnDownCtrl->SetCtrlID(Dui_CtrlIDInner_SpinBtnDown);
		m_pVertRightCtrl->InsertChild(m_pBtnDownCtrl);
	}

	//info
	if (m_pBtnUpCtrl)
	{
		m_pBtnUpCtrl->RegisterControlCallBack(this);
	}
	if (m_pBtnDownCtrl)
	{
		m_pBtnDownCtrl->RegisterControlCallBack(this);
	}
	if (m_pEditValueCtrl)
	{
		m_pEditValueCtrl->RegisterControlCallBack(this);
	}

	//visible
	DuiInitAttriVisible(m_AttributeGroupScroll, false);
	DuiInitAttriVisible(m_AttributeGroupChild, false);

	return;
}

void CDUISpinCtrl::PerformAdjustValue(double lfValue)
{
	if (NULL == m_pEditValueCtrl) return;

	double lfNumAdjust = min(max(lfValue, m_pEditValueCtrl->GetNumberMinLimit()), m_pEditValueCtrl->GetNumberMaxLimit());
	if (EditText_NumberDouble == GetEditTextType())
	{
		m_pEditValueCtrl->SetText(CMMStrHelp::Format(_T("%0.6lf"), lfNumAdjust));
	}
	else
	{
		m_pEditValueCtrl->SetText(CMMStrHelp::Format(_T("%d"), (int)lfNumAdjust));
	}

	return;
}