#ifndef __DUIDOCKBARCTRL_H__
#define __DUIDOCKBARCTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//dock bar effect timer
#define DockBar_Effect_Elapse					(15)

//dock bar default
#define DockBar_IconSize_Default				(48)
#define DockBar_ScaleMax_Default				(1.6f)
#define DockBar_Scale_Epsilon					(0.002f)
#define DockBar_EffectRange_Icon_Default		(2)				//icons affected each side of mouse
#define DockBar_ReflectionRatio_Default			(0.45f)			//reflection height / icon height, same as GenerateReflectionBitmap

//item value which is set last time, keep it to avoid refresh loop
struct tagDuiDockItemSet
{
	int											nWidth = -1;
	int											nHeight = -1;
	int											nPaddingTop = -1;
};

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIDockBarCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUIDockBarCtrl = { 0x59C6EC41,0xE19A,0x498D,0x81,0x0D,0x44,0xF0,0xE6,0x72,0x7C,0x77 };
class XUI_API CDUIDockBarCtrl
	: public CDUIListViewCtrl
	, public IDuiControlCallBack
{
	MMDeclare_Super(CDUIListViewCtrl)
	DuiDeclare_CreateControl(CDUIDockBarCtrl)
	MMDeclare_ClassName(CDUIDockBarCtrl)

public:
	CDUIDockBarCtrl(void);
	~CDUIDockBarCtrl(void);

	//attribute
protected:
	CDUIAttributeGroup							m_AttributeGroupDockBar;
	CDUIAttributeCombox							m_AttributeEffectType;
	CDUIAttributeBool							m_AttributeAlignBottom = true;
	CDUIAttributeFloat							m_AttributeScaleMax = DockBar_ScaleMax_Default; //max scale of icon
	CDUIAttributeLong							m_AttributeEffectRange;							//icon count affected each side of mouse, 0 == auto
	CDUIAttributeFloat							m_AttributeReflectionRatio = DockBar_ReflectionRatio_Default; //reflection height / icon height of the item background image(icon + reflection)
	CDUIAttriImageSection						m_AttributeDockImageBk;							//background of the base icon area, the magnified part and the reflection are not included
	
	//variant
protected:
	std::vector<float>							m_vecItemScale;								//current scale of every item
	std::vector<tagDuiDockItemSet>				m_vecItemSet;								//value set to item last time
	std::vector<CDUISize>						m_vecItemBaseSize;							//base size(pixel) of every item, captured at first layout
	int											m_nMousePosX = 0;							//mouse x in dock item range
	int											m_nMouseRefPosX = 0;						//item range left captured while the effect is at rest
	bool										m_bMouseInDock = false;
	UINT_PTR									m_uEffectTimer = 0;							//effect timer of window

	//override
protected:
	bool OnAttributeChange(CDUIAttributeObject *pAttributeObj) override;
	void OnDpiChanged(int nScalePre) override;
	void OnNotify(CDUIControlBase *pControl, const DuiNotify &Notify) override;
	void OnRelease(CDUIControlBase *pControl) override;
	
	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;

	//refresh
	void RefreshView() override;

	//tile type
	CDUISize GetSwitchTileItemSize() override;
	bool IsAutoCalcItemWidth() override;
	bool IsAutoCalcItemHeight() override;

	//animation
	void OnDragMoveBegin(CDUIControlBase *pDragCtrl) override;

	//child
	bool InsertChild(CDUIControlBase *pChild, int nPos = -1) override;
	bool IsAutoCalcChildPaddingH() override;
	bool IsAutoCalcChildPaddingV() override;
	bool Remove(CDUIControlBase *pControl) override;
	bool RemoveAt(int nIndex) override;
	void RemoveAll() override;

	//dock bar
	virtual enDuiDockBarEffectType GetEffectType();
	virtual void SetEffectType(enDuiDockBarEffectType EffectType);
	virtual tagDuiImageSection GetDockImageBk();
	virtual void SetDockImageBk(const tagDuiImageSection &ImageSection);
	virtual CDUIRect GetDockImageBkRect();
	virtual float GetReflectionRatio();
	virtual void SetReflectionRatio(float fRatio);

	//icon
	virtual bool IsAlignBottom();
	virtual void SetAlignBottom(bool bBottom);

	//effect
	virtual float GetScaleMax();
	virtual void SetScaleMax(float fScaleMax);
	virtual int GetEffectRange();
	virtual void SetEffectRange(int nRange);

	//item
	virtual float GetItemScale(int nIndex);

	//message
protected:
	bool OnDuiMouseMove(const CDUIPoint &pt, const DuiMessage &Msg) override;
	void OnDuiMouseLeave(const CDUIPoint &pt, const DuiMessage &Msg) override;
	void OnDuiWndManagerDetach() override;

	//property
protected:
	void InitProperty() override;
	void InitComplete() override;

	//paint
protected:
	void PaintBkImage(HDC hDC) override;

	//notify
protected:
	void OnItemMouseMove(CDUIControlBase *pControl, const DuiNotify &Notify);
	void OnItemMouseEnter(CDUIControlBase *pControl, const DuiNotify &Notify);
	void OnItemMouseLeave(CDUIControlBase *pControl, const DuiNotify &Notify);

	//help
protected:
	void SyncItemScale();
	void ResetItemScale();
	void ResetItemSet();

	void ApplyItemLayout();
	SIZE GetBaseItemSize(int nIndex);
	int GetBaseItemCenterX(int nIndex);
	int GetEffectRangeValue();
	void UpdateMousePosX(int nAbsoluteX);

	void StartEffectTimer();
	void StopEffect();
	void OnEffectElapse();
	bool UpdateMouseMoveEffect();
	void OnDockAttributeChanged(bool bResetScale);
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
