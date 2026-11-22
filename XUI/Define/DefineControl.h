#ifndef __DUIDEFINECONTROL_H__
#define __DUIDEFINECONTROL_H__

#pragma once					

//////////////////////////////////////////////////////////////////////////
#ifdef DUI_DESIGN
#define DuiInitAttriVisible(Attri, Visible) { Attri.SetVisible(Visible); }
#define DuiModifyAttriDescribe(Attri, Describe) { Attri.SetAttributeDescr(Describe); }
#else
#define DuiInitAttriVisible(Attri, Visible)
#define DuiModifyAttriDescribe(Attri, Describe)
#endif

#define DuiGetAttriGroupIndex(Attri) (CDUIGlobal::GetAttributeGroupIndex(&Attri))
#define DuiSetAttriGroupIndex(Attri, nIndex) (CDUIGlobal::SetAttributeGroupIndex(&Attri, nIndex))

//////////////////////////////////////////////////////////////////////////
//describe
#define Dui_Prop_Object								(_T("PropertyObj"))
#define Dui_Window									(_T("Window"))
#define Dui_WindowBase								(_T("WindowBase"))
#define Dui_WindowWin32								(_T("WindowWin32"))
#define Dui_WindowSDL								(_T("WindowSDL"))
#define Dui_WindowVirtual							(_T("WindowVirtual"))

#define Dui_Ctrl_Base								(_T("BaseCtrl"))
#define Dui_Ctrl_Static								(_T("StaticCtrl"))
#define Dui_Ctrl_Edit								(_T("EditCtrl"))
#define Dui_Ctrl_RichEdit							(_T("RichEditCtrl"))
#define Dui_Ctrl_XEdit								(_T("XEditCtrl"))
#define Dui_Ctrl_Button								(_T("ButtonCtrl"))
#define Dui_Ctrl_CheckBox							(_T("CheckBoxCtrl"))
#define Dui_Ctrl_RadioBox							(_T("RadioBoxCtrl"))
#define Dui_Ctrl_Progress							(_T("ProgressCtrl"))
#define Dui_Ctrl_ProgressCircle						(_T("ProgressCricleCtrl"))
#define Dui_Ctrl_Slider								(_T("SliderCtrl"))
#define Dui_Ctrl_ScrollBar							(_T("ScrollBarCtrl"))
#define Dui_Ctrl_Combox								(_T("ComboxCtrl"))
#define Dui_Ctrl_Animate							(_T("AnimateCtrl"))
#define Dui_Ctrl_MenuItem							(_T("MenuItemCtrl"))
#define Dui_Ctrl_Menu								(_T("MenuCtrl"))
#define Dui_Ctrl_HotKey								(_T("HotKeyCtrl"))

#define Dui_Ctrl_Container							(_T("ContainerCtrl"))
#define Dui_Ctrl_Group								(_T("GroupCtrl"))
#define Dui_Ctrl_HorizontalLayout					(_T("HorizontalLayoutCtrl"))
#define Dui_Ctrl_VerticalLayout						(_T("VerticalLayoutCtrl"))
#define Dui_Ctrl_ListHeaderItem						(_T("ListHeaderItemCtrl"))
#define Dui_Ctrl_ListHeader							(_T("ListHeaderCtrl"))
#define Dui_Ctrl_ListItem							(_T("ListItemCtrl"))
#define Dui_Ctrl_ListView							(_T("ListViewCtrl"))
#define Dui_Ctrl_DockBar							(_T("DockBarCtrl"))
#define Dui_Ctrl_IPAddress							(_T("IPAddressCtrl"))
#define Dui_Ctrl_ColorPalette						(_T("ColorPaletteCtrl"))
#define Dui_Ctrl_Spin								(_T("SpinCtrl"))
#define Dui_Ctrl_Calendar							(_T("CalendarCtrl"))
#define Dui_Ctrl_RotateMenuItem						(_T("RotateMenuItemCtrl"))
#define Dui_Ctrl_RotateMenu							(_T("RotateMenuCtrl"))
#define Dui_Ctrl_Instrument							(_T("InstrumentCtrl"))
#define Dui_Ctrl_Clock								(_T("ClockCtrl"))
#define Dui_Ctrl_DateTime							(_T("DateTimeCtrl"))
#define Dui_Ctrl_Water								(_T("WaterCtrl"))
#define Dui_Ctrl_TreeView							(_T("TreeViewCtrl"))
#define Dui_Ctrl_TreeNode							(_T("TreeNodeCtrl"))
#define Dui_Ctrl_TabLayout							(_T("TabLayoutCtrl"))
#define Dui_Ctrl_KLineChart							(_T("KLineChartCtrl"))
#define Dui_Ctrl_WebBrowser							(_T("WebBrowserCtrl"))

//////////////////////////////////////////////////////////////////////////

#endif

