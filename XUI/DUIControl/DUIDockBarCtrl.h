#ifndef __DUIDOCKBARCTRL_H__
#define __DUIDOCKBARCTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//dock bar default
#define DockBar_ScaleMax_Default				(1.6f)
#define DockBar_EffectRange_Icon_Default		(1)				//icons affected each side of mouse
#define DockBar_ReflectionRatio_Default			(0.45f)			//reflection height / icon height, same as GenerateReflectionBitmap

//item value which is set last time, keep it to avoid refresh loop
struct tagDuiDockBarItem
{
	CDUIListItemCtrl *							pItem = NULL;
	CDUISize									szEffect;
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
	CDUIAttributeFloat							m_AttributeScaleMax = DockBar_ScaleMax_Default;
	CDUIAttributeLong							m_AttributeEffectRange = DockBar_EffectRange_Icon_Default;
	CDUIAttributeFloat							m_AttributeReflectionRatio = DockBar_ReflectionRatio_Default; 

	//variant
protected:
	std::vector<CDUISize>						m_vecItemBaseSize;
	CDUIPoint									m_ptMousePos;

	//override
protected:
	bool OnAttributeChange(CDUIAttributeObject *pAttributeObj) override;
	void OnNotify(CDUIControlBase *pControl, const DuiNotify &Notify) override;
	void OnRelease(CDUIControlBase *pControl) override;
	
	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;

	//refresh
	CDUIRect GetBorderRect() override;

	//tile type
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
	virtual float GetScaleMax();
	virtual void SetScaleMax(float fScaleMax);
	virtual int GetEffectRange();
	virtual void SetEffectRange(int nRange);
	virtual float GetReflectionRatio();
	virtual void SetReflectionRatio(float fRatio);

	//message
protected:
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

	//help
protected:
	void StartEffect();
	void StopEffect();
	void PerformMouseMoveEffect();
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
