#include "StdAfx.h"
#include "MMService.h"

#if !defined(DuiPlatform_SDL)

#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "Ws2_32.lib")

//////////////////////////////////////////////////////////////////////////////////
#define FILE_DEVICE_SCSI				0x0000001b
#define IOCTL_SCSI_MINIPORT_IDENTIFY	( ( FILE_DEVICE_SCSI << 16 ) + 0x0501 )

#define IOCTL_SCSI_MINIPORT				0x0004D008  //  see NTDDSCSI.H for definition

#define IDENTIFY_BUFFER_SIZE			512
#define SENDIDLENGTH					( sizeof( SENDCMDOUTPARAMS ) + IDENTIFY_BUFFER_SIZE )

#define IDE_ATAPI_IDENTIFY				0xA1  //  Returns ID sector for ATAPI.
#define IDE_ATA_IDENTIFY				0xEC  //  Returns ID sector for ATA.
#define DFP_RECEIVE_DRIVE_DATA			0x0007c088

struct tagAstatInfo
{
	ADAPTER_STATUS						AdapterStatus;
	NAME_BUFFER							NameBuff[16];
};

//////////////////////////////////////////////////////////////////////////////////
bool CMMService::SetClipboardString(HWND hWnd, LPCTSTR pszString)
{
	HANDLE hData = NULL;
	BOOL bOpenClopboard = FALSE;

	__try
	{
		bOpenClopboard = OpenClipboard(hWnd);
		if (bOpenClopboard == FALSE) __leave;

		if (EmptyClipboard() == FALSE) __leave;

		hData = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, CountStringBuffer(pszString));
		if (hData == NULL) __leave;

		lstrcpy((LPTSTR)GlobalLock(hData), pszString);
		GlobalUnlock(hData);

#ifndef _UNICODE
		::SetClipboardData(CF_TEXT, hData);
#else
		::SetClipboardData(CF_UNICODETEXT, hData);
#endif
	}

	__finally
	{
		if (bOpenClopboard == TRUE) CloseClipboard();

		if (AbnormalTermination() == TRUE)
		{
			assert(FALSE);
		}
	}

	return true;
}

bool CMMService::SetClipboardFiles(HWND hWndOwner, std::vector<CMMString> vecFiles, bool bCopy /*= true*/)
{
	if (vecFiles.empty()) return false;

	UINT uDropEffect = RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT);

	//effect
	HGLOBAL hGblEffect = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE, sizeof(DWORD));
	DWORD *pDropEffect = (DWORD*)GlobalLock(hGblEffect);
	*pDropEffect = bCopy ? DROPEFFECT_COPY : DROPEFFECT_MOVE;
	GlobalUnlock(hGblEffect);

	//len
	UINT uFileListLen = 0;
	for (auto &strFile : vecFiles)
	{
		if (strFile.empty()) continue;

		uFileListLen += (strFile.length() + 1) * sizeof(strFile[0]);
	}

	UINT uGblLen = sizeof(DROPFILES) + (uFileListLen + sizeof(TCHAR));
	HGLOBAL hGblFiles = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE, uGblLen);
	BYTE *pData = (BYTE*)GlobalLock(hGblFiles);

	//drop
	DROPFILES DropFiles = {};
	DropFiles.pFiles = sizeof(DROPFILES);
	DropFiles.pt.x = 0;
	DropFiles.pt.y = 0;
	DropFiles.fNC = FALSE;
	DropFiles.fWide = TRUE;
	memcpy(pData, (LPVOID)(&DropFiles), sizeof(DROPFILES));

	//files
	BYTE *pFileList = pData + sizeof(DROPFILES);
	for (auto &strFile : vecFiles)
	{
		if (strFile.empty()) continue;

		int nSize = (strFile.length() + 1) * sizeof(strFile[0]);
		memcpy(pFileList, (BYTE*)strFile.c_str(), nSize);
		pFileList += nSize;
	}

	GlobalUnlock(hGblFiles);

	//set
	if (OpenClipboard(hWndOwner))
	{
		EmptyClipboard();

		SetClipboardData(CF_HDROP, hGblFiles);
		SetClipboardData(uDropEffect, hGblEffect);

		CloseClipboard();

		return true;
	}

	::GlobalFree(hGblEffect);
	::GlobalFree(hGblFiles);

	return false;
}

CMMString CMMService::GetClipboardString()
{
	CMMString strText;

	do
	{
		if (false == OpenClipboard(NULL)) break;

		bool bUnicode = IsClipboardFormatAvailable(CF_UNICODETEXT);
		HANDLE hData = bUnicode ? GetClipboardData(CF_UNICODETEXT) : GetClipboardData(CF_TEXT);
		if (NULL == hData)
		{
			CloseClipboard();

			break;
		}
		if (bUnicode)
		{
			strText = (LPCTSTR)GlobalLock(hData);
		}
		else
		{
			strText = (LPCSTR)GlobalLock(hData);
		}

		GlobalUnlock(hData);
		CloseClipboard();

	} while (false);

	return strText;
}

std::vector<CMMString> CMMService::GetClipboardFile()
{
	std::vector<CMMString> vecFile;

	do
	{
		bool bDrop = IsClipboardFormatAvailable(CF_HDROP);
		if (false == bDrop) break;
		if (false == OpenClipboard(NULL)) break;

		HDROP hDropInfo = (HDROP)GetClipboardData(CF_HDROP);
		if (NULL == hDropInfo)
		{
			CloseClipboard();

			break;
		}
		
		UINT nFilesCount = DragQueryFile(hDropInfo, INFINITE, NULL, 0);
		for (UINT i = 0; i < nFilesCount; i++)
		{
			TCHAR szFile[MAX_PATH + 1] = {};
			DragQueryFile(hDropInfo, i, szFile, MMCountString(szFile));
			if (false == PathFileExists(szFile)) continue;

			vecFile.push_back(szFile);
		}

		CloseClipboard();

	} while (false);

	return vecFile;
}

DROPEFFECT CMMService::GetClipboardDropEffect()
{
	DROPEFFECT DropEffect = 0;

	do
	{
		if (false == OpenClipboard(NULL)) break;

		auto ClipFormat = RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT);
		HANDLE hData = GetClipboardData(ClipFormat);
		if (NULL == hData)
		{
			CloseClipboard();

			break;
		}

		DropEffect = *(DROPEFFECT*)GlobalLock(hData);

		GlobalUnlock(hData);
		CloseClipboard();

	} while (false);

	return DropEffect;
}

int CMMService::GetCompileDate()
{
	char sMonth[5] = "";
	int nMonth, nDay, nYear;
	char sMonthName[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
	char sTime[20] = __DATE__;
	sscanf(sTime, "%s %d %d", sMonth, &nDay, &nYear);

	nMonth = (strstr(sMonthName, sMonth) - sMonthName) / 3 + 1;

	return  nYear * 10000 + nMonth * 100 + nDay;
}

CMMString CMMService::IPAddrToString(DWORD dwIPAddr)
{
	struct in_addr in;
	memcpy((char*)&in, (char*)&dwIPAddr, 4);

	return CA2T(inet_ntoa(in));
}

DWORD CMMService::StringToIPAddr(CMMString strIPAddr)
{
	DWORD dwIPAddr = inet_addr((LPCSTR)CT2CA(strIPAddr));
	if (INADDR_NONE == dwIPAddr)
	{
		LPHOSTENT lpHost = gethostbyname((LPCSTR)CT2CA(strIPAddr));
		if (NULL == lpHost) return INADDR_NONE;

		dwIPAddr = ((LPIN_ADDR)lpHost->h_addr)->s_addr;
	}

	return dwIPAddr;
}

bool CMMService::GetMachineID(TCHAR szMachineID[Len_Machine_ID])
{
	GetImportIDEx(szMachineID);
	if (_T('0') != szMachineID[0]) return true;

	TCHAR szMACAddress[Len_Network_ID] = {};
	GetMACAddress(szMACAddress);

	CMMString strMacAddr = CMMMD5Checksum::GetMD5((BYTE*)szMACAddress, sizeof(szMACAddress));
	lstrcpyn(szMachineID, strMacAddr, MMCountString(szMachineID));

	return true;
}

CMMString CMMService::GetMachineID()
{
	TCHAR szMachineID[Len_Machine_ID] = {};
	GetMachineID(szMachineID);

	return szMachineID;
}

bool CMMService::GetMACAddress(TCHAR szMACAddress[Len_Network_ID])
{
	HINSTANCE hInstance = NULL;

	__try
	{
		//DLL
		hInstance = LoadLibrary(_T("NetApi32.dll"));
		if (hInstance == NULL) __leave;

		typedef BYTE __stdcall NetBiosProc(NCB * Ncb);
		NetBiosProc * pNetBiosProc = (NetBiosProc *)GetProcAddress(hInstance, "Netbios");
		if (pNetBiosProc == NULL) __leave;

		NCB Ncb;
		LANA_ENUM LanaEnum;
		ZeroMemory(&Ncb, sizeof(Ncb));
		ZeroMemory(&LanaEnum, sizeof(LanaEnum));

		Ncb.ncb_command = NCBENUM;
		Ncb.ncb_length = sizeof(LanaEnum);
		Ncb.ncb_buffer = (BYTE *)&LanaEnum;
		if ((pNetBiosProc(&Ncb) != NRC_GOODRET) || (LanaEnum.length == 0)) __leave;

		if (LanaEnum.length > 0)
		{
			tagAstatInfo Adapter;
			ZeroMemory(&Adapter, sizeof(Adapter));

			Ncb.ncb_command = NCBRESET;
			Ncb.ncb_lana_num = LanaEnum.lana[0];
			if (pNetBiosProc(&Ncb) != NRC_GOODRET) __leave;

			Ncb.ncb_command = NCBASTAT;
			Ncb.ncb_length = sizeof(Adapter);
			Ncb.ncb_buffer = (BYTE *)&Adapter;
			Ncb.ncb_lana_num = LanaEnum.lana[0];
			strcpy((char *)Ncb.ncb_callname, "*");
			if (pNetBiosProc(&Ncb) != NRC_GOODRET) __leave;

			for (INT i = 0; i < 6; i++)
			{
				assert((i * 2) < Len_Network_ID);
				_stprintf(&szMACAddress[i * 2], _T("%02X"), Adapter.AdapterStatus.adapter_address[i]);
			}
		}
	}

	__finally
	{
		if (hInstance != NULL)
		{
			FreeLibrary(hInstance);
			hInstance = NULL;
		}
		if (AbnormalTermination() == TRUE)
		{
			assert(FALSE);
		}
	}

	return true;
}

BOOL CMMService::GetMacAddr(BYTE* szSystemInfo, UINT uSystemInfoLen)
{
	UINT uErrorCode = 0;
	IP_ADAPTER_INFO iai;
	ULONG uSize = 0;
	DWORD dwResult = GetAdaptersInfo(&iai, &uSize);
	if (dwResult == ERROR_BUFFER_OVERFLOW)
	{
		IP_ADAPTER_INFO* piai = (IP_ADAPTER_INFO*)HeapAlloc(GetProcessHeap(), 0, uSize);
		if (piai != NULL)
		{
			dwResult = GetAdaptersInfo(piai, &uSize);
			if (ERROR_SUCCESS == dwResult)
			{
				IP_ADAPTER_INFO* piai2 = piai;
				while (piai2 != NULL && (uSystemInfoLen + piai2->AddressLength) < 4096U)
				{
					CopyMemory(szSystemInfo + uSystemInfoLen, piai2->Address, piai2->AddressLength);
					uSystemInfoLen += piai2->AddressLength;
					piai2 = piai2->Next;
				}
			}
			else
			{
				uErrorCode = 0xF0000000U + dwResult;
			}
			assert(HeapFree(GetProcessHeap(), 0, piai));
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		uErrorCode = 0xE0000000U + dwResult;
	}
	if (uErrorCode != 0U)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CMMService::GetHdiskSerial(BYTE* szSystemInfo, UINT uSystemInfoLen)
{
	OSVERSIONINFOEX ovi = {};
	ovi.dwOSVersionInfoSize = sizeof(ovi);
	CMMVersion::GetOSVersionInfo(&ovi);

	if (ovi.dwPlatformId != VER_PLATFORM_WIN32_NT)
	{
		// Only Windows 2000, Windows XP, Windows Server 2003...
		return FALSE;
	}
	else
	{
		if (!WinNTHDSerialNumAsPhysicalRead(szSystemInfo, &uSystemInfoLen, 1024))
		{
			WinNTHDSerialNumAsScsiRead(szSystemInfo, &uSystemInfoLen, 1024);
		}
	}
	return TRUE;
}

// CPU ID
void CMMService::GetCpuID(BYTE* szSystemInfo, UINT uSystemInfoLen)
// CPU ID
{
	BOOL bException = FALSE;
	BYTE szCpu[16] = {};
	UINT uCpuID = 0U;

	__try
	{
#ifndef _WIN64
		_asm
		{
			mov eax, 0
			cpuid
			mov dword ptr szCpu[0], ebx
			mov dword ptr szCpu[4], edx
			mov dword ptr szCpu[8], ecx
			mov eax, 1
			cpuid
			mov uCpuID, edx
		}
#endif
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		bException = TRUE;
	}

	if (!bException)
	{
		CopyMemory(szSystemInfo + uSystemInfoLen, &uCpuID, sizeof(UINT));
		uSystemInfoLen += sizeof(UINT);

		uCpuID = strlen((char*)szCpu);
		CopyMemory(szSystemInfo + uSystemInfoLen, szCpu, uCpuID);
		uSystemInfoLen += uCpuID;
	}
}

BOOL CMMService::DoIdentify(HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP,
	PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum,
	PDWORD lpcbBytesReturned)
{
	// Set up data structures for IDENTIFY command.
	pSCIP->cBufferSize = IDENTIFY_BUFFER_SIZE;
	pSCIP->irDriveRegs.bFeaturesReg = 0;
	pSCIP->irDriveRegs.bSectorCountReg = 1;
	pSCIP->irDriveRegs.bSectorNumberReg = 1;
	pSCIP->irDriveRegs.bCylLowReg = 0;
	pSCIP->irDriveRegs.bCylHighReg = 0;

	// calc the drive number.
	pSCIP->irDriveRegs.bDriveHeadReg = 0xA0 | ((bDriveNum & 1) << 4);

	// The command can either be IDE identify or ATAPI identify.
	pSCIP->irDriveRegs.bCommandReg = bIDCmd;
	pSCIP->bDriveNumber = bDriveNum;
	pSCIP->cBufferSize = IDENTIFY_BUFFER_SIZE;

	return DeviceIoControl(hPhysicalDriveIOCTL, DFP_RECEIVE_DRIVE_DATA,
		(LPVOID)pSCIP,
		sizeof(SENDCMDINPARAMS) - 1,
		(LPVOID)pSCOP,
		sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1,
		lpcbBytesReturned, NULL);
}

bool CMMService::RegisterHotKey(HWND hWnd, UINT uKeyID, WORD wHotKey)
{
	BYTE cbModifiers = 0;
	if (HIBYTE(wHotKey)&HOTKEYF_ALT) cbModifiers |= MOD_ALT;
	if (HIBYTE(wHotKey)&HOTKEYF_SHIFT) cbModifiers |= MOD_SHIFT;
	if (HIBYTE(wHotKey)&HOTKEYF_CONTROL) cbModifiers |= MOD_CONTROL;

	BOOL bSuccess = ::RegisterHotKey(hWnd, uKeyID, cbModifiers, LOBYTE(wHotKey));

	return (bSuccess == TRUE) ? true : false;
}

bool CMMService::UnRegisterHotKey(HWND hWnd, UINT uKeyID)
{
	BOOL bSuccess = ::UnregisterHotKey(hWnd, uKeyID);

	return (bSuccess == TRUE) ? true : false;
}

bool CMMService::GetWorkDirectory(OUT TCHAR szWorkDirectory[], IN WORD wBufferCount)
{
	TCHAR szModulePath[MAX_PATH] = _T("");
	GetModuleFileName(NULL, szModulePath, MMCountArray(szModulePath));

	for (INT i = lstrlen(szModulePath); i >= 0; i--)
	{
		if (szModulePath[i] == _T('\\'))
		{
			szModulePath[i] = 0;
			break;
		}
	}

	assert(szModulePath[0] != 0);
	lstrcpyn(szWorkDirectory, szModulePath, wBufferCount);

	return true;
}

CMMString CMMService::GetWorkDirectory()
{
	TCHAR szPath[MAX_PATH] = {};
	GetWorkDirectory(szPath, MMCountString(szPath));

	return szPath;
}

CMMString CMMService::GetCurrentPath()
{
	TCHAR szModulePath[MAX_PATH + 1] = _T("");
	::GetCurrentDirectory(MAX_PATH, szModulePath);

	return szModulePath;
}

CMMString CMMService::GetUserDataPath(CMMString strFolderName)
{
	TCHAR szPath[MAX_PATH + 1] = {};
	if (false == SHGetSpecialFolderPath(NULL, szPath, CSIDL_LOCAL_APPDATA, FALSE)
		&& false == SHGetSpecialFolderPath(NULL, szPath, CSIDL_APPDATA, FALSE)
		&& false == ::GetTempPath(MAX_PATH, szPath))
	{
		return CMMString();
	}

	if (false == strFolderName.empty())
	{
		PathAppend(szPath, strFolderName);
	}

	PathAddBackslash(szPath);
	SHCreateDirectory(NULL, szPath);

	return szPath;
}

CMMString CMMService::GetLocalTempPath()
{
	TCHAR szPath[MAX_PATH + 1] = {};
	GetTempPath(MAX_PATH, szPath);
	PathAddBackslash(szPath);
	return szPath;
}

CMMString CMMService::GetAppName()
{
	int nPos = -1;
	std::wstring sName;
	TCHAR szPath[MAX_PATH + 1] = { 0 };
	GetModuleFileName(NULL, szPath, MAX_PATH);

	sName = szPath;
	nPos = sName.rfind(L"\\");
	if (nPos > 0)
		sName = sName.substr(nPos + 1, sName.length() - nPos - 1);
	else
		sName = L"";

	return sName.c_str();
}

CMMString CMMService::GetAppFile()
{
	TCHAR szPath[MAX_PATH + 1] = {};
	GetModuleFileName(NULL, szPath, MAX_PATH);

	return szPath;
}

//GUID
CMMString CMMService::ProductGUID()
{
	CMMString strGUID;
	GUID guid;
	CoCreateGuid(&guid);
	strGUID.Format(_T("{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}"),
		guid.Data1, guid.Data2,
		guid.Data3, guid.Data4[0],
		guid.Data4[1], guid.Data4[2],
		guid.Data4[3], guid.Data4[4],
		guid.Data4[5], guid.Data4[6],
		guid.Data4[7]);

	return strGUID;
}

int CMMService::RandValue(int nMin, int nMax)
{
	if (nMax <= nMin) return nMin;
	return nMin + rand() % (nMax - nMin + 1);
}

double CMMService::RandValue(double lfMin, double lfMax)
{
	if (lfMax <= lfMin) return lfMin;
	return lfMin + (lfMax - lfMin) * (rand() * 1.0 / RAND_MAX);
}

bool CMMService::IsZero(DWORD dwNum)
{
	return (abs((long long)dwNum) <= 1e-15);
}

std::string CMMService::EncryptBase64(unsigned char const* bytes_to_encode, unsigned int in_len)
{
	static const std::string base64_chars =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";

	std::string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (in_len--)
	{
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3)
		{
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; (i < 4); i++) ret += base64_chars[char_array_4[i]];

			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
			ret += base64_chars[char_array_4[j]];

		while ((i++ < 3))
			ret += '=';
	}

	return ret;
}

std::string CMMService::EncryptBase64Image(unsigned char const *bytes_to_encode, unsigned int in_len)
{
	std::string strBase64 = CMMService::EncryptBase64(bytes_to_encode, in_len);
	enMMImageType ImageType = CMMFile::ParseImageType(std::vector<BYTE>(bytes_to_encode, bytes_to_encode + in_len));
	std::string strUrl = "data:image/";
	switch (ImageType)
	{
		case ImageType_Png:
		{
			strUrl += "png;base64,";

			break;
		}
		case ImageType_Bmp:
		{
			strUrl += "bmp;base64,";

			break;
		}
		case ImageType_Jpg:
		{
			strUrl += "jpeg ;base64,";

			break;
		}
		case ImageType_Gif:
		{
			strUrl += "gif ;base64,";

			break;
		}
	}

	strUrl += strBase64;

	return strUrl;
}

int CMMService::CompareVersion(LPCTSTR lpszVersion1, LPCTSTR lpszVersion2)
{
	if (NULL == lpszVersion1 || NULL == lpszVersion2) return 0;

	std::vector<int> vecVersion1 = CMMStrHelp::ParseIntFromString(lpszVersion1, _T('.'));
	std::vector<int> vecVersion2 = CMMStrHelp::ParseIntFromString(lpszVersion2, _T('.'));
	int nCount = max(vecVersion1.size(), vecVersion2.size());
	for (int i = 0; i < nCount; i++)
	{
		int nV1 = (i < vecVersion1.size()) ? vecVersion1[i] : 0;
		int nV2 = (i < vecVersion2.size()) ? vecVersion2[i] : 0;
		if (nV1 > nV2) return 1;
		if (nV1 < nV2) return -1;
	}

	return 0;
}

BOOL CMMService::WinNTHDSerialNumAsScsiRead(BYTE* dwSerial, UINT* puSerialLen, UINT uMaxSerialLen)
{
	BOOL bInfoLoaded = FALSE;

	for (int iController = 0; iController < 2; ++iController)
	{
		HANDLE hScsiDriveIOCTL = 0;
		TCHAR   szDriveName[256];

		//  Try to get a handle to PhysicalDrive IOCTL, report failure
		//  and exit if can't.
		_snwprintf(szDriveName, sizeof(szDriveName), L"\\\\.\\Scsi%d:", iController);

		//  Windows NT, Windows 2000, any rights should do
		hScsiDriveIOCTL = CreateFile(szDriveName,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, 0, NULL);

		// if (hScsiDriveIOCTL == INVALID_HANDLE_VALUE)
		//    printf ("Unable to open SCSI controller %d, error code: 0x%lX\n",
		//            controller, GetLastError ());

		if (hScsiDriveIOCTL != INVALID_HANDLE_VALUE)
		{
			int iDrive = 0;
			for (iDrive = 0; iDrive < 2; ++iDrive)
			{
				char szBuffer[sizeof(SRB_IO_CONTROL) + SENDIDLENGTH] = {};

				SRB_IO_CONTROL* p = (SRB_IO_CONTROL*)szBuffer;
				SENDCMDINPARAMS* pin = (SENDCMDINPARAMS*)(szBuffer + sizeof(SRB_IO_CONTROL));
				DWORD dwResult;

				p->HeaderLength = sizeof(SRB_IO_CONTROL);
				p->Timeout = 10000;
				p->Length = SENDIDLENGTH;
				p->ControlCode = IOCTL_SCSI_MINIPORT_IDENTIFY;
				strncpy((char*)p->Signature, "SCSIDISK", 8);

				pin->irDriveRegs.bCommandReg = IDE_ATA_IDENTIFY;
				pin->bDriveNumber = iDrive;

				if (DeviceIoControl(hScsiDriveIOCTL, IOCTL_SCSI_MINIPORT,
					szBuffer,
					sizeof(SRB_IO_CONTROL) + sizeof(SENDCMDINPARAMS) - 1,
					szBuffer,
					sizeof(SRB_IO_CONTROL) + SENDIDLENGTH,
					&dwResult, NULL))
				{
					SENDCMDOUTPARAMS* pOut = (SENDCMDOUTPARAMS*)(szBuffer + sizeof(SRB_IO_CONTROL));
					IDSECTOR* pId = (IDSECTOR*)(pOut->bBuffer);
					if (pId->sModelNumber[0])
					{
						if (*puSerialLen + 20U <= uMaxSerialLen)
						{
							// 序列号
							CopyMemory(dwSerial + *puSerialLen, ((USHORT*)pId) + 10, 20);

							// Cut off the trailing blanks
							UINT i = 0;
							for (i = 20; i != 0U && ' ' == dwSerial[*puSerialLen + i - 1]; --i)
							{
							}
							*puSerialLen += i;

							// 型号
							CopyMemory(dwSerial + *puSerialLen, ((USHORT*)pId) + 27, 40);
							// Cut off the trailing blanks
							for (i = 40; i != 0U && ' ' == dwSerial[*puSerialLen + i - 1]; --i)
							{
							}
							*puSerialLen += i;

							bInfoLoaded = TRUE;
						}
						else
						{
							::CloseHandle(hScsiDriveIOCTL);
							return bInfoLoaded;
						}
					}
				}
			}
			::CloseHandle(hScsiDriveIOCTL);
		}
	}
	return bInfoLoaded;
}


BOOL CMMService::WinNTHDSerialNumAsPhysicalRead(BYTE* dwSerial, UINT* puSerialLen, UINT uMaxSerialLen)
{
#define  DFP_GET_VERSION          0x00074080
	BOOL bInfoLoaded = FALSE;

	for (UINT uDrive = 0; uDrive < 4; ++uDrive)
	{
		HANDLE hPhysicalDriveIOCTL = 0;

		//  Try to get a handle to PhysicalDrive IOCTL, report failure
		//  and exit if can't.
		TCHAR szDriveName[256];
		_snwprintf(szDriveName, sizeof(szDriveName), L"\\\\.\\PhysicalDrive%d", uDrive);

		//  Windows NT, Windows 2000, must have admin rights
		hPhysicalDriveIOCTL = CreateFile(szDriveName,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, 0, NULL);

		if (hPhysicalDriveIOCTL != INVALID_HANDLE_VALUE)
		{
			GETVERSIONOUTPARAMS VersionParams = {};
			DWORD               cbBytesReturned = 0;

			// Get the version, etc of PhysicalDrive IOCTL
			if (DeviceIoControl(hPhysicalDriveIOCTL, DFP_GET_VERSION,
				NULL,
				0,
				&VersionParams,
				sizeof(GETVERSIONOUTPARAMS),
				&cbBytesReturned, NULL))
			{
				// If there is a IDE device at number "i" issue commands
				// to the device
				if (VersionParams.bIDEDeviceMap != 0)
				{
					BYTE             bIDCmd = 0;   // IDE or ATAPI IDENTIFY cmd
					SENDCMDINPARAMS  scip = {};

					// Now, get the ID sector for all IDE devices in the system.
					// If the device is ATAPI use the IDE_ATAPI_IDENTIFY command,
					// otherwise use the IDE_ATA_IDENTIFY command
					bIDCmd = (VersionParams.bIDEDeviceMap >> uDrive & 0x10) ? IDE_ATAPI_IDENTIFY : IDE_ATA_IDENTIFY;
					BYTE IdOutCmd[sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1] = {};

					if (DoIdentify(hPhysicalDriveIOCTL,
						&scip,
						(PSENDCMDOUTPARAMS)&IdOutCmd,
						(BYTE)bIDCmd,
						(BYTE)uDrive,
						&cbBytesReturned))
					{
						if (*puSerialLen + 20U <= uMaxSerialLen)
						{
							CopyMemory(dwSerial + *puSerialLen, ((USHORT*)(((PSENDCMDOUTPARAMS)IdOutCmd)->bBuffer)) + 10, 20);  // 序列号

																																// Cut off the trailing blanks
							UINT i = 0;
							for (i = 20; i != 0U && ' ' == dwSerial[*puSerialLen + i - 1]; --i) {}
							*puSerialLen += i;

							CopyMemory(dwSerial + *puSerialLen, ((USHORT*)(((PSENDCMDOUTPARAMS)IdOutCmd)->bBuffer)) + 27, 40); // 型号

																															   // Cut off the trailing blanks
							for (i = 40; i != 0U && ' ' == dwSerial[*puSerialLen + i - 1]; --i) {}
							*puSerialLen += i;

							bInfoLoaded = TRUE;
						}
						else
						{
							::CloseHandle(hPhysicalDriveIOCTL);
							return bInfoLoaded;
						}
					}
				}
			}
			CloseHandle(hPhysicalDriveIOCTL);
		}
	}
	return bInfoLoaded;
}

bool CMMService::GetImportIDEx(TCHAR szMachineID[Len_Machine_ID], TCHAR szCPUID[20])
{
	// 1. CPU
	unsigned int cpuInfo[4] = { 0 };
	__cpuid((int*)cpuInfo, 1); // eax=1 -> Processor Info
	DWORD s1 = cpuInfo[3];     // EDX
	DWORD s2 = cpuInfo[0];     // EAX

	TCHAR szCpuBuf[32] = { 0 };
	_stprintf_s(szCpuBuf, _T("%08X%08X"), s1, s2);

	if (szCPUID)
		_tcsncpy_s(szCPUID, 20, szCpuBuf, _TRUNCATE);

	// 2. 硬盘序列号
	DWORD volumeSerial = 0;
	if (!GetVolumeInformation(_T("C:\\"), nullptr, 0, &volumeSerial, nullptr, nullptr, nullptr, 0))
		volumeSerial = 0;

	// 3. 合并信息
	BYTE buffer[64] = { 0 };
	memcpy(buffer, &s1, sizeof(s1));
	memcpy(buffer + 4, &s2, sizeof(s2));
	memcpy(buffer + 8, &volumeSerial, sizeof(volumeSerial));

	// 4. 生成 MD5
	assert(Len_Machine_ID >= Len_MD5);
	CMMString strMacAddr = CMMMD5Checksum::GetMD5(buffer, sizeof(s1) + sizeof(s2) + sizeof(volumeSerial));
	assert(Len_Machine_ID >= (int)strMacAddr.size() + 1);
	_tcsncpy_s(szMachineID, Len_Machine_ID, strMacAddr.c_str(), _TRUNCATE);

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
#else // DuiPlatform_SDL

namespace
{
	void CopyTChar(TCHAR *pszDst, WORD wBufferCount, const CMMString &strSrc)
	{
		if (NULL == pszDst || 0 == wBufferCount) return;

		pszDst[0] = 0;
		const size_t nCopy = (strSrc.length() < (size_t)(wBufferCount - 1)) ? strSrc.length() : (size_t)(wBufferCount - 1);
		if (nCopy > 0)
		{
			memcpy(pszDst, strSrc.c_str(), nCopy * sizeof(TCHAR));
		}

		pszDst[nCopy] = 0;
	}

	CMMString TrimTrailingSlash(CMMString strPath)
	{
		while (false == strPath.empty())
		{
			const TCHAR ch = strPath[strPath.length() - 1];
			if (ch != _T('\\') && ch != _T('/')) break;
			strPath.erase(strPath.length() - 1, 1);
		}

		return strPath;
	}
}

bool CMMService::SetClipboardString(HWND /*hWnd*/, LPCTSTR lpszString)
{
	if (NULL == lpszString) lpszString = _T("");
	const std::string strUtf8 = MMStringToUtf8(lpszString);
	return SDL_SetClipboardText(strUtf8.c_str());
}

CMMString CMMService::GetClipboardString()
{
	char *pszText = SDL_GetClipboardText();
	if (NULL == pszText) return CMMString();

	CMMString strText = Utf8ToMMString(pszText).c_str();
	SDL_free(pszText);
	return strText;
}

std::vector<CMMString> CMMService::GetClipboardFile()
{
	return std::vector<CMMString>();
}

int CMMService::GetCompileDate()
{
	char sMonth[5] = "";
	int nMonth = 0, nDay = 0, nYear = 0;
	char sMonthName[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
	char sTime[20] = __DATE__;
	sscanf(sTime, "%s %d %d", sMonth, &nDay, &nYear);
	nMonth = (int)((strstr(sMonthName, sMonth) - sMonthName) / 3 + 1);
	return nYear * 10000 + nMonth * 100 + nDay;
}

CMMString CMMService::IPAddrToString(DWORD dwIPAddr)
{
	const BYTE *p = (const BYTE *)&dwIPAddr;
	CMMString strIP;
	strIP.Format(_T("%u.%u.%u.%u"), (UINT)p[0], (UINT)p[1], (UINT)p[2], (UINT)p[3]);
	return strIP;
}

DWORD CMMService::StringToIPAddr(CMMString strIPAddr)
{
	const std::string strUtf8 = MMStringToUtf8(strIPAddr);
	unsigned int a = 0, b = 0, c = 0, d = 0;
	if (4 != sscanf(strUtf8.c_str(), "%u.%u.%u.%u", &a, &b, &c, &d))
	{
		return INADDR_NONE;
	}
	if (a > 255 || b > 255 || c > 255 || d > 255)
	{
		return INADDR_NONE;
	}

	DWORD dwIPAddr = 0;
	BYTE *p = (BYTE *)&dwIPAddr;
	p[0] = (BYTE)a;
	p[1] = (BYTE)b;
	p[2] = (BYTE)c;
	p[3] = (BYTE)d;
	return dwIPAddr;
}

bool CMMService::GetWorkDirectory(OUT TCHAR szWorkDirectory[], IN WORD wBufferCount)
{
	if (NULL == szWorkDirectory || 0 == wBufferCount) return false;

	CMMString strPath = TrimTrailingSlash(GetWorkDirectory());
	CopyTChar(szWorkDirectory, wBufferCount, strPath);
	return false == strPath.empty();
}

CMMString CMMService::GetWorkDirectory()
{
	const char *pszBase = SDL_GetBasePath();
	if (NULL == pszBase) return CMMString();

	CMMString strPath = TrimTrailingSlash(Utf8ToMMString(pszBase));
#if defined(__APPLE__)
	// SDL_GetBasePath() for a .app is Contents/Resources (not visible as a
	// normal folder in Finder). Walk up to the directory that contains the
	// .app so relative paths match Windows/Linux: exe in Debug/, DuiProj at ../
	const CMMString strBundleRes(_T(".app/Contents/Resources"));
	if (strPath.size() > strBundleRes.size()
		&& 0 == strPath.compare(strPath.size() - strBundleRes.size(), strBundleRes.size(), strBundleRes))
	{
		strPath.resize(strPath.size() - strBundleRes.size());
		const size_t nSlash = strPath.find_last_of(_T('/'));
		if (CMMString::npos != nSlash)
		{
			strPath.resize(nSlash);
		}
	}
#endif
	return strPath;
}

CMMString CMMService::GetCurrentPath()
{
	char *pszPath = SDL_GetCurrentDirectory();
	if (NULL == pszPath) return CMMString();

	CMMString strPath = TrimTrailingSlash(Utf8ToMMString(pszPath));
	SDL_free(pszPath);
	return strPath;
}

CMMString CMMService::GetUserDataPath(CMMString strFolderName)
{
	const std::string strOrg = "DUIThink";
	const std::string strApp = strFolderName.empty() ? "App" : MMStringToUtf8(strFolderName);
	char *pszPath = SDL_GetPrefPath(strOrg.c_str(), strApp.c_str());
	if (NULL == pszPath) return CMMString();

	CMMString strPath = Utf8ToMMString(pszPath);
	SDL_free(pszPath);

	if (false == strPath.empty())
	{
		const TCHAR ch = strPath[strPath.length() - 1];
		if (ch != _T('\\') && ch != _T('/'))
		{
#ifdef _WIN32
			strPath += _T('\\');
#else
			strPath += _T('/');
#endif
		}
	}

	return strPath;
}

CMMString CMMService::GetLocalTempPath()
{
	const char *pszTemp = getenv("TMPDIR");
	if (NULL == pszTemp || 0 == pszTemp[0]) pszTemp = getenv("TEMP");
	if (NULL == pszTemp || 0 == pszTemp[0]) pszTemp = getenv("TMP");
#ifdef _WIN32
	if (NULL == pszTemp || 0 == pszTemp[0]) pszTemp = ".";
#else
	if (NULL == pszTemp || 0 == pszTemp[0]) pszTemp = "/tmp";
#endif

	CMMString strPath = Utf8ToMMString(pszTemp);
	if (false == strPath.empty())
	{
		const TCHAR ch = strPath[strPath.length() - 1];
		if (ch != _T('\\') && ch != _T('/'))
		{
#ifdef _WIN32
			strPath += _T('\\');
#else
			strPath += _T('/');
#endif
		}
	}

	return strPath;
}

CMMString CMMService::GetAppName()
{
	const char *pszName = SDL_GetAppMetadataProperty(SDL_PROP_APP_METADATA_NAME_STRING);
	if (pszName && pszName[0]) return Utf8ToMMString(pszName);

	CMMString strPath = GetWorkDirectory();
	const int nPos = (int)strPath.rfind(_T('\\'));
	const int nPos2 = (int)strPath.rfind(_T('/'));
	const int nSlash = max(nPos, nPos2);
	if (nSlash >= 0 && nSlash + 1 < (int)strPath.length())
	{
		return CMMString(strPath.substr(nSlash + 1).c_str());
	}

	return strPath;
}

CMMString CMMService::GetAppFile()
{
	CMMString strDir = GetWorkDirectory();
	CMMString strName = GetAppName();
	if (strDir.empty()) return strName;
	if (strName.empty()) return strDir;

	const TCHAR ch = strDir[strDir.length() - 1];
	if (ch == _T('\\') || ch == _T('/')) return strDir + strName;
#ifdef _WIN32
	return strDir + _T('\\') + strName;
#else
	return strDir + _T('/') + strName;
#endif
}

CMMString CMMService::ProductGUID()
{
	GUID guid = {};
	for (int i = 0; i < 4; ++i)
	{
		const Uint32 uBits = SDL_rand_bits();
		memcpy(((BYTE *)&guid) + i * 4, &uBits, 4);
	}

	CMMString strGUID;
	strGUID.Format(_T("{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}"),
		guid.Data1, guid.Data2,
		guid.Data3, guid.Data4[0],
		guid.Data4[1], guid.Data4[2],
		guid.Data4[3], guid.Data4[4],
		guid.Data4[5], guid.Data4[6],
		guid.Data4[7]);

	return strGUID;
}

int CMMService::RandValue(int nMin, int nMax)
{
	if (nMax <= nMin) return nMin;
	const int nRange = nMax - nMin;
	return nMin + (int)(SDL_rand(nRange));
}

double CMMService::RandValue(double lfMin, double lfMax)
{
	if (lfMax <= lfMin) return lfMin;
	return lfMin + (lfMax - lfMin) * (double)SDL_randf();
}

bool CMMService::IsZero(DWORD dwNum)
{
	return (abs((long long)dwNum) <= 1e-15);
}

std::string CMMService::EncryptBase64(unsigned char const* bytes_to_encode, unsigned int in_len)
{
	static const std::string base64_chars =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";

	std::string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (in_len--)
	{
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3)
		{
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; (i < 4); i++) ret += base64_chars[char_array_4[i]];
			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j < 3; j++) char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++) ret += base64_chars[char_array_4[j]];
		while ((i++ < 3)) ret += '=';
	}

	return ret;
}

int CMMService::CompareVersion(LPCTSTR lpszVersion1, LPCTSTR lpszVersion2)
{
	if (NULL == lpszVersion1 || NULL == lpszVersion2) return 0;

	std::vector<int> vecVersion1 = CMMStrHelp::ParseIntFromString(lpszVersion1, _T('.'));
	std::vector<int> vecVersion2 = CMMStrHelp::ParseIntFromString(lpszVersion2, _T('.'));
	int nCount = max((int)vecVersion1.size(), (int)vecVersion2.size());
	for (int i = 0; i < nCount; i++)
	{
		int nV1 = (i < (int)vecVersion1.size()) ? vecVersion1[i] : 0;
		int nV2 = (i < (int)vecVersion2.size()) ? vecVersion2[i] : 0;
		if (nV1 > nV2) return 1;
		if (nV1 < nV2) return -1;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////
#endif
