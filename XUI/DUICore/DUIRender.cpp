#include "StdAfx.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_STATIC
#include "../DUIUtils/stb_image_write.h"

/////////////////////////////////////////////////////////////////////////////////////
CDUIRenderClip::~CDUIRenderClip()
{
	if (NULL == m_hDC) return;

	ASSERT(::GetObjectType(m_hDC) == OBJ_DC || ::GetObjectType(m_hDC) == OBJ_MEMDC);
	ASSERT(::GetObjectType(m_hRgn) == OBJ_REGION);
	ASSERT(::GetObjectType(m_hOldRgn) == OBJ_REGION);
	::SelectClipRgn(m_hDC, m_hOldRgn);
	MMSafeDeleteObject(m_hOldRgn);
	MMSafeDeleteObject(m_hRgn);

	return;
}

void CDUIRenderClip::GenerateClip(HDC hDC, RECT rcPaint)
{
	m_hDC = hDC;
	RECT rcClip = {};
	::GetClipBox(hDC, &rcClip);

	m_hOldRgn = ::CreateRectRgnIndirect(&rcClip);
	m_hRgn = ::CreateRectRgnIndirect(&rcPaint);

	::CombineRgn(m_hRgn, m_hRgn, m_hOldRgn, RGN_AND);
	::SelectClipRgn(hDC, m_hRgn);

	return;
}

void CDUIRenderClip::GenerateRoundClip(HDC hDC, RECT rcPaint, RECT rcItem, int width, int height)
{
	m_hDC = hDC;
	RECT rcClip = {};
	::GetClipBox(hDC, &rcClip);

	m_hOldRgn = ::CreateRectRgnIndirect(&rcClip);
	m_hRgn = ::CreateRectRgnIndirect(&rcPaint);
	HRGN hRgnItem = ::CreateRoundRectRgn(rcItem.left, rcItem.top, rcItem.right, rcItem.bottom, width, height);

	::CombineRgn(m_hRgn, m_hRgn, hRgnItem, RGN_AND);
	::CombineRgn(m_hRgn, m_hRgn, m_hOldRgn, RGN_AND);
	::SelectClipRgn(hDC, m_hRgn);
	MMSafeDeleteObject(hRgnItem);

	return;
}

void CDUIRenderClip::GenerateEllipseClip(HDC hDC, RECT rcPaint, RECT rcItem)
{
	m_hDC = hDC;
	RECT rcClip = {};
	::GetClipBox(hDC, &rcClip);

	m_hOldRgn = ::CreateEllipticRgnIndirect(&rcClip);
	m_hRgn = ::CreateEllipticRgnIndirect(&rcPaint);
	HRGN hRgnItem = ::CreateEllipticRgnIndirect(&rcItem);

	::CombineRgn(m_hRgn, m_hRgn, hRgnItem, RGN_AND);
	::CombineRgn(m_hRgn, m_hRgn, m_hOldRgn, RGN_AND);
	::SelectClipRgn(hDC, m_hRgn);
	MMSafeDeleteObject(hRgnItem);

	return;
}

/////////////////////////////////////////////////////////////////////////////////////
static COLORREF PixelAlpha(COLORREF clrSrc, double src_darken, COLORREF clrDest, double dest_darken)
{
	return RGB(GetRValue(clrSrc) * src_darken + GetRValue(clrDest) * dest_darken,
		GetGValue(clrSrc) * src_darken + GetGValue(clrDest) * dest_darken,
		GetBValue(clrSrc) * src_darken + GetBValue(clrDest) * dest_darken);
}

static BOOL WINAPI AlphaBitBlt(HDC hDC, int nDestX, int nDestY, int dwWidth, int dwHeight, HDC hSrcDC, \
	int nSrcX, int nSrcY, int wSrc, int hSrc, BLENDFUNCTION ftn)
{
	HDC hTempDC = ::CreateCompatibleDC(hDC);
	if (NULL == hTempDC)
		return FALSE;

	//Creates Source DIB
	COLORREF *pSrcBits = NULL;
	HBITMAP hSrcDib = CDUIRenderEngine::CreateARGB32Bitmap(hSrcDC, dwWidth, dwHeight, (BYTE **)&pSrcBits, false);
	if ((NULL == hSrcDib) || (NULL == pSrcBits))
	{
		MMSafeDeleteDC(hTempDC);
		return FALSE;
	}

	HBITMAP hOldTempBmp = (HBITMAP)::SelectObject(hTempDC, hSrcDib);
	::StretchBlt(hTempDC, 0, 0, dwWidth, dwHeight, hSrcDC, nSrcX, nSrcY, wSrc, hSrc, SRCCOPY);
	::SelectObject(hTempDC, hOldTempBmp);

	//Creates Destination DIB
	COLORREF *pDestBits = NULL;
	HBITMAP hDestDib = CDUIRenderEngine::CreateARGB32Bitmap(hSrcDC, dwWidth, dwHeight, (BYTE **)&pDestBits, false);
	if ((NULL == hDestDib) || (NULL == pDestBits))
	{
		MMSafeDeleteObject(hSrcDib);
		MMSafeDeleteDC(hTempDC);
		return FALSE;
	}

	::SelectObject(hTempDC, hDestDib);
	::BitBlt(hTempDC, 0, 0, dwWidth, dwHeight, hDC, nDestX, nDestY, SRCCOPY);
	::SelectObject(hTempDC, hOldTempBmp);

	double src_darken;
	BYTE nAlpha;

	for (int pixel = 0; pixel < dwWidth * dwHeight; pixel++, pSrcBits++, pDestBits++)
	{
		nAlpha = LOBYTE(*pSrcBits >> 24);
		src_darken = (double)(nAlpha * ftn.SourceConstantAlpha) / 255.0 / 255.0;
		if (src_darken < 0.0) src_darken = 0.0;
		*pDestBits = PixelAlpha(*pSrcBits, src_darken, *pDestBits, 1.0 - src_darken);
	} //for

	::SelectObject(hTempDC, hDestDib);
	::BitBlt(hDC, nDestX, nDestY, dwWidth, dwHeight, hTempDC, 0, 0, SRCCOPY);
	::SelectObject(hTempDC, hOldTempBmp);

	MMSafeDeleteObject(hDestDib);
	MMSafeDeleteObject(hSrcDib);
	MMSafeDeleteDC(hTempDC);

	return TRUE;
}

static void ConstructRoundRectBezier(
	const CDUIRect& rc,
	const CDUIRect& rd,      // 各个圆角半径
	Gdiplus::GraphicsPath& path)
{
	float left = (float)rc.left;
	float top = (float)rc.top;
	float right = (float)rc.right;
	float bottom = (float)rc.bottom;

	float rtl = (float)rd.left;
	float rtr = (float)rd.top;
	float rbr = (float)rd.right;
	float rbl = (float)rd.bottom;

	float w = right - left;
	float h = bottom - top;
	if (w <= 0 || h <= 0) return;

	// 修正半径（不能超过宽/高的一半）
	rtl = min(rtl, h * 0.5f);
	rtl = min(rtl, w * 0.5f);

	rtr = min(rtr, h * 0.5f);
	rtr = min(rtr, w * 0.5f);

	rbr = min(rbr, h * 0.5f);
	rbr = min(rbr, w * 0.5f);

	rbl = min(rbl, h * 0.5f);
	rbl = min(rbl, w * 0.5f);

	// 圆形逼近系数
	const float K = 0.5522847498f;

	path.Reset();

	// === 开始于左上角（顶边中点）===
	path.StartFigure();

	// ---- top side ----
	path.AddLine(left + rtl, top, right - rtr, top);

	// ---- top-right corner (Bezier) ----
	path.AddBezier(
		right - rtr, top,
		right - rtr + K * rtr, top,
		right, top + rtr - K * rtr,
		right, top + rtr);

	// ---- right side ----
	path.AddLine(right, top + rtr, right, bottom - rbr);

	// ---- bottom-right corner ----
	path.AddBezier(
		right, bottom - rbr,
		right, bottom - rbr + K * rbr,
		right - rbr + K * rbr, bottom,
		right - rbr, bottom);

	// ---- bottom side ----
	path.AddLine(right - rbr, bottom, left + rbl, bottom);

	// ---- bottom-left corner ----
	path.AddBezier(
		left + rbl, bottom,
		left + rbl - K * rbl, bottom,
		left, bottom - rbl + K * rbl,
		left, bottom - rbl);

	// ---- left side ----
	path.AddLine(left, bottom - rbl, left, top + rtl);

	// ---- top-left corner ----
	path.AddBezier(
		left, top + rtl,
		left, top + rtl - K * rtl,
		left + rtl - K * rtl, top,
		left + rtl, top);

	path.CloseFigure();

	return;
}

static void ConstructRoundPath(const CDUIRect &rcDraw, const CDUIRect &rcRound, int nLineSize, Gdiplus::GraphicsPath &Path)
{
	if (rcDraw.GetWidth() <= 0 || rcDraw.GetHeight() <= 0) return;

	// 修正圆角半径（不能超过矩形一半）
	int rt = max(0, min(rcRound.top, rcDraw.GetHeight() / 2));
	int rr = max(0, min(rcRound.right, rcDraw.GetWidth() / 2));
	int rb = max(0, min(rcRound.bottom, rcDraw.GetHeight() / 2));
	int rl = max(0, min(rcRound.left, rcDraw.GetWidth() / 2));

	// 开始构造路径
	Path.Reset();

	// top-right
	Path.AddArc(
		rcDraw.right - rt * 2 - nLineSize,
		rcDraw.top,
		rt * 2, rt * 2,
		270, 90);

	// right
	Path.AddLine(
		rcDraw.right - nLineSize,
		rcDraw.top + rt,
		rcDraw.right - nLineSize,
		rcDraw.bottom - rr);

	// bottom-right
	Path.AddArc(
		rcDraw.right - rr * 2 - nLineSize,
		rcDraw.bottom - rr * 2 - nLineSize,
		rr * 2, rr * 2,
		0, 90);

	// bottom
	Path.AddLine(
		rcDraw.right - rr,
		rcDraw.bottom - nLineSize,
		rcDraw.left + rb,
		rcDraw.bottom - nLineSize);

	// bottom-left
	Path.AddArc(
		rcDraw.left,
		rcDraw.bottom - rb * 2 - nLineSize,
		rb * 2, rb * 2,
		90, 90);

	// left
	Path.AddLine(
		rcDraw.left,
		rcDraw.bottom - rb,
		rcDraw.left,
		rcDraw.top + rl);

	// top-left
	Path.AddArc(
		rcDraw.left,
		rcDraw.top,
		rl * 2, rl * 2,
		180, 90);

	Path.CloseFigure();

	return;
}

static void ConstructRoundPath(const CDUIRect &rcDraw, const CDUIRect &rcRound, CDUISize szBreakTop, int nLineSize, Gdiplus::GraphicsPath &Path)
{
	if (rcDraw.GetWidth() <= 0 || rcDraw.GetHeight() <= 0) return;

	if (szBreakTop.cx <= 0 && szBreakTop.cy <= 0)
	{
		ConstructRoundPath(rcDraw, rcRound, nLineSize, Path);
		return;
	}

	// 修正圆角半径边界
	int rt = max(0, min(rcRound.top, rcDraw.GetHeight() / 2));
	int rr = max(0, min(rcRound.right, rcDraw.GetWidth() / 2));
	int rb = max(0, min(rcRound.bottom, rcDraw.GetHeight() / 2));
	int rl = max(0, min(rcRound.left, rcDraw.GetWidth() / 2));

	// 计算缺口的起点和终点
	int nBreakStart = rcDraw.left + rl + szBreakTop.cx;
	int nBreakEnd = nBreakStart + szBreakTop.cy;
	nBreakEnd = min(nBreakEnd, rcDraw.right - rt);

	Path.Reset();

	// 1. 从缺口右侧起点到右上圆角水平线终点
	Path.AddLine(nBreakEnd, rcDraw.top, rcDraw.right - rt * 2, rcDraw.top);

	// 2. 右上圆角
	Path.AddArc(rcDraw.right - rt * 2 - nLineSize, rcDraw.top, rt * 2, rt * 2, 270, 90);

	// 3. 右边线
	Path.AddLine(rcDraw.right - nLineSize, rcDraw.top + rt, rcDraw.right - nLineSize, rcDraw.bottom - rr);

	// 4. 右下圆角
	Path.AddArc(rcDraw.right - rr * 2 - nLineSize, rcDraw.bottom - rr * 2 - nLineSize, rr * 2, rr * 2, 0, 90);

	// 5. 底部边线
	Path.AddLine(rcDraw.right - rr, rcDraw.bottom - nLineSize, rcDraw.left + rb, rcDraw.bottom - nLineSize);

	// 6. 左下圆角
	Path.AddArc(rcDraw.left, rcDraw.bottom - rb * 2 - nLineSize, rb * 2, rb * 2, 90, 90);

	// 7. 左边线
	Path.AddLine(rcDraw.left, rcDraw.bottom - rb, rcDraw.left, rcDraw.top + rl);

	// 8. 左上圆角
	Path.AddArc(rcDraw.left, rcDraw.top, rl * 2, rl * 2, 180, 90);

	// 9. 左上圆角结束到缺口左侧起点
	Path.AddLine(rcDraw.left + rl * 2, rcDraw.top, nBreakStart, rcDraw.top);

	// 缺口处不封口，不调用 Path.CloseFigure()
	return;
}

static void ConstructParallelogramPath(const CDUIRect &rcDraw, int nRound, int nLineSize, Gdiplus::GraphicsPath &Path)
{
	nRound = min(nRound, rcDraw.GetWidth());
	Path.AddLine(rcDraw.left + nRound, rcDraw.top, rcDraw.right, rcDraw.top); //top
	Path.AddLine(rcDraw.right - nLineSize, rcDraw.top, rcDraw.right - nLineSize - nRound, rcDraw.bottom); //right
	Path.AddLine(rcDraw.right - nRound, rcDraw.bottom - nLineSize, rcDraw.left, rcDraw.bottom - nLineSize); //bottom
	Path.AddLine(rcDraw.left, rcDraw.bottom, rcDraw.left + nRound, rcDraw.top); //left

	return;
}

static void ConstructRhombPath(const CDUIRect &rcDraw, int nLineSize, Gdiplus::GraphicsPath &Path)
{
	Path.AddLine(rcDraw.left + rcDraw.GetWidth() / 2 - 1, rcDraw.top + nLineSize / 2, rcDraw.right, rcDraw.top + rcDraw.GetHeight() / 2); //top
	Path.AddLine(rcDraw.right, rcDraw.top + rcDraw.GetHeight() / 2, rcDraw.right - rcDraw.GetWidth() / 2, rcDraw.bottom - nLineSize); //right
	Path.AddLine(rcDraw.right - rcDraw.GetWidth() / 2, rcDraw.bottom - nLineSize, rcDraw.left, rcDraw.bottom - rcDraw.GetHeight() / 2); //bottom
	Path.AddLine(rcDraw.left, rcDraw.bottom - rcDraw.GetHeight() / 2, rcDraw.left + rcDraw.GetWidth() / 2 + 1, rcDraw.top + nLineSize / 2); //left

	return;
}

static void ConstructEllipsePath(const CDUIRect &rcDraw, Gdiplus::GraphicsPath &Path)
{
	Path.AddEllipse(rcDraw.left, rcDraw.top, rcDraw.GetWidth(), rcDraw.GetHeight());

	return;
}

static void ConstructRhombPoints(const CDUIRect &rcDraw, int nLineSize, std::vector<Gdiplus::Point> &vecPoint)
{
	vecPoint.clear();
	vecPoint.push_back({ rcDraw.left + rcDraw.GetWidth() / 2, rcDraw.top });
	vecPoint.push_back({ rcDraw.right, rcDraw.top + rcDraw.GetHeight() / 2 });
	vecPoint.push_back({ rcDraw.left + rcDraw.GetWidth() / 2, rcDraw.bottom - nLineSize });
	vecPoint.push_back({ rcDraw.left, rcDraw.top + rcDraw.GetHeight() / 2 });

	return;
}

static void ConstructTextureBrushMatrix(Gdiplus::TextureBrush &Brush, Gdiplus::GraphicsPath &Path, Gdiplus::Bitmap *pBmp)
{
	if (NULL == pBmp) return;

	UINT bmpW = pBmp->GetWidth();
	UINT bmpH = pBmp->GetHeight();
	if (bmpW == 0 || bmpH == 0) return;  // 避免除零

	Gdiplus::RectF bounds;
	Path.GetBounds(&bounds);

	if (bounds.Width <= 0 || bounds.Height <= 0)
		return;  // 无效路径，不应用纹理

	Gdiplus::REAL sx = bounds.Width / (Gdiplus::REAL)bmpW;
	Gdiplus::REAL sy = bounds.Height / (Gdiplus::REAL)bmpH;

	// 避免 0，NaN，Inf
	const Gdiplus::REAL MIN_SCALE = 0.001f;
	if (!std::isfinite(sx) || sx < MIN_SCALE) sx = MIN_SCALE;
	if (!std::isfinite(sy) || sy < MIN_SCALE) sy = MIN_SCALE;

	Gdiplus::Matrix matrix;

	matrix.Scale(sx, sy);
	matrix.Translate(bounds.X, bounds.Y, Gdiplus::MatrixOrderAppend);

	Brush.SetTransform(&matrix);

	return;
}

// 与 DrawShadowText / 桌面图标字一致：对字形覆盖率做一次高斯模糊（sigma ≈ 1.4）。
// 笔画内部被核盖满，阴影更重；离开笔画后按距离衰减。偏移只决定投影方向。
static const int c_nTextShadowRadius = 3;
static const int c_nTextShadowOffsetX = 1;
static const int c_nTextShadowOffsetY = 1;
static void DuiBlurTextShadowAlpha(BYTE *pBits, int nWidth, int nHeight, int nStride, int nGain)
{
	// 归一化到 256 的可分离高斯核，半径 3
	static const int c_nKernel[7] = { 7, 27, 57, 74, 57, 27, 7 };
	static const int c_nDiv = 256;
	if (NULL == pBits || nWidth <= 0 || nHeight <= 0 || nStride < nWidth * 4 || nGain <= 0) return;

	std::vector<BYTE> vecAlpha((size_t)nWidth * (size_t)nHeight);
	for (int y = 0; y < nHeight; ++y)
	{
		const BYTE *pRow = pBits + (size_t)y * nStride;
		BYTE *pOut = vecAlpha.data() + (size_t)y * nWidth;
		for (int x = 0; x < nWidth; ++x)
		{
			int nSum = 0;
			for (int k = -c_nTextShadowRadius; k <= c_nTextShadowRadius; ++k)
			{
				int sx = x + k;
				if (sx < 0) sx = 0;
				else if (sx >= nWidth) sx = nWidth - 1;
				nSum += pRow[sx * 4 + 3] * c_nKernel[k + c_nTextShadowRadius];
			}
			pOut[x] = (BYTE)((nSum + c_nDiv / 2) / c_nDiv);
		}
	}

	for (int y = 0; y < nHeight; ++y)
	{
		BYTE *pRow = pBits + (size_t)y * nStride;
		for (int x = 0; x < nWidth; ++x)
		{
			int nSum = 0;
			for (int k = -c_nTextShadowRadius; k <= c_nTextShadowRadius; ++k)
			{
				int sy = y + k;
				if (sy < 0) sy = 0;
				else if (sy >= nHeight) sy = nHeight - 1;
				nSum += vecAlpha[(size_t)sy * nWidth + x] * c_nKernel[k + c_nTextShadowRadius];
			}
			int nAlpha = (nSum + c_nDiv / 2) / c_nDiv;
			nAlpha = nAlpha * nGain / 255;
			if (nAlpha > 255) nAlpha = 255;
			pRow[x * 4 + 0] = 0;
			pRow[x * 4 + 1] = 0;
			pRow[x * 4 + 2] = 0;
			pRow[x * 4 + 3] = (BYTE)nAlpha;
		}
	}
}

static void DuiDrawBlurredTextShadow(Gdiplus::Graphics &Gp, Gdiplus::Font &font, const Gdiplus::RectF &rectF,
	LPCTSTR lpszText, Gdiplus::StringFormat &stringFormat, BYTE cbTextA)
{
	const int nTextW = (int)ceil(rectF.Width);
	const int nTextH = (int)ceil(rectF.Height);
	if (nTextW <= 0 || nTextH <= 0 || nTextW > 2048 || nTextH > 2048) return;

	const int nPad = c_nTextShadowRadius + 3;
	const int nBmpW = nTextW + nPad * 2;
	const int nBmpH = nTextH + nPad * 2;
	Gdiplus::Bitmap bmpShadow(nBmpW, nBmpH, PixelFormat32bppPARGB);
	if (Gdiplus::Ok != bmpShadow.GetLastStatus()) return;

	{
		Gdiplus::Graphics gShadow(&bmpShadow);
		gShadow.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
		gShadow.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		gShadow.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
		gShadow.SetCompositingQuality(Gdiplus::CompositingQuality::CompositingQualityHighQuality);

		Gdiplus::RectF rcText((Gdiplus::REAL)nPad, (Gdiplus::REAL)nPad, rectF.Width, rectF.Height);
		Gdiplus::SolidBrush brushMask(Gdiplus::Color(255, 0, 0, 0));
		gShadow.DrawString(lpszText, -1, &font, rcText, &stringFormat, &brushMask);
	}

	Gdiplus::BitmapData data = {};
	Gdiplus::Rect rcLock(0, 0, nBmpW, nBmpH);
	if (Gdiplus::Ok != bmpShadow.LockBits(&rcLock, Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeWrite, PixelFormat32bppPARGB, &data)
		|| NULL == data.Scan0
		|| data.Stride < nBmpW * 4)
	{
		if (NULL != data.Scan0) bmpShadow.UnlockBits(&data);
		return;
	}

	// 模糊会摊薄细笔画的覆盖率，拉回笔画中心的浓度，边缘仍按高斯衰减
	DuiBlurTextShadowAlpha((BYTE *)data.Scan0, nBmpW, nBmpH, data.Stride, (int)cbTextA * 5 / 2);
	bmpShadow.UnlockBits(&data);

	Gp.DrawImage(&bmpShadow,
		Gdiplus::Rect((INT)rectF.X - nPad + c_nTextShadowOffsetX, (INT)rectF.Y - nPad + c_nTextShadowOffsetY, nBmpW, nBmpH),
		0, 0, nBmpW, nBmpH, Gdiplus::UnitPixel);

	return;
}

/////////////////////////////////////////////////////////////////////////////////////
void CDUIRenderEngine::DrawImage(HDC hDC, HBITMAP hBitmap, const CDUIRect &rcItem, const CDUIRect &rcPaint, const CDUIRect &rcBmpPart, const CDUIRect &rcCorner,
	BYTE cbAlpha, bool bAlpha, bool bCornerHole, bool bTiledX, bool bTiledY, const CDUIRect &rcRound, enDuiRoundType RoundType)
{
	ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);
	if (NULL == hDC || NULL == hBitmap) return;

	//verify
	CDUIRect rcTemp;
	CDUIRect rcDest;
	if (false == IntersectRect(&rcTemp, &rcItem, &rcPaint)) return;

	//info
	typedef BOOL(WINAPI *LPALPHABLEND)(HDC, int, int, int, int, HDC, int, int, int, int, BLENDFUNCTION);
	static LPALPHABLEND lpAlphaBlend = AlphaBlend;
	if (NULL == lpAlphaBlend) lpAlphaBlend = AlphaBitBlt;

	//scene
	bool bRoundPaint = (rcRound.left > 0 || rcRound.top > 0 || rcRound.right > 0 || rcRound.bottom > 0 || Round_Normal != RoundType);
	CDUIMemDC *pMemDC = bRoundPaint ? new CDUIMemDC(hDC, rcItem, CDUIRect(0, 0, rcItem.left + rcItem.GetWidth(), rcItem.top + rcItem.GetHeight())) : NULL;
	HDC hDCPaint = pMemDC ? *pMemDC : hDC;
	HDC hDCClone = ::CreateCompatibleDC(hDC);
	HBITMAP hBmpOldClone = (HBITMAP)::SelectObject(hDCClone, hBitmap);
	int nSaveDC = SaveDC(hDCPaint);

	//draw
	bool bAlphaBlend = (lpAlphaBlend && (bAlpha || cbAlpha < 255));
	BLENDFUNCTION bf = { AC_SRC_OVER, 0, cbAlpha, AC_SRC_ALPHA };

	//middle
	if (false == bCornerHole)
	{
		do
		{
			rcDest.Offset(rcItem.left - rcDest.left, rcItem.top - rcDest.top);
			rcDest.left += rcCorner.left;
			rcDest.top += rcCorner.top;
			rcDest.right = rcItem.right - rcCorner.right;
			rcDest.bottom = rcItem.bottom - rcCorner.bottom;
			if (false == ::IntersectRect(&rcTemp, &rcPaint, &rcDest)) break;

			if (false == bTiledX && false == bTiledY)
			{
				if (bAlphaBlend)
				{
					lpAlphaBlend(hDCPaint, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(), hDCClone, \
						rcBmpPart.left + rcCorner.left, rcBmpPart.top + rcCorner.top, \
						rcBmpPart.GetWidth() - rcCorner.left - rcCorner.right, \
						rcBmpPart.GetHeight() - rcCorner.top - rcCorner.bottom, bf);
				}
				else
				{
					::StretchBlt(hDCPaint, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(), hDCClone, \
						rcBmpPart.left + rcCorner.left, rcBmpPart.top + rcCorner.top, \
						rcBmpPart.GetWidth() - rcCorner.left - rcCorner.right, \
						rcBmpPart.GetHeight() - rcCorner.top - rcCorner.bottom, SRCCOPY);
				}

				break;
			}

			if (bTiledX && bTiledY)
			{
				LONG lWidth = rcBmpPart.GetWidth() - rcCorner.left - rcCorner.right;
				LONG lHeight = rcBmpPart.GetHeight() - rcCorner.top - rcCorner.bottom;
				if (lWidth <= 0 || lHeight <= 0) break;

				int iTimesX = (rcDest.GetWidth() + lWidth - 1) / lWidth;
				int iTimesY = (rcDest.GetHeight() + lHeight - 1) / lHeight;
				for (int j = 0; j < iTimesY; ++j)
				{
					LONG lDestTop = rcDest.top + lHeight * j;
					LONG lDestBottom = rcDest.top + lHeight * (j + 1);
					LONG lDrawHeight = lHeight;
					if (lDestBottom > rcDest.bottom)
					{
						lDrawHeight -= lDestBottom - rcDest.bottom;
						lDestBottom = rcDest.bottom;
					}
					for (int i = 0; i < iTimesX; ++i)
					{
						LONG lDestLeft = rcDest.left + lWidth * i;
						LONG lDestRight = rcDest.left + lWidth * (i + 1);
						LONG lDrawWidth = lWidth;
						if (lDestRight > rcDest.right)
						{
							lDrawWidth -= lDestRight - rcDest.right;
							lDestRight = rcDest.right;
						}

						if (bAlphaBlend)
						{
							lpAlphaBlend(hDCPaint, lDestLeft, lDestTop, lDestRight - lDestLeft, lDestBottom - lDestTop, hDCClone,
								rcBmpPart.left + rcCorner.left, rcBmpPart.top + rcCorner.top, lDrawWidth, lDrawHeight, bf);
						}
						else
						{
							::BitBlt(hDCPaint, lDestLeft, lDestTop, lDestRight - lDestLeft, lDestBottom - lDestTop, hDCClone, \
								rcBmpPart.left + rcCorner.left, rcBmpPart.top + rcCorner.top, SRCCOPY);
						}
					}
				}

				break;
			}

			if (bTiledX)
			{
				LONG lWidth = rcBmpPart.GetWidth() - rcCorner.left - rcCorner.right;
				if (lWidth <= 0) break;

				int iTimes = (rcDest.GetWidth() + lWidth - 1) / lWidth;
				for (int i = 0; i < iTimes; ++i)
				{
					LONG lDestLeft = rcDest.left + lWidth * i;
					LONG lDestRight = rcDest.left + lWidth * (i + 1);
					LONG lDrawWidth = lWidth;
					if (lDestRight > rcDest.right)
					{
						lDrawWidth -= lDestRight - rcDest.right;
						lDestRight = rcDest.right;
					}
					if (bAlphaBlend)
					{
						lpAlphaBlend(hDCPaint, lDestLeft, rcDest.top, lDestRight - lDestLeft, rcDest.GetHeight(),
							hDCClone, rcBmpPart.left + rcCorner.left, rcBmpPart.top + rcCorner.top, \
							lDrawWidth, rcBmpPart.GetHeight() - rcCorner.top - rcCorner.bottom, bf);
					}
					else
					{
						::StretchBlt(hDCPaint, lDestLeft, rcDest.top, lDestRight - lDestLeft, rcDest.GetHeight(),
							hDCClone, rcBmpPart.left + rcCorner.left, rcBmpPart.top + rcCorner.top, \
							lDrawWidth, rcBmpPart.GetHeight() - rcCorner.top - rcCorner.bottom, SRCCOPY);
					}
				}

				break;
			}

			//bTiledY
			{
				LONG lHeight = rcBmpPart.GetHeight() - rcCorner.top - rcCorner.bottom;
				if (lHeight <= 0) break;

				int iTimes = (rcDest.GetHeight() + lHeight - 1) / lHeight;
				for (int i = 0; i < iTimes; ++i)
				{
					LONG lDestTop = rcDest.top + lHeight * i;
					LONG lDestBottom = rcDest.top + lHeight * (i + 1);
					LONG lDrawHeight = lHeight;
					if (lDestBottom > rcDest.bottom)
					{
						lDrawHeight -= lDestBottom - rcDest.bottom;
						lDestBottom = rcDest.bottom;
					}
					if (bAlphaBlend)
					{
						lpAlphaBlend(hDCPaint, rcDest.left, lDestTop, rcDest.GetWidth(), lDestBottom - lDestTop,
							hDCClone, rcBmpPart.left + rcCorner.left, rcBmpPart.top + rcCorner.top, \
							rcBmpPart.GetWidth() - rcCorner.left - rcCorner.right, lDrawHeight, bf);
					}
					else
					{
						::StretchBlt(hDCPaint, rcDest.left, lDestTop, rcDest.GetWidth(), lDestBottom - lDestTop,
							hDCClone, rcBmpPart.left + rcCorner.left, rcBmpPart.top + rcCorner.top, \
							rcBmpPart.GetWidth() - rcCorner.left - rcCorner.right, lDrawHeight, SRCCOPY);
					}
				}
			}

		} while (false);
	}

	//corner
	if (rcCorner.left > 0 || rcCorner.right > 0 || rcCorner.top > 0 || rcCorner.bottom > 0)
	{
		//left-top
		if (rcCorner.left > 0 && rcCorner.top > 0)
		{
			rcDest.Offset(rcItem.left - rcDest.left, rcItem.top - rcDest.top);
			rcDest.right = rcDest.left + min(rcCorner.left, rcItem.GetWidth());
			rcDest.bottom = rcDest.top + min(rcCorner.top, rcItem.GetHeight());
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
			{
				if (bAlphaBlend)
				{
					lpAlphaBlend(hDCPaint, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(), hDCClone, \
						rcBmpPart.left, rcBmpPart.top, rcCorner.left, rcCorner.top, bf);
				}
				else
				{
					::StretchBlt(hDCPaint, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(), hDCClone, \
						rcBmpPart.left, rcBmpPart.top, rcCorner.left, rcCorner.top, SRCCOPY);
				}
			}
		}
		//left
		if (rcCorner.left > 0)
		{
			rcDest.Offset(rcItem.left - rcDest.left, rcItem.top - rcDest.top);
			rcDest.top += rcCorner.top;
			rcDest.right = rcDest.left + min(rcCorner.left, rcItem.GetWidth());
			rcDest.bottom = rcItem.bottom - rcCorner.bottom;
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
			{
				if (bAlphaBlend)
				{
					lpAlphaBlend(hDCPaint, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(), hDCClone, \
						rcBmpPart.left, rcBmpPart.top + rcCorner.top,
						rcCorner.left, rcBmpPart.GetHeight() - rcCorner.top - rcCorner.bottom, bf);
				}
				else
				{
					::StretchBlt(hDCPaint, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(), hDCClone, \
						rcBmpPart.left, rcBmpPart.top + rcCorner.top,
						rcCorner.left, rcBmpPart.GetHeight() - rcCorner.top - rcCorner.bottom, SRCCOPY);
				}
			}
		}
		//top
		if (rcCorner.top > 0)
		{
			rcDest.Offset(rcItem.left - rcDest.left, rcItem.top - rcDest.top);
			rcDest.left += rcCorner.left;
			rcDest.right = rcItem.right - rcCorner.right;
			rcDest.bottom = rcDest.top + min(rcCorner.top, rcItem.GetHeight());
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
			{
				if (bAlphaBlend)
				{
					lpAlphaBlend(hDCPaint, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(), hDCClone, \
						rcBmpPart.left + rcCorner.left, rcBmpPart.top,
						rcBmpPart.GetWidth() - rcCorner.left - rcCorner.right, rcCorner.top, bf);
				}
				else
				{
					::StretchBlt(hDCPaint, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(), hDCClone, \
						rcBmpPart.left + rcCorner.left, rcBmpPart.top,
						rcBmpPart.GetWidth() - rcCorner.left - rcCorner.right, rcCorner.top, SRCCOPY);
				}
			}
		}

		//right-bottom
		if (rcCorner.right > 0 && rcCorner.bottom > 0)
		{
			rcDest.Offset(rcItem.right - rcDest.right, rcItem.bottom - rcDest.bottom);
			rcDest.left = rcDest.right - rcCorner.right;
			rcDest.top = rcDest.bottom - rcCorner.bottom;
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
			{
				if (bAlphaBlend)
				{
					lpAlphaBlend(hDCPaint, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(), hDCClone, \
						rcBmpPart.right - rcCorner.right, rcBmpPart.bottom - rcCorner.bottom,
						rcCorner.right, rcCorner.bottom, bf);
				}
				else
				{
					::StretchBlt(hDCPaint, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(), hDCClone, \
						rcBmpPart.right - rcCorner.right, rcBmpPart.bottom - rcCorner.bottom,
						rcCorner.right, rcCorner.bottom, SRCCOPY);
				}
			}
		}
		//right
		if (rcCorner.right > 0)
		{
			rcDest.Offset(rcItem.right - rcDest.right, rcItem.top - rcDest.top);
			rcDest.left = rcDest.right - rcCorner.right;
			rcDest.top += rcCorner.top;
			rcDest.bottom = rcItem.bottom - rcCorner.bottom;
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
			{
				if (bAlphaBlend)
				{
					lpAlphaBlend(hDCPaint, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(), hDCClone, \
						rcBmpPart.right - rcCorner.right, rcBmpPart.top + rcCorner.top,
						rcCorner.right, rcBmpPart.GetHeight() - rcCorner.top - rcCorner.bottom, bf);
				}
				else
				{
					::StretchBlt(hDCPaint, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(), hDCClone, \
						rcBmpPart.right - rcCorner.right, rcBmpPart.top + rcCorner.top,
						rcCorner.right, rcBmpPart.GetHeight() - rcCorner.top - rcCorner.bottom, SRCCOPY);
				}
			}
		}
		//bottom
		if (rcCorner.bottom > 0)
		{
			rcDest.Offset(rcItem.left - rcDest.left, rcItem.bottom - rcDest.bottom);
			rcDest.left += rcCorner.left;
			rcDest.right = rcItem.right - rcCorner.right;
			rcDest.top = rcDest.bottom - rcCorner.bottom;
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
			{
				if (bAlphaBlend)
				{
					lpAlphaBlend(hDCPaint, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(), hDCClone, \
						rcBmpPart.left + rcCorner.left, rcBmpPart.bottom - rcCorner.bottom, \
						rcBmpPart.GetWidth() - rcCorner.left - rcCorner.right, rcCorner.bottom, bf);
				}
				else
				{
					::StretchBlt(hDCPaint, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(), hDCClone, \
						rcBmpPart.left + rcCorner.left, rcBmpPart.bottom - rcCorner.bottom, \
						rcBmpPart.GetWidth() - rcCorner.left - rcCorner.right, rcCorner.bottom, SRCCOPY);
				}
			}
		}

		//right-top
		if (rcCorner.right > 0 && rcCorner.top > 0)
		{
			rcDest.Offset(rcItem.right - rcDest.right, rcItem.top - rcDest.top);
			rcDest.left = rcDest.right - rcCorner.right;
			rcDest.bottom = rcDest.top + min(rcCorner.top, rcItem.GetHeight());
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
			{
				if (bAlphaBlend)
				{
					lpAlphaBlend(hDCPaint, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(), hDCClone, \
						rcBmpPart.right - rcCorner.right, rcBmpPart.top, rcCorner.right, rcCorner.top, bf);
				}
				else
				{
					::StretchBlt(hDCPaint, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(), hDCClone, \
						rcBmpPart.right - rcCorner.right, rcBmpPart.top, rcCorner.right, rcCorner.top, SRCCOPY);
				}
			}
		}

		//left-bottom
		if (rcCorner.left > 0 && rcCorner.bottom > 0)
		{
			rcDest.Offset(rcItem.left - rcDest.left, rcItem.bottom - rcDest.bottom);
			rcDest.right = rcDest.left + rcCorner.left;
			rcDest.top = rcDest.bottom - rcCorner.bottom;
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
			{
				if (bAlphaBlend)
				{
					lpAlphaBlend(hDCPaint, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(), hDCClone, \
						rcBmpPart.left, rcBmpPart.bottom - rcCorner.bottom, rcCorner.left, rcCorner.bottom, bf);
				}
				else
				{
					::StretchBlt(hDCPaint, rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight(), hDCClone, \
						rcBmpPart.left, rcBmpPart.bottom - rcCorner.bottom, rcCorner.left, rcCorner.bottom, SRCCOPY);
				}
			}
		}
	}

	::SelectObject(hDCClone, hBmpOldClone);
	MMSafeDeleteDC(hDCClone);
	RestoreDC(hDCPaint, nSaveDC);

	//round
	if (bRoundPaint)
	{
		do
		{
			HBITMAP hBitmap = CopyBitmap(hDCPaint, rcItem);
			Gdiplus::Bitmap *pBmp = GetAlphaBitmap(hBitmap);
			MMSafeDeleteObject(hBitmap);
			if (NULL == pBmp) break;

			DrawImage(hDC, pBmp, rcItem, rcRound, RoundType);

			MMSafeDelete(pBmp);

		} while (false);

		pMemDC->ResetDC();
	}

	MMSafeDelete(pMemDC);

	return;
}

void CDUIRenderEngine::DrawImage(HDC hDC, Gdiplus::Bitmap *pBmp, const CDUIRect &rcItem, const CDUIRect &rcPaint, const CDUIRect &rcBmpPart, const CDUIRect &rcCorner,
	bool bCornerHole, bool bTiledX, bool bTiledY, const CDUIRect &rcRound, enDuiRoundType RoundType)
{
	ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);
	if (NULL == hDC || NULL == pBmp) return;

	//no nine grid and roundpaint
	bool bRoundPaint = (rcRound.left > 0 || rcRound.top > 0 || rcRound.right > 0 || rcRound.bottom > 0 || Round_Normal != RoundType);
	if (bRoundPaint
		&& false == bCornerHole
		&& false == bTiledX
		&& false == bTiledY
		&& rcCorner.left == 0 && rcCorner.right == 0 && rcCorner.top == 0 && rcCorner.bottom == 0)
	{
		if (0 == rcBmpPart.left && 0 == rcBmpPart.top && rcBmpPart.GetWidth() == pBmp->GetWidth() && rcBmpPart.GetHeight() == pBmp->GetHeight())
		{
			return DrawImage(hDC, pBmp, rcItem, rcRound, RoundType);
		}

		Gdiplus::Bitmap *pBmpPart = pBmp->Clone(rcBmpPart.left, rcBmpPart.top, rcBmpPart.GetWidth(), rcBmpPart.GetHeight(), pBmp->GetPixelFormat());
		DrawImage(hDC, pBmpPart, rcItem, rcRound, RoundType);
		MMSafeDelete(pBmpPart);

		return;
	}

	//verify
	CDUIRect rcTemp;
	CDUIRect rcDest;
	if (false == IntersectRect(&rcTemp, &rcItem, &rcPaint)) return;

	//scene
	CDUIMemDC *pMemDC = bRoundPaint ? new CDUIMemDC(hDC, rcItem, CDUIRect(0, 0, rcItem.left + rcItem.GetWidth(), rcItem.top + rcItem.GetHeight())) : NULL;
	HDC hDCPaint = pMemDC ? *pMemDC : hDC;

	//info
	Gdiplus::Graphics Gp(hDCPaint);
	Gp.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	Gp.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

	//draw middle
	if (false == bCornerHole)
	{
		do
		{
			rcDest.Offset(rcItem.left - rcDest.left, rcItem.top - rcDest.top);
			rcDest.left += rcCorner.left;
			rcDest.top += rcCorner.top;
			rcDest.right = rcItem.right - rcCorner.right;
			rcDest.bottom = rcItem.bottom - rcCorner.bottom;
			if (false == ::IntersectRect(&rcTemp, &rcPaint, &rcDest)) break;

			if (false == bTiledX && false == bTiledY)
			{
				Gp.DrawImage(pBmp, Gdiplus::Rect(rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight()), \
					rcBmpPart.left + rcCorner.left, rcBmpPart.top + rcCorner.top, \
					rcBmpPart.GetWidth() - rcCorner.left - rcCorner.right, \
					rcBmpPart.GetHeight() - rcCorner.top - rcCorner.bottom, Gdiplus::UnitPixel);

				break;
			}

			if (bTiledX && bTiledY)
			{
				LONG lWidth = rcBmpPart.GetWidth() - rcCorner.left - rcCorner.right;
				LONG lHeight = rcBmpPart.GetHeight() - rcCorner.top - rcCorner.bottom;
				if (lWidth <= 0 || lHeight <= 0) break;

				int iTimesX = (rcDest.GetWidth() + lWidth - 1) / lWidth;
				int iTimesY = (rcDest.GetHeight() + lHeight - 1) / lHeight;
				for (int j = 0; j < iTimesY; ++j)
				{
					LONG lDestTop = rcDest.top + lHeight * j;
					LONG lDestBottom = rcDest.top + lHeight * (j + 1);
					LONG lDrawHeight = lHeight;
					if (lDestBottom > rcDest.bottom)
					{
						lDrawHeight -= lDestBottom - rcDest.bottom;
						lDestBottom = rcDest.bottom;
					}
					for (int i = 0; i < iTimesX; ++i)
					{
						LONG lDestLeft = rcDest.left + lWidth * i;
						LONG lDestRight = rcDest.left + lWidth * (i + 1);
						LONG lDrawWidth = lWidth;
						if (lDestRight > rcDest.right)
						{
							lDrawWidth -= lDestRight - rcDest.right;
							lDestRight = rcDest.right;
						}

						Gp.DrawImage(pBmp, Gdiplus::Rect(lDestLeft, lDestTop, lDestRight - lDestLeft, lDestBottom - lDestTop),
							rcBmpPart.left + rcCorner.left, rcBmpPart.top + rcCorner.top, lDrawWidth, lDrawHeight, Gdiplus::UnitPixel);
					}
				}

				break;
			}

			if (bTiledX)
			{
				LONG lWidth = rcBmpPart.GetWidth() - rcCorner.left - rcCorner.right;
				if (lWidth <= 0) break;

				int iTimes = (rcDest.GetWidth() + lWidth - 1) / lWidth;
				for (int i = 0; i < iTimes; ++i)
				{
					LONG lDestLeft = rcDest.left + lWidth * i;
					LONG lDestRight = rcDest.left + lWidth * (i + 1);
					LONG lDrawWidth = lWidth;
					if (lDestRight > rcDest.right)
					{
						lDrawWidth -= lDestRight - rcDest.right;
						lDestRight = rcDest.right;
					}

					Gp.DrawImage(pBmp, Gdiplus::Rect(lDestLeft, rcDest.top, lDestRight - lDestLeft, rcDest.GetHeight()),
						rcBmpPart.left + rcCorner.left, rcBmpPart.top + rcCorner.top, \
						lDrawWidth, rcBmpPart.GetHeight() - rcCorner.top - rcCorner.bottom, Gdiplus::UnitPixel);
				}

				break;
			}

			//bTiledY
			{
				LONG lHeight = rcBmpPart.GetHeight() - rcCorner.top - rcCorner.bottom;
				if (lHeight <= 0) break;

				int iTimes = (rcDest.GetHeight() + lHeight - 1) / lHeight;
				for (int i = 0; i < iTimes; ++i)
				{
					LONG lDestTop = rcDest.top + lHeight * i;
					LONG lDestBottom = rcDest.top + lHeight * (i + 1);
					LONG lDrawHeight = lHeight;
					if (lDestBottom > rcDest.bottom)
					{
						lDrawHeight -= lDestBottom - rcDest.bottom;
						lDestBottom = rcDest.bottom;
					}

					Gp.DrawImage(pBmp, Gdiplus::Rect(rcDest.left, lDestTop, rcDest.GetWidth(), lDestBottom - lDestTop),
						rcBmpPart.left + rcCorner.left, rcBmpPart.top + rcCorner.top, \
						rcBmpPart.GetWidth() - rcCorner.left - rcCorner.right, lDrawHeight, Gdiplus::UnitPixel);
				}
			}

		} while (false);
	}

	//draw corner
	if (rcCorner.left > 0 || rcCorner.right > 0 || rcCorner.top > 0 || rcCorner.bottom > 0)
	{
		//left-top
		if (rcCorner.left > 0 && rcCorner.top > 0)
		{
			rcDest.Offset(rcItem.left - rcDest.left, rcItem.top - rcDest.top);
			rcDest.right = rcDest.left + min(rcCorner.left, rcItem.GetWidth());
			rcDest.bottom = rcDest.top + min(rcCorner.top, rcItem.GetHeight());
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
			{
				Gp.DrawImage(pBmp, Gdiplus::Rect(rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight()), \
					rcBmpPart.left, rcBmpPart.top, rcCorner.left, rcCorner.top, Gdiplus::UnitPixel);
			}
		}
		//left
		if (rcCorner.left > 0)
		{
			rcDest.Offset(rcItem.left - rcDest.left, rcItem.top - rcDest.top);
			rcDest.top += rcCorner.top;
			rcDest.right = rcDest.left + min(rcCorner.left, rcItem.GetWidth());
			rcDest.bottom = rcItem.bottom - rcCorner.bottom;
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
			{
				Gp.DrawImage(pBmp, Gdiplus::Rect(rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight()), \
					rcBmpPart.left, rcBmpPart.top + rcCorner.top,
					rcCorner.left, rcBmpPart.GetHeight() - rcCorner.top - rcCorner.bottom, Gdiplus::UnitPixel);
			}
		}
		//top
		if (rcCorner.top > 0)
		{
			rcDest.Offset(rcItem.left - rcDest.left, rcItem.top - rcDest.top);
			rcDest.left += rcCorner.left;
			rcDest.right = rcItem.right - rcCorner.right;
			rcDest.bottom = rcDest.top + min(rcCorner.top, rcItem.GetHeight());
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
			{
				Gp.DrawImage(pBmp, Gdiplus::Rect(rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight()), \
					rcBmpPart.left + rcCorner.left, rcBmpPart.top,
					rcBmpPart.GetWidth() - rcCorner.left - rcCorner.right, rcCorner.top, Gdiplus::UnitPixel);
			}
		}

		//right-bottom
		if (rcCorner.right > 0 && rcCorner.bottom > 0)
		{
			rcDest.Offset(rcItem.right - rcDest.right, rcItem.bottom - rcDest.bottom);
			rcDest.left = rcDest.right - rcCorner.right;
			rcDest.top = rcDest.bottom - rcCorner.bottom;
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
			{
				Gp.DrawImage(pBmp, Gdiplus::Rect(rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight()), \
					rcBmpPart.right - rcCorner.right, rcBmpPart.bottom - rcCorner.bottom,
					rcCorner.right, rcCorner.bottom, Gdiplus::UnitPixel);
			}
		}
		//right
		if (rcCorner.right > 0)
		{
			rcDest.Offset(rcItem.right - rcDest.right, rcItem.top - rcDest.top);
			rcDest.left = rcDest.right - rcCorner.right;
			rcDest.top += rcCorner.top;
			rcDest.bottom = rcItem.bottom - rcCorner.bottom;
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
			{
				Gp.DrawImage(pBmp, Gdiplus::Rect(rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight()), \
					rcBmpPart.right - rcCorner.right, rcBmpPart.top + rcCorner.top,
					rcCorner.right, rcBmpPart.GetHeight() - rcCorner.top - rcCorner.bottom, Gdiplus::UnitPixel);
			}
		}
		//bottom
		if (rcCorner.bottom > 0)
		{
			rcDest.Offset(rcItem.left - rcDest.left, rcItem.bottom - rcDest.bottom);
			rcDest.left += rcCorner.left;
			rcDest.right = rcItem.right - rcCorner.right;
			rcDest.top = rcDest.bottom - rcCorner.bottom;
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
			{
				Gp.DrawImage(pBmp, Gdiplus::Rect(rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight()), \
					rcBmpPart.left + rcCorner.left, rcBmpPart.bottom - rcCorner.bottom, \
					rcBmpPart.GetWidth() - rcCorner.left - rcCorner.right, rcCorner.bottom, Gdiplus::UnitPixel);
			}
		}

		//right-top
		if (rcCorner.right > 0 && rcCorner.top > 0)
		{
			rcDest.Offset(rcItem.right - rcDest.right, rcItem.top - rcDest.top);
			rcDest.left = rcDest.right - rcCorner.right;
			rcDest.bottom = rcDest.top + min(rcCorner.top, rcItem.GetHeight());
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
			{
				Gp.DrawImage(pBmp, Gdiplus::Rect(rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight()), \
					rcBmpPart.right - rcCorner.right, rcBmpPart.top, rcCorner.right, rcCorner.top, Gdiplus::UnitPixel);
			}
		}

		//left-bottom
		if (rcCorner.left > 0 && rcCorner.bottom > 0)
		{
			rcDest.Offset(rcItem.left - rcDest.left, rcItem.bottom - rcDest.bottom);
			rcDest.right = rcDest.left + rcCorner.left;
			rcDest.top = rcDest.bottom - rcCorner.bottom;
			if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
			{
				Gp.DrawImage(pBmp, Gdiplus::Rect(rcDest.left, rcDest.top, rcDest.GetWidth(), rcDest.GetHeight()), \
					rcBmpPart.left, rcBmpPart.bottom - rcCorner.bottom, rcCorner.left, rcCorner.bottom, Gdiplus::UnitPixel);
			}
		}
	}

	//round
	if (bRoundPaint)
	{
		do
		{
			Gdiplus::Bitmap *pBmp = GetAlphaBitmap(CopyBitmap(hDCPaint, rcItem));
			if (NULL == pBmp) break;

			DrawImage(hDC, pBmp, rcItem, rcRound, RoundType);

			MMSafeDelete(pBmp);

		} while (false);

		pMemDC->ResetDC();
	}

	MMSafeDelete(pMemDC);

	return;
}

void CDUIRenderEngine::DrawImage(HDC hDC, Gdiplus::Bitmap *pBmp, const CDUIRect &rcItem, const CDUIRect &rcRound, enDuiRoundType RoundType)
{
	ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);
	if (NULL == hDC || NULL == pBmp) return;

	Gdiplus::Graphics Gp(hDC);
	Gp.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	Gp.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

	//for clip
	CDUIRect rcDraw = rcItem;
	rcDraw.right--;
	rcDraw.bottom--;

	do
	{
		if (Round_Normal == RoundType)
		{
			if (rcRound.left > 0
				|| rcRound.top > 0
				|| rcRound.right > 0
				|| rcRound.bottom > 0)
			{
				Gdiplus::GraphicsPath Path;
				ConstructRoundPath(rcDraw, rcRound, 0, Path);

				Gdiplus::TextureBrush Brush(pBmp, Gdiplus::WrapModeClamp);
				ConstructTextureBrushMatrix(Brush, Path, pBmp);

				Gp.FillPath(&Brush, &Path);
			}
			else
			{
				Gp.DrawImage(pBmp, Gdiplus::Rect(rcDraw.left, rcDraw.top, rcDraw.GetWidth(), rcDraw.GetHeight()), 0, 0, pBmp->GetWidth(), pBmp->GetHeight(), Gdiplus::UnitPixel);
			}

			break;
		}

		Gdiplus::TextureBrush BrushBmp((Gdiplus::Image*)pBmp, Gdiplus::WrapModeClamp);
		Gdiplus::GraphicsPath Path;
		switch (RoundType)
		{
			case Round_Parallelogram:
			{
				ConstructParallelogramPath(rcDraw, rcDraw.GetWidth() / 3, 0, Path);
				ConstructTextureBrushMatrix(BrushBmp, Path, pBmp);

				Gp.FillPath(&BrushBmp, &Path);

				break;
			}
			case Round_Rhomb:
			{
				ConstructRhombPath(rcDraw, 0, Path);
				ConstructTextureBrushMatrix(BrushBmp, Path, pBmp);

				Gp.FillPath(&BrushBmp, &Path);

				break;
			}
			case Round_Ellipse:
			{
				ConstructEllipsePath(rcDraw, Path);
				ConstructTextureBrushMatrix(BrushBmp, Path, pBmp);

				Gp.FillPath(&BrushBmp, &Path);

				break;
			}
		}

	} while (false);

	return;
}

void CDUIRenderEngine::DrawImage(HDC hDC, Gdiplus::Bitmap *pBmp, const CDUIRect &rcItem, Gdiplus::REAL fAngle)
{
	ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);
	if (NULL == hDC || NULL == pBmp) return;

	const int nSrcW = (int)pBmp->GetWidth();
	const int nSrcH = (int)pBmp->GetHeight();
	const int nDstW = rcItem.GetWidth();
	const int nDstH = rcItem.GetHeight();
	if (nSrcW <= 0 || nSrcH <= 0 || nDstW <= 0 || nDstH <= 0) return;

	Gdiplus::Graphics Gp(hDC);
	Gp.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	Gp.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

	const Gdiplus::REAL fCx = rcItem.left + nDstW / 2.0f;
	const Gdiplus::REAL fCy = rcItem.top + nDstH / 2.0f;
	if (0.0f != fAngle)
	{
		Gp.TranslateTransform(fCx, fCy);
		Gp.RotateTransform(fAngle);
		Gp.TranslateTransform(-fCx, -fCy);
	}
	Gp.DrawImage(pBmp, Gdiplus::Rect(rcItem.left, rcItem.top, nDstW, nDstH), 0, 0, nSrcW, nSrcH, Gdiplus::UnitPixel);

	return;
}

void CDUIRenderEngine::DrawLine(HDC hDC, const CDUIRect &rcItem, int nLineSize, Gdiplus::ARGB dwPenColor, enDuiLineStyle LineStyle)
{
	ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);

	Gdiplus::Graphics Gp(hDC);
	Gp.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	Gp.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

	Gdiplus::Pen Pen(Gdiplus::Color(dwPenColor), nLineSize);
	Pen.SetDashStyle((Gdiplus::DashStyle)LineStyle);
	Pen.SetAlignment(Gdiplus::PenAlignmentInset);
	Gp.DrawLine(&Pen, rcItem.left, rcItem.top, rcItem.right, rcItem.bottom);

	return;
}

void CDUIRenderEngine::DrawRect(HDC hDC, const CDUIRect &rcItem, int nLineSize, Gdiplus::ARGB dwPenColor, CDUISize szBreakTop, enDuiLineStyle LineStyle)
{
	ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);

	//gdi+
	//if (bGdiplusRender)
	{
		//topbreak
		if (szBreakTop.cx > 0 || szBreakTop.cy > 0)
		{
			//adjust
			CDUIRect rcDraw = rcItem;
			//rcDraw.top += nLineSize;
			//rcDraw.left += nLineSize;
			rcDraw.right -= nLineSize;
			rcDraw.bottom -= nLineSize;

			std::vector<CDUIPoint> vecPtList;
			vecPtList.push_back({ rcDraw.left + szBreakTop.cx + szBreakTop.cy, rcDraw.top });
			vecPtList.push_back({ rcDraw.right, rcDraw.top });
			vecPtList.push_back({ rcDraw.right, rcDraw.bottom });
			vecPtList.push_back({ rcDraw.left, rcDraw.bottom });
			vecPtList.push_back({ rcDraw.left, rcDraw.top });
			vecPtList.push_back({ rcDraw.left + szBreakTop.cx, rcDraw.top });
			DrawPath(hDC, vecPtList, nLineSize, dwPenColor, LineStyle);

			return;
		}

		//rect
		Gdiplus::Graphics Gp(hDC);
		Gdiplus::Pen Pen(Gdiplus::Color(dwPenColor), (Gdiplus::REAL)nLineSize);
		Pen.SetDashStyle((Gdiplus::DashStyle)LineStyle);
		Pen.SetAlignment(Gdiplus::PenAlignmentInset);

		Gp.DrawRectangle(&Pen, rcItem.left, rcItem.top, rcItem.GetWidth(), rcItem.GetHeight());

		return;
	}

	HPEN hPen = ::CreatePen(PS_SOLID | PS_INSIDEFRAME, nLineSize, RGB(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor)));
	HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
	::SelectObject(hDC, ::GetStockObject(HOLLOW_BRUSH));
	::Rectangle(hDC, rcItem.left, rcItem.top, rcItem.right, rcItem.bottom);
	::SelectObject(hDC, hOldPen);
	MMSafeDeleteObject(hPen);

	return;
}

void CDUIRenderEngine::DrawPath(HDC hDC, const std::vector<CDUIPoint> &vecPtList, int nLineSize, Gdiplus::ARGB dwPenColor, enDuiLineStyle LineStyle)
{
	Gdiplus::Graphics Gp(hDC);
	Gp.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	Gp.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

	//path
	Gdiplus::GraphicsPath Path;
	for (int n = 1; n < vecPtList.size(); n++)
	{
		Path.AddLine(vecPtList[n - 1].x, vecPtList[n - 1].y, vecPtList[n].x, vecPtList[n].y);
	}

	Gdiplus::Pen Pen(Gdiplus::Color(dwPenColor), nLineSize);
	Pen.SetAlignment(Gdiplus::PenAlignmentInset);
	Pen.SetDashStyle((Gdiplus::DashStyle)LineStyle);
	Gp.DrawPath(&Pen, &Path);

	return;
}

void CDUIRenderEngine::DrawRound(HDC hDC, const CDUIRect &rcItem, const CDUIRect &rcRound, int nLineSize, Gdiplus::ARGB dwPenColor)
{
	ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);

	Gdiplus::Graphics Gp(hDC);
	Gp.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	Gp.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

	//left top right bottom
	Gdiplus::Pen Pen(Gdiplus::Color(dwPenColor), nLineSize);
	Pen.SetAlignment(Gdiplus::PenAlignmentInset);
	Gp.DrawArc(&Pen, rcItem.left, rcItem.top, rcRound.left * 2, rcRound.left, 180, 90); // 左上圆角
	Gp.DrawArc(&Pen, rcItem.right - rcRound.top * 2 - nLineSize, rcItem.top, rcRound.top * 2, rcRound.top * 2, 270, 90); // 右上圆角
	Gp.DrawArc(&Pen, rcItem.right - rcRound.right * 2 - nLineSize, rcItem.bottom - rcRound.right * 2 - nLineSize, rcRound.right * 2, rcRound.right * 2, 0, 90); // 右下圆角
	Gp.DrawArc(&Pen, rcItem.left, rcItem.bottom - rcRound.bottom * 2 - nLineSize, rcRound.bottom * 2, rcRound.bottom * 2, 90, 90); // 左下圆角

	return;
}

void CDUIRenderEngine::DrawRoundRect(HDC hDC, const CDUIRect &rcItem, const CDUIRect &rcRound, int nLineSize, Gdiplus::ARGB dwPenColor, CDUISize szBreakTop)
{
	ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);

	//gdi+
	{
		Gdiplus::Graphics Gp(hDC);
		Gp.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		Gp.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

		//adjust
		CDUIRect rcDraw = rcItem;
		rcDraw.top += nLineSize;
		rcDraw.left += nLineSize;

		//path
		Gdiplus::GraphicsPath Path;
		ConstructRoundPath(rcDraw, rcRound, szBreakTop, nLineSize, Path);

		Gdiplus::Pen Pen(Gdiplus::Color(dwPenColor), nLineSize);
		Pen.SetAlignment(Gdiplus::PenAlignmentInset);
		Gp.DrawPath(&Pen, &Path);

		return;
	}

	HPEN hPen = ::CreatePen(PS_SOLID | PS_INSIDEFRAME, nLineSize, RGB(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor)));
	HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDC, ::GetStockObject(HOLLOW_BRUSH));
	::RoundRect(hDC, rcItem.left, rcItem.top, rcItem.right, rcItem.bottom, rcRound.left, rcRound.top);
	::SelectObject(hDC, hOldBrush);
	::SelectObject(hDC, hOldPen);
	MMSafeDeleteObject(hPen);

	return;
}

void CDUIRenderEngine::DrawParallelogram(HDC hDC, const CDUIRect &rcItem, int nLineSize, Gdiplus::ARGB dwPenColor, enDuiLineStyle LineStyle)
{
	Gdiplus::Graphics Gp(hDC);
	Gp.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	Gp.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

	//path
	CDUIRect rcDraw = rcItem;
	Gdiplus::GraphicsPath Path;
	ConstructParallelogramPath(rcDraw, rcDraw.GetWidth() / 3, nLineSize, Path);

	Gdiplus::Pen Pen(Gdiplus::Color(dwPenColor), nLineSize);
	Pen.SetAlignment(Gdiplus::PenAlignmentInset);
	Pen.SetDashStyle((Gdiplus::DashStyle)LineStyle);
	Gp.DrawPath(&Pen, &Path);

	return;
}

void CDUIRenderEngine::DrawRhomb(HDC hDC, const CDUIRect &rcItem, int nLineSize, Gdiplus::ARGB dwPenColor, enDuiLineStyle LineStyle)
{
	Gdiplus::Graphics Gp(hDC);
	Gp.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	Gp.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

	//path
	CDUIRect rcDraw = rcItem;
	Gdiplus::GraphicsPath Path;
	ConstructRhombPath(rcDraw, nLineSize, Path);

	Gdiplus::Pen Pen(Gdiplus::Color(dwPenColor), nLineSize);
	Pen.SetAlignment(Gdiplus::PenAlignmentInset);
	Pen.SetDashStyle((Gdiplus::DashStyle)LineStyle);
	Gp.DrawPath(&Pen, &Path);

	return;
}

void CDUIRenderEngine::DrawEllipse(HDC hDC, const CDUIRect &rcItem, int nLineSize, Gdiplus::ARGB dwPenColor, enDuiLineStyle LineStyle)
{
	Gdiplus::Graphics Gp(hDC);
	Gp.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	Gp.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

	Gdiplus::Pen Pen(Gdiplus::Color(dwPenColor), nLineSize);
	Pen.SetAlignment(Gdiplus::PenAlignmentInset);
	Pen.SetDashStyle((Gdiplus::DashStyle)LineStyle);
	Gp.DrawEllipse(&Pen, rcItem.left, rcItem.top, rcItem.GetWidth(), rcItem.GetHeight());

	return;
}

void CDUIRenderEngine::DrawArc(HDC hDC, const CDUIRect &rcItem, int nLineSize, Gdiplus::ARGB dwPenColor, float fStartAngle, float fSweepAngle, enDuiLineStyle LineStyle)
{
	ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);

	Gdiplus::Graphics Gp(hDC);
	Gp.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	Gp.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

	//left top right bottom
	Gdiplus::Pen Pen(Gdiplus::Color(dwPenColor), nLineSize);
	Pen.SetAlignment(Gdiplus::PenAlignmentInset);
	Gp.DrawArc(&Pen, rcItem.left, rcItem.top, rcItem.GetWidth(), rcItem.GetHeight(), fStartAngle, fSweepAngle);

	return;
}

void CDUIRenderEngine::FillRect(HDC hDC, const CDUIRect &rcItem, Gdiplus::ARGB dwColor, Gdiplus::ARGB dwColorGradient)
{
	if (dwColor <= 0x00ffffff) return;

	Gdiplus::Graphics Gp(hDC);
	Gp.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

	Gdiplus::Brush *pBrush = NULL;
	if (0 != dwColorGradient)
	{
		pBrush = new Gdiplus::LinearGradientBrush(Gdiplus::Point(rcItem.left, rcItem.top), Gdiplus::Point(rcItem.right, rcItem.bottom), dwColor, dwColorGradient);
	}
	else
	{
		pBrush = new Gdiplus::SolidBrush(dwColor);
	}

	Gp.FillRectangle(pBrush, rcItem.left, rcItem.top, rcItem.GetWidth(), rcItem.GetHeight());

	MMSafeDelete(pBrush);

	return;
}

void CDUIRenderEngine::FillRoundRect(HDC hDC, const CDUIRect &rcItem, const CDUIRect &rcRound, int nLineSize, Gdiplus::ARGB dwColor, Gdiplus::ARGB dwColorGradient)
{
	Gdiplus::Graphics Gp(hDC);
	Gp.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	Gp.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
	Gp.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

	//adjust
	CDUIRect rcDraw = rcItem;
	rcDraw.top += nLineSize;
	rcDraw.left += nLineSize;

	//for clip
	rcDraw.right--;
	rcDraw.bottom--;

	//path
	Gdiplus::GraphicsPath Path;
	ConstructRoundPath(rcDraw, rcRound, nLineSize, Path);

	//draw
	Gdiplus::Brush *pBrush = NULL;
	if (0 != dwColorGradient)
	{
		pBrush = new Gdiplus::LinearGradientBrush(Gdiplus::Point(rcItem.left, rcItem.top), Gdiplus::Point(rcItem.right, rcItem.bottom), dwColor, dwColorGradient);
	}
	else
	{
		pBrush = new Gdiplus::SolidBrush(dwColor);
	}

	Gp.FillPath(pBrush, &Path);

	MMSafeDelete(pBrush);

	return;
}

void CDUIRenderEngine::FillParallelogram(HDC hDC, const CDUIRect &rcItem, Gdiplus::ARGB dwColor, Gdiplus::ARGB dwColorGradient)
{
	if (dwColor <= 0x00ffffff) return;

	Gdiplus::Graphics Gp(hDC);
	Gp.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	Gp.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

	//adjust
	CDUIRect rcDraw = rcItem;

	//path
	Gdiplus::GraphicsPath Path;
	ConstructParallelogramPath(rcDraw, rcDraw.GetWidth() / 3, 0, Path);

	//draw
	Gdiplus::Brush *pBrush = NULL;
	if (0 != dwColorGradient)
	{
		pBrush = new Gdiplus::LinearGradientBrush(Gdiplus::Point(rcItem.left, rcItem.top), Gdiplus::Point(rcItem.right, rcItem.bottom), dwColor, dwColorGradient);
	}
	else
	{
		pBrush = new Gdiplus::SolidBrush(dwColor);
	}

	Gp.FillPath(pBrush, &Path);

	MMSafeDelete(pBrush);

	return;
}

void CDUIRenderEngine::FillRhomb(HDC hDC, const CDUIRect &rcItem, Gdiplus::ARGB dwColor, Gdiplus::ARGB dwColorGradient)
{
	if (dwColor <= 0x00ffffff) return;

	Gdiplus::Graphics Gp(hDC);
	Gp.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	Gp.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

	//adjust
	CDUIRect rcDraw = rcItem;

	//path
	Gdiplus::GraphicsPath Path;
	ConstructRhombPath(rcDraw, 0, Path);

	//draw
	Gdiplus::Brush *pBrush = NULL;
	if (0 != dwColorGradient)
	{
		pBrush = new Gdiplus::LinearGradientBrush(Gdiplus::Point(rcItem.left, rcItem.top), Gdiplus::Point(rcItem.right, rcItem.bottom), dwColor, dwColorGradient);
	}
	else
	{
		pBrush = new Gdiplus::SolidBrush(dwColor);
	}

	Gp.FillPath(pBrush, &Path);

	MMSafeDelete(pBrush);

	return;
}

void CDUIRenderEngine::FillEllipse(HDC hDC, const CDUIRect &rcItem, Gdiplus::ARGB dwColor, Gdiplus::ARGB dwColorGradient)
{
	if (dwColor <= 0x00ffffff) return;

	Gdiplus::Graphics Gp(hDC);
	Gp.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	Gp.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	
	//draw
	Gdiplus::Brush *pBrush = NULL;
	if (0 != dwColorGradient)
	{
		pBrush = new Gdiplus::LinearGradientBrush(Gdiplus::Point(rcItem.left, rcItem.top), Gdiplus::Point(rcItem.right, rcItem.bottom), dwColor, dwColorGradient);
	}
	else
	{
		pBrush = new Gdiplus::SolidBrush(dwColor);
	}

	Gp.FillEllipse(pBrush, rcItem.left, rcItem.top, rcItem.GetWidth(), rcItem.GetHeight());

	MMSafeDelete(pBrush);

	return;
}

void CDUIRenderEngine::DrawText(HDC hDC, HFONT hFont, CDUIRect &rcItem, LPCTSTR lpszText, Gdiplus::ARGB dwTextColor, UINT dwTextStyle,
	bool bGdiplusRender, Gdiplus::TextRenderingHint RenderType, bool bCeilFloat, bool bShadow)
{
	ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);
	if (MMInvalidString(lpszText) || NULL == hFont) return;

	if (bGdiplusRender)
	{
		Gdiplus::Graphics Gp(hDC);
		Gdiplus::Font font(hDC, hFont);
		Gdiplus::RectF rectF((Gdiplus::REAL)rcItem.left, (Gdiplus::REAL)rcItem.top, (Gdiplus::REAL)(rcItem.GetWidth()), (Gdiplus::REAL)(rcItem.GetHeight()));

		// 阴影字在透明底上不能用 ClearType，否则会出彩边/颗粒
		Gp.SetTextRenderingHint(RenderType);
		Gp.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		Gp.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
		Gp.SetCompositingQuality(Gdiplus::CompositingQuality::CompositingQualityHighQuality);

		Gdiplus::StringFormat stringFormat = stringFormat.GenericTypographic();

		if ((dwTextStyle & DT_END_ELLIPSIS) != 0)
		{
			stringFormat.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
		}
		if ((dwTextStyle & DT_PATH_ELLIPSIS) != 0)
		{
			stringFormat.SetTrimming(Gdiplus::StringTrimmingEllipsisPath);
		}

		int formatFlags = 0;
		if ((dwTextStyle & DT_NOCLIP) != 0)
		{
			formatFlags |= Gdiplus::StringFormatFlagsNoClip;
		}
		if ((dwTextStyle & DT_SINGLELINE) != 0)
		{
			formatFlags |= Gdiplus::StringFormatFlagsNoWrap;
		}

		stringFormat.SetFormatFlags(formatFlags);

		if ((dwTextStyle & DT_LEFT) != 0)
		{
			stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);
		}
		else if ((dwTextStyle & DT_CENTER) != 0)
		{
			stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
		}
		else if ((dwTextStyle & DT_RIGHT) != 0)
		{
			stringFormat.SetAlignment(Gdiplus::StringAlignmentFar);
		}
		else
		{
			stringFormat.SetAlignment(Gdiplus::StringAlignmentNear);
		}

		if ((dwTextStyle & DT_TOP) != 0)
		{
			stringFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
		}
		else if ((dwTextStyle & DT_VCENTER) != 0)
		{
			stringFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
		}
		else if ((dwTextStyle & DT_BOTTOM) != 0)
		{
			stringFormat.SetLineAlignment(Gdiplus::StringAlignmentFar);
		}
		else
		{
			stringFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
		}

		if ((dwTextStyle & DT_CALCRECT) != 0)
		{
			Gdiplus::RectF bounds;
			Gp.MeasureString(lpszText, -1, &font, rectF, &stringFormat, &bounds);

			if (bounds.Width < 1)
			{
				if ('\t' == *lpszText)
				{
					bounds.Width = 60;
				}
				else if (' ' == *lpszText)
				{
					bounds.Width = 8;
				}
			}
			if (bCeilFloat)
			{
				rcItem.right = rcItem.left + ceil(bounds.Width);
				rcItem.bottom = rcItem.top + ceil(bounds.Height);
			}
			else
			{
				rcItem.right = rcItem.left + (long)round(bounds.Width);
				rcItem.bottom = rcItem.top + (long)round(bounds.Height);
			}
			if (bShadow)
			{
				rcItem.right += c_nTextShadowRadius + c_nTextShadowOffsetX;
				rcItem.bottom += c_nTextShadowRadius + c_nTextShadowOffsetY;
			}
		}
		else
		{
			BYTE cbTextA = DUIARGBGetA(dwTextColor);
			if (0 == cbTextA) cbTextA = 255;

			if (bShadow)
			{
				DuiDrawBlurredTextShadow(Gp, font, rectF, lpszText, stringFormat, cbTextA);
			}

			Gdiplus::SolidBrush brush(Gdiplus::Color(cbTextA,
				DUIARGBGetR(dwTextColor), DUIARGBGetG(dwTextColor), DUIARGBGetB(dwTextColor)));
			Gp.DrawString(lpszText, -1, &font, rectF, &stringFormat, &brush);
		}

		return;
	}

	//gdi
	int nSaveDC = SaveDC(hDC);
	::SetBkMode(hDC, TRANSPARENT);
	::SetTextColor(hDC, RGB(DUIARGBGetR(dwTextColor), DUIARGBGetG(dwTextColor), DUIARGBGetB(dwTextColor)));
	HFONT hFontOld = (HFONT)::SelectObject(hDC, hFont);
	
	if (bShadow)
	{
		::DrawShadowText(hDC, lpszText, -1, &rcItem, dwTextStyle | DT_NOPREFIX, RGB(DUIARGBGetR(dwTextColor), DUIARGBGetG(dwTextColor), DUIARGBGetB(dwTextColor)), RGB(0, 0, 0), 2, 2);
	}
	else
	{
		::DrawText(hDC, lpszText, -1, &rcItem, dwTextStyle | DT_NOPREFIX);
	}

	::SelectObject(hDC, hFontOld);
	RestoreDC(hDC, nSaveDC);

	//tab width
	if ((dwTextStyle & DT_CALCRECT) != 0)
	{
		if (rcItem.GetWidth() < 1 && '\t' == *lpszText)
		{
			rcItem.right = rcItem.left + 60;
		}
	}

	return;
}

void CDUIRenderEngine::DrawRichText(HDC hDC, CDUIRect &rcItem, const tagDuiRichText &RichText,
	bool bGdiplusRender, Gdiplus::TextRenderingHint RenderType, int nLineSpace, bool bShadow)
{
	ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);
	if (RichText.vecRichTextItem.empty()) return;

	UINT dwTextStyle = RichText;
	if (0 == (DT_CALCRECT & dwTextStyle))
	{
		tagDuiRichText RichTextTemp = RichText;
		RichTextTemp.dwTextStyle |= DT_CALCRECT;

		CDUIRect rcMeasure(rcItem.left, rcItem.top, rcItem.left, rcItem.top);
		MapLineVecDuiRichTextDraw mapLineVecRichTextDraw;
		CDUIRenderHelp::MeasureRichText(hDC, RichTextTemp, mapLineVecRichTextDraw, rcMeasure, rcItem, bGdiplusRender, RenderType, nLineSpace);
		CDUIRenderHelp::AdjustRichText(RichTextTemp, rcItem, mapLineVecRichTextDraw, rcMeasure);

		CDUIRenderEngine::DrawRichText(hDC, rcItem, mapLineVecRichTextDraw, dwTextStyle, bGdiplusRender, RenderType, bShadow);

		return;
	}

	CDUIRect rcMeasure(rcItem.left, rcItem.top, rcItem.left, rcItem.top);
	MapLineVecDuiRichTextDraw mapLineVecRichTextDraw;
	CDUIRenderHelp::MeasureRichText(hDC, RichText, mapLineVecRichTextDraw, rcMeasure, rcItem, bGdiplusRender, RenderType, nLineSpace);

	rcItem = rcMeasure;

	return;
}

void CDUIRenderEngine::DrawRichText(HDC hDC, CDUIRect &rcItem, const MapLineVecDuiRichTextDraw &mapLineVecRichTextDraw, UINT dwTextStyle,
	bool bGdiplusRender, Gdiplus::TextRenderingHint RenderType, bool bShadow)
{
	Gdiplus::ARGB dwColorBk[] = { 0xff000000, 0xffff0000, 0xff00ff00, 0xff0000ff };
	int nColorBk = 0;
	bool bEndEllipsis = (dwTextStyle & DT_END_ELLIPSIS);
	dwTextStyle = dwTextStyle & ~DT_WORDBREAK & ~DT_END_ELLIPSIS | DT_SINGLELINE;
	dwTextStyle = 0 == (dwTextStyle & DT_RIGHT) ? dwTextStyle & ~DT_CENTER | DT_LEFT : dwTextStyle;
	dwTextStyle = 0 == (dwTextStyle & DT_BOTTOM) ? dwTextStyle & ~DT_VCENTER | DT_BOTTOM : dwTextStyle;

	for (int nLine = 0; nLine < mapLineVecRichTextDraw.size(); nLine++)
	{
		auto FindIt = mapLineVecRichTextDraw.find(nLine);
		if (FindIt == mapLineVecRichTextDraw.end()) continue;

		//line verify
		CDUIRect rcLine = CDUIRenderHelp::GetLineRange(mapLineVecRichTextDraw, nLine);
		if (false == ::IntersectRect(&rcLine, &rcLine, &rcItem)) continue;

		//draw line
		auto &vecRichTextDraw = FindIt->second;
		for (int n = 0; n < vecRichTextDraw.size(); n++)
		{
			tagDuiRichTextDraw RichTextDrawItem = vecRichTextDraw[n];
			CDUIRect rcIntersect;
			if (false == ::IntersectRect(&rcIntersect, &RichTextDrawItem.rcDraw, &rcLine)) continue;

			switch (RichTextDrawItem.ItemType)
			{
				case RichTextItem_Text:
				{
					if (n == vecRichTextDraw.size() - 1
						&& mapLineVecRichTextDraw.rbegin()->first == nLine
						&& bEndEllipsis)
					{
						dwTextStyle |= DT_END_ELLIPSIS;
					}

					//DrawColor(hDC, RichTextDraw.rcText, dwColorBk[nColorBk++]);
					HFONT hFont = RichTextDrawItem.pFontBase ? RichTextDrawItem.pFontBase->GetHandle() : NULL;
					DrawText(hDC, hFont, RichTextDrawItem.rcDraw, RichTextDrawItem.strText, RichTextDrawItem.dwColor, dwTextStyle, bGdiplusRender, RenderType, true, bShadow);

					break;
				}
				case RichTextItem_Image:
				{
					CDUIImageBase *pImageBase = CDUIGlobal::GetInstance()->GetImageResource(RichTextDrawItem.strImageResName);
					if (NULL == pImageBase) break;

					tagDuiAnimateImageInfo AnimateImageInfo = pImageBase->GetAnimateImageInfo();
					switch (AnimateImageInfo.AnimateImageType)
					{
						case AnimateImage_Gif:
						{
							Gdiplus::Bitmap *pBmpAnimate = pImageBase->GetAnimateImage();
							if (pBmpAnimate)
							{
								GUID pageGuid = Gdiplus::FrameDimensionTime;
								pBmpAnimate->SelectActiveFrame(&pageGuid, RichTextDrawItem.nAnimateFrameCur);
								CDUIRenderEngine::DrawImage(hDC, pBmpAnimate, RichTextDrawItem.rcDraw);
							}

							break;
						}
						default:
						{
							CDUIRect rcBmpPart(0, 0, pImageBase->GetWidth(), pImageBase->GetHeight());
							CDUIRenderEngine::DrawImage(hDC, pImageBase->GetHandle(), RichTextDrawItem.rcDraw, RichTextDrawItem.rcDraw, rcBmpPart, CDUIRect(), 255, pImageBase->IsAlpha());

							break;
						}
					}

					break;
				}
				default:
					break;
			}
		}
	}

	return;
}

SIZE CDUIRenderEngine::GetTextSize(HDC hDC, HFONT hFont, LPCTSTR lpszText, UINT dwTextStyle)
{
	SIZE size = { 0,0 };
	ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);
	if (MMInvalidString(lpszText) || NULL == hFont) return size;

	int nSaveDC = SaveDC(hDC);
	::SetBkMode(hDC, TRANSPARENT);
	HFONT hOldFont = (HFONT)::SelectObject(hDC, hFont);
	GetTextExtentPoint32(hDC, lpszText, _tcslen(lpszText), &size);
	::SelectObject(hDC, hOldFont);
	RestoreDC(hDC, nSaveDC);

	return size;
}

void CDUIRenderEngine::RGBToHSL(IN Gdiplus::ARGB dwColor, OUT float *pH, OUT float *pS, OUT float *pL)
{
	float R = (float)(DUIARGBGetR(dwColor) / 255.0f);  //RGB from 0 to 255
	float G = (float)(DUIARGBGetG(dwColor) / 255.0f);
	float B = (float)(DUIARGBGetB(dwColor) / 255.0f);

	float fMin = min(R, min(G, B));		//Min. value of RGB
	float fMax = max(R, max(G, B));		//Max. value of RGB
	float fDelta = fMax - fMin;				//Delta RGB value

	float H = 0.0f, S = 0.0f, L = (fMax + fMin) / 2.0f;

	if (fDelta != 0)
	{
		//Chromatic data...
		float del_R, del_G, del_B;

		if (L < 0.5) S = fDelta / (fMax + fMin);
		else S = fDelta / (2.0f - fMax - fMin);

		del_R = (((fMax - R) / 6.0f) + (fDelta / 2.0f)) / fDelta;
		del_G = (((fMax - G) / 6.0f) + (fDelta / 2.0f)) / fDelta;
		del_B = (((fMax - B) / 6.0f) + (fDelta / 2.0f)) / fDelta;

		if (R == fMax) H = del_B - del_G;
		else if (G == fMax) H = (1.0f / 3.0f) + del_R - del_B;
		else if (B == fMax) H = (2.0f / 3.0f) + del_G - del_R;

		if (H < 0.0f) H += 1.0f;
		if (H > 1.0f)  H -= 1.0f;
	}

	*pH = H;
	*pS = S;
	*pL = L;

	return;
}

Gdiplus::ARGB CDUIRenderEngine::HSLToRGB(float H, float S, float L)
{
	static const float OneThird = 1.0f / 3;

	Gdiplus::ARGB dwColor = 0;
	const float
		q = 2 * L < 1 ? L * (1 + S) : (L + S - L * S),
		p = 2 * L - q,
		h = H / 360,
		tr = h + OneThird,
		tg = h,
		tb = h - OneThird,
		ntr = tr < 0 ? tr + 1 : (tr > 1 ? tr - 1 : tr),
		ntg = tg < 0 ? tg + 1 : (tg > 1 ? tg - 1 : tg),
		ntb = tb < 0 ? tb + 1 : (tb > 1 ? tb - 1 : tb),
		R = 255 * (6 * ntr < 1 ? p + (q - p) * 6 * ntr : (2 * ntr < 1 ? q : (3 * ntr < 2 ? p + (q - p) * 6 * (2.0f * OneThird - ntr) : p))),
		G = 255 * (6 * ntg < 1 ? p + (q - p) * 6 * ntg : (2 * ntg < 1 ? q : (3 * ntg < 2 ? p + (q - p) * 6 * (2.0f * OneThird - ntg) : p))),
		B = 255 * (6 * ntb < 1 ? p + (q - p) * 6 * ntb : (2 * ntb < 1 ? q : (3 * ntb < 2 ? p + (q - p) * 6 * (2.0f * OneThird - ntb) : p)));

	BYTE cbR = R < 0 ? 0 : (R > 255 ? 255 : R);
	BYTE cbG = G < 0 ? 0 : (G > 255 ? 255 : G);
	BYTE cbB = B < 0 ? 0 : (B > 255 ? 255 : B);

	return DUIARGB(0xff, cbR, cbG, cbB);
}

Gdiplus::ARGB CDUIRenderEngine::AdjustColor(Gdiplus::ARGB dwColor, short H, short S, short L)
{
	if (H == 180 && S == 100 && L == 100) return dwColor;
	float fH, fS, fL;
	float S1 = S / 100.0f;
	float L1 = L / 100.0f;
	RGBToHSL(dwColor, &fH, &fS, &fL);
	fH += (H - 180);
	fH = fH > 0 ? fH : fH + 360;
	fS *= S1;
	fL *= L1;
	dwColor = HSLToRGB(fH, fS, fL);
	return dwColor;
}

void CDUIRenderEngine::AdjustImage(CDUIImageBase *pImageBase, short H, short S, short L)
{
	if (NULL == pImageBase || NULL == pImageBase->GetHandle()) return;
	if (H == 180 && S == 100 && L == 100) return;

	BITMAP Bmp = {};
	GetObject(pImageBase->GetHandle(), sizeof(Bmp), &Bmp);
	if (NULL == Bmp.bmBits || 32 != Bmp.bmBitsPixel) return;

	float fH, fS, fL;
	float S1 = S / 100.0f;
	float L1 = L / 100.0f;
	for (int i = 0; i < Bmp.bmWidth * Bmp.bmHeight; i++)
	{
		RGBToHSL(*((Gdiplus::ARGB *)Bmp.bmBits + i), &fH, &fS, &fL);
		fH += (H - 180);
		fH = fH > 0 ? fH : fH + 360;
		fS *= S1;
		fL *= L1;
		*((Gdiplus::ARGB *)Bmp.bmBits + i) = HSLToRGB(fH, fS, fL);
	}

	return;
}

void CDUIRenderEngine::AdjustImage(HBITMAP hBitmap, Gdiplus::ARGB dwColorSrc, Gdiplus::ARGB dwColorDest)
{
	if (NULL == hBitmap || dwColorSrc < 0x00ffffff || dwColorSrc == dwColorDest) return;

	BITMAP Bmp = {};
	GetObject(hBitmap, sizeof(Bmp), &Bmp);
	if (NULL == Bmp.bmBits || 32 != Bmp.bmBitsPixel) return;

	for (int nHeight = 0; nHeight < Bmp.bmHeight; nHeight++)
	{
		BYTE *pBits = (BYTE *)Bmp.bmBits + (Bmp.bmWidthBytes * nHeight);
		for (int nWidth = 0; nWidth < Bmp.bmWidth; nWidth++)
		{
			// BGRA pixel is 32-bit. DWORD is unsigned long (8 bytes on LP64) — do not use DWORD*.
			Gdiplus::ARGB &dwColor = *((Gdiplus::ARGB *)pBits + nWidth);
			if (dwColor != dwColorSrc) continue;

			dwColor = dwColorDest;
		}
	}

	return;
}

HBITMAP CDUIRenderEngine::CreateARGB32Bitmap(HDC hDC, int cx, int cy, BYTE **pBits, bool bPositive)
{
	BITMAPINFO BmpSrc = {};
	BmpSrc.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	BmpSrc.bmiHeader.biWidth = cx;
	BmpSrc.bmiHeader.biHeight = bPositive ? -cy : cy;
	BmpSrc.bmiHeader.biPlanes = 1;
	BmpSrc.bmiHeader.biBitCount = 32;
	BmpSrc.bmiHeader.biCompression = BI_RGB;
	BmpSrc.bmiHeader.biSizeImage = cx * cy * sizeof(Gdiplus::ARGB);
	BmpSrc.bmiHeader.biXPelsPerMeter = 0;
	BmpSrc.bmiHeader.biYPelsPerMeter = 0;
	BmpSrc.bmiHeader.biClrUsed = 0;
	BmpSrc.bmiHeader.biClrImportant = 0;

	HBITMAP hBitmap = CreateDIBSection(hDC, &BmpSrc, DIB_RGB_COLORS, (void **)pBits, NULL, NULL);

	return hBitmap;
}

HBITMAP CDUIRenderEngine::GenerateBitmap(CDUIControlBase *pControl, const CDUIRect &rcItem, Gdiplus::ARGB dwFilterColor)
{
	//draw control to bitmap
	HDC hDC = ::GetDC(NULL);
	HDC hDCPaint = ::CreateCompatibleDC(hDC);
	LPBYTE pBmpBits = NULL;
	HBITMAP hBmpPaint = CreateARGB32Bitmap(hDCPaint, rcItem.right, rcItem.bottom, &pBmpBits);
	if (NULL == hBmpPaint)
	{
		MMSafeDeleteDC(hDCPaint);
		ReleaseDC(NULL, hDC);
		return NULL;
	}

	HBITMAP hBmpOld = (HBITMAP)::SelectObject(hDCPaint, hBmpPaint);
	pControl->OnDraw(hDCPaint, rcItem, true);
	AdjustImage(hBmpPaint, dwFilterColor, 0);

	//copy control size bitmap
	HDC hDCClone = ::CreateCompatibleDC(hDCPaint);
	HBITMAP hBitmap = CreateARGB32Bitmap(hDCClone, rcItem.GetWidth(), rcItem.GetHeight(), &pBmpBits);
	if (hBitmap)
	{
		HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hDCClone, hBitmap);
		::BitBlt(hDCClone, 0, 0, rcItem.GetWidth(), rcItem.GetHeight(), hDCPaint, rcItem.left, rcItem.top, SRCCOPY);
		::SelectObject(hDCClone, hOldBitmap);
		MMSafeDeleteDC(hDCClone);
		::GdiFlush();
	}

	// Cleanup
	::SelectObject(hDCPaint, hBmpOld);
	MMSafeDeleteObject(hBmpPaint);
	MMSafeDeleteDC(hDCPaint);
	ReleaseDC(NULL, hDC);

	return hBitmap;
}

Gdiplus::Bitmap * CDUIRenderEngine::GenerateBitmap(const std::vector<BYTE> &vecFileData)
{
	if (vecFileData.empty()) return NULL;

	HGLOBAL hMem = ::GlobalAlloc(GMEM_FIXED, vecFileData.size());
	BYTE *pMem = (BYTE*)::GlobalLock(hMem);
	if (NULL == hMem || NULL == pMem) return NULL;

	CopyMemory(pMem, vecFileData.data(), vecFileData.size());

	IStream *pIStream = NULL;
	::CreateStreamOnHGlobal(hMem, TRUE, &pIStream);
	if (NULL == pIStream)
	{
		::GlobalUnlock(hMem);

		return NULL;
	}

	Gdiplus::Bitmap *pBmp = Gdiplus::Bitmap::FromStream(pIStream);
	pIStream->Release();
	::GlobalUnlock(hMem);

	return pBmp;
}

Gdiplus::Bitmap * CDUIRenderEngine::GenerateEllipseBitmap(Gdiplus::Bitmap *pBmp, const CDUISize &szGenerate)
{
	if (NULL == pBmp) return NULL;

	Gdiplus::Bitmap *pBmpScaled = GenerateThumbnail(pBmp, szGenerate);
	if (NULL == pBmpScaled) return NULL;

	// 临时 DC/位图用于缩放原图到 nScaleWidth x nScaleHeight
	HDC hDC = GetDC(NULL);
	HDC hMemDC = CreateCompatibleDC(hDC);
	HBITMAP hBitmap = CreateCompatibleBitmap(hDC, szGenerate.cx, szGenerate.cy);
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, hBitmap);

	{
		Gdiplus::Graphics gp(hMemDC);
		gp.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		gp.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

		Gdiplus::TextureBrush brush((Gdiplus::Image*)pBmpScaled, Gdiplus::WrapModeClamp);
		
		// 填充椭圆（减1以避免边界像素裁切）
		gp.FillEllipse(&brush, 0, 0, szGenerate.cx - 1, szGenerate.cy - 1);
	}

	// 从 hBitmap3 获取最终带 alpha 的 Bitmap
	Gdiplus::Bitmap *pBmpFinal = GetAlphaBitmap(hBitmap);

	// cleanup GDI objects + 中间 Gdiplus 位图
	SelectObject(hMemDC, hOldBmp);
	DeleteObject(hBitmap);
	DeleteDC(hMemDC);
	ReleaseDC(NULL, hDC);

	MMSafeDelete(pBmpScaled);

	return pBmpFinal;
}

Gdiplus::Bitmap * CDUIRenderEngine::GenerateThumbnail(Gdiplus::Bitmap *pBmp, const CDUISize &szGenerate)
{
	if (pBmp == nullptr || szGenerate.cx <= 0 || szGenerate.cy <= 0) return nullptr;

	const UINT srcW = pBmp->GetWidth();
	const UINT srcH = pBmp->GetHeight();
	if (0 == srcW || 0 == srcH) return nullptr;

	double scaleX = static_cast<double>(szGenerate.cx) / static_cast<double>(srcW);
	double scaleY = static_cast<double>(szGenerate.cy) / static_cast<double>(srcH);
	double scale = (scaleX < scaleY) ? scaleX : scaleY;
	if (scale > 1.0) scale = 1.0;

	const INT destW = static_cast<INT>(srcW * scale);
	const INT destH = static_cast<INT>(srcH * scale);
	if (destW <= 0 || destH <= 0) return nullptr;

	auto pBmpThumb = new Gdiplus::Bitmap(destW, destH, PixelFormat32bppPARGB);
	if (pBmpThumb == nullptr || pBmpThumb->GetLastStatus() != Gdiplus::Ok)
	{
		delete pBmpThumb;
		return nullptr;
	}

	Gdiplus::Graphics g(pBmpThumb);
	g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
	g.DrawImage(pBmp, Gdiplus::Rect(0, 0, destW, destH),
		0, 0, srcW, srcH, Gdiplus::UnitPixel);

	return pBmpThumb;
}

HBITMAP CDUIRenderEngine::CopyBitmap(HDC hDC, const CDUIRect &rcItem, Gdiplus::ARGB dwFilterColor)
{
	LPBYTE pBmpBits = NULL;
	HDC hPaintDC = ::CreateCompatibleDC(hDC);
	HBITMAP hBitmap = CreateARGB32Bitmap(hDC, rcItem.GetWidth(), rcItem.GetHeight(), &pBmpBits);
	ASSERT(hPaintDC);
	ASSERT(hBitmap);
	if (NULL == hPaintDC || NULL == hBitmap)
	{
		int nError = ::GetLastError();
		MMSafeDeleteDC(hPaintDC);
		MMSafeDeleteObject(hBitmap);
		return NULL;
	}

	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hPaintDC, hBitmap);
	::BitBlt(hPaintDC, 0, 0, rcItem.GetWidth(), rcItem.GetHeight(), hDC, rcItem.left, rcItem.top, SRCCOPY);
	AdjustImage(hBitmap, dwFilterColor, 0);

	// Cleanup
	::SelectObject(hPaintDC, hOldBitmap);
	MMSafeDeleteDC(hPaintDC);

	return hBitmap;
}

HBITMAP CDUIRenderEngine::CopyBitmap(HBITMAP hBitmap, Gdiplus::ARGB dwFilterColor, bool bDisablePallete, float fAlpha)
{
	if (NULL == hBitmap) return NULL;

	BITMAP bmp = {};
	if (GetObject(hBitmap, sizeof(BITMAP), &bmp) == 0) return NULL;
	if (bmp.bmWidth <= 0 || bmp.bmHeight <= 0) return NULL;
	if ((long long)bmp.bmWidth * (long long)bmp.bmHeight > 4096LL * 4096LL) return NULL;

	int nLinesize = bmp.bmWidth * 4;
	LONG cbSize = nLinesize * bmp.bmHeight;
	std::vector<BYTE> vecPixel;
	vecPixel.resize(cbSize);
	if (vecPixel.empty()) return NULL;

	LONG cbCopied = 0;
	HDC hDC = GetDC(NULL);
	if (0 == cbCopied)
	{
		BITMAPINFO bmpInfo = {};
		bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmpInfo.bmiHeader.biWidth = bmp.bmWidth;
		bmpInfo.bmiHeader.biHeight = -bmp.bmHeight;
		bmpInfo.bmiHeader.biPlanes = 1;
		bmpInfo.bmiHeader.biBitCount = 32;
		bmpInfo.bmiHeader.biCompression = BI_RGB;
		cbCopied = GetDIBits(hDC, hBitmap, 0, bmp.bmHeight, vecPixel.data(), &bmpInfo, DIB_RGB_COLORS);
	}
	if (0 == cbCopied)
	{
		ReleaseDC(NULL, hDC);
		return NULL;
	}

	LPBYTE pBits = nullptr;
	HBITMAP hBmpClone = CDUIRenderEngine::CreateARGB32Bitmap(hDC, bmp.bmWidth, bmp.bmHeight, &pBits);
	ReleaseDC(NULL, hDC);
	if (NULL == hBmpClone || NULL == pBits) return nullptr;

	BYTE *src = vecPixel.data();
	int nHeight = bmp.bmHeight;
	for (UINT y = 0; y < nHeight; y++)
	{
		memcpy(pBits + y * nLinesize, src + y * nLinesize, nLinesize);

		if (0 == dwFilterColor && false == bDisablePallete && 1.0 == fAlpha) continue;

		BYTE *pBitsLine = pBits + (nLinesize * y);
		for (int nWidth = 0; nWidth < bmp.bmWidth; nWidth++)
		{
			BYTE *pPixel = pBitsLine + (nWidth * 4);
			// BGRA pixel is 32-bit. DWORD is unsigned long (8 bytes on LP64) — do not use DWORD*.
			Gdiplus::ARGB &dwColor = *(Gdiplus::ARGB *)pPixel;
			if (dwColor == dwFilterColor)
			{
				dwColor = 0;

				continue;
			}
			else if (bDisablePallete && pPixel[3] > 0)
			{
				BYTE a = pPixel[3];
				BYTE b, g, r;
				if (a == 255)
				{
					b = pPixel[0];
					g = pPixel[1];
					r = pPixel[2];
				}
				else
				{
					//realColor = storedColor * 255 / alpha
					b = (BYTE)min(255, pPixel[0] * 255 / a);
					g = (BYTE)min(255, pPixel[1] * 255 / a);
					r = (BYTE)min(255, pPixel[2] * 255 / a);
				}

				BYTE gray = (BYTE)(0.299 * r + 0.587 * g + 0.114 * b);

				//premultily alpha
				pPixel[0] = (BYTE)(gray * a / 255); // B
				pPixel[1] = (BYTE)(gray * a / 255); // G
				pPixel[2] = (BYTE)(gray * a / 255); // R
			}
			if (1.0 != fAlpha && pPixel[3] > 0)
			{
				BYTE a = pPixel[3];
				BYTE newAlpha = (BYTE)(a * fAlpha);
				pPixel[0] = (BYTE)(pPixel[0] * newAlpha / a);
				pPixel[1] = (BYTE)(pPixel[1] * newAlpha / a);
				pPixel[2] = (BYTE)(pPixel[2] * newAlpha / a);
				pPixel[3] = newAlpha;
			}
		}
	}

	return hBmpClone;
}

Gdiplus::Bitmap * CDUIRenderEngine::GetAlphaBitmap(HBITMAP hBitmap, bool bPreMultiplyArgb)
{
	if (NULL == hBitmap) return NULL;

	BITMAP bmp = {};
	if (GetObject(hBitmap, sizeof(BITMAP), &bmp) == 0) return NULL;
	if (bmp.bmWidth <= 0 || bmp.bmHeight <= 0) return NULL;
	if ((long long)bmp.bmWidth * (long long)bmp.bmHeight > 4096LL * 4096LL) return NULL;

	int nLinesize = bmp.bmWidth * 4;
	int nHeight = bmp.bmHeight;

	Gdiplus::PixelFormat PixelFormat = bPreMultiplyArgb ? PixelFormat32bppPARGB : PixelFormat32bppARGB;
	Gdiplus::Bitmap *pBitmap = new Gdiplus::Bitmap(bmp.bmWidth, bmp.bmHeight, PixelFormat);
	if (NULL == pBitmap) return NULL;

	Gdiplus::BitmapData bitmapData;
	Gdiplus::Rect rect(0, 0, bmp.bmWidth, bmp.bmHeight);
	if (Gdiplus::Ok != pBitmap->LockBits(&rect, Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeWrite, PixelFormat, &bitmapData))
	{
		MMSafeDelete(pBitmap);
		return NULL;
	}

	BYTE *pixelsDest = (BYTE *)bitmapData.Scan0;

#if defined(DuiPlatform_SDL)
	// SDL HBITMAPs are always top-down (stb / CreateARGB32Bitmap bPositive=true).
	// Copy directly — avoid GetDIBits(+height) bottom-up + flip round-trip.
	LPBYTE pSrcBits = (LPBYTE)bmp.bmBits;
	if (NULL == pSrcBits)
	{
		pBitmap->UnlockBits(&bitmapData);
		MMSafeDelete(pBitmap);
		return NULL;
	}
	for (int y = 0; y < nHeight; y++)
	{
		memcpy(pixelsDest + y * nLinesize, pSrcBits + y * nLinesize, nLinesize);
	}
#else
	LONG cbSize = nLinesize * nHeight;
	std::vector<BYTE> vecPixel;
	vecPixel.resize(cbSize);
	if (vecPixel.empty())
	{
		pBitmap->UnlockBits(&bitmapData);
		MMSafeDelete(pBitmap);
		return NULL;
	}

	BITMAPINFO bmpInfo = {};
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = bmp.bmWidth;
	bmpInfo.bmiHeader.biHeight = bmp.bmHeight;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	HDC hDC = GetDC(NULL);
	LONG cbCopied = GetDIBits(hDC, hBitmap, 0, bmp.bmHeight, vecPixel.data(), &bmpInfo, DIB_RGB_COLORS);
	ReleaseDC(NULL, hDC);

	if (cbCopied == 0)
	{
		pBitmap->UnlockBits(&bitmapData);
		MMSafeDelete(pBitmap);
		return NULL;
	}

	for (int y = 0; y < nHeight; y++)
	{
		// GetDIBits with positive biHeight returns bottom-up; convert to top-down for GDI+.
		memcpy_s(
			(pixelsDest + y * nLinesize),
			nLinesize,
			(vecPixel.data() + (nHeight - y - 1) * nLinesize),
			nLinesize);
	}
#endif

	pBitmap->UnlockBits(&bitmapData);
	return pBitmap;
}

HBITMAP CDUIRenderEngine::GetHBITMAP(Gdiplus::Bitmap *pBmp, bool bKeepAlpha)
{
	if (!pBmp) return nullptr;

	UINT width = pBmp->GetWidth();
	UINT height = pBmp->GetHeight();

	LPBYTE pBits = nullptr;
	HDC hDC = GetDC(NULL);
	HBITMAP hBitmap = CreateARGB32Bitmap(hDC, pBmp->GetWidth(), pBmp->GetHeight(), &pBits);
	ReleaseDC(NULL, hDC);
	if (NULL == hBitmap || NULL == pBits) return nullptr;

	Gdiplus::BitmapData bmpData;
	Gdiplus::Rect rect(0, 0, width, height);
	if (pBmp->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bmpData) != Gdiplus::Ok)
	{
		MMSafeDeleteObject(hBitmap);
		return nullptr;
	}

	BYTE* src = (BYTE*)bmpData.Scan0;
	int stride = width * 4;

	for (UINT y = 0; y < height; y++)
	{
		memcpy(pBits + y * stride, src + y * bmpData.Stride, stride);
	}
	
	pBmp->UnlockBits(&bmpData);

	if (false == bKeepAlpha)
	{
		// 如果不保留 alpha，则把 alpha 通道设为全不透明
		for (UINT i = 0; i < width * height; i++)
		{
			pBits[3] = 255; // alpha
			pBits += 4;
		}
	}

	return hBitmap;
}

bool CDUIRenderEngine::SaveImage(HBITMAP hBitmap, CMMString strFile)
{
	if (NULL == hBitmap || strFile.empty()) return false;

	BITMAP bmp = {};
	if (GetObject(hBitmap, sizeof(BITMAP), &bmp) == 0) return false;
	if (bmp.bmWidth <= 0 || bmp.bmHeight <= 0) return false;

	//info
	int nWidth = bmp.bmWidth;
	int nHeight = bmp.bmHeight;
	LONG cbSize = nWidth * nHeight * 4;
	std::vector<BYTE> vecPixel(cbSize, 0);

	//get pixel data
	BITMAPINFO bmpInfo = {};
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = nWidth;
	bmpInfo.bmiHeader.biHeight = -nHeight;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	HDC hDC = GetDC(NULL);
	LONG cbCopied = GetDIBits(hDC, hBitmap, 0, nHeight, vecPixel.data(), &bmpInfo, DIB_RGB_COLORS);
	ReleaseDC(NULL, hDC);
	if (cbCopied == 0) return false;

	//stb_image_write BGRA->RGBA，restore Alpha premultiply
	for (int i = 0; i < cbSize; i += 4)
	{
		BYTE b = vecPixel[i];
		BYTE g = vecPixel[i + 1];
		BYTE r = vecPixel[i + 2];
		BYTE a = vecPixel[i + 3];

		if (a > 0 && a < 255)
		{
			r = min((r * 255) / a, 255);
			g = min((g * 255) / a, 255);
			b = min((b * 255) / a, 255);
		}

		vecPixel[i]     = r;  // R
		vecPixel[i + 1] = g;  // G
		vecPixel[i + 2] = b;  // B
		vecPixel[i + 3] = a;  // A
	}

	//ext
	CMMString strName, strExt;
	CMMFile::ParseFileName(strFile, strName, strExt);

	//save
	int nRes = 0;
	int nChannels = 4;
	int nStride = nWidth * 4;
	if (0 == strExt.CompareNoCase(_T("bmp")))
	{
		nRes = stbi_write_bmp((LPCSTR)CT2CA(strFile), nWidth, nHeight, nChannels, vecPixel.data());
	}
	else if (0 == strExt.CompareNoCase(_T("jpg")) || 0 == strExt.CompareNoCase(_T("jpeg")))
	{
		nRes = stbi_write_jpg((LPCSTR)CT2CA(strFile), nWidth, nHeight, nChannels, vecPixel.data(), 90);
	}
	else if (0 == strExt.CompareNoCase(_T("tga")))
	{
		nRes = stbi_write_tga((LPCSTR)CT2CA(strFile), nWidth, nHeight, nChannels, vecPixel.data());
	}
	else
	{
		nRes = stbi_write_png((LPCSTR)CT2CA(strFile), nWidth, nHeight, nChannels, vecPixel.data(), nStride);
	}

	return (nRes != 0);
}

bool CDUIRenderEngine::SaveImage(Gdiplus::Bitmap *pBmp, CMMString strFile)
{
	if (NULL == pBmp) return false;

	HBITMAP hBitmap = NULL;
	pBmp->GetHBITMAP(Gdiplus::Color(0, 0, 0, 0), &hBitmap);
	bool bRes = SaveImage(hBitmap, strFile);
	MMSafeDeleteObject(hBitmap);

	return bRes;
}

void CDUIRenderEngine::ClearPixel(LPBYTE pBits, int nWidthBitmap, CDUIRect rcClear)
{
	if (NULL == pBits) return;

	for (int nTop = rcClear.top; nTop < rcClear.bottom; ++nTop)
	{
		memset((Gdiplus::ARGB *)pBits + nWidthBitmap * nTop + rcClear.left, 0, rcClear.GetWidth() * sizeof(Gdiplus::ARGB));
	}

	return;
}

void CDUIRenderEngine::ClearPixelAlpha(LPBYTE pBits, int nWidthBitmap, CDUIRect rcClear)
{
	if (NULL == pBits) return;

	for (int nTop = rcClear.top; nTop < rcClear.bottom; ++nTop)
	{
		for (int nLeft = rcClear.left; nLeft < rcClear.right; ++nLeft)
		{
			Gdiplus::ARGB &wColor = *((Gdiplus::ARGB *)pBits + nWidthBitmap * nTop + nLeft);
			((LPBYTE)&wColor)[3] = 0;
		}
	}

	return;
}

void CDUIRenderEngine::RestorePixelAlpha(LPBYTE pBits, int nWidthBitmap, CDUIRect rcRestore)
{
	if (NULL == pBits) return;

	for (int nTop = rcRestore.top; nTop < rcRestore.bottom; ++nTop)
	{
		for (int nLeft = rcRestore.left; nLeft < rcRestore.right; ++nLeft)
		{
			LPBYTE lpColor = (LPBYTE)((Gdiplus::ARGB *)pBits + nWidthBitmap * nTop + nLeft);
			lpColor[3] = 255;
		}
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
CDUIMemDC::CDUIMemDC(HDC hDC, const CDUIRect rcDest, const CDUIRect rcClient, bool bCopyBitmap)
{
	m_hDC = hDC;
	m_rcDest = rcDest;
	m_rcClient = rcClient;
	m_bCopyBitmap = bCopyBitmap;

	m_hMemDC = ::CreateCompatibleDC(m_hDC);
	m_hBitmap = CDUIRenderEngine::CreateARGB32Bitmap(m_hDC, m_rcClient.GetWidth(), m_rcClient.GetHeight(), &m_pBmpBits);
	m_hOldBitmap = (HBITMAP)::SelectObject(m_hMemDC, m_hBitmap);

	if (m_bCopyBitmap)
	{
		::BitBlt(m_hMemDC, m_rcDest.left, m_rcDest.top, m_rcDest.GetWidth(), m_rcDest.GetHeight(),
			m_hDC, m_rcDest.left, m_rcDest.top, SRCCOPY);
	}

	return;
}

CDUIMemDC::CDUIMemDC(HDC hDC, const CDUIRect rcDest, bool bCopyBitmap)
{
	m_bFromStart = true;
	m_hDC = hDC;
	m_rcDest = rcDest;
	m_bCopyBitmap = bCopyBitmap;

	m_hMemDC = ::CreateCompatibleDC(m_hDC);
	m_hBitmap = CDUIRenderEngine::CreateARGB32Bitmap(m_hDC, m_rcDest.GetWidth(), m_rcDest.GetHeight(), &m_pBmpBits);
	m_hOldBitmap = (HBITMAP)::SelectObject(m_hMemDC, m_hBitmap);

	if (m_bCopyBitmap)
	{
		::BitBlt(m_hMemDC, 0, 0, m_rcDest.GetWidth(), m_rcDest.GetHeight(),
			m_hDC, m_rcDest.left, m_rcDest.top, SRCCOPY);
	}

	return;
}

CDUIMemDC::~CDUIMemDC(void)
{
	if (m_hMemDC)
	{
		if (false == m_bFromStart)
		{
			::BitBlt(m_hDC, m_rcDest.left, m_rcDest.top, m_rcDest.GetWidth(), m_rcDest.GetHeight(),
				m_hMemDC, m_rcDest.left, m_rcDest.top, SRCCOPY);
		}
		else
		{
			::BitBlt(m_hDC, m_rcDest.left, m_rcDest.top, m_rcDest.GetWidth(), m_rcDest.GetHeight(),
				m_hMemDC, 0, 0, SRCCOPY);
		}
	}

	ResetDC();

	return;
}

CDUIMemDC::operator HDC() const
{
	return m_hMemDC;
}

void CDUIMemDC::ResetDC()
{
	if (m_hMemDC)
	{
		SelectObject(m_hMemDC, m_hOldBitmap);
	}

	MMSafeDeleteObject(m_hBitmap);
	MMSafeDeleteDC(m_hMemDC);
	m_pBmpBits = NULL;

	return;
}

HDC CDUIMemDC::GetSrcHDC()
{
	return m_hDC;
}

HDC CDUIMemDC::GetMemHDC()
{
	return m_hMemDC;
}

HBITMAP CDUIMemDC::GetMemBitmap()
{
	return m_hBitmap;
}

LPBYTE CDUIMemDC::GetMemBmpBits()
{
	return m_pBmpBits;
}

CDUIRect CDUIMemDC::GetPaint()
{
	return m_rcDest;
}

CDUIRect CDUIMemDC::GetClient()
{
	return m_rcClient;
}

const CDUIMemDC &CDUIMemDC::operator = (CDUIMemDC)
{
	return *this;
}

//////////////////////////////////////////////////////////////////////////
