#include "stdafx.h"
#include "MMFile.h"

//////////////////////////////////////////////////////////////////////////
#define FILEKIND(ext, type) mapFileType[ext] = type

const BYTE g_HeaderPng[] = { 0x89,0x50,0x4E,0x47 };
const BYTE g_HeaderBmp[] = { 0x42,0x4D };
const BYTE g_HeaderJpg[] = { 0xFF,0xD8,0xFF,0xE0,0x00,0x10,0x4A,0x46,0x49,0x46 };
const BYTE g_HeaderXML[] = { 0x3C, 0x3F, 0x78, 0x6D, 0x6C };
const BYTE g_HeaderGif[] = { 0x47,0x49,0x46 };
const BYTE g_HeaderWave[] = { 0x52, 0x49, 0x46, 0x46 };
const BYTE g_HeaderMp3[] = { 0x49, 0x44, 0x33, 0x03 };
const BYTE g_HeaderMp3Stream[] = { 0xFF, 0xFA, 0x95, 0x6C };
const BYTE g_HeaderFlash[] = { 0x43, 0x57, 0x53 };

struct FileTypeMap
{
	std::map<CMMString, enMMFileType> mapFileType;

	FileTypeMap()
	{
		FILEKIND(_T("doc"), FileType_Doc);
		FILEKIND(_T("docx"), FileType_Doc);
		FILEKIND(_T("dot"), FileType_Doc);
		FILEKIND(_T("dotm"), FileType_Doc);
		FILEKIND(_T("pdf"), FileType_Doc);
		FILEKIND(_T("xps"), FileType_Doc);
		FILEKIND(_T("htm"), FileType_Doc);
		FILEKIND(_T("html"), FileType_Doc);
		FILEKIND(_T("mht"), FileType_Doc);
		FILEKIND(_T("mhtml"), FileType_Doc);
		FILEKIND(_T("xml"), FileType_Doc);
		FILEKIND(_T("txt"), FileType_Doc);
		FILEKIND(_T("rtf"), FileType_Doc);
		FILEKIND(_T("wtf"), FileType_Doc);
		FILEKIND(_T("odt"), FileType_Doc);
		FILEKIND(_T("xls"), FileType_Doc);
		FILEKIND(_T("xlsx"), FileType_Doc);
		FILEKIND(_T("xlsb"), FileType_Doc);
		FILEKIND(_T("xltx"), FileType_Doc);
		FILEKIND(_T("xltm"), FileType_Doc);
		FILEKIND(_T("xlt"), FileType_Doc);
		FILEKIND(_T("csv"), FileType_Doc);
		FILEKIND(_T("prn"), FileType_Doc);
		FILEKIND(_T("dif"), FileType_Doc);
		FILEKIND(_T("slk"), FileType_Doc);
		FILEKIND(_T("xlam"), FileType_Doc);
		FILEKIND(_T("xla"), FileType_Doc);
		FILEKIND(_T("ods"), FileType_Doc);
		FILEKIND(_T("ppt"), FileType_Doc);
		FILEKIND(_T("pptx"), FileType_Doc);
		FILEKIND(_T("pptm"), FileType_Doc);
		FILEKIND(_T("pot"), FileType_Doc);
		FILEKIND(_T("potm"), FileType_Doc);
		FILEKIND(_T("potx"), FileType_Doc);
		FILEKIND(_T("ppsx"), FileType_Doc);
		FILEKIND(_T("pps"), FileType_Doc);
		FILEKIND(_T("ppsm"), FileType_Doc);
		FILEKIND(_T("ppam"), FileType_Doc);
		FILEKIND(_T("thmx"), FileType_Doc);
		FILEKIND(_T("ppa"), FileType_Doc);
		FILEKIND(_T("xml"), FileType_Doc);
		FILEKIND(_T("xmsl"), FileType_Doc);
		FILEKIND(_T("xlsm"), FileType_Doc);
		FILEKIND(_T("wps"), FileType_Doc);
		FILEKIND(_T("config"), FileType_Doc);

		//图片
		FILEKIND(_T("bmp"), FileType_Image);
		FILEKIND(_T("jpg"), FileType_Image);
		FILEKIND(_T("jpeg"), FileType_Image);
		FILEKIND(_T("png"), FileType_Image);
		FILEKIND(_T("gif"), FileType_Image);
		FILEKIND(_T("tif"), FileType_Image);
		FILEKIND(_T("tiff"), FileType_Image);
		FILEKIND(_T("dib"), FileType_Image);
		FILEKIND(_T("psd"), FileType_Image);
		FILEKIND(_T("eps"), FileType_Image);
		FILEKIND(_T("raw"), FileType_Image);
		FILEKIND(_T("pxr"), FileType_Image);
		FILEKIND(_T("mac"), FileType_Image);
		FILEKIND(_T("tga"), FileType_Image);
		FILEKIND(_T("vst"), FileType_Image);
		FILEKIND(_T("pcd"), FileType_Image);
		FILEKIND(_T("pct"), FileType_Image);
		FILEKIND(_T("ai"), FileType_Image);
		FILEKIND(_T("fpx"), FileType_Image);
		FILEKIND(_T("cal"), FileType_Image);
		FILEKIND(_T("img"), FileType_Image);
		FILEKIND(_T("wi"), FileType_Image);
		FILEKIND(_T("eps"), FileType_Image);
		FILEKIND(_T("ico"), FileType_Image);
		FILEKIND(_T("cr2"), FileType_Image);
		FILEKIND(_T("crw"), FileType_Image);
		FILEKIND(_T("cur"), FileType_Image);
		FILEKIND(_T("ani"), FileType_Image);
		FILEKIND(_T("psb"), FileType_Image);
		FILEKIND(_T("sai"), FileType_Image);
		FILEKIND(_T("svg"), FileType_Image);
		FILEKIND(_T("svgz"), FileType_Image);
		
		//movie
		FILEKIND("drc", FileType_Movie);
		FILEKIND("dsm", FileType_Movie);
		FILEKIND("dsv", FileType_Movie);
		FILEKIND("dsa", FileType_Movie);
		FILEKIND("dss", FileType_Movie);
		FILEKIND("vob", FileType_Movie);
		FILEKIND("ifo", FileType_Movie);
		FILEKIND("d2v", FileType_Movie);
		FILEKIND("flv", FileType_Movie);
		FILEKIND("fli", FileType_Movie);
		FILEKIND("flc", FileType_Movie);
		FILEKIND("flic", FileType_Movie);
		FILEKIND("ivf", FileType_Movie);
		FILEKIND("mkv", FileType_Movie);
		FILEKIND("mpg", FileType_Movie);
		FILEKIND("mpeg", FileType_Movie);
		FILEKIND("mpe", FileType_Movie);
		FILEKIND("m1v", FileType_Movie);
		FILEKIND("m2v", FileType_Movie);
		FILEKIND("mpv2", FileType_Movie);
		FILEKIND("mp2v", FileType_Movie);
		FILEKIND("dat", FileType_Movie);
		FILEKIND("ts", FileType_Movie);
		FILEKIND("tp", FileType_Movie);
		FILEKIND("tpr", FileType_Movie);
		FILEKIND("pva", FileType_Movie);
		FILEKIND("pss", FileType_Movie);
		FILEKIND("mp4", FileType_Movie);
		FILEKIND("m4v", FileType_Movie);
		FILEKIND("m4p", FileType_Movie);
		FILEKIND("m4b", FileType_Movie);
		FILEKIND("3gpp", FileType_Movie);
		FILEKIND("3gp", FileType_Movie);
		FILEKIND("3g2", FileType_Movie);
		FILEKIND("3gp2", FileType_Movie);
		FILEKIND("ogm", FileType_Movie);
		FILEKIND("mov", FileType_Movie);
		FILEKIND("qt", FileType_Movie);
		FILEKIND("amr", FileType_Movie);
		FILEKIND("ratdvd", FileType_Movie);
		FILEKIND("rt", FileType_Movie);
		FILEKIND("rp", FileType_Movie);
		FILEKIND("smi", FileType_Movie);
		FILEKIND("smil", FileType_Movie);
		FILEKIND("rm", FileType_Movie);
		FILEKIND("ram", FileType_Movie);
		FILEKIND("rmvb", FileType_Movie);
		FILEKIND("rpm", FileType_Movie);
		FILEKIND("roq", FileType_Movie);
		FILEKIND("swf", FileType_Movie);
		FILEKIND("smk", FileType_Movie);
		FILEKIND("bik", FileType_Movie);
		FILEKIND("wmv", FileType_Movie);
		FILEKIND("wmp", FileType_Movie);
		FILEKIND("wm", FileType_Movie);
		FILEKIND("asf", FileType_Movie);
		FILEKIND("avi", FileType_Movie);
		FILEKIND("asx", FileType_Movie);
		FILEKIND("m3u", FileType_Movie);
		FILEKIND("pls", FileType_Movie);
		FILEKIND("wvx", FileType_Movie);
		FILEKIND("wax", FileType_Movie);
		FILEKIND("wmx", FileType_Movie);
		FILEKIND("mpcpl", FileType_Movie);
		FILEKIND("wma", FileType_Movie);
		FILEKIND("wav", FileType_Movie);
		FILEKIND("ra", FileType_Movie);
		FILEKIND("ogg", FileType_Movie);
		FILEKIND("mpc", FileType_Movie);
		FILEKIND("m4a", FileType_Movie);
		FILEKIND("aac", FileType_Movie);
		FILEKIND("mpa", FileType_Movie);
		FILEKIND("mp2", FileType_Movie);
		FILEKIND("m1a", FileType_Movie);
		FILEKIND("m2a", FileType_Movie);
		FILEKIND("mp3", FileType_Movie);
		FILEKIND("mid", FileType_Movie);
		FILEKIND("midi", FileType_Movie);
		FILEKIND("rmi", FileType_Movie);
		FILEKIND("mka", FileType_Movie);
		FILEKIND("ac3", FileType_Movie);
		FILEKIND("dts", FileType_Movie);
		FILEKIND("cda", FileType_Movie);
		FILEKIND("au", FileType_Movie);
		FILEKIND("snd", FileType_Movie);
		FILEKIND("aif", FileType_Movie);
		FILEKIND("aifc", FileType_Movie);
		FILEKIND("aiff", FileType_Movie);

		// 压缩文件
		FILEKIND(_T("001"), FileType_Zip);
		FILEKIND(_T("7z"), FileType_Zip);
		FILEKIND(_T("a"), FileType_Zip);
		FILEKIND(_T("apm"), FileType_Zip);
		FILEKIND(_T("ar"), FileType_Zip);
		FILEKIND(_T("arj"), FileType_Zip);
		FILEKIND(_T("bz2"), FileType_Zip);
		FILEKIND(_T("bzip2"), FileType_Zip);
		FILEKIND(_T("cab"), FileType_Zip);
		FILEKIND(_T("cpio"), FileType_Zip);
		FILEKIND(_T("cramfs"), FileType_Zip);
		FILEKIND(_T("deb"), FileType_Zip);
		FILEKIND(_T("dmg"), FileType_Zip);
		FILEKIND(_T("epub"), FileType_Zip);
		FILEKIND(_T("esd"), FileType_Zip);
		FILEKIND(_T("ext"), FileType_Zip);
		FILEKIND(_T("ext2"), FileType_Zip);
		FILEKIND(_T("ext3"), FileType_Zip);
		FILEKIND(_T("ext4"), FileType_Zip);
		FILEKIND(_T("gpt"), FileType_Zip);
		FILEKIND(_T("gz"), FileType_Zip);
		FILEKIND(_T("gzip"), FileType_Zip);
		FILEKIND(_T("hfs"), FileType_Zip);
		FILEKIND(_T("hfsx"), FileType_Zip);
		FILEKIND(_T("hxi"), FileType_Zip);
		FILEKIND(_T("hxq"), FileType_Zip);
		FILEKIND(_T("hxr"), FileType_Zip);
		FILEKIND(_T("hxs"), FileType_Zip);
		FILEKIND(_T("hxw"), FileType_Zip);
		FILEKIND(_T("ihex"), FileType_Zip);
		FILEKIND(_T("img"), FileType_Zip);
		FILEKIND(_T("iso"), FileType_Zip);
		FILEKIND(_T("jar"), FileType_Zip);
		FILEKIND(_T("lha"), FileType_Zip);
		FILEKIND(_T("lib"), FileType_Zip);
		FILEKIND(_T("lit"), FileType_Zip);
		FILEKIND(_T("lzh"), FileType_Zip);
		FILEKIND(_T("lzma"), FileType_Zip);
		FILEKIND(_T("lzma86"), FileType_Zip);
		FILEKIND(_T("ova"), FileType_Zip);
		FILEKIND(_T("pkg"), FileType_Zip);
		FILEKIND(_T("pmd"), FileType_Zip);
		FILEKIND(_T("qcow"), FileType_Zip);
		FILEKIND(_T("qcow2"), FileType_Zip);
		FILEKIND(_T("qcow2c"), FileType_Zip);
		FILEKIND(_T("r00"), FileType_Zip);
		FILEKIND(_T("rar"), FileType_Zip);
		FILEKIND(_T("scap"), FileType_Zip);
		FILEKIND(_T("squashfs"), FileType_Zip);
		FILEKIND(_T("swm"), FileType_Zip);
		FILEKIND(_T("tar"), FileType_Zip);
		FILEKIND(_T("taz"), FileType_Zip);
		FILEKIND(_T("tbz"), FileType_Zip);
		FILEKIND(_T("tbz2"), FileType_Zip);
		FILEKIND(_T("txz"), FileType_Zip);
		FILEKIND(_T("uefif"), FileType_Zip);
		FILEKIND(_T("vdi"), FileType_Zip);
		FILEKIND(_T("vmdk"), FileType_Zip);
		FILEKIND(_T("wim"), FileType_Zip);
		FILEKIND(_T("xar"), FileType_Zip);
		FILEKIND(_T("xip"), FileType_Zip);
		FILEKIND(_T("xpi"), FileType_Zip);
		FILEKIND(_T("xz"), FileType_Zip);
		FILEKIND(_T("z"), FileType_Zip);
		FILEKIND(_T("z01"), FileType_Zip);
		FILEKIND(_T("zip"), FileType_Zip);
		FILEKIND(_T("zipx"), FileType_Zip);

		//程序
		FILEKIND(_T("exe"), FileType_Exe);
		FILEKIND(_T("msi"), FileType_Exe);

		//lnk
		FILEKIND(_T("lnk"), FileType_Lnk);
	}

	enMMFileType operator[](LPCTSTR lpszExt)
	{
		auto obj = mapFileType.find(lpszExt);
		return obj == mapFileType.end() ? FileType_File : (obj->second);
	}

	static FileTypeMap & GetInstance()
	{
		static FileTypeMap Obj;
		return Obj;
	}
};

CMMString CMMFile::CombinFile(LPCTSTR lpszPath, LPCTSTR lpszFileName)
{
	CMMString strPath = lpszPath;
	if (strPath.Right(1) != _T('\\'))
	{
		strPath += _T('\\');
	}
	
	return strPath + lpszFileName;
}

enMMFileType CMMFile::ParseFileType(LPCTSTR lpszFile)
{
	if (MMInvalidString(lpszFile)) return FileType_None;
#if defined DuiPlatform_SDL
	SDL_PathInfo info;
	if (SDL_GetPathInfo(CT2CA(lpszFile), &info) && info.type == SDL_PATHTYPE_DIRECTORY) 
	{
		return FileType_Dir;
	}
#else
	if (::PathIsDirectory(lpszFile)) return FileType_Dir;
#endif

	CMMString strFile = lpszFile;

	//ext
	CMMString strName, strExt;
	ParseFileName(strFile, strName, strExt);
	if (strExt.length() > 0)
	{
		return FileTypeMap::GetInstance()[strExt];
	}

	std::vector<BYTE> vecData;
	GetFileData(strFile, vecData);
	if (vecData.empty()) return FileType_None;

	if (memcmp(vecData.data(), g_HeaderPng, sizeof(g_HeaderPng)) == 0) return FileType_Image;
	if (memcmp(vecData.data(), g_HeaderBmp, sizeof(g_HeaderBmp)) == 0) return FileType_Image;
	if (memcmp(vecData.data(), g_HeaderGif, sizeof(g_HeaderGif)) == 0) return FileType_Image;
	if (memcmp(vecData.data(), g_HeaderJpg, sizeof(g_HeaderJpg)) == 0) return FileType_Image;
	if (memcmp(vecData.data(), g_HeaderWave, sizeof(g_HeaderWave)) == 0) return FileType_Movie;
	if (memcmp(vecData.data(), g_HeaderMp3, sizeof(g_HeaderMp3)) == 0) return FileType_Movie;
	if (memcmp(vecData.data(), g_HeaderMp3Stream, sizeof(g_HeaderMp3Stream)) == 0) return FileType_Movie;
	if (memcmp(vecData.data(), g_HeaderFlash, sizeof(g_HeaderFlash)) == 0) return FileType_Movie;
	if (memcmp(vecData.data(), g_HeaderXML, sizeof(g_HeaderXML)) == 0) return FileType_File;

	return FileType_None;
}

enMMImageType CMMFile::ParseImageType(LPCTSTR lpszFile)
{
	if (MMInvalidString(lpszFile)) return ImageType_None;

	std::vector<BYTE> vecData;
	GetFileData(lpszFile, vecData);
	if (vecData.empty()) return ImageType_None;

	return ParseImageType(vecData);
}

enMMImageType CMMFile::ParseImageType(const std::vector<BYTE> &vecData)
{
	if (vecData.size() >= sizeof(g_HeaderPng) && memcmp(vecData.data(), g_HeaderPng, sizeof(g_HeaderPng)) == 0) return ImageType_Png;
	if (vecData.size() >= sizeof(g_HeaderBmp) && memcmp(vecData.data(), g_HeaderBmp, sizeof(g_HeaderBmp)) == 0) return ImageType_Bmp;
	if (vecData.size() >= sizeof(g_HeaderGif) && memcmp(vecData.data(), g_HeaderGif, sizeof(g_HeaderGif)) == 0) return ImageType_Gif;
	if (vecData.size() >= sizeof(g_HeaderJpg) && memcmp(vecData.data(), g_HeaderJpg, sizeof(g_HeaderJpg)) == 0) return ImageType_Jpg;

	return ImageType_None;
}

bool CMMFile::IsWebpFile(LPCTSTR lpszFile)
{
	if (MMInvalidString(lpszFile)) return false;

	std::vector<BYTE> vecData;
	GetFileData(lpszFile, vecData);
	return IsWebpFile(vecData);
}

bool CMMFile::IsWebpFile(const std::vector<BYTE> &vecData)
{
	// WebP 文件头至少需要 12 个字节
	if (vecData.size() < 12)
	{
		return false;
	}

	// 检查前 4 个字节是否为 'RIFF'
	bool bIsRiff = (vecData[0] == 'R' && vecData[1] == 'I' && 
		vecData[2] == 'F' && vecData[3] == 'F');

	// 检查第 8 到 11 字节是否为 'WEBP'
	bool bIsWebp = (vecData[8] == 'W' && vecData[9] == 'E' && 
		vecData[10] == 'B' && vecData[11] == 'P');

	return bIsRiff && bIsWebp;
}

bool CMMFile::IsGifFile(LPCTSTR lpszFile)
{
	if (MMInvalidString(lpszFile)) return false;

	std::vector<BYTE> vecData;
	GetFileData(lpszFile, vecData);
	return IsGifFile(vecData);
}

bool CMMFile::IsGifFile(const std::vector<BYTE> &vecData)
{
	// GIF 文件头至少有 6 个字节 (如 "GIF89a" 或 "GIF87a")
	if (vecData.size() < 6) return false;

	// 前三个字节必须是 'G' 'I' 'F'
	if (vecData[0] != 'G' || vecData[1] != 'I' || vecData[2] != 'F')
		return false;

	// 后三个字节通常是版本号 "87a" 或 "89a"
	bool bIs87a = (vecData[3] == '8' && vecData[4] == '7' && vecData[5] == 'a');
	bool bIs89a = (vecData[3] == '8' && vecData[4] == '9' && vecData[5] == 'a');

	return bIs87a || bIs89a;
}

bool CMMFile::IsSvgFile(LPCTSTR lpszFile)
{
	if (MMInvalidString(lpszFile)) return false;

	std::vector<BYTE> vecData;
	GetFileData(lpszFile, vecData, 512);
	return IsSvgFile(vecData);
}

bool CMMFile::IsSvgFile(const std::vector<BYTE> &vecData)
{
	if (vecData.size() < 4) return false;

	size_t nCheckSize = min((size_t)512, vecData.size());
	auto itEnd = vecData.begin() + nCheckSize;

	// 查找 "<svg"
	const char szSvgLower[] = "<svg";
	auto itLower = std::search(vecData.begin(), itEnd, szSvgLower, szSvgLower + 4);
	if (itLower != itEnd) return true;

	// 查找 "<SVG"
	const char szSvgUpper[] = "<SVG";
	auto itUpper = std::search(vecData.begin(), itEnd, szSvgUpper, szSvgUpper + 4);
	if (itUpper != itEnd) return true;

	return false;
}

bool CMMFile::ParseFileName(LPCTSTR lpszFileName, CMMString &strName, CMMString &strExt)
{
	if (NULL == lpszFileName) return false;

	CMMString strFile = lpszFileName;
	strFile.TrimRight(_T('\\'));
	strFile.TrimRight(_T('/'));
	int nPos = strFile.rfind(_T('\\'));
	nPos = max(strFile.rfind(_T('/')), nPos);

	CMMString strFileName = strFile.Right(strFile.length() - nPos - 1);
	nPos = strFileName.rfind(_T('.'));
	if (nPos == -1)
	{
		strName = strFileName;
		return true;
	}

	strName = strFileName.Left(nPos);
	strExt = strFileName.Right(strFileName.length() - nPos - 1);

	return true;
}

bool CMMFile::ParseFilePathName(LPCTSTR lpszFileFull, CMMString &strPath, CMMString &strName)
{
	strPath.clear();
	strName.clear();

	if (NULL == lpszFileFull) return false;

	// Only strip trailing separators — trimming leading '/' breaks Unix absolute paths.
	CMMString strFile = lpszFileFull;
	strFile.TrimRight(_T('\\'));
	strFile.TrimRight(_T('/'));
#if defined(DuiPlatform_SDL) && !defined(_WIN32)
	strFile.Replace(_T('\\'), _T('/'));
	int nPos = (int)strFile.rfind(_T('/'));
#else
	strFile.Replace(_T('/'), _T('\\'));
	int nPos = (int)strFile.rfind(_T('\\'));
#endif

	//no path
	if (-1 == nPos)
	{
		strName = strFile;
		return true;
	}

	strPath = strFile.Left(nPos + 1);
	strName = strFile.Right(strFile.length() - nPos - 1);

	return true;
}

enMMFileEncode CMMFile::GetFileEncode(FILE *pFile)
{
	if (NULL == pFile) return FileEncode_Null;

	fseek(pFile, 0, SEEK_END);
	int nFileSize = ftell(pFile);
	if (nFileSize <= 0) return FileEncode_Null;

	std::vector<BYTE> vecData;
	vecData.resize(nFileSize);
	fseek(pFile, 0, SEEK_SET);
	int nSizeRead = fread(vecData.data(), 1, nFileSize, pFile);
	if (vecData.size() < 2)
	{
		return CMMStrHelp::IsUTF8Encode(vecData) ? FileEncode_UTF8 : FileEncode_Ansi;
	}

	unsigned char ch = vecData[0];
	int p = ch << 8;
	p += vecData[1];

	switch (p)//判断文本前两个字节
	{
		case 0xfffe:  //65534		
		{
			return FileEncode_Unicode;
		}
		case 0xfeff://65279
		{
			return FileEncode_Unicode_big_endian;
		}
		case 0xefbb://61371
		{
			return FileEncode_UTF8_Bom;
		}
		default:
		{
			return CMMStrHelp::IsUTF8Encode(vecData) ? FileEncode_UTF8 : FileEncode_Ansi;
		}
	}

	return FileEncode_Ansi;
}

enMMFileEncode CMMFile::GetFileEncode(const char *pStr)
{
	if (NULL == pStr) return FileEncode_Null;

	if (strlen(pStr) < 2)
	{
		return CMMStrHelp::IsUTF8Encode(std::vector<BYTE>(pStr, pStr + strlen(pStr))) ? FileEncode_UTF8 : FileEncode_Ansi;
	}

	unsigned char ch = pStr[0];
	int p = ch << 8;
	ch = pStr[1];
	p += ch;

	switch (p)//判断文本前两个字节
	{
		case 0xfffe:  //65534
		{
			return FileEncode_Unicode;
		}
		case 0xfeff://65279
		{
			return FileEncode_Unicode_big_endian;
		}
		case 0xefbb://61371
		{
			return FileEncode_UTF8_Bom;
		}
		default:
		{
			return CMMStrHelp::IsUTF8Encode(std::vector<BYTE>(pStr, pStr + strlen(pStr))) ? FileEncode_UTF8 : FileEncode_Ansi;
		}
	}

	return FileEncode_Ansi;
}

bool CMMFile::GetFileData(IN LPCTSTR lpszFileFull, OUT std::vector<BYTE> &vecData, DWORD dwSizeLimit)
{
	bool bFullPath = true;
	CMMString strFile = lpszFileFull;
#if defined(DuiPlatform_SDL) && !defined(_WIN32)
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

	//path
#if defined(DuiPlatform_SDL)
	if (false == bFullPath)
	{
		strFile = CMMService::GetWorkDirectory() + _T('/') + strFile;
	}

	strFile.Replace(_T('\\'), _T('/'));
	size_t nFileSize = 0;
	void *pData = SDL_LoadFile(CT2CA(strFile).c_str(), &nFileSize);
	if (NULL == pData) return false;

	if (nFileSize > (size_t)dwSizeLimit)
	{
		nFileSize = (size_t)dwSizeLimit;
	}
	if (0 == nFileSize)
	{
		SDL_free(pData);
		return false;
	}

	vecData.assign((BYTE *)pData, (BYTE *)pData + nFileSize);
	SDL_free(pData);
	return true;

	/*FILE *pFile = fopen(CT2CA(strFile), "rb");
	if (NULL == pFile) return false;

	fseek(pFile, 0, SEEK_END);
	int nFileSize = ftell(pFile);
	if (nFileSize <= 0)
	{
		fclose(pFile);
		return false;
	}

	vecData.resize(nFileSize);
	fseek(pFile, 0, SEEK_SET);
	int nSizeRead = fread(vecData.data(), 1, nFileSize, pFile);
	if (nSizeRead != nFileSize)
	{
		assert(false);
		fclose(pFile);
		return false;
	}

	fclose(pFile);

	return true;	*/
#else
	if (false == bFullPath)
	{
		strFile = CMMService::GetWorkDirectory() + _T('\\') + strFile;
	}
	{
		HANDLE hFile = CreateFile(strFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE == hFile) return false;

		DWORD dwFileSize = 0;
		dwFileSize = ::GetFileSize(hFile, NULL);
		if (dwFileSize <= 0 || dwFileSize >= (DWORD)-1)
		{
			CloseHandle(hFile);

			return true;
		}

		dwFileSize = min(dwFileSize, dwSizeLimit);
		vecData.resize(dwFileSize);
		ReadFile(hFile, vecData.data(), dwFileSize, &dwFileSize, NULL);
		CloseHandle(hFile);

		return true;
	}
#endif
}

bool CMMFile::GetFileData(IN LPCTSTR lpszFileFull, OUT CMMString &strData)
{
	std::vector<BYTE> vecData;
	bool bRes = GetFileData(lpszFileFull, vecData);
	if (false == bRes || vecData.empty()) return bRes;

	vecData.push_back('\0');
	LPBYTE pByte = vecData.data();
	enMMFileEncode FileEncode = GetFileEncode((LPSTR)vecData.data());
	switch (FileEncode)
	{
		case FileEncode_Ansi:
		{
			strData = (LPCTSTR)CA2CT((LPCSTR)pByte);

			return true;
		}
		case FileEncode_Unicode:
		{
			if (vecData.size() >= 2)
			{
				pByte += 2;
				strData = (LPCTSTR)pByte;

				return true;
			}

			break;
		}
		case FileEncode_Unicode_big_endian:
		{
			if (vecData.size() >= 2)
			{
				pByte += 2;
				DWORD dwSize = vecData.size() / 2 - 1;
				for (DWORD nSwap = 0; nSwap < dwSize; nSwap++)
				{
					CHAR nTemp = pByte[(nSwap << 1) + 0];
					pByte[(nSwap << 1) + 0] = pByte[(nSwap << 1) + 1];
					pByte[(nSwap << 1) + 1] = nTemp;
				}

				strData = (LPCTSTR)pByte;

				return true;
			}

			break;
		}
		case FileEncode_UTF8_Bom:
		{
			if (vecData.size() >= 3)
			{
				pByte += 3;
				strData = (LPCTSTR)CA2CT((LPCSTR)pByte, CP_UTF8);

				return true;
			}

			break;
		}
		case FileEncode_UTF8:
		{
			strData = (LPCTSTR)CA2CT((LPCSTR)pByte, CP_UTF8);

			return true;
		}
	}

	return false;
}

#ifndef DuiPlatform_SDL
PCIDLIST_ABSOLUTE CMMFile::GetPCIDLFromPath(LPCTSTR lpszFileFull)
{
	PIDLIST_ABSOLUTE pCID = NULL;
	HRESULT hRes = ::SHParseDisplayName(lpszFileFull, NULL, &pCID, NULL, NULL);

	if (false == SUCCEEDED(hRes)) return NULL;

	return pCID;
}

CMMString CMMFile::GetFileTip(IN LPCTSTR lpszFileFull)
{
	CMMString strTip;
	HRESULT hRes = E_FAIL;

	IShellFolder *pIShellFolder = NULL;
	IQueryInfo *pIQueryInfo = NULL;
	PCUITEMID_CHILD pPIDLChild = nullptr;
	PCIDLIST_ABSOLUTE pPCIDL = GetPCIDLFromPath(lpszFileFull);
	if (NULL == pPCIDL) return strTip;

	hRes = SHBindToParent(pPCIDL, IID_IShellFolder, (void**)&pIShellFolder, &pPIDLChild);
	if (SUCCEEDED(hRes) && pIShellFolder)
	{
		hRes = pIShellFolder->GetUIObjectOf(0, 1, &pPIDLChild, IID_IQueryInfo, 0, (void**)&pIQueryInfo);
	}
	if (SUCCEEDED(hRes) && pIQueryInfo)
	{
		LPTSTR info = nullptr;
		hRes = pIQueryInfo->GetInfoTip(QITIPF_USESLOWTIP, &info);
		if (SUCCEEDED(hRes) && info)
		{
			strTip = info;
			CoTaskMemFree(info);
		}

		pIQueryInfo->Release();
	}
	if (pIShellFolder)
	{
		pIShellFolder->Release();
	}

	return strTip;
}

SYSTEMTIME CMMFile::GetFileModifyTime(IN LPCTSTR lpszFileFull)
{
	WIN32_FILE_ATTRIBUTE_DATA FileData = {};
	if (GetFileAttributesEx(lpszFileFull, GetFileExInfoStandard, &FileData))
	{
		FILETIME FileTime = {};
		FileTimeToLocalFileTime(&FileData.ftLastWriteTime, &FileTime);

		SYSTEMTIME tFileTime = {};
		FileTimeToSystemTime(&FileTime, &tFileTime);
		return tFileTime;
	}

	return {};
}

SYSTEMTIME CMMFile::GetFileCreateTime(IN LPCTSTR lpszFileFull)
{
	WIN32_FILE_ATTRIBUTE_DATA FileData = {};
	if (GetFileAttributesEx(lpszFileFull, GetFileExInfoStandard, &FileData))
	{
		FILETIME FileTime = {};
		FileTimeToLocalFileTime(&FileData.ftCreationTime, &FileTime);

		SYSTEMTIME tFileTime = {};
		FileTimeToSystemTime(&FileTime, &tFileTime);
		return tFileTime;
	}

	return {};
}

SYSTEMTIME CMMFile::GetFileLastAccessTime(IN LPCTSTR lpszFileFull)
{
	WIN32_FILE_ATTRIBUTE_DATA FileData = {};
	if (GetFileAttributesEx(lpszFileFull, GetFileExInfoStandard, &FileData))
	{
		FILETIME FileTime = {};
		FileTimeToLocalFileTime(&FileData.ftLastAccessTime, &FileTime);

		SYSTEMTIME tFileTime = {};
		FileTimeToSystemTime(&FileTime, &tFileTime);
		return tFileTime;
	}

	return {};
}

uint64_t CMMFile::GetFileSize(IN LPCTSTR lpszFileFull)
{
	WIN32_FILE_ATTRIBUTE_DATA FileData = {};
	if (GetFileAttributesEx(lpszFileFull, GetFileExInfoStandard, &FileData))
	{
		DWORD buf[2] = {};
		buf[0] = FileData.nFileSizeLow;
		buf[1] = FileData.nFileSizeHigh;
		return *(uint64_t*)&buf;
	}

	return 0;
}

bool CMMFile::GetFileVersion(IN LPCTSTR lpszFileFull, OUT DWORD &dwVersionInfo)
{
	//设置结果
	dwVersionInfo = 0L;

	//接收缓冲
	BYTE cbInfoBuffer[1024];
	ZeroMemory(cbInfoBuffer, sizeof(cbInfoBuffer));

	//模块信息
	DWORD dwFileHandle = NULL;
	if (GetFileVersionInfo(lpszFileFull, dwFileHandle, sizeof(cbInfoBuffer), cbInfoBuffer) == FALSE) return false;

	//获取信息
	UINT uQuerySize = 0;
	VS_FIXEDFILEINFO * pFixedFileInfo = NULL;
	if (VerQueryValue(cbInfoBuffer, _T("\\"), (VOID * *)&pFixedFileInfo, &uQuerySize) == FALSE) return false;

	//设置结果
	if ((pFixedFileInfo != NULL) && (uQuerySize == sizeof(VS_FIXEDFILEINFO)))
	{
		//设置版本
		WORD wVersion1 = HIWORD(pFixedFileInfo->dwFileVersionMS);
		WORD wVersion2 = LOWORD(pFixedFileInfo->dwFileVersionMS);
		WORD wVersion3 = HIWORD(pFixedFileInfo->dwFileVersionLS);
		WORD wVersion4 = LOWORD(pFixedFileInfo->dwFileVersionLS);
		dwVersionInfo = MAKELONG(MAKEWORD(wVersion1, wVersion2), MAKEWORD(wVersion3, wVersion4));

		return true;
	}

	return false;
}

CMMString CMMFile::GetFileVersion(IN LPCTSTR lpszFileFull)
{
	DWORD dwVersion = 0;
	GetFileVersion(lpszFileFull, dwVersion);

	CMMString strVersion;
	strVersion.Format(_T("%d.%d.%d.%d"), ((LPBYTE)&dwVersion)[0], ((LPBYTE)&dwVersion)[1], ((LPBYTE)&dwVersion)[2], ((LPBYTE)&dwVersion)[3]);

	return strVersion;
}

CMMString CMMFile::GetAppVersion()
{
	return GetFileVersion(CMMService::GetAppFile());
}

std::vector<CMMString> CMMFile::GetFolderOfDir(IN LPCTSTR lpszDirFull)
{
	std::vector<CMMString> vecFolder;

	if (false == PathFileExists(lpszDirFull)) return vecFolder;

	CMMString strDir = lpszDirFull;
	if (strDir.Right(1) != _T('/')
		&& strDir.Right(1) != _T('\\'))
	{
		strDir += _T('\\');
	}

	CMMString strPathFind = strDir + _T("*.*");

	WIN32_FIND_DATA FindData = {};
	HANDLE hFindFile = FindFirstFile(strPathFind, &FindData);
	if (INVALID_HANDLE_VALUE == hFindFile) return vecFolder;

	do
	{
		CMMString strFile = FindData.cFileName;
		if (strFile.empty() || _T(".") == strFile || _T("..") == strFile) continue;

		//path
		if (FILE_ATTRIBUTE_DIRECTORY & FindData.dwFileAttributes)
		{
			vecFolder.push_back(strDir + strFile);
		}

	} while (FindNextFile(hFindFile, &FindData));

	FindClose(hFindFile);

	return vecFolder;
}

std::vector<CMMString> CMMFile::GetFileOfDir(IN LPCTSTR lpszDirFull)
{
	std::vector<CMMString> vecFile;

	if (false == PathFileExists(lpszDirFull)) return vecFile;

	CMMString strDir = lpszDirFull;
	if (strDir.Right(1) != _T('/')
		&& strDir.Right(1) != _T('\\'))
	{
		strDir += _T('\\');
	}

	CMMString strPathFind = strDir + _T("*.*");

	WIN32_FIND_DATA FindData = {};
	HANDLE hFindFile = FindFirstFile(strPathFind, &FindData);
	if (INVALID_HANDLE_VALUE == hFindFile) return vecFile;

	do
	{
		CMMString strFile = FindData.cFileName;
		if (strFile.empty() || _T(".") == strFile || _T("..") == strFile) continue;

		//path
		if (0 == (FILE_ATTRIBUTE_DIRECTORY & FindData.dwFileAttributes))
		{
			vecFile.push_back(strDir + strFile);
		}

	} while (FindNextFile(hFindFile, &FindData));

	FindClose(hFindFile);

	return vecFile;
}

std::vector<CMMString> CMMFile::GetFileAndFolderOfDir(IN LPCTSTR lpszDirFull)
{
	std::vector<CMMString> vecFiles;

	if (false == PathFileExists(lpszDirFull)) return vecFiles;

	CMMString strDir = lpszDirFull;
	if (strDir.Right(1) != _T('/')
		&& strDir.Right(1) != _T('\\'))
	{
		strDir += _T('\\');
	}

	CMMString strPathFind = strDir + _T("*.*");

	WIN32_FIND_DATA FindData = {};
	HANDLE hFindFile = FindFirstFile(strPathFind, &FindData);
	if (INVALID_HANDLE_VALUE == hFindFile) return vecFiles;

	do
	{
		CMMString strFile = FindData.cFileName;
		if (strFile.empty() || _T(".") == strFile || _T("..") == strFile) continue;

		vecFiles.push_back(strDir + strFile);

	} while (FindNextFile(hFindFile, &FindData));

	FindClose(hFindFile);

	return vecFiles;
}

uint64_t CMMFile::GetFolderSize(IN LPCTSTR lpszDirFull)
{
	uint64_t uSize = 0;
	auto vecFiles = GetFileAndFolderOfDir(lpszDirFull);
	for (CMMString strFile : vecFiles)
	{
		if (GetFileAttributes(strFile) & FILE_ATTRIBUTE_DIRECTORY)
		{
			uSize += GetFolderSize(strFile);
		}
		else
		{
			uSize += GetFileSize(strFile);
		}
	}

	return uSize;
}

bool CMMFile::WriteFileData(IN LPCTSTR lpszFileFull, IN CMMString &strData, bool bClearOld)
{
	FILE *pFile = fopen(CT2CA(lpszFileFull), "rb");
	if (NULL == pFile) return false;

	enMMFileEncode FileEncoe = GetFileEncode(pFile);
	fclose(pFile);
	pFile = NULL;

	pFile = bClearOld ? fopen(CT2CA(lpszFileFull), "w+") : fopen(CT2CA(lpszFileFull), "at");
	if (NULL == pFile) return false;

	fseek(pFile, 0, SEEK_END);

	switch (FileEncoe)
	{
		case FileEncode_UTF8_Bom:
		{
			CMMStringA strDataA = CT2CA(strData, CP_UTF8);

			if (bClearOld)
			{
				CMMStringA strSign;
				strSign.reserve(3);
				strSign.SetAt(0, 0xEF);
				strSign.SetAt(1, 0xBB);
				strSign.SetAt(2, 0xBF);
				strDataA.Insert(0, strSign);
			}

			fwrite(strDataA, strDataA.GetLength(), 1, pFile);
			
			break;
		}
		case FileEncode_Unicode:
		{
			CMMString strFileData = strData;

			if (bClearOld)
			{
				CMMString strSign;
				strSign.resize(2);
				strSign[0] = 0xFF;
				strSign[1] = 0xFE;
				strFileData.insert(0, strSign);
			}

			fwrite(strFileData, strFileData.length() * sizeof(TCHAR), 1, pFile);

			break;
		}
		case FileEncode_Unicode_big_endian:
		{
			break;
		}
		case FileEncode_UTF8:
		{
			CMMStringA strDataA = CT2CA(strData, CP_UTF8);

			fwrite(strDataA, strDataA.GetLength(), 1, pFile);

			break;
		}
		case FileEncode_Ansi:
		default:
		{
			CMMStringA strDataA = CT2CA(strData);

			fwrite(strDataA, strDataA.GetLength(), 1, pFile);

			break;
		}
	}

	fclose(pFile);
	pFile = NULL;

	return true;
}

bool CMMFile::WriteFileData(IN LPCTSTR lpszFileFull, IN std::vector<BYTE> &vecData, bool bClearOld)
{
	if (vecData.empty()) return false;

	FILE *pFile = bClearOld ? fopen(CT2CA(lpszFileFull), "wb+") : fopen(CT2CA(lpszFileFull), "ab+");
	if (NULL == pFile) return false;

	fseek(pFile, 0, SEEK_END);
	fwrite(vecData.data(), vecData.size(), 1, pFile);

	fclose(pFile);
	pFile = NULL;

	return true;
}

bool CMMFile::WriteFileData(IN LPCTSTR lpszFileFull, IN std::vector<BYTE> &vecData, int nOffsetOfFirst)
{
	if (vecData.empty()) return false;

	FILE *pFile = fopen(CT2CA(lpszFileFull), "ab+");
	if (NULL == pFile) return false;

	fseek(pFile, nOffsetOfFirst, SEEK_SET);
	fwrite(vecData.data(), vecData.size(), 1, pFile);

	fclose(pFile);
	pFile = NULL;

	return true;
}

bool CMMFile::RemoveFileByHasName(CMMString strPath, CMMString strHasName)
{
	if (false == PathFileExists(strPath)) return false;

	if (strPath[strPath.length() - 1] != _T('/')
		&& strPath[strPath.length() - 1] != _T('\\'))
	{
		strPath += _T('\\');
	}

	CMMString strPathFind = strPath + _T("*.*");

	WIN32_FIND_DATA FindData = {};
	HANDLE hFindFile = FindFirstFile(strPathFind, &FindData);
	if (INVALID_HANDLE_VALUE == hFindFile) return false;

	do
	{
		CMMString strFile = FindData.cFileName;
		if (strFile.empty() || _T(".") == strFile || _T("..") == strFile)continue;

		if (-1 != strFile.find(strHasName))
		{
			DeleteFile(strPath + strFile);
		}

		//path
		if (FILE_ATTRIBUTE_DIRECTORY & FindData.dwFileAttributes)
		{
			RemoveFileByHasName(strPath + strFile, strHasName);
		}

	} while (FindNextFile(hFindFile, &FindData));

	FindClose(hFindFile);

	return true;
}

bool CMMFile::ClearFilesOfFolder(CMMString strPath)
{
	if (false == PathFileExists(strPath)) return false;

	if (strPath[strPath.length() - 1] != _T('/')
		&& strPath[strPath.length() - 1] != _T('\\'))
	{
		strPath += _T('\\');
	}

	CMMString strPathFind = strPath + _T("*.*");

	WIN32_FIND_DATA FindData = {};
	HANDLE hFindFile = FindFirstFile(strPathFind, &FindData);
	if (INVALID_HANDLE_VALUE == hFindFile) return false;

	do
	{
		CMMString strFile = FindData.cFileName;
		if (strFile.empty() || _T(".") == strFile || _T("..") == strFile) continue;

		OperatorFileOrFolder(strPath + strFile, _T(""), FO_DELETE);

	} while (FindNextFile(hFindFile, &FindData));

	FindClose(hFindFile);

	return true;
}

bool CMMFile::DeleteFolder(CMMString strPath)
{
	if (false == PathFileExists(strPath) || false == PathIsDirectory(strPath)) return false;

	return OperatorFileOrFolder(strPath, _T(""), FO_DELETE);
}

bool CMMFile::OpenFolderAndSelectFile(CMMString strFileFull)
{
	strFileFull.Replace(_T("/"), _T("\\"));

	PIDLIST_ABSOLUTE pCID = NULL;
	HRESULT hRes = ::SHParseDisplayName(strFileFull, NULL, &pCID, NULL, NULL);
	if (FAILED(hRes)) return false;

	hRes = ::SHOpenFolderAndSelectItems(pCID, 0, NULL, 0); // 第二个参数cidl置为0，表示是选中文件

	return SUCCEEDED(hRes);
}

bool CMMFile::OperatorFileOrFolder(CMMString strSrc, CMMString strDest, int nOperator, bool bUI, bool bMultiFile, bool bSameDir)
{
	strSrc += _T('\0');
	strDest += _T('\0');

	SHFILEOPSTRUCT FileOp = {};
	SecureZeroMemory((void*)&FileOp, sizeof(SHFILEOPSTRUCT));
	//secureZeroMemory和ZeroMerory的区别
	//根据MSDN上，ZeryMerory在当缓冲区的字符串超出生命周期的时候，
	//会被编译器优化，从而缓冲区的内容会被恶意软件捕捉到。
	//引起软件安全问题，特别是对于密码这些比较敏感的信息而说。
	//而SecureZeroMemory则不会引发此问题，保证缓冲区的内容会被正确的清零。
	//如果涉及到比较敏感的内容，尽量使用SecureZeroMemory函数。

	FileOp.fFlags = FOF_ALLOWUNDO;
	FileOp.fFlags |= (bSameDir ? FOF_RENAMEONCOLLISION : 0);
	FileOp.fFlags |= (bMultiFile ? FOF_MULTIDESTFILES : 0);
	FileOp.fFlags |= (bUI ? 0 : (FOF_NOCONFIRMATION | FOF_NO_UI | FOF_NOCONFIRMMKDIR));
	FileOp.hNameMappings = NULL;							//文件映射
	FileOp.hwnd = NULL;										//消息发送的窗口句柄；
	FileOp.lpszProgressTitle = NULL;						//文件操作进度窗口标题 
	FileOp.pFrom = strSrc;									//源文件及路径 
	FileOp.pTo = strDest;									//目标文件及路径 
	FileOp.wFunc = nOperator;								//操作类型 

	int nRes = SHFileOperation(&FileOp);
	return 0 == nRes;
}

bool CMMFile::OperatorSaveToFile(HWND hWndParent, std::unordered_map<CMMString, CMMString> mapFilter, OUT CMMString &strFileSave)
{
	CComPtr<IFileSaveDialog> pIFileSaveDialog = NULL;
	HRESULT hRes = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pIFileSaveDialog));
	if (false == SUCCEEDED(hRes)) return false;

	//filter
	std::vector<COMDLG_FILTERSPEC> vecFilter;
	vecFilter.push_back({ _T("所有文件"), _T("*.*") });
	for (auto &FilterItem : mapFilter)
	{
		vecFilter.push_back({ FilterItem.first, FilterItem.second });
	}

	//show
	pIFileSaveDialog->SetFileTypes(vecFilter.size(), vecFilter.data());
	pIFileSaveDialog->SetFileTypeIndex(0);
	pIFileSaveDialog->Show(hWndParent);

	//res
	CComPtr<IShellItem> pItem = NULL;
	hRes = pIFileSaveDialog->GetResult(&pItem);
	if (false == SUCCEEDED(hRes)) return false;

	LPWSTR szPath = NULL;
	hRes = pItem->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &szPath);
	if (SUCCEEDED(hRes) && szPath)
	{
		strFileSave = szPath;

		CoTaskMemFree(szPath);
	}

	return true;
}

bool CMMFile::OperatorSelectFile(HWND hWndParent, std::unordered_map<CMMString, CMMString> mapFilter, OUT std::vector<CMMString> &vecFileSelect)
{
	CComPtr<IFileOpenDialog> pIFileOpenDialog = NULL;
	HRESULT hRes = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pIFileOpenDialog));
	if (false == SUCCEEDED(hRes)) return false;

	//filter
	std::vector<COMDLG_FILTERSPEC> vecFilter;
	vecFilter.push_back({ _T("所有文件"), _T("*.*") });
	for (auto &FilterItem : mapFilter)
	{
		vecFilter.push_back({ FilterItem.first, FilterItem.second });
	}

	//show
	DWORD dwFlags;
	pIFileOpenDialog->GetOptions(&dwFlags);
	pIFileOpenDialog->SetOptions(dwFlags | FOS_FORCEFILESYSTEM | FOS_ALLOWMULTISELECT);
	pIFileOpenDialog->SetFileTypes(vecFilter.size(), vecFilter.data());
	pIFileOpenDialog->SetFileTypeIndex(0);
	pIFileOpenDialog->Show(hWndParent);

	//res
	CComPtr<IShellItemArray> pItems = NULL;
	hRes = pIFileOpenDialog->GetResults(&pItems);
	if (false == SUCCEEDED(hRes)) return false;
	
	DWORD dwCount = 0;
	if (false == SUCCEEDED(pItems->GetCount(&dwCount))) return false;
	
	//select
	for (int i = 0; i < dwCount; i++)
	{
		CComPtr<IShellItem> pItem = NULL;
		if (SUCCEEDED(pItems->GetItemAt(i, &pItem)))
		{
			//DWORD dwAttrs = 0;
			//if (SUCCEEDED(pItem->GetAttributes(SFGAO_LINK, &dwAttrs)) && (dwAttrs & SFGAO_LINK)) continue;
			
			LPWSTR szPath = NULL;
			hRes = pItem->GetDisplayName(SIGDN_FILESYSPATH, &szPath);
			if (SUCCEEDED(hRes) && szPath)
			{
				vecFileSelect.push_back(szPath);
				
				::CoTaskMemFree(szPath);
			}
		}
	}

	return true;
}

bool CMMFile::OperatorSelectFolder(HWND hWndParent, OUT CMMString &strFolderSelect)
{
	CComPtr<IFileDialog> pIFileDialog = NULL;
	HRESULT hRes = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pIFileDialog));
	if (false == SUCCEEDED(hRes)) return false;

	pIFileDialog->SetOptions(FOS_PICKFOLDERS);
	pIFileDialog->Show(hWndParent);

	CComPtr<IShellItem> pFolderItem = NULL;
	hRes = pIFileDialog->GetResult(&pFolderItem);
	if (false == SUCCEEDED(hRes)) return false;

	LPWSTR szFolderPath = NULL;
	hRes = pFolderItem->GetDisplayName(SIGDN_FILESYSPATH, &szFolderPath);
	if (false == SUCCEEDED(hRes))
	{
		::CoTaskMemFree(szFolderPath);

		return false;
	}

	strFolderSelect = szFolderPath;
	::CoTaskMemFree(szFolderPath);

	return true;
}
#endif