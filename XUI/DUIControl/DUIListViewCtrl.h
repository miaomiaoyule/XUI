#ifndef __DUILISTVIEWCTRL_H__
#define __DUILISTVIEWCTRL_H__

#pragma once
#pragma pack(1)

///////////////////////////////////////////////////////////////////////////
#define VER_CDUIListViewCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUIListViewCtrl = { 0xE4DE289D,0x472A,0x45C4,0xBA,0x8D,0xAB,0x2A,0xDA,0x58,0xEF,0x9D };
class XUI_API CDUIListViewCtrl
	: public CDUIVerticalLayoutCtrl
{
	friend class CDUIListHeaderCtrl;
	friend class CDUIListItemCtrl;
	friend class CDUICalendarCtrl;
	friend class CDUIComboxCtrl;

	MMDeclare_Super(CDUIVerticalLayoutCtrl)
	DuiDeclare_CreateControl(CDUIListViewCtrl)
	MMDeclare_ClassName(CDUIListViewCtrl)

public:
	CDUIListViewCtrl(void);
	~CDUIListViewCtrl(void);

	//attribute
protected:
	//listview
	CDUIAttributeGroup					m_AttributeGroupListView;
	CDUIAttributeBool					m_AttributeUseListHeader;
	CDUIAttributeCombox					m_AttributeListViewType;
	CDUIAttributeBool					m_AttributeMultiSel;
	CDUIAttributeBool					m_AttributeScrollSelect;
	CDUIAttributeDuiSelect				m_AttributeItemModel;
	CDUIAttributeLong					m_AttributeItemModelInitCount;
	CDUIAttriImageSection				m_AttributeImageMouseDragSel;

	//list type
	CDUIAttributeGroup					m_AttributeGroupListType;
	CDUIAttributeLong					m_AttributeSwitchListItemHeight = 30;
	CDUIAttributeBool					m_AttributeSelectIconVisible;
	CDUIAttributeBool					m_AttributeSelectDbClick;
	CDUIAttributeLong					m_AttributeSelectIconLeftPadding;
	CDUIAttributeLong					m_AttributeSelectIconFixedWidth = 30;
	CDUIAttributeLong					m_AttributeSelectIconFixedHeight = 30;
	CDUIAttriImageSection				m_AttributeImageSelectIconNormal;
	CDUIAttriImageSection				m_AttributeImageSelectIconHot;
	CDUIAttriImageSection				m_AttributeImageSelectIconPushed;
	CDUIAttriImageSection				m_AttributeImageSelectIconSelNormal;
	CDUIAttriImageSection				m_AttributeImageSelectIconSelHot;
	CDUIAttriImageSection				m_AttributeImageSelectIconSelPushed;

	//tile type
	CDUIAttributeGroup					m_AttributeGroupTileType;
	CDUIAttributeBool					m_AttributeAutoCalcItemWidth;
	CDUIAttributeBool					m_AttributeAutoCalcItemHeight;
	CDUIAttributeBool					m_AttributeLeftFromRange;
	CDUIAttributeBool					m_AttributeTopFromRange;
	CDUIAttributeSize					m_AttributeSwitchTileItemSize = SIZE{ 50, 50 };

	//item text
	CDUIAttributeGroup					m_AttributeGroupItemText;
	CDUIAttributeTextStyle				m_AttributeItemTextStyleNormal;
	CDUIAttributeTextStyle				m_AttributeItemTextStyleHot;
	CDUIAttributeTextStyle				m_AttributeItemTextStyleSelNormal;
	CDUIAttributeTextStyle				m_AttributeItemTextStyleSelHot;
	CDUIAttributeTextStyle				m_AttributeItemTextStyleDisabled;
	CDUIAttributeRect					m_AttributeItemTextPadding;

	//item status
	CDUIAttributeGroup					m_AttributeGroupItemStatus;
	CDUIAttributeColorSwitch			m_AttributeColorItemStatusNormal;
	CDUIAttributeColorSwitch			m_AttributeColorItemStatusHot = Name_ColorSelBk;
	CDUIAttributeColorSwitch			m_AttributeColorItemStatusSelNormal;
	CDUIAttributeColorSwitch			m_AttributeColorItemStatusSelHot = Name_ColorSelBk;
	CDUIAttributeColorSwitch			m_AttributeColorItemStatusDisabled;
	CDUIAttriImageSection				m_AttributeImageItemStatusNormal;
	CDUIAttriImageSection				m_AttributeImageItemStatusHot;
	CDUIAttriImageSection				m_AttributeImageItemStatusSelNormal;
	CDUIAttriImageSection				m_AttributeImageItemStatusSelHot;
	CDUIAttriImageSection				m_AttributeImageItemStatusDisabled;

	//line
	CDUIAttributeGroup					m_AttributeGroupLine;
	CDUIAttributeColorSwitch			m_AttributeColorLine;
	CDUIAttributeBool					m_AttributeShowRowLine;
	CDUIAttributeBool					m_AttributeShowColumnLine;

	//variant
protected:
	CDUIListHeaderCtrl *				m_pListHeader = NULL;
	CDUIPoint							m_ptMouseDown;
	CDUIRect							m_rcMouseDragSel;

	int									m_nCurSel = -1;
	VecDUIListItemCtrl					m_vecSelItem;
	VecDUIListItemCtrl					m_vecSelItemMouseDragPre;

	CDUISize							m_szTotalRange;
	int									m_nFirstShowIndex = -1;
	int									m_nLastShowIndex = -1;
	int									m_nVisibleCount = 0;

	int									m_nRow = 0;
	int									m_nColumn = 0;
	int									m_nHeightRow = 0;
	int									m_nWidthColumn = 0;

	PLVCompareFunc						m_pCompareFunc = NULL;
	UINT_PTR							m_CompareData = 0;

	//override
protected:
	bool RegisterControlListen(IDuiControlListen *pIDuiControlListen) override;
	bool UnRegisterControlListen(IDuiControlListen *pIDuiControlListen) override;

	//override
protected:
	void OnDpiChanged(int nScalePre) override;
	bool OnAttributeChange(CDUIAttributeObject *pAttributeObj) override;

	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;
	CDUIListViewCtrl * Clone(bool bIncludeChild = true, bool bRefreshCtrlID = true) override;

	//basic
	UINT InitCtrlID() override;
	void RefreshCtrlID(bool bSelfSingle = false) override;
	bool SetWndOwner(CDUIWnd *pWndOwner) override;
	void SetVisible(bool bVisible = true) override;
	void SetAnimateDrag(bool bAnimateDrag) override;

	//refresh
	void NeedRefreshView() override;
	void RefreshView() override;
	bool DoPaint(HDC hDC, bool bGenerateBmp = false) override;

	//listview
	virtual tagDuiListInfo GetListInfo();
	virtual CDUISize GetTotalRange();
	virtual bool IsUseListHeader();
	virtual void SetUseListHeader(bool bUseListHeader);
	virtual enDuiListViewType GetListViewType();
	virtual void SwitchListViewType(enDuiListViewType ListViewType);
	virtual tagDuiImageSection GetImageSectionMouseDragSel();
	virtual void SetImageSectionMouseDragSel(const tagDuiImageSection &ImageSection);

	//list type
	virtual CDUIListHeaderCtrl * GetListHeader() const;
	virtual int GetSwitchListItemHeight();
	virtual void SetSwitchListItemHeight(int nHeight);
	virtual bool IsSelectIconVisible();
	virtual void SetSelectIconVisible(bool bVisible);
	virtual bool IsSelectDbClick();
	virtual void SetSelectDbClick(bool bDbClick);
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

	//tile type
	virtual CDUISize GetSwitchTileItemSize();
	virtual void SetSwitchTileItemSize(CDUISize szItem);
	virtual bool IsAutoCalcItemWidth();
	virtual void SetAutoCalcItemWidth(bool bAuto);
	virtual bool IsAutoCalcItemHeight();
	virtual void SetAutoCalcItemHeight(bool bAuto);
	virtual bool IsLeftFromRange();
	virtual void SetLeftFromRange(bool bLeftFromRange);
	virtual bool IsTopFromRange();
	virtual void SetTopFromRange(bool bTopFromRange);

	//select
	virtual bool IsScrollSelect();
	virtual void SetScrollSelect(bool bScrollSelect);
	virtual bool IsMultiSelect() ;
	virtual void SetMultiSelect(bool bMultiSel);
	virtual int GetSelectItemCount();
	virtual int GetCurSel() const;
	virtual int GetFirstSelect() const;
	virtual int GetNextSelItem(int nItem) const;
	virtual VecDUIListItemCtrl GetSelectItem();
	virtual bool IsSelected(int nIndex);
	virtual bool SelectItem(int nIndex, bool bTakeFocus = false);
	virtual bool SelectItem(CDUIListItemCtrl *pItem, bool bTakeFocus = false);
	virtual bool SelectItem(int nFirst, int nLast);
	virtual bool UnSelectItem(int nIndex, bool bOthers = false);
	virtual void SelectAllItems();
	virtual void UnSelectAllItems();
	virtual int FindSelectable(WORD wKey = VK_UP);

	//child
	virtual CMMString GetItemModel();
	virtual bool SetItemModel(CMMString strModel);
	virtual int GetItemInitCount();
	virtual void SetItemInitCount(int nCount);
	virtual void InitChild();
	virtual bool InsertChild(CDUIControlBase *pChild, int nPos = -1) override;
	virtual bool InsertChild(int nItemModelCount, int nPos = -1);
	virtual int GetChildIndex(CDUIControlBase *pChild) const override;
	virtual int GetHeaderChildIndex(CDUIControlBase *pChild) const;
	bool SwapChild(int nIndexFirst, int nIndexNext, bool bNeedRefreshView = true) override;
	CDUIListItemCtrl * GetChildAt(int nIndex) const override;
	virtual CDUIListHeaderItemCtrl * GetHeaderChildAt(int nIndex) const;
	bool SetChildIndex(CDUIControlBase *pControl, int nIndex) override;
	virtual bool SetHeaderChildIndex(CDUIControlBase *pControl, int nIndex);
	int GetChildCount() const override;
	bool VerifyChild(CDUIControlBase *pControl) override;
	virtual int GetHeaderChildCount() const;
	virtual int GetRowCount();
	virtual int GetColumnCount();
	virtual CDUIRect GetColumnRect(int nIndex) const;
	int GetChildPaddingH() override;
	int GetChildPaddingV() override;
	CDUISize GetChildPadding() override;
	void SetChildPadding(int nChildPaddingH, int nChildPaddingV) override;
	virtual CDUISize GetShowIndex();
	bool Remove(CDUIControlBase *pControl) override;
	bool RemoveAt(int nIndex) override;
	void RemoveAll() override;
	int TranslateIndex(CDUIPoint pt) override;
	void EnsureVisible(int nIndex, bool bCenter = false) override;
	virtual CDUIPoint TranslateRowColumn(int nIndex);

	//item property
	virtual RECT GetItemTextPadding();
	virtual Gdiplus::ARGB GetItemTextColorNormal();
	virtual Gdiplus::ARGB GetItemTextColorHot();
	virtual Gdiplus::ARGB GetItemTextColorSelNormal();
	virtual Gdiplus::ARGB GetItemTextColorSelHot();
	virtual Gdiplus::ARGB GetItemTextColorDisabled();
	virtual tagDuiTextStyle GetItemTextStyleNormal();
	virtual tagDuiTextStyle GetItemTextStyleHot();
	virtual tagDuiTextStyle GetItemTextStyleSelNormal();
	virtual tagDuiTextStyle GetItemTextStyleSelHot();
	virtual tagDuiTextStyle GetItemTextStyleDisabled();
	virtual Gdiplus::ARGB GetItemStatusColorNormal();
	virtual Gdiplus::ARGB GetItemStatusColorHot();
	virtual Gdiplus::ARGB GetItemStatusColorSelNormal();
	virtual Gdiplus::ARGB GetItemStatusColorSelHot();
	virtual Gdiplus::ARGB GetItemStatusColorDisabled();
	virtual vector<CMMString> GetItemStatusColorResSwitchNormal();
	virtual vector<CMMString> GetItemStatusColorResSwitchHot();
	virtual vector<CMMString> GetItemStatusColorResSwitchSelNormal();
	virtual vector<CMMString> GetItemStatusColorResSwitchSelHot();
	virtual vector<CMMString> GetItemStatusColorResSwitchDisabled();
	virtual tagDuiImageSection GetItemStatusImageSectionNormal();
	virtual tagDuiImageSection GetItemStatusImageSectionHot();
	virtual tagDuiImageSection GetItemStatusImageSectionSelNormal();
	virtual tagDuiImageSection GetItemStatusImageSectionSelHot();
	virtual tagDuiImageSection GetItemStatusImageSectionDisabled();

	virtual void SetItemTextPadding(RECT rcPadding);
	virtual void SetItemTextStyleAll(const tagDuiTextStyle &TextStyle);
	virtual void SetItemTextStyleNormal(const tagDuiTextStyle &TextStyle);
	virtual void SetItemTextStyleHot(const tagDuiTextStyle &TextStyle);
	virtual void SetItemTextStyleSelNormal(const tagDuiTextStyle &TextStyle);
	virtual void SetItemTextStyleSelHot(const tagDuiTextStyle &TextStyle);
	virtual void SetItemTextStyleDisabled(const tagDuiTextStyle &TextStyle);
	virtual void SetItemStatusColorResSwitchNormal(const vector<CMMString> &vecResSwitch);
	virtual void SetItemStatusColorResSwitchHot(const vector<CMMString> &vecResSwitch);
	virtual void SetItemStatusColorResSwitchSelNormal(const vector<CMMString> &vecResSwitch);
	virtual void SetItemStatusColorResSwitchSelHot(const vector<CMMString> &vecResSwitch);
	virtual void SetItemStatusColorResSwitchDisabled(const vector<CMMString> &vecResSwitch);
	virtual void SetItemStatusImageSectionNormal(const tagDuiImageSection &ImageSection);
	virtual void SetItemStatusImageSectionHot(const tagDuiImageSection &ImageSection);
	virtual void SetItemStatusImageSectionSelNormal(const tagDuiImageSection &ImageSection);
	virtual void SetItemStatusImageSectionSelHot(const tagDuiImageSection &ImageSection);
	virtual void SetItemStatusImageSectionDisabled(const tagDuiImageSection &ImageSection);

	virtual Gdiplus::ARGB GetLineColor();
	virtual void SetLineColorResSwitch(const vector<CMMString> &vecResSwitch);
	virtual bool IsShowRowLine();
	virtual void SetShowRowLine(bool bShowLine = false);
	virtual bool IsShowColumnLine();
	virtual void SetShowColumnLine(bool bShowLine = false);

	//sort
	virtual bool SortItems(PLVCompareFunc pfnCompare, UINT_PTR dwData);

	//message
protected:
	bool OnDuiLButtonDown(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiLButtonUp(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiLButtonDlk(const CDUIPoint &pt, const DuiMessage &Msg) override;

	bool OnDuiRButtonDown(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiRButtonUp(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiRButtonDlk(const CDUIPoint &pt, const DuiMessage &Msg) override;

	bool OnDuiMouseWheel(const CDUIPoint &pt, const DuiMessage &Msg) override;
	bool OnDuiMouseMove(const CDUIPoint &pt, const DuiMessage &Msg) override;

	LRESULT	OnDuiKeyDown(const DuiMessage &Msg) override;
	LRESULT	OnDuiKeyUp(const DuiMessage &Msg) override;
	LRESULT	OnDuiContextMenu(const DuiMessage &Msg) override;

	//item action
protected:
	void OnItemVisible(CDUIListItemCtrl *pItem);
	void OnItemFixedWidth(int nWidthOld, int nWidthNew);
	void OnItemFixedHeight(int nHeightOld, int nHeightNew);

	//property
protected:
	void InitProperty() override;
	void InitComplete() override;
	bool SaveAttribute(tinyxml2::XMLElement *pNode, bool bIncludeChild = true) override;

	//paint
protected:
	virtual void PaintMouseDragSel(HDC hDC);

	//help
protected:
	CDUIRect GetLayoutRangeOfScrollBar() override;
	CDUIRect GetLayoutRangeOfItem() override;
	void ScrollChilds(CDUISize szScroll);

	int GetMinSelItemIndex();
	int GetMaxSelItemIndex();

	virtual CDUIListItemCtrl * ProductItemDefault();
	void SetInternVisible(bool bVisible = true, bool bTraversal = false) override;
	CDUIControlBase * FindControl(FindControlProc Proc, LPVOID pData, UINT uFlags) override;
	void ReapControl() override;
	void PerformMouseDragSel();

	int __cdecl ItemComareFunc(const void *pItemFirst, const void *pItemNext);

	//list help
protected:
	void InsertChildOnListType(CDUIListItemCtrl *pItem);
	void RefreshViewOnListType();
	void CalcShowItemOnListType();
	void ScrollChildsUpOnListType(CDUISize szScroll);
	void ScrollChildsDownOnListType(CDUISize szScroll);
	int FindSelectableOnListType(int nIndexCur, bool bPositive = true) const;

	//tile help
protected:
	void InsertChildOnTileTypeH(CDUIListItemCtrl *pItem);
	void InsertChildOnTileTypeV(CDUIListItemCtrl *pItem);
	void RefreshViewOnTileTypeH();
	void RefreshViewOnTileTypeV();
	void CalcShowItemOnTileTypeH();
	void CalcShowItemOnTileTypeV();
	void CalcVisibleCount();
	void CalcTileTypeInfo();
	void ScrollChildsUpOnTileTypeH(CDUISize szScroll);
	void ScrollChildsDownOnTileTypeH(CDUISize szScroll);
	void ScrollChildsUpOnTileTypeV(CDUISize szScroll);
	void ScrollChildsDownOnTileTypeV(CDUISize szScroll);
	int FindSelectableOnTileTypeH(WORD wKey);
	int FindSelectableOnTileTypeV(WORD wKey);

	//static
protected:
	static int __cdecl ItemComareFunc(void *pVlocale, const void *pItemFirst, const void *pItemNext);
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif