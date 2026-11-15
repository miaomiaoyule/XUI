#ifndef __DUICALENDARCTRL_H__
#define __DUICALENDARCTRL_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
typedef std::function<void(CDUIListItemCtrl *pDay, int nYear, int nMonth, int nDay)> OnConstructDay;

//////////////////////////////////////////////////////////////////////////
class XUI_API CDUICalendarWnd : public CDUIPopupWnd
{
	MMDeclare_Super(CDUIPopupWnd)
	MMDeclare_ClassName()
	DuiDeclare_Message_Map()

public:
	CDUICalendarWnd(CMMString strDuiName);
	virtual ~CDUICalendarWnd();

	//variant
protected:
	CDUIPoint							m_ptTrack;
	CDUICalendarCtrl *					m_pShowCalendarView = NULL;

	//method
public:
	void Init(HWND hWndParent);
	void Init(HWND hWndParent, CDUIPoint ptTrack);
	void Close(UINT nRet = IDOK) override;

	CDUICalendarCtrl * GetCalendarCtrl();

	//message
protected:
	LRESULT OnCreate(WPARAM wParam, LPARAM lParam) override;
	LRESULT OnKillFocus(WPARAM wParam, LPARAM lParam) override;
	LRESULT OnKeyDown(WPARAM /*wParam*/, LPARAM /*lParam*/) override;
	LRESULT OnClose(WPARAM /*wParam*/, LPARAM /*lParam*/) override;
	LRESULT OnWndMessage(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/) override;

	//notify
protected:

	//help
protected:
	void AdjustCalendar();
};

//////////////////////////////////////////////////////////////////////////
#define VER_CDUICalendarCtrl INTERFACE_VERSION(1,1)
static const GUID IID_CDUICalendarCtrl = { 0xA6D76CAA,0xF4CD,0x47F1,0xAD,0x6E,0xB1,0xFF,0x9A,0xE0,0x8B,0xB9 };
class XUI_API CDUICalendarCtrl
	: public CDUIVerticalLayoutCtrl
	, public CDUINotifyPump
	, public IDuiControlCallBack
{
	MMDeclare_Super(CDUIVerticalLayoutCtrl)
	DuiDeclare_CreateControl(CDUICalendarCtrl)
	MMDeclare_ClassName(CDUICalendarCtrl)
	DuiDeclare_Message_Map()

public:
	CDUICalendarCtrl(void);
	~CDUICalendarCtrl(void);

	//attribute
protected:
	CDUIAttributeGroup					m_AttributeGroupHeader;
	CDUIAttributeBool					m_AttributeHeaderVisible = true;
	CDUIAttributeLong					m_AttributeHeaderHeight = 30;
	CDUIAttributeSize					m_AttributeLeftRightBtnSize = SIZE{ 30, 30 };
	CDUIAttributeTextStyle				m_AttributeTextStyleYearNormal = DT_SINGLELINE | DT_END_ELLIPSIS | DT_RIGHT | DT_VCENTER;
	CDUIAttributeTextStyle				m_AttributeTextStyleYearHot = DT_SINGLELINE | DT_END_ELLIPSIS | DT_RIGHT | DT_VCENTER;
	CDUIAttributeTextStyle				m_AttributeTextStyleMonthNormal = DT_SINGLELINE | DT_END_ELLIPSIS | DT_LEFT | DT_VCENTER;
	CDUIAttributeTextStyle				m_AttributeTextStyleMonthHot = DT_SINGLELINE | DT_END_ELLIPSIS | DT_LEFT | DT_VCENTER;
	CDUIAttributeColorSwitch			m_AttributeColorYearNormal;
	CDUIAttributeColorSwitch			m_AttributeColorYearHot;
	CDUIAttributeColorSwitch			m_AttributeColorMonthNormal;
	CDUIAttributeColorSwitch			m_AttributeColorMonthHot;
	CDUIAttriImageSection				m_AttributeImageLeftBtnNormal;
	CDUIAttriImageSection				m_AttributeImageLeftBtnHot;
	CDUIAttriImageSection				m_AttributeImageRightBtnNormal;
	CDUIAttriImageSection				m_AttributeImageRightBtnHot;

	CDUIAttributeGroup					m_AttributeGroupDays;
	CDUIAttributeCombox					m_AttributeWeekStyle;
	CDUIAttributeLong					m_AttributeWeekHeight = 30;
	CDUIAttributeColorSwitch			m_AttributeColorHotDayBk;
	CDUIAttributeColorSwitch			m_AttributeColorSelDayBk;
	CDUIAttributeColorSwitch			m_AttributeColorTodayBk;
	CDUIAttriImageSection				m_AttributeImageHotDayBk;
	CDUIAttriImageSection				m_AttributeImageSelDayBk;
	CDUIAttriImageSection				m_AttributeImageTodayBk;
	CDUIAttributeTextStyle				m_AttributeTextStyleWeek = DT_SINGLELINE | DT_END_ELLIPSIS | DT_CENTER | DT_VCENTER;
	CDUIAttributeTextStyle				m_AttributeTextStyleOtherMonthDay = DT_SINGLELINE | DT_END_ELLIPSIS | DT_CENTER | DT_VCENTER;
	CDUIAttributeTextStyle				m_AttributeTextStyleCurMonthDay = DT_SINGLELINE | DT_END_ELLIPSIS | DT_CENTER | DT_VCENTER;
	CDUIAttributeTextStyle				m_AttributeTextStyleHotMonthDay = DT_SINGLELINE | DT_END_ELLIPSIS | DT_CENTER | DT_VCENTER;
	CDUIAttributeTextStyle				m_AttributeTextStyleToday = DT_SINGLELINE | DT_END_ELLIPSIS | DT_CENTER | DT_VCENTER;
	CDUIAttributeText					m_AttributeTextWeekDay[Dui_Count_WeekDay];

	//variant
protected:
	CDUIHorizontalLayoutCtrl *			m_pHorizHeaderCtrl = NULL;
	CDUIButtonCtrl *					m_pBtnLeftCtrl = NULL;
	CDUIButtonCtrl *					m_pBtnYearCtrl = NULL;
	CDUIButtonCtrl *					m_pBtnMonthCtrl = NULL;
	CDUIButtonCtrl *					m_pBtnRightCtrl = NULL;

	CDUITabLayoutCtrl *					m_pTabSwitchCtrl = NULL;
	CDUIVerticalLayoutCtrl *			m_pVertPageCtrl1 = NULL;
	CDUIVerticalLayoutCtrl *			m_pVertPageCtrl2 = NULL;
	CDUIHorizontalLayoutCtrl *			m_pHorizWeekCtrl1 = NULL;
	CDUIHorizontalLayoutCtrl *			m_pHorizWeekCtrl2 = NULL;
	CDUIListViewCtrl *					m_pListDayCtrl1 = NULL;
	CDUIListViewCtrl *					m_pListDayCtrl2 = NULL;

	tagDuiCalendarCmd					m_CalendarCmd;
	OnConstructDay						m_pConstructDayCallBack;

	static LPCTSTR						m_szWeekDayAttriName[Dui_Count_WeekDay];

	//override
protected:
	bool OnAttributeChange(CDUIAttributeObject *pAttributeObj) override;

	//override
protected:
	void OnRelease(CDUIControlBase *pControl) override;
	void OnSize(CDUIControlBase *pControl) override;

	//method
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;
	CMMString GetDescribe() const override;

	//basic
	bool SetWndOwner(CDUIWnd *pWndOwner) override;

	//calendar;
	virtual bool IsHeaderVisible();
	virtual void SetHeaderVisible(bool bVisible);
	virtual int GetHeaderHeight();
	virtual void SetHeaderHeight(int nHeight);
	virtual CDUISize GetLeftRightBtnSize();
	virtual void SetLeftRightBtnSize(CDUISize szXY);
	virtual enDuiCalendarWeekType GetWeekStyle();
	virtual void SetWeekStyle(enDuiCalendarWeekType WeekStyle);
	virtual int GetWeekHeight();
	virtual void SetWeekHeight(int nHeight);
	virtual tagDuiTextStyle GetTextStyleYearNormal();
	virtual tagDuiTextStyle GetTextStyleYearHot();
	virtual tagDuiTextStyle GetTextStyleMonthNormal();
	virtual tagDuiTextStyle GetTextStyleMonthHot();
	virtual tagDuiTextStyle GetTextStyleWeek();
	virtual tagDuiTextStyle GetTextStyleOtherMonthDay();
	virtual tagDuiTextStyle GetTextStyleCurMonthDay();
	virtual tagDuiTextStyle GetTextStyleHotMonthDay();
	virtual tagDuiTextStyle GetTextStyleToday();
	virtual void SetTextStyleYearNormal(const tagDuiTextStyle &TextStyle);
	virtual void SetTextStyleYearHot(const tagDuiTextStyle &TextStyle);
	virtual void SetTextStyleMonthNormal(const tagDuiTextStyle &TextStyle);
	virtual void SetTextStyleMonthHot(const tagDuiTextStyle &TextStyle);
	virtual void SetTextStyleWeek(const tagDuiTextStyle &TextStyle);
	virtual void SetTextStyleOtherMonthDay(const tagDuiTextStyle &TextStyle);
	virtual void SetTextStyleCurMonthDay(const tagDuiTextStyle &TextStyle);
	virtual void SetTextStyleHotMonthDay(const tagDuiTextStyle &TextStyle);
	virtual void SetTextStyleToday(const tagDuiTextStyle &TextStyle);
	virtual tagDuiImageSection GetImageSectionLeftBtnNormal();
	virtual tagDuiImageSection GetImageSectionLeftBtnHot();
	virtual tagDuiImageSection GetImageSectionRightBtnNormal();
	virtual tagDuiImageSection GetImageSectionRightBtnHot();
	virtual tagDuiImageSection GetImageSectionHotDayBk();
	virtual tagDuiImageSection GetImageSectionSelDayBk();
	virtual tagDuiImageSection GetImageSectionTodayBk();
	virtual void SetImageSectionLeftBtnNormal(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionLeftBtnHot(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionRightBtnNormal(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionRightBtnHot(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionHotDayBk(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionSelDayBk(const tagDuiImageSection &ImageSection);
	virtual void SetImageSectionTodayBk(const tagDuiImageSection &ImageSection);
	virtual vector<CMMString> GetColorResSwitchHotDayBk();
	virtual vector<CMMString> GetColorResSwitchSelDayBk();
	virtual vector<CMMString> GetColorResSwitchTodayBk();
	virtual void SetColorResSwitchHotDayBk(const vector<CMMString> &vecResSwitch);
	virtual void SetColorResSwitchSelDayBk(const vector<CMMString> &vecResSwitch);
	virtual void SetColorResSwitchTodayBk(const vector<CMMString> &vecResSwitch);
	virtual tagDuiCalendarCmd GetCalendarSelect();
	virtual void SetCalendarSelect(tagDuiCalendarCmd tTime);

	//weekday 1-7
	virtual CMMString GetMondayText();
	virtual CMMString GetTuesdayText();
	virtual CMMString GetWednesdayText();
	virtual CMMString GetThursdayText();
	virtual CMMString GetFridayText();
	virtual CMMString GetSaturdayText();
	virtual CMMString GetSundayText();
	virtual CMMString GetWeekText(int nWeekDay);

	//callback
	virtual void AttatchConstructDayCallback(OnConstructDay pOnConstructDay);

	//method
protected:
	//child
	bool InsertChild(CDUIControlBase *pChild, int nPos = -1) override;

	//proprety
protected:
	void InitProperty() override;
	void InitComplete() override;
	bool SaveAttribute(tinyxml2::XMLElement *pNode, bool bIncludeChild = true) override;

	//notify
protected:
	void OnDuiMouseWheel(const DuiNotify &Notify);
	void OnDuiItemSelected(const DuiNotify &Notify);
	void OnDuiItemClick(const DuiNotify &Notify);
	void OnDuiBtnLeft(const DuiNotify &Notify);
	void OnDuiBtnRight(const DuiNotify &Notify);
	void OnDuiBtnYear(const DuiNotify &Notify);
	void OnDuiBtnMonth(const DuiNotify &Notify);

	//help
protected:
	void InitChildren();
	void InitCtrlInfo();
	void InitCalendarInfo();

	int ParseYear(DWORD dwDate);
	int ParseMonth(DWORD dwDate);
	int ParseDay(DWORD dwDate);
	DWORD ConstructDate(int nYear, int nMonth, int nDay);
	void ConstructHeader(int nYear, int nMonth);
	void ConstructDay(CDUIListItemCtrl *pDay, int nYear, int nMonth, int nDay);
	void ConstructCalendar(CDUIListViewCtrl *pListDays, int nYear, int nMonth);

	//help
protected:
	bool IsLeapYear(int nYear);
	int GetWeekDay(int nYear, int nMonth, int nDay);
	int GetYearDays(int nYear, int nMonth, int nDay);
	int GetMonthDays(int nYear, int nMonth);
	bool GetPreYearMonth(IN int nYear, IN int nMonth, OUT int &nYearOut, OUT int &nMonthOut);
	bool GetNextYearMonth(IN int nYear, IN int nMonth, OUT int &nYearOut, OUT int &nMonthOut);

	//help
protected:
	void SwitchYearMonth(bool bPre);
	void SwitchYearMonth(int nYear, int nMonth);
	void PopupYearMonthWnd(bool bYear);
};

//////////////////////////////////////////////////////////////////////////
class XUI_API CDUICalendar
{
public:
	CDUICalendar();
	virtual ~CDUICalendar();

protected:

	//method
public:
	virtual void LoadCalendar(LPCTSTR lpszCalendar);
	virtual void DestroyCalendar();
	virtual CDUICalendarCtrl * GetCalendarCtrl();
	virtual tagDuiCalendarCmd TrackPopupCalendar(HWND hWndParent, CDUIPoint pt = { -1, -1 });
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
