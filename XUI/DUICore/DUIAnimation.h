#ifndef __DUIANIMATION_H__
#define __DUIANIMATION_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
struct tagAnimationData
{
public:
	tagAnimationData(int nAnimateID, int nTotalFrame, int nElapse, bool bLoop)
	{
		this->nAnimationID = nAnimateID;
		this->nTotalFrame = nTotalFrame;
		this->nElapse = nElapse;
		this->bLoop = bLoop;
	}

	int									nAnimationID = 0;
	int									nElapse = 0;

	int									nTotalFrame = 0;
	int									nCurFrame = 0;

	bool								bLoop;
	bool								bFirstLoop = TRUE;
};

typedef std::vector<tagAnimationData> VecAnimationData;

//////////////////////////////////////////////////////////////////////////
class IDUIAnimation
{
public:
	virtual ~IDUIAnimation() {}

	//timer
	virtual bool SetTimer(UINT uTimerID, UINT nElapse) = 0;
	virtual bool KillTimer(UINT uTimerID) = 0;
	virtual bool KillTimer() = 0;

	virtual bool StartAnimation(int nAnimationID = Dui_TimerAnimate_ID, int nElapse = Dui_TimerAnimate_Elapse, int nTotalFrame = ANIMATE_FRAME, bool bLoop = FALSE) = 0;
	virtual bool OnAnimationElapse(int nAnimationID) = 0;
	virtual void StopAnimation(int nAnimationID = Dui_TimerAnimate_ID) = 0;
	virtual bool IsAnimationRunning(int nAnimationID) = 0;
	virtual int GetCurrentFrame(int nAnimationID) = 0;
	virtual bool SetCurrentFrame(int nAnimationID, int nFrame) = 0;

	virtual void OnAnimationStart(int nAnimationID, bool bFirstLoop) = 0;
	virtual void OnAnimationStep(int nAnimationID, int nTotalFrame, int nCurFrame) = 0;
	virtual void OnAnimationStop(int nAnimationID) = 0;

	enum
	{
		ANIMATE_FRAME = 15
	};
};

//////////////////////////////////////////////////////////////////////////
class XUI_API CDUIAnimation : public IDUIAnimation
{
public:
	CDUIAnimation();
	virtual ~CDUIAnimation();

	void OnDragMoveBegin(CDUIControlBase *pDragCtrl) {};
	void OnDragMoving(CDUIControlBase *pDragCtrl) {};
	void OnDragMoveEnd(CDUIControlBase *pDragCtrl) {};

	bool StartAnimation(int nAnimationID = Dui_TimerAnimate_ID, int nElapse = Dui_TimerAnimate_Elapse, int nTotalFrame = ANIMATE_FRAME, bool bLoop = FALSE);
	bool OnAnimationElapse(int nAnimationID);
	void StopAnimation(int nAnimationID = Dui_TimerAnimate_ID);
	bool IsAnimationRunning(int nAnimationID);
	int GetCurrentFrame(int nAnimationID);
	bool SetCurrentFrame(int nAnimationID, int nFrame);

	void OnAnimationStart(int nAnimationID, bool bFirstLoop) {}
	void OnAnimationStep(int nAnimationID, int nTotalFrame, int nCurFrame) {}
	void OnAnimationStop(int nAnimationID) {}

protected:
	tagAnimationData * GetAnimationDataByID(int nAnimationID);
	bool RemoveAnimationData(int nAnimationID);

protected:
	VecAnimationData 					m_vecAnimationData;
};

//////////////////////////////////////////////////////////////////////////
struct tagDragMove
{
	CDUIControlBase *					pControl = NULL;
	CDUIRect							rcDest;
};

struct tagDuiDragCtrl
{
	tagDuiDragCtrl(CDUIControlBase *pControl, int nIndex)
		: pControl(pControl)
		, nIndex(nIndex)
	{

	}

	CDUIControlBase *					pControl = NULL;
	int									nIndex = -1;
};

class XUI_API CDUIAnimationDrag
	: public CDUIAnimation
	, public CDUINotifyPump
{
	friend class CDUIHorizontalLayoutCtrl;
	friend class CDUIVerticalLayoutCtrl;

	MMDeclare_Super(CDUIAnimation)
	DuiDeclare_Message_Map()

public:
	CDUIAnimationDrag(CDUIContainerCtrl *pBindContainerCtrl, enDuiAnimateDragType DragStyle = DragType_H);
	virtual ~CDUIAnimationDrag();

	//成员变量
protected:
	CDUIContainerCtrl *					m_pBindContainerCtrl = NULL;
	enDuiAnimateDragType				m_DragType = DragType_H;
	CDUIControlBase *					m_pDragingCtrl = NULL;
	vector<tagDuiDragCtrl>				m_vecDragCtrl;
	vector<CDUIRect>					m_vecDragPre;
	vector<tagDragMove>					m_vecPosDest;
	POINT								m_ptMouseLast = {};

	//override
protected:
	bool SetTimer(UINT uTimerID, UINT nElapse);
	bool KillTimer(UINT uTimerID);
	bool KillTimer();

	//method
public:
	enDuiAnimateDragType GetDragType();
	void SetDragType(enDuiAnimateDragType DragType);
	bool IsAnimateDraging();
	CDUIControlBase * GetDragingCtrl();

	//animate
protected:
	void OnDragMoveBegin(CDUIControlBase *pDragCtrl);
	void OnDragMoving(CDUIControlBase *pDragCtrl);
	void OnDragMoveEnd(CDUIControlBase *pDragCtrl);

	void OnAnimationStep(int nAnimationID, int nTotalFrame, int nCurFrame);

	//message
protected:
	void OnDuiWinDragEnter(const DuiNotify &Notify);
	void OnDuiWinDragLeave(const DuiNotify &Notify);
	void OnDuiWinDragOver(const DuiNotify &Notify);
	void OnDuiWinDrop(const DuiNotify &Notify);
	void OnDuiWinDragFinish(const DuiNotify &Notify);

	//help
protected:
	void FindMoveCtrlStyleH(SIZE szOffset, int &nIndexDrag, CDUIRect rcDragCtrl);
	void FindMoveCtrlStyleV(SIZE szOffset, int &nIndexDrag, CDUIRect rcDragCtrl);
	void FindMoveCtrlStyleHV(SIZE szOffset, int &nIndexDrag, CDUIRect rcDragCtrl);
	void PerformSwapCtrl(int &nIndexDrag, int nIndexStop);
	void SetPosDest(CDUIControlBase *pControl, CDUIRect rcPosDest);
	bool IsDragingCtrl(CDUIControlBase *pControl);

	//help
protected:
	virtual int GetChildCount() const;
	virtual int GetChildIndex(CDUIControlBase *pChild) const;
	virtual CDUIControlBase * GetChildAt(int nIndex) const;
};

//////////////////////////////////////////////////////////////////////////
class XUI_API CDUIAnimationWnd
	: public CDUIAnimation
{
	MMDeclare_Super(CDUIAnimation)
	
public:
	CDUIAnimationWnd();
	virtual ~CDUIAnimationWnd();

protected:
	HDC									m_hMemDCSrc = NULL;
	HDC									m_hMemDCAni = NULL;
	HBITMAP								m_hBitmapSrc = NULL;
	HBITMAP								m_hBitmapAni = NULL;
	HBITMAP								m_hBitmapReplase = NULL;
	HBITMAP								m_hBitmapSrcOld = NULL;
	HBITMAP								m_hBitmapAniOld = NULL;
	Gdiplus::Bitmap *					m_pBitmapSrc = NULL;
	Gdiplus::Bitmap *					m_pBitmapReplase = NULL;
	LPBYTE								m_pBmpAniBits = NULL;
	BITMAP								m_BmpInfoSrc = {};

	CDUIRect							m_rcWndSrc;
	CDUIRect							m_rcWndAnimation;
	CDUISize							m_szSpringValue;
	CDUISize							m_szRotateRadius;		
	CDUISize							m_szRoundRadius;
	bool								m_bShow = true;
	bool								m_bWndLayered = false;
	bool								m_bAnimating = false;

	//override
protected:
	bool StartAnimation(int nAnimationID = Dui_TimerAnimate_ID, int nElapse = Dui_TimerAnimate_Elapse, int nTotalFrame = ANIMATE_FRAME, bool bLoop = FALSE) override;
	void OnAnimationStart(int nAnimationID, bool bFirstLoop) override;
	void OnAnimationStep(int nAnimationID, int nTotalFrame, int nCurFrame) override;
	void OnAnimationStop(int nAnimationID) override;

	//animationwnd
protected:
	void OnDrawAnimationWnd();

	//method
public:
	virtual HWND GetWndHandle() { return NULL; }
	virtual HDC GetWndDC() { return NULL; }
	virtual BYTE GetWndAlpha() { return 255; }
	virtual enDuiAnimateWndType GetAnimateWndType() { return AnimateWnd_None; }
	virtual int GetAnimateWndFrame() { return 0; }
	virtual int GetAnimateWndElapse() { return 0; }
	virtual CDUIContainerCtrl * GetRootCtrl() { return NULL; }
	virtual bool IsWndLayered() { return false; }
	virtual void SetWndLayered(bool bLayered) {}
	void StartAnimationWnd(bool bShow = true);
	void StopAnimationWnd();
	bool IsAnimatingWnd();

	//help
protected:
	void UnInit();
	void OnAnimateWndLeft(int nAnimationID, int nTotalFrame, int nCurFrame);
	void OnAnimateWndLeftDiffuse(int nAnimationID, int nTotalFrame, int nCurFrame);
	void OnAnimateWndRight(int nAnimationID, int nTotalFrame, int nCurFrame);
	void OnAnimateWndRightDiffuse(int nAnimationID, int nTotalFrame, int nCurFrame);
	void OnAnimateWndTop(int nAnimationID, int nTotalFrame, int nCurFrame);
	void OnAnimateWndTopDiffuse(int nAnimationID, int nTotalFrame, int nCurFrame);
	void OnAnimateWndBottom(int nAnimationID, int nTotalFrame, int nCurFrame);
	void OnAnimateWndBottomDiffuse(int nAnimationID, int nTotalFrame, int nCurFrame);
	void OnAnimateWndSize(int nAnimationID, int nTotalFrame, int nCurFrame);
	void OnAnimateWndSizeSpring(int nAnimationID, int nTotalFrame, int nCurFrame);
	void OnAnimateWndSizeDiffuse(int nAnimationID, int nTotalFrame, int nCurFrame);
	void OnAnimateWndCenterDiffuse(int nAnimationID, int nTotalFrame, int nCurFrame);
	void OnAnimateWndLeftRightDiffuse(int nAnimationID, int nTotalFrame, int nCurFrame);
	void OnAnimateWndLeftRightExpand(int nAnimationID, int nTotalFrame, int nCurFrame);
	void OnAnimateWndLeftRightExpandSpring(int nAnimationID, int nTotalFrame, int nCurFrame);
	void OnAnimateWndLeftRightCombin(int nAnimationID, int nTotalFrame, int nCurFrame);
	void OnAnimateWndTopBottomDiffuse(int nAnimationID, int nTotalFrame, int nCurFrame);
	void OnAnimateWndTopBottomExpand(int nAnimationID, int nTotalFrame, int nCurFrame);
	void OnAnimateWndTopBottomExpandSpring(int nAnimationID, int nTotalFrame, int nCurFrame);
	void OnAnimateWndTopBottomCombin(int nAnimationID, int nTotalFrame, int nCurFrame);
	void OnAnimateWnd3DRotateLeftRight(int nAnimationID, int nTotalFrame, int nCurFrame);
	void OnAnimateWnd3DRotateTopBottom(int nAnimationID, int nTotalFrame, int nCurFrame);
	void OnAnimateWnd3DRoundLeft(int nAnimationID, int nTotalFrame, int nCurFrame);
	void OnAnimateWnd3DRoundRight(int nAnimationID, int nTotalFrame, int nCurFrame);
	void OnAnimateWnd3DRoundTop(int nAnimationID, int nTotalFrame, int nCurFrame);
	void OnAnimateWnd3DRoundBottom(int nAnimationID, int nTotalFrame, int nCurFrame);
	void OnAnimateWnd3DRotateReplaseLR(int nAnimationID, int nTotalFrame, int nCurFrame);
	void OnAnimateWnd3DRotateReplaseTB(int nAnimationID, int nTotalFrame, int nCurFrame);
	void OnAnimateWnd3DRoundReplaseLR(int nAnimationID, int nTotalFrame, int nCurFrame);
	void OnAnimateWnd3DRoundReplaseTB(int nAnimationID, int nTotalFrame, int nCurFrame);

	//help
protected:
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif 