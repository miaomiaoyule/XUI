#ifndef __DUIEXPORTDEFINERESOURCE_H__
#define __DUIEXPORTDEFINERESOURCE_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
#define Name_ColorNone					_T("0x00000000")
#define Name_OneAlpha					_T("0x10000000")
#define Name_ColorBlack					_T("0xff000000")
#define Name_ColorWhite					_T("0xffffffff")
#define Name_ColorRed					_T("0xffff0000")
#define Name_ColorGreen					_T("0xff00ff00")
#define Name_ColorBlue					_T("0xff0000ff")
#define Name_ColorGray					_T("0xffcccccc")
#define Name_ColorDefault				_T("0xfff0f0f0")
#define Name_ColorSelBk					_T("0xff0078d7")
#define Name_Colorff0066cc				_T("0xff0066cc")
#define Name_Colorffe5f3ff				_T("0xffe5f3ff")
#define Name_Colorffcde8ff				_T("0xffcde8ff")
#define Name_Colorff252429				_T("0xff252429")
#define Name_Colorff727176				_T("0xff727176")
#define Name_Colorffbababf				_T("0xffbababf")
#define Name_Colorff2f6693				_T("0xff2f6693")
#define Name_Colorff0a0a0a				_T("0xff0a0a0a")
#define Name_Colorff333237				_T("0xff333237")
#define Name_Colorff414143				_T("0xff414143")
#define Name_Colorff3f3f41				_T("0xff3f3f41")
#define Name_Color11dcdcdc				_T("0x11dcdcdc")
#define Name_Colorffc1c1c1				_T("0xffc1c1c1")
#define Name_Colorffcce4f7				_T("0xffcce4f7")
#define Name_Colordcfaad3b				_T("0xdcfaad3b")
#define Name_Colorff616163				_T("0xff616163")
#define Name_Colorff27ccef				_T("0xff27ccef")
#define Name_Colorff616af1				_T("0xff616af1")

#define Name_ImageScrollBarHoriz		_T("{88D041B7-27A9-4819-9986-BF3A631B2A36}")
#define Name_ImageScrollBarVert			_T("{689FF69E-A8B1-453C-A1FE-58D234E97A06}")
#define Name_ImageCheckExpand			_T("{F1127AB2-0950-430F-A490-E90A40C394CD}")
#define Name_ImageCheckExpandTreeNode	_T("{0A9C16DA-415E-40DB-BB10-24E1F6855595}")
#define Name_ImageSliderBk				_T("{AE3A1229-31BA-480A-9B1B-F98A92D797AD}")
#define Name_ImageSliderThumb			_T("{F8B7DCBC-923C-4E60-A2FA-1032B9BBFDEE}")
#define Name_ImageBtnMin				_T("{54D213E5-B49F-4A54-A5AC-7B3EE84ABE6C}")
#define Name_ImageBtnMax				_T("{11F140C1-5089-416A-AFC8-FAF0BC31DCFC}")
#define Name_ImageBtnRestore			_T("{E3B96193-7CF7-4D67-BB8A-99961F2A6DAA}")
#define Name_ImageBtnClose				_T("{66EB709C-5356-4559-894B-CE733B57AFB3}")
#define Name_ImageBtnArrowUp			_T("{27B6E60D-8BF0-4080-8E69-8C32E4BA3634}")
#define Name_ImageBtnArrowDown			_T("{19B40436-9EB4-4F5C-8A3B-37C5A1266C4A}")
#define Name_ImageBtnLeft				_T("{3ED746FD-6AA5-4D94-B5C0-0804750E786A}")
#define Name_ImageBtnRight				_T("{0EC922B7-6ECE-48F9-B65E-0317A4CF2BA6}")
#define Name_ImageCheckBox				_T("{95A0A73A-4E45-4D47-A9AB-5FC7E0F3A641}")
#define Name_ImageRadioBox				_T("{982B9EAD-50CE-49C7-9332-896DC826A32E}")
#define Name_ImageProgress				_T("{3FD27CF4-C264-46DE-A8B4-E8D4865BE862}")
#define Name_ImageComboxBk				_T("{6115C34A-A7D3-4C3F-8222-3BBBAE1ECE38}")
#define Name_ImageCardMaster			_T("{3FA79189-52BE-413F-B120-A2A5CDAB1B56}")
#define Name_ImageClockDial				_T("{55B3D83B-F683-4D24-932E-B6B8593E8A21}")
#define Name_ImageClockPointer			_T("{C3D2A82D-D4C7-4ECF-8825-89CE3D514115}")
#define Name_ImageGif					_T("{FA94D6A6-40B8-41CB-83FA-B886716BE3C3}")
#define Name_ImageSequenceFrame			_T("{3482C09B-88D0-4A30-B854-9DBC23B83587}")
#define Name_ImageButton				_T("{5D038E0C-9B44-4CCA-AAF3-9B14175FEF27}")
#define Name_ImageExpandToolsArrow		_T("{792618B7-4B73-47BE-ABBE-6C21591B5D9B}")
#define Name_ImageControlIcon			_T("{A7F2889A-E9C4-4B4D-B53D-4867D4E7551F}")
#define Name_ImageToolBar				_T("{5AD8A847-1A93-4075-83FB-498D11197A64}")

static LPCTSTR g_szPublicColor[] = 
{
	Name_OneAlpha,
	Name_ColorBlack,
	Name_ColorWhite,
	Name_ColorRed,
	Name_ColorGreen,
	Name_ColorBlue,
	Name_ColorGray,
	Name_ColorDefault,
	Name_ColorSelBk,
	Name_Colorff0066cc,
	Name_Colorffe5f3ff,
	Name_Colorffcde8ff,
	Name_Colorff252429,
	Name_Colorff727176,
	Name_Colorffbababf,
	Name_Colorff2f6693,
	Name_Colorff0a0a0a,
	Name_Colorff333237,
	Name_Colorff414143,
	Name_Colorff3f3f41,
	Name_Color11dcdcdc,
	Name_Colorffc1c1c1,
	Name_Colorffcce4f7,
	Name_Colordcfaad3b,
	Name_Colorff616163,
	Name_Colorff27ccef,
	Name_Colorff616af1,
};

static LOGFONT g_PublicFont[] =
{
	{ 8, 0, 0, 0, FW_NORMAL, false, false, false, GB2312_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, _T("微软雅黑") },
	{ 8, 0, 0, 0, FW_BOLD, false, false, false, GB2312_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, _T("微软雅黑") },
	{ 12, 0, 0, 0, FW_NORMAL, false, false, false, GB2312_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, _T("微软雅黑") },
	{ 12, 0, 0, 0, FW_BOLD, false, false, false, GB2312_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, _T("微软雅黑") },
	{ 14, 0, 0, 0, FW_NORMAL, false, false, false, GB2312_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, _T("微软雅黑") },
	{ 14, 0, 0, 0, FW_BOLD, false, false, false, GB2312_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, _T("微软雅黑") },
	{ 16, 0, 0, 0, FW_NORMAL, false, false, false, GB2312_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, _T("微软雅黑") },
	{ 16, 0, 0, 0, FW_BOLD, false, false, false, GB2312_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, _T("微软雅黑") }
};

static std::unordered_map<CMMString, CMMString> g_mapPublicImage = 
{
	{ Name_ImageScrollBarHoriz, _T("SkinDesign/ScrollBarHoriz.png") },
	{ Name_ImageScrollBarVert, _T("SkinDesign/ScrollBarVert.png") },
	{ Name_ImageCheckExpand, _T("SkinDesign/CheckExpand.png") },
	{ Name_ImageCheckExpandTreeNode, _T("SkinDesign/CheckExpandTreeNode.png") },
	{ Name_ImageSliderBk, _T("SkinDesign/SliderBk.bmp") },
	{ Name_ImageSliderThumb, _T("SkinDesign/SliderThumb.png") },
	{ Name_ImageBtnMin, _T("SkinDesign/BtnMin.png") },
	{ Name_ImageBtnMax, _T("SkinDesign/BtnMax.png") },
	{ Name_ImageBtnRestore, _T("SkinDesign/BtnRestore.png") },
	{ Name_ImageBtnClose, _T("SkinDesign/BtnClose.png") },
	{ Name_ImageBtnArrowUp, _T("SkinDesign/BtnArrowUp.png") },
	{ Name_ImageBtnArrowDown, _T("SkinDesign/BtnArrowDown.png") },
	{ Name_ImageBtnLeft, _T("SkinDesign/BtnLeft.png") },
	{ Name_ImageBtnRight, _T("SkinDesign/BtnRight.png") },
	{ Name_ImageCheckBox, _T("SkinDesign/CheckBox.png") },
	{ Name_ImageRadioBox, _T("SkinDesign/RadioBox.png") },
	{ Name_ImageProgress, _T("SkinDesign/Progress.png") },
	{ Name_ImageComboxBk, _T("SkinDesign/Combox.png") },
	{ Name_ImageCardMaster, _T("SkinDesign/ImageCardMaster.png") },
	{ Name_ImageClockDial, _T("SkinDesign/ClockDial.png") },
	{ Name_ImageClockPointer, _T("SkinDesign/ClockPointer.png") },
	{ Name_ImageGif, _T("SkinDesign/Face.gif") },
	{ Name_ImageSequenceFrame, _T("SkinDesign/SequenceFrame.png") },
	{ Name_ImageButton, _T("SkinDesign/Button.png") },
	{ Name_ImageExpandToolsArrow, _T("SkinDesign/ExpandToolsArrow.svg") },
	{ Name_ImageControlIcon, _T("SkinDesign/ControlIcon.png") },
	{ Name_ImageToolBar, _T("SkinDesign/ImageToolBar.png") },
};

//////////////////////////////////////////////////////////////////////////
enum enDuiResVersion
{
	DuiResVersion_0 = 0,
	DuiResVersion_3 = 3,
	DuiResVersion_5 = 5,
	DuiResVersion_Max = DuiResVersion_5,
};

enum enDuiResType
{
	DuiResType_None = -1,
	DuiResType_Font,
	DuiResType_Image,
	DuiResType_Color,
};

enum enDuiImageType
{
	DuiImageType_Normal,
	DuiImageType_Gif,
	DuiImageType_Svg,
	DuiImageType_Webp,
};

//////////////////////////////////////////////////////////////////////////

#endif