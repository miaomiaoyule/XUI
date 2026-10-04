#ifndef __MM_STRING_H__
#define __MM_STRING_H__

#pragma once

////////////////////////////////////////////////////////////////////////////
inline std::string WStringToUtf8(const std::wstring& ws)
{
	std::string out;
	out.reserve(ws.size() * 3);
	for (size_t i = 0; i < ws.size(); ++i)
	{
		unsigned int cp = (unsigned int)ws[i];
		// UTF-16 surrogate pair (Windows wchar_t)
		if (sizeof(wchar_t) == 2 && cp >= 0xD800 && cp <= 0xDBFF && i + 1 < ws.size())
		{
			const unsigned int low = (unsigned int)ws[i + 1];
			if (low >= 0xDC00 && low <= 0xDFFF)
			{
				cp = 0x10000 + (((cp - 0xD800) << 10) | (low - 0xDC00));
				++i;
			}
		}
		if (cp <= 0x7F)
		{
			out.push_back((char)cp);
		}
		else if (cp <= 0x7FF)
		{
			out.push_back((char)(0xC0 | (cp >> 6)));
			out.push_back((char)(0x80 | (cp & 0x3F)));
		}
		else if (cp <= 0xFFFF)
		{
			out.push_back((char)(0xE0 | (cp >> 12)));
			out.push_back((char)(0x80 | ((cp >> 6) & 0x3F)));
			out.push_back((char)(0x80 | (cp & 0x3F)));
		}
		else
		{
			out.push_back((char)(0xF0 | (cp >> 18)));
			out.push_back((char)(0x80 | ((cp >> 12) & 0x3F)));
			out.push_back((char)(0x80 | ((cp >> 6) & 0x3F)));
			out.push_back((char)(0x80 | (cp & 0x3F)));
		}
	}

	return out;
}

inline std::wstring Utf8ToWString(const std::string& str)
{
	std::wstring out;
	out.reserve(str.size());
	const unsigned char *p = (const unsigned char *)str.data();
	const unsigned char *end = p + str.size();
	while (p < end)
	{
		unsigned int cp = 0;
		const unsigned char c = *p++;
		if (c < 0x80)
		{
			cp = c;
		}
		else if ((c & 0xE0) == 0xC0 && p < end)
		{
			cp = ((c & 0x1F) << 6) | (*p++ & 0x3F);
		}
		else if ((c & 0xF0) == 0xE0 && p + 1 < end)
		{
			cp = ((c & 0x0F) << 12) | ((p[0] & 0x3F) << 6) | (p[1] & 0x3F);
			p += 2;
		}
		else if ((c & 0xF8) == 0xF0 && p + 2 < end)
		{
			cp = ((c & 0x07) << 18) | ((p[0] & 0x3F) << 12) | ((p[1] & 0x3F) << 6) | (p[2] & 0x3F);
			p += 3;
		}
		else
		{
			continue; // skip invalid byte
		}

		if (sizeof(wchar_t) == 2 && cp > 0xFFFF)
		{
			cp -= 0x10000;
			out.push_back((wchar_t)(0xD800 + (cp >> 10)));
			out.push_back((wchar_t)(0xDC00 + (cp & 0x3FF)));
		}
		else
		{
			out.push_back((wchar_t)cp);
		}
	}

	return out;
}

inline std::string WStringToGbk(const std::wstring &src)
{
	if (src.empty()) return "";

#ifdef DuiPlatform_SDL
	char *pszGbk = SDL_iconv_string("GBK", "WCHAR_T",
		(const char *)src.c_str(), (src.size() + 1) * sizeof(wchar_t));
	if (NULL == pszGbk) return "";
	std::string strOut(pszGbk);
	SDL_free(pszGbk);
	return strOut;
#else
	return CT2CA(src.c_str());
#endif
}

inline std::wstring GbkToWString(const std::string &str)
{
	if (str.empty()) return std::wstring();

#ifdef DuiPlatform_SDL
	char *pszWide = SDL_iconv_string("WCHAR_T", "GBK", str.c_str(), str.size() + 1);
	if (NULL == pszWide) return std::wstring();
	std::wstring strOut((const wchar_t *)pszWide);
	SDL_free(pszWide);
	return strOut;
#else
	return (LPCWSTR)CA2CT(str.c_str());
#endif
}

inline std::wstring AnsiToWString(const std::string &str)
{
	if (str.empty()) return std::wstring();

#ifdef DuiPlatform_SDL
	// "" = current locale encoding (system ANSI / local charset)
	char *pszWide = SDL_iconv_string("WCHAR_T", "", str.c_str(), str.size() + 1);
	if (NULL == pszWide) return std::wstring();
	std::wstring strOut((const wchar_t *)pszWide);
	SDL_free(pszWide);
	return strOut;
#else
	return (LPCWSTR)CA2CT(str.c_str());
#endif
}

#if defined(DuiPlatform_SDL) && (defined(UNICODE) || defined(_UNICODE)) && !defined(_MSC_VER)
// glibc/Bionic vswprintf only: %s is char*. MSVC _vsnwprintf keeps %s as wchar_t*
// and must NOT go through this adapter (Windows / future MSVC-Android).
inline std::wstring MMAdaptMsvcWidePrintfFormat(const wchar_t *pszFmt)
{
	std::wstring strOut;
	if (NULL == pszFmt) return strOut;
	strOut.reserve(wcslen(pszFmt) + 16);
	const wchar_t *p = pszFmt;
	while (*p)
	{
		if (L'%' != *p)
		{
			strOut.push_back(*p++);
			continue;
		}
		strOut.push_back(*p++);
		if (0 == *p) break;
		if (L'%' == *p)
		{
			strOut.push_back(*p++);
			continue;
		}
		while (*p && wcschr(L"-+ #0", *p)) strOut.push_back(*p++);
		if (L'*' == *p) strOut.push_back(*p++);
		else while (*p >= L'0' && *p <= L'9') strOut.push_back(*p++);
		if (L'.' == *p)
		{
			strOut.push_back(*p++);
			if (L'*' == *p) strOut.push_back(*p++);
			else while (*p >= L'0' && *p <= L'9') strOut.push_back(*p++);
		}
		bool bLong = false;
		bool bShort = false;
		if (L'l' == *p)
		{
			bLong = true;
			strOut.push_back(*p++);
			if (L'l' == *p) strOut.push_back(*p++);
		}
		else if (L'h' == *p)
		{
			bShort = true;
			strOut.push_back(*p++);
			if (L'h' == *p) strOut.push_back(*p++);
		}
		else if (L'w' == *p || L'L' == *p || L'z' == *p || L't' == *p || L'j' == *p)
		{
			if (L'w' == *p) bLong = true;
			strOut.push_back(*p++);
		}
		else if (L'I' == *p)
		{
			strOut.push_back(*p++);
			if ((L'6' == *p && p[1] == L'4') || (L'3' == *p && p[1] == L'2'))
			{
				strOut.push_back(*p++);
				strOut.push_back(*p++);
			}
		}
		if (0 == *p) break;
		if (L's' == *p && false == bLong && false == bShort)
		{
			strOut.append(L"ls");
			++p;
			continue;
		}
		if (L'S' == *p && false == bLong && false == bShort)
		{
			strOut.push_back(L's');
			++p;
			continue;
		}
		strOut.push_back(*p++);
	}
	return strOut;
}
#endif

////////////////////////////////////////////////////////////////////////////
class CMMStringA : public std::string
{
public:
	MMDeclare_Super(std::string)
	CMMStringA()
	{
	}
	CMMStringA(const std::string &strSrc)
		: std::string(strSrc)
	{

	}
	CMMStringA(LPCTSTR lpszStr)
#if defined(DuiPlatform_SDL)
		// SDL narrow strings are UTF-8
		: std::string(WStringToUtf8(NULL == lpszStr ? L"" : lpszStr))
#else
		: std::string(WStringToGbk(lpszStr))
#endif
	{

	}
	CMMStringA(LPCSTR lpszStr)
		: std::string(lpszStr)
	{

	}
	CMMStringA(LPCSTR lpszStr, int nLen)
		: std::string(lpszStr, nLen)
	{

	}
	operator LPCSTR() const
	{
		return c_str();
	}
	operator LPSTR() const
	{
		return (LPSTR)c_str();
	}
	void SetAt(int nPos, CHAR ch)
	{
		if (nPos < 0 || nPos >= length()) return;

		operator[](nPos) = ch;

		return;
	}
	void Insert(int nPos, LPCSTR lpszStr)
	{
		if (nPos < 0 || nPos >= length()) return;

		insert(nPos, lpszStr);

		return;
	}
	int GetLength()
	{
		return length();
	}
	friend CMMStringA operator + (LPCTSTR lpszLeft, const CMMStringA &strRight)
	{
#if defined(DuiPlatform_SDL)
		CMMStringA strTemp(WStringToUtf8(NULL == lpszLeft ? L"" : lpszLeft));
#else
		CMMStringA strTemp(WStringToGbk(lpszLeft));
#endif
		strTemp += strRight;

		return strTemp;
	}
};

////////////////////////////////////////////////////////////////////////////
class CMMStringW : public std::wstring
{
public:
	MMDeclare_Super(std::wstring)
	CMMStringW()
	{

	}
#if defined(DuiPlatform_SDL)
	// XML / SDL narrow text is UTF-8 (not GBK). GbkToWString via SDL_iconv often fails → empty.
	CMMStringW(LPCSTR lpszStr)
		: std::wstring(Utf8ToWString(NULL == lpszStr ? ("") : lpszStr))
	{

	}
	CMMStringW(LPCSTR lpszStr, int nLen)
		: std::wstring(Utf8ToWString(std::string(NULL == lpszStr ? ("") : lpszStr, nLen < 0 ? strlen(lpszStr) : nLen)))
	{

	}
#else
	CMMStringW(LPCSTR lpszStr)
		: std::wstring((LPCTSTR)CA2CT(NULL == lpszStr ? ("") : lpszStr))
	{

	}
	CMMStringW(LPCSTR lpszStr, int nLen)
		: std::wstring((LPCTSTR)CA2CT(std::string(NULL == lpszStr ? ("") : lpszStr, nLen < 0 ? strlen(lpszStr) : nLen).c_str()))
	{

	}
#endif
	CMMStringW(CHAR ch, int nCount)
		: CMMStringW((TCHAR)ch, nCount)
	{

	}
	CMMStringW(TCHAR ch)
		: std::wstring(1, ch)
	{

	}
	CMMStringW(TCHAR ch, int nCount)
		: std::wstring(nCount, ch)
	{

	}
	CMMStringW(LPCTSTR lpszStr)
		: std::wstring(NULL == lpszStr ? _T("") : lpszStr)
	{

	}
	CMMStringW(LPCTSTR lpszStr, int nLen)
		: std::wstring(NULL == lpszStr ? _T("") : lpszStr, nLen < 0 ? lstrlen(lpszStr) : nLen)
	{

	}
	CMMStringW(const CMMStringW &strSrc)
		: std::wstring(strSrc)
	{

	}
	CMMStringW(const std::wstring &strSrc)
		: std::wstring(strSrc)
	{

	}
#ifndef DuiPlatform_SDL
	CMMStringW(CString &strSrc)
		: std::wstring(strSrc)
	{

	}
#endif
	CMMStringW Mid(int nFrom) const
	{
		return length() > nFrom ? c_str() + nFrom : _T("");
	}
	CMMStringW Mid(int nFrom, int nCount) const
	{
		return length() > nFrom ? CMMStringW(c_str() + nFrom, nCount) : CMMStringW(_T(""));
	}
	CMMStringW Left(int nCount) const
	{
		return CMMStringW(c_str(), min(length(), nCount));
	}
	CMMStringW Right(int nCount) const
	{
		return (c_str() + max(0, (int)length() - nCount));
	}
	CMMStringW & MakeLower()
	{
		std::transform(begin(), end(), begin(),
			[](wchar_t ch) { return static_cast<wchar_t>(towlower(ch)); });

		return *this;
	}
	CMMStringW & Trim(TCHAR ch = _T(' '))
	{
		while (length() > 0 && (front() == ch || (_T(' ') == ch && (_T('\t') == front() || _T('\n') == front() || _T('\r') == front()))))
		{
			erase(begin());
		}
		while (length() > 0 && (back() == ch || (_T(' ') == ch && (_T('\t') == back() || _T('\n') == back() || _T('\r') == back()))))
		{
			erase(--rbegin().base());
		}

		return *this;
	}
	CMMStringW & Trim(LPCTSTR pszTargets)
	{
		return TrimLeft(pszTargets).TrimRight(pszTargets);
	}
	CMMStringW & TrimLeft(LPCTSTR pszTargets)
	{
		int nLen = lstrlen(pszTargets);
		while (Left(nLen) == pszTargets)
		{
			erase(begin(), begin() + nLen);
		}

		return(*this);
	}
	CMMStringW & TrimRight(TCHAR ch)
	{
		return TrimRight(CMMStringW(ch).c_str());
	}
	CMMStringW & TrimRight(LPCTSTR pszTargets)
	{
		int nLen = lstrlen(pszTargets);
		while (Right(nLen) == pszTargets)
		{
			erase((rbegin() + nLen).base(), rbegin().base());
		}

		return(*this);
	}
	CMMStringW & Replace(TCHAR chSrc, TCHAR chDest)
	{
		std::replace(begin(), end(), chSrc, chDest);

		return *this;
	}
	CMMStringW & Replace(LPCTSTR lpszSrc, LPCTSTR lpszDest)
	{
		if (lstrlen(lpszSrc) <= 0) return *this;

		CMMStringW strTemp;
		int nPosFrom = 0;
		int nPosFind = find(lpszSrc);
		while (-1 != nPosFind)
		{
			strTemp += Mid(nPosFrom, nPosFind - nPosFrom);
			strTemp += lpszDest;
			nPosFrom = nPosFind + lstrlen(lpszSrc);
			nPosFind = find(lpszSrc, nPosFrom);
		}
		if (false == strTemp.empty())
		{
			if (nPosFrom < length())
			{
				strTemp += GetBuffer(nPosFrom);
			}

			*this = strTemp;
		}

		return *this;
	}
	CMMStringW & Format(LPCTSTR pstrFormat, va_list Args)
	{
		if (NULL == pstrFormat) return *this;

#if defined(_MSC_VER) && (_MSC_VER <= 1400)
		TCHAR *szBuffer = NULL;
		int size = 512, nLen, counts;
		szBuffer = (TCHAR *)malloc(size);
		ZeroMemory(szBuffer, size);
		while (TRUE)
		{
			counts = size / sizeof(TCHAR);
			nLen = _vsntprintf(szBuffer, counts, pstrFormat, Args);
			if (nLen != -1 && nLen < counts)
			{
				break;
			}
			if (nLen == -1)
			{
				size *= 2;
			}
			else
			{
				size += 1 * sizeof(TCHAR);
			}

			if ((szBuffer = (TCHAR *)realloc(szBuffer, size)) != NULL)
			{
				ZeroMemory(szBuffer, size);
			}
			else
			{
				break;
			}
		}

		Assign(szBuffer);
		free(szBuffer);
#elif defined(DuiPlatform_SDL)
		// Non-MSVC: no _vsntprintf; vswprintf often cannot query with NULL/0 — grow buffer
		size_t capacity = 256;
		TCHAR *szBuffer = NULL;
		for (;;)
		{
			TCHAR *pNew = (TCHAR *)realloc(szBuffer, capacity * sizeof(TCHAR));
			if (NULL == pNew)
			{
				free(szBuffer);
				clear();
				return *this;
			}
			szBuffer = pNew;
			ZeroMemory(szBuffer, capacity * sizeof(TCHAR));

			va_list argsCopy;
			va_copy(argsCopy, Args);
#ifdef UNICODE
#if defined(_MSC_VER)
			int nLen = _vsnwprintf(szBuffer, capacity, pstrFormat, argsCopy);
#else
			const std::wstring strFmt = MMAdaptMsvcWidePrintfFormat(pstrFormat);
			int nLen = vswprintf(szBuffer, capacity, strFmt.c_str(), argsCopy);
#endif
#else
			int nLen = vsnprintf(szBuffer, capacity, pstrFormat, argsCopy);
#endif
			va_end(argsCopy);

#ifdef UNICODE
			if (nLen >= 0 && (size_t)nLen < capacity)
			{
				assign(szBuffer, (size_t)nLen);
				free(szBuffer);
				return *this;
			}
			if (capacity >= (size_t)1024 * 1024)
			{
				free(szBuffer);
				clear();
				return *this;
			}
			capacity *= 2;
#else
			if (nLen < 0)
			{
				free(szBuffer);
				clear();
				return *this;
			}
			if ((size_t)nLen >= capacity)
			{
				capacity = (size_t)nLen + 1;
				continue;
			}
			assign(szBuffer, (size_t)nLen);
			free(szBuffer);
			return *this;
#endif
		}
#else
		// MSVC: _vsntprintf(NULL,0,...) can query required length
		va_list argsLen;
		va_copy(argsLen, Args);
		int nLen = _vsntprintf(NULL, 0, pstrFormat, argsLen);
		va_end(argsLen);
		if (nLen < 0)
		{
			clear();
			return *this;
		}
		int nSize = (nLen + 1) * sizeof(TCHAR);
		TCHAR *szBuffer = (TCHAR *)malloc(nSize);
		if (NULL == szBuffer)
		{
			clear();
			return *this;
		}
		ZeroMemory(szBuffer, nSize);
		_vsntprintf(szBuffer, nLen + 1, pstrFormat, Args);
		operator = (szBuffer);
		free(szBuffer);
#endif

		return *this;
	}
	CMMStringW & Format(LPCTSTR pstrFormat, ...)
	{
		va_list Args;

		va_start(Args, pstrFormat);
		Format(pstrFormat, Args);
		va_end(Args);

		return *this;
	}
	CMMStringW & AppendFormat(LPCTSTR pstrFormat, ...)
	{
		va_list Args;

		va_start(Args, pstrFormat);
		CMMStringW strTemp;
		strTemp.Format(pstrFormat, Args);
		operator += (strTemp);
		va_end(Args);

		return *this;
	}
	bool empty() const
	{
		return (length() > 0 && _T('\0') == at(0)) || __super::empty();
	}
	void SetAt(int nPos, TCHAR ch)
	{
		if (nPos < 0 || nPos >= length()) return;

		operator[](nPos) = ch;

		return;
	}
	int find(LPCTSTR lpszFind, int nPosOffset = 0) const
	{
		return (int)__super::find(lpszFind, nPosOffset);
	}
	int find(TCHAR ch, int nPosOffset = 0) const
	{
		return (int)__super::find(ch, nPosOffset);
	}
	int rfind(LPCTSTR lpszFind, int nPosOffset = -1) const
	{
		return (int)__super::rfind(lpszFind, nPosOffset);
	}
	int rfind(TCHAR ch, int nPosOffset = -1) const
	{
		return (int)__super::rfind(ch, nPosOffset);
	}
	int CompareNoCase(LPCTSTR lpszRight)
	{
		CMMStringW strThis = *this;
		CMMStringW strRight = lpszRight;
		strThis.MakeLower();
		strRight.MakeLower();

		return strThis.compare(strRight);
	}
	TCHAR & operator[](int nPos)
	{
		return __super::operator[](nPos);
	}
	CMMStringW & operator = (LPCSTR lpszRight)
	{
		if (NULL == lpszRight) return *this;

#if defined(DuiPlatform_SDL)
		__super::operator = (Utf8ToWString(lpszRight));
#else
		__super::operator = ((LPCTSTR)CA2CT(lpszRight));
#endif
		return *this;
	}
	CMMStringW & operator = (LPCTSTR lpszRight)
	{
		if (NULL == lpszRight) return *this;

		__super::operator = (lpszRight);

		return *this;
	}
	LPTSTR GetBuffer(int nFrom = 0)
	{
		return (LPTSTR)(c_str() + max(0, min(nFrom, length())));
	}
	LPTSTR GetBufferSetLength(int nLen)
	{
		resize(nLen);
		operator[](nLen - 1) = '\0';

		return GetBuffer();
	}
	operator LPCTSTR() const
	{
		return c_str();
	}
	friend CMMStringW operator + (const CMMStringW &strLeft, TCHAR ch)
	{
		CMMStringW strTemp = strLeft;
		strTemp.operator += (ch);

		return strTemp;
	}
	friend CMMStringW operator + (const CMMStringW &strLeft, CHAR ch)
	{
		CMMStringW strTemp = strLeft;
		strTemp.operator += (ch);

		return strTemp;
	}
	friend CMMStringW operator + (TCHAR ch, const CMMStringW &strRight)
	{
		CMMStringW strTemp(ch);
		strTemp += strRight;

		return strTemp;
	}
	friend CMMStringW operator + (const CMMStringW &strLeft, LPCTSTR lpszRight)
	{
		CMMStringW strTemp = strLeft;
		strTemp += lpszRight;
		return strTemp;
	}
	friend CMMStringW operator + (const CMMStringW &strLeft, const CMMStringW &strRight)
	{
		CMMStringW strTemp = strLeft;
		strTemp += strRight;
		return strTemp;
	}
	friend CMMStringW operator + (LPCTSTR lpszLeft, const CMMStringW &strRight)
	{
		CMMStringW strLeft = lpszLeft;
		strLeft += strRight;
		return strLeft;
	}
	friend bool operator == (const CMMStringW &strLeft, const CMMStringW &strRight)
	{
		return 0 == strLeft.compare(strRight);
	}
	friend bool operator == (const CMMStringW &strLeft, LPCTSTR lpszRight)
	{
		return 0 == strLeft.compare(lpszRight);
	}
	friend bool operator == (LPCTSTR lpszLeft, const CMMStringW &strRight)
	{
		return 0 == strRight.compare(lpszLeft);
	}
	friend bool operator != (LPCTSTR lpszLeft, const CMMStringW &strRight)
	{
		return 0 != strRight.compare(lpszLeft);
	}
	friend bool operator != (const CMMStringW &strLeft, const CMMStringW &strRight)
	{
		return 0 != strLeft.compare(strRight);
	}
	friend bool operator != (const CMMStringW &strLeft, LPCTSTR lpszRight)
	{
		return 0 != strLeft.compare(lpszRight);
	}
};

//////////////////////////////////////////////////////////////////////////
#if defined(DuiPlatform_SDL)
// 跨平台统一宽字符（或改成统一 UTF-8 的 CMMStringA）
using CMMString = CMMStringW;
#else
#if defined(UNICODE) || defined(_UNICODE)
using CMMString = CMMStringW;
#else
using CMMString = CMMStringA;
#endif
#endif

#if defined(DuiPlatform_SDL)
inline std::string MMStringToUtf8(const std::wstring &ws) { return WStringToUtf8(ws); }
inline std::wstring Utf8ToMMString(const std::string &str) { return Utf8ToWString(str); }

inline CMMString CA2CT(const std::string &s, int nCodePage = CP_UTF8)
{
	if (s.empty()) return {};
	if (CP_ACP == nCodePage) return GbkToWString(s);
	return Utf8ToWString(s);
}

inline CMMStringA CT2CA(const CMMString &s, int nCodePage = CP_UTF8)
{
	if (s.empty()) return CMMStringA();
	if (CP_ACP == nCodePage) return WStringToGbk(s);
	return WStringToUtf8(s);
}
#endif

//////////////////////////////////////////////////////////////////////////

#endif
