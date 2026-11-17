#include "StdAfx.h"
#include "DUICalendarCtrl.h"

#define CALENDAR_OFFSET_LRBTN			(15)
#define CALENDAR_COUNT_DAYROW			(6)
#define CALENDAR_COUNT_YEAR				(25)
#define CALENDAR_COUNT_MONTH			(12)

#define CALENDAR_YEAR_START				(1601)
#define CALENDAR_MONTH_START			(1)
#define CALENDAR_DAY_START				(1)

//////////////////////////////////////////////////////////////////////////
MMImplement_ClassName(CDUICalendarWnd)
DuiBegin_Message_Map(CDUICalendarWnd, CDUINotifyPump)
DuiEnd_Message_Map()

CDUICalendarWnd::CDUICalendarWnd(CMMString strDuiName)
	: CDUIPopupWnd(strDuiName)
{
	CDUIControlBase *pRootCtrl = CDUIGlobal::GetInstance()->LoadDui(GetDuiName(), this);
	m_pShowCalendarView = MMInterfaceHelper(CDUICalendarCtrl, pRootCtrl);

	return;
}

CDUICalendarWnd::~CDUICalendarWnd()
{
	Close();

	MMSafeDelete(m_pShowCalendarView);

	return;
}

void CDUICalendarWnd::Init(HWND hWndParent)
{
	assert(m_pShowCalendarView);
	if (IsWindow(m_hWnd) || NULL == m_pShowCalendarView) return;

	Create(hWndParent, _T("DuiCalendarWnd"), WS_POPUP | WS_VISIBLE, WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST);

	// HACK: Don't deselect the parent's caption
	hWndParent = m_hWnd;
	while (::GetParent(hWndParent)) hWndParent = ::GetParent(hWndParent);
	::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);

	//focus
	m_pShowCalendarView->SetFocus();

	return;
}

void CDUICalendarWnd::Init(HWND hWndParent, CDUIPoint ptTrack)
{
	m_ptTrack = ptTrack;

	Init(hWndParent);

	return;
}

void CDUICalendarWnd::Close(UINT nRet)
{
	//save
#ifdef DUI_DESIGN
	if (GetRootCtrl())
	{
		CDUIGlobal::GetInstance()->SaveDui(GetDuiName(), this);
	}
#endif

	__super::Close(nRet);

	DetachRootCtrl();

	return;
}

CDUICalendarCtrl * CDUICalendarWnd::GetCalendarCtrl()
{
	return m_pShowCalendarView;
}

LRESULT CDUICalendarWnd::OnCreate(WPARAM wParam, LPARAM lParam)
{
	DetachRootCtrl();

	__super::OnCreate(wParam, lParam);

	//init view
	AttachRootCtrl(m_pShowCalendarView);

	AdjustCalendar();

	return 0;
}

LRESULT CDUICalendarWnd::OnKillFocus(WPARAM wParam, LPARAM lParam)
{
	__super::OnKillFocus(wParam, lParam);

#ifdef DUI_DESIGN
	HMONITOR hMonitor = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONULL);
	if (NULL == hMonitor) return 0;
#endif

	if (g_pDuiCalendarWnd && IsWindow(g_pDuiCalendarWnd->GetWndHandle()))
	{
		HWND hWndFocus = (HWND)wParam;
		while (hWndFocus)
		{
			if (hWndFocus == g_pDuiCalendarWnd->GetWndHandle()) return 0;

			hWndFocus = ::GetParent(hWndFocus);
		}

		g_pDuiCalendarWnd->Close();
	}

	return 0;
}

LRESULT CDUICalendarWnd::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	__super::OnKeyDown(wParam, lParam);

	if (VK_ESCAPE == wParam)
	{
		Close();
	}

	return 0;
}

LRESULT CDUICalendarWnd::OnClose(WPARAM wParam, LPARAM lParam)
{
	__super::OnClose(wParam, lParam);

	return 0;
}

LRESULT CDUICalendarWnd::OnWndMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return  __super::OnWndMessage(uMsg, wParam, lParam);
}

void CDUICalendarWnd::AdjustCalendar()
{
	if (NULL == m_pShowCalendarView) return;

	CDUIRect rcWnd = GetWindowRect();
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	CDUIRect rcWork = oMonitor.rcWork;

	//adjust pt
	if (-1 == m_ptTrack.x && -1 == m_ptTrack.y)
	{
		CDUIRect rcWndParent;
		::GetWindowRect(::GetParent(m_hWnd), &rcWndParent);

		m_ptTrack.x = rcWndParent.left + rcWndParent.GetWidth() / 2 - rcWnd.GetWidth() / 2;
		m_ptTrack.y = rcWndParent.top + rcWndParent.GetHeight() / 2 - rcWnd.GetHeight() / 2;
	}

	rcWnd.Offset(m_ptTrack.x - rcWnd.left, m_ptTrack.y - rcWnd.top);

	if (rcWnd.right > rcWork.right) rcWnd.Offset(rcWork.right - rcWnd.right, 0);
	if (rcWnd.bottom > rcWork.bottom) rcWnd.Offset(0, -rcWnd.GetHeight());

	SetForegroundWindow(m_hWnd);
	MoveWindow(m_hWnd, rcWnd.left, rcWnd.top, rcWnd.GetWidth(), rcWnd.GetHeight(), FALSE);

	return;
}

//////////////////////////////////////////////////////////////////////////
LPCTSTR CDUICalendarCtrl::m_szWeekDayAttriName[Dui_Count_WeekDay] = { _T("Monday"), _T("Tuesday"), _T("Wednesday"), _T("Thursday"), _T("Friday"), _T("Saturday"), _T("Sunday"), };

DuiImplement_CreateControl(CDUICalendarCtrl)
MMImplement_ClassName(CDUICalendarCtrl)

DuiBegin_Message_Map(CDUICalendarCtrl, CDUINotifyPump)
	Dui_On_Notify(DuiNotify_MouseWheel, OnDuiMouseWheel)
	Dui_On_Notify(DuiNotify_ItemSelected, OnDuiItemSelected)
	Dui_On_Notify(DuiNotify_ItemClick, OnDuiItemClick)
	Dui_On_Notify(DuiNotify_TabSelectChanged, OnDuiTabSelectChanged)
	Dui_On_Notify_Ctrl(DuiNotify_Click, Dui_CtrlIDInner_CalendarBtnLeft, OnDuiBtnLeft)
	Dui_On_Notify_Ctrl(DuiNotify_Click, Dui_CtrlIDInner_CalendarBtnRight, OnDuiBtnRight)
	Dui_On_Notify_Ctrl(DuiNotify_Click, Dui_CtrlIDInner_CalendarBtnYear, OnDuiBtnYear)
	Dui_On_Notify_Ctrl(DuiNotify_Click, Dui_CtrlIDInner_CalendarBtnMonth, OnDuiBtnMonth)
DuiEnd_Message_Map()

CDUICalendarCtrl::CDUICalendarCtrl(void)
{
	static tagDuiCombox AttriCombox;
	if (AttriCombox.vecItem.empty())
	{
		AttriCombox.vecItem.push_back({ CalendarWeek_FirstMonday, _T("FirstMonday") });
		AttriCombox.vecItem.push_back({ CalendarWeek_FirstSunday, _T("FirstSunday") });
	}

	m_AttributeWeekStyle.SetCombox(AttriCombox);
	m_AttributeWeekStyle.SelectItem(CalendarWeek_FirstMonday);

	m_AttributeTextWeekDay[0].SetValue(_T("星期一"));
	m_AttributeTextWeekDay[1].SetValue(_T("星期二"));
	m_AttributeTextWeekDay[2].SetValue(_T("星期三"));
	m_AttributeTextWeekDay[3].SetValue(_T("星期四"));
	m_AttributeTextWeekDay[4].SetValue(_T("星期五"));
	m_AttributeTextWeekDay[5].SetValue(_T("星期六"));
	m_AttributeTextWeekDay[6].SetValue(_T("星期天"));

	return;
}

CDUICalendarCtrl::~CDUICalendarCtrl(void)
{
}

bool CDUICalendarCtrl::OnAttributeChange(CDUIAttributeObject *pAttributeObj)
{
	if (__super::OnAttributeChange(pAttributeObj)) return true;

	if (pAttributeObj == &m_AttributeHeaderVisible
		|| pAttributeObj == &m_AttributeHeaderHeight
		|| pAttributeObj == &m_AttributeLeftRightBtnSize
		|| pAttributeObj == &m_AttributeTextStyleYearNormal
		|| pAttributeObj == &m_AttributeTextStyleYearHot
		|| pAttributeObj == &m_AttributeTextStyleMonthNormal
		|| pAttributeObj == &m_AttributeTextStyleMonthHot
		|| pAttributeObj == &m_AttributeColorYearNormal
		|| pAttributeObj == &m_AttributeColorYearHot
		|| pAttributeObj == &m_AttributeColorMonthNormal
		|| pAttributeObj == &m_AttributeColorMonthHot
		|| pAttributeObj == &m_AttributeImageLeftBtnNormal
		|| pAttributeObj == &m_AttributeImageLeftBtnHot
		|| pAttributeObj == &m_AttributeImageRightBtnNormal
		|| pAttributeObj == &m_AttributeImageRightBtnHot
		|| pAttributeObj == &m_AttributeWeekStyle
		|| pAttributeObj == &m_AttributeWeekHeight
		|| pAttributeObj == &m_AttributeColorHotDayBk
		|| pAttributeObj == &m_AttributeColorSelDayBk
		|| pAttributeObj == &m_AttributeColorTodayBk
		|| pAttributeObj == &m_AttributeImageHotDayBk
		|| pAttributeObj == &m_AttributeImageSelDayBk
		|| pAttributeObj == &m_AttributeImageTodayBk
		|| pAttributeObj == &m_AttributeTextStyleWeek
		|| pAttributeObj == &m_AttributeTextStyleHotMonthDay
		|| pAttributeObj == &m_AttributeTextStyleOtherMonthDay
		|| pAttributeObj == &m_AttributeTextStyleCurMonthDay
		|| pAttributeObj == &m_AttributeTextStyleToday
		|| (m_AttributeTextWeekDay + Dui_Count_WeekDay) != find_if(m_AttributeTextWeekDay, m_AttributeTextWeekDay + Dui_Count_WeekDay, [&](CDUIAttributeText &Attribute)
	{
		return &Attribute == pAttributeObj;
	}))
	{
		InitCtrlInfo();

		return true;
	}

	return false;
}

void CDUICalendarCtrl::OnRelease(CDUIControlBase *pControl)
{
	if (NULL == pControl) return;

	return;
}

void CDUICalendarCtrl::OnSize(CDUIControlBase *pControl)
{
	if (NULL == m_pListDayCtrl1 || NULL == m_pListDayCtrl2) return;
	if (m_pListDayCtrl1 != pControl && m_pListDayCtrl2 != pControl) return;

	CDUISize szItem = { pControl->GetWidth() / Dui_Count_WeekDay, pControl->GetHeight() / CALENDAR_COUNT_DAYROW };
	int nXSurplus = pControl->GetWidth() % Dui_Count_WeekDay;
	int nYSurplus = pControl->GetHeight() % CALENDAR_COUNT_DAYROW;
	m_pListDayCtrl1->SetSwitchTileItemSize(szItem);
	m_pListDayCtrl1->SetRangeInset({ nXSurplus / 2, nYSurplus / 2, 0, 0 });
	m_pListDayCtrl2->SetSwitchTileItemSize(szItem);
	m_pListDayCtrl2->SetRangeInset({ nXSurplus / 2, nYSurplus / 2, 0, 0 });

	return;
}

LPVOID CDUICalendarCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUICalendarCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUICalendarCtrl::GetDescribe() const
{
	return Dui_Ctrl_Calendar;
}

bool CDUICalendarCtrl::SetWndOwner(CDUIWnd *pWndOwner)
{
	if (m_pWndOwner)
	{
		m_pWndOwner->RemoveINotify(this);
	}

	if (false == __super::SetWndOwner(pWndOwner)) return false;

	if (m_pWndOwner)
	{
		m_pWndOwner->AddINotify(this);
	}

	return true;
}

bool CDUICalendarCtrl::DoPaint(HDC hDC, bool bGenerateBmp)
{
	//calendar info
	if (0 == m_pBtnYearCtrl->GetTag())
	{
		InitCalendarInfo();
	}

	return __super::DoPaint(hDC, bGenerateBmp);
}

bool CDUICalendarCtrl::IsHeaderVisible()
{
	return m_AttributeHeaderVisible.GetValue();
}

void CDUICalendarCtrl::SetHeaderVisible(bool bVisible)
{
	if (bVisible == IsHeaderVisible()) return;

	m_AttributeHeaderVisible.SetValue(bVisible);

	InitCtrlInfo();

	Invalidate();

	return;
}

int CDUICalendarCtrl::GetHeaderHeight()
{
	return m_AttributeHeaderHeight.GetValue();
}

void CDUICalendarCtrl::SetHeaderHeight(int nHeight)
{
	if (nHeight == GetHeaderHeight()) return;

	m_AttributeHeaderHeight.SetValue(nHeight);

	InitCtrlInfo();

	Invalidate();

	return;
}

CDUISize CDUICalendarCtrl::GetLeftRightBtnSize()
{
	return m_AttributeLeftRightBtnSize.GetValue();
}

void CDUICalendarCtrl::SetLeftRightBtnSize(CDUISize szXY)
{
	if (szXY == GetLeftRightBtnSize()) return;

	m_AttributeLeftRightBtnSize.SetValue(szXY);

	InitCtrlInfo();

	Invalidate();

	return;
}

enDuiCalendarWeekType CDUICalendarCtrl::GetWeekStyle()
{
	return (enDuiCalendarWeekType)m_AttributeWeekStyle.GetCurSelItem();
}

void CDUICalendarCtrl::SetWeekStyle(enDuiCalendarWeekType WeekStyle)
{
	if (WeekStyle == GetWeekStyle()) return;

	m_AttributeWeekStyle.SelectItem(WeekStyle);

	InitCtrlInfo();

	Invalidate();

	return;
}

int CDUICalendarCtrl::GetWeekHeight()
{
	return m_AttributeWeekHeight.GetValue();
}

void CDUICalendarCtrl::SetWeekHeight(int nHeight)
{
	if (nHeight == GetWeekHeight()) return;

	m_AttributeWeekHeight.SetValue(nHeight);

	InitCtrlInfo();

	Invalidate();

	return;
}

tagDuiTextStyle CDUICalendarCtrl::GetTextStyleYearNormal()
{
	return m_AttributeTextStyleYearNormal.GetTextStyle();
}

tagDuiTextStyle CDUICalendarCtrl::GetTextStyleYearHot()
{
	return m_AttributeTextStyleYearHot.GetTextStyle();
}

tagDuiTextStyle CDUICalendarCtrl::GetTextStyleMonthNormal()
{
	return m_AttributeTextStyleMonthNormal.GetTextStyle();
}

tagDuiTextStyle CDUICalendarCtrl::GetTextStyleMonthHot()
{
	return m_AttributeTextStyleMonthHot.GetTextStyle();
}

tagDuiTextStyle CDUICalendarCtrl::GetTextStyleWeek()
{
	return m_AttributeTextStyleWeek.GetTextStyle();
}

tagDuiTextStyle CDUICalendarCtrl::GetTextStyleOtherMonthDay()
{
	return m_AttributeTextStyleOtherMonthDay.GetTextStyle();
}

tagDuiTextStyle CDUICalendarCtrl::GetTextStyleCurMonthDay()
{
	return m_AttributeTextStyleCurMonthDay.GetTextStyle();
}

tagDuiTextStyle CDUICalendarCtrl::GetTextStyleHotMonthDay()
{
	return m_AttributeTextStyleHotMonthDay.GetTextStyle();
}

tagDuiTextStyle CDUICalendarCtrl::GetTextStyleToday()
{
	return m_AttributeTextStyleToday.GetTextStyle();
}

void CDUICalendarCtrl::SetTextStyleYearNormal(const tagDuiTextStyle &TextStyle)
{
	if (TextStyle == GetTextStyleYearNormal()) return;

	m_AttributeTextStyleYearNormal.SetTextStyle(TextStyle);

	InitCtrlInfo();

	Invalidate();

	return;
}

void CDUICalendarCtrl::SetTextStyleYearHot(const tagDuiTextStyle &TextStyle)
{
	if (TextStyle == GetTextStyleYearHot()) return;

	m_AttributeTextStyleYearHot.SetTextStyle(TextStyle);

	InitCtrlInfo();

	Invalidate();

	return;
}

void CDUICalendarCtrl::SetTextStyleMonthNormal(const tagDuiTextStyle &TextStyle)
{
	if (TextStyle == GetTextStyleMonthNormal()) return;

	m_AttributeTextStyleMonthNormal.SetTextStyle(TextStyle);

	InitCtrlInfo();

	Invalidate();

	return;
}

void CDUICalendarCtrl::SetTextStyleMonthHot(const tagDuiTextStyle &TextStyle)
{
	if (TextStyle == GetTextStyleMonthHot()) return;

	m_AttributeTextStyleMonthHot.SetTextStyle(TextStyle);

	InitCtrlInfo();

	Invalidate();

	return;
}

void CDUICalendarCtrl::SetTextStyleWeek(const tagDuiTextStyle &TextStyle)
{
	if (TextStyle == GetTextStyleWeek()) return;

	m_AttributeTextStyleWeek.SetTextStyle(TextStyle);

	InitCtrlInfo();

	Invalidate();

	return;
}

void CDUICalendarCtrl::SetTextStyleOtherMonthDay(const tagDuiTextStyle &TextStyle)
{
	if (TextStyle == GetTextStyleOtherMonthDay()) return;

	m_AttributeTextStyleOtherMonthDay.SetTextStyle(TextStyle);

	InitCtrlInfo();

	Invalidate();

	return;
}

void CDUICalendarCtrl::SetTextStyleCurMonthDay(const tagDuiTextStyle &TextStyle)
{
	if (TextStyle == GetTextStyleCurMonthDay()) return;

	m_AttributeTextStyleCurMonthDay.SetTextStyle(TextStyle);

	InitCtrlInfo();

	Invalidate();

	return;
}

void CDUICalendarCtrl::SetTextStyleHotMonthDay(const tagDuiTextStyle &TextStyle)
{
	if (TextStyle == GetTextStyleHotMonthDay()) return;

	m_AttributeTextStyleHotMonthDay.SetTextStyle(TextStyle);

	InitCtrlInfo();

	Invalidate();

	return;
}

void CDUICalendarCtrl::SetTextStyleToday(const tagDuiTextStyle &TextStyle)
{
	if (TextStyle == GetTextStyleToday()) return;

	m_AttributeTextStyleToday.SetTextStyle(TextStyle);

	InitCtrlInfo();

	Invalidate();

	return;
}

tagDuiImageSection CDUICalendarCtrl::GetImageSectionLeftBtnNormal()
{
	return m_AttributeImageLeftBtnNormal.GetImageSection();
}

tagDuiImageSection CDUICalendarCtrl::GetImageSectionLeftBtnHot()
{
	return m_AttributeImageLeftBtnHot.GetImageSection();
}

tagDuiImageSection CDUICalendarCtrl::GetImageSectionRightBtnNormal()
{
	return m_AttributeImageRightBtnNormal.GetImageSection();
}

tagDuiImageSection CDUICalendarCtrl::GetImageSectionRightBtnHot()
{
	return m_AttributeImageRightBtnHot.GetImageSection();
}

tagDuiImageSection CDUICalendarCtrl::GetImageSectionHotDayBk()
{
	return m_AttributeImageHotDayBk.GetImageSection();
}

tagDuiImageSection CDUICalendarCtrl::GetImageSectionSelDayBk()
{
	return m_AttributeImageSelDayBk.GetImageSection();
}

tagDuiImageSection CDUICalendarCtrl::GetImageSectionTodayBk()
{
	return m_AttributeImageTodayBk.GetImageSection();
}

void CDUICalendarCtrl::SetImageSectionLeftBtnNormal(const tagDuiImageSection &ImageSection)
{
	if (ImageSection == GetImageSectionLeftBtnNormal()) return;

	m_AttributeImageLeftBtnNormal.SetImageSection(ImageSection);

	InitCtrlInfo();

	Invalidate();

	return;
}

void CDUICalendarCtrl::SetImageSectionLeftBtnHot(const tagDuiImageSection &ImageSection)
{
	if (ImageSection == GetImageSectionLeftBtnHot()) return;

	m_AttributeImageLeftBtnHot.SetImageSection(ImageSection);

	InitCtrlInfo();

	Invalidate();

	return;
}

void CDUICalendarCtrl::SetImageSectionRightBtnNormal(const tagDuiImageSection &ImageSection)
{
	if (ImageSection == GetImageSectionRightBtnNormal()) return;

	m_AttributeImageRightBtnNormal.SetImageSection(ImageSection);

	InitCtrlInfo();

	Invalidate();

	return;
}

void CDUICalendarCtrl::SetImageSectionRightBtnHot(const tagDuiImageSection &ImageSection)
{
	if (ImageSection == GetImageSectionRightBtnHot()) return;

	m_AttributeImageRightBtnHot.SetImageSection(ImageSection);

	InitCtrlInfo();

	Invalidate();

	return;
}

void CDUICalendarCtrl::SetImageSectionHotDayBk(const tagDuiImageSection &ImageSection)
{
	if (ImageSection == GetImageSectionHotDayBk()) return;

	m_AttributeImageHotDayBk.SetImageSection(ImageSection);

	InitCtrlInfo();

	Invalidate();

	return;
}

void CDUICalendarCtrl::SetImageSectionSelDayBk(const tagDuiImageSection &ImageSection)
{
	if (ImageSection == GetImageSectionSelDayBk()) return;

	m_AttributeImageSelDayBk.SetImageSection(ImageSection);

	InitCtrlInfo();

	Invalidate();

	return;
}

void CDUICalendarCtrl::SetImageSectionTodayBk(const tagDuiImageSection &ImageSection)
{
	if (ImageSection == GetImageSectionTodayBk()) return;

	m_AttributeImageTodayBk.SetImageSection(ImageSection);

	InitCtrlInfo();

	Invalidate();

	return;
}

vector<CMMString> CDUICalendarCtrl::GetColorResSwitchHotDayBk()
{
	return m_AttributeColorHotDayBk.GetColorResSwitch();
}

vector<CMMString> CDUICalendarCtrl::GetColorResSwitchSelDayBk()
{
	return m_AttributeColorSelDayBk.GetColorResSwitch();
}

vector<CMMString> CDUICalendarCtrl::GetColorResSwitchTodayBk()
{
	return m_AttributeColorTodayBk.GetColorResSwitch();
}

void CDUICalendarCtrl::SetColorResSwitchHotDayBk(const vector<CMMString> &vecResSwitch)
{
	if (vecResSwitch == GetColorResSwitchHotDayBk()) return;

	m_AttributeColorHotDayBk.SetColorResSwitch(vecResSwitch);

	InitCtrlInfo();

	Invalidate();

	return;
}

void CDUICalendarCtrl::SetColorResSwitchSelDayBk(const vector<CMMString> &vecResSwitch)
{
	if (vecResSwitch == GetColorResSwitchSelDayBk()) return;

	m_AttributeColorSelDayBk.SetColorResSwitch(vecResSwitch);

	InitCtrlInfo();

	Invalidate();

	return;
}

void CDUICalendarCtrl::SetColorResSwitchTodayBk(const vector<CMMString> &vecResSwitch)
{
	if (vecResSwitch == GetColorResSwitchTodayBk()) return;

	m_AttributeColorTodayBk.SetColorResSwitch(vecResSwitch);

	InitCtrlInfo();

	Invalidate();

	return;
}

tagDuiCalendarCmd CDUICalendarCtrl::GetCalendarCur()
{
	return m_CalendarCur;
}

void CDUICalendarCtrl::SetCalendarCur(tagDuiCalendarCmd CalendarCmd)
{
	//switch
	CDUIListViewCtrl *pListCurCtrl = m_pListDayCtrl1->IsVisible() ? m_pListDayCtrl1 : m_pListDayCtrl2;
	if (NULL == pListCurCtrl) return;

	int nYear = ParseYear(pListCurCtrl->GetTag()), nMonth = ParseMonth(pListCurCtrl->GetTag());
	if (CalendarCmd.tDate.wYear != nYear 
		|| CalendarCmd.tDate.wMonth != nMonth
		|| m_CalendarCur.tDate.wYear != CalendarCmd.tDate.wYear
		|| m_CalendarCur.tDate.wMonth != CalendarCmd.tDate.wMonth
		|| m_CalendarCur.tDate.wDay != CalendarCmd.tDate.wDay)
	{
		m_CalendarCur = CalendarCmd;
		SwitchYearMonth(CalendarCmd.tDate.wYear, CalendarCmd.tDate.wMonth);

		return;
	}

	//select
	m_CalendarCur = CalendarCmd;
	pListCurCtrl->UnSelectAllItems();
	if (m_CalendarCur.bSelected)
	{
		PerformSelectDay(pListCurCtrl, m_CalendarCur.tDate.wYear, m_CalendarCur.tDate.wMonth, m_CalendarCur.tDate.wDay);
	}

	return;
}

void CDUICalendarCtrl::SwitchYearMonth(bool bPre)
{
	if (NULL == m_pTabSwitchCtrl || NULL == m_pListDayCtrl1 || NULL == m_pListDayCtrl2) return;

	CDUIListViewCtrl *pListCurCtrl = m_pListDayCtrl1->IsVisible() ? m_pListDayCtrl1 : m_pListDayCtrl2;
	CDUIListViewCtrl *pListJumpCtrl = pListCurCtrl == m_pListDayCtrl1 ? m_pListDayCtrl2 : m_pListDayCtrl1;
	CDUIVerticalLayoutCtrl *pVertJumpCtrl = pListCurCtrl == m_pListDayCtrl1 ? m_pVertPageCtrl2 : m_pVertPageCtrl1;
	int nYear = ParseYear(pListCurCtrl->GetTag()), nMonth = ParseMonth(pListCurCtrl->GetTag());

	if (bPre)
	{
		int nYearPre = 0, nMonthPre = 0;
		if (GetPreYearMonth(nYear, nMonth, nYearPre, nMonthPre))
		{
			ConstructHeader(nYearPre, nMonthPre);
			ConstructCalendar(pListJumpCtrl, nYearPre, nMonthPre);

			m_pTabSwitchCtrl->SetChildIndex(pVertJumpCtrl, 0);
			m_pTabSwitchCtrl->Select(0);
		}

		return;
	}

	int nYearNext = 0, nMonthNext = 0;
	if (GetNextYearMonth(nYear, nMonth, nYearNext, nMonthNext))
	{
		ConstructHeader(nYearNext, nMonthNext);
		ConstructCalendar(pListJumpCtrl, nYearNext, nMonthNext);

		m_pTabSwitchCtrl->SetChildIndex(pVertJumpCtrl, 1);
		m_pTabSwitchCtrl->Select(1);
	}

	return;
}

void CDUICalendarCtrl::SwitchYearMonth(int nYear, int nMonth)
{
	if (NULL == m_pTabSwitchCtrl || NULL == m_pListDayCtrl1 || NULL == m_pListDayCtrl2) return;

	CDUIListViewCtrl *pListCurCtrl = m_pListDayCtrl1->IsVisible() ? m_pListDayCtrl1 : m_pListDayCtrl2;
	CDUIListViewCtrl *pListJumpCtrl = pListCurCtrl == m_pListDayCtrl1 ? m_pListDayCtrl2 : m_pListDayCtrl1;
	CDUIVerticalLayoutCtrl *pVertJumpCtrl = pListCurCtrl == m_pListDayCtrl1 ? m_pVertPageCtrl2 : m_pVertPageCtrl1;

	ConstructHeader(nYear, nMonth);
	ConstructCalendar(pListJumpCtrl, nYear, nMonth);

	m_pTabSwitchCtrl->SetChildIndex(pVertJumpCtrl, 0);
	m_pTabSwitchCtrl->Select(0);

	return;
}

CMMString CDUICalendarCtrl::GetMondayText()
{
	return m_AttributeTextWeekDay[0].GetValue();
}

CMMString CDUICalendarCtrl::GetTuesdayText()
{
	return m_AttributeTextWeekDay[1].GetValue();
}

CMMString CDUICalendarCtrl::GetWednesdayText()
{
	return m_AttributeTextWeekDay[2].GetValue();
}

CMMString CDUICalendarCtrl::GetThursdayText()
{
	return m_AttributeTextWeekDay[3].GetValue();
}

CMMString CDUICalendarCtrl::GetFridayText()
{
	return m_AttributeTextWeekDay[4].GetValue();
}

CMMString CDUICalendarCtrl::GetSaturdayText()
{
	return m_AttributeTextWeekDay[5].GetValue();
}

CMMString CDUICalendarCtrl::GetSundayText()
{
	return m_AttributeTextWeekDay[6].GetValue();
}

CMMString CDUICalendarCtrl::GetWeekText(int nWeekDay)
{
	return nWeekDay < MMCountArray(m_AttributeTextWeekDay) ? m_AttributeTextWeekDay[nWeekDay].GetValue() : _T("");
}

void CDUICalendarCtrl::AttatchConstructDayCallback(OnConstructDay pOnConstructDay)
{
	m_pConstructDayCallBack = (pOnConstructDay);

	return;
}

bool CDUICalendarCtrl::InsertChild(CDUIControlBase *pChild, int nPos)
{
	return false;
}

void CDUICalendarCtrl::InitProperty()
{
	__super::InitProperty();

	//create
	DuiCreateGroupAttribute(m_AttributeGroupHeader, _T("Header"));
	DuiCreateAttribute(m_AttributeHeaderVisible, _T("HeaderVisible"), _T(""), m_AttributeGroupHeader);
	DuiCreateAttribute(m_AttributeHeaderHeight, _T("HeaderHeight"), _T(""), m_AttributeGroupHeader);
	DuiCreateAttribute(m_AttributeLeftRightBtnSize, _T("LeftRightBtnSize"), _T(""), m_AttributeGroupHeader);
	DuiCreateAttribute(m_AttributeTextStyleYearNormal, _T("TextStyleYearNormal"), _T(""), m_AttributeGroupHeader);
	DuiCreateAttribute(m_AttributeTextStyleYearHot, _T("TextStyleYearHot"), _T(""), m_AttributeGroupHeader);
	DuiCreateAttribute(m_AttributeTextStyleMonthNormal, _T("TextStyleMonthNormal"), _T(""), m_AttributeGroupHeader);
	DuiCreateAttribute(m_AttributeTextStyleMonthHot, _T("TextStyleMonthHot"), _T(""), m_AttributeGroupHeader);
	DuiCreateAttribute(m_AttributeColorYearNormal, _T("ColorYearNormal"), _T(""), m_AttributeGroupHeader);
	DuiCreateAttribute(m_AttributeColorYearHot, _T("ColorYearHot"), _T(""), m_AttributeGroupHeader);
	DuiCreateAttribute(m_AttributeColorMonthNormal, _T("ColorMonthNormal"), _T(""), m_AttributeGroupHeader);
	DuiCreateAttribute(m_AttributeColorMonthHot, _T("ColorMonthHot"), _T(""), m_AttributeGroupHeader);
	DuiCreateAttribute(m_AttributeImageLeftBtnNormal, _T("ImageLeftBtnNormal"), _T(""), m_AttributeGroupHeader);
	DuiCreateAttribute(m_AttributeImageLeftBtnHot, _T("ImageLeftBtnHot"), _T(""), m_AttributeGroupHeader);
	DuiCreateAttribute(m_AttributeImageRightBtnNormal, _T("ImageRightBtnNormal"), _T(""), m_AttributeGroupHeader);
	DuiCreateAttribute(m_AttributeImageRightBtnHot, _T("ImageRightBtnHot"), _T(""), m_AttributeGroupHeader);

	DuiCreateGroupAttribute(m_AttributeGroupDays, _T("Days"));
	DuiCreateAttribute(m_AttributeWeekStyle, _T("WeekStyle"), _T(""), m_AttributeGroupDays);
	DuiCreateAttribute(m_AttributeWeekHeight, _T("WeekHeight"), _T(""), m_AttributeGroupDays);
	DuiCreateAttribute(m_AttributeColorHotDayBk, _T("ColorHotDayBk"), _T(""), m_AttributeGroupDays);
	DuiCreateAttribute(m_AttributeColorSelDayBk, _T("ColorSelDayBk"), _T(""), m_AttributeGroupDays);
	DuiCreateAttribute(m_AttributeColorTodayBk, _T("ColorTodayBk"), _T(""), m_AttributeGroupDays);
	DuiCreateAttribute(m_AttributeImageHotDayBk, _T("ImageHotDayBk"), _T(""), m_AttributeGroupDays);
	DuiCreateAttribute(m_AttributeImageSelDayBk, _T("ImageSelDayBk"), _T(""), m_AttributeGroupDays);
	DuiCreateAttribute(m_AttributeImageTodayBk, _T("ImageTodayBk"), _T(""), m_AttributeGroupDays);
	DuiCreateAttribute(m_AttributeTextStyleWeek, _T("TextStyleWeek"), _T(""), m_AttributeGroupDays);
	DuiCreateAttribute(m_AttributeTextStyleOtherMonthDay, _T("TextStyleOtherMonthDay"), _T(""), m_AttributeGroupDays);
	DuiCreateAttribute(m_AttributeTextStyleCurMonthDay, _T("TextStyleCurMonthDay"), _T(""), m_AttributeGroupDays);
	DuiCreateAttribute(m_AttributeTextStyleHotMonthDay, _T("TextStyleHotMonthDay"), _T(""), m_AttributeGroupDays);
	DuiCreateAttribute(m_AttributeTextStyleToday, _T("TextStyleToday"), _T(""), m_AttributeGroupDays);
	for (int n = 0; n < Dui_Count_WeekDay; n++)
	{
		DuiCreateAttribute(m_AttributeTextWeekDay[n], m_szWeekDayAttriName[n], m_szWeekDayAttriName[n], m_AttributeGroupDays);
	}

	return;
}

void CDUICalendarCtrl::InitComplete()
{
	__super::InitComplete();

	DuiInitAttriVisible(m_AttributeAutoCalcWidth, false);
	DuiInitAttriVisible(m_AttributeAutoCalcHeight, false);
	DuiInitAttriVisible(m_AttributeAnimateDrag, false);
	DuiInitAttriVisible(m_AttributeGroupScroll, false);
	DuiInitAttriVisible(m_AttributeGroupChild, false);

	EnableScrollBar(false, false);

	//init ctrl
	InitChildren();

	//ctrl info
	InitCtrlInfo();

	return;
}

bool CDUICalendarCtrl::SaveAttribute(tinyxml2::XMLElement *pNode, bool bIncludeChild)
{
	return __super::SaveAttribute(pNode, false);
}

void CDUICalendarCtrl::OnDuiMouseWheel(const DuiNotify &Notify)
{
	if (Notify.pNotifyCtrl != m_pListDayCtrl1 && Notify.pNotifyCtrl != m_pListDayCtrl2) return;

	//direction
	int nWheelDelta = (int)(short)HIWORD(Notify.wParam);
	int nWheelCount = abs(nWheelDelta / WHEEL_DELTA);
	bool bPositive = nWheelDelta > 0;

	if (bPositive)
	{
		SwitchYearMonth(true);
	}
	else
	{
		SwitchYearMonth(false);
	}

	return;
}

void CDUICalendarCtrl::OnDuiItemSelected(const DuiNotify &Notify)
{
	if (NULL == m_pListDayCtrl1 || NULL == m_pListDayCtrl2) return;
	if (m_pListDayCtrl1 != Notify.pNotifyCtrl && m_pListDayCtrl2 != Notify.pNotifyCtrl) return;

	CDUIListViewCtrl *pListDays = m_pListDayCtrl1 == Notify.pNotifyCtrl ? m_pListDayCtrl1 : m_pListDayCtrl2;
	CDUIListItemCtrl *pDay = pListDays->GetChildAt(pListDays->GetCurSel());

	if (NULL == pDay) return;

	m_CalendarCur.bSelected = true;
	m_CalendarCur.tDate.wYear = ParseYear(pDay->GetTag());
	m_CalendarCur.tDate.wMonth = ParseMonth(pDay->GetTag());
	m_CalendarCur.tDate.wDay = ParseDay(pDay->GetTag());
	m_pWndOwner->SendNotify(this, DuiNotify_DateSelectChanged);

	return;
}

void CDUICalendarCtrl::OnDuiItemClick(const DuiNotify &Notify)
{
	if (NULL == m_pListDayCtrl1 || NULL == m_pListDayCtrl2) return;
	if (m_pListDayCtrl1 != Notify.pNotifyCtrl && m_pListDayCtrl2 != Notify.pNotifyCtrl) return;

	CDUIListViewCtrl *pListDays = m_pListDayCtrl1 == Notify.pNotifyCtrl ? m_pListDayCtrl1 : m_pListDayCtrl2;
	CDUIListItemCtrl *pDay = pListDays->GetChildAt(pListDays->GetCurSel());

	if (NULL == pDay) return;

	m_CalendarCur.bSelected = true;
	m_CalendarCur.tDate.wYear = ParseYear(pDay->GetTag());
	m_CalendarCur.tDate.wMonth = ParseMonth(pDay->GetTag());
	m_CalendarCur.tDate.wDay = ParseDay(pDay->GetTag());
	g_DuiCalendarCmd = m_CalendarCur;

	if (g_pDuiCalendarWnd)
	{
		g_pDuiCalendarWnd->Close();
	}

	m_pWndOwner->SendNotify(this, DuiNotify_DateSelectChanged);

	return;
}

void CDUICalendarCtrl::OnDuiTabSelectChanged(const DuiNotify &Notify)
{
	if (NULL == Notify.pNotifyCtrl || m_pTabSwitchCtrl != Notify.pNotifyCtrl) return;

	m_pWndOwner->SendNotify(this, DuiNotify_CalendarSwitchChanged);

	return;
}

void CDUICalendarCtrl::OnDuiBtnLeft(const DuiNotify &Notify)
{
	if (m_pBtnLeftCtrl != Notify.pNotifyCtrl) return;

	SwitchYearMonth(true);

	return;
}

void CDUICalendarCtrl::OnDuiBtnRight(const DuiNotify &Notify)
{
	if (m_pBtnRightCtrl != Notify.pNotifyCtrl) return;

	SwitchYearMonth(false);

	return;
}

void CDUICalendarCtrl::OnDuiBtnYear(const DuiNotify &Notify)
{
	if (NULL == m_pWndOwner
		|| NULL == m_pBtnYearCtrl
		|| Notify.pNotifyCtrl != m_pBtnYearCtrl) return;

	PopupYearMonthWnd(true);

	return;
}

void CDUICalendarCtrl::OnDuiBtnMonth(const DuiNotify &Notify)
{
	if (NULL == m_pWndOwner
		|| NULL == m_pBtnMonthCtrl
		|| Notify.pNotifyCtrl != m_pBtnMonthCtrl) return;

	PopupYearMonthWnd(false);
	
	return;
}

void CDUICalendarCtrl::InitChildren()
{
	m_pHorizHeaderCtrl = new CDUIHorizontalLayoutCtrl;
	m_pBtnLeftCtrl = new CDUIButtonCtrl;
	m_pBtnYearCtrl = new CDUIButtonCtrl;
	m_pBtnMonthCtrl = new CDUIButtonCtrl;
	m_pBtnRightCtrl = new CDUIButtonCtrl;
	CDUIControlBase *pControlLeft = new CDUIControlBase;
	CDUIControlBase *pControlRight = new CDUIControlBase;
	m_pHorizHeaderCtrl->Init();
	m_pBtnLeftCtrl->Init();
	m_pBtnYearCtrl->Init();
	m_pBtnMonthCtrl->Init();
	m_pBtnRightCtrl->Init();
	pControlLeft->Init();
	pControlRight->Init();
	m_pHorizHeaderCtrl->InsertChild(m_pBtnLeftCtrl);
	m_pHorizHeaderCtrl->InsertChild(pControlLeft);
	m_pHorizHeaderCtrl->InsertChild(m_pBtnYearCtrl);
	m_pHorizHeaderCtrl->InsertChild(m_pBtnMonthCtrl);
	m_pHorizHeaderCtrl->InsertChild(pControlRight);
	m_pHorizHeaderCtrl->InsertChild(m_pBtnRightCtrl);
	__super::InsertChild(m_pHorizHeaderCtrl);

	m_pTabSwitchCtrl = new CDUITabLayoutCtrl;
	m_pVertPageCtrl1 = new CDUIVerticalLayoutCtrl;
	m_pVertPageCtrl2 = new CDUIVerticalLayoutCtrl;
	m_pHorizWeekCtrl1 = new CDUIHorizontalLayoutCtrl;
	m_pHorizWeekCtrl2 = new CDUIHorizontalLayoutCtrl;
	m_pListDayCtrl1 = new CDUIListViewCtrl;
	m_pListDayCtrl2 = new CDUIListViewCtrl;
	m_pTabSwitchCtrl->Init();
	m_pVertPageCtrl1->Init();
	m_pVertPageCtrl2->Init();
	m_pHorizWeekCtrl1->Init();
	m_pHorizWeekCtrl2->Init();
	m_pListDayCtrl1->Init();
	m_pListDayCtrl2->Init();
	m_pListDayCtrl1->RegisterControlCallBack(this);
	m_pListDayCtrl2->RegisterControlCallBack(this);
	m_pVertPageCtrl1->InsertChild(m_pHorizWeekCtrl1);
	m_pVertPageCtrl1->InsertChild(m_pListDayCtrl1);
	m_pVertPageCtrl2->InsertChild(m_pHorizWeekCtrl2);
	m_pVertPageCtrl2->InsertChild(m_pListDayCtrl2);
	m_pTabSwitchCtrl->InsertChild(m_pVertPageCtrl1);
	m_pTabSwitchCtrl->InsertChild(m_pVertPageCtrl2);
	__super::InsertChild(m_pTabSwitchCtrl);

	return;
}

void CDUICalendarCtrl::InitCtrlInfo()
{
	if (NULL == m_pHorizHeaderCtrl
		|| NULL == m_pBtnLeftCtrl
		|| NULL == m_pBtnRightCtrl
		|| NULL == m_pBtnYearCtrl
		|| NULL == m_pBtnMonthCtrl
		|| NULL == m_pHorizWeekCtrl1
		|| NULL == m_pHorizWeekCtrl2
		|| NULL == m_pListDayCtrl1
		|| NULL == m_pListDayCtrl2) return;

	//attribute
	{
		m_pHorizHeaderCtrl->EnableScrollBar(false, false);
		m_pHorizHeaderCtrl->SetPadding(CALENDAR_OFFSET_LRBTN, 0, CALENDAR_OFFSET_LRBTN, 0);
		m_pHorizHeaderCtrl->SetFixedHeight(GetHeaderHeight());
		m_pHorizHeaderCtrl->SetVisible(IsHeaderVisible());
	}
	{
		m_pBtnLeftCtrl->SetCtrlID(Dui_CtrlIDInner_CalendarBtnLeft);
		m_pBtnLeftCtrl->SetPadding(0, 0, 0, 0);
		m_pBtnLeftCtrl->SetPaddingVertCenter();
		m_pBtnLeftCtrl->SetFixedWidth(GetLeftRightBtnSize().cx);
		m_pBtnLeftCtrl->SetFixedHeight(GetLeftRightBtnSize().cy);
		m_pBtnLeftCtrl->SetImageSectionNormal(m_AttributeImageLeftBtnNormal.GetImageSection());
		m_pBtnLeftCtrl->SetImageSectionHot(m_AttributeImageLeftBtnHot.GetImageSection());
	}
	{
		m_pBtnRightCtrl->SetCtrlID(Dui_CtrlIDInner_CalendarBtnRight);
		m_pBtnRightCtrl->SetPadding(0, 0, 0, 0);
		m_pBtnRightCtrl->SetPaddingVertCenter();
		m_pBtnRightCtrl->SetFixedWidth(GetLeftRightBtnSize().cx);
		m_pBtnRightCtrl->SetFixedHeight(GetLeftRightBtnSize().cy);
		m_pBtnRightCtrl->SetImageSectionNormal(m_AttributeImageRightBtnNormal.GetImageSection());
		m_pBtnRightCtrl->SetImageSectionHot(m_AttributeImageRightBtnHot.GetImageSection());
	}
	{
		m_pBtnYearCtrl->SetCtrlID(Dui_CtrlIDInner_CalendarBtnYear);
		m_pBtnYearCtrl->SetAutoCalcWidth(true);
		m_pBtnYearCtrl->SetAutoCalcHeight(true);
		m_pBtnYearCtrl->SetPaddingVertCenter();
		m_pBtnYearCtrl->SetTextStyleNormal(m_AttributeTextStyleYearNormal.GetTextStyle());
		m_pBtnYearCtrl->SetTextStyleHot(m_AttributeTextStyleYearHot.GetTextStyle());
		m_pBtnYearCtrl->SetColorResSwitchNormal(m_AttributeColorYearNormal.GetColorResSwitch());
		m_pBtnYearCtrl->SetColorResSwitchHot(m_AttributeColorYearHot.GetColorResSwitch());

		m_pBtnMonthCtrl->SetCtrlID(Dui_CtrlIDInner_CalendarBtnMonth);
		m_pBtnMonthCtrl->SetAutoCalcWidth(true);
		m_pBtnMonthCtrl->SetAutoCalcHeight(true);
		m_pBtnMonthCtrl->SetPaddingVertCenter();
		m_pBtnMonthCtrl->SetTextStyleNormal(m_AttributeTextStyleMonthNormal.GetTextStyle());
		m_pBtnMonthCtrl->SetTextStyleHot(m_AttributeTextStyleMonthHot.GetTextStyle());
		m_pBtnMonthCtrl->SetColorResSwitchNormal(m_AttributeColorMonthNormal.GetColorResSwitch());
		m_pBtnMonthCtrl->SetColorResSwitchHot(m_AttributeColorMonthHot.GetColorResSwitch());
	}
	{
		m_pHorizWeekCtrl1->EnableScrollBar(false, false);
		m_pHorizWeekCtrl1->SetFixedHeight(GetWeekHeight());
		if (m_pHorizWeekCtrl1->GetChildCount() <= 0)
		{
			for (int n = 0; n < Dui_Count_WeekDay; n++)
			{
				CDUIStaticCtrl *pWeek = new CDUIStaticCtrl;
				pWeek->Init();
				m_pHorizWeekCtrl1->InsertChild(pWeek);
			}
		}
		for (int n = 0; n < m_pHorizWeekCtrl1->GetChildCount(); n++)
		{
			MMInterfaceHelper(CDUIStaticCtrl, m_pHorizWeekCtrl1->GetChildAt(n), pWeek);
			if (NULL == pWeek) continue;

			pWeek->SetText(GetWeekText(n));
			pWeek->SetTextStyle(m_AttributeTextStyleWeek.GetTextStyle());
		}
	}
	{
		m_pHorizWeekCtrl2->EnableScrollBar(false, false);
		m_pHorizWeekCtrl2->SetFixedHeight(GetWeekHeight());
		if (m_pHorizWeekCtrl2->GetChildCount() <= 0)
		{
			for (int n = 0; n < Dui_Count_WeekDay; n++)
			{
				CDUIStaticCtrl *pWeek = new CDUIStaticCtrl;
				pWeek->Init();
				m_pHorizWeekCtrl2->InsertChild(pWeek);
			}
		}
		for (int n = 0; n < m_pHorizWeekCtrl2->GetChildCount(); n++)
		{
			MMInterfaceHelper(CDUIStaticCtrl, m_pHorizWeekCtrl2->GetChildAt(n), pWeek);
			if (NULL == pWeek) continue;

			pWeek->SetText(GetWeekText(n));
			pWeek->SetTextStyle(m_AttributeTextStyleWeek.GetTextStyle());
		}
	}
	{
		m_pListDayCtrl1->SetCtrlID(Dui_CtrlIDInner_CalendarListDay);
		m_pListDayCtrl1->EnableScrollBar(false, false);
		m_pListDayCtrl1->SetPadding(0, 0, 0, 0);
		m_pListDayCtrl1->SwitchListViewType(ListView_TileH);
		m_pListDayCtrl1->SetItemStatusColorResSwitchHot(m_AttributeColorHotDayBk.GetColorResSwitch());
		m_pListDayCtrl1->SetItemStatusColorResSwitchSelNormal(m_AttributeColorSelDayBk.GetColorResSwitch());
		m_pListDayCtrl1->SetItemStatusColorResSwitchSelHot(m_AttributeColorSelDayBk.GetColorResSwitch());
		m_pListDayCtrl1->SetItemStatusImageSectionHot(m_AttributeImageHotDayBk.GetImageSection());
		m_pListDayCtrl1->SetItemStatusImageSectionSelNormal(m_AttributeImageSelDayBk.GetImageSection());
		m_pListDayCtrl1->SetItemStatusImageSectionSelHot(m_AttributeImageSelDayBk.GetImageSection());
		m_pListDayCtrl1->SetItemTextStyleNormal(m_AttributeTextStyleCurMonthDay.GetTextStyle());
		m_pListDayCtrl1->SetItemTextStyleHot(m_AttributeTextStyleHotMonthDay.GetTextStyle());
		m_pListDayCtrl1->SetItemTextStyleSelHot(m_AttributeTextStyleHotMonthDay.GetTextStyle());
		m_pListDayCtrl1->SetAutoCalcItemWidth(true);
		m_pListDayCtrl1->SetAutoCalcItemHeight(true);
		for (int n = 0; n < m_pListDayCtrl1->GetChildCount(); n++)
		{
			CDUIListItemCtrl *pDay = m_pListDayCtrl1->GetChildAt(n);
			if (NULL == pDay) continue;

			ConstructDay(pDay, ParseYear(pDay->GetTag()), ParseMonth(pDay->GetTag()), ParseDay(pDay->GetTag()));
		}
	}
	{
		m_pListDayCtrl2->SetCtrlID(Dui_CtrlIDInner_CalendarListDay);
		m_pListDayCtrl2->EnableScrollBar(false, false);
		m_pListDayCtrl2->SetPadding(0, 0, 0, 0);
		m_pListDayCtrl2->SwitchListViewType(ListView_TileH);
		m_pListDayCtrl2->SetItemStatusColorResSwitchHot(m_AttributeColorHotDayBk.GetColorResSwitch());
		m_pListDayCtrl2->SetItemStatusColorResSwitchSelNormal(m_AttributeColorSelDayBk.GetColorResSwitch());
		m_pListDayCtrl2->SetItemStatusColorResSwitchSelHot(m_AttributeColorSelDayBk.GetColorResSwitch());
		m_pListDayCtrl2->SetItemStatusImageSectionHot(m_AttributeImageHotDayBk.GetImageSection());
		m_pListDayCtrl2->SetItemStatusImageSectionSelNormal(m_AttributeImageSelDayBk.GetImageSection());
		m_pListDayCtrl2->SetItemStatusImageSectionSelHot(m_AttributeImageSelDayBk.GetImageSection());
		m_pListDayCtrl2->SetItemTextStyleNormal(m_AttributeTextStyleCurMonthDay.GetTextStyle());
		m_pListDayCtrl2->SetItemTextStyleHot(m_AttributeTextStyleHotMonthDay.GetTextStyle());
		m_pListDayCtrl2->SetItemTextStyleSelHot(m_AttributeTextStyleHotMonthDay.GetTextStyle());
		m_pListDayCtrl2->SetAutoCalcItemWidth(true);
		m_pListDayCtrl2->SetAutoCalcItemHeight(true);
		for (int n = 0; n < m_pListDayCtrl2->GetChildCount(); n++)
		{
			CDUIListItemCtrl *pDay = m_pListDayCtrl2->GetChildAt(n);
			if (NULL == pDay) continue;

			ConstructDay(pDay, ParseYear(pDay->GetTag()), ParseMonth(pDay->GetTag()), ParseDay(pDay->GetTag()));
		}
	}
	{
		m_pVertPageCtrl1->EnableScrollBar(false, false);
		m_pVertPageCtrl2->EnableScrollBar(false, false);
		m_pTabSwitchCtrl->EnableScrollBar(false, false);
	}

	return;
}

void CDUICalendarCtrl::InitCalendarInfo()
{
	if (NULL == m_pTabSwitchCtrl || NULL == m_pListDayCtrl1 || NULL == m_pListDayCtrl2) return;

	//time
	GetLocalTime(&m_CalendarCur.tDate);

	//cur month
	ConstructHeader(m_CalendarCur.tDate.wYear, m_CalendarCur.tDate.wMonth);
	ConstructCalendar(m_pListDayCtrl1, m_CalendarCur.tDate.wYear, m_CalendarCur.tDate.wMonth);

	m_pTabSwitchCtrl->Select(m_pVertPageCtrl1);

	return;
}

int CDUICalendarCtrl::ParseYear(DWORD dwDate)
{
	return dwDate >> 16;
}

int CDUICalendarCtrl::ParseMonth(DWORD dwDate)
{
	return (BYTE)(dwDate >> 8);
}

int CDUICalendarCtrl::ParseDay(DWORD dwDate)
{
	return (BYTE)dwDate;
}

DWORD CDUICalendarCtrl::ConstructDate(int nYear, int nMonth, int nDay)
{
	return ((WORD)nYear << 16) | ((BYTE)nMonth << 8) | (BYTE)(nDay);
}

void CDUICalendarCtrl::ConstructHeader(int nYear, int nMonth)
{
	if (NULL == m_pBtnYearCtrl || NULL == m_pBtnMonthCtrl) return;

	m_pBtnYearCtrl->SetText(CMMStrHelp::Format(_T("%d年"), nYear));
	m_pBtnYearCtrl->SetTag(ConstructDate(nYear, nMonth, 1));
	m_pBtnMonthCtrl->SetText(CMMStrHelp::Format(_T("%d月"), nMonth));
	m_pBtnMonthCtrl->SetTag(ConstructDate(nYear, nMonth, 1));

	return;
}

void CDUICalendarCtrl::ConstructDay(CDUIListItemCtrl *pDay, int nYear, int nMonth, int nDay)
{
	if (NULL == pDay) return;

	//today
	SYSTEMTIME LocalTime = {};
	GetLocalTime(&LocalTime);

	bool bCurMonth = (LocalTime.wYear == nYear && LocalTime.wMonth == nMonth);
	bool bCurDay = bCurMonth && (LocalTime.wDay == nDay);

	if (false == bCurMonth)
	{
		pDay->SetTextStyle(m_AttributeTextStyleOtherMonthDay.GetTextStyle());
	}
	if (bCurDay)
	{
		pDay->SetTextStyle(m_AttributeTextStyleToday.GetTextStyle());
		pDay->SetBkColor(m_AttributeColorTodayBk.GetColorResSwitch());
		pDay->SetBkImageSection(m_AttributeImageTodayBk.GetImageSection());
	}
	pDay->SetText(CMMStrHelp::Format(_T("%d"), nDay));
	pDay->SetTag(ConstructDate(nYear, nMonth, nDay));
	pDay->EnableScrollBar(false, false);

	//callback
	if (m_pConstructDayCallBack)
	{
		m_pConstructDayCallBack(pDay, nYear, nMonth, nDay);
	}

	return;
}

void CDUICalendarCtrl::ConstructCalendar(CDUIListViewCtrl *pListDays, int nYear, int nMonth)
{
	if (NULL == pListDays) return;

	pListDays->RemoveAll();
	pListDays->SetTag(ConstructDate(nYear, nMonth, 1));

	//today
	SYSTEMTIME LocalTime = {};
	GetLocalTime(&LocalTime);

	//pre month
	int nYearPre = 0, nMonthPre = 0;
	GetPreYearMonth(nYear, nMonth, nYearPre, nMonthPre);
	int nWeekDay = GetWeekDay(nYear, nMonth, 1);
	int nPreMonthDays = GetMonthDays(nYearPre, nMonthPre);
	int nPreWeekDays = 1 == nWeekDay ? Dui_Count_WeekDay : nWeekDay - 1;
	for (int nDay = nPreMonthDays - nPreWeekDays + 1; nDay <= nPreMonthDays; nDay++)
	{
		CDUIListItemCtrl *pDay = pListDays->ProductItemDefault();
		if (NULL == pDay) continue;

		ConstructDay(pDay, nYearPre, nMonthPre, nDay);

		pListDays->InsertChild(pDay);
	}

	//cur month
	int nCurMonthDays = GetMonthDays(nYear, nMonth);
	for (int nDay = 1; nDay <= nCurMonthDays; nDay++)
	{
		CDUIListItemCtrl *pDay = pListDays->ProductItemDefault();
		if (NULL == pDay) continue;

		ConstructDay(pDay, nYear, nMonth, nDay);

		pListDays->InsertChild(pDay);
	}

	//next month
	int nYearNext = 0, nMonthNext = 0;
	GetNextYearMonth(nYear, nMonth, nYearNext, nMonthNext);
	int nNextWeekDays = CALENDAR_COUNT_DAYROW * Dui_Count_WeekDay - nPreWeekDays - nCurMonthDays;
	for (int nDay = 1; nDay <= nNextWeekDays; nDay++)
	{
		CDUIListItemCtrl *pDay = pListDays->ProductItemDefault();
		if (NULL == pDay) continue;

		ConstructDay(pDay, nYearNext, nMonthNext, nDay);

		pListDays->InsertChild(pDay);
	}

	//select
	pListDays->UnSelectAllItems();
	if (m_CalendarCur.bSelected)
	{
		PerformSelectDay(pListDays, m_CalendarCur.tDate.wYear, m_CalendarCur.tDate.wMonth, m_CalendarCur.tDate.wDay);
	}

	return;
}

bool CDUICalendarCtrl::IsLeapYear(int nYear)
{
	return 0 == nYear % 400 || (0 == nYear % 4 && 0 != nYear % 100);
}

int CDUICalendarCtrl::GetWeekDay(int nYear, int nMonth, int nDay)
{
	if (nYear < CALENDAR_YEAR_START || nMonth <= 0 || nMonth > 12 || nDay <= 0 || nDay > 31) return -1;

	int nDays = GetYearDays(nYear, nMonth, nDay);
	for (int n = CALENDAR_YEAR_START; n < nYear; n++)
	{
		nDays += IsLeapYear(n) ? 366 : 365;
	}

	return 1 + (nDays - 1) % 7;
}

int CDUICalendarCtrl::GetYearDays(int nYear, int nMonth, int nDay)
{
	bool bLeapYear = IsLeapYear(nYear);

	int nDays = 0;
	int nDaysFebruary = bLeapYear ? 29 : 28;
	switch (nMonth)
	{
		case 12: nDays += 30;
		case 11: nDays += 31;
		case 10: nDays += 30;
		case 9: nDays += 31;
		case 8: nDays += 31;
		case 7: nDays += 30;
		case 6: nDays += 31;
		case 5: nDays += 30;
		case 4: nDays += 31;
		case 3: nDays += nDaysFebruary;
		case 2: nDays += 31;
		case 1: break;
	}

	nDays += nDay;

	return nDays;
}

int CDUICalendarCtrl::GetMonthDays(int nYear, int nMonth)
{
	bool bLeapYear = IsLeapYear(nYear);
	switch (nMonth)
	{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
		{
			return 31;
		}
		case 4:
		case 6:
		case 9:
		case 11:
		{
			return 30;
		}
		case 2:
		{
			return bLeapYear ? 29 : 28;
		}
	}

	return 0;
}

bool CDUICalendarCtrl::GetPreYearMonth(IN int nYear, IN int nMonth, OUT int &nYearOut, OUT int &nMonthOut)
{
	nMonthOut = 1 == nMonth ? 12 : nMonth - 1;
	nYearOut = 1 == nMonth ? nYear - 1 : nYear;

	if (nYear < CALENDAR_YEAR_START) return false;

	return true;
}

bool CDUICalendarCtrl::GetNextYearMonth(IN int nYear, IN int nMonth, OUT int &nYearOut, OUT int &nMonthOut)
{
	nMonthOut = 12 == nMonth ? 1 : nMonth + 1;
	nYearOut = 12 == nMonth ? nYear + 1 : nYear;

	if (nYear < CALENDAR_YEAR_START) return false;

	return true;
}

void CDUICalendarCtrl::PerformSelectDay(CDUIListViewCtrl *pListDays, int nYear, int nMonth, int nDay)
{
	if (NULL == pListDays) return;

	DWORD dwDate = ConstructDate(nYear, nMonth, nDay);
	for (int n = 0; n < pListDays->GetChildCount(); n++)
	{
		CDUIListItemCtrl *pDay = pListDays->GetChildAt(n);
		if (NULL == pDay) continue;

		if (dwDate == pDay->GetTag())
		{
			pListDays->SelectItem(n);

			break;
		}
	}

	return;
}

void CDUICalendarCtrl::PopupYearMonthWnd(bool bYear)
{
	//menu
	CDUIMenu Menu;
	Menu.LoadMenu(_T(""));
	if (NULL == g_pDuiMenuWndRoot) return;

	SYSTEMTIME SysTime = {};
	GetLocalTime(&SysTime);

	CDUISize szItem = bYear ? m_AttributeTextStyleYearNormal.MeasureString(_T("9999")) : m_AttributeTextStyleMonthNormal.MeasureString(_T("99"));
	szItem.cx += 1;
	tagDuiTextStyle TextStyleNormal = bYear ? m_AttributeTextStyleYearNormal.GetTextStyle() : m_AttributeTextStyleMonthNormal.GetTextStyle();
	tagDuiTextStyle TextStyleHot = bYear ? m_AttributeTextStyleYearHot.GetTextStyle() : m_AttributeTextStyleMonthHot.GetTextStyle();
	TextStyleNormal.dwTextStyle |= (DT_VCENTER | DT_CENTER);
	TextStyleHot.dwTextStyle |= (DT_VCENTER | DT_CENTER);

	//menu view
	CDUIMenuCtrl *pPopupView = new CDUIMenuCtrl();
	pPopupView->Init();
	pPopupView->SetBkColor({ Name_ColorDefault });
	pPopupView->SetItemTextStyleNormal(TextStyleNormal);
	pPopupView->SetItemTextStyleHot(TextStyleHot);
	pPopupView->SetItemTextStyleSelNormal(TextStyleNormal);
	pPopupView->SetItemTextStyleSelHot(TextStyleNormal);
	pPopupView->SetItemStatusColorResSwitchNormal(bYear ? m_AttributeColorYearNormal.GetColorResSwitch() : m_AttributeColorMonthNormal.GetColorResSwitch());
	pPopupView->SetItemStatusColorResSwitchHot(bYear ? m_AttributeColorYearHot.GetColorResSwitch() : m_AttributeColorMonthHot.GetColorResSwitch());
	pPopupView->SetItemStatusColorResSwitchSelNormal({ Name_ColorSelBk });
	pPopupView->SetSwitchTileItemSize(szItem);
	pPopupView->SetChildPadding(5, 5);
	pPopupView->SwitchListViewType(enDuiListViewType::ListView_TileH);
	pPopupView->SetUseListHeader(false);
	if (bYear)
	{
		pPopupView->CDUIListViewCtrl::InsertChild((CALENDAR_COUNT_YEAR));
		for (int nYear = SysTime.wYear - CALENDAR_COUNT_YEAR / 2; nYear < SysTime.wYear + (CALENDAR_COUNT_YEAR + 1) / 2; nYear++)
		{
			CDUIMenuItemCtrl *pYear = pPopupView->GetMenuItem(nYear - (SysTime.wYear - CALENDAR_COUNT_YEAR / 2));
			pYear->SetText(CMMStrHelp::Format(_T("%d"), nYear));
			pYear->SetTag(ConstructDate(nYear, ParseMonth(m_pBtnYearCtrl->GetTag()), 1));

			if (nYear == ParseYear(m_pBtnYearCtrl->GetTag()))
			{
				pYear->SetBkColor({ Name_ColorSelBk });
			}
		}
	}
	else
	{
		pPopupView->CDUIListViewCtrl::InsertChild((CALENDAR_COUNT_MONTH));	
		for (int nMonth = 1; nMonth <= CALENDAR_COUNT_MONTH; nMonth++)
		{
			CDUIMenuItemCtrl *pMonth = pPopupView->GetMenuItem(nMonth - 1);
			pMonth->SetText(CMMStrHelp::Format(_T("%d"), nMonth));
			pMonth->SetTag(ConstructDate(ParseYear(m_pBtnMonthCtrl->GetTag()), nMonth, 1));

			if (nMonth == ParseMonth(m_pBtnMonthCtrl->GetTag()))
			{
				pMonth->SetBkColor({ Name_ColorSelBk });
			}
		}
	}

	//size
	CDUISize szWnd;
	const int nCount = max(1, pPopupView->GetChildCount());
	const int nColumn = (int)ceil(sqrt((double)nCount));
	const int nRow = (int)ceil(nCount / (double)nColumn);
	szWnd.cx = (szItem.cx + pPopupView->GetChildPaddingH()) * nColumn;
	szWnd.cy = (szItem.cy + pPopupView->GetChildPaddingV()) * nRow;
	g_pDuiMenuWndRoot->SetGdiplusRenderText(true);
	g_pDuiMenuWndRoot->SetGdiplusRenderTextType(Gdiplus::TextRenderingHint::TextRenderingHintAntiAliasGridFit);
	g_pDuiMenuWndRoot->SetWndInitSize(szWnd.cx, szWnd.cy);
	g_pDuiMenuWndRoot->SetWndLayered(true);
	g_pDuiMenuWndRoot->SetCaptionHeight(0);

	//pos
	CDUIPoint ptTrack;
	if (bYear)
	{
		ptTrack = CDUIPoint(m_pBtnYearCtrl->GetAbsoluteRect().left, m_pBtnYearCtrl->GetAbsoluteRect().top);
		ptTrack.Offset(-szWnd.cx, m_pBtnYearCtrl->GetHeight() / 2 - szWnd.cy / 2);	
	}
	else
	{
		ptTrack = CDUIPoint(m_pBtnMonthCtrl->GetAbsoluteRect().right, m_pBtnMonthCtrl->GetAbsoluteRect().top);
		ptTrack.Offset(0, m_pBtnMonthCtrl->GetHeight() / 2 - szWnd.cy / 2);
	}

	::ClientToScreen(m_pWndOwner->GetWndHandle(), &ptTrack);
	g_pDuiMenuWndRoot->SetMenuView(pPopupView);
	tagDuiMenuCmd MenuCmd = Menu.TrackPopupMenu(m_pWndOwner->GetWndHandle(), ptTrack);

	//select
	if (MenuCmd.uMenuTag)
	{
		SwitchYearMonth(ParseYear(MenuCmd.uMenuTag), ParseMonth(MenuCmd.uMenuTag));
	}

	//focus
	SetFocus();

	return;
}

//////////////////////////////////////////////////////////////////////////
CDUICalendar::CDUICalendar()
{
}

CDUICalendar::~CDUICalendar()
{
	DestroyCalendar();

	return;
}

void CDUICalendar::LoadCalendar(LPCTSTR lpszCalendar)
{
	DestroyCalendar();

	g_pDuiCalendarWnd = new CDUICalendarWnd(lpszCalendar);

	return;
}

void CDUICalendar::DestroyCalendar()
{
	MMSafeDelete(g_pDuiCalendarWnd);

	return;
}

CDUICalendarCtrl * CDUICalendar::GetCalendarCtrl()
{
	if (NULL == g_pDuiCalendarWnd) return NULL;

	return g_pDuiCalendarWnd->GetCalendarCtrl();
}

tagDuiCalendarCmd CDUICalendar::TrackPopupCalendar(HWND hWndParent, CDUIPoint pt)
{
	if (NULL == g_pDuiCalendarWnd) return g_DuiCalendarCmd;

	g_pDuiCalendarWnd->Init(hWndParent, pt);
	g_pDuiCalendarWnd->DoBlock();
	DestroyCalendar();

	return g_DuiCalendarCmd;
}

//////////////////////////////////////////////////////////////////////////