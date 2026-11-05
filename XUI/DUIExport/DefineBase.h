#ifndef __DUIEXPORTDEFINEBASE_H__
#define __DUIEXPORTDEFINEBASE_H__

#pragma once

class CDUIAttributeObject;
class CDUIAttributeGroup;
class CDUIAttributeText;
class CDUIAttributeBool;
class CDUIAttributePosition;
class CDUIAttributeLong;
class CDUIAttributeCursor;
class CDUIAttributeColorSwitch;
class CDUIAttriImageSection;
class CDUIAttributeRect;
class CDUIAttributeCombox;
class CDUIAttributeSize;
class CDUIAttributeColor;
class CDUIAttributeTextStyle;
class CDUIWndBase;
class CDUIWnd;
class CDUIWndVirtual;
class CDUIWndSDL;
class CDUIResourceBase;
class CDUIImageBase;
class CDUIFontBase;
class CDUIPropertyObject;
class CDUIRadioBoxCtrl;
class CDUIControlBase;
class CDUICheckBoxCtrl;
class CDUIContainerCtrl;
class CDUITabLayoutCtrl;
class CDUIListHeaderItemCtrl;
class CDUIListHeaderCtrl;
class CDUIListViewCtrl;
class CDUITreeNodeCtrl;
class CDUITreeViewCtrl;
class CDUIWkeBrowserCtrl;
class CDUIComboxCtrl;
class CDUIScrollBarCtrl;
class CDUILayoutView;
class CDUICalendarCtrl;

//////////////////////////////////////////////////////////////////////////
#define DUI_WNDSTYLE_CONTAINER			(0)
#define DUI_WNDSTYLE_FRAME				(WS_VISIBLE | WS_OVERLAPPEDWINDOW)
#define DUI_WNDSTYLE_CHILD				(WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)
#define DUI_WNDSTYLE_DIALOG				(WS_VISIBLE | WS_POPUPWINDOW | WS_CAPTION | WS_DLGFRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)

#define DUI_WNDSTYLE_EX_FRAME			(WS_EX_WINDOWEDGE)
#define DUI_WNDSTYLE_EX_DIALOG			(WS_EX_TOOLWINDOW | WS_EX_DLGMODALFRAME)

#define DUI_CLASSSTYLE_CONTAINER		(0)
#define DUI_CLASSSTYLE_FRAME			(CS_VREDRAW | CS_HREDRAW)
#define DUI_CLASSSTYLE_CHILD			(CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_SAVEBITS)
#define DUI_CLASSSTYLE_DIALOG			(CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_SAVEBITS)

//////////////////////////////////////////////////////////////////////////
#define WM_DPICHANGED					(0x02E0)

/////////////////////////////////////////////////////////////////////////////////////
#ifndef ASSERT
#define ASSERT(expr)  _ASSERTE(expr)
#endif

#define DUITRACEMSG DUI__TraceMsg

/////////////////////////////////////////////////////////////////////////////////////
//Flags for CDUIControlBase::GetControlFlags()
#define DUIFLAG_SETCURSOR				0x00000001

//Flags for FindControl()
#define DuiFind_All						0x00000000
#define DuiFind_Visible					0x00000001
#define DuiFind_Enabled					0x00000002
#define DuiFind_HitTest					0x00000004
#define DuiFind_UpdateTest				0x00000008
#define DuiFind_TopFirst				0x00000010
#define DuiFind_ThisView				0x00000020
#define DuiFind_MeFirst					0x00000040

//////////////////////////////////////////////////////////////////////////
#include <type_traits>
#define DuiDpiScaleCtrl(x)				(m_pWndOwner ? static_cast<typename std::decay<decltype(x)>::type>(m_pWndOwner->GetDpiObj().Scale(x)) : (x))
#define DuiDpiScaleBackCtrl(x)			(m_pWndOwner ? static_cast<typename std::decay<decltype(x)>::type>(m_pWndOwner->GetDpiObj().ScaleBack(x)) : (x))
#define DuiDpiScaleVerifyCtrl(xTarget, xScaleBack) (m_pWndOwner ? static_cast<typename std::decay<decltype(xScaleBack)>::type>(m_pWndOwner->GetDpiObj().ScaleVerify(xTarget, xScaleBack)) : (xScaleBack))
#define DuiDpiScaleWnd(x)				(static_cast<typename std::decay<decltype(x)>::type>(GetDpiObj().Scale(x)))
#define DuiDpiScaleBackWnd(x)			(static_cast<typename std::decay<decltype(x)>::type>(GetDpiObj().ScaleBack(x)))
#define DuiDpiScaleVerifyWnd(xTarget, xScaleBack) (static_cast<typename std::decay<decltype(xScaleBack)>::type>(GetDpiObj().ScaleVerify(xTarget, xScaleBack)))
#define DuiDpiScaleAttri(x)				(m_pOwner ? static_cast<typename std::decay<decltype(x)>::type>(m_pOwner->GetDpiObj().Scale(x)) : (x))

/////////////////////////////////////////////////////////////////////////////////////
#define DUIBGR(b,g,r)					((DWORD)((((DWORD)(BYTE)(b))<<16) | (((WORD)((BYTE)(g))<<8) | (BYTE)(r))))
#define DUIRGB(r,g,b)					((DWORD)((((DWORD)(BYTE)(r))<<16) | (((WORD)((BYTE)(g))<<8) | (BYTE)(b))))
#define DUIABGR(a,b,g,r)				((DWORD)((((DWORD)(BYTE)(a))<<24) | DUIBGR(b,g,r)))
#define DUIBGRA(b,g,r,a)				((DWORD)(((DUIBGR(b,g,r))<<8) | (BYTE)(a)))
#define DUIARGB(a,r,g,b)				((DWORD)((((DWORD)(BYTE)(a))<<24) | DUIRGB(r,g,b)))
#define DUICOLORREF(b,g,r)				(DUIBGR(b,g,r))
#define DUIARGBGetA(argb)				(LOBYTE((argb)>>24))   
#define DUIARGBGetR(argb)				(LOBYTE(((DWORD)(argb)) >> 16))
#define DUIARGBGetG(argb)				(LOBYTE((WORD)(argb)>>8))  
#define DUIARGBGetB(argb)				(LOBYTE(argb)) 
#define DUIABGRGetA(abgr)				(LOBYTE((abgr)>>24))   
#define DUIABGRGetR(abgr)				(LOBYTE(abgr)) 
#define DUIABGRGetG(abgr)				(LOBYTE((WORD)(abgr)>>8))  
#define DUIABGRGetB(abgr)				(LOBYTE(((DWORD)(abgr)) >> 16))
#define DUIRGBGetR(rgb)					(LOBYTE((rgb)>>16))   
#define DUIRGBGetG(rgb)					(LOBYTE(((WORD)(rgb)) >> 8))
#define DUIRGBGetB(rgb)					(LOBYTE(rgb)) 
#define DUIBGRGetR(bgr)					(LOBYTE(bgr))  
#define DUIBGRGetG(bgr)					(LOBYTE(((WORD)(bgr)) >> 8))
#define DUIBGRGetB(bgr)					(LOBYTE((bgr)>>16))  
#define DUICOLORREFGetR(bgr)			(DUIBGRGetR(bgr))  
#define DUICOLORREFGetG(bgr)			(DUIBGRGetG(bgr))
#define DUICOLORREFGetB(bgr)			(DUIBGRGetB(bgr)) 

///////////////////////////////////////////////////////////////////////////////////////
#define OpenZip OpenZipU
#define CloseZip(hz) CloseZipU(hz)
extern XUI_API HZIPDT OpenZipU(void *z, unsigned int len, DWORD flags, const LPCTSTR pPassword);
extern XUI_API ZRESULT CloseZipU(HZIPDT hz);
#ifdef _UNICODE
#define GetZipItem GetZipItemW
#define FindZipItem FindZipItemW
#else
#define GetZipItem GetZipItemA
#define FindZipItem FindZipItemA
#endif
extern XUI_API ZRESULT GetZipItemA(HZIPDT hz, int index, ZIPENTRYA *ze);
extern XUI_API ZRESULT GetZipItemW(HZIPDT hz, int index, ZIPENTRYW *ze);
extern XUI_API ZRESULT FindZipItemA(HZIPDT hz, const TCHAR *name, bool ic, int *index, ZIPENTRYA *ze);
extern XUI_API ZRESULT FindZipItemW(HZIPDT hz, const TCHAR *name, bool ic, int *index, ZIPENTRYW *ze);
extern XUI_API ZRESULT UnzipItem(HZIPDT hz, int index, void *dst, unsigned int len, DWORD flags);

/////////////////////////////////////////////////////////////////////////////////////
//register create control
#define DuiRegisterCreateControl(ClassName)\
	{CDUIFactoryHelper DUIFactoryModule;\
	DUIFactoryModule.CreateInstance();\
	DUIFactoryModule->RegistControlClass((#ClassName), (DUICreateControlObj)ClassName::CreateControlObj);\
	DUIFactoryModule.CloseInstance();}

//find ctrl
#define Dui_DDX_Control(ClassName, Variant, CtrlID)\
	Variant = MMInterfaceHelper(ClassName, FindControl(CtrlID));

//declare
#define DuiDeclare_CreateControl(ClassName)\
public:\
	static CDUIControlBase * CreateControlObj();

#define DuiDeclare_CreateResource(ClassName)\
public:\
	static CDUIResourceBase * CreateResourceObj();

#define DuiDeclare_CreateAttribute(ClassName)\
public:\
	static CDUIAttributeObject * CreateAttributeObj();

//implement
#define DuiImplement_CreateControl(ClassName)\
 CDUIControlBase * ClassName::CreateControlObj()\
{ return new ClassName; }

#define DuiImplement_CreateResource(ClassName)\
	CDUIResourceBase * ClassName::CreateResourceObj()\
{ return new ClassName; }

#define DuiImplement_CreateAttribute(ClassName)\
	CDUIAttributeObject * ClassName::CreateAttributeObj()\
{ return new ClassName(); }

/////////////////////////////////////////////////////////////////////////////////////
//res type
enum enDuiFileResType
{
	DuiFileResType_File = 1,
	DuiFileResType_Zip,
	DuiFileResType_ResZip,
};

//dui type
enum enDuiType
{
	DuiType_Dlg,
	DuiType_View,
	DuiType_ModelListItem,
	DuiType_ModelTreeNode,
	DuiType_Menu,
	DuiType_3DMenu,
	DuiType_Calendar,
};

//////////////////////////////////////////////////////////////////////////

#endif