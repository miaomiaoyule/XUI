#include "StdAfx.h"
#include "DUIDateTimeCtrl.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateControl(CDUIDateTimeCtrl)
MMImplement_ClassName(CDUIDateTimeCtrl)

CDUIDateTimeCtrl::CDUIDateTimeCtrl(void)
{
	m_AttributeBindCalendarDui.SetDuiType(DuiType_Calendar);

	return;
}

CDUIDateTimeCtrl::~CDUIDateTimeCtrl(void)
{
}

bool CDUIDateTimeCtrl::OnAttributeChange(CDUIAttributeObject *pAttributeObj)
{
	if (__super::OnAttributeChange(pAttributeObj)) return true;

	return false;
}

void CDUIDateTimeCtrl::OnNotify(CDUIControlBase *pControl, const DuiNotify &Notify)
{
	if (pControl == m_pBtnDateTimeCtrl)
	{
		if (DuiNotify_Click == Notify.NotifyType)
		{
			PerformActiveCalendar();
		}

		return;
	}

	return;
}

LPVOID CDUIDateTimeCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIDateTimeCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIDateTimeCtrl::GetDescribe() const
{
	return Dui_Ctrl_DateTime;
}

void CDUIDateTimeCtrl::RefreshView()
{
	__super::RefreshView();

	return;
}

bool CDUIDateTimeCtrl::InsertChild(CDUIControlBase *pChild, int nPos)
{
	if (NULL == m_pEditDateTimeCtrl
		&& MMInterfaceHelper(CDUIXEditCtrl, pChild)
		&& __super::InsertChild(pChild, nPos))
	{
		m_pEditDateTimeCtrl = MMInterfaceHelper(CDUIXEditCtrl, pChild);

		return true;
	}
	if (NULL == m_pBtnDateTimeCtrl
		&& MMInterfaceHelper(CDUIButtonCtrl, pChild)
		&& __super::InsertChild(pChild, nPos))
	{
		m_pBtnDateTimeCtrl = MMInterfaceHelper(CDUIButtonCtrl, pChild);

		return true;
	}

	return false;
}

CDUIControlBase * CDUIDateTimeCtrl::DetachChild(int nIndex)
{
	return NULL;
}

CDUIControlBase * CDUIDateTimeCtrl::DetachChild(CDUIControlBase *pControl)
{
	return NULL;
}

bool CDUIDateTimeCtrl::SetChildIndex(CDUIControlBase *pControl, int nIndex)
{
	return false;
}

bool CDUIDateTimeCtrl::SwapChild(CDUIControlBase *pFirstCtrl, CDUIControlBase *pNextCtrl, bool bNeedRefreshView)
{
	return false;
}

bool CDUIDateTimeCtrl::SwapChild(int nIndexFirst, int nIndexNext, bool bNeedRefreshView)
{
	return false;
}

void CDUIDateTimeCtrl::SetChildPadding(int nChildPaddingH, int nChildPaddingV)
{
	return;
}

bool CDUIDateTimeCtrl::Remove(CDUIControlBase *pControl)
{
	return false;
}

bool CDUIDateTimeCtrl::RemoveAt(int nIndex)
{
	return false;
}

void CDUIDateTimeCtrl::RemoveAll()
{
	return;
}

CDUIXEditCtrl * CDUIDateTimeCtrl::GetDataTimeEditCtrl()
{
	return m_pEditDateTimeCtrl;
}

CDUIButtonCtrl * CDUIDateTimeCtrl::GetDateTimeBtnCtrl()
{
	return m_pBtnDateTimeCtrl;
}

CMMString CDUIDateTimeCtrl::GetBindCalendarDui()
{
	return m_AttributeBindCalendarDui.GetCurSelDescribe();
}

void CDUIDateTimeCtrl::SetBindCalendarDui(CMMString strCalendarDui)
{
	m_AttributeBindCalendarDui.SelectItem(strCalendarDui);

	return;
}

SYSTEMTIME CDUIDateTimeCtrl::GetDateTime()
{
	return m_tDateTime;
}

void CDUIDateTimeCtrl::SetDateTime(SYSTEMTIME tDateTime)
{
	m_tDateTime = tDateTime;

	if (m_pEditDateTimeCtrl)
	{
		CMMString strDateTime = CMMStrHelp::Format(CMMStrHelp::Format(_T("%04d-%02d-%02d"), m_tDateTime.wYear, m_tDateTime.wMonth, m_tDateTime.wDay));
		m_pEditDateTimeCtrl->SetText(strDateTime);
	}

	return;
}

CDUISize CDUIDateTimeCtrl::MeasureString(LPCTSTR lpszText)
{
	return m_pEditDateTimeCtrl ? m_pEditDateTimeCtrl->MeasureString(lpszText) : CDUISize{};
}

HFONT CDUIDateTimeCtrl::GetFont()
{
	return m_pEditDateTimeCtrl ? m_pEditDateTimeCtrl->GetFont() : NULL;
}

HFONT CDUIDateTimeCtrl::GetFontNormal()
{
	return m_pEditDateTimeCtrl ? m_pEditDateTimeCtrl->GetFontNormal() : NULL;
}

HFONT CDUIDateTimeCtrl::GetFontHot()
{
	return m_pEditDateTimeCtrl ? m_pEditDateTimeCtrl->GetFontHot() : NULL;
}

HFONT CDUIDateTimeCtrl::GetFontPushed()
{
	return m_pEditDateTimeCtrl ? m_pEditDateTimeCtrl->GetFontPushed() : NULL;
}

HFONT CDUIDateTimeCtrl::GetFontFocus()
{
	return m_pEditDateTimeCtrl ? m_pEditDateTimeCtrl->GetFontFocus() : NULL;
}

HFONT CDUIDateTimeCtrl::GetFontDisabled()
{
	return m_pEditDateTimeCtrl ? m_pEditDateTimeCtrl->GetFontDisabled() : NULL;
}

Gdiplus::ARGB CDUIDateTimeCtrl::GetTextColor()
{
	return m_pEditDateTimeCtrl ? m_pEditDateTimeCtrl->GetTextColor() : NULL;
}

Gdiplus::ARGB CDUIDateTimeCtrl::GetTextColorNormal()
{
	return m_pEditDateTimeCtrl ? m_pEditDateTimeCtrl->GetTextColorNormal() : NULL;
}

Gdiplus::ARGB CDUIDateTimeCtrl::GetTextColorHot()
{
	return m_pEditDateTimeCtrl ? m_pEditDateTimeCtrl->GetTextColorHot() : NULL;
}

Gdiplus::ARGB CDUIDateTimeCtrl::GetTextColorPushed()
{
	return m_pEditDateTimeCtrl ? m_pEditDateTimeCtrl->GetTextColorPushed() : NULL;
}

Gdiplus::ARGB CDUIDateTimeCtrl::GetTextColorFocus()
{
	return m_pEditDateTimeCtrl ? m_pEditDateTimeCtrl->GetTextColorFocus() : NULL;
}

Gdiplus::ARGB CDUIDateTimeCtrl::GetTextColorDisabled()
{
	return m_pEditDateTimeCtrl ? m_pEditDateTimeCtrl->GetTextColorDisabled() : NULL;
}

tagDuiTextStyle CDUIDateTimeCtrl::GetTextStyle()
{
	return m_pEditDateTimeCtrl ? m_pEditDateTimeCtrl->GetTextStyle() : tagDuiTextStyle{};
}

tagDuiTextStyle CDUIDateTimeCtrl::GetTextStyleNormal()
{
	return m_pEditDateTimeCtrl ? m_pEditDateTimeCtrl->GetTextStyleNormal() : tagDuiTextStyle{};
}

tagDuiTextStyle CDUIDateTimeCtrl::GetTextStyleHot()
{
	return m_pEditDateTimeCtrl ? m_pEditDateTimeCtrl->GetTextStyleHot() : tagDuiTextStyle{};
}

tagDuiTextStyle CDUIDateTimeCtrl::GetTextStylePushed()
{
	return m_pEditDateTimeCtrl ? m_pEditDateTimeCtrl->GetTextStylePushed() : tagDuiTextStyle{};
}

tagDuiTextStyle CDUIDateTimeCtrl::GetTextStyleFocus()
{
	return m_pEditDateTimeCtrl ? m_pEditDateTimeCtrl->GetTextStyleFocus() : tagDuiTextStyle{};
}

tagDuiTextStyle CDUIDateTimeCtrl::GetTextStyleDisabled()
{
	return m_pEditDateTimeCtrl ? m_pEditDateTimeCtrl->GetTextStyleDisabled() : tagDuiTextStyle{};
}

tagDuiTextStyle CDUIDateTimeCtrl::GetTextStyleActive()
{
	return m_pEditDateTimeCtrl ? m_pEditDateTimeCtrl->GetTextStyleActive() : tagDuiTextStyle{};
}

void CDUIDateTimeCtrl::SetTextStyle(const tagDuiTextStyle &TextStyle)
{
	if (m_pEditDateTimeCtrl) m_pEditDateTimeCtrl->SetTextStyle(TextStyle);
}

void CDUIDateTimeCtrl::SetTextStyleNormal(const tagDuiTextStyle &TextStyle)
{
	if (m_pEditDateTimeCtrl) m_pEditDateTimeCtrl->SetTextStyleNormal(TextStyle);
}

void CDUIDateTimeCtrl::SetTextStyleHot(const tagDuiTextStyle &TextStyle)
{
	if (m_pEditDateTimeCtrl) m_pEditDateTimeCtrl->SetTextStyleHot(TextStyle);
}

void CDUIDateTimeCtrl::SetTextStylePushed(const tagDuiTextStyle &TextStyle)
{
	if (m_pEditDateTimeCtrl) m_pEditDateTimeCtrl->SetTextStylePushed(TextStyle);
}

void CDUIDateTimeCtrl::SetTextStyleFocus(const tagDuiTextStyle &TextStyle)
{
	if (m_pEditDateTimeCtrl) m_pEditDateTimeCtrl->SetTextStyleFocus(TextStyle);
}

void CDUIDateTimeCtrl::SetTextStyleDisabled(const tagDuiTextStyle &TextStyle)
{
	if (m_pEditDateTimeCtrl) m_pEditDateTimeCtrl->SetTextStyleDisabled(TextStyle);
}

tagDuiImageSection CDUIDateTimeCtrl::GetBtnImageSectionNormal()
{
	return m_pBtnDateTimeCtrl ? m_pBtnDateTimeCtrl->GetImageSectionNormal() : tagDuiImageSection();
}

tagDuiImageSection CDUIDateTimeCtrl::GetBtnImageSectionHot()
{
	return m_pBtnDateTimeCtrl ? m_pBtnDateTimeCtrl->GetImageSectionHot() : tagDuiImageSection();
}

tagDuiImageSection CDUIDateTimeCtrl::GetBtnImageSectionPushed()
{
	return m_pBtnDateTimeCtrl ? m_pBtnDateTimeCtrl->GetImageSectionPushed() : tagDuiImageSection();
}

tagDuiImageSection CDUIDateTimeCtrl::GetBtnImageSectionFocus()
{
	return m_pBtnDateTimeCtrl ? m_pBtnDateTimeCtrl->GetImageSectionFocus() : tagDuiImageSection();
}

tagDuiImageSection CDUIDateTimeCtrl::GetBtnImageSectionDisabled()
{
	return m_pBtnDateTimeCtrl ? m_pBtnDateTimeCtrl->GetImageSectionDisabled() : tagDuiImageSection();
}

void CDUIDateTimeCtrl::SetBtnImageSectionNormal(const tagDuiImageSection &ImageSection)
{
	if (m_pBtnDateTimeCtrl) m_pBtnDateTimeCtrl->SetImageSectionNormal(ImageSection);
}

void CDUIDateTimeCtrl::SetBtnImageSectionHot(const tagDuiImageSection &ImageSection)
{
	if (m_pBtnDateTimeCtrl) m_pBtnDateTimeCtrl->SetImageSectionHot(ImageSection);
}

void CDUIDateTimeCtrl::SetBtnImageSectionPushed(const tagDuiImageSection &ImageSection)
{
	if (m_pBtnDateTimeCtrl) m_pBtnDateTimeCtrl->SetImageSectionPushed(ImageSection);
}

void CDUIDateTimeCtrl::SetBtnImageSectionFocus(const tagDuiImageSection &ImageSection)
{
	if (m_pBtnDateTimeCtrl) m_pBtnDateTimeCtrl->SetImageSectionFocus(ImageSection);
}

void CDUIDateTimeCtrl::SetBtnImageSectionDisabled(const tagDuiImageSection &ImageSection)
{
	if (m_pBtnDateTimeCtrl) m_pBtnDateTimeCtrl->SetImageSectionDisabled(ImageSection);
}

void CDUIDateTimeCtrl::OnDuiWndManagerAttach()
{
	__super::OnDuiWndManagerAttach();

	return;
}

void CDUIDateTimeCtrl::InitProperty()
{
	__super::InitProperty();

	DuiCreateGroupAttribute(m_AttributeGroupDateTime, _T("DateTime"));
	DuiCreateAttribute(m_AttributeBindCalendarDui, _T("BindCalendarDui"), _T(""), m_AttributeGroupDateTime);

	return;
}

void CDUIDateTimeCtrl::InitComplete()
{
	__super::InitComplete();

	DuiInitAttriVisible(m_AttributeGroupAnimation, false);
	DuiInitAttriVisible(m_AttributeAutoCalcWidth, false);
	DuiInitAttriVisible(m_AttributeAutoCalcHeight, false);
	DuiInitAttriVisible(m_AttributeGroupScroll, false);
	DuiInitAttriVisible(m_AttributeGroupChild, false);

	//control
	if (NULL == m_pEditDateTimeCtrl)
	{
		CDUIXEditCtrl *pEditCtrl = new CDUIXEditCtrl();
		pEditCtrl->Init();

		InsertChild(pEditCtrl);
	}
	if (NULL == m_pBtnDateTimeCtrl)
	{
		CDUIButtonCtrl *pBtnCtrl = new CDUIButtonCtrl();
		pBtnCtrl->Init();
		pBtnCtrl->SetPaddingR(5);
		pBtnCtrl->SetPaddingVertCenter(0);
		pBtnCtrl->SetFixedWidth(25);
		pBtnCtrl->SetFixedHeight(25);
		InsertChild(pBtnCtrl);
	}

	//date time
	GetLocalTime(&m_tDateTime);
	SetDateTime(m_tDateTime);

	EnableScrollBar(false, false);

	//register
	if (m_pEditDateTimeCtrl)
	{
		m_pEditDateTimeCtrl->RegisterControlCallBack(this);
	}
	if (m_pBtnDateTimeCtrl)
	{
		m_pBtnDateTimeCtrl->RegisterControlCallBack(this);
	}

	return;
}

void CDUIDateTimeCtrl::PerformActiveCalendar()
{
	CMMString strCalendarDui = GetBindCalendarDui();
	if (strCalendarDui.empty())
	{
		MessageBox(GetWndHandle(), _T("Please Select Bind UI of Calendar"), NULL, NULL);

		return;
	}

	CDUICalendar Calendar;
	Calendar.LoadCalendar(strCalendarDui);
	if (NULL == g_pDuiCalendarWnd) return;

	CDUISize szWnd = g_pDuiCalendarWnd->GetWndInitSize();
	CDUIRect rcCtrl = GetAbsoluteRect();
	MMClientToScreen(rcCtrl, GetWndHandle());
	CDUIPoint ptPopup(rcCtrl.left + rcCtrl.GetWidth() / 2 - szWnd.cx / 2, rcCtrl.bottom);
	tagDuiCalendarCmd CalendarCmd = Calendar.TrackPopupCalendar(GetWndHandle(), ptPopup);

	if (CalendarCmd.bSelected)
	{
		SetDateTime(CalendarCmd.tTimeSelected);
	}

	return;
}