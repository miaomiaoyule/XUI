#ifndef __DUIEXPORTDEFINEMESSAGE_H__
#define __DUIEXPORTDEFINEMESSAGE_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
typedef std::vector<CDUIControlBase*> VecDuiControlBase;
typedef std::deque<CDUIControlBase*> DequeDuiControlBase;
typedef std::vector<CDUIRadioBoxCtrl*> VecDuiRadioBoxCtrl;
typedef std::unordered_map<CMMString, VecDuiRadioBoxCtrl> MapDuiRadioBoxGroup;

//////////////////////////////////////////////////////////////////////////
//custom message
//** user should custom wnd message greater than WM_DUIINNER_FINISH **//
#define WM_DUIINNER_START				(WM_USER + 1989)
#define WM_DUIINNER_FINISH				(WM_DUIINNER_START + 10)

//////////////////////////////////////////////////////////////////////////
enum enDuiNotifyType
{
	DuiNotify_Invalid = -1,
	DuiNotify_WndInited = 0,
	DuiNotify_WndLayout = 1,
	DuiNotify_WndAnimateStart = 2,
	DuiNotify_WndAnimateFinish = 3,
	DuiNotify_DragAnimateStart = 10,
	DuiNotify_DragAnimateMoving = 11,
	DuiNotify_DragAnimateFinish = 12,
	DuiNotify_DpiChanged = 13,
	DuiNotify_CtrlIDChanged = 40,
	DuiNotify_VisibleChanged = 41,
	DuiNotify_WndManagerAttach = 50,
	DuiNotify_WndManagerDetach = 51,
	DuiNotify_Timer = 52,
	DuiNotify_Command = 53,
	DuiNotify_SetFocus = 111,
	DuiNotify_KillFocus = 112,
	DuiNotify_Menu = 113,
	DuiNotify_TextChanged = 114,
	DuiNotify_Edited = 115,
	DuiNotify_Return = 116,
	DuiNotify_ValueChanged = 120,
	DuiNotify_Scroll = 121,
	DuiNotify_LButtonDown = 150,
	DuiNotify_LButtonUp = 151,
	DuiNotify_Click = 152,
	DuiNotify_LDbClick = 153,
	DuiNotify_RButtonDown = 154,
	DuiNotify_RButtonUp = 155,
	DuiNotify_RClick = 156,
	DuiNotify_RDbClick = 157,
	DuiNotify_MouseEnter = 160,
	DuiNotify_MouseHover = 161,
	DuiNotify_MouseMove = 162,
	DuiNotify_MouseLeave = 163,
	DuiNotify_MouseWheel = 164,
	DuiNotify_SetCursor = 165,
	DuiNotify_KeyDown = 200,
	DuiNotify_KeyUp = 201,
	DuiNotify_Char = 202,
	DuiNotify_SysKeyDown = 203,
	DuiNotify_SysKeyUp = 204,
	DuiNotify_SysChar = 205,
	DuiNotify_SelectChanged = 250,
	DuiNotify_ComboxExpand = 251,
	DuiNotify_TabSelectChanged = 252,
	DuiNotify_SwitchListViewType = 260,
	DuiNotify_HeadItemButtonDown = 261,
	DuiNotify_HeadItemButtonUp = 262,
	DuiNotify_HeadItemClick = 263,
	DuiNotify_ItemButtonDown = 300,
	DuiNotify_ItemButtonUp = 301,
	DuiNotify_ItemClick = 302,
	DuiNotify_ItemDbClick = 303,
	DuiNotify_ItemRButtonDown = 310,
	DuiNotify_ItemRButtonUp = 311,
	DuiNotify_ItemRDbClick = 312,
	DuiNotify_ItemMouseEnter = 320,
	DuiNotify_ItemMouseMove = 321,
	DuiNotify_ItemMouseLeave = 322,
	DuiNotify_ItemActive = 330,
	DuiNotify_ItemSelected = 331,
	DuiNotify_ItemUnSelected = 332,
	DuiNotify_ItemTextChange = 333,
	DuiNotify_ItemEditFinish = 334,
	DuiNotify_ItemExpand = 340,
	DuiNotify_ItemUnExpand = 341,
	DuiNotify_ItemKeyDown = 350,
	DuiNotify_ItemKeyUp = 351,
	DuiNotify_WinDragStart = 400,
	DuiNotify_WinDragCreateObj = 401,
	DuiNotify_WinDragCreateBmp = 402,
	DuiNotify_WinDragFinish = 403,
	DuiNotify_WinDragEnter = 410,
	DuiNotify_WinDragOver = 411,
	DuiNotify_WinDragLeave = 412,
	DuiNotify_WinDrop = 413,
};

//notify sig
enum DuiSig
{
	DuiSign_end = 0,									//[marks end of message map]
	DuiSign_l_w_l,										//LRESULT (WPARAM, LPARAM)
	DuiSign_v_n,										//void (DuiNotify)
};

//message struct
typedef struct tagDuiMessage
{
	WPARAM								wParam = 0;
	LPARAM								lParam = 0;
	DWORD								dwTimestamp = 0;
	POINT								ptMousePre = {};
	POINT								ptMouse = {};
	TCHAR								chKey = 0;
	WORD								wKeyState = 0;
	CMMString							strText;			
	CDUIControlBase *					pMsgCtrl = NULL;
}DuiMessage;

typedef struct tagDuiNotify
{
	enum enDuiNotifyExtendType
	{
		DuiNotifyExtend_None,
		DuiNotifyExtend_Combox,
		DuiNotifyExtend_ListView,
		DuiNotifyExtend_TreeView,
		DuiNotifyExtend_WndAnimate,
	};

	enDuiNotifyType						NotifyType = DuiNotify_Invalid;
	CDUIControlBase *					pNotifyCtrl = NULL;						//maybe released, you should verify valid
	UINT								uCtrlID = 0;
	DWORD								dwTimestamp = 0;
	POINT								ptMouse = {};
	POINT								ptMousePre = {};
	WORD								wKeyState = 0;
	WPARAM								wParam = 0;
	LPARAM								lParam = 0;

	struct tagDuiNotifyExtend
	{
		enDuiNotifyExtendType			Type = DuiNotifyExtend_None;
		struct
		{
			CDUIComboxCtrl *			pComboxCtrl;
			int							nIndexItem;
		}Combox;
		struct tagDuiNotifyListView
		{
			int							nIndexItem;
			CMMString					strTextOld;
		}ListView;
		struct tagDuiNotifyTreeView
		{
			CDUITreeNodeCtrl *			pTreeNode;
			CDUITreeNodeCtrl *			pRootNode;
			CDUITreeViewCtrl *			pRootView;
			CMMString					strTextOld;
		}TreeView;
		struct tagDuiNotifyWndAnimate
		{
			bool						bShow;
		}WndAnimate;
	}DuiNotifyExtend = {};
}DuiNotify;

typedef struct tagDuiMenuCmd
{
	UINT								uMenuID = 0;
	UINT								uMenuTag = 0;
	CMMString							strMenuUserData;
	CMMString							strMenuText;
}DuiMenuCmd;

typedef struct tagDuiCalendarCmd
{
	bool								bSelected = false;
	SYSTEMTIME							tTimeSelected = {};
}DuiCalendarCmd;

typedef std::vector<enDuiNotifyType> VecDuiNotifyType;
typedef std::vector<DuiNotify> VecDuiNotify;

//////////////////////////////////////////////////////////////////////////
class CDUINotifyPump;
typedef void (CDUINotifyPump::*Dui_PMsg)(const DuiNotify &Msg);

union DuiMessageMapFunctions
{
	Dui_PMsg pfn;   // generic member function pointer
	LRESULT(CDUINotifyPump::*pfn_Notify_l_w_l)(WPARAM, LPARAM);
	void (CDUINotifyPump::*pfn_Notify_v_n)(const DuiNotify&);
};

//msg entry
struct Dui_MsgMapEntry
{
	enDuiNotifyType						Notify;			//dui nofity
	UINT								uCtrlID;		//control id
	UINT								nSig;           //signature type (action) or pointer to message #
	Dui_PMsg							pfn;			//routine to call (or special value)
};

//////////////////////////////////////////////////////////////////////////
struct Dui_MsgMap
{
	const Dui_MsgMap * (PASCAL* pfnGetBaseMap)();
	const Dui_MsgMapEntry *lpEntries;
};

#define DuiDeclare_Message_Map()                                         \
protected:                                                                \
	static const Dui_MsgMap * PASCAL _GetBaseMessageMap();                 \
	static const Dui_MsgMap * PASCAL GetThisDuiMessageMap();               \
	virtual const Dui_MsgMap * GetDuiMessageMap() const;                      \

#define DuiBaseBegin_Message_Map(theClass)                              \
	const Dui_MsgMap * PASCAL theClass::_GetBaseMessageMap()               \
{ return NULL; }                                                  \
	const Dui_MsgMap * theClass::GetDuiMessageMap() const                     \
{ return GetThisDuiMessageMap(); }                                 \
	const Dui_MsgMap * PASCAL theClass::GetThisDuiMessageMap()               \
	{                                                                     \
		typedef theClass ThisClass;                                       \
		static const Dui_MsgMapEntry _messageEntries[] =                  \
{  

#define DuiBegin_Message_Map(theClass, baseClass)                        \
	const Dui_MsgMap * PASCAL theClass::_GetBaseMessageMap()               \
{ return baseClass::GetThisDuiMessageMap(); }                       \
	const Dui_MsgMap * theClass::GetDuiMessageMap() const                     \
{ return GetThisDuiMessageMap(); }                                 \
	const Dui_MsgMap * PASCAL theClass::GetThisDuiMessageMap()               \
	{                                                                     \
		typedef theClass ThisClass;                                       \
		static const Dui_MsgMapEntry _messageEntries[] =                  \
{                                                                     \

#define DuiEnd_Message_Map()                                             \
{ DuiNotify_Invalid, 0, DuiSign_end, (Dui_PMsg)0 }                           \
};                                                                   \
	static const Dui_MsgMap messageMap =                              \
	{ &ThisClass::_GetBaseMessageMap, &_messageEntries[0] };          \
		return &messageMap;                                               \
	}

//////////////////////////////////////////////////////////////////////////
#define Dui_On_Notify(NotifyCode, memberFxn)                                \
{ NotifyCode, 0, DuiSign_v_n, (Dui_PMsg)(&ThisClass::memberFxn)},                  \

#define Dui_On_Notify_Ctrl(NotifyCode, CtrlID, memberFxn)                 \
{ NotifyCode, CtrlID, DuiSign_v_n, (Dui_PMsg)(&ThisClass::memberFxn) },                \

#define Dui_On_Click_Ctrl(CtrlID, memberFxn)                           \
{ DuiNotify_Click, CtrlID, DuiSign_v_n, (Dui_PMsg)(&ThisClass::memberFxn) },      \

#define Dui_On_SelectChanged_Ctrl(CtrlID, memberFxn)                   \
{ DuiNotify_SelectChanged, CtrlID, DuiSign_v_n,(Dui_PMsg)(&ThisClass::memberFxn) }, \

#define Dui_On_KillFocus_Ctrl(CtrlID, memberFxn)                       \
{ DuiNotify_KillFocus, CtrlID, DuiSign_v_n, (Dui_PMsg)(&ThisClass::memberFxn) },     \

#define Dui_On_Timer()                                                    \
{ DuiNotify_Timer, 0, DuiSign_v_n, (Dui_PMsg)(&ThisClass::OnDuiTimer) },          \

//////////////////////////////////////////////////////////////////////////

#endif
