#ifndef __DUILISTVIEWITEMCTRL_H__
#define __DUILISTVIEWITEMCTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIListItemCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUIListItemCtrl = { 0xFFEE2302,0x6B5D,0x4688,0xB0,0x95,0x1E,0x9F,0x28,0x28,0x85,0x98 };
class XUI_API CDUIListItemCtrl
	: public CDUIHorizontalLayoutCtrl
	, public IDuiControlCallBack
{
	friend class CDUIListViewCtrl;

	MMDeclare_Super(CDUIHorizontalLayoutCtrl)
	DuiDeclare_CreateControl(CDUIListItemCtrl)
	MMDeclare_ClassName(CDUIListItemCtrl)

public:
	CDUIListItemCtrl(void);
	~CDUIListItemCtrl(void);

	//property
protected:
	//text
	CDUIAttributeGroup					m_AttributeGroupText;
	CDUIAttributeText					m_AttributeText;
	CDUIAttributeTextStyle				m_AttributeTextStyle;
	CDUIAttributeRect					m_AttributeTextPadding;

	//variant
protected:
	CDUIListViewCtrl *					m_pOwner = NULL;
	CDUICheckBoxCtrl *					m_pCheckSelectCtrl = NULL;
	CDUIXEditCtrl *						m_pEditTextCtrl = NULL;
	int									m_nIndex = -1;

	//override
protected:
	void OnNotify(CDUIControlBase *pControl, const DuiNotify &Notify) override;
	void OnRelease(CDUIControlBase *pControl) override;
	void OnSize(CDUIControlBase *pControl) override;

	//diable method, because first child is checkicon, second child is edit.
	//traverse user custom child use GetChildCountUser()、GetChildAtUser()、RemoveAtUser().
protected:
	int GetChildCount() const override;
	bool InsertChild(CDUIControlBase *pChild, int nPos = -1) override;
	CDUIControlBase * GetChildAt(int nIndex) const override;
	bool RemoveAt(int nIndex) override;

	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;
	UINT GetControlFlags()  override;

	//item
	bool Active() override;
	virtual void EnsureVisible(bool bCenter = false);
	virtual int GetIndex() const;
	virtual void SetIndex(int nIndex);
	virtual CDUIListViewCtrl * GetOwner();
	virtual void SetOwner(CDUIListViewCtrl *pOwner);
	virtual bool IsSelected() const;
	virtual bool Select(bool bSelect = true);
	virtual CDUICheckBoxCtrl * GetSelectIconCtrl();
	virtual bool IsSelectIconVisible();
	virtual void SetSelectIconVisible(bool bVisible);
	virtual int GetSelectIconLeftPadding();
	virtual void SetSelectIconLeftPadding(int nLeft);
	virtual int GetSelectIconFixedWidth();
	virtual void SetSelectIconFixedWidth(int nWidth);
	virtual int GetSelectIconFixedHeight();
	virtual void SetSelectIconFixedHeight(int nHeight);
	virtual tagDuiImageSection GetImageSectionSelIconNormal();
	virtual tagDuiImageSection GetImageSectionSelIconHot();
	virtual tagDuiImageSection GetImageSectionSelIconPushed();
	virtual tagDuiImageSection GetImageSectionSelIconSelNormal();
	virtual tagDuiImageSection GetImageSectionSelIconSelHot();
	virtual tagDuiImageSection GetImageSectionSelIconSelPushed();
	virtual void SetImageSectionSelIconNormal(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionSelIconHot(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionSelIconPushed(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionSelIconSelNormal(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionSelIconSelHot(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionSelIconSelPushed(const tagDuiImageSection &ImageSection);

	//layout
	void SetFixedWidth(int nWidth) override;
	void SetFixedHeight(int nHeight) override;

	//basic
	void SetVisible(bool bVisible = true) override;
	void SetEnabled(bool bEnabled) override;
	void SetOwnerModelCtrl(CDUIControlBase *pOwnerModelCtrl) override;

	//child
	bool Remove(CDUIControlBase *pControl) override;
	void RemoveAll() override;
	virtual bool InsertChildAtUser(CDUIControlBase *pChild, int nPos = -1);
	virtual bool RemoveAtUser(int nIndex);
	virtual int GetChildCountUser() const;
	virtual CDUIControlBase * GetChildAtUser(int nIndex) const;

	//refresh
	void RefreshView() override;
	virtual void RefreshSubItem();
	bool DoPaint(HDC hDC, bool bGenerateBmp = false) override;

	//text
	virtual CMMString GetText();
	virtual bool SetText(LPCTSTR lpszText);
	virtual HFONT GetFont();
	virtual Gdiplus::ARGB GetTextColor();
	virtual tagDuiTextStyle GetTextStyle();
	virtual tagDuiTextStyle GetTextStyleActive();
	virtual void SetTextStyle(const tagDuiTextStyle &TextStyle);
	virtual RECT GetTextPadding();
	virtual void SetTextPadding(RECT rcPadding);
	virtual void PerformEditText();
	virtual CDUIXEditCtrl * GetEditTextCtrl();

	//message
protected:
	bool OnDuiLButtonDown(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiLButtonUp(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiLButtonDlk(const CDUIPoint &pt, const DuiMessage &Msg) override;

	bool OnDuiRButtonDown(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiRButtonUp(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiRButtonDlk(const CDUIPoint &pt, const DuiMessage &Msg) override;

	bool OnDuiMouseEnter(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiMouseMove(const CDUIPoint &pt, const DuiMessage &Msg) override;
	void OnDuiMouseLeave(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiMouseWheel(const CDUIPoint &pt, const DuiMessage &Msg) override;

	bool OnDuiSetFocus() override;
	bool OnDuiKillFocus() override;

	void OnDuiTimer(UINT uTimerID, const DuiMessage &Msg) override;
	void OnDuiSize(CDUIRect rcParentAbs) override;

	LRESULT	OnDuiKeyDown(const DuiMessage &Msg) override;
	LRESULT OnDuiKeyUp(const DuiMessage &Msg) override;
	LRESULT	OnDuiContextMenu(const DuiMessage &Msg) override;

	//property
protected:
	void InitProperty() override;
	void InitComplete() override;

	//paint
protected:
	void PaintText(HDC hDC) override;
	virtual void PaintItemColor(HDC hDC);
	virtual void PaintItemImage(HDC hDC);
	virtual void PaintLine(HDC hDC);
	virtual void PaintLineOnListType(HDC hDC);
	virtual void PaintLineOnTileType(HDC hDC);

	//help
protected:
	virtual void SendNotify(enDuiNotifyType NotifyType, WPARAM wParam = 0, LPARAM lParam = 0, CMMString strTextOld = _T(""));
	virtual void PerformItemMouseDown(bool bLeft, const CDUIPoint &pt);
	virtual CDUIRect GetTextRange();
	virtual CDUIRect GetBackRange();
	virtual tagDuiListInfo GetItemStyleInfo();
	virtual CDUIAttributeTextStyle * GetAttributeTextStyleCur();
};

typedef std::vector<CDUIListItemCtrl*> VecDUIListItemCtrl;

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif