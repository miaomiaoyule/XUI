#pragma once

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIWndBase INTERFACE_VERSION(1,1)
static const GUID IID_CDUIWndBase = { 0xaa660f7a, 0xd3b, 0x43c9, 0xb2, 0x8d, 0xb1, 0xe8, 0xbb, 0x83, 0x4, 0x94 };
class XUI_API CDUIWndBase
	: public CDUIPropertyObject
	, public CDUIAnimationWnd
	, public CDUINotifyPump
	, public IDuiPreMessage
	, public IMMWndInterface
{
	friend class CDUIContainerCtrl;
	friend class CDUIXEditCtrl;
	friend class CDUIRichEditCtrl;

	MMDeclare_Super(CDUIPropertyObject)
	MMDeclare_ClassName(CDUIWndBase)
	DuiDeclare_Message_Map()

public:
	using MouseEventPtr = bool (CDUIControlBase::*)(const CDUIPoint&, const DuiMessage&);
	using KeyboardEventPtr = LRESULT (CDUIControlBase::*)(const DuiMessage&);
	using ModelEventPtr = LRESULT (CDUIControlBase::*)(const DuiMessage&);

public:
	CDUIWndBase(LPCTSTR lpszDuiName = NULL, HWND hWndParent = NULL);
	virtual ~CDUIWndBase();

	//attribute
protected:
	CDUIAttributeGroup							m_AttributeGroupWndInfo;
	CDUIAttributeLong							m_AttributeWndCaptionHeight = 30;
	CDUIAttributeSize							m_AttributeWndInitSize = SIZE{ 500, 500 };
	CDUIAttributeSize							m_AttributeWndMinSize;
	CDUIAttributeSize							m_AttributeWndMaxSize;
	CDUIAttributeRect							m_AttributeWndResizeTrack;

	CDUIAttributeGroup							m_AttributeGroupLayered;
	CDUIAttributeBool							m_AttributeWndLayered;
	CDUIAttributeLong							m_AttributeWndAlpha = 255;

	CDUIAttributeGroup							m_AttributeGroupRenderText;
	CDUIAttributeBool							m_AttributeGdiplusRenderText;
	CDUIAttributeCombox							m_AttributeGdiplusRenderType;

	CDUIAttributeGroup							m_AttributeGroupRenderImage;
	CDUIAttributeBool							m_AttributeGdiplusRenderImage;

	CDUIAttributeGroup							m_AttributeGroupAnimation;
	CDUIAttributeCombox							m_AttributeAnimationType;
	CDUIAttributeLong							m_AttributeAnimationFrame = ANIMATE_FRAME;
	CDUIAttributeLong							m_AttributeAnimationElapse = Dui_TimerAnimate_Elapse;

	CDUIAttributeGroup							m_AttributeGroupCustom;
	CDUIAttributeLong							m_AttributeUserTag;

	//variant
protected:
	CMMString									m_strDuiName;
	HWND										m_hWndParent = NULL;
	HWND										m_hWnd = NULL;
	uint32_t									m_uWndID = 0;
	bool										m_bSubWindow = false;
	CDUIPoint									m_ptCreate = {};
	UINT										m_uCtrlIDClose = 0;
	CMMDpi										m_DpiInfo;

	//paint
	HDC											m_hDCPaint = NULL;
	HDC											m_hMemDcBackground = NULL;
	HBITMAP										m_hBmpBackgroundOld = NULL;
	HBITMAP										m_hBmpBackground = NULL;
	LPBYTE										m_pBmpBackgroundBits = NULL;

	//tip
	HWND										m_hWndTooltip = NULL;
	TOOLINFO									m_ToolTip = {};
	int											m_nToolTipHoverTime = 400UL;
	CMMString									m_strToolTip;

	//info
	UINT										m_uTimerID = 0x1000;
	POINT										m_ptMousePosPre;
	POINT										m_ptMousePosLast;
	POINT										m_ptMousePosDown;
	DWORD										m_dwMouseDownTick = 0;
	bool										m_bMouseTracking = false;
	bool										m_bRefreshToolTipNeeded = false;
	bool										m_bRefreshViewNeeded = false;
	bool										m_bPostedAppMsg = false;
	bool										m_bFirstLayout = true;

	//designer
	bool										m_bDesigned = false;

	//caret
	CDUIRect									m_rcCaret;
	bool										m_bCaretActive = false;

	//ctrl
	CDUIContainerCtrl *							m_pRootCtrl = NULL;
	CDUIControlBase *							m_pCaptureCtrl = NULL;
	CDUIControlBase *							m_pFocusCtrl = NULL;
	CDUIControlBase *							m_pHoverCtrl = NULL;
	CDUIControlBase *							m_pEventCtrl = NULL;
	CDUIControlBase *							m_pWinDragCtrl = NULL;
	CDUIControlBase *							m_pWinDragEnterCtrl = NULL;
	VecDuiControlBase							m_vecMouseEnterCtrl;

	//drop
	tagDuiDropData								m_DropData;

	VecIDuiNotify								m_vecINotify;
	VecDuiNotify								m_vecAsynNotify;
	VecDuiTimerInfo								m_vecTimers;
	DuiTimerStore								m_TimerStore;
	DequeDuiControlBase							m_queDelayDelete;
	VecDuiControlBase							m_vecFoundControls;
	MapDuiControlBase							m_mapControl;
	MapDuiRadioBoxGroup							m_mapRadioBoxGroup;

	//override
protected:
	bool RegisterControlListen(IDuiControlListen *pIDuiControlListen) override;
	bool UnRegisterControlListen(IDuiControlListen *pIDuiControlListen) override;
	bool OnAttributeChange(CDUIAttributeObject *pAttributeObj) override;

	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;
	bool IsDesigned();
	void SetDesigned(bool bDesigned);
	virtual CMMString GetDuiName() const;
	virtual UINT GetClassStyle() const;
	virtual UINT MapWndKeyState();

	//create
	virtual LPCTSTR GetSuperClassName() const;
	virtual HWND Create(HWND hWndParent, LPCTSTR lpszName, DWORD dwStyle, DWORD dwExStyle, const RECT rc);
	virtual HWND Create(HWND hWndParent, LPCTSTR lpszName, DWORD dwStyle, DWORD dwExStyle, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int cx = CW_USEDEFAULT, int cy = CW_USEDEFAULT);
	virtual HWND SubWindow(HWND hWnd);
	virtual void UnSubWindow();
	virtual void ShowWindow(bool bShow = true, bool bTakeFocus = true);
	virtual void ShowWindow(int nCmdShow) override;
	virtual UINT DoModal();
	virtual UINT DoBlock();
	virtual void Close(UINT nRet = IDOK);
	virtual void CenterWindow();
	virtual void SetIcon(UINT nRes);
	virtual LRESULT SendMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
	virtual LRESULT PostMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);

	//capture
	virtual void SetCapture();
	virtual void ReleaseCapture();
	virtual bool IsCaptured();
	virtual CDUIControlBase * GetCaptureControl();

	//focus
	virtual CDUIControlBase * GetFocusControl();
	virtual void SetFocusControl(CDUIControlBase *pFocusCtrl);

	//hover
	virtual CDUIControlBase * GetHoverCtrl();

	//root
	virtual bool AttachRootCtrl(CDUIContainerCtrl *pControl);
	virtual bool ReleaseRootCtrl();
	virtual CDUIContainerCtrl * GetRootCtrl() override;
	virtual CDUIContainerCtrl * DetachRootCtrl();

	//manager ctrl
	virtual bool InitControls(CDUIControlBase *pControl);
	virtual void InitControlIDHash(CDUIControlBase *pControl);
	virtual void UnInitControlIDHash(CDUIControlBase *pControl);
	virtual void ReapControl(CDUIControlBase *pControl);
	virtual bool VerifyControl(CDUIControlBase *pControl);

	//tag
	virtual UINT_PTR GetTag();
	virtual void SetTag(UINT_PTR pTag);

	//timer
	virtual bool SetTimer(CDUIPropertyObject *pPropObj, UINT uTimerID, UINT uElapse);
	virtual bool FindTimer(CDUIPropertyObject *pPropObj, UINT uTimerID);
	virtual bool KillTimer(CDUIPropertyObject *pPropObj, UINT uTimerID);
	virtual bool KillTimer(CDUIPropertyObject *pPropObj);
	virtual bool RemoveAllTimer();
	virtual bool AsyncTask(std::function<void()> pFunc);
	virtual UINT_PTR TimerTask(unsigned int ms, bool bRepeat, std::function<void()> pFunc);
	virtual bool StopTimer(UINT_PTR timerId);

	//radio
	virtual bool AddRadioBoxToGroup(CDUIRadioBoxCtrl *pControl);
	virtual VecDuiRadioBoxCtrl GetRadioBoxGroup(LPCTSTR lpszGroup);
	virtual bool RemoveRadioBoxFromGroup(CDUIRadioBoxCtrl *pControl);
	virtual void RemoveAllRadioBoxGroup();
	virtual CDUIRadioBoxCtrl * GetRadioBoxOfBindTabCtrl(CDUITabLayoutCtrl *pTabCtrl, long lBindIndex);

	//tips
	virtual HWND GetToolTipWindow() const;
	virtual int GetToolTipHoverTime() const;
	virtual void SetToolTipHoverTime(int nTime);
	virtual void RefreshToolTip(CMMString strToolTip);

	//refresh
	virtual void RefreshLayout();
	virtual void Invalidate() override;
	virtual bool IsRefreshViewNeeded() const;
	virtual void NeedRefreshView();
	virtual POINT GetMousePosLast() const;
	virtual POINT GetMousePosDown() const;

	//window
	virtual operator HWND() const;
	virtual HWND GetWndHandle();
	virtual HWND GetParent() override;
	virtual UINT GetWndID() override;
	virtual HDC GetWndDC() override;
	virtual void ResizeWnd(int cx = -1, int cy = -1);
	virtual void AdjustWndPos();
	virtual bool IsMaximized();
	virtual bool IsMinimized();
	virtual bool IsVirtualWnd() override;
	virtual bool IsWindowVisible() override;
	virtual CMMRect GetWindowRect() override;
	virtual CMMRect GetClientRect() override;
	virtual SIZE GetWndInitSize();
	virtual void SetWndInitSize(int cx, int cy);
	virtual SIZE GetWndMinSize();
	virtual void SetWndMinSize(int cx, int cy);
	virtual SIZE GetWndMaxSize();
	virtual void SetWndMaxSize(int cx, int cy);
	virtual RECT GetResizeTrack();
	virtual void SetResizeTrack(RECT &rcResizeTrack);
	virtual int GetCaptionHeight();
	virtual void SetCaptionHeight(int nHeight);
	virtual const CMMDpi & GetDpiObj() override;
	virtual int GetDpi();
	virtual bool SetDpi(int nDpi);
	virtual int GetScale() override;
	virtual bool SetScale(int nScale);
	virtual HBITMAP GetBackgroundBmp();
	virtual LPBYTE GetBackgroundBits();
	virtual void SetWindowPos(HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags) override;

	//caret
	virtual bool CreateCaret(HBITMAP hBmp, int nWidth, int nHeight);
	virtual bool IsShowCaret();
	virtual void ShowCaret(bool bShow);
	virtual void SetCaretPos(CDUIPoint pt);

	//wnd alpha
	virtual BYTE GetWndAlpha() override;
	virtual void SetWndAlpha(BYTE cbWndAlpha);
	virtual bool IsWndLayered() override;
	virtual void SetWndLayered(bool bLayered) override;

	//gdiplus text
	virtual bool IsGdiplusRenderText();
	virtual void SetGdiplusRenderText(bool bGdiplus);
	virtual Gdiplus::TextRenderingHint GetGdiplusRenderTextType();
	virtual void SetGdiplusRenderTextType(Gdiplus::TextRenderingHint RenderType);

	//gdiplus image
	virtual bool IsGdiplusRenderImage();
	virtual void SetGdiplusRenderImage(bool bGdiplus);

	//animation
	virtual enDuiAnimateWndType GetAnimateWndType() override;
	virtual void SetAnimateWndType(enDuiAnimateWndType AnimateWndType);
	virtual int GetAnimateWndFrame() override;
	virtual void SetAnimateWndFrame(int nFrame);
	virtual int GetAnimateWndElapse() override;
	virtual void SetAnimateWndElapse(int nElapse);

	//notify
	virtual bool AddINotify(IDuiNotify *pINotify);
	virtual bool RemoveINotify(IDuiNotify *pINotify);
	virtual void SendNotify(DuiNotify &Notify);
	virtual void SendNotify(CDUIControlBase *pControl, enDuiNotifyType NotifyType, WPARAM wParam = 0, LPARAM lParam = 0);
	virtual void PostNotify(DuiNotify &Notify);
	virtual void PostNotify(CDUIControlBase *pControl, enDuiNotifyType NotifyType, WPARAM wParam = 0, LPARAM lParam = 0);

	//find ctrl
	virtual CDUIControlBase * FindControl(POINT pt) const;
	virtual CDUIControlBase * FindControl(UINT uCtrlID) const;
	virtual CDUIControlBase * FindControlByShortCut(TCHAR chChar) const;
	virtual CDUIControlBase * FindControlByDrop(POINT pt) const;
	virtual CDUIControlBase * FindSubControlByPoint(CDUIContainerCtrl *pParent, POINT pt);
	virtual CDUIControlBase * FindSubControlByID(CDUIContainerCtrl *pParent, UINT uCtrlID);
	virtual VecDuiControlBase FindSubControlsByClass(CDUIContainerCtrl *pParent, LPCTSTR pstrClass);

	//dragdrop
	virtual void Register(HWND hWnd) {}

	//override
protected:
	bool SetTimer(UINT uTimerID, UINT nElapse) override;
	bool KillTimer(UINT uTimerID) override;
	bool KillTimer() override;

	//property	   
protected:
	void InitProperty() override;
	void InitComplete() override;
	bool SaveAttribute(tinyxml2::XMLElement *pNode, bool bIncludeChild = true) override;

	//message
protected:
	virtual LRESULT OnPreWndMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled) override;
	virtual LRESULT OnWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnOldWndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnCreate(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnClose(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnDestroy(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnNcActivate(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnNcCalcSize(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnNcPaint(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnNcHitTest(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnGetMinMaxInfo(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnExitSizeMove(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnSysCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnLButtonDlk(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnRButtonDown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnRButtonUp(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnRButtonDlk(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnMouseMove(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnMouseWheel(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnTimer(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnMoving(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnMove(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnSizing(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnSize(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnWindowPosChanging(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnWindowPosChanged(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnKeyDown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnKeyUp(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnChar(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnSysKeyDown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnSysKeyUp(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnSysChar(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnPaint(CDUIRect rcPaint);
	virtual LRESULT OnContextMenu(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnSetFocus(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnKillFocus(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnSetCursor(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnDpiChanged(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnImeComPosition(WPARAM wParam, LPARAM lParam);

	//notify
protected:
	virtual void OnFindControl() {}
	virtual void OnInitDialog() {}
	virtual void OnFinalMessage();
	virtual void OnDuiTimer(const DuiNotify &Notify);
	virtual void OnDuiClick(const DuiNotify &Notify);
	virtual void OnDuiWndAnimateFinish(const DuiNotify &Notify);

	//help
protected:
	virtual void UpdateImeCompositionPos() {}
	virtual void OnDuiDelayDelete();
	virtual void OnDpiChanged(int nScalePre);
	virtual void AdjustWndSize();
	virtual void DelayDelete(CDUIControlBase *pControl);
	virtual void DispatchNotifyAsyn();
	virtual void PostAppMsg();
	virtual void EnsurePaintScene();
	virtual void ReleasePaintScene();
	virtual void DispatchModelMouseEvent(CDUIControlBase *pCtrl, MouseEventPtr pFunc, const CDUIPoint &pt, const DuiMessage &Msg, bool bClick);
	virtual void DispatchModelKeyboardEvent(CDUIControlBase *pCtrl, KeyboardEventPtr pFunc, const DuiMessage &Msg);
	virtual void DispatchModelEvent(CDUIControlBase *pCtrl, ModelEventPtr pFunc, const DuiMessage &Msg);

	//help
private:
	VecDuiControlBase & GetFoundControls();
	static CDUIControlBase * CALLBACK __FindControlFromIDHash(CDUIControlBase *pThis, LPVOID pData);
	static CDUIControlBase * CALLBACK __FindControlFromCount(CDUIControlBase *pThis, LPVOID pData);
	static CDUIControlBase * CALLBACK __FindControlFromPoint(CDUIControlBase *pThis, LPVOID pData);
	static CDUIControlBase * CALLBACK __FindControlFromShortcut(CDUIControlBase *pThis, LPVOID pData);
	static CDUIControlBase * CALLBACK __FindControlFromDrop(CDUIControlBase *pThis, LPVOID pData);
	static CDUIControlBase * CALLBACK __FindControlFromID(CDUIControlBase *pThis, LPVOID pData);
	static CDUIControlBase * CALLBACK __FindControlsFromClass(CDUIControlBase *pThis, LPVOID pData);
	static CDUIControlBase * CALLBACK __FindControlsFromUpdate(CDUIControlBase *pThis, LPVOID pData);
};

