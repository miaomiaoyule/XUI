#ifndef __DUIRENDER_H__
#define __DUIRENDER_H__

#pragma once

#if defined(DuiPlatform_SDL)
#include "DUIRender/IDuiCanvas.h"
#endif

/////////////////////////////////////////////////////////////////////////////////////
class XUI_API CDUIRenderClip
{
public:
	~CDUIRenderClip();
	HDC									m_hDC = NULL;
	HRGN								m_hRgn = NULL;
	HRGN								m_hOldRgn = NULL;

	void GenerateClip(HDC hDC, RECT rcPaint);
	void GenerateRoundClip(HDC hDC, RECT rcPaint, RECT rcItem, int width, int height);
	void GenerateEllipseClip(HDC hDC, RECT rcPaint, RECT rcItem);
};

/////////////////////////////////////////////////////////////////////////////////////
//
class XUI_API CDUIRenderEngine
{
public:
	//draw argb
	static void DrawImage(HDC hDC, HBITMAP hBitmap, const CDUIRect &rcItem, const CDUIRect &rcPaint, const CDUIRect &rcBmpPart, const CDUIRect &rcCorner,
		BYTE cbAlpha = 255, bool bAlpha = false, bool bCornerHole = false, bool bTiledX = false, bool bTiledY = false, const CDUIRect &rcRound = {}, enDuiRoundType RoundType = Round_Normal);
	static void DrawImage(HDC hDC, Gdiplus::Bitmap *pBmp, const CDUIRect &rcItem, const CDUIRect &rcPaint, const CDUIRect &rcBmpPart, const CDUIRect &rcCorner,
		bool bCornerHole = false, bool bTiledX = false, bool bTiledY = false, const CDUIRect &rcRound = {}, enDuiRoundType RoundType = Round_Normal);
	static void DrawImage(HDC hDC, Gdiplus::Bitmap *pBmp, const CDUIRect &rcItem, const CDUIRect &rcRound = {}, enDuiRoundType RoundType = Round_Normal);
	static void DrawImage(HDC hDC, Gdiplus::Bitmap *pBmp, const CDUIRect &rcItem, Gdiplus::REAL fAngle);
	static void DrawLine(HDC hDC, const CDUIRect &rcItem, int nLineSize, Gdiplus::ARGB dwPenColor, enDuiLineStyle LineStyle = LineStyle_Solid);
	static void DrawRect(HDC hDC, const CDUIRect &rcItem, int nLineSize, Gdiplus::ARGB dwPenColor, CDUISize szBreakTop = {}, enDuiLineStyle LineStyle = LineStyle_Solid);
	static void DrawPath(HDC hDC, const std::vector<CDUIPoint> &vecPtList, int nLineSize, Gdiplus::ARGB dwPenColor, enDuiLineStyle LineStyle = LineStyle_Solid);
	static void DrawRound(HDC hDC, const CDUIRect &rcItem, const CDUIRect &rcRound, int nLineSize, Gdiplus::ARGB dwPenColor);
	static void DrawRoundRect(HDC hDC, const CDUIRect &rcItem, const CDUIRect &rcRound, int nLineSize, Gdiplus::ARGB dwPenColor, CDUISize szBreakTop = {});
	static void DrawParallelogram(HDC hDC, const CDUIRect &rcItem, int nLineSize, Gdiplus::ARGB dwPenColor, enDuiLineStyle LineStyle = LineStyle_Solid);
	static void DrawRhomb(HDC hDC, const CDUIRect &rcItem, int nLineSize, Gdiplus::ARGB dwPenColor, enDuiLineStyle LineStyle = LineStyle_Solid);
	static void DrawEllipse(HDC hDC, const CDUIRect &rcItem, int nLineSize, Gdiplus::ARGB dwPenColor, enDuiLineStyle LineStyle = LineStyle_Solid);
	static void DrawArc(HDC hDC, const CDUIRect &rcItem, int nLineSize, Gdiplus::ARGB dwPenColor, float fStartAngle, float fSweepAngle, enDuiLineStyle LineStyle = LineStyle_Solid);
	static void FillRect(HDC hDC, const CDUIRect &rcItem, Gdiplus::ARGB dwColor, Gdiplus::ARGB dwColorGradient = 0);
	static void FillRoundRect(HDC hDC, const CDUIRect &rcItem, const CDUIRect &rcRound, int nLineSize, Gdiplus::ARGB dwColor, Gdiplus::ARGB dwColorGradient = 0);
	static void FillParallelogram(HDC hDC, const CDUIRect &rcItem, Gdiplus::ARGB dwColor, Gdiplus::ARGB dwColorGradient = 0);
	static void FillRhomb(HDC hDC, const CDUIRect &rcItem, Gdiplus::ARGB dwColor, Gdiplus::ARGB dwColorGradient = 0);
	static void FillEllipse(HDC hDC, const CDUIRect &rcItem, Gdiplus::ARGB dwColor, Gdiplus::ARGB dwColorGradient = 0);

	//draw text
	static void DrawText(HDC hDC, HFONT hFont, CDUIRect &rcItem, LPCTSTR lpszText, Gdiplus::ARGB dwTextColor, UINT dwTextStyle, bool bGdiplusRender = false, Gdiplus::TextRenderingHint RenderType = Gdiplus::TextRenderingHintSystemDefault, bool bCeilFloat = true, bool bShadow = false);
	static void DrawRichText(HDC hDC, CDUIRect &rcItem, const tagDuiRichText &RichText, bool bGdiplusRender = false, Gdiplus::TextRenderingHint RenderType = Gdiplus::TextRenderingHintSystemDefault, int nLineSpace = 0, bool bShadow = false);
	static void DrawRichText(HDC hDC, CDUIRect &rcItem, const MapLineVecDuiRichTextDraw &mapLineVecRichTextDraw, UINT dwTextStyle, bool bGdiplusRender = false, Gdiplus::TextRenderingHint RenderType = Gdiplus::TextRenderingHintSystemDefault, bool bShadow = false);
	static SIZE GetTextSize(HDC hDC, HFONT hFont, LPCTSTR lpszText, UINT dwTextStyle);
	
	//help
	static void RGBToHSL(IN Gdiplus::ARGB dwColor, OUT float *pH, OUT float *pS, OUT float *pL);
	static Gdiplus::ARGB HSLToRGB(float H, float S, float L);
	static Gdiplus::ARGB AdjustColor(Gdiplus::ARGB dwColor, short H, short S, short L);
	static void AdjustImage(CDUIImageBase *pImageBase, short H, short S, short L);
	static void AdjustImage(HBITMAP hBitmap, Gdiplus::ARGB dwColorSrc, Gdiplus::ARGB dwColorDest);
	static HBITMAP CreateARGB32Bitmap(HDC hDC, int cx, int cy, BYTE** pBits, bool bPositive = true);
	static HBITMAP GenerateBitmap(CDUIControlBase *pControl, const CDUIRect &rcItem, Gdiplus::ARGB dwFilterColor = 0);
	static Gdiplus::Bitmap * GenerateBitmap(const std::vector<BYTE> &vecFileData);
	static Gdiplus::Bitmap * GenerateEllipseBitmap(Gdiplus::Bitmap *pBmp, const CDUISize &szGenerate);
	static Gdiplus::Bitmap * GenerateThumbnail(Gdiplus::Bitmap *pBmp, const CDUISize &szGenerate);
	static HBITMAP CopyBitmap(HDC hDC, const CDUIRect &rcItem, Gdiplus::ARGB dwFilterColor = 0);
	static HBITMAP CopyBitmap(HBITMAP hBitmap, Gdiplus::ARGB dwFilterColor = 0, bool bDisablePallete = false, float fAlpha = 1.0);
	static Gdiplus::Bitmap * GetAlphaBitmap(HBITMAP hBitmap, bool bPreMultiplyArgb = true);
	static HBITMAP GetHBITMAP(Gdiplus::Bitmap *pBmp, bool bKeepAlpha = true);
	static bool SaveImage(HBITMAP hBitmap, CMMString strFile);
	static bool SaveImage(Gdiplus::Bitmap *pBmp, CMMString strFile);

	//alpha help
	static void ClearPixel(LPBYTE pBits, int nWidthBitmap, CDUIRect rcClear);
	static void ClearPixelAlpha(LPBYTE pBits, int nWidthBitmap, CDUIRect rcClear);
	static void RestorePixelAlpha(LPBYTE pBits, int nWidthBitmap, CDUIRect rcRestore);
};

//////////////////////////////////////////////////////////////////////////
class XUI_API CDUIMemDC
{
public:
	//************************************
	// Description:	create membitmap size is Width of rcClient, Heigh of rcClient
	// if bCopyBitmap is true, copy hDC from rcDest to membitmap rcDest
	// if bCopyBitmap is true, finaly copy membitmap from rcDest to dc rcDest
	//************************************
	CDUIMemDC(HDC hDC, const CDUIRect rcDest, const CDUIRect rcClient, bool bCopyBitmap = true);

	//************************************
	// Description:	create membitmap size is Width of rcDest, Heigh of rcDest
	// if bCopyBitmap is true, copy hDC from rcDest to membitmap point(0,0)
	// finaly copy membitmap from point(0,0) to dc rcDest
	//************************************
	CDUIMemDC(HDC hDC, const CDUIRect rcDest, bool bCopyBitmap = true);

	virtual ~CDUIMemDC(void);

public:
	operator HDC() const;

	void ResetDC();
	HDC GetSrcHDC();
	HDC GetMemHDC();
	HBITMAP GetMemBitmap();
	LPBYTE GetMemBmpBits();

	CDUIRect GetPaint();
	CDUIRect GetClient();

protected:
	HDC									m_hDC = NULL;
	HDC									m_hMemDC = NULL;

	HBITMAP								m_hBitmap = NULL;
	HBITMAP								m_hOldBitmap = NULL;
	LPBYTE								m_pBmpBits = NULL;

	CDUIRect							m_rcDest;
	CDUIRect							m_rcClient;

	bool								m_bFromStart = false;
	bool								m_bCopyBitmap = true;

private:
	const CDUIMemDC & operator = (CDUIMemDC);
};

//////////////////////////////////////////////////////////////////////////

#endif 
