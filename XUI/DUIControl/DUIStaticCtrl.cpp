#include "StdAfx.h"
#include "DUIStaticCtrl.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateControl(CDUIStaticCtrl)
MMImplement_ClassName(CDUIStaticCtrl)

CDUIStaticCtrl::CDUIStaticCtrl(void)
{
}

CDUIStaticCtrl::~CDUIStaticCtrl(void)
{
	return;
}

bool CDUIStaticCtrl::OnAttributeChange(CDUIAttributeObject *pAttributeObj)
{
	if (__super::OnAttributeChange(pAttributeObj))
	{
		if (pAttributeObj == &m_AttributeVisible
			|| pAttributeObj == &m_AttributePosition)
		{
			NeedMeasureSize();
		}

		return true;
	}
	if (pAttributeObj == &m_AttributeText)
	{
		SetText(GetText());

		NeedMeasureSize();

		return true;
	}
	if (pAttributeObj == &m_AttributeAutoCalcWidth
		|| pAttributeObj == &m_AttributeAutoCalcHeight
		|| pAttributeObj == &m_AttributeTextStyle
		|| pAttributeObj == &m_AttributeTextPadding)
	{
		NeedMeasureSize();

		return true;
	}

	return false;
}

void CDUIStaticCtrl::OnDpiChanged(int nScalePre)
{
	__super::OnDpiChanged(nScalePre);

	m_bNeedMeasureSize = true;

	return;
}

LPVOID CDUIStaticCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIStaticCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIStaticCtrl::GetDescribe() const
{
	return Dui_Ctrl_Static;
}

bool CDUIStaticCtrl::SetAbsoluteRect(CDUIRect rcAbsolute)
{
	NeedMeasureSize();

	return __super::SetAbsoluteRect(rcAbsolute);
}

void CDUIStaticCtrl::SetMinWidth(long lWidth)
{
	NeedMeasureSize();

	return __super::SetMinWidth(lWidth);
}

void CDUIStaticCtrl::SetMaxWidth(long lWidth)
{
	NeedMeasureSize();

	return __super::SetMaxWidth(lWidth);
}

void CDUIStaticCtrl::SetMinHeight(long lHeight)
{
	NeedMeasureSize();

	return __super::SetMinHeight(lHeight);
}

void CDUIStaticCtrl::SetMaxHeight(long lHeight)
{
	NeedMeasureSize();

	return __super::SetMaxHeight(lHeight);
}

void CDUIStaticCtrl::RefreshView()
{
	__super::RefreshView();

	if (m_bNeedMeasureSize && (IsAutoCalcWidth() || IsAutoCalcHeight()))
	{
		m_bNeedMeasureSize = false;
		tagDuiTextStyle TextStyle = GetTextStyle();

		//refresh width
		if (IsAutoCalcWidth()
			&& TextStyle.IsWordBreak())
		{
			SetFixedWidth(0);

			if (GetParent())
			{
				GetParent()->RefreshView();

				m_bNeedMeasureSize = false;
			}
		}

		//measure
		CDUISize szMeasure = MeasureString();
		CDUIRect rcTextPadding = GetTextPadding();
		int nWidthAuto = szMeasure.cx + rcTextPadding.left + rcTextPadding.right;
		int nHeightAuto = szMeasure.cy + rcTextPadding.top + rcTextPadding.bottom;
		nWidthAuto = min(GetMaxWidth(), max(nWidthAuto, GetMinWidth()));
		nHeightAuto = min(GetMaxHeight(), max(nHeightAuto, GetMinHeight()));
		nWidthAuto = DuiDpiScaleBackCtrl(nWidthAuto);
		nHeightAuto = DuiDpiScaleBackCtrl(nHeightAuto);
		if (IsAutoCalcWidth())
		{
			bool bCalcHeight = false;
			do 
			{
				if (TextStyle.IsWordBreak())
				{
					bCalcHeight = true;
					SetFixedWidth(nWidthAuto);
				}
				else if (DuiDpiScaleCtrl(nWidthAuto) != GetFixedWidth()
					&& nWidthAuto != DuiDpiScaleBackCtrl(GetFixedWidth()))
				{
					bCalcHeight = true;
					SetFixedWidth(nWidthAuto);
				}

			} while (false);
			if (bCalcHeight 
				&& GetParent()
				&& IsAutoCalcHeight())
			{
				GetParent()->RefreshView();
				m_bNeedMeasureSize = false;

				szMeasure = MeasureString();
				szMeasure.cy = szMeasure.cy + rcTextPadding.top + rcTextPadding.bottom;
				szMeasure.cy = min(GetMaxHeight(), max(szMeasure.cy, GetMinHeight()));
				nHeightAuto = DuiDpiScaleBackCtrl(szMeasure.cy);
			}
		}
		if (IsAutoCalcHeight()
			&& DuiDpiScaleCtrl(nHeightAuto) != GetFixedHeight()
			&& nHeightAuto != DuiDpiScaleBackCtrl(GetFixedHeight()))
		{
			SetFixedHeight(nHeightAuto);
		}

		m_bNeedMeasureSize = false;
	}

	return;
}

void CDUIStaticCtrl::NeedMeasureSize()
{
	m_bNeedMeasureSize = true;

	NeedRefreshView();

	return;
}

bool CDUIStaticCtrl::IsAutoCalcWidth()
{
	return m_AttributeAutoCalcWidth.GetValue();
}

void CDUIStaticCtrl::SetAutoCalcWidth(bool bAutoCalcWidth)
{
	if (bAutoCalcWidth == IsAutoCalcWidth()) return;

	m_AttributeAutoCalcWidth.SetValue(bAutoCalcWidth);

	NeedMeasureSize();

	return;
}

bool CDUIStaticCtrl::IsAutoCalcHeight()
{
	return m_AttributeAutoCalcHeight.GetValue();
}

void CDUIStaticCtrl::SetAutoCalcHeight(bool bAutoCalcHeight)
{
	if (bAutoCalcHeight == IsAutoCalcHeight()) return;

	m_AttributeAutoCalcHeight.SetValue(bAutoCalcHeight);

	NeedMeasureSize();

	return;
}

CDUISize CDUIStaticCtrl::MeasureString(LPCTSTR lpszText)
{
	if (false == m_AttributeRichText.IsEmpty())
	{
		return  m_AttributeRichText.MeasureString();
	}

	return m_AttributeTextStyle.MeasureString(MMInvalidString(lpszText) ? (LPCTSTR)GetText() : lpszText);
}

CMMString CDUIStaticCtrl::GetText()
{
	return m_AttributeText.GetValue();
}

bool CDUIStaticCtrl::SetText(LPCTSTR lpszText)
{
	if (lpszText == GetText()) return true;

	m_AttributeText.SetValue(lpszText);

	if (m_pWndOwner)
	{
		m_pWndOwner->SendNotify(this, DuiNotify_TextChanged);
	}
	if (IsAutoCalcWidth() || IsAutoCalcHeight())
	{
		NeedMeasureSize();

		return true;
	}

	Invalidate();

	return true;
}

bool CDUIStaticCtrl::IsShadowText()
{
	return m_AttribuetShadow.GetValue();
}

void CDUIStaticCtrl::SetShadowText(bool bShadow)
{
	if (bShadow == IsShadowText()) return;

	m_AttribuetShadow.SetValue(bShadow);

	Invalidate();

	return;
}

HFONT CDUIStaticCtrl::GetFont()
{
	return m_AttributeTextStyle.GetFont();
}

Gdiplus::ARGB CDUIStaticCtrl::GetTextColor()
{
	return m_AttributeTextStyle.GetTextColor();
}

tagDuiTextStyle CDUIStaticCtrl::GetTextStyle()
{
	return m_AttributeTextStyle.GetTextStyle();
}

tagDuiTextStyle CDUIStaticCtrl::GetTextStyleActive()
{
	return GetTextStyle();
}

void CDUIStaticCtrl::SetTextStyle(const tagDuiTextStyle &TextStyle)
{
	m_AttributeTextStyle.SetTextStyle(TextStyle);

	if (IsAutoCalcWidth() || IsAutoCalcHeight())
	{
		NeedMeasureSize();

		return;
	}

	Invalidate();

	return;
}

VecDuiRichTextItem CDUIStaticCtrl::GetRichTextItem()
{
	return m_AttributeRichText.GetRichTextItem();
}

bool CDUIStaticCtrl::SetRichTextItem(const VecDuiRichTextItem &vecRichTextItem)
{
	tagDuiRichText RichText = GetRichText();
	RichText.vecRichTextItem = vecRichTextItem;
	return SetRichText(RichText);
}

tagDuiRichText CDUIStaticCtrl::GetRichText()
{
	return m_AttributeRichText.GetRichText();
}

bool CDUIStaticCtrl::SetRichText(const tagDuiRichText &RichText)
{
	if (RichText == GetRichText()) return true;

	m_AttributeRichText.SetRichText(RichText);

	if (m_pWndOwner)
	{
		m_pWndOwner->SendNotify(this, DuiNotify_TextChanged);
	}
	if (IsAutoCalcWidth() || IsAutoCalcHeight())
	{
		NeedMeasureSize();

		return true;
	}

	Invalidate();

	return true;
}

int CDUIStaticCtrl::GetRichTextLineSpace()
{
	return DuiDpiScaleCtrl(m_AttributeRichTextLineSpace.GetValue());
}

void CDUIStaticCtrl::SetRichTextLineSpace(int nLineSpace)
{
	m_AttributeRichTextLineSpace.SetValue(nLineSpace);

	NeedRefreshView();

	return;
}

RECT CDUIStaticCtrl::GetTextPadding()
{
	return DuiDpiScaleCtrl(m_AttributeTextPadding.GetValue());
}

void CDUIStaticCtrl::SetTextPadding(RECT rcPadding)
{
	if (DuiDpiScaleCtrl(rcPadding) == GetTextPadding()) return;

	m_AttributeTextPadding.SetValue(rcPadding);

	if (IsAutoCalcWidth() || IsAutoCalcHeight())
	{
		NeedMeasureSize();

		return;
	}

	Invalidate();

	return;
}

void CDUIStaticCtrl::InitProperty()
{
	__super::InitProperty();

	DuiCreateAttribute(m_AttributeAutoCalcWidth, _T("AutoCalcWidth"), _T(""), m_AttributeGroupPosition);
	DuiCreateAttribute(m_AttributeAutoCalcHeight, _T("AutoCalcHeight"), _T(""), m_AttributeGroupPosition);

	DuiCreateGroupAttribute(m_AttributeGroupText, _T("Text"));
	DuiCreateAttribute(m_AttributeText, _T("Text"), _T(""), m_AttributeGroupText);
	DuiCreateAttribute(m_AttribuetShadow, _T("Shadow"), _T(""), m_AttributeGroupText);
	DuiCreateAttribute(m_AttributeTextStyle, _T("TextStyle"), _T(""), m_AttributeGroupText);
	DuiCreateAttribute(m_AttributeRichText, _T("RichText"), _T(""), m_AttributeGroupText);
	DuiCreateAttribute(m_AttributeRichTextLineSpace, _T("RichTextLineSpace"), _T(""), m_AttributeGroupText);
	DuiCreateAttribute(m_AttributeTextPadding, _T("TextPadding"), _T(""), m_AttributeGroupText);

	return;
}

void CDUIStaticCtrl::PaintText(HDC hDC)
{
	if (NULL == m_pWndOwner) return;

	//range
	CDUIRect rcRange = GetTextRange();

	//richtext
	if (false == m_AttributeRichText.IsEmpty())
	{
		m_AttributeRichText.Draw(hDC, rcRange, m_pWndOwner->IsGdiplusRenderText(), m_pWndOwner->GetGdiplusRenderTextType(), GetRichTextLineSpace(), IsShadowText());

		return;
	}

	//text
	CMMString strText = GetText();
	if (strText.empty()) return;

	m_AttributeTextStyle.Draw(hDC, rcRange, strText, m_pWndOwner->IsGdiplusRenderText(), m_pWndOwner->GetGdiplusRenderTextType(), IsShadowText());

	return;
}

void CDUIStaticCtrl::OnDuiSize(CDUIRect rcParentAbs)
{
	int nWidthOld = GetWidth();

	__super::OnDuiSize(rcParentAbs);

	if (IsAutoCalcHeight() && nWidthOld != GetWidth())
	{
		NeedMeasureSize();
	}

	return;
}

CDUIAttributeTextStyle * CDUIStaticCtrl::GetAttributeTextStyleActive()
{
	return &m_AttributeTextStyle;
}

CDUIRect CDUIStaticCtrl::GetTextRange()
{
	CDUIRect rcRange = GetAbsoluteRect();
	CDUIRect rcTextPadding = GetTextPadding();

	rcRange.left += rcTextPadding.left;
	rcRange.right -= rcTextPadding.right;
	rcRange.top += rcTextPadding.top;
	rcRange.bottom -= rcTextPadding.bottom;
	rcRange.CheckRect();

	return rcRange;
}