#include "StdAfx.h"
#include "DUIGdiPortable.h"
#include "DUICanvasRaster.h"

#if defined(DuiPlatform_SDL)

//////////////////////////////////////////////////////////////////////////
static DuiGdiBrush g_HollowBrush;
static thread_local HGDIOBJ t_selectedPen = NULL;
static thread_local HGDIOBJ t_selectedBrush = NULL;
static thread_local HFONT t_selectedFont = NULL;
static thread_local DWORD t_textColor = 0xFF000000;

//////////////////////////////////////////////////////////////////////////
BOOL DeleteObject(HGDIOBJ hObject)
{
	if (NULL == hObject) return FALSE;
	if (hObject == &g_HollowBrush) return TRUE;
	IDuiNativeGdi *pNative = (IDuiNativeGdi *)hObject;
	const IDuiNativeGdi::enKind kind = pNative->GetNativeKind();
	if (IDuiNativeGdi::Kind_Region == kind
		|| IDuiNativeGdi::Kind_Pen == kind
		|| IDuiNativeGdi::Kind_Brush == kind
		|| IDuiNativeGdi::Kind_Canvas == kind
		|| IDuiNativeGdi::Kind_Image == kind
		|| IDuiNativeGdi::Kind_Font == kind)
	{
		delete pNative;
		return TRUE;
	}
	return FALSE;
}

BOOL DeleteDC(HDC hdc)
{
	return DeleteObject((HGDIOBJ)hdc);
}

HDC CreateCompatibleDC(HDC hdc)
{
	IDuiCanvas *pSrc = DuiCanvasFromHDC(hdc);
	const int nWidth = pSrc ? max(1, pSrc->GetWidth()) : 1;
	const int nHeight = pSrc ? max(1, pSrc->GetHeight()) : 1;
	return DuiHDCFromCanvas(new CDUICanvasRaster(nWidth, nHeight));
}

HGDIOBJ SelectObject(HDC hdc, HGDIOBJ hgdiobj)
{
	if (NULL == hgdiobj) return NULL;
	IDuiNativeGdi *pObj = (IDuiNativeGdi *)hgdiobj;
	const IDuiNativeGdi::enKind kind = pObj->GetNativeKind();

	if (IDuiNativeGdi::Kind_Image == kind)
	{
		CDUICanvasRaster *pCanvas = dynamic_cast<CDUICanvasRaster *>(DuiCanvasFromHDC(hdc));
		if (pCanvas) pCanvas->SelectBitmap((IDuiImage *)pObj);
		return NULL;
	}
	if (IDuiNativeGdi::Kind_Font == kind)
	{
		HFONT old = t_selectedFont;
		t_selectedFont = (HFONT)hgdiobj;
		return (HGDIOBJ)old;
	}
	if (IDuiNativeGdi::Kind_Pen == kind)
	{
		HGDIOBJ old = t_selectedPen;
		t_selectedPen = hgdiobj;
		return old;
	}
	if (IDuiNativeGdi::Kind_Brush == kind)
	{
		HGDIOBJ old = t_selectedBrush;
		t_selectedBrush = hgdiobj;
		return old;
	}
	return NULL;
}

BOOL BitBlt(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD)
{
	IDuiCanvas *pDst = DuiCanvasFromHDC(hdcDest);
	IDuiCanvas *pSrc = DuiCanvasFromHDC(hdcSrc);
	if (NULL == pDst || NULL == pSrc) return FALSE;
	pDst->BlitFrom(pSrc, nXDest, nYDest, nWidth, nHeight, nXSrc, nYSrc);
	return TRUE;
}

BOOL StretchBlt(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, int wSrc, int hSrc, DWORD)
{
	IDuiCanvas *pDst = DuiCanvasFromHDC(hdcDest);
	IDuiCanvas *pSrc = DuiCanvasFromHDC(hdcSrc);
	if (NULL == pDst || NULL == pSrc) return FALSE;
	pDst->StretchBlitFrom(pSrc, nXDest, nYDest, nWidth, nHeight, nXSrc, nYSrc, wSrc, hSrc);
	return TRUE;
}

DWORD GetObjectType(HGDIOBJ h)
{
	IDuiNativeGdi *pNative = (IDuiNativeGdi *)h;
	if (NULL == pNative) return 0;
	switch (pNative->GetNativeKind())
	{
		case IDuiNativeGdi::Kind_Canvas: return OBJ_MEMDC;
		case IDuiNativeGdi::Kind_Image: return OBJ_BITMAP;
		case IDuiNativeGdi::Kind_Font: return OBJ_FONT;
		case IDuiNativeGdi::Kind_Region: return OBJ_REGION;
		case IDuiNativeGdi::Kind_Pen: return OBJ_PEN;
		case IDuiNativeGdi::Kind_Brush: return OBJ_BRUSH;
		default: break;
	}
	return 0;
}

int SaveDC(HDC hdc)
{
	IDuiCanvas *pCanvas = DuiCanvasFromHDC(hdc);
	if (NULL == pCanvas) return 0;
	pCanvas->Save();
	return 1;
}

BOOL RestoreDC(HDC hdc, int)
{
	IDuiCanvas *pCanvas = DuiCanvasFromHDC(hdc);
	if (NULL == pCanvas) return FALSE;
	pCanvas->Restore();
	return TRUE;
}

HFONT CreateFontIndirect(const LOGFONT *lplf)
{
	if (NULL == lplf) return NULL;
	int nSize = lplf->lfHeight;
	if (nSize < 0) nSize = -nSize;
	if (nSize == 0) nSize = 12;
	return (HFONT)CDUIFontRaster::Create(lplf->lfFaceName, nSize, lplf->lfWeight,
		lplf->lfItalic != 0, lplf->lfUnderline != 0, lplf->lfStrikeOut != 0);
}

HGDIOBJ GetStockObject(int i)
{
	if (HOLLOW_BRUSH == i || NULL_BRUSH == i)
	{
		g_HollowBrush.bHollow = true;
		return &g_HollowBrush;
	}
	return NULL;
}

int GetObject(HGDIOBJ h, int c, LPVOID pv)
{
	IDuiNativeGdi *pNative = (IDuiNativeGdi *)h;
	if (NULL == pNative || NULL == pv || c < (int)sizeof(BITMAP)) return 0;
	if (IDuiNativeGdi::Kind_Image != pNative->GetNativeKind()
		&& IDuiNativeGdi::Kind_Canvas != pNative->GetNativeKind())
	{
		return 0;
	}

	BITMAP *pBmp = (BITMAP *)pv;
	memset(pBmp, 0, sizeof(BITMAP));
	pBmp->bmWidth = pNative->GetWidth();
	pBmp->bmHeight = pNative->GetHeight();
	pBmp->bmWidthBytes = pNative->GetPitch();
	pBmp->bmPlanes = 1;
	pBmp->bmBitsPixel = 32;
	pBmp->bmBits = pNative->GetBits();
	return sizeof(BITMAP);
}

int GetDeviceCaps(HDC, int nIndex)
{
	if (LOGPIXELSX == nIndex || LOGPIXELSY == nIndex) return 96;
	return 0;
}

BOOL UpdateLayeredWindow(HWND, HDC, POINT *, SIZE *, HDC, POINT *, DWORD, BLENDFUNCTION *, DWORD)
{
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
// Region / clip
HRGN CreateRectRgnIndirect(const RECT *lprect)
{
	if (NULL == lprect) return NULL;
	DuiGdiRegion *p = new DuiGdiRegion();
	p->shape = DuiGdiRegion::Shape_Rect;
	p->rcRegion = *lprect;
	return (HRGN)p;
}

HRGN CreateEllipticRgnIndirect(const RECT *lprect)
{
	if (NULL == lprect) return NULL;
	DuiGdiRegion *p = new DuiGdiRegion();
	p->shape = DuiGdiRegion::Shape_Ellipse;
	p->rcRegion = *lprect;
	return (HRGN)p;
}

HRGN CreateRoundRectRgn(int x1, int y1, int x2, int y2, int w, int h)
{
	DuiGdiRegion *p = new DuiGdiRegion();
	p->shape = DuiGdiRegion::Shape_Round;
	p->rcRegion = { x1, y1, x2, y2 };
	p->nRoundX = w;
	p->nRoundY = h;
	return (HRGN)p;
}

int CombineRgn(HRGN hrgnDest, HRGN hrgnSrc1, HRGN hrgnSrc2, int fnCombineMode)
{
	DuiGdiRegion *pD = (DuiGdiRegion *)hrgnDest;
	DuiGdiRegion *pA = (DuiGdiRegion *)hrgnSrc1;
	DuiGdiRegion *pB = (DuiGdiRegion *)hrgnSrc2;
	if (NULL == pD || NULL == pA || NULL == pB) return 0;
	if (RGN_AND == fnCombineMode || 1 == fnCombineMode)
	{
		IntersectRect(&pD->rcRegion, &pA->rcRegion, &pB->rcRegion);
		if (DuiGdiRegion::Shape_Rect != pB->shape)
		{
			pD->shape = pB->shape;
			pD->nRoundX = pB->nRoundX;
			pD->nRoundY = pB->nRoundY;
		}
		else if (DuiGdiRegion::Shape_Rect != pA->shape)
		{
			pD->shape = pA->shape;
			pD->nRoundX = pA->nRoundX;
			pD->nRoundY = pA->nRoundY;
		}
		else pD->shape = DuiGdiRegion::Shape_Rect;
		return 1;
	}
	pD->rcRegion = pA->rcRegion;
	pD->shape = pA->shape;
	return 1;
}

int SelectClipRgn(HDC hdc, HRGN hRgn)
{
	IDuiCanvas *pCanvas = DuiCanvasFromHDC(hdc);
	if (NULL == pCanvas) return 0;

	pCanvas->SelectClipRgn(hRgn);

	return 1;
}

int GetClipBox(HDC hdc, LPRECT lprect)
{
	if (NULL == lprect) return 0;
	CDUICanvasRaster *pCanvas = dynamic_cast<CDUICanvasRaster *>(DuiCanvasFromHDC(hdc));
	if (NULL == pCanvas)
	{
		lprect->left = lprect->top = 0;
		lprect->right = lprect->bottom = 0;
		return 0;
	}
	*lprect = pCanvas->GetClipBoxRect();
	return 1;
}

//////////////////////////////////////////////////////////////////////////
HPEN CreatePen(int, int nWidth, DWORD crColor)
{
	DuiGdiPen *p = new DuiGdiPen();
	p->nWidth = max(1, nWidth);
	// COLORREF is 0x00BBGGRR
	p->dwColor = DUIARGB(255, GetRValue(crColor), GetGValue(crColor), GetBValue(crColor));
	return (HPEN)p;
}

BOOL Rectangle(HDC hdc, int left, int top, int right, int bottom)
{
	IDuiCanvas *pCanvas = DuiCanvasFromHDC(hdc);
	if (NULL == pCanvas) return FALSE;
	RECT rcRegion = { left, top, right, bottom };
	DuiGdiPen *pPen = (DuiGdiPen *)t_selectedPen;
	const int nWidth = pPen ? pPen->nWidth : 1;
	const DWORD dwColor = pPen ? pPen->dwColor : DUIARGB(255, 0, 0, 0);
	SIZE sz = {};
	pCanvas->DrawRect(rcRegion, nWidth, dwColor, sz, 0);
	return TRUE;
}

BOOL RoundRect(HDC hdc, int left, int top, int right, int bottom, int width, int height)
{
	IDuiCanvas *pCanvas = DuiCanvasFromHDC(hdc);
	if (NULL == pCanvas) return FALSE;
	RECT rcRegion = { left, top, right, bottom };
	RECT rcRound = { width, height, width, height };
	DuiGdiPen *pPen = (DuiGdiPen *)t_selectedPen;
	const int nWidth = pPen ? pPen->nWidth : 1;
	const DWORD dwColor = pPen ? pPen->dwColor : DUIARGB(255, 0, 0, 0);
	pCanvas->DrawRoundRect(rcRegion, rcRound, nWidth, dwColor);
	return TRUE;
}

int SetBkMode(HDC, int) { return TRANSPARENT; }

int SetTextColor(HDC, DWORD color)
{
	DWORD old = t_textColor;
	// COLORREF BGR -> Gdiplus::ARGB
	t_textColor = DUIARGB(255, GetRValue(color), GetGValue(color), GetBValue(color));
	return (int)old;
}

int DrawText(HDC hdc, LPCTSTR lpchText, int cchText, LPRECT lprc, UINT dwStyle)
{
	IDuiCanvas *pCanvas = DuiCanvasFromHDC(hdc);
	if (NULL == pCanvas || NULL == lprc || NULL == lpchText) return 0;

	CMMString text = (cchText < 0) ? CMMString(lpchText) : CMMString(lpchText, cchText);
	RECT rcRegion = *lprc;
	IDuiFont *pFont = DuiFontFromHFONT(t_selectedFont);
	pCanvas->DrawText(pFont, rcRegion, text.c_str(), t_textColor, dwStyle);

	if (dwStyle & DT_CALCRECT)
	{
		*lprc = rcRegion;
	}

	return rcRegion.bottom - lprc->top;
}

HDC GetDC(HWND) { return NULL; }
int ReleaseDC(HWND, HDC) { return 1; }
void GdiFlush() {}

int GetDIBits(HDC, HBITMAP hbm, UINT, UINT cLines, LPVOID lpvBits, LPBITMAPINFO lpbi, UINT)
{
	IDuiNativeGdi *pNative = (IDuiNativeGdi *)hbm;
	if (NULL == pNative || NULL == lpvBits || NULL == lpbi) return 0;
	if (IDuiNativeGdi::Kind_Image != pNative->GetNativeKind()
		&& IDuiNativeGdi::Kind_Canvas != pNative->GetNativeKind())
	{
		return 0;
	}

	LPBYTE pSrc = pNative->GetBits();
	const int nWidth = pNative->GetWidth();
	const int nHeight = pNative->GetHeight();
	if (NULL == pSrc || nWidth <= 0 || nHeight <= 0) return 0;

	const int nPitch = nWidth * 4;
	const UINT nCopy = min(cLines, (UINT)nHeight);
	BYTE *pDst = (BYTE *)lpvBits;
	// Raster HBITMAPs are always top-down (same as CreateARGB32Bitmap bPositive=true / biHeight<0).
	// Match Win32 GetDIBits: biHeight>0 => bottom-up output; biHeight<0 => top-down output.
	const bool bBottomUp = lpbi->bmiHeader.biHeight > 0;
	for (UINT y = 0; y < nCopy; ++y)
	{
		const int nSrcY = bBottomUp ? (nHeight - 1 - (int)y) : (int)y;
		memcpy(pDst + y * nPitch, pSrc + nSrcY * nPitch, (size_t)nPitch);
	}
	return (int)nCopy;
}

HRESULT CreateStreamOnHGlobal(HGLOBAL, BOOL, IStream **ppstm)
{
	if (ppstm) *ppstm = NULL;
	return E_FAIL;
}

HBITMAP CreateCompatibleBitmap(HDC hdc, int cx, int cy)
{
	(void)hdc;
	return (HBITMAP)new CDUIImageRaster(max(1, cx), max(1, cy), true);
}

HBITMAP CreateDIBSection(HDC, const BITMAPINFO *pbmi, UINT, void **ppvBits, HANDLE, DWORD)
{
	if (NULL == pbmi) return NULL;
	const int cx = abs(pbmi->bmiHeader.biWidth);
	const int cy = abs(pbmi->bmiHeader.biHeight);
	// Always top-down scanlines (Win32 equivalent: biHeight < 0 / CreateARGB32Bitmap bPositive=true).
	// Callers that fill from stb/WebP must write top row first.
	CDUIImageRaster *pImage = new CDUIImageRaster(max(1, cx), max(1, cy), true);
	if (ppvBits) *ppvBits = pImage->GetBits();
	return (HBITMAP)pImage;
}

BOOL WINAPI AlphaBlend(HDC hdcDest, int xoriginDest, int yoriginDest, int wDest, int hDest,
	HDC hdcSrc, int xoriginSrc, int yoriginSrc, int wSrc, int hSrc, BLENDFUNCTION)
{
	return StretchBlt(hdcDest, xoriginDest, yoriginDest, wDest, hDest,
		hdcSrc, xoriginSrc, yoriginSrc, wSrc, hSrc, SRCCOPY);
}

int DrawShadowText(HDC hdc, LPCTSTR lpchText, int cchText, LPRECT lprc, UINT dwStyle,
	DWORD crText, DWORD, int, int)
{
	SetTextColor(hdc, crText);
	return DrawText(hdc, lpchText, cchText, lprc, dwStyle);
}

BOOL GetTextExtentPoint32(HDC, LPCTSTR lpString, int c, LPSIZE psizl)
{
	if (NULL == psizl || NULL == lpString) return FALSE;
	IDuiFont *pFont = DuiFontFromHFONT(t_selectedFont);
	CMMString text = (c < 0) ? CMMString(lpString) : CMMString(lpString, c);
	SIZE sz = {};
	if (pFont) sz = pFont->MeasureText(text.c_str(), 0, DT_SINGLELINE);
	*psizl = sz;
	return TRUE;
}

HGLOBAL GlobalAlloc(UINT, SIZE_T dwBytes)
{
	return (HGLOBAL)malloc((size_t)dwBytes);
}

LPVOID GlobalLock(HGLOBAL hMem)
{
	return (LPVOID)hMem;
}

BOOL GlobalUnlock(HGLOBAL hMem)
{
	if (hMem)
	{
		free(hMem)
	}

	return TRUE;
}

#endif
