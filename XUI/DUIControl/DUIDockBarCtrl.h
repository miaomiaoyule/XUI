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
#define DockBar_WaveDelay_Default				(50)
#define DockBar_WaveDuration_Default			(320)
#define DockBar_Scale_Epsilon					(0.002f)

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
	CDUIAttributeLong							m_AttributeEffectRange;						//mouse move effect range, 0 == auto
	CDUIAttributeLong							m_AttributeWaveDelay = DockBar_WaveDelay_Default;		//wave delay of neighbor item, ms
	CDUIAttributeLong							m_AttributeWaveDuration = DockBar_WaveDuration_Default;	//wave duration of single item, ms

	//variant
protected:
	std::vector<float>							m_vecItemScale;								//current scale of every item
	std::vector<tagDuiDockItemSet>				m_vecItemSet;								//value set to item last time
	std::vector<CDUISize>						m_vecItemBaseSize;							//base size(pixel) of every item, captured at first layout
	int											m_nEffectItem = -1;							//item which trigger the select wave
	DWORD										m_dwEffectTick = 0;							//select wave start tick
	int											m_nMousePosX = 0;							//mouse x in dock item range
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

	//basic
	void RefreshView() override;

	//layout of list view
	CDUISize GetSwitchTileItemSize() override;
	bool IsAutoCalcChildPaddingH() override;
	bool IsAutoCalcChildPaddingV() override;
	bool IsAutoCalcItemWidth() override;
	bool IsAutoCalcItemHeight() override;

	//child
	bool InsertChild(CDUIControlBase *pChild, int nPos = -1) override;
	bool Remove(CDUIControlBase *pControl) override;
	bool RemoveAt(int nIndex) override;
	void RemoveAll() override;

	//dock bar
	virtual enDuiDockBarEffectType GetEffectType();
	virtual void SetEffectType(enDuiDockBarEffectType EffectType);

	//icon
	virtual bool IsAlignBottom();
	virtual void SetAlignBottom(bool bBottom);

	//effect
	virtual float GetScaleMax();
	virtual void SetScaleMax(float fScaleMax);
	virtual int GetEffectRange();
	virtual void SetEffectRange(int nRange);
	virtual int GetWaveDelay();
	virtual void SetWaveDelay(int nDelay);
	virtual int GetWaveDuration();
	virtual void SetWaveDuration(int nDuration);

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

	void StartEffect(int nIndex);
	void StartEffectTimer();
	void StopEffect();
	void OnEffectElapse();
	bool UpdateSelectEffect(DWORD dwTick);
	bool UpdateMouseMoveEffect();
	void OnDockAttributeChanged(bool bResetScale);
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
