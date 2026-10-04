#ifndef __MM_FILE_H__
#define __MM_FILE_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
//file type
enum enMMFileType
{
	FileType_None,
	FileType_Dir,
	FileType_Doc,
	FileType_Image,
	FileType_Movie,
	FileType_Zip,
	FileType_Exe,
	FileType_Lnk,
	FileType_File,
};

//image type
enum enMMImageType
{
	ImageType_None,
	ImageType_Png,
	ImageType_Bmp,
	ImageType_Jpg,
	ImageType_Gif,
};

//file encode
enum enMMFileEncode
{
	FileEncode_Null = -1,
	FileEncode_Ansi = 0,
	FileEncode_Unicode,
	FileEncode_Unicode_big_endian,
	FileEncode_UTF8_Bom,
	FileEncode_UTF8,
};

//////////////////////////////////////////////////////////////////////////
class MMHELPER_API CMMFile
{
public:
	//************************************
	// Description:	combin folder + filename
	// Parameter: 	LPCTSTR lpszPath 例：C:\Text or C:\Text\
	// Parameter: 	LPCTSTR lpszFileName 例：Text.txt
	//************************************
	static CMMString CombinFile(LPCTSTR lpszPath, LPCTSTR lpszFileName);

	//************************************
	// Description:	parse filetype from file or ext
	// Parameter: 	LPCTSTR lpszFile 例：C:\Text.txt or Text.txt or txt or C:\Text
	//************************************
	static enMMFileType ParseFileType(LPCTSTR lpszFile);
	static enMMImageType ParseImageType(LPCTSTR lpszFile);
	static enMMImageType ParseImageType(const std::vector<BYTE> &vecData);
	static bool IsWebpFile(LPCTSTR lpszFile);
	static bool IsWebpFile(const std::vector<BYTE> &vecData);
	static bool IsGifFile(LPCTSTR lpszFile);
	static bool IsGifFile(const std::vector<BYTE> &vecData);
	static bool IsSvgFile(LPCTSTR lpszFile);
	static bool IsSvgFile(const std::vector<BYTE> &vecData);

	//************************************
	// Description:	从一个完整路径字符串解析出文件名、扩展名
	// Parameter: 	LPCTSTR lpszFileName 例：C:\Text.txt 或 Text.txt
	// Parameter: 	CMMString & strName 例：Text
	// Parameter: 	CMMString & strExt 例：txt
	//************************************
	static bool ParseFileName(LPCTSTR lpszFileName, CMMString &strName, CMMString &strExt);

	//************************************
	// Description:	从一个完整路径字符串解析出路径、文件名
	// Parameter: 	LPCTSTR lpszFileFull 例：C:\Text.txt 
	// Parameter: 	CMMString & strPath 例：C:\
	// Parameter: 	CMMString & strName 例：Text.txt
	//************************************
	static bool ParseFilePathName(LPCTSTR lpszFileFull, CMMString &strPath, CMMString &strName);

	//************************************
	// Description:	
	// Parameter: 	FILE * pFile
	//************************************
	static enMMFileEncode GetFileEncode(FILE *pFile);

	//************************************
	// Description:	
	// Parameter: 	const char * pStr 根据文件内容判断文档格式，长度应该 >= 2，否则返回[FileEncode_Null]
	//************************************
	static enMMFileEncode GetFileEncode(const char *pStr);

	//************************************
	// Description:	读取文件数据
	// Parameter: 	IN LPCTSTR lpszFileFull 例：C:\Text.txt 
	// Parameter: 	OUT std::vector<BYTE> vecData
	// Parameter: 	DWORD dwSizeLimit : 限制取文件的前dwSizeLimit个字节
	//************************************
	static bool GetFileData(IN LPCTSTR lpszFileFull, OUT std::vector<BYTE> &vecData, DWORD dwSizeLimit = -1);

	//************************************
	// Description:	读取文件数据
	// Parameter: 	LPCTSTR lpszFileFull 例：C:\Text.txt 
	// Parameter: 	CMMString &strData
	//************************************
	static bool GetFileData(IN LPCTSTR lpszFileFull, OUT CMMString &strData);

#ifndef DuiPlatform_SDL
	//************************************
	// Description:	解析文件的PCID
	// Parameter: 	LPCTSTR lpszFileFull 例：C:\Text.txt 
	//************************************
	static PCIDLIST_ABSOLUTE GetPCIDLFromPath(LPCTSTR lpszFileFull);

	//************************************
	// Description:	读取文件tip
	// Parameter: 	LPCTSTR lpszFileFull 例：C:\Text.txt 
	//************************************
	static CMMString GetFileTip(IN LPCTSTR lpszFileFull);

	//************************************
	// Description:	return local file modifytime
	// Parameter: 	LPCTSTR lpszFileFull 例：C:\Text.txt 
	//************************************
	static SYSTEMTIME GetFileModifyTime(IN LPCTSTR lpszFileFull);

	//************************************
	// Description:	return local file createtime
	// Parameter: 	LPCTSTR lpszFileFull 例：C:\Text.txt 
	//************************************
	static SYSTEMTIME GetFileCreateTime(IN LPCTSTR lpszFileFull);

	//************************************
	// Description:	return local file last access time
	// Parameter: 	LPCTSTR lpszFileFull 例：C:\Text.txt 
	//************************************
	static SYSTEMTIME GetFileLastAccessTime(IN LPCTSTR lpszFileFull);

	//************************************
	// Description:	return file size
	// Parameter: 	LPCTSTR lpszFileFull 例：C:\Text.txt 
	//************************************
	static uint64_t GetFileSize(IN LPCTSTR lpszFileFull);

	//************************************
	// Description:	return file version
	// Parameter: 	LPCTSTR lpszFileFull 例：C:\Text.txt 
	//************************************
	static bool GetFileVersion(IN LPCTSTR lpszFileFull, OUT DWORD &dwVersionInfo);
	static CMMString GetFileVersion(IN LPCTSTR lpszFileFull);
	static CMMString GetAppVersion();

	//************************************
	// Description:	get folder of dir
	// Parameter: 	LPCTSTR lpszDirFull 例：C:\windows
	//************************************
	static std::vector<CMMString> GetFolderOfDir(IN LPCTSTR lpszDirFull);

	//************************************
	// Description:	get file(uninclude folder) of dir
	// Parameter: 	LPCTSTR lpszDirFull 例：C:\windows
	//************************************
	static std::vector<CMMString> GetFileOfDir(IN LPCTSTR lpszDirFull);

	//************************************
	// Description:	get all files of dir
	// Parameter: 	LPCTSTR lpszDirFull 例：C:\windows
	//************************************
	static std::vector<CMMString> GetFileAndFolderOfDir(IN LPCTSTR lpszDirFull);

	//************************************
	// Description:	get size of dir
	// Parameter: 	LPCTSTR lpszDirFull 例：C:\windows
	//************************************
	static uint64_t GetFolderSize(IN LPCTSTR lpszDirFull);

	//************************************
	// Description:	write string to file
	// Parameter: 	LPCTSTR lpszFileFull 例：C:\Text.txt 
	// Parameter: 	CMMString &strData 
	//************************************
	static bool WriteFileData(IN LPCTSTR lpszFileFull, IN CMMString &strData, bool bClearOld = false);
	
	//************************************
	// Description:	write byte to file
	// Parameter: 	LPCTSTR lpszFileFull 例：C:\Text.txt 
	// Parameter: 	std::vector<BYTE> &vecData 
	//************************************
	static bool WriteFileData(IN LPCTSTR lpszFileFull, IN std::vector<BYTE> &vecData, bool bClearOld = false);
	
	//************************************
	// Description:	write byte to file
	// Parameter: 	LPCTSTR lpszFileFull 例：C:\Text.txt 
	// Parameter: 	std::vector<BYTE> &vecData 
	//************************************
	static bool WriteFileData(IN LPCTSTR lpszFileFull, IN std::vector<BYTE> &vecData, int nOffsetOfFirst = 0);

	//************************************
	// Description:	递归清除路径下的含有特定名字的文件
	// Parameter: 	CMMString strPath 例：C:\Program Files (x86)
	// Parameter: 	CMMString strHasName 例：qq
	//************************************
	static bool RemoveFileByHasName(CMMString strPath, CMMString strHasName);

	//************************************
	// Description:	清空路径下的文件
	// Parameter: 	CMMString strPath 例：C:\Program Files (x86)
	//************************************
	static bool ClearFilesOfFolder(CMMString strPath);

	//************************************
	// Description:	删除文件夹
	// Parameter: 	CMMString strPath 例：C:\Program Files (x86)
	//************************************
	static bool DeleteFolder(CMMString strPath);

	//************************************
	// Description:	open folder and select file 
	// Parameter: 	CMMString strFileFull 例：C:\user\desktop\test.txt
	//************************************
	static bool OpenFolderAndSelectFile(CMMString strFileFull);

	//************************************
	// Description:	copy or move file or folder to dest 
	// Parameter: 	CMMString strSrc 例：C:\user\desktop\test.txt C:\user\desktop\test
	// Parameter: 	CMMString strDest 例：D:\user\desktop\test.txt D:\user\desktop\test
	//************************************
	static bool OperatorFileOrFolder(CMMString strSrc, CMMString strDest, int nOperator = FO_COPY, bool bUI = false, bool bMultiFile = false, bool bSameDir = false);

	//************************************
	// Description:	get user save filename 
	// Parameter: 	HWND hWndParent
	// Parameter: 	std::unordered_map<CMMString, CMMString> mapFilter 例：	L"png", L"*.png"
	//																		L"all", L"*.jpg;*.jpeg;*.bmp;*.png;*.mp4"
	//************************************
	static bool OperatorSaveToFile(HWND hWndParent, std::unordered_map<CMMString, CMMString> mapFilter, OUT CMMString &strFileSave);

	//************************************
	// Description:	select file as filter ext 
	// Parameter: 	HWND hWndParent
	// Parameter: 	std::unordered_map<CMMString, CMMString> mapFilter 例：	L"png", L"*.png"
	//																		L"all", L"*.jpg;*.jpeg;*.bmp;*.png;*.mp4"
	//************************************
	static bool OperatorSelectFile(HWND hWndParent, std::unordered_map<CMMString, CMMString> mapFilter, OUT std::vector<CMMString> &vecFileSelect);

	//************************************
	// Description:	select folder
	// Parameter: 	HWND hWndParent															
	//************************************
	static bool OperatorSelectFolder(HWND hWndParent, OUT CMMString &strFolderSelect);
#endif
};

//////////////////////////////////////////////////////////////////////////
#endif

