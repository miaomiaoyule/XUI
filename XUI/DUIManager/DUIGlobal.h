#ifndef __DUIGLOBAL_H__
#define __DUIGLOBAL_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
typedef std::unordered_map<CMMString, CDUIControlBase*> MapDuiModelStore;
typedef std::map<UINT, CMMString> MapDuiControlID;
typedef std::map<uint32_t, uint32_t> MapValueIDSwitch;

//输出
LPCTSTR XUI_API DUI__TraceMsg(UINT uMsg);

//////////////////////////////////////////////////////////////////////////
class CDTDesignerView;
class CProjectView;
class CPropertiesView;
class CImageResView;
class CFontResView;
class CColorGridWnd;

//////////////////////////////////////////////////////////////////////////
class XUI_API CDUIGlobal 
	: public CMMAsyncObject
	, public CMMServiceItem
{
	friend class CDUIControlBase;
	friend class CDUIContainerCtrl;
	friend class CDUIMenuItemCtrl;
	friend class CDUIMenuWnd;
	friend class CDUIRotateMenuCtrl;
	friend class CDUICalendarWnd;
	friend class CDUIXmlPack;
	friend class CDUIAttributeObject;
	friend class CDUIAttriImageSection;
	friend class CDUIAttributeTextStyle;
	friend class CDUIAttributeRichText;
	friend class CDUIAttributeText;
	friend class CDUIAttributeColorSwitch;
	friend class CDUIAttributeCombox;
	friend class CDUIAttributePosition;
	friend class CDUIAttributeHotKey;
	friend class CDUIAttributeRect;
	friend class CDUIAttributeSize;
	friend class CDUIAttributeTabSelect;
	friend class CDUIAttributeCtrlID;
	friend class CDUIPropertyObject;
	friend class CDUIImageBase;
	friend class CDUIWnd;
	friend class COperatorHistory;
	friend class CUICommandElement;
	friend class CDesignerView;
	friend class CProjectView;
	friend class CPropertiesView;
	friend class CDesignerTitle;
	friend class CImageResView;
	friend class CFontResView;
	friend class CColorResView;
	friend class CColorGridWnd;
	friend class CControlView;
	friend class CVSManager;
	friend class CDlgWizardVariant;
	friend class CDlgWizardNotify;
	friend class CNotifyView;
	friend class CAiBuilder;
	friend class CDTDesignerApp;

private:
	CDUIGlobal(void);
	~CDUIGlobal(void);

	//variant
private:
	//path file
	CMMString							m_strLastError;
	CMMString							m_strProjectPath;
	CMMString							m_strProjectName;
	CMMString							m_strSkinDir;
	CMMString							m_strDlgDir;
	CMMString							m_strViewDir;
	CMMString							m_strModelListItemDir;
	CMMString							m_strModelTreeNodeDir;
	CMMString							m_strMenuDir;
	CMMString							m_str3DMenuDir;
	CMMString							m_strCalendarDir;
	CMMString							m_strFontResDefault;

	//resource	
	MapDuiFontBase						m_mapResourceFont;
	MapDuiColorBase						m_mapResourceColor;
	MapDuiImageBase						m_mapResourceImage;
	VecDuiFile							m_vecDui;

	//manager
	VecIDuiPreMessage					m_vecPreMessage;
	MapDuiModelStore					m_mapModelStore;
	VecIDuiResourceCallBack				m_vecIResourceCallBack;

	//attribute
	bool								m_bAttriWaitSave = false;
	MapAttributeName					m_mapAttriNameValue;
	MapDuiControlID						m_mapControlIDValue;
	MapDuiControlID						m_mapControlIDSave;
	UnorderMapDuiAttributeRichText		m_mapAttriRichTextValue;
	MapDuiAttributeRichText				m_mapAttriRichTextSave;
	UnorderMapDuiAttributeText			m_mapAttriTextValue;
	MapDuiAttributeText					m_mapAttriTextSave;
	UnorderMapDuiAttributeTextStyle		m_mapAttriTextStyleValue;
	MapDuiAttributeTextStyle			m_mapAttriTextStyleSave;
	UnorderMapDuiAttributeColorRes		m_mapAttriColorResValue;
	MapDuiAttributeColorRes				m_mapAttriColorResSave;
	UnorderMapDuiAttributeCombox		m_mapAttriComboxValue;
	MapDuiAttributeCombox				m_mapAttriComboxSave;
	UnorderMapDuiAttributePosition		m_mapAttriPositionValue;
	MapDuiAttributePosition				m_mapAttriPositionSave;
	UnorderMapDuiAttributeRect			m_mapAttriRectValue;
	MapDuiAttributeRect					m_mapAttriRectSave;
	UnorderMapDuiAttriImageSection		m_mapAttriImageSectionValue;
	MapDuiAttriImageSection				m_mapAttriImageSectionSave;
	MapValueIDSwitch					m_mapAttriImageSectionSwitch;

	//info
	std::recursive_mutex				m_DataLock;
	bool								m_bInited = false;
	bool								m_bProjectExist = false;
	CMMThreadPool						m_ThreadPool;
	CMMDpi								m_DpiInfo;
	std::vector<HMODULE>				m_vecModuleExtendDll;
	MapWnd								m_mapWnd;
	HINSTANCE							m_hInstance = NULL;
	HINSTANCE							m_hInstanceResource = NULL;
	enDuiFileResType					m_DuiFileResType = DuiFileResType_File;
	enDuiResVersion						m_DuiResVersion = DuiResVersion_0;
	int									m_nIndexSwitchRes = 0;
	UINT								m_uMaxControlID = Dui_CtrlIDInner_Finish;
	UINT								m_uMaxResourceIndex = 0;

	//zipres
	HZIPDT								m_hResourceZip = NULL;
	std::vector<BYTE>					m_vecZipData;

	//gdiplus
#ifndef DuiPlatform_SDL
	ULONG_PTR							m_uToken = 0;
	Gdiplus::GdiplusStartupInput		m_GdiplusInput;
#endif

	//override
protected:	
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled) override;

	//override
protected:
	void OnMessage(PtrMMServiceMsg pMessage) override;

	//method
public:
	static CDUIGlobal * GetInstance();
	bool Init(HINSTANCE hInstance);
	bool UnInit() override;
	bool IsLoadProject();
	bool LoadProjectFromFile(LPCTSTR lpszProjFile);
	bool LoadProjectFromZip(LPCTSTR lpszZipFile, LPCTSTR lpszPassword, LPCTSTR lpszProjName);
	bool LoadProjectFromZip(void *pData, UINT uDataLen, LPCTSTR lpszPassword, LPCTSTR lpszProjName);
	bool LoadProjectFromResZip(HINSTANCE hResModule, LPCTSTR lpszZipName, LPCTSTR lpszPassword, LPCTSTR lpszProjName, LPCTSTR lpszResType = _T("ZIP"));
	CMMString GetDuiLastError();
	void SetDuiLastError(CMMString strError);
	enDuiResVersion GetResVersion() { return m_DuiResVersion; }

	//message
	bool AddPreMessagePtr(IDuiPreMessage *pInterface);
	bool RemovePreMessagePtr(IDuiPreMessage *pInterface);
	bool TranslateMessage(const LPMSG pMsg);

	//wnd
	MapWnd GetWndAll();
	tagDuiFile GetWndInfo(CDUIWnd *pWnd);
	CDUIWnd * GetWndByHandle(HWND hWnd);

	//switch skin
	void PerformSwitchRes(int nIndexRes);
	int GetSwitchResIndex();

	//dpi
	const CMMDpi & GetDpiObj();
	int GetDpi();
	bool SetDpi(int nDpi);
	int GetScale();
	bool SetScale(int nScale);

	//ui
	void LoadWnd(const CMMString &strName, CDUIWnd *pWnd);
	CDUIControlBase * LoadDui(const CMMString &strName, CDUIWnd *pWnd = NULL);
	CDUIControlBase * ParseDui(tinyxml2::XMLElement *pNodeXml);
	CDUIControlBase * ParseDui(LPCTSTR lpszXml);

	//resource
public:
	//data
	bool ExtractResourceData(vector<BYTE> &vecData, CMMString strFile);

	//res obj
	bool RegisterResourceCallBack(IDuiResourceCallBack *pIResourceCallBack);
	bool UnRegisterResourceCallBack(IDuiResourceCallBack *pIResourceCallBack);
	bool AddResource(CDUIResourceBase *pResourceObj);

	int GetFontResourceCount();
	CDUIFontBase * GetFontResource(const CMMString &strName);
	CDUIFontBase * GetFontResource(int nIndex);
	CDUIFontBase * GetFontResDefault();
	MapDuiFontBase GetFontResourceAll();
	bool RemoveFontResource(const CMMString &strName);

	int GetColorResourceCount();
	CDUIColorBase * GetColorResource(const CMMString &strName);
	CDUIColorBase * GetColorResource(int nIndex);
	CDUIColorBase * GetColorResource(Gdiplus::ARGB dwColor);
	MapDuiColorBase GetColorResourceAll();
	bool RemoveColorResource(const CMMString &strName);

	int GetImageResourceCount();
	CDUIImageBase * GetImageResource(const CMMString &strName);
	CDUIImageBase * GetImageResource(int nIndex);
	CDUIImageBase * GetImageResourceByFile(const CMMString &strFileFull);
	MapDuiImageBase GetImageResourceAll();
	bool RemoveImageResource(const CMMString &strName);

	int GetDuiCount(enDuiType DuiType);
	CMMString GetDuiName(int nIndex, enDuiType DuiType);
	enDuiType GetDuiType(const CMMString &strName);
	CMMString GetDuiFile(int nIndex, enDuiType DuiType);
	CMMString GetDuiFile(const CMMString &strName);
	CMMString GetDuiFileFull(const CMMString &strName);
	bool RemoveDui(const CMMString &strName);

	//path
	CMMString GetProjectPath();
	CMMString GetProjectName();
	CMMString GetProjectFileFull();
	CMMString GetSkinPath();
	CMMString GetDuiPath(enDuiType DuiType);

	//res info
	HINSTANCE GetInstanceHandle();
	HINSTANCE GetResourceDll();
	HZIPDT GetResourceZipHandle();
	enDuiFileResType GetDuiFileResType();

	//ui manager
protected:
	bool SaveProject();
	bool CloseProject(bool bSaveProject = true);

	//ctrl id
	void AddCtrlID(UINT uCtrlID, CMMString strCtrlID);
	UINT FindCtrlID(CMMString strCtrlID);
	CMMString FindCtrlID(UINT uCtrlID);
	UINT GenerateCtrlID();
	CMMString GenerateCtrlID(CDUIControlBase *pControl);
	bool ModifyCtrlID(CMMString strCtrlIDOld, CMMString strCtrlIDNew, CDUIControlBase *pControl);
	
	//dui
	CMMString CreateDlg();
	CMMString CreateView();
	CMMString CreateModelListItem();
	CMMString CreateModelTreeNode();
	CMMString CreateMenu(CDUIWnd *pWndParentMenu);
	CMMString Create3DMenu();
	CMMString CreateCalendar();
	bool SaveDui(LPCTSTR lpszName, CDUIWnd *pWnd);
	CMMString SaveDui(CDUIPropertyObject *pPropObj, bool bIncludeChild = true);
	
	//refresh
	bool RefreshAttibute(tinyxml2::XMLElement *pNodeXml, CDUIPropertyObject *pPropObj);

	//help
protected:
	//config
	void LoadPublicResource();
	void LoadConfigCtrl(const CMMString &strConfigFile);
	bool SetProjectPath(LPCTSTR lpszPath);
	bool SetProjectName(LPCTSTR lpszName);
	bool SetResVersion(enDuiResVersion ResVersion);
	
	//resource
	bool AddFontResource(CDUIFontBase *pResourceObj);
	bool AddImageResource(CDUIImageBase *pResourceObj);
	bool AddColorResource(CDUIColorBase *pResourceObj);
	bool AddDui(enDuiType DuiType, const CMMString &strName, const CMMString &strFile);
	bool RenameFontResource(const CMMString &strNameOld, const CMMString &strNameNew);
	bool RenameImageResource(const CMMString &strNameOld, const CMMString &strNameNew);
	bool RenameColorResource(const CMMString &strNameOld, const CMMString &strNameNew);
	bool RenameDui(const CMMString &strNameOld, const CMMString &strNameNew);
	void SetFontResDefault(const CMMString &strName);
	void OnDpiChanged(int nScalePre);
	void OnResourceAdd(CDUIResourceBase *pResourceObj);
	void OnResourceRemove(CDUIResourceBase *pResourceObj);
	void OnResourceRename(CDUIResourceBase *pResourceObj, const CMMString &strNameOld);
	void OnResourceSwitch(int nIndexRes);

	//attribute
	LPCTSTR GetAttriName(uint32_t uValueID);
	uint32_t SetAttriName(LPCTSTR lpszName);
	bool SetAttriName(tinyxml2::XMLElement *pNode);
	bool SaveAttriName(tinyxml2::XMLElement *pNode);
	tagDuiRichText GetAttriRichText(uint32_t uValueID);
	uint32_t SetAttriRichText(const tagDuiRichText &RichText);
	bool SetAttriRichText(tinyxml2::XMLElement *pNode);
	bool SaveAttriRichText(tinyxml2::XMLElement *pNode);
	LPCTSTR GetAttriText(uint32_t uValueID);
	uint32_t SetAttriText(LPCTSTR lpszText);
	bool SetAttriText(tinyxml2::XMLElement *pNode);
	bool SaveAttriText(tinyxml2::XMLElement *pNode);
	tagDuiTextStyle GetAttriTextStyle(uint32_t uValueID);
	uint32_t SetAttriTextStyle(const tagDuiTextStyle &TextStyle);
	bool SetAttriTextStyle(tinyxml2::XMLElement *pNode);
	bool SaveAttriTextStyle(tinyxml2::XMLElement *pNode);
	vector<CMMString> GetAttriColorResSwitch(uint32_t uValueID);
	uint32_t SetAttriColorResSwitch(const vector<CMMString> &vecColorRes);
	bool SetAttriColorResSwitch(tinyxml2::XMLElement *pNode);
	bool SaveAttriColorResSwitch(tinyxml2::XMLElement *pNode);
	tagDuiCombox GetAttriCombox(uint32_t uValueID);
	uint32_t SetAttriCombox(const tagDuiCombox &AttriCombox);
	bool SetAttriCombox(tinyxml2::XMLElement *pNode);
	bool SaveAttriCombox(tinyxml2::XMLElement *pNode);
	tagDuiPosition GetAttriPosition(uint32_t uValueID);
	uint32_t SetAttriPosition(const tagDuiPosition &Position);
	bool SetAttriPosition(tinyxml2::XMLElement *pNode);
	bool SaveAttriPosition(tinyxml2::XMLElement *pNode);
	CDUIRect GetAttriRect(uint32_t uValueID);
	uint32_t SetAttriRect(const CDUIRect &rect);
	bool SetAttriRect(tinyxml2::XMLElement *pNode);
	bool SaveAttriRect(tinyxml2::XMLElement *pNode);
	tagDuiImageSection GetAttriImageSection(uint32_t uValueID);
	uint32_t SetAttriImageSection(const tagDuiImageSection &ImageSection);
	bool SetAttriImageSection(tinyxml2::XMLElement *pNode);
	bool SaveAttriImageSection(tinyxml2::XMLElement *pNode);
	void OnAttriValueIDRead(enDuiAttributeType AttriType, OUT uint32_t &uID);
	bool SaveAttriValue(tinyxml2::XMLDocument &xmlDoc);

	//Wnd
	void AddWnd(CDUIWnd *pWnd);
	void RenameWnd(const CMMString &strNameOld, const CMMString &strNameNew);
	void RenameWnd(CDUIWnd *pWnd, const CMMString &strNameNew);
	void SetWndDuiType(CDUIWnd *pWnd, enDuiType DuiType);
	void RemoveWnd(CDUIWnd *pWnd);

	//release
	void ReleaseResource();
	void ReleaseFontResource();
	void ReleaseImageResource();
	void ReleaseColorResource();
	void ReleaseDui();

	//static
public:
	static void MessageLoop();

	//static help
public:
	static void RegisterWndNotify(IDuiWndNotify *pIDuiWndNotify);
	static void UnRegisterWndNotify(IDuiWndNotify *pIDuiWndNotify);

	//static help
public:
	static void PerformNotifyChildAdd(CDUIPropertyObject *pPropertyObj, CDUIControlBase *pChild);
	static void PerformNotifyChildRemove(CDUIPropertyObject *pPropertyObj, CDUIControlBase *pChild);
	static void PerformNotifyVisibleChange(CDUIPropertyObject *pPropertyObj);
	static VecDuiRichTextItem ParseVecRichTextItem(LPCSTR lpszValue);
};

//////////////////////////////////////////////////////////////////////////

#endif