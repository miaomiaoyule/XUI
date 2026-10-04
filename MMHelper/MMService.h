#ifndef __MM_SERVICE_H__
#define __MM_SERVICE_H__

#pragma once

#if !defined(DuiPlatform_SDL)
#include "winioctl.h"
#include "Iprtrmib.h"
#include "Iptypes.h"
#include "Iphlpapi.h"

//////////////////////////////////////////////////////////////////////////////////
typedef struct _IDSECTOR
{
	USHORT  wGenConfig;
	USHORT  wNumCyls;
	USHORT  wReserved;
	USHORT  wNumHeads;
	USHORT  wBytesPerTrack;
	USHORT  wBytesPerSector;
	USHORT  wSectorsPerTrack;
	USHORT  wVendorUnique[3];
	CHAR    sSerialNumber[20];
	USHORT  wBufferType;
	USHORT  wBufferSize;
	USHORT  wECCSize;
	CHAR    sFirmwareRev[8];
	CHAR    sModelNumber[40];
	USHORT  wMoreVendorUnique;
	USHORT  wDoubleWordIO;
	USHORT  wCapabilities;
	USHORT  wReserved1;
	USHORT  wPIOTiming;
	USHORT  wDMATiming;
	USHORT  wBS;
	USHORT  wNumCurrentCyls;
	USHORT  wNumCurrentHeads;
	USHORT  wNumCurrentSectorsPerTrack;
	ULONG   ulCurrentSectorCapacity;
	USHORT  wMultSectorStuff;
	ULONG   ulTotalAddressableSectors;
	USHORT  wSingleWordDMA;
	USHORT  wMultiWordDMA;
	BYTE    bReserved[128];
} IDSECTOR, *PIDSECTOR;

typedef struct _SRB_IO_CONTROL
{
	ULONG HeaderLength;
	UCHAR Signature[8];
	ULONG Timeout;
	ULONG ControlCode;
	ULONG ReturnCode;
	ULONG Length;
} SRB_IO_CONTROL, *PSRB_IO_CONTROL;

typedef struct _GETVERSIONOUTPARAMS
{
	BYTE bVersion;
	BYTE bRevision;
	BYTE bReserved;
	BYTE bIDEDeviceMap;
	DWORD fCapabilities;
	DWORD dwReserved[4];
} GETVERSIONOUTPARAMS, *PGETVERSIONOUTPARAMS, *LPGETVERSIONOUTPARAMS;
#endif // !DuiPlatform_SDL

//////////////////////////////////////////////////////////////////////////
class MMHELPER_API CMMService
{
	//system
public:
	//clipboard
	static bool SetClipboardString(HWND hWnd, LPCTSTR lpszString);
	static CMMString GetClipboardString();
	static std::vector<CMMString> GetClipboardFile();
#if !defined(DuiPlatform_SDL)
	static bool SetClipboardFiles(HWND hWndOwner, std::vector<CMMString> vecFiles, bool bCopy = true);
	static DROPEFFECT GetClipboardDropEffect();
#endif

	//compile date
	static int GetCompileDate();

	//IP to str
	static CMMString IPAddrToString(DWORD dwIPAddr);
	static DWORD StringToIPAddr(CMMString strIPAddr);

#if !defined(DuiPlatform_SDL)
	//machine
	static bool GetMachineID(TCHAR szMachineID[Len_Machine_ID]);
	static CMMString GetMachineID();
	static bool GetMACAddress(TCHAR szMACAddress[Len_Network_ID]);
	static BOOL GetMacAddr(BYTE* szSystemInfo, UINT uSystemInfoLen);
	static BOOL GetHdiskSerial(BYTE* szSystemInfo, UINT uSystemInfoLen);
	static void GetCpuID(BYTE* szSystemInfo, UINT uSystemInfoLen);
	static BOOL DoIdentify(HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP, PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum, PDWORD lpcbBytesReturned);

	//hotkey
	static bool RegisterHotKey(HWND hWnd, UINT uKeyID, WORD wHotKey);
	static bool UnRegisterHotKey(HWND hWnd, UINT uKeyID);
#endif

	//work dir
	static bool GetWorkDirectory(OUT TCHAR szWorkDirectory[], IN WORD wBufferCount);
	static CMMString GetWorkDirectory();
	static CMMString GetCurrentPath();
	static CMMString GetUserDataPath(CMMString strFolderName);
	static CMMString GetLocalTempPath();
	static CMMString GetAppName();
	static CMMString GetAppFile();

	//guid
	static CMMString ProductGUID();

	//rand
	static int RandValue(int nMin, int nMax);
	static double RandValue(double lfMin, double lfMax);
	static bool IsZero(DWORD dwNum);

	//encrypt
	static std::string EncryptBase64(unsigned char const* bytes_to_encode, unsigned int in_len);
	static std::string EncryptBase64Image(unsigned char const* bytes_to_encode, unsigned int in_len);

	//version
	static int CompareVersion(LPCTSTR lpszVersion1, LPCTSTR lpszVersion2);

#if !defined(DuiPlatform_SDL)
	//help
private:
	static BOOL WinNTHDSerialNumAsScsiRead(BYTE* dwSerial, UINT* puSerialLen, UINT uMaxSerialLen);
	static BOOL WinNTHDSerialNumAsPhysicalRead(BYTE* dwSerial, UINT* puSerialLen, UINT uMaxSerialLen);
	static bool GetImportIDEx(TCHAR szMachineID[Len_Machine_ID], TCHAR szCPUID[20] = NULL);
#endif
};

//////////////////////////////////////////////////////////////////////////////////
#endif
