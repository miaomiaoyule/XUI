#ifndef __DUIEXPORTDEFINECONTROL_H__
#define __DUIEXPORTDEFINECONTROL_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
typedef std::unordered_map<UINT, CDUIControlBase*> MapDuiControlBase;
typedef int (CALLBACK *PLVCompareFunc)(CDUIControlBase *pCtrlFirst, CDUIControlBase *pCtrlNext, UINT_PTR dwData);
typedef CDUIControlBase* (CALLBACK* FindControlProc)(CDUIControlBase*, LPVOID pData);

//////////////////////////////////////////////////////////////////////////
#define Dui_Min_ThumbSize							(5)			
#define Dui_Min_ThumbInterval						(100)
#define Dui_Min_AngleSpace							(10)
#define Dui_Max_AngleSpace							(360)
#define Dui_Count_WeekDay							(7)
#define Dui_ScrollSpeed_Normal						(8)
#define Dui_Max_ListColumn							(32)
#define Dui_Offset_GroupBorderTopBreak				(10)

//////////////////////////////////////////////////////////////////////////
#define DuiCreateGroupAttribute(Variant, Name) \
	CreateGroupAttribute(Variant, Name);

#define DuiCreateAttribute(Variant, Name, Describe, AttriGroup) \
	CreateAttribute(Variant, Name, Describe, &AttriGroup);

#define DuiInitAttriValue(Attri, Value) { if(false == Attri.IsModified()) Attri.SetValue(Value); }
#define DuiInitAttriRectValue(Attri, l, t, r, b) { if(false == Attri.IsModified()) Attri.SetValue(l, t, r, b); }
#define DuiInitAttriSizeValue(Attri, cx, cy) { if(false == Attri.IsModified()) Attri.SetValue(cx, cy); }
#define DuiInitAttriColor(Attri, Color) { if(false == Attri.IsModified()) Attri.SetColor(Color); }
#define DuiInitAttriColorRes(Attri, Color) { if(false == Attri.IsModified()) Attri.SetColorResSwitch({Color}); }
#define DuiInitAttriImageSec(Attri, ImageSec) { if(false == Attri.IsModified()) Attri.SetImageSection(ImageSec); }
#define DuiInitAttriAlignLeft(Attri, bLeft) { if(false == Attri.IsModified()) Attri.SetAlignLeft(bLeft); }
#define DuiInitAttriAlignCenter(Attri, bCenter) { if(false == Attri.IsModified()) Attri.SetAlignCenter(bCenter); }
#define DuiInitAttriAlignRight(Attri, bRight) { if(false == Attri.IsModified()) Attri.SetAlignRight(bRight); }
#define DuiInitAttriVAlignTop(Attri, bTop) { if(false == Attri.IsModified()) Attri.SetVAlignTop(bTop); }
#define DuiInitAttriVAlignCenter(Attri, bCenter) { if(false == Attri.IsModified()) Attri.SetVAlignCenter(bCenter); }
#define DuiInitAttriVAlignBottom(Attri, bBottom) { if(false == Attri.IsModified()) Attri.SetVAlignBottom(bBottom); }
#define DuiSwapAttriIndex(AttriFirst, AttriNext) { if(AttriFirst.GetGroup()) AttriFirst.GetGroup()->SwapAttribute(&AttriFirst, &AttriNext); }
#define DuiCompatibleAttriName(NameOld, NameNew, Version) { CDUIGlobal::GetInstance()->GetResVersion() < Version ? NameOld : NameNew }
#define DuiModifyAttriName(Attri, NameNew, Version) { if(CDUIGlobal::GetInstance()->GetResVersion() < Version) Attri.SetAttributeName(NameNew); }

//////////////////////////////////////////////////////////////////////////
//timer
//** user should settimer id is greater than Dui_TimerInner_Finish for control **//
#define Dui_TimerInner_Start						(1)
#define Dui_TimerScrollAuto_ID						(2)	
#define Dui_TimerScrollSlowdown_ID					(3)
#define Dui_TimerAnimate_ID							(4)
#define Dui_TimerWkeRefresh_ID						(5)
#define Dui_TimerDelayRefresh_ID					(6)
#define Dui_TimerCaret_ID							(7)
#define Dui_TimerClock_ID							(8)
#define Dui_TimerInputChar_ID						(9)
#define Dui_TimerDelaySlider_ID						(10)
#define Dui_TimerInner_Finish						(50)

#define Dui_TimerScrollAuto_Elapse					(30)						
#define Dui_TimerScrollSlowdown_Elapse				(30)
#define Dui_TimerAnimate_Elapse						(15)
#define Dui_TimerWkeRefresh_Elapse					(30)
#define Dui_TimerDelayRefresh_Elapse				(50)
#define Dui_TimerXEditAnimate_Elapse				(100)
#define Dui_TimerCaret_Elapse						(500)
#define Dui_TimerClock_Elapse						(1000)
#define Dui_TimerInputChar_Elapse					(30)
#define Dui_TimerDelaySlider_Elapse					(30)

//////////////////////////////////////////////////////////////////////////
//ctrlid
#define Dui_CtrlIDInner_Start						(1)
#define Dui_CtrlIDInner_BtnClose					(2)
#define Dui_CtrlIDInner_BtnMin						(3)
#define Dui_CtrlIDInner_BtnRestore					(4)
#define Dui_CtrlIDInner_BtnMax						(5)
#define Dui_CtrlIDInner_BtnOk						(6)
#define Dui_CtrlIDInner_BtnCancel					(7)
#define Dui_CtrlIDInner_DlgRoot						(10)
#define Dui_CtrlIDInner_DlgCaption					(11)
#define Dui_CtrlIDInner_DlgLogo						(12)
#define Dui_CtrlIDInner_DlgTitle					(13)
#define Dui_CtrlIDInner_DragTmp						(20)
#define Dui_CtrlIDInner_ListItemEdit				(29)
#define Dui_CtrlIDInner_ListItemSelect				(30)
#define Dui_CtrlIDInner_HeaderItemSelect			(31)
#define Dui_CtrlIDInner_HeaderItemCheck				(32)
#define Dui_CtrlIDInner_TreeNodeVertContainer		(40)
#define Dui_CtrlIDInner_TreeNodeHorizContainer		(41)
#define Dui_CtrlIDInner_TreeNodeExpand				(42)
#define Dui_CtrlIDInner_TreeNodeView				(43)
#define Dui_CtrlIDInner_SpinEditValue				(50)
#define Dui_CtrlIDInner_SpinVertRight				(51)
#define Dui_CtrlIDInner_SpinBtnUp					(52)
#define Dui_CtrlIDInner_SpinBtnDown					(53)
#define Dui_CtrlIDInner_CalendarListDay				(60)
#define Dui_CtrlIDInner_CalendarBtnLeft				(61)
#define Dui_CtrlIDInner_CalendarBtnRight			(62)
#define Dui_CtrlIDInner_CalendarBtnYear				(63)
#define Dui_CtrlIDInner_CalendarBtnMonth			(64)
#define Dui_CtrlIDInner_ColorPaletteEdit			(70)
#define Dui_CtrlIDInner_DefaultBtn					(100)
#define Dui_CtrlIDInner_DefaultHScrollBar			(101)
#define Dui_CtrlIDInner_DefaultVScrollBar			(102)
#define Dui_CtrlIDInner_Finish						(100000)

//////////////////////////////////////////////////////////////////////////
//round type
enum enDuiRoundType
{
	Round_Normal,
	Round_Parallelogram,
	Round_Rhomb,
	Round_Ellipse,
};

//mouse cursor
enum enDuiCursor
{
	Cursor_Arrow = 32512,
	Cursor_IBeam = 32513,
	Cursor_Wait = 32514,
	Cursor_Cross = 32515,
	Cursor_UpArrow = 32516,
	Cursor_SizeNWSE = 32642,
	Cursor_SizeNESW = 32643,
	Cursor_SizeWE = 32644,
	Cursor_SizeNS = 32645,
	Cursor_SizeALL = 32646,
	Cursor_No = 32648,
	Cursor_Hand = 32649,
};

//border style
enum enDuiLineStyle
{
	LineStyle_Solid = 0,
	LineStyle_Dash = 1,					/* -------  */
	LineStyle_Dot = 2,					/* .......  */
	LineStyle_DashDot = 3,				/* _._._._  */
	LineStyle_DashDotDot = 4,			/* _.._.._  */
	LineStyle_Null = 5,
	ListStyle_Count,
};

//border separate
enum enDuiBorderSeparate
{
	BorderSeparate_None = 0x00,
	BorderSeparate_Left = 0x01,
	BorderSeparate_Top = 0x02,
	BorderSeparate_Right = 0x04,
	BorderSeparate_Bottom = 0x08,
};

//control status
enum enDuiControlStatus
{
	ControlStatus_None = 0x00,
	ControlStatus_Normal = 0x01,
	ControlStatus_Hot = 0x02,
	ControlStatus_Pushed = 0x04,
};

//control custom back
enum enDuiCustomBackAlign
{
	CustomBack_Left = 0x0001,
	CustomBack_Center = 0x0002,
	CustomBack_Right = 0x0003,
	CustomBack_Top = 0x0010,
	CustomBack_VCenter = 0x0020,
	CustomBack_Bottom = 0x0030,
	CustomBack_Scale = 0x0100,	//Geometric scaling
	CustomBack_ScaleSuitable = 0x0200,	//Geometric scaling
	CustomBack_Stretch = 0x0300,
};

enum enDuiProgressType
{
	ProgressType_None = -1,
	ProgressType_Horiz = 0,
	ProgressType_Vert
};

enum enDuiProgressMoveType
{
	ProgressMove_None = -1,
	ProgressMove_Positive = 0,
	ProgressMove_Reverse
};

enum enDuiListViewType
{
	ListView_List,
	ListView_TileH,
	ListView_TileV,
};

//animationdrag
enum enDuiAnimateDragType
{
	DragType_H = 0,
	DragType_V,
	DragType_HV
};

enum enDuiAnimateSelSwithType
{
	AnimateSelSwitch_None,
	AnimateSelSwitch_H,
	AnimateSelSwitch_V,
};

enum enDuiCalendarWeekType
{
	CalendarWeek_FirstSunday,
	CalendarWeek_FirstMonday,
};

enum enDuiEditTextType
{
	EditText_None,
	EditText_Number,
	EditText_NumberInt,
	EditText_NumberDouble,
};

enum enDuiRichTextItemType
{
	RichTextItem_Text,
	RichTextItem_Image,
};

enum enDuiKLineChartType
{
	KLineChart_Day,
	KLineChart_Week,
	KLineChart_Month,
};

enum enDuiDockBarEffectType
{
	DockBarEffect_None,
	DockBarEffect_MouseMove,
};

//////////////////////////////////////////////////////////////////////////

#endif

