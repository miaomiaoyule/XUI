#ifndef __DUISPINCTRL_H__
#define __DUISPINCTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
class CDUIXEditCtrl;

//////////////////////////////////////////////////////////////////////////
#define VER_CDUISpinCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUISpinCtrl = { 0xFD64C6EC,0xA2AC,0x4228,0xB2,0x02,0x3E,0xA7,0x72,0x42,0x97,0xF7 };
class XUI_API CDUISpinCtrl
	: public CDUIHorizontalLayoutCtrl
	, public IDuiControlCallBack
{
	MMDeclare_Super(CDUIHorizontalLayoutCtrl)
	DuiDeclare_CreateControl(CDUISpinCtrl)
	MMDeclare_ClassName(CDUISpinCtrl)

public:
	CDUISpinCtrl(void);
	~CDUISpinCtrl(void);

	//attribute
protected:
	CDUIAttributeGroup					m_AttributeGroupSpin;
	CDUIAttributeCombox					m_AttributeEditTextType;
	CDUIAttributeRect					m_AttributeEditTextPadding;
	CDUIAttributeTextStyle				m_AttributeTextStyleNormal;
	CDUIAttributeTextStyle				m_AttributeTextStyleHot;
	CDUIAttributeTextStyle				m_AttributeTextStylePushed;
	CDUIAttributeTextStyle				m_AttributeTextStyleFocus;
	CDUIAttributeLong					m_AttributeUpDownBtnRightPadding;
	CDUIAttributeLong					m_AttributeUpDownBtnFixedWidth = 30;
	CDUIAttriImageSection				m_AttributeImageUpBtnNormal;
	CDUIAttriImageSection				m_AttributeImageUpBtnHot;
	CDUIAttriImageSection				m_AttributeImageUpBtnPushed;
	CDUIAttriImageSection				m_AttributeImageDownBtnNormal;
	CDUIAttriImageSection				m_AttributeImageDownBtnHot;
	CDUIAttriImageSection				m_AttributeImageDownBtnPushed;

	//variant
protected:
	CDUIXEditCtrl *						m_pEditValueCtrl = NULL;
	CDUIVerticalLayoutCtrl *			m_pVertRightCtrl = NULL;
	CDUIButtonCtrl *					m_pBtnUpCtrl = NULL;
	CDUIButtonCtrl *					m_pBtnDownCtrl = NULL;

	//override
protected:
	bool OnAttributeChange(CDUIAttributeObject *pAttributeObj) override;

	//override
protected:
	void OnNotify(CDUIControlBase *pControl, const DuiNotify &Notify) override;
	void OnRelease(CDUIControlBase *pControl) override;

	void OnDuiClickBtnUp(const DuiNotify &Notify);
	void OnDuiClickBtnDown(const DuiNotify &Notify);
	void OnDuiTextChangeValue(const DuiNotify &Notify);

	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;

	//child
	bool Remove(CDUIControlBase *pControl) override;
	bool RemoveAt(int nIndex) override;
	void RemoveAll() override;

	//spin
	virtual CMMString GetText();
	virtual bool SetText(LPCTSTR lpszText);
	virtual enDuiEditTextType GetEditTextType();
	virtual void SetEditTextType(enDuiEditTextType TextType);
	virtual CDUIRect GetEditTextPadding();
	virtual void SetEditTextPadding(CDUIRect rcPadding);
	virtual tagDuiTextStyle GetTextStyleNormal();
	virtual tagDuiTextStyle GetTextStyleHot();
	virtual tagDuiTextStyle GetTextStylePushed();
	virtual tagDuiTextStyle GetTextStyleFocus();
	virtual void SetTextStyleNormal(const tagDuiTextStyle &TextStyle);
	virtual void SetTextStyleHot(const tagDuiTextStyle &TextStyle);
	virtual void SetTextStylePushed(const tagDuiTextStyle &TextStyle);
	virtual void SetTextStyleFocus(const tagDuiTextStyle &TextStyle);
	virtual int GetUpDownBtnRightPadding();
	virtual void SetUpDownBtnRightPadding(int nPadding);
	virtual int GetUpDownBtnFixedWidth();
	virtual void SetUpDownBtnFixedWidth(int nWidth);
	virtual tagDuiImageSection GetImageSectionUpBtnNormal();
	virtual tagDuiImageSection GetImageSectionUpBtnHot();
	virtual tagDuiImageSection GetImageSectionUpBtnPushed();
	virtual tagDuiImageSection GetImageSectionDownBtnNormal();
	virtual tagDuiImageSection GetImageSectionDownBtnHot();
	virtual tagDuiImageSection GetImageSectionDownBtnPushed();
	virtual void SetImageSectionUpBtnNormal(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionUpBtnHot(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionUpBtnPushed(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionDownBtnNormal(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionDownBtnHot(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionDownBtnPushed(const tagDuiImageSection &ImageSection);

	//message
protected:

	//property
protected:
	void InitProperty() override;
	void InitComplete() override;

	//help
protected:
	void PerformAdjustValue(double lfValue);
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
