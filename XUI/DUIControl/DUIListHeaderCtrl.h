#ifndef __DUILISTVIEWHEADERCTRL_H__
#define __DUILISTVIEWHEADERCTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIListHeaderCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUIListHeaderCtrl = { 0x99FCAA5B,0xAD12,0x4A09,0xB6,0xF9,0x7B,0x95,0x10,0x45,0x35,0x4F };
class XUI_API CDUIListHeaderCtrl
	: public CDUIHorizontalLayoutCtrl
	, public IDuiControlCallBack
{
	friend class CDUIListViewCtrl;

	MMDeclare_Super(CDUIHorizontalLayoutCtrl)
	DuiDeclare_CreateControl(CDUIListHeaderCtrl)
	MMDeclare_ClassName(CDUIListHeaderCtrl)

public:
	CDUIListHeaderCtrl();
	~CDUIListHeaderCtrl(void);

	//attribute
protected:
	CDUIAttributeGroup					m_AttributeGroupHeader;
	CDUIAttributeBool					m_AttributeScaleItem;
	CDUIAttributeLong					m_AttributeItemInitCount;

	//variant
protected:
	CDUIListViewCtrl *					m_pOwner = NULL;
	CDUIListHeaderItemCtrl *			m_pHeaderItemSelectCtrl = NULL;
	CDUICheckBoxCtrl *					m_pCheckSelectCtrl = NULL;
	int									m_nColumn = 0;
	CDUIRect							m_rcColumn[Dui_Max_ListColumn];

	//override
protected:
	bool OnAttributeChange(CDUIAttributeObject *pAttributeObj) override;

	//override
protected:
	void OnNotify(CDUIControlBase *pControl, const DuiNotify &Notify) override;
	void OnSize(CDUIControlBase *pControl) override;
	void OnVisibleChanged(CDUIControlBase *pControl) override;

	//diable method, because first child is checkicon, second child is edit.
	//traverse user custom child use GetChildCountUser()¡¢GetChildAtUser()¡¢RemoveAtUser().
protected:
	int GetChildCount() const override;
	bool InsertChild(CDUIControlBase *pChild, int nPos = -1) override;
	CDUIListHeaderItemCtrl * GetChildAt(int nIndex) const override;
	bool RemoveAt(int nIndex) override;

	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;

	//listview
	virtual CDUIListViewCtrl * GetOwner();
	virtual void SetOwner(CDUIListViewCtrl *pOwner);

	//scale
	virtual void SetScaleHeader(bool bScale);
	virtual bool IsScaleHeader();
	virtual float GetScaleValue(int nWidth);

	//column
	virtual int GetColumnCount();
	virtual CDUIRect GetColumnRect(int nIndex);

	//child
	virtual int GetItemInitCount();
	virtual void SetIteminitCount(int nCount);
	virtual void InitChild();
	virtual bool InsertChild(int nItemCount, int nPos = -1);
	virtual bool InsertChildAtUser(CDUIControlBase *pChild, int nPos = -1);
	virtual int GetChildCountAtUser();
	virtual CDUIListHeaderItemCtrl * GetChildAtUser(int nIndex);
	virtual CDUIListHeaderItemCtrl * GetHeaderItemSelect();
	virtual bool RemoveAtUser(int nIndex);
	bool Remove(CDUIControlBase *pControl) override;
	void RemoveAll() override;

	//refresh
	void RefreshView() override;

	//message
protected:

	//listview change
protected:
	void OnVisibleChangedSelectIcon();
	void OnSelectChangedListItem();

	//property
protected:
	void InitProperty() override;
	void InitComplete() override;

	//help
protected:
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif