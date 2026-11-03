#include "StdAfx.h"
#include "DUIGlobal.h"

//////////////////////////////////////////////////////////////////////////
#define DuiInitCtrlIDItem(CtrlID) m_mapControlIDValue[CtrlID] = _T(#CtrlID)

//////////////////////////////////////////////////////////////////////////
CDUIGlobal::CDUIGlobal(void)
	: CMMServiceItem(&m_ThreadPool)
{
#ifndef DuiPlatform_SDL
	//Gdiplus
	m_uToken = 0;
	Gdiplus::GdiplusStartup(&m_uToken, &m_GdiplusInput, NULL);
#endif

	return;
}

CDUIGlobal::~CDUIGlobal(void)
{
	UnInit();

	return;
}

LRESULT CDUIGlobal::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	switch (uMsg)
	{
	}

	return CMMAsyncObject::HandleMessage(uMsg, wParam, lParam, bHandled);
}

void CDUIGlobal::OnMessage(PtrMMServiceMsg pMessage)
{
	return;
}

CDUIGlobal * CDUIGlobal::GetInstance()
{
	static CDUIGlobal Obj;

	return &Obj;
}

bool CDUIGlobal::Init(HINSTANCE hInstance)
{
	if (m_bInited) return true;

	UnInit();

	// SDL VIDEO must be ready before any SDL_CreateWindow (incl. CMMAsyncObject).
#if defined(DuiPlatform_SDL)
	if (false == SDL_Init(SDL_INIT_VIDEO))
	{
		MMTRACE(_T("SDL_Init(VIDEO) failed: %s"), (LPCTSTR)CA2CT(SDL_GetError(), CP_ACP));
		return false;
	}
#endif

	if (false == CMMAsyncObject::Init())
	{
		MMTRACE(_T("CMMAsyncObject::Init failed"));
#if defined(DuiPlatform_SDL)
		SDL_Quit();
#endif
		return false;
	}

	CMMServiceItem::Init();

	m_hInstance = hInstance;
	m_bInited = true;
	m_ThreadPool.Run(2);

	//public res
	LoadPublicResource();

	//design ctrl
#ifdef DUI_DESIGN
	LoadConfigCtrl(Dui_FileDesignCtrl);
#endif

	return true;
}

//uninit
bool CDUIGlobal::UnInit()
{
	if (false == m_bInited) return true;
	if (false == CMMAsyncObject::UnInit()) return false;
	if (false == CMMServiceItem::UnInit()) return false;

	//stop thread
	m_ThreadPool.Stop();

	//instance res
	m_DuiFileResType = DuiFileResType_File;
	m_nIndexSwitchRes = 0;
	m_bInited = false;
	m_hInstance = NULL;
	m_hInstanceResource = NULL;
	m_vecZipData.clear();
	if (m_hResourceZip)
	{
		CloseZip(m_hResourceZip);
		m_hResourceZip = NULL;
	}

	//close project
	CloseProject();

	//third
#ifndef DuiPlatform_SDL
#ifdef MMSvgEnable
	CMMSvg::GetInstance()->UnInit();
#endif
#endif

	//extend dll
	for (HMODULE hDllModule : m_vecModuleExtendDll)
	{
		if (NULL == hDllModule) continue;

		FreeLibrary(hDllModule);
	}
	
	m_vecModuleExtendDll.clear();

	//platform
#if defined(DuiPlatform_SDL)
	SDL_Quit();
#endif

	return true;
}

bool CDUIGlobal::IsLoadProject()
{
	return m_bProjectExist;
}

bool CDUIGlobal::LoadProjectFromFile(LPCTSTR lpszProjFile)
{
	ReleaseResource();
	ReleaseDui();

	m_DuiFileResType = DuiFileResType_File;
	CMMString strProjFileFull = lpszProjFile;

	//relative path
#if defined(DuiPlatform_SDL) && !defined(WIN32)
	if (strProjFileFull.empty() || _T('/') != strProjFileFull[0])
	{
		strProjFileFull = CMMService::GetWorkDirectory() + _T('/') + strProjFileFull;
	}

	strProjFileFull.Replace(_T('\\'), _T('/'));
#else
	if (-1 == strProjFileFull.find(_T(':')))
	{
		strProjFileFull = CMMService::GetWorkDirectory() + _T('\\') + strProjFileFull;
	}
#endif

	//load
	if (false == CDUIXmlPack::LoadProject(strProjFileFull)) return false;

	//design ctrl
	LoadConfigCtrl(Dui_FileDesignCtrl);

	m_bProjectExist = true;

	//switch res value id for some old project
#ifdef DUI_DESIGN
	if (DuiResVersion_Max != GetResVersion())
	{
		SetResVersion(DuiResVersion_Max);

		UnorderMapDuiAttriImageSection mapAttriImageSectionValue = m_mapAttriImageSectionValue;
		for (auto &ImageSectionItem : mapAttriImageSectionValue)
		{
			uint32_t uID = ImageSectionItem.second.GetID();
			if (uID != ImageSectionItem.first)
			{
				m_mapAttriImageSectionValue.erase(ImageSectionItem.first);
				m_mapAttriImageSectionValue[uID] = ImageSectionItem.second;
				m_mapAttriImageSectionSwitch[ImageSectionItem.first] = uID;
			}
		}
	}
	if (false == m_mapAttriImageSectionSwitch.empty())
	{
		//load dui
		std::vector<CDUIWnd *> vecDlg;
		for (auto Dui : m_vecDui)
		{
			CDUIWnd *pDlg = new CDUIWnd(Dui.strName);
			pDlg->Create(NULL, _T(""), DUI_WNDSTYLE_DIALOG & ~WS_VISIBLE, DUI_WNDSTYLE_EX_DIALOG);
			vecDlg.push_back(pDlg);
		}

		SaveProject();

		//release wnd
		for (auto pDlg : vecDlg)
		{
			MMSafeDelete(pDlg);
		}

		m_mapAttriImageSectionSwitch.clear();
	}
#endif

	return true;
}

bool CDUIGlobal::LoadProjectFromZip(LPCTSTR lpszZipFile, LPCTSTR lpszPassword, LPCTSTR lpszProjName)
{
	CMMString strZipFileFull = lpszZipFile;

	//relative path
#if defined(DuiPlatform_SDL) && !defined(WIN32)
	if (strZipFileFull.empty() || _T('/') != strZipFileFull[0])
	{
		strZipFileFull = CMMService::GetWorkDirectory() + _T('/') + strZipFileFull;
	}

	strZipFileFull.Replace(_T('\\'), _T('/'));
#else
	if (-1 == strZipFileFull.find(_T(':')))
	{
		strZipFileFull = CMMService::GetWorkDirectory() + _T('\\') + strZipFileFull;
	}
#endif

	//fileinfo
	vector<BYTE> vecData;
	if (false == CMMFile::GetFileData(strZipFileFull, vecData)
		|| vecData.empty())
	{
		return false;
	}

	return LoadProjectFromZip(vecData.data(), vecData.size(), lpszPassword, lpszProjName);
}

bool CDUIGlobal::LoadProjectFromZip(void *pData, UINT uDataLen, LPCTSTR lpszPassword, LPCTSTR lpszProjName)
{
	if (m_hResourceZip)
	{
		CloseZip(m_hResourceZip);
		m_hResourceZip = NULL;
	}

	ReleaseResource();
	ReleaseDui();

	m_DuiFileResType = DuiFileResType_Zip;
	m_vecZipData.assign((BYTE*)pData, (BYTE*)pData + uDataLen);
	m_hResourceZip = OpenZip(m_vecZipData.data(), m_vecZipData.size(), 3, lpszPassword);
	if (NULL == m_hResourceZip) return false;

	//load
	if (false == CDUIXmlPack::LoadProject(CMMString(lpszProjName) + Dui_Name_ProjectExt)) return false;

	//design ctrl
	LoadConfigCtrl(Dui_FileDesignCtrl);

	m_bProjectExist = true;

	return true;
}

bool CDUIGlobal::LoadProjectFromResZip(HINSTANCE hResModule, LPCTSTR lpszZipName, LPCTSTR lpszPassword, LPCTSTR lpszProjName, LPCTSTR lpszResType)
{
#if defined DuiPlatform_SDL
	return false;
#else
	if (NULL == hResModule) return false;

	m_hInstanceResource = hResModule;

	//load res
	CMMResource Resource;
	Resource.Load(GetResourceDll(), lpszResType, lpszZipName, m_vecZipData);

	//unzip
	if (m_hResourceZip)
	{
		CloseZip(m_hResourceZip);
		m_hResourceZip = NULL;
	}

	ReleaseResource();
	ReleaseDui();

	m_DuiFileResType = DuiFileResType_ResZip;
	m_hResourceZip = OpenZip(m_vecZipData.data(), m_vecZipData.size(), 3, lpszPassword);
	if (NULL == m_hResourceZip) return false;

	//load
	if (false == CDUIXmlPack::LoadProject(CMMString(lpszProjName) + Dui_Name_ProjectExt)) return false;

	//design ctrl
	LoadConfigCtrl(Dui_FileDesignCtrl);

	m_bProjectExist = true;

	return true;
#endif
}

CMMString CDUIGlobal::GetDuiLastError()
{
	return m_strLastError;
}

void CDUIGlobal::SetDuiLastError(CMMString strError)
{
	m_strLastError = strError;

	return;
}

bool CDUIGlobal::AddPreMessagePtr(IDuiPreMessage *pInterface)
{
	if (find(m_vecPreMessage.begin(), m_vecPreMessage.end(), pInterface) != m_vecPreMessage.end()) return true;

	m_vecPreMessage.push_back(pInterface);

	return true;
}

bool CDUIGlobal::RemovePreMessagePtr(IDuiPreMessage *pInterface)
{
	m_vecPreMessage.erase(std::remove(m_vecPreMessage.begin(), m_vecPreMessage.end(), pInterface), m_vecPreMessage.end());

	return true;
}

bool CDUIGlobal::TranslateMessage(const LPMSG pMsg)
{
	if (NULL == pMsg) return false;

	bool bHandled = false;

	//pre message
	for (int n = m_vecPreMessage.size() - 1; n >= 0; n--)
	{
		IDuiPreMessage *pPreMessage = m_vecPreMessage[n];
		if (NULL == pPreMessage)
		{
			assert(false);
			break;
		}

		pPreMessage->OnPreWndMessage(pMsg->hwnd, pMsg->message, pMsg->wParam, pMsg->lParam, bHandled);

		if (bHandled)
		{
			return true;
		}
	}

	return false;
}

MapWnd CDUIGlobal::GetWndAll()
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	return m_mapWnd;
}

tagDuiFile CDUIGlobal::GetWndInfo(CDUIWnd *pWnd)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	auto FindIt = m_mapWnd.find(pWnd);
	if (FindIt == m_mapWnd.end()) return {};

	return FindIt->second;
}

CDUIWnd * CDUIGlobal::GetWndByHandle(HWND hWnd)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	auto FindIt = find_if(m_mapWnd.begin(), m_mapWnd.end(), [=](const std::pair<CDUIWnd *, tagDuiFile> &Wnd)
	{
		return Wnd.first->GetWndHandle() == hWnd;
	});
	if (FindIt != m_mapWnd.end())
	{
		return FindIt->first;
	}

	return NULL;
}

void CDUIGlobal::PerformSwitchRes(int nIndexRes)
{
	if (m_nIndexSwitchRes == nIndexRes) return;

	m_nIndexSwitchRes = nIndexRes;

	OnResourceSwitch(GetSwitchResIndex());

	return;
}

int CDUIGlobal::GetSwitchResIndex()
{
	return m_nIndexSwitchRes;
}

const CMMDpi & CDUIGlobal::GetDpiObj()
{
	return m_DpiInfo;
}

int CDUIGlobal::GetDpi()
{
	return m_DpiInfo.GetDpi();
}

bool CDUIGlobal::SetDpi(int nDpi)
{
	if (nDpi < 96 || nDpi == GetDpi()) return false;

	int nScalePre = GetScale();

	m_DpiInfo.SetDpi(nDpi);

	OnDpiChanged(nScalePre);

	return true;
}

int CDUIGlobal::GetScale()
{
	return m_DpiInfo.GetScale();
}

bool CDUIGlobal::SetScale(int nScale)
{
	if (nScale < 100 || nScale == GetScale()) return false;

	return SetDpi(MulDiv(nScale, 96, 100));
}

void CDUIGlobal::LoadWnd(const CMMString &strName, CDUIWnd *pWnd)
{
	//has
	auto FindIt = find_if(m_vecDui.begin(), m_vecDui.end(), [&](tagDuiFile &DuiFile)
	{
		return DuiFile.strName == strName;
	});
	if (FindIt == m_vecDui.end())
	{
		SetDuiLastError(CMMStrHelp::Format(_T("duiname:[%s]不存在\n"), strName.c_str()));

		return;
	}

	enDuiType DuiType = FindIt->DuiType;

	//load
	CMMString strFileFull = GetDuiPath(DuiType) + FindIt->strFile;
	CDUIXmlPack::LoadWnd(strFileFull, pWnd);

	if (pWnd)
	{
		RenameWnd(pWnd, strName);
		SetWndDuiType(pWnd, DuiType);
	}

	return;
}

CDUIControlBase * CDUIGlobal::LoadDui(const CMMString &strName, CDUIWnd *pWnd)
{
	//has
	auto FindIt = find_if(m_vecDui.begin(), m_vecDui.end(), [&](tagDuiFile &DuiFile)
	{
		return DuiFile.strName == strName;
	});
	if (FindIt == m_vecDui.end())
	{
		SetDuiLastError(CMMStrHelp::Format(_T("duiname:[%s]不存在\n"), strName.c_str()));

		return NULL;
	}

	enDuiType DuiType = FindIt->DuiType;
	CDUIControlBase *pRootCtrl = NULL;

	//model store
	if (DuiType_ModelListItem == DuiType
		|| DuiType_ModelTreeNode == DuiType)
	{
		auto FindStore = m_mapModelStore.find(strName);
		if (FindStore != m_mapModelStore.end() && FindStore->second)
		{
			pRootCtrl = FindStore->second->Clone();
		}	
	}

	//load
	if (NULL == pRootCtrl)
	{
		CMMString strFileFull = GetDuiPath(DuiType) + FindIt->strFile;
		pRootCtrl = CDUIXmlPack::LoadDui(strFileFull, pWnd);

		if (NULL == pRootCtrl) return NULL;

		if (DuiType_ModelListItem == DuiType
			|| DuiType_ModelTreeNode == DuiType)
		{
			m_mapModelStore[strName] = pRootCtrl->Clone();
		}
	}
	if (pWnd)
	{
		RenameWnd(pWnd, strName);
		SetWndDuiType(pWnd, DuiType);
	}

	return pRootCtrl;
}

CDUIControlBase * CDUIGlobal::ParseDui(tinyxml2::XMLElement *pNodeXml)
{
	if (NULL == pNodeXml) return NULL;

	return CDUIXmlPack::ParseDui(pNodeXml);
}

CDUIControlBase * CDUIGlobal::ParseDui(LPCTSTR lpszXml)
{
	if (NULL == lpszXml) return NULL;

	return CDUIXmlPack::ParseDui(lpszXml);
}

int CDUIGlobal::GetFontResourceCount()
{
	return m_mapResourceFont.size();
}

CDUIFontBase * CDUIGlobal::GetFontResource(const CMMString &strName)
{
	auto FindRes = m_mapResourceFont.find(strName);

	return FindRes != m_mapResourceFont.end() ? FindRes->second : NULL;
}

CDUIFontBase * CDUIGlobal::GetFontResource(int nIndex)
{
	int n = 0;
	for (auto FindIt = m_mapResourceFont.begin(); n <= nIndex && FindIt != m_mapResourceFont.end(); ++FindIt, n++)
	{
		if (n == nIndex)
		{
			return FindIt->second;
		}
	}

	return NULL;
}

CDUIFontBase * CDUIGlobal::GetFontResDefault()
{
	return GetFontResource(m_strFontResDefault);
}

MapDuiFontBase CDUIGlobal::GetFontResourceAll()
{
	return m_mapResourceFont;
}

bool CDUIGlobal::RemoveFontResource(const CMMString &strName)
{
	auto FindIt = m_mapResourceFont.find(strName);
	if (FindIt == m_mapResourceFont.end()) return true;
	if (NULL == FindIt->second || FindIt->second->IsDesign()) return false;

	OnResourceRemove(FindIt->second);

	MMSafeDelete(FindIt->second);
	m_mapResourceFont.erase(FindIt);

	//default
	if (m_strFontResDefault == strName)
	{
		m_strFontResDefault.clear();

		CDUIFontBase *pFontBase = GetFontResource(0);
		if (pFontBase)
		{
			m_strFontResDefault = pFontBase->GetResourceName();
		}
		else
		{
			m_strFontResDefault.clear();
		}
	}

	return true;
}

int CDUIGlobal::GetColorResourceCount()
{
	return m_mapResourceColor.size();
}

CDUIColorBase * CDUIGlobal::GetColorResource(const CMMString &strName)
{
	auto FindRes = m_mapResourceColor.find(strName);

	return FindRes != m_mapResourceColor.end() ? FindRes->second : NULL;
}

CDUIColorBase * CDUIGlobal::GetColorResource(int nIndex)
{
	int n = 0;
	for (auto FindIt = m_mapResourceColor.begin(); n <= nIndex && FindIt != m_mapResourceColor.end(); ++FindIt, n++)
	{
		if (n == nIndex)
		{
			return FindIt->second;
		}
	}

	return NULL;
}

CDUIColorBase * CDUIGlobal::GetColorResource(Gdiplus::ARGB dwColor)
{
	for (auto Item : m_mapResourceColor)
	{
		if (NULL == Item.second || dwColor != Item.second->GetColor()) continue;

		return Item.second;
	}

	return NULL;
}

MapDuiColorBase CDUIGlobal::GetColorResourceAll()
{
	return m_mapResourceColor;
}

bool CDUIGlobal::RemoveColorResource(const CMMString &strName)
{
	auto FindIt = m_mapResourceColor.find(strName);
	if (FindIt == m_mapResourceColor.end()) return true;
	if (NULL == FindIt->second || FindIt->second->IsDesign()) return false;

	OnResourceRemove(FindIt->second);

	MMSafeDelete(FindIt->second);
	m_mapResourceColor.erase(FindIt);

	return true;
}

int CDUIGlobal::GetImageResourceCount()
{
	return m_mapResourceImage.size();
}

CDUIImageBase* CDUIGlobal::GetImageResource(const CMMString &strName)
{
	auto FindRes = m_mapResourceImage.find(strName);

	return FindRes != m_mapResourceImage.end() ? FindRes->second : NULL;
}

CDUIImageBase * CDUIGlobal::GetImageResource(int nIndex)
{
	int n = 0;
	for (auto FindIt = m_mapResourceImage.begin(); n <= nIndex && FindIt != m_mapResourceImage.end(); ++FindIt, n++)
	{
		if (n == nIndex)
		{
			return FindIt->second;
		}
	}

	return NULL;
}

CDUIImageBase * CDUIGlobal::GetImageResourceByFile(const CMMString &strFileFull)
{
	auto FindRes = find_if(m_mapResourceImage.begin(), m_mapResourceImage.end(), [&](const std::pair<CMMString, CDUIImageBase*> &ResourceImageItem)
	{
		return 0 == ResourceImageItem.second->GetImageFileFull().CompareNoCase(strFileFull);
	});

	return FindRes != m_mapResourceImage.end() ? FindRes->second : NULL;
}

MapDuiImageBase CDUIGlobal::GetImageResourceAll()
{
	return m_mapResourceImage;
}

bool CDUIGlobal::RemoveImageResource(const CMMString &strName)
{
	auto FindIt = m_mapResourceImage.find(strName);
	if (FindIt == m_mapResourceImage.end()) return true;

	CDUIImageBase *pImageBase = FindIt->second;
	if (NULL == pImageBase || pImageBase->IsDesign()) return false;

	m_mapResourceImage.erase(FindIt);

	CMMString strImage = pImageBase->GetImageFileFull();
	if (true == PathFileExists(strImage) && false == DeleteFile(strImage))
	{
		assert(false);
		return false;
	}

	OnResourceRemove(pImageBase);

	MMSafeDelete(pImageBase);

	return true;
}

int CDUIGlobal::GetDuiCount(enDuiType DuiType)
{
	int nCount = 0;
	for (auto It = m_vecDui.begin(); It != m_vecDui.end(); ++It)
	{
		if (DuiType == It->DuiType) nCount++;
	}

	return nCount;
}

CMMString CDUIGlobal::GetDuiName(int nIndex, enDuiType DuiType)
{
	int nFind = 0;
	for (auto It = m_vecDui.begin(); It != m_vecDui.end(); ++It)
	{
		if (DuiType == It->DuiType)
		{
			if (nFind == nIndex)
			{
				return It->strName;
			}

			nFind++;
		}
	}

	return _T("");
}

enDuiType CDUIGlobal::GetDuiType(const CMMString &strName)
{
	auto FindIt = find_if(m_vecDui.begin(), m_vecDui.end(), [&](tagDuiFile &DuiFile)
	{
		return DuiFile.strName == strName;
	});
	if (FindIt == m_vecDui.end())
	{
		return DuiType_Dlg;
	}

	return FindIt->DuiType;
}

CMMString CDUIGlobal::GetDuiFile(int nIndex, enDuiType DuiType)
{
	return GetDuiFile(GetDuiName(nIndex, DuiType));
}

CMMString CDUIGlobal::GetDuiFile(const CMMString &strName)
{
	auto FindIt = find_if(m_vecDui.begin(), m_vecDui.end(), [&](tagDuiFile &DuiFile)
	{
		return DuiFile.strName == strName;
	});
	if (FindIt == m_vecDui.end())
	{
		return _T("");
	}

	return FindIt->strFile;
}

CMMString CDUIGlobal::GetDuiFileFull(const CMMString &strName)
{
	enDuiType DuiType = GetDuiType(strName);
	CMMString strFile = GetDuiFile(strName);

	return GetDuiPath(DuiType) + strFile;
}

bool CDUIGlobal::RemoveDui(const CMMString &strName)
{
	if (strName.empty()) return false;

	auto FindIt = find_if(m_vecDui.begin(), m_vecDui.end(), [&](tagDuiFile &DuiFile)
	{
		return DuiFile.strName == strName;
	});
	if (FindIt == m_vecDui.end()) return false;

	//menu
	if (DuiType_Menu == FindIt->DuiType)
	{
		CDUIMenuCtrl *pMenu = dynamic_cast<CDUIMenuCtrl*>(LoadDui(strName, NULL));
		if (pMenu)
		{
			for (int i = 0; i < pMenu->GetMenuItemCount(); i++)
			{
				CDUIMenuItemCtrl *pMenuItem = pMenu->GetMenuItem(i);
				if (NULL == pMenuItem) continue;

				pMenuItem->SetHasExpandMenu(false);
			}
		}
	}

	//file
	CMMString strFile = GetDuiFileFull(strName);
	DeleteFile(strFile);

	m_vecDui.erase(FindIt);

	return true;
}

CMMString CDUIGlobal::GetProjectPath()
{
	return m_strProjectPath;
}

CMMString CDUIGlobal::GetProjectName()
{
	return m_strProjectName;
}

CMMString CDUIGlobal::GetProjectFileFull()
{
	return m_strProjectPath + m_strProjectName + Dui_Name_ProjectExt;
}

CMMString CDUIGlobal::GetSkinPath()
{
	return m_strSkinDir;
}

CMMString CDUIGlobal::GetDuiPath(enDuiType DuiType)
{
	switch (DuiType)
	{
		case DuiType_Dlg:
		{
			return m_strDlgDir;
		}
		case DuiType_View:
		{
			return m_strViewDir;
		}
		case DuiType_ModelListItem:
		{
			return m_strModelListItemDir;
		}
		case DuiType_ModelTreeNode:
		{
			return m_strModelTreeNodeDir;
		}
		case DuiType_Menu:
		{
			return m_strMenuDir;
		}
		case DuiType_3DMenu:
		{
			return m_str3DMenuDir;
		}
		case DuiType_Calendar:
		{
			return m_strCalendarDir;
		}
	}

	return _T("");
}

HINSTANCE CDUIGlobal::GetInstanceHandle()
{
	return m_hInstance;
}

HINSTANCE CDUIGlobal::GetResourceDll()
{
	return m_hInstanceResource ? m_hInstanceResource : m_hInstance;
}

HZIPDT CDUIGlobal::GetResourceZipHandle()
{
	return m_hResourceZip;
}

enDuiFileResType CDUIGlobal::GetDuiFileResType()
{
	return m_DuiFileResType;
}

bool CDUIGlobal::SaveProject()
{
#ifndef DUI_DESIGN
	return true;
#endif

	if (m_strProjectPath.empty() || m_strProjectName.empty()) return false;

	return CDUIXmlPack::SaveProject(m_strProjectPath, m_strProjectName, m_mapResourceFont, \
		m_mapResourceImage, m_mapResourceColor, m_vecDui, m_mapWnd, m_strFontResDefault);
}

bool CDUIGlobal::CloseProject(bool bSaveProject)
{
	if (bSaveProject)
	{
		SaveProject();
	}

	m_strProjectPath.clear();
	m_strProjectName.clear();
	m_strSkinDir.clear();
	m_strDlgDir.clear();
	m_strViewDir.clear();
	m_strModelListItemDir.clear();
	m_strModelTreeNodeDir.clear();
	m_strMenuDir.clear();
	m_str3DMenuDir.clear();
	m_strCalendarDir.clear();

	ReleaseResource();
	ReleaseDui();

	m_bProjectExist = false;

	return true;
}

void CDUIGlobal::AddCtrlID(UINT uCtrlID, CMMString strCtrlID)
{
	if ((0 < uCtrlID && uCtrlID < Dui_CtrlIDInner_Finish)
		|| m_mapControlIDValue.find(uCtrlID) != m_mapControlIDValue.end()) return;

	m_mapControlIDValue[uCtrlID] = strCtrlID;
	m_uMaxControlID = max(uCtrlID, m_uMaxControlID);

	return;
}

UINT CDUIGlobal::FindCtrlID(CMMString strCtrlID)
{
	for (auto &Item : m_mapControlIDValue)
	{
		if (Item.second == strCtrlID) return Item.first;
	}

	return 0;
}

CMMString CDUIGlobal::FindCtrlID(UINT uCtrlID)
{
	auto FindIt = m_mapControlIDValue.find(uCtrlID);
	if (FindIt == m_mapControlIDValue.end()) return _T("");

	return FindIt->second;
}

UINT CDUIGlobal::GenerateCtrlID()
{
	return ++m_uMaxControlID;
}

CMMString CDUIGlobal::GenerateCtrlID(CDUIControlBase *pControl)
{
	if (NULL == pControl) return _T("");

	UINT uCtrlID = pControl->GetCtrlID();
	0 == uCtrlID ? uCtrlID = GenerateCtrlID() : uCtrlID;
	CMMString strCtrlID = _T("IDC_") + pControl->GetDescribe() + CMMStrHelp::Format(_T("_%u"), uCtrlID);
	do
	{
		if (FindCtrlID(strCtrlID) <= 0) break;

		strCtrlID += _T("X");

	} while (true);

	return strCtrlID;
}

bool CDUIGlobal::ModifyCtrlID(CMMString strCtrlIDOld, CMMString strCtrlIDNew, CDUIControlBase *pControl)
{
	if (strCtrlIDNew.empty()) return false;

	//same
	if (FindCtrlID(strCtrlIDNew) > 0)
	{
		m_strLastError = _T("You Set Same String Ctrl ID");

		return false;
	}

	//modify
	for (auto &Item : m_mapControlIDValue)
	{
		if (Item.second == strCtrlIDOld)
		{
			Item.second = strCtrlIDNew;

#ifdef DUI_DESIGN
			return CDUIXmlPack::SaveCtrlID(m_mapControlIDValue);
#else
			return true;
#endif
		}
	}

	return false;
}

CMMString CDUIGlobal::CreateDlg()
{
	//name
	CMMString strName = _T("Dialog");
	CMMString strFile = GetDuiFile(strName);
	while (false == strFile.empty())
	{
		strName += _T("-");
		strName += CMMService::ProductGUID();
		strFile = GetDuiFile(strName);
	}

	strFile = strName + _T(".xml");

	//control
	CDUIWnd *pWnd = new CDUIWnd();
	CMMString strFileModel = CMMString(Dui_FolderUIModel) + _T("\\") + Dui_FileModelDlgXml;
	DuiFileResType_File == GetDuiFileResType() ? strFileModel = CMMService::GetWorkDirectory() + _T("\\") + strFileModel : strFileModel;
	CDUIContainerCtrl *pRootCtrl = MMDynamicPtr(CDUIContainerCtrl, CDUIXmlPack::LoadDui(strFileModel, pWnd));
	if (NULL == pRootCtrl)
	{
		pRootCtrl = new CDUIContainerCtrl;
		pRootCtrl->Init();
		pRootCtrl->SetBkColor({ Name_ColorDefault });
	}

	if (NULL == pWnd || NULL == pRootCtrl) return _T("");

	pWnd->AttachRootCtrl(pRootCtrl);
	CDUIXmlPack::SaveXmlUI(GetDuiPath(DuiType_Dlg) + strFile, pWnd);
	MMSafeDelete(pWnd);

	AddDui(DuiType_Dlg, strName, strFile);

	return strName;
}

CMMString CDUIGlobal::CreateView()
{
	//name
	CMMString strName = _T("View");
	CMMString strFile = GetDuiFile(strName);
	while (false == strFile.empty())
	{
		strName += _T("-");
		strName += CMMService::ProductGUID();
		strFile = GetDuiFile(strName);
	}

	strFile = strName + _T(".xml");

	//control
	CDUIWnd *pWnd = new CDUIWnd();
	CMMString strFileModel = CMMString(Dui_FolderUIModel) + _T("\\") + Dui_FileModelViewXml;
	DuiFileResType_File == GetDuiFileResType() ? strFileModel = CMMService::GetWorkDirectory() + _T("\\") + strFileModel : strFileModel;
	CDUIContainerCtrl *pRootCtrl = MMDynamicPtr(CDUIContainerCtrl, CDUIXmlPack::LoadDui(strFileModel, pWnd));
	if (pRootCtrl)
	{
		pRootCtrl->RefreshCtrlID();
	}
	if (NULL == pRootCtrl)
	{
		pRootCtrl = new CDUIContainerCtrl;
		pRootCtrl->SetBkColor({ Name_ColorDefault });
	}

	if (NULL == pWnd || NULL == pRootCtrl) return _T("");

	pWnd->Init();
	pRootCtrl->Init();
	pWnd->AttachRootCtrl(pRootCtrl);
	CDUIXmlPack::SaveXmlUI(GetDuiPath(DuiType_View) + strFile, pWnd);
	MMSafeDelete(pWnd);

	AddDui(DuiType_View, strName, strFile);

	return strName;
}

CMMString CDUIGlobal::CreateModelListItem()
{
	//name
	CMMString strName = _T("ModelListItem");
	CMMString strFile = GetDuiFile(strName);
	while (false == strFile.empty())
	{
		strName += _T("-");
		strName += CMMService::ProductGUID();
		strFile = GetDuiFile(strName);
	}

	strFile += strName + _T(".xml");

	//control
	CDUIWnd *pWnd = new CDUIWnd();
	CDUIListItemCtrl *pRootCtrl = new CDUIListItemCtrl;

	if (NULL == pWnd || NULL == pRootCtrl) return _T("");

	pWnd->Init();
	pRootCtrl->Init();
	pRootCtrl->SetPadding(0, 0, 0, 0);
	pRootCtrl->SetFixedHeight(30);
	pRootCtrl->SetBkColor({ Name_ColorDefault });
	pRootCtrl->SetText(pRootCtrl->GetDescribe());
	pRootCtrl->SetBkColor({ Name_ColorGray });

	tagDuiTextStyle TextStyle;
	TextStyle.dwTextStyle |= DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS;
	pRootCtrl->SetTextStyle(TextStyle);
	pWnd->AttachRootCtrl(pRootCtrl);
	CDUIXmlPack::SaveXmlUI(GetDuiPath(DuiType_ModelListItem) + strFile, pWnd);
	MMSafeDelete(pWnd);

	AddDui(DuiType_ModelListItem, strName, strFile);

	return strName;
}

CMMString CDUIGlobal::CreateModelTreeNode()
{
	//name
	CMMString strName = _T("ModelTreeNode");
	CMMString strFile = GetDuiFile(strName);
	while (false == strFile.empty())
	{
		strName += _T("-");
		strName += CMMService::ProductGUID();
		strFile = GetDuiFile(strName);
	}

	strFile += strName + _T(".xml");

	//control
	CDUIWnd *pWnd = new CDUIWnd();
	CDUITreeNodeCtrl *pRootCtrl = new CDUITreeNodeCtrl;

	if (NULL == pWnd || NULL == pRootCtrl) return _T("");

	pWnd->Init();
	pRootCtrl->Init();
	pRootCtrl->SetPadding(0, 0, 0, 0);
	pRootCtrl->SetFixedHeight(30);
	pWnd->AttachRootCtrl(pRootCtrl);
	CDUIXmlPack::SaveXmlUI(GetDuiPath(DuiType_ModelTreeNode) + strFile, pWnd);
	MMSafeDelete(pWnd);

	AddDui(DuiType_ModelTreeNode, strName, strFile);

	return strName;
}

CMMString CDUIGlobal::CreateMenu(CDUIWnd *pWndParentMenu)
{
	//name
	CMMString strName = pWndParentMenu ? Dui_Resource_Key_SubMenu : ("Menu");
	CMMString strFile = GetDuiFile(strName);
	while (false == strFile.empty())
	{
		strName += _T("-");
		strName += CMMService::ProductGUID();
		strFile = GetDuiFile(strName);
	}

	//file
	strFile = strName + _T(".xml");

	//control
	CDUIWnd *pWnd = new CDUIWnd();
	CMMString strFileModel = CMMString(Dui_FolderUIModel) + _T("\\") + Dui_FileModelMenuXml;
	DuiFileResType_File == GetDuiFileResType() ? strFileModel = CMMService::GetWorkDirectory() + _T("\\") + strFileModel : strFileModel;
	CDUIContainerCtrl *pRootCtrl = MMDynamicPtr(CDUIContainerCtrl, CDUIXmlPack::LoadDui(strFileModel, pWnd));
	if (NULL == pRootCtrl)
	{
		pRootCtrl = new CDUIMenuCtrl;
		pRootCtrl->Init();
		pRootCtrl->SetBkColor({ Name_ColorDefault });
	}
	if (NULL == pWnd || NULL == pRootCtrl)
	{
		return _T("");
	}
	if (pWndParentMenu)
	{
		(*pWnd).CDUIPropertyObject::operator=(*pWndParentMenu);
		(*pRootCtrl).CDUIPropertyObject::operator=(*pWndParentMenu->GetRootCtrl());
	}

	pWnd->AttachRootCtrl(pRootCtrl);
	pWnd->SetCaptionHeight(0);
	CDUIXmlPack::SaveXmlUI(GetDuiPath(DuiType_Menu) + strFile, pWnd);
	MMSafeDelete(pWnd);

	AddDui(DuiType_Menu, strName, strFile);

	return strName;
}

CMMString CDUIGlobal::Create3DMenu()
{
	//name
	CMMString strName = _T("3DMenu");
	CMMString strFile = GetDuiFile(strName);
	while (false == strFile.empty())
	{
		strName += _T("-");
		strName += CMMService::ProductGUID();
		strFile = GetDuiFile(strName);
	}

	//file
	strFile = strName + _T(".xml");

	//control
	CDUIWnd *pWnd = new CDUIWnd();
	CMMString strFileModel = CMMString(Dui_FolderUIModel) + _T("\\") + Dui_FileModel3DMenuXml;
	DuiFileResType_File == GetDuiFileResType() ? strFileModel = CMMService::GetWorkDirectory() + _T("\\") + strFileModel : strFileModel;
	CDUIContainerCtrl *pRootCtrl = MMDynamicPtr(CDUIContainerCtrl, CDUIXmlPack::LoadDui(strFileModel, pWnd));
	if (NULL == pRootCtrl)
	{
		pRootCtrl = new CDUIRotateMenuCtrl;
		pRootCtrl->Init();
		pRootCtrl->SetBkColor({ Name_ColorDefault });
	}

	if (NULL == pWnd || NULL == pRootCtrl) return _T("");

	pWnd->AttachRootCtrl(pRootCtrl);
	CDUIXmlPack::SaveXmlUI(GetDuiPath(DuiType_3DMenu) + strFile, pWnd);
	MMSafeDelete(pWnd);

	AddDui(DuiType_3DMenu, strName, strFile);

	return strName;
}

CMMString CDUIGlobal::CreateCalendar()
{
	//name
	CMMString strName = _T("Calendar");
	CMMString strFile = GetDuiFile(strName);
	while (false == strFile.empty())
	{
		strName += _T("-");
		strName += CMMService::ProductGUID();
		strFile = GetDuiFile(strName);
	}

	//file
	strFile = strName + _T(".xml");

	//control
	CDUIWnd *pWnd = new CDUIWnd();
	CMMString strFileModel = CMMString(Dui_FolderUIModel) + _T("\\") + Dui_FileModelCalendarXml;
	DuiFileResType_File == GetDuiFileResType() ? strFileModel = CMMService::GetWorkDirectory() + _T("\\") + strFileModel : strFileModel;
	CDUIContainerCtrl *pRootCtrl = MMDynamicPtr(CDUIContainerCtrl, CDUIXmlPack::LoadDui(strFileModel, pWnd));
	if (NULL == pRootCtrl)
	{
		pRootCtrl = new CDUICalendarCtrl;
		pRootCtrl->Init();
		pRootCtrl->SetBkColor({ Name_ColorDefault });
	}

	if (NULL == pWnd || NULL == pRootCtrl) return _T("");

	pWnd->AttachRootCtrl(pRootCtrl);
	CDUIXmlPack::SaveXmlUI(GetDuiPath(DuiType_Calendar) + strFile, pWnd);
	MMSafeDelete(pWnd);

	AddDui(DuiType_Calendar, strName, strFile);

	return strName;
}

bool CDUIGlobal::SaveDui(LPCTSTR lpszName, CDUIWnd *pWnd)
{
	auto FindIt = m_mapModelStore.find(lpszName);
	if (FindIt != m_mapModelStore.end())
	{
		MMSafeDelete(FindIt->second);
		m_mapModelStore.erase(FindIt);
	}

	CMMString strFile;
	strFile = GetDuiFileFull(lpszName);

	return CDUIXmlPack::SaveXmlUI(strFile, pWnd);
}

CMMString CDUIGlobal::SaveDui(CDUIPropertyObject *pPropObj, bool bIncludeChild)
{
	return CDUIXmlPack::SaveDui(pPropObj, bIncludeChild);
}

bool CDUIGlobal::RefreshAttibute(tinyxml2::XMLElement *pNodeXml, CDUIPropertyObject *pPropObj)
{
	if (NULL == pNodeXml || NULL == pPropObj) return false;

	return CDUIXmlPack::RefreshAttibute(pNodeXml, pPropObj);
}

void CDUIGlobal::LoadPublicResource()
{
	//image
	for (auto &ImageItem : g_mapPublicImage)
	{
		auto pImageBase = new CDUIImageBase(ImageItem.first, ImageItem.second);
		pImageBase->SetDesign(true);
		if (false == AddResource(pImageBase))
		{
			MMSafeDelete(pImageBase);
		}
	}

	//color
	for (auto &lpszColor : g_szPublicColor)
	{
		auto pColorBase = new CDUIColorBase(lpszColor, _tcstoul(lpszColor, NULL, 16));
		pColorBase->SetDesign(true);
		if (false == AddResource(pColorBase))
		{
			MMSafeDelete(pColorBase);
		}
	}

	//font
	for (auto &lgFont : g_PublicFont)
	{
		CMMString strName = CDUIFontBase::FormatFontDescribe(lgFont.lfFaceName, lgFont.lfHeight, lgFont.lfWeight, lgFont.lfItalic, lgFont.lfUnderline, lgFont.lfStrikeOut);
		auto pFontBase = new CDUIFontBase(strName, lgFont.lfFaceName, lgFont.lfHeight, lgFont.lfWeight, lgFont.lfItalic, lgFont.lfUnderline, lgFont.lfStrikeOut);
		pFontBase->SetDesign(true);
		if (false == AddResource(pFontBase))
		{
			MMSafeDelete(pFontBase);
		}
	}

	//ctrlid
	DuiInitCtrlIDItem(Dui_CtrlIDInner_BtnClose);
	DuiInitCtrlIDItem(Dui_CtrlIDInner_BtnMin);
	DuiInitCtrlIDItem(Dui_CtrlIDInner_BtnRestore);
	DuiInitCtrlIDItem(Dui_CtrlIDInner_BtnMax);
	DuiInitCtrlIDItem(Dui_CtrlIDInner_BtnOk);
	DuiInitCtrlIDItem(Dui_CtrlIDInner_BtnCancel);
	DuiInitCtrlIDItem(Dui_CtrlIDInner_DlgRoot);
	DuiInitCtrlIDItem(Dui_CtrlIDInner_DlgCaption);
	DuiInitCtrlIDItem(Dui_CtrlIDInner_DlgLogo);
	DuiInitCtrlIDItem(Dui_CtrlIDInner_DlgTitle);
	DuiInitCtrlIDItem(Dui_CtrlIDInner_DragTmp);
	DuiInitCtrlIDItem(Dui_CtrlIDInner_ListItemEdit);
	DuiInitCtrlIDItem(Dui_CtrlIDInner_ListItemSelect);
	DuiInitCtrlIDItem(Dui_CtrlIDInner_HeaderItemSelect);
	DuiInitCtrlIDItem(Dui_CtrlIDInner_HeaderItemCheck);
	DuiInitCtrlIDItem(Dui_CtrlIDInner_TreeNodeVertContainer);
	DuiInitCtrlIDItem(Dui_CtrlIDInner_TreeNodeHorizContainer);
	DuiInitCtrlIDItem(Dui_CtrlIDInner_TreeNodeExpand);
	DuiInitCtrlIDItem(Dui_CtrlIDInner_TreeNodeView);
	DuiInitCtrlIDItem(Dui_CtrlIDInner_SpinEditValue);
	DuiInitCtrlIDItem(Dui_CtrlIDInner_SpinVertRight);
	DuiInitCtrlIDItem(Dui_CtrlIDInner_SpinBtnUp);
	DuiInitCtrlIDItem(Dui_CtrlIDInner_SpinBtnDown);
	DuiInitCtrlIDItem(Dui_CtrlIDInner_CalendarListDay);
	DuiInitCtrlIDItem(Dui_CtrlIDInner_CalendarBtnLeft);
	DuiInitCtrlIDItem(Dui_CtrlIDInner_CalendarBtnRight);
	DuiInitCtrlIDItem(Dui_CtrlIDInner_CalendarBtnYear);
	DuiInitCtrlIDItem(Dui_CtrlIDInner_CalendarBtnMonth);
	DuiInitCtrlIDItem(Dui_CtrlIDInner_ColorPaletteEdit);
	DuiInitCtrlIDItem(Dui_CtrlIDInner_DefaultBtn);
	DuiInitCtrlIDItem(Dui_CtrlIDInner_DefaultHScrollBar);
	DuiInitCtrlIDItem(Dui_CtrlIDInner_DefaultVScrollBar);

	return;
}

void CDUIGlobal::LoadConfigCtrl(const CMMString &strConfigFile)
{
	return;

	tinyxml2::XMLDocument xmlDoc;
	bool bRes = true;
	if (DuiFileResType_File == GetDuiFileResType())
	{
		CMMString strFile = CMMService::GetWorkDirectory();
		strFile = strFile + _T("\\") + Dui_FolderConfig + _T("\\") + strConfigFile;

		bRes = (XML_SUCCESS == xmlDoc.LoadFile((LPCSTR)CT2CA(strFile)));
	}
	if (false == bRes)
	{
		std::vector<BYTE> vecData;
		CMMString strFile = strConfigFile;
		bRes = (ExtractResourceData(vecData, strFile)
			&& false == vecData.empty()
			&& XML_SUCCESS == xmlDoc.Parse((const char*)vecData.data(), vecData.size()));
	}
	if (false == bRes)
	{
		assert(false);
		CMMString strWarning = CMMStrHelp::Format(_T("Failed of Load [%s]，Please Pack Your Project From DUIThink"), (LPCTSTR)strConfigFile);
		MessageBox(NULL, strWarning, NULL, NULL);

		return;
	}

	tinyxml2::XMLElement *pXmlRoot = xmlDoc.RootElement();
	if (NULL == pXmlRoot) return;

	tinyxml2::XMLElement *pNodeXml = pXmlRoot->FirstChildElement();
	if (NULL == pNodeXml) return;

	do
	{
		CMMString strName = pNodeXml->Name();

		if (Dui_Name_NodeExtendCtrl == strName || Dui_Name_NodeCustomCtrl == strName)
		{
			tinyxml2::XMLElement *pNodeXmlSub = pNodeXml->FirstChildElement();
			if (NULL == pNodeXmlSub) continue;

			do
			{
#ifdef DUI_DESIGN
				CMMString strDllName = pNodeXmlSub->Attribute((LPCSTR)CT2CA(Dui_Key_DllNameDesign));
#else
				CMMString strDllName = pNodeXmlSub->Attribute((LPCSTR)CT2CA(Dui_Key_DllNamePublish));
#endif

				if (true == strDllName.empty()) continue;

				HMODULE hModule = LoadLibrary(strDllName);
				if (NULL == hModule)
				{
					assert(false);
					CMMString strWarning;
					strWarning.Format(_T("Failed load extenddll【%s】, Make sure it in the running directory"), strDllName.c_str());
					MessageBox(NULL, strWarning, NULL, NULL);

					continue;
				}

				m_vecModuleExtendDll.push_back(hModule);

			} while (pNodeXmlSub = pNodeXmlSub->NextSiblingElement(), pNodeXmlSub);

			continue;
		}

	} while (pNodeXml = pNodeXml->NextSiblingElement(), pNodeXml);

	return;
}

bool CDUIGlobal::SetProjectPath(LPCTSTR lpszPath)
{
	if (DuiFileResType_File == GetDuiFileResType()
		&& (false == ::PathFileExists(lpszPath) || false == ::PathIsDirectory(lpszPath))) return false;

	m_strProjectPath = lpszPath;
	m_strSkinDir = m_strProjectPath + Dui_Resource_ImageRes + _T('\\');
	m_strDlgDir = m_strProjectPath + Dui_Resource_Dlg + _T('\\');
	m_strViewDir = m_strProjectPath + Dui_Resource_View + _T('\\');
	m_strModelListItemDir = m_strProjectPath + Dui_Resource_ModelListItem + _T('\\');
	m_strModelTreeNodeDir = m_strProjectPath + Dui_Resource_ModelTreeNode + _T('\\');
	m_strMenuDir = m_strProjectPath + Dui_Resource_Menu + _T('\\');
	m_str3DMenuDir = m_strProjectPath + Dui_Resource_3DMenu + _T('\\');
	m_strCalendarDir = m_strProjectPath + Dui_Resource_Calendar + _T('\\');

	return true;
}

bool CDUIGlobal::SetProjectName(LPCTSTR lpszName)
{
	if (MMInvalidString(lpszName)) return false;

	m_strProjectName = lpszName;

	return true;
}

bool CDUIGlobal::SetResVersion(enDuiResVersion ResVersion)
{
	m_DuiResVersion = ResVersion;

	return true;
}

bool CDUIGlobal::AddResource(CDUIResourceBase *pResourceObj)
{
	if (NULL == pResourceObj || MMInvalidString(pResourceObj->GetResourceName())) return false;

	pResourceObj->SetIndex(m_uMaxResourceIndex++);

	switch (pResourceObj->GetResourceType())
	{
		case DuiResType_Font:
		{
			return AddFontResource(MMStaticPtr(CDUIFontBase, pResourceObj));
		}
		case DuiResType_Image:
		{
			return AddImageResource(MMStaticPtr(CDUIImageBase, pResourceObj));
		}
		case DuiResType_Color:
		{
			return AddColorResource(MMStaticPtr(CDUIColorBase, pResourceObj));
		}
		default:
		{
			break;
		}
	}

	return false;
}

bool CDUIGlobal::ExtractResourceData(vector<BYTE> &vecData, CMMString strFile)
{
	bool bFullPath = true;
#if defined(DuiPlatform_SDL) && !defined(WIN32)
	if (strFile.empty() || _T('/') != strFile[0])
	{
		bFullPath = false;
	}

	strFile.Replace(_T('\\'), _T('/'));
#else
	if (-1 == strFile.find(_T(':')))
	{
		bFullPath = false;
	}
#endif

	do
	{
		//full path
		if (bFullPath) break;

		//zip
		if (DuiFileResType_Zip == GetDuiFileResType()
			|| DuiFileResType_ResZip == GetDuiFileResType())
		{
			HZIPDT hz = (HZIPDT)GetResourceZipHandle();
			if (NULL == hz) break;

			ZIPENTRYDT ze = {};
			int i = 0;

			strFile.Replace(_T("\\"), _T("/"));
			if (0 != FindZipItem(hz, strFile, true, &i, &ze)) break;

			DWORD dwSize = ze.unc_size;
			if (0 == dwSize) return false;

			vecData.resize(dwSize);
			int res = UnzipItem(hz, i, vecData.data(), dwSize, 3);
			if (0x00000000 != res && 0x00000600 != res)
			{
				vecData.clear();
				return false;
			}
		}

	} while (false);

	//file
	if (DuiFileResType_File == GetDuiFileResType() || vecData.empty())
	{
		//full path
		if (false == bFullPath)
		{
			strFile = GetProjectPath() + strFile;
		}

		return CMMFile::GetFileData(strFile, vecData);
	}

	return true;
}

bool CDUIGlobal::RegisterResourceCallBack(IDuiResourceCallBack *pIResourceCallBack)
{
	if (NULL == pIResourceCallBack) return false;

	if (find(m_vecIResourceCallBack.begin(), m_vecIResourceCallBack.end(), pIResourceCallBack) != m_vecIResourceCallBack.end()) return false;

	m_vecIResourceCallBack.push_back(pIResourceCallBack);

	return true;
}

bool CDUIGlobal::UnRegisterResourceCallBack(IDuiResourceCallBack *pIResourceCallBack)
{
	if (NULL == pIResourceCallBack) return false;

	auto FindIt = find(m_vecIResourceCallBack.begin(), m_vecIResourceCallBack.end(), pIResourceCallBack);
	if (FindIt == m_vecIResourceCallBack.end()) return false;

	m_vecIResourceCallBack.erase(FindIt);

	return true;
}

bool CDUIGlobal::AddFontResource(CDUIFontBase *pResourceObj)
{
	if (NULL == pResourceObj) return false;

	auto pFontBase = GetFontResource(pResourceObj->GetResourceName());
	if (pFontBase)
	{
		//::MessageBox(NULL, _T("Have Same Name Resource"), NULL, NULL);

		return false;
	}

	m_mapResourceFont[pResourceObj->GetResourceName()] = pResourceObj;

	//default
	if (m_strFontResDefault.empty())
	{
		m_strFontResDefault = pResourceObj ? pResourceObj->GetResourceName() : _T("");
	}

	//notify
	OnResourceAdd(pResourceObj);

	return true;
}

bool CDUIGlobal::AddImageResource(CDUIImageBase *pResourceObj)
{
	if (NULL == pResourceObj) return false;

	auto pImageBase = GetImageResource(pResourceObj->GetResourceName());
	if (pImageBase)
	{
		//::MessageBox(NULL, _T("Have Same Name Resource"), NULL, NULL);

		return false;
	}

	m_mapResourceImage[pResourceObj->GetResourceName()] = pResourceObj;

	//notify
	OnResourceAdd(pResourceObj);

	return true;
}

bool CDUIGlobal::AddColorResource(CDUIColorBase *pResourceObj)
{
	if (NULL == pResourceObj) return false;

	auto pColorBase = GetColorResource(pResourceObj->GetResourceName());
	if (pColorBase)
	{
		//::MessageBox(NULL, _T("Have Same Name Resource"), NULL, NULL);

		return false;
	}

	m_mapResourceColor[pResourceObj->GetResourceName()] = pResourceObj;

	//notify
	OnResourceAdd(pResourceObj);

	return true;
}

bool CDUIGlobal::AddDui(enDuiType DuiType, const CMMString &strName, const CMMString &strFile)
{
	CMMString strFileFull = GetDuiPath(DuiType) + strFile;
	if (DuiFileResType_File == GetDuiFileResType() && false == PathFileExists(strFileFull))
	{
		assert(false);
		return false;
	}

	CMMString strDuiFile = GetDuiFile(strName);
	if (false == strDuiFile.empty())
	{
		::MessageBox(NULL, _T("Have Same Name DirectUI"), NULL, NULL);

		return false;
	}

	m_vecDui.push_back(tagDuiFile{ DuiType, strName, strFile });

	return true;
}

bool CDUIGlobal::RenameFontResource(const CMMString &strNameOld, const CMMString &strNameNew)
{
	if (strNameOld.empty() || strNameNew.empty()) return false;

	CDUIFontBase *pFontBaseOld = GetFontResource(strNameOld);
	CDUIFontBase *pFontBaseNew = GetFontResource(strNameNew);
	if (NULL == pFontBaseOld || pFontBaseNew) return false;
	if (false == pFontBaseOld->SetResourceName(strNameNew)) return false;

	m_mapResourceFont.erase(strNameOld);
	m_mapResourceFont[strNameNew] = pFontBaseOld;

	//default
	if (m_strFontResDefault == strNameOld)
	{
		m_strFontResDefault = strNameNew;
	}

	OnResourceRename(pFontBaseOld, strNameOld);

	return true;
}

bool CDUIGlobal::RenameImageResource(const CMMString &strNameOld, const CMMString &strNameNew)
{
	if (strNameOld.empty() || strNameNew.empty()) return false;

	CDUIImageBase *pImageBaseOld = GetImageResource(strNameOld);
	CDUIImageBase *pImageBaseNew = GetImageResource(strNameNew);
	if (NULL == pImageBaseOld || pImageBaseNew) return false;
	if (false == pImageBaseOld->SetResourceName(strNameNew)) return false;

	m_mapResourceImage.erase(strNameOld);
	m_mapResourceImage[strNameNew] = pImageBaseOld;

	OnResourceRename(pImageBaseOld, strNameOld);

	return true;
}

bool CDUIGlobal::RenameColorResource(const CMMString &strNameOld, const CMMString &strNameNew)
{
	if (strNameOld.empty() || strNameNew.empty()) return false;

	CDUIColorBase *pColorBaseOld = GetColorResource(strNameOld);
	CDUIColorBase *pColorBaseNew = GetColorResource(strNameNew);

	if (NULL == pColorBaseOld || pColorBaseNew) return false;
	if (false == pColorBaseOld->SetResourceName(strNameNew)) return false;

	m_mapResourceColor.erase(strNameOld);
	m_mapResourceColor[strNameNew] = pColorBaseOld;

	OnResourceRename(pColorBaseOld, strNameOld);

	return true;
}

bool CDUIGlobal::RenameDui(const CMMString &strNameOld, const CMMString &strNameNew)
{
	//find old
	auto FindDuiOld = find_if(m_vecDui.begin(), m_vecDui.end(), [&](tagDuiFile &DuiFile)
	{
		return DuiFile.strName == strNameOld;
	});
	auto FindDuiNew = find_if(m_vecDui.begin(), m_vecDui.end(), [&](tagDuiFile &DuiFile)
	{
		return DuiFile.strName == strNameNew;
	});
	if (FindDuiOld == m_vecDui.end() || FindDuiNew != m_vecDui.end()) return false;

	//modify file
	enDuiType DuiType = FindDuiOld->DuiType;
	CMMString strFile = FindDuiOld->strFile;

	CMMString strFileOld = GetDuiFileFull(strNameOld);
	CMMString strFileNameNew = strNameNew + _T(".xml");
	CMMString strFileNew = GetDuiPath(DuiType) + strFileNameNew;

	if (false == MoveFile(strFileOld, strFileNew))
	{
		MessageBox(NULL, _T("Error Because Same Filename。"), _T("提示"), MB_ICONINFORMATION);
		return false;
	}

	//remove
	m_vecDui.erase(FindDuiOld);

	//add
	if (false == AddDui(DuiType, strNameNew, strFileNameNew))
	{
		return false;
	}

	//wndmanager
	RenameWnd(strNameOld, strNameNew);

	SaveProject();

	return true;
}

void CDUIGlobal::SetFontResDefault(const CMMString &strName)
{
	if (strName.empty()) return;

	m_strFontResDefault = strName;

	return;
}

void CDUIGlobal::OnDpiChanged(int nScalePre)
{
	//has load project
	if (false == IsLoadProject()) return;

	//manager
	{
		std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

		for (auto Wnd : m_mapWnd)
		{
			if (NULL == Wnd.first) continue;

			Wnd.first->OnDpiChanged(nScalePre);
		}
	}

	//font
	for (auto &ResourceFontItem : m_mapResourceFont)
	{
		if (NULL == ResourceFontItem.second) continue;

		ResourceFontItem.second->OnDpiChanged(GetScale());
	}

	//image
	for (auto &ResourceImageItem : m_mapResourceImage)
	{
		if (NULL == ResourceImageItem.second) continue;

		ResourceImageItem.second->OnDpiChanged(GetScale());
	}

	return;
}

void CDUIGlobal::OnResourceAdd(CDUIResourceBase *pResourceObj)
{
	if (false == IsLoadProject()) return;

	for (int n = 0; n < m_vecIResourceCallBack.size(); n++)
	{
		IDuiResourceCallBack *pIResourceCallBack = m_vecIResourceCallBack[n];
		if (NULL == pIResourceCallBack) continue;

		pIResourceCallBack->OnResourceAdd(pResourceObj);
	}

	return;
}

void CDUIGlobal::OnResourceRemove(CDUIResourceBase *pResourceObj)
{
	if (NULL == pResourceObj || false == IsLoadProject()) return;

	{
		std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

		for (auto Wnd : m_mapWnd)
		{
			if (NULL == Wnd.first) continue;

			Wnd.first->Invalidate();
		}
		for (int n = 0; n < m_vecIResourceCallBack.size(); n++)
		{
			IDuiResourceCallBack *pIResourceCallBack = m_vecIResourceCallBack[n];
			if (NULL == pIResourceCallBack) continue;

			pIResourceCallBack->OnResourceRemove(pResourceObj);
		}
	}

	return;
}

void CDUIGlobal::OnResourceRename(CDUIResourceBase *pResourceObj, const CMMString &strNameOld)
{
	if (NULL == pResourceObj || false == IsLoadProject()) return;

	{
		std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

		for (int n = 0; n < m_vecIResourceCallBack.size(); n++)
		{
			IDuiResourceCallBack *pIResourceCallBack = m_vecIResourceCallBack[n];
			if (NULL == pIResourceCallBack) continue;

			pIResourceCallBack->OnResourceRename(pResourceObj, strNameOld);
		}
	}

	return;
}

void CDUIGlobal::OnResourceSwitch(int nIndexRes)
{
	//manager
	{
		std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

		for (auto Wnd : m_mapWnd)
		{
			if (NULL == Wnd.first) continue;

			Wnd.first->Invalidate();
		}
	}

	return;
}

LPCTSTR CDUIGlobal::GetAttriName(uint32_t uValueID)
{
	if (uValueID <= 0) return _T("");

	auto FindIt = m_mapAttriNameValue.find(uValueID);
	if (FindIt == m_mapAttriNameValue.end()) return _T("");

	return FindIt->second.c_str();
}

uint32_t CDUIGlobal::SetAttriName(LPCTSTR lpszName)
{
	uint32_t uID = CMMHash::GetHash(lpszName);
	m_mapAttriNameValue[uID] = lpszName;

	return uID;
}

bool CDUIGlobal::SetAttriName(tinyxml2::XMLElement *pNode)
{
	if (NULL == pNode) return false;

	uint32_t uValueID = 0;
	LPCSTR lpszName = NULL;

	do
	{
		const tinyxml2::XMLAttribute *pNodeAttribute = pNode->FirstAttribute();
		if (NULL == pNodeAttribute) break;

		do
		{
			if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriObjValueID))
			{
				uValueID = strtoul(pNodeAttribute->Value(), NULL, 10);

				continue;
			}
			if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriObjName))
			{
				lpszName = pNodeAttribute->Value();

				continue;
			}

		} while (pNodeAttribute = pNodeAttribute->Next(), pNodeAttribute);

	} while (false);

	if (NULL == lpszName || uValueID <= 0) return true;

	switch (m_DuiResVersion)
	{
		case DuiResVersion_0:
		{
			m_mapAttriNameValue[uValueID] = lpszName;

			break;
		}
		default:
		{
			m_mapAttriNameValue[uValueID] = CA2CT(lpszName, CP_UTF8);

			break;
		}
	}
	
	return true;
}

bool CDUIGlobal::SaveAttriName(tinyxml2::XMLElement *pNode)
{
	if (NULL == pNode) return false;

	for (auto &NameItem : m_mapAttriNameValue)
	{
		auto &strName = NameItem.second;
		if (strName.empty()) continue;

		uint32_t uID = CMMHash::GetHash(strName);
		if (uID != NameItem.first)
		{
			assert(false);
			
			continue;
		}

		tinyxml2::XMLElement *pValue = pNode->GetDocument()->NewElement("a");
		if (NULL == pValue)
		{
			assert(false);
			continue;
		}

		//id
		pValue->SetAttribute(Dui_Key_AttriObjValueID, NameItem.first);

		//name
		pValue->SetAttribute(Dui_Key_AttriObjName, (LPCSTR)CT2CA(strName, CP_UTF8));

		pNode->LinkEndChild(pValue);
	}

	return true;
}

tagDuiRichText CDUIGlobal::GetAttriRichText(uint32_t uValueID)
{
	if (uValueID <= 0) return {};

	auto FindIt = m_mapAttriRichTextValue.find(uValueID);

	return FindIt == m_mapAttriRichTextValue.end() ? tagDuiRichText{} : FindIt->second;
}

uint32_t CDUIGlobal::SetAttriRichText(const tagDuiRichText &RichText)
{
	uint32_t uID = RichText.GetID();
	DuiAttriModifyValue(RichText, m_mapAttriRichTextValue, uID);

	return uID;
}

bool CDUIGlobal::SetAttriRichText(tinyxml2::XMLElement *pNode)
{
	if (NULL == pNode) return false;

	uint32_t uValueID = 0;
	tagDuiRichText RichText;

	do
	{
		const tinyxml2::XMLAttribute *pNodeAttribute = pNode->FirstAttribute();
		if (NULL == pNodeAttribute) break;

		do
		{
			if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriObjValueID))
			{
				uValueID = strtoul(pNodeAttribute->Value(), NULL, 10);

				continue;
			}
			if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriVecRichTextItem))
			{
				RichText.vecRichTextItem = ParseVecRichTextItem(pNodeAttribute->Value());

				continue;
			}
			if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriRichTextStyle))
			{
				RichText.dwTextStyle = strtoul(pNodeAttribute->Value(), NULL, 10);

				continue;
			}
			if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriRichTextLineLimit))
			{
				RichText.nLineLimit = strtoul(pNodeAttribute->Value(), NULL, 10);

				continue;
			}

		} while (pNodeAttribute = pNodeAttribute->Next(), pNodeAttribute);

	} while (false);

	if (RichText.vecRichTextItem.empty() || uValueID <= 0) return true;

	m_mapAttriRichTextValue[uValueID] = (RichText);

	return true;
}

bool CDUIGlobal::SaveAttriRichText(tinyxml2::XMLElement *pNode)
{
	if (NULL == pNode) return false;

	for (auto &RichTextItem : m_mapAttriRichTextSave)
	{
		tagDuiRichText &RichText = RichTextItem.second;
		if (RichText.vecRichTextItem.empty()) continue;

		//verify
		uint32_t uID = RichText.GetID();
		if (uID != RichTextItem.first)
		{
			assert(false);

			continue;
		}

		//node
		tinyxml2::XMLElement *pValue = pNode->GetDocument()->NewElement("a");
		if (NULL == pValue)
		{
			assert(false);
			continue;
		}

		//id
		pValue->SetAttribute(Dui_Key_AttriObjValueID, RichTextItem.first);

		//rich item
		{
			tinyxml2::XMLDocument xmlDoc;
			tinyxml2::XMLElement *pNodeVecRichTextItem = xmlDoc.NewElement(Dui_Key_AttriVecRichTextItem);
			for (int n = 0; n < RichText.vecRichTextItem.size(); n++)
			{
				tagDuiRichTextItem &RichTextItem = RichText.vecRichTextItem[n];

				tinyxml2::XMLElement *pNodeRichTextItem = xmlDoc.NewElement(Dui_Key_AttriRichTextItem);
				if (NULL == pNodeRichTextItem) continue;

				//font
				CMMString strValueCombin;
				for (auto &strResName : RichTextItem.vecFontResSwitch)
				{
					strValueCombin += strResName;
					strValueCombin += _T(';');
				}
				pNodeRichTextItem->SetAttribute(Dui_Key_AttriRichTextItemFontRes, (LPCSTR)CT2CA(strValueCombin, CP_UTF8));

				//color
				strValueCombin.clear();
				for (auto &strResName : RichTextItem.vecColorResSwitch)
				{
					strValueCombin += strResName;
					strValueCombin += _T(';');
				}
				pNodeRichTextItem->SetAttribute(Dui_Key_AttriRichTextItemColorRes, (LPCSTR)CT2CA(strValueCombin));

				//text
				pNodeRichTextItem->SetAttribute(Dui_Key_AttriRichTextItemText, (LPCSTR)CT2CA(RichTextItem.strText, CP_UTF8));

				pNodeVecRichTextItem->LinkEndChild(pNodeRichTextItem);
			}

			//push
			tinyxml2::XMLPrinter xmlPrinter;
			pNodeVecRichTextItem->Accept(&xmlPrinter);

			pValue->SetAttribute(Dui_Key_AttriVecRichTextItem, xmlPrinter.CStr());

			xmlDoc.DeleteNode(pNodeVecRichTextItem);
		}

		//textstyle
		pValue->SetAttribute(Dui_Key_AttriRichTextStyle, (UINT)RichText.dwTextStyle);

		//linelimit
		pValue->SetAttribute(Dui_Key_AttriRichTextLineLimit, RichText.nLineLimit);

		pNode->LinkEndChild(pValue);
	}

	return true;
}

LPCTSTR CDUIGlobal::GetAttriText(uint32_t uValueID)
{
	if (uValueID <= 0) return _T("");

	auto FindIt = m_mapAttriTextValue.find(uValueID);
	if (FindIt == m_mapAttriTextValue.end()) return _T("");

	// Do NOT use ternary with CMMString here: GCC may materialize a temporary
	// CMMString and return its dangling c_str() → garbled CJK text on Linux.
	return FindIt->second.c_str();
}

uint32_t CDUIGlobal::SetAttriText(LPCTSTR lpszText)
{
	uint32_t uID = CMMHash::GetHash(lpszText);
	DuiAttriModifyValue(lpszText, m_mapAttriTextValue, uID);

	return uID;
}

bool CDUIGlobal::SetAttriText(tinyxml2::XMLElement *pNode)
{
	if (NULL == pNode) return false;

	uint32_t uValueID = 0;
	LPCSTR lpszText = NULL;

	do
	{
		const tinyxml2::XMLAttribute *pNodeAttribute = pNode->FirstAttribute();
		if (NULL == pNodeAttribute) break;

		do
		{
			if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriObjValueID))
			{
				uValueID = strtoul(pNodeAttribute->Value(), NULL, 10);

				continue;
			}
			if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriText))
			{
				lpszText = pNodeAttribute->Value();

				continue;
			}

		} while (pNodeAttribute = pNodeAttribute->Next(), pNodeAttribute);

	} while (false);

	if (NULL == lpszText || uValueID <= 0) return true;

	switch (m_DuiResVersion)
	{
		case DuiResVersion_0:
		{
			m_mapAttriTextValue[uValueID] = lpszText;

			break;
		}
		default:
		{
			m_mapAttriTextValue[uValueID] = CA2CT(lpszText, CP_UTF8);

			break;
		}
	}

	return true;
}

bool CDUIGlobal::SaveAttriText(tinyxml2::XMLElement *pNode)
{
	if (NULL == pNode) return false;

	for (auto &TextItem : m_mapAttriTextSave)
	{
		CMMString &strText = TextItem.second;
		if (strText.empty()) continue;

		//verify
		uint32_t uID = CMMHash::GetHash(strText);
		if (uID != TextItem.first)
		{
			assert(false);

			continue;
		}

		tinyxml2::XMLElement *pValue = pNode->GetDocument()->NewElement("a");
		if (NULL == pValue)
		{
			assert(false);
			continue;
		}

		//id
		pValue->SetAttribute(Dui_Key_AttriObjValueID, TextItem.first);

		//text
		pValue->SetAttribute(Dui_Key_AttriText, (LPCSTR)CT2CA(strText, CP_UTF8));

		pNode->LinkEndChild(pValue);
	}

	return true;
}

tagDuiTextStyle CDUIGlobal::GetAttriTextStyle(uint32_t uValueID)
{
	if (uValueID <= 0) return {};

	auto FindIt = m_mapAttriTextStyleValue.find(uValueID);

	return FindIt == m_mapAttriTextStyleValue.end() ? tagDuiTextStyle{} : FindIt->second;
}

uint32_t CDUIGlobal::SetAttriTextStyle(const tagDuiTextStyle &TextStyle)
{
	uint32_t uID = TextStyle.GetID();
	DuiAttriModifyValue(TextStyle, m_mapAttriTextStyleValue, uID);

	return uID;
}

bool CDUIGlobal::SetAttriTextStyle(tinyxml2::XMLElement *pNode)
{
	if (NULL == pNode) return false;

	uint32_t uValueID = 0;
	tagDuiTextStyle TextStyle;

	do
	{
		const tinyxml2::XMLAttribute *pNodeAttribute = pNode->FirstAttribute();
		if (NULL == pNodeAttribute) break;

		do
		{
			if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriObjValueID))
			{
				uValueID = strtoul(pNodeAttribute->Value(), NULL, 10);

				continue;
			}
			if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriTextStyleFontRes))
			{
				switch (m_DuiResVersion)
				{
					case DuiResVersion_0:
					{
						TextStyle.vecFontResSwitch = CMMStrHelp::ParseStrFromString(pNodeAttribute->Value(), ";");

						break;
					}
					default:
					{
						TextStyle.vecFontResSwitch = CMMStrHelp::ParseStrFromString(pNodeAttribute->Value(), ";", CP_UTF8);

						break;
					}
				}
				
				continue;
			}
			if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriTextStyleColorRes))
			{
				TextStyle.vecColorResSwitch = CMMStrHelp::ParseStrFromString(pNodeAttribute->Value(), ";", CP_UTF8);

				continue;
			}
			if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriTextStyleStyle))
			{
				TextStyle.dwTextStyle = strtoul(pNodeAttribute->Value(), NULL, 10);

				continue;
			}

		} while (pNodeAttribute = pNodeAttribute->Next(), pNodeAttribute);

	} while (false);

	if (uValueID <= 0) return true;

	m_mapAttriTextStyleValue[uValueID] = TextStyle;

	return true;
}

bool CDUIGlobal::SaveAttriTextStyle(tinyxml2::XMLElement *pNode)
{
	if (NULL == pNode) return false;

	for (auto &TextStyleItem : m_mapAttriTextStyleSave)
	{
		tagDuiTextStyle &TextStyle = TextStyleItem.second;

		//verify
		uint32_t uID = TextStyle.GetID();
		if (uID != TextStyleItem.first)
		{
			assert(false);

			continue;
		}

		//node
		tinyxml2::XMLElement *pValue = pNode->GetDocument()->NewElement("a");
		if (NULL == pValue)
		{
			assert(false);
			continue;
		}

		//id
		pValue->SetAttribute(Dui_Key_AttriObjValueID, TextStyleItem.first);

		//font
		CMMString strValueCombin;
		for (auto &strResName : TextStyle.vecFontResSwitch)
		{
			strValueCombin += strResName;
			strValueCombin += _T(';');
		}
		pValue->SetAttribute(Dui_Key_AttriTextStyleFontRes, (LPCSTR)CT2CA(strValueCombin, CP_UTF8));

		//color
		strValueCombin.clear();
		for (auto &strResName : TextStyle.vecColorResSwitch)
		{
			strValueCombin += strResName;
			strValueCombin += _T(';');
		}
		pValue->SetAttribute(Dui_Key_AttriTextStyleColorRes, (LPCSTR)CT2CA(strValueCombin));

		//textstyle
		pValue->SetAttribute(Dui_Key_AttriTextStyleStyle, (UINT)TextStyle.dwTextStyle);

		pNode->LinkEndChild(pValue);
	}

	return true;
}

vector<CMMString> CDUIGlobal::GetAttriColorResSwitch(uint32_t uValueID)
{
	if (uValueID <= 0) return {};

	auto FindIt = m_mapAttriColorResValue.find(uValueID);

	return FindIt == m_mapAttriColorResValue.end() ? std::vector<CMMString>() : FindIt->second;
}

uint32_t CDUIGlobal::SetAttriColorResSwitch(const vector<CMMString> &vecColorRes)
{
	//id
	uint32_t uID = CMMHash::GetHash(vecColorRes);
	DuiAttriModifyValue(vecColorRes, m_mapAttriColorResValue, uID);

	return uID;
}

bool CDUIGlobal::SetAttriColorResSwitch(tinyxml2::XMLElement *pNode)
{
	if (NULL == pNode) return false;

	uint32_t uValueID = 0;
	std::vector<CMMString> vecColorRes;

	do
	{
		const tinyxml2::XMLAttribute *pNodeAttribute = pNode->FirstAttribute();
		if (NULL == pNodeAttribute) break;

		do
		{
			if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriObjValueID))
			{
				uValueID = strtoul(pNodeAttribute->Value(), NULL, 10);

				continue;
			}
			if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriColorRes))
			{
				vecColorRes = CMMStrHelp::ParseStrFromString(pNodeAttribute->Value(), ";", CP_UTF8);

				continue;
			}

		} while (pNodeAttribute = pNodeAttribute->Next(), pNodeAttribute);

	} while (false);

	if (vecColorRes.empty() || uValueID <= 0) return true;

	m_mapAttriColorResValue[uValueID] = vecColorRes;

	return true;
}

bool CDUIGlobal::SaveAttriColorResSwitch(tinyxml2::XMLElement *pNode)
{
	if (NULL == pNode) return false;

	for (auto &ColorResItem : m_mapAttriColorResSave)
	{
		auto &vecColorRes = ColorResItem.second;
		if (vecColorRes.empty()) continue;

		//verify
		uint32_t uID = CMMHash::GetHash(vecColorRes);
		if (uID != ColorResItem.first)
		{
			assert(false);

			continue;
		}

		//node
		tinyxml2::XMLElement *pValue = pNode->GetDocument()->NewElement("a");
		if (NULL == pValue)
		{
			assert(false);
			continue;
		}

		//id
		pValue->SetAttribute(Dui_Key_AttriObjValueID, ColorResItem.first);

		CMMString strText;
		for (auto &strResName : vecColorRes)
		{
			strText += strResName;
			strText += _T(';');
		}

		pValue->SetAttribute(Dui_Key_AttriColorRes, (LPSTR)CT2CA(strText));

		pNode->LinkEndChild(pValue);
	}

	return true;
}

tagDuiCombox CDUIGlobal::GetAttriCombox(uint32_t uValueID)
{
	if (uValueID <= 0) return {};

	auto FindIt = m_mapAttriComboxValue.find(uValueID);

	return FindIt == m_mapAttriComboxValue.end() ? tagDuiCombox{} : FindIt->second;
}

uint32_t CDUIGlobal::SetAttriCombox(const tagDuiCombox &AttriCombox)
{
	uint32_t uID = ((tagDuiCombox&)AttriCombox).GetID();
	DuiAttriModifyValue(AttriCombox, m_mapAttriComboxValue, uID);

	return uID;
}

bool CDUIGlobal::SetAttriCombox(tinyxml2::XMLElement *pNode)
{
	if (NULL == pNode) return false;

	uint32_t uValueID = 0;
	tagDuiCombox AttriCombox;

	do
	{
		const tinyxml2::XMLAttribute *pNodeAttribute = pNode->FirstAttribute();
		if (NULL == pNodeAttribute) break;

		do
		{
			if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriObjValueID))
			{
				uValueID = strtoul(pNodeAttribute->Value(), NULL, 10);

				continue;
			}
			if (strstr(pNodeAttribute->Name(), Dui_Key_AttriComboxItem))
			{
				int nItem = 0;
				char szDescrib[MAX_PATH] = {};
#if defined(DuiPlatform_SDL) && !defined(WIN32)
				sscanf(pNodeAttribute->Value(), StrComboxItem, &nItem, szDescrib);
#else
				_snscanf_s(pNodeAttribute->Value(), -1, StrComboxItem, &nItem, szDescrib, MAX_PATH);
#endif

				AttriCombox.vecItem.push_back({ nItem, szDescrib });

				continue;
			}

		} while (pNodeAttribute = pNodeAttribute->Next(), pNodeAttribute);

	} while (false);

	if (AttriCombox.vecItem.empty() || uValueID <= 0) return true;

	m_mapAttriComboxValue[uValueID] = AttriCombox;

	return true;
}

bool CDUIGlobal::SaveAttriCombox(tinyxml2::XMLElement *pNode)
{
	if (NULL == pNode) return false;

	for (auto &AttriComboxItem : m_mapAttriComboxSave)
	{
		auto &AttriCombox = AttriComboxItem.second;
		if (AttriCombox.vecItem.empty()) continue;

		//verify
		uint32_t uID = AttriCombox.GetID();
		if (uID != AttriComboxItem.first)
		{
			assert(false);

			continue;
		}

		//node
		tinyxml2::XMLElement *pValue = pNode->GetDocument()->NewElement("a");
		if (NULL == pValue)
		{
			assert(false);
			continue;
		}

		//id
		pValue->SetAttribute(Dui_Key_AttriObjValueID, AttriComboxItem.first);

		//item
		for (auto &Item : AttriCombox.vecItem)
		{
			CMMString strKey;
			strKey.Format(_T("%s%d"), (LPCTSTR)CA2CT(Dui_Key_AttriComboxItem), Item.nItem);

			CMMString strValue;
			strValue.Format((LPCTSTR)CA2CT(StrComboxItem), Item.nItem, Item.strDescribe.GetBuffer());

			pValue->SetAttribute((LPCSTR)CT2CA(strKey), (LPCSTR)CT2CA(strValue));
		}

		pNode->LinkEndChild(pValue);
	}

	return true;
}

tagDuiPosition CDUIGlobal::GetAttriPosition(uint32_t uValueID)
{
	if (uValueID <= 0) return {};

	auto FindIt = m_mapAttriPositionValue.find(uValueID);

	return FindIt == m_mapAttriPositionValue.end() ? tagDuiPosition{} : FindIt->second;
}

uint32_t CDUIGlobal::SetAttriPosition(const tagDuiPosition &Position)
{
	uint32_t uID = Position.GetID();
	DuiAttriModifyValue(Position, m_mapAttriPositionValue, uID);

	return uID;
}

bool CDUIGlobal::SetAttriPosition(tinyxml2::XMLElement *pNode)
{
	if (NULL == pNode) return false;

	uint32_t uValueID = 0;
	tagDuiPosition Position;

	do
	{
		const tinyxml2::XMLAttribute *pNodeAttribute = pNode->FirstAttribute();
		if (NULL == pNodeAttribute) break;

		do
		{
			if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriObjValueID))
			{
				uValueID = strtoul(pNodeAttribute->Value(), NULL, 10);

				continue;
			}
			if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriPosition))
			{
#if defined(DuiPlatform_SDL) && !defined(WIN32)
				sscanf(pNodeAttribute->Value(), StrPosition, &Position.bFloat,
					&Position.HorizPosition.HorizAlignType, &Position.HorizPosition.nLeftAlignValue, &Position.HorizPosition.nRightAlignValue, &Position.HorizPosition.nFixedWidth,
					&Position.VertPosition.VertAlignType, &Position.VertPosition.nTopAlignValue, &Position.VertPosition.nBottomAlignValue, &Position.VertPosition.nFixedHeight);
#else
				_snscanf_s(pNodeAttribute->Value(), -1, StrPosition, &Position.bFloat,
					&Position.HorizPosition.HorizAlignType, &Position.HorizPosition.nLeftAlignValue, &Position.HorizPosition.nRightAlignValue, &Position.HorizPosition.nFixedWidth,
					&Position.VertPosition.VertAlignType, &Position.VertPosition.nTopAlignValue, &Position.VertPosition.nBottomAlignValue, &Position.VertPosition.nFixedHeight);
#endif

				continue;
			}

		} while (pNodeAttribute = pNodeAttribute->Next(), pNodeAttribute);

	} while (false);

	if (uValueID <= 0) return true;

	m_mapAttriPositionValue[uValueID] = Position;

	return true;
}

bool CDUIGlobal::SaveAttriPosition(tinyxml2::XMLElement *pNode)
{
	if (NULL == pNode) return false;

	for (auto &PositionItem : m_mapAttriPositionSave)
	{
		auto &Position = PositionItem.second;

		//verify
		uint32_t uID = Position.GetID();
		if (uID != PositionItem.first)
		{
			assert(false);

			continue;
		}

		//node
		tinyxml2::XMLElement *pValue = pNode->GetDocument()->NewElement("a");
		if (NULL == pValue)
		{
			assert(false);
			continue;
		}

		//id
		pValue->SetAttribute(Dui_Key_AttriObjValueID, PositionItem.first);

		//pos
		pValue->SetAttribute(Dui_Key_AttriPosition, 
			CMMStrHelp::FormatA(StrPosition,
				(int)Position.bFloat,
				Position.HorizPosition.HorizAlignType, 
				Position.HorizPosition.nLeftAlignValue, 
				Position.HorizPosition.nRightAlignValue, 
				Position.HorizPosition.nFixedWidth,
				Position.VertPosition.VertAlignType, 
				Position.VertPosition.nTopAlignValue, 
				Position.VertPosition.nBottomAlignValue, 
				Position.VertPosition.nFixedHeight).c_str());

		pNode->LinkEndChild(pValue);
	}

	return true;
}

CDUIRect CDUIGlobal::GetAttriRect(uint32_t uValueID)
{
	if (uValueID <= 0) return {};

	auto FindIt = m_mapAttriRectValue.find(uValueID);

	return FindIt == m_mapAttriRectValue.end() ? RECT{} : FindIt->second;
}

uint32_t CDUIGlobal::SetAttriRect(const CDUIRect &rect)
{
	uint32_t uID = rect.GetID();
	DuiAttriModifyValue(rect, m_mapAttriRectValue, uID);

	return uID;
}

bool CDUIGlobal::SetAttriRect(tinyxml2::XMLElement *pNode)
{
	if (NULL == pNode) return false;

	uint32_t uValueID = 0;
	CDUIRect rcItem;

	do
	{
		const tinyxml2::XMLAttribute *pNodeAttribute = pNode->FirstAttribute();
		if (NULL == pNodeAttribute) break;

		do
		{
			if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriObjValueID))
			{
				uValueID = strtoul(pNodeAttribute->Value(), NULL, 10);

				continue;
			}
			if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriRectValue))
			{
				LPSTR pStr = NULL;
				rcItem.left = strtol(pNodeAttribute->Value(), &pStr, 10); ASSERT(pStr);
				rcItem.top = strtol(pStr + 1, &pStr, 10); ASSERT(pStr);
				rcItem.right = strtol(pStr + 1, &pStr, 10); ASSERT(pStr);
				rcItem.bottom = strtol(pStr + 1, &pStr, 10); ASSERT(pStr);

				continue;
			}

		} while (pNodeAttribute = pNodeAttribute->Next(), pNodeAttribute);

	} while (false);

	if (uValueID <= 0) return true;

	m_mapAttriRectValue[uValueID] = rcItem;

	return true;
}

bool CDUIGlobal::SaveAttriRect(tinyxml2::XMLElement *pNode)
{
	if (NULL == pNode) return false;

	for (auto &RectItem : m_mapAttriRectSave)
	{
		auto &rcItem = RectItem.second;

		//verify
		uint32_t uID = rcItem.GetID();
		if (uID != RectItem.first)
		{
			assert(false);

			continue;
		}

		//node
		tinyxml2::XMLElement *pValue = pNode->GetDocument()->NewElement("a");
		if (NULL == pValue)
		{
			assert(false);
			continue;
		}

		//id
		pValue->SetAttribute(Dui_Key_AttriObjValueID, RectItem.first);

		//rect
		pValue->SetAttribute(Dui_Key_AttriRectValue, CMMStrHelp::FormatA("%d,%d,%d,%d", rcItem.left, rcItem.top, rcItem.right, rcItem.bottom).c_str());

		pNode->LinkEndChild(pValue);
	}

	return true;
}

tagDuiImageSection CDUIGlobal::GetAttriImageSection(uint32_t uValueID)
{
	if (uValueID <= 0) return {};

	auto FindIt = m_mapAttriImageSectionValue.find(uValueID);

	return FindIt == m_mapAttriImageSectionValue.end() ? tagDuiImageSection{} : FindIt->second;
}

uint32_t CDUIGlobal::SetAttriImageSection(const tagDuiImageSection &ImageSection)
{
	uint32_t uID = ImageSection.GetID();
	DuiAttriModifyValue(ImageSection, m_mapAttriImageSectionValue, uID);

	return uID;
}

bool CDUIGlobal::SetAttriImageSection(tinyxml2::XMLElement *pNode)
{
	if (NULL == pNode) return false;

	uint32_t uValueID = 0;
	tagDuiImageSection ImageSection;

	do
	{
		const tinyxml2::XMLAttribute *pNodeAttribute = pNode->FirstAttribute();
		if (NULL == pNodeAttribute) break;

		do
		{
			if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriObjValueID))
			{
				uValueID = strtoul(pNodeAttribute->Value(), NULL, 10);

				continue;
			}
			if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriImageSecImageRes))
			{
				switch (m_DuiResVersion)
				{
					case DuiResVersion_0:
					{
						ImageSection.vecImageResSwitch = CMMStrHelp::ParseStrFromString(pNodeAttribute->Value(), ";");

						break;
					}
					default:
					{
						ImageSection.vecImageResSwitch = CMMStrHelp::ParseStrFromString(pNodeAttribute->Value(), ";", CP_UTF8);

						break;
					}
				}
				
				continue;
			}
			if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriImageSecHorizAlign))
			{
				ImageSection.HorizImageAlign = (enDuiHorizImageAlignType)strtol(pNodeAttribute->Value(), NULL, 10);

				continue;
			}
			if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriImageSecVertAlign))
			{
				ImageSection.VertImageAlign = (enDuiVertImageAlignType)strtol(pNodeAttribute->Value(), NULL, 10);

				continue;
			}
			if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriImageSecHorizOffset))
			{
				ImageSection.nHorizOffset = (short)strtol(pNodeAttribute->Value(), NULL, 10);

				continue;
			}
			if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriImageSecVertOffset))
			{
				ImageSection.nVertOffset = (short)strtol(pNodeAttribute->Value(), NULL, 10);

				continue;
			}
			if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriImageSecSourceType))
			{
				ImageSection.ImageSourceType = (enDuiImageSourceType)strtol(pNodeAttribute->Value(), NULL, 10);

				continue;
			}
			if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriImageSecSourcePartType))
			{
				ImageSection.bPartVert = (bool)strtol(pNodeAttribute->Value(), NULL, 10);

				continue;
			}
			if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriImageSecSourcePartAll))
			{
				ImageSection.cbPartAll = (BYTE)strtol(pNodeAttribute->Value(), NULL, 10);

				continue;
			}
			if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriImageSecSourcePartSel))
			{
				ImageSection.cbPartSel = (BYTE)strtol(pNodeAttribute->Value(), NULL, 10);

				continue;
			}
			if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriImageSecSourceCustom))
			{
				LPSTR pStr = NULL;
				CDUIRect rcTemp;
				rcTemp.left = strtol(pNodeAttribute->Value(), &pStr, 10); ASSERT(pStr);
				rcTemp.top = strtol(pStr + 1, &pStr, 10); ASSERT(pStr);
				rcTemp.right = strtol(pStr + 1, &pStr, 10); ASSERT(pStr);
				rcTemp.bottom = strtol(pStr + 1, &pStr, 10); ASSERT(pStr);

				ImageSection.mapSourceCustomScale[100] = rcTemp;

				continue;
			}
			if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriImageSecAlpha))
			{
				ImageSection.cbAlpha = (BYTE)strtol(pNodeAttribute->Value(), NULL, 10);

				continue;
			}
			if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriImageSecMask))
			{
				ImageSection.dwMask = (Gdiplus::ARGB)strtoul(pNodeAttribute->Value(), NULL, 10);

				continue;
			}
			if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriImageSecCorner))
			{
				LPSTR pStr = NULL;
				CDUIRect rcTemp;
				rcTemp.left = strtol(pNodeAttribute->Value(), &pStr, 10); ASSERT(pStr);
				rcTemp.top = strtol(pStr + 1, &pStr, 10); ASSERT(pStr);
				rcTemp.right = strtol(pStr + 1, &pStr, 10); ASSERT(pStr);
				rcTemp.bottom = strtol(pStr + 1, &pStr, 10); ASSERT(pStr);

				ImageSection.rcCorner = rcTemp;

				continue;
			}
			if (0 == strcmp(pNodeAttribute->Name(), Dui_Key_AttriImageSecCornerHole))
			{
				ImageSection.bCornerHole = strtol(pNodeAttribute->Value(), NULL, 10);

				continue;
			}
			
		} while (pNodeAttribute = pNodeAttribute->Next(), pNodeAttribute);

	} while (false);

	if (ImageSection.vecImageResSwitch.empty() || uValueID <= 0) return true;

	m_mapAttriImageSectionValue[uValueID] = ImageSection;

	return true;
}

bool CDUIGlobal::SaveAttriImageSection(tinyxml2::XMLElement *pNode)
{
	if (NULL == pNode) return false;

	for (auto &ImageSectionItem : m_mapAttriImageSectionSave)
	{
		auto &ImageSection = ImageSectionItem.second;
		if (ImageSection.vecImageResSwitch.empty()) continue;

		//verify
		uint32_t uID = ImageSection.GetID();
		if (uID != ImageSectionItem.first)
		{
			assert(false);

			continue;
		}

		//node
		tinyxml2::XMLElement *pValue = pNode->GetDocument()->NewElement("a");
		if (NULL == pValue)
		{
			assert(false);
			continue;
		}

		//id
		pValue->SetAttribute(Dui_Key_AttriObjValueID, ImageSectionItem.first);

		//imageres
		CMMString strValueCombin;
		for (auto &strResName : ImageSection.vecImageResSwitch)
		{
			strValueCombin += strResName;
			strValueCombin += _T(';');
		}
		pValue->SetAttribute(Dui_Key_AttriImageSecImageRes, (LPCSTR)CT2CA(strValueCombin, CP_UTF8));

		//Dest
		pValue->SetAttribute(Dui_Key_AttriImageSecHorizAlign, CMMStrHelp::FormatA("%d", ImageSection.HorizImageAlign).c_str());
		pValue->SetAttribute(Dui_Key_AttriImageSecVertAlign, CMMStrHelp::FormatA("%d", ImageSection.VertImageAlign).c_str());
		pValue->SetAttribute(Dui_Key_AttriImageSecHorizOffset, CMMStrHelp::FormatA("%d", (int)ImageSection.nHorizOffset).c_str());
		pValue->SetAttribute(Dui_Key_AttriImageSecVertOffset, CMMStrHelp::FormatA("%d", (int)ImageSection.nVertOffset).c_str());

		//Source
		pValue->SetAttribute(Dui_Key_AttriImageSecSourceType, CMMStrHelp::FormatA("%d", (int)ImageSection.ImageSourceType).c_str());
		pValue->SetAttribute(Dui_Key_AttriImageSecSourcePartType, CMMStrHelp::FormatA("%d", (int)ImageSection.bPartVert).c_str());
		pValue->SetAttribute(Dui_Key_AttriImageSecSourcePartAll, CMMStrHelp::FormatA("%d", (int)ImageSection.cbPartAll).c_str());
		pValue->SetAttribute(Dui_Key_AttriImageSecSourcePartSel, CMMStrHelp::FormatA("%d", (int)ImageSection.cbPartSel).c_str());

		if (ImageSection.mapSourceCustomScale.find(100) != ImageSection.mapSourceCustomScale.end())
		{
			CDUIRect rcSource = ImageSection.mapSourceCustomScale[100];
			pValue->SetAttribute(Dui_Key_AttriImageSecSourceCustom, CMMStrHelp::FormatA("%d,%d,%d,%d", rcSource.left, rcSource.top, rcSource.right, rcSource.bottom).c_str());
		}

		//Alpha
		pValue->SetAttribute(Dui_Key_AttriImageSecAlpha, ImageSection.cbAlpha);

		//Mask
		pValue->SetAttribute(Dui_Key_AttriImageSecMask, (UINT)ImageSection.dwMask);

		//Corner
		pValue->SetAttribute(Dui_Key_AttriImageSecCorner, CMMStrHelp::FormatA("%d,%d,%d,%d", ImageSection.rcCorner.left, ImageSection.rcCorner.top, ImageSection.rcCorner.right, ImageSection.rcCorner.bottom).c_str());

		//Corner Hole
		pValue->SetAttribute(Dui_Key_AttriImageSecCornerHole, (int)ImageSection.bCornerHole);

		pNode->LinkEndChild(pValue);
	}

	return true;
}

void CDUIGlobal::OnAttriValueIDRead(enDuiAttributeType AttriType, OUT uint32_t &uID)
{
	//switch value
	switch (AttriType)
	{
		case DuiAttribute_ImageSection:
		{
			DuiAttriSwitchValue(uID, m_mapAttriImageSectionSwitch);

			break;
		}
	}

	//save value
	if (false == m_bAttriWaitSave) return;

	switch (AttriType)
	{
		case DuiAttribute_CtrlID:
		{
			DuiAttriReadValue(uID, m_mapControlIDSave, m_mapControlIDValue);

			break;
		}
		case DuiAttribute_RichText:
		{
			DuiAttriReadValue(uID, m_mapAttriRichTextSave, m_mapAttriRichTextValue);

			break;
		}
		case DuiAttribute_Text:
		{
			DuiAttriReadValue(uID, m_mapAttriTextSave, m_mapAttriTextValue);

			break;
		}
		case DuiAttribute_TextStyle:
		{
			DuiAttriReadValue(uID, m_mapAttriTextStyleSave, m_mapAttriTextStyleValue);

			break;
		}
		case DuiAttribute_ColorSwitch:
		{
			DuiAttriReadValue(uID, m_mapAttriColorResSave, m_mapAttriColorResValue);

			break;
		}
		case DuiAttribute_Combox:
		case DuiAttribute_DuiSelect:
		case DuiAttribute_TabSelect:
		{
			DuiAttriReadValue(uID, m_mapAttriComboxSave, m_mapAttriComboxValue);

			break;
		}
		case DuiAttribute_Position:
		{
			DuiAttriReadValue(uID, m_mapAttriPositionSave, m_mapAttriPositionValue);

			break;
		}
		case DuiAttribute_Rect:
		{
			DuiAttriReadValue(uID, m_mapAttriRectSave, m_mapAttriRectValue);

			break;
		}
		case DuiAttribute_ImageSection:
		{
			DuiAttriReadValue(uID, m_mapAttriImageSectionSave, m_mapAttriImageSectionValue);

			break;
		}
	}

	return;
}

bool CDUIGlobal::SaveAttriValue(tinyxml2::XMLDocument &xmlDoc)
{
	if (m_bAttriWaitSave) return true;

	//release pre
	m_bAttriWaitSave = true;

	for (auto ModelStoreItem : m_mapModelStore)
	{
		MMSafeDelete(ModelStoreItem.second);
	}

	m_mapModelStore.clear();

	//load dui
	for (auto Dui : m_vecDui)
	{
		CDUIWnd Dlg;
		CDUIControlBase *pControl = LoadDui(Dui.strName, &Dlg);

		MMSafeDelete(pControl);
	}

	//load dialog
	CMMString strFileModel = CMMString(Dui_FolderUIModel) + _T("\\") + Dui_FileModelDlgXml;
	strFileModel = CMMService::GetWorkDirectory() + _T("\\") + strFileModel;
	CDUIContainerCtrl *pRootCtrl = MMDynamicPtr(CDUIContainerCtrl, CDUIXmlPack::LoadDui(strFileModel, NULL));
	MMSafeDelete(pRootCtrl);

	//load view
	strFileModel = CMMString(Dui_FolderUIModel) + _T("\\") + Dui_FileModelViewXml;
	strFileModel = CMMService::GetWorkDirectory() + _T("\\") + strFileModel;
	pRootCtrl = MMDynamicPtr(CDUIContainerCtrl, CDUIXmlPack::LoadDui(strFileModel, NULL));
	MMSafeDelete(pRootCtrl);

	m_bAttriWaitSave = false;

	//ctrl id
	CDUIXmlPack::SaveCtrlID(m_mapControlIDSave);
	m_mapControlIDValue = m_mapControlIDSave;

	//attribute
	tinyxml2::XMLElement *pNode = xmlDoc.NewElement((LPCSTR)CT2CA(CDUIAttributeObject().GetClass()));
	SaveAttriName(pNode);
	xmlDoc.LinkEndChild(pNode);

	pNode = xmlDoc.NewElement((LPCSTR)CT2CA(CDUIAttributeRichText().GetClass()));
	SaveAttriRichText(pNode);
	xmlDoc.LinkEndChild(pNode);

	pNode = xmlDoc.NewElement((LPCSTR)CT2CA(CDUIAttributeText().GetClass()));
	SaveAttriText(pNode);
	xmlDoc.LinkEndChild(pNode);

	pNode = xmlDoc.NewElement((LPCSTR)CT2CA(CDUIAttributeTextStyle().GetClass()));
	SaveAttriTextStyle(pNode);
	xmlDoc.LinkEndChild(pNode);

	pNode = xmlDoc.NewElement((LPCSTR)CT2CA(CDUIAttributeColorSwitch().GetClass()));
	SaveAttriColorResSwitch(pNode);
	xmlDoc.LinkEndChild(pNode);

	pNode = xmlDoc.NewElement((LPCSTR)CT2CA(CDUIAttributeCombox().GetClass()));
	SaveAttriCombox(pNode);
	xmlDoc.LinkEndChild(pNode);

	pNode = xmlDoc.NewElement((LPCSTR)CT2CA(CDUIAttributePosition().GetClass()));
	SaveAttriPosition(pNode);
	xmlDoc.LinkEndChild(pNode);

	pNode = xmlDoc.NewElement((LPCSTR)CT2CA(CDUIAttributeRect().GetClass()));
	SaveAttriRect(pNode);
	xmlDoc.LinkEndChild(pNode);

	pNode = xmlDoc.NewElement((LPCSTR)CT2CA(CDUIAttriImageSection().GetClass()));
	SaveAttriImageSection(pNode);
	xmlDoc.LinkEndChild(pNode);

	m_mapAttriRichTextSave.clear();
	m_mapAttriTextSave.clear();
	m_mapAttriTextStyleSave.clear();
	m_mapAttriColorResSave.clear();
	m_mapAttriComboxSave.clear();
	m_mapAttriPositionSave.clear();
	m_mapAttriRectSave.clear();
	m_mapAttriImageSectionSave.clear();
	m_mapControlIDSave.clear();

	return true;
}

void CDUIGlobal::AddWnd(CDUIWnd *pWnd)
{
	if (NULL == pWnd) return;

	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	auto FindIt = m_mapWnd.find(pWnd);
	if (FindIt != m_mapWnd.end()) return;

	m_mapWnd[pWnd] = { DuiType_Dlg, _T("") };

	return;
}

void CDUIGlobal::RenameWnd(const CMMString &strNameOld, const CMMString &strNameNew)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	auto FindIt = find_if(m_mapWnd.begin(), m_mapWnd.end(), [&](const std::pair<CDUIWnd*, tagDuiFile> &Wnd)
	{
		return Wnd.second.strName == strNameOld;
	});
	if (FindIt != m_mapWnd.end())
	{
		FindIt->second.strName = strNameNew;
	}

	return;
}

void CDUIGlobal::RenameWnd(CDUIWnd *pWnd, const CMMString &strNameNew)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	auto FindIt = m_mapWnd.find(pWnd);
	if (FindIt == m_mapWnd.end()) return;

	FindIt->second.strName = strNameNew;

	return;
}

void CDUIGlobal::SetWndDuiType(CDUIWnd *pWnd, enDuiType DuiType)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	auto FindIt = m_mapWnd.find(pWnd);
	if (FindIt == m_mapWnd.end()) return;

	FindIt->second.DuiType = DuiType;

	return;
}

void CDUIGlobal::RemoveWnd(CDUIWnd *pWnd)
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	auto FindIt = m_mapWnd.find(pWnd);
	if (FindIt == m_mapWnd.end()) return;

	m_mapWnd.erase(FindIt);

	return;
}

void CDUIGlobal::ReleaseResource()
{
	ReleaseFontResource();
	ReleaseImageResource();
	ReleaseColorResource();

#ifndef DUI_DESIGN
	m_mapAttriNameValue.clear();
	m_mapAttriRichTextValue.clear();
	m_mapAttriTextValue.clear();
	m_mapAttriTextStyleValue.clear();
	m_mapAttriColorResValue.clear();
	m_mapAttriComboxValue.clear();
	m_mapAttriPositionValue.clear();
	m_mapAttriRectValue.clear();
	m_mapAttriImageSectionValue.clear();
#endif

	m_uMaxControlID = Dui_CtrlIDInner_Finish;
	MapDuiControlID mapControlID = m_mapControlIDValue;
	for (auto ControlIDItem : mapControlID)
	{
		if (ControlIDItem.first < Dui_CtrlIDInner_Finish) continue;

		m_mapControlIDValue.erase(ControlIDItem.first);
	}

	return;
}

void CDUIGlobal::ReleaseFontResource()
{
	for (auto ResourceIt = m_mapResourceFont.begin(); ResourceIt != m_mapResourceFont.end();)
	{
		if (ResourceIt->second
			&& ResourceIt->second->IsDesign()
			&& m_bInited)
		{
			++ResourceIt;

			continue;
		}

		OnResourceRemove(ResourceIt->second);

		MMSafeDelete(ResourceIt->second);

		ResourceIt = m_mapResourceFont.erase(ResourceIt);
	}

	return;
}

void CDUIGlobal::ReleaseImageResource()
{
	for (auto ResourceIt = m_mapResourceImage.begin(); ResourceIt != m_mapResourceImage.end();)
	{
		if (ResourceIt->second
			&& ResourceIt->second->IsDesign()
			&& m_bInited)
		{
			++ResourceIt;

			continue;
		}

		OnResourceRemove(ResourceIt->second);

		MMSafeDelete(ResourceIt->second);

		ResourceIt = m_mapResourceImage.erase(ResourceIt);
	}

	return;
}

void CDUIGlobal::ReleaseColorResource()
{
	for (auto ResourceIt = m_mapResourceColor.begin(); ResourceIt != m_mapResourceColor.end();)
	{
		if (ResourceIt->second
			&& ResourceIt->second->IsDesign()
			&& m_bInited)
		{
			++ResourceIt;

			continue;
		}

		OnResourceRemove(ResourceIt->second);

		MMSafeDelete(ResourceIt->second);

		ResourceIt = m_mapResourceColor.erase(ResourceIt);
	}

	return;
}

void CDUIGlobal::ReleaseDui()
{
	std::lock_guard<std::recursive_mutex> Lock(m_DataLock);

	m_vecDui.clear();
	m_mapWnd.clear();

	for (auto ModelStoreItem : m_mapModelStore)
	{
		MMSafeDelete(ModelStoreItem.second);
	}
	m_mapModelStore.clear();

	return;
}

void CDUIGlobal::MessageLoop()
{
#if defined(DuiPlatform_SDL)
	SDL_Event e = {};
	while (true)
	{
		if (false == SDL_WaitEvent(&e))
		{
			assert(false);
			MMTRACE(_T("EXCEPTION: SDL_WaitEvent failed\n"));
			continue;
		}

		MSG Msg = {};
		if (false == MMSdlEventToMsg(e, Msg) || false == CDUIGlobal::GetInstance()->TranslateMessage(&Msg))
		{
			MMSdlDispatchEvent(e);
		}

		if (SDL_EVENT_QUIT == e.type)
		{
			break;
		}
	}
#else
	BOOL bRet = 0;
	MSG Msg = {};
	while ((bRet = GetMessage(&Msg, NULL, 0, 0)) != 0)
	{
		// handle the error and possibly exit
		if (-1 == bRet)
		{
			assert(false);
			MMTRACE(_T("EXCEPTION: %s(%d)\n"), __FILET__, __LINE__);

			continue;
		}

		if (false == CDUIGlobal::GetInstance()->TranslateMessage(&Msg))
		{
			try
			{
				::TranslateMessage(&Msg);
				::DispatchMessage(&Msg);
			}
			catch (...)
			{
				assert(false);
				MMTRACE(_T("EXCEPTION: %s(%d)\n"), __FILET__, __LINE__);
#ifdef _DEBUG
				throw "CDUIWnd::MessageLoop";
#endif
			}
		}
	}
#endif

	return;
}

void CDUIGlobal::RegisterWndNotify(IDuiWndNotify *pIDuiWndNotify)
{
	g_pIDuiWndNotify = pIDuiWndNotify;

	return;
}

void CDUIGlobal::UnRegisterWndNotify(IDuiWndNotify *pIDuiWndNotify)
{
	if (g_pIDuiWndNotify != pIDuiWndNotify) return;

	g_pIDuiWndNotify = NULL;

	return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
CMMString DUI__TraceMsg(UINT uMsg)
{
#define MSGDEF(x) if(uMsg==x) return _T(#x)
	MSGDEF(WM_SETCURSOR);
	MSGDEF(WM_NCHITTEST);
	MSGDEF(WM_NCPAINT);
	MSGDEF(WM_PAINT);
	MSGDEF(WM_ERASEBKGND);
	MSGDEF(WM_NCMOUSEMOVE);
	MSGDEF(WM_MOUSEMOVE);
	MSGDEF(WM_MOUSELEAVE);
	MSGDEF(WM_MOUSEHOVER);
	MSGDEF(WM_NOTIFY);
	MSGDEF(WM_COMMAND);
	MSGDEF(WM_MEASUREITEM);
	MSGDEF(WM_DRAWITEM);
	MSGDEF(WM_LBUTTONDOWN);
	MSGDEF(WM_LBUTTONUP);
	MSGDEF(WM_LBUTTONDBLCLK);
	MSGDEF(WM_RBUTTONDOWN);
	MSGDEF(WM_RBUTTONUP);
	MSGDEF(WM_RBUTTONDBLCLK);
	MSGDEF(WM_SETFOCUS);
	MSGDEF(WM_KILLFOCUS);
	MSGDEF(WM_MOVE);
	MSGDEF(WM_SIZE);
	MSGDEF(WM_SIZING);
	MSGDEF(WM_MOVING);
	MSGDEF(WM_GETMINMAXINFO);
	MSGDEF(WM_CAPTURECHANGED);
	MSGDEF(WM_WINDOWPOSCHANGED);
	MSGDEF(WM_WINDOWPOSCHANGING);
	MSGDEF(WM_NCCALCSIZE);
	MSGDEF(WM_NCCREATE);
	MSGDEF(WM_NCDESTROY);
	MSGDEF(WM_TIMER);
	MSGDEF(WM_KEYDOWN);
	MSGDEF(WM_KEYUP);
	MSGDEF(WM_CHAR);
	MSGDEF(WM_SYSKEYDOWN);
	MSGDEF(WM_SYSKEYUP);
	MSGDEF(WM_SYSCOMMAND);
	MSGDEF(WM_SYSCHAR);
	MSGDEF(WM_VSCROLL);
	MSGDEF(WM_HSCROLL);
	MSGDEF(WM_CHAR);
	MSGDEF(WM_SHOWWINDOW);
	MSGDEF(WM_PARENTNOTIFY);
	MSGDEF(WM_CREATE);
	MSGDEF(WM_NCACTIVATE);
	MSGDEF(WM_ACTIVATE);
	MSGDEF(WM_ACTIVATEAPP);
	MSGDEF(WM_CLOSE);
	MSGDEF(WM_DESTROY);
	MSGDEF(WM_GETICON);
	MSGDEF(WM_GETTEXT);
	MSGDEF(WM_GETTEXTLENGTH);
	
	CMMString strMsg;
	strMsg.Format(_T("0x%04X"), uMsg);
	return strMsg;
}