#include "StdAfx.h"
#include "DUIGdiplusPortable.h"
#include "DUICanvasRaster.h"

#if defined(DuiPlatform_SDL)

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace Gdiplus
{
//////////////////////////////////////////////////////////////////////////
TextureBrush::TextureBrush(Image *pImage, WrapMode)
	: m_pBmp(dynamic_cast<Bitmap *>(pImage))
	, m_bOwned(false)
{
}

Brush * TextureBrush::Clone() const
{
	TextureBrush *p = new TextureBrush(m_pBmp, WrapModeClamp);
	p->m_matrix = m_matrix;
	return p;
}

//////////////////////////////////////////////////////////////////////////
Bitmap::Bitmap(int nWidth, int nHeight, PixelFormat fmt)
{
	m_format = fmt;
	Reset(nWidth, nHeight);
}

Bitmap::Bitmap(int nWidth, int nHeight, const BYTE *pBGRA)
{
	Reset(nWidth, nHeight);
	if (pBGRA && false == m_vecBits.empty())
	{
		memcpy(m_vecBits.data(), pBGRA, m_vecBits.size());
	}
}

void Bitmap::Reset(int nWidth, int nHeight)
{
	m_nWidth = max(0, nWidth);
	m_nHeight = max(0, nHeight);
	m_vecBits.assign((size_t)m_nWidth * (size_t)m_nHeight * 4, 0);
}

Status Bitmap::GetHBITMAP(const Color &, HBITMAP *hbmReturn)
{
	if (NULL == hbmReturn) return InvalidParameter;
	*hbmReturn = NULL;
	if (m_nWidth <= 0 || m_nHeight <= 0 || m_vecBits.empty()) return GenericError;

	CDUIImageRaster *pImage = new CDUIImageRaster(m_nWidth, m_nHeight, true);
	if (NULL == pImage || NULL == pImage->GetBits())
	{
		delete pImage;
		return OutOfMemory;
	}
	memcpy(pImage->GetBits(), m_vecBits.data(), m_vecBits.size());
	*hbmReturn = (HBITMAP)pImage;
	return Ok;
}

Bitmap * Bitmap::Clone(INT x, INT y, INT nWidth, INT nHeight, INT) const
{
	if (nWidth <= 0 || nHeight <= 0 || x < 0 || y < 0) return NULL;
	if (x + nWidth > m_nWidth || y + nHeight > m_nHeight) return NULL;
	Bitmap *pClone = new Bitmap(nWidth, nHeight, m_format);
	for (int row = 0; row < nHeight; ++row)
	{
		memcpy(pClone->GetBits() + row * nWidth * 4,
			m_vecBits.data() + ((y + row) * m_nWidth + x) * 4,
			(size_t)nWidth * 4);
	}
	return pClone;
}

Status Bitmap::LockBits(const Rect *rect, UINT, PixelFormat format, BitmapData *lockedBitmapData)
{
	if (NULL == lockedBitmapData || m_vecBits.empty()) return InvalidParameter;
	(void)rect;
	lockedBitmapData->Width = (UINT)m_nWidth;
	lockedBitmapData->Height = (UINT)m_nHeight;
	lockedBitmapData->Stride = m_nWidth * 4;
	lockedBitmapData->PixelFormat = format ? format : m_format;
	lockedBitmapData->Scan0 = GetBits();
	return Ok;
}

//////////////////////////////////////////////////////////////////////////
void GraphicsPath::AddLine(INT x1, INT y1, INT x2, INT y2)
{
	POINT a = { x1, y1 }, b = { x2, y2 };
	if (m_pts.empty()) m_pts.push_back(a);
	else if (m_pts.back().x != a.x || m_pts.back().y != a.y) m_pts.push_back(a);
	m_pts.push_back(b);
}

void GraphicsPath::AddArc(INT x, INT y, INT w, INT h, REAL startAngle, REAL sweepAngle)
{
	if (w <= 0 || h <= 0) return;
	const int nSeg = max(8, (int)(fabsf(sweepAngle) / 6.0f));
	const REAL cx = x + w * 0.5f;
	const REAL cy = y + h * 0.5f;
	const REAL rx = w * 0.5f;
	const REAL ry = h * 0.5f;
	for (int i = 0; i <= nSeg; ++i)
	{
		REAL t = startAngle + sweepAngle * (REAL)i / (REAL)nSeg;
		REAL rad = t * (REAL)M_PI / 180.0f;
		POINT pt = { (LONG)(cx + rx * cosf(rad) + 0.5f), (LONG)(cy + ry * sinf(rad) + 0.5f) };
		if (m_pts.empty() || m_pts.back().x != pt.x || m_pts.back().y != pt.y)
			m_pts.push_back(pt);
	}
}

void GraphicsPath::AddEllipse(INT x, INT y, INT w, INT h)
{
	AddArc(x, y, w, h, 0, 360);
	m_closed = true;
}

void GraphicsPath::AddBezier(REAL x1, REAL y1, REAL x2, REAL y2, REAL x3, REAL y3, REAL x4, REAL y4)
{
	const int nSeg = 12;
	for (int i = 0; i <= nSeg; ++i)
	{
		REAL t = (REAL)i / (REAL)nSeg;
		REAL u = 1.0f - t;
		REAL x = u * u * u * x1 + 3 * u * u * t * x2 + 3 * u * t * t * x3 + t * t * t * x4;
		REAL y = u * u * u * y1 + 3 * u * u * t * y2 + 3 * u * t * t * y3 + t * t * t * y4;
		POINT pt = { (LONG)(x + 0.5f), (LONG)(y + 0.5f) };
		if (m_pts.empty() || m_pts.back().x != pt.x || m_pts.back().y != pt.y)
			m_pts.push_back(pt);
	}
}

Status GraphicsPath::GetBounds(RectF *bounds) const
{
	if (NULL == bounds) return InvalidParameter;
	if (m_pts.empty()) { *bounds = RectF(); return Ok; }
	INT minX = m_pts[0].x, maxX = m_pts[0].x, minY = m_pts[0].y, maxY = m_pts[0].y;
	for (size_t i = 1; i < m_pts.size(); ++i)
	{
		minX = min(minX, (INT)m_pts[i].x);
		maxX = max(maxX, (INT)m_pts[i].x);
		minY = min(minY, (INT)m_pts[i].y);
		maxY = max(maxY, (INT)m_pts[i].y);
	}
	*bounds = RectF((REAL)minX, (REAL)minY, (REAL)(maxX - minX), (REAL)(maxY - minY));
	return Ok;
}

//////////////////////////////////////////////////////////////////////////
Graphics::Graphics(HDC hdc)
	: m_pCanvas(DuiCanvasFromHDC(hdc))
	, m_bOwnCanvas(false)
{
}

Graphics::Graphics(Bitmap *bmp)
	: m_pCanvas(NULL)
	, m_bOwnCanvas(false)
{
	if (NULL == bmp || NULL == bmp->GetBits()) return;
	CDUICanvasRaster *pCanvas = new CDUICanvasRaster(1, 1);
	if (pCanvas->AttachBits(bmp->GetBits(), (int)bmp->GetWidth(), (int)bmp->GetHeight()))
	{
		m_pCanvas = pCanvas;
		m_bOwnCanvas = true;
	}
	else
	{
		delete pCanvas;
	}
}

Graphics::~Graphics()
{
	if (m_bOwnCanvas)
	{
		delete m_pCanvas;
		m_pCanvas = NULL;
		m_bOwnCanvas = false;
	}
}

Status Graphics::DrawLine(Pen *pen, INT x1, INT y1, INT x2, INT y2)
{
	if (NULL == m_pCanvas || NULL == pen) return InvalidParameter;
	m_pCanvas->DrawLine(x1, y1, x2, y2, PenWidth(pen), PenColor(pen), pen->GetStyle());
	return Ok;
}

Status Graphics::DrawRectangle(Pen *pen, INT x, INT y, INT w, INT h)
{
	if (NULL == m_pCanvas || NULL == pen) return InvalidParameter;
	RECT rc = { x, y, x + w, y + h };
	SIZE sz = {};
	m_pCanvas->DrawRect(rc, PenWidth(pen), PenColor(pen), sz, pen->GetStyle());
	return Ok;
}

Status Graphics::DrawEllipse(Pen *pen, INT x, INT y, INT w, INT h)
{
	if (NULL == m_pCanvas || NULL == pen) return InvalidParameter;
	RECT rc = { x, y, x + w, y + h };
	m_pCanvas->DrawEllipse(rc, PenWidth(pen), PenColor(pen), pen->GetStyle());
	return Ok;
}

Status Graphics::DrawArc(Pen *pen, INT x, INT y, INT w, INT h, REAL startAngle, REAL sweepAngle)
{
	if (NULL == m_pCanvas || NULL == pen) return InvalidParameter;
	RECT rc = { x, y, x + w, y + h };
	m_pCanvas->DrawArc(rc, PenWidth(pen), PenColor(pen), startAngle, sweepAngle, pen->GetStyle());
	return Ok;
}

Status Graphics::DrawPath(Pen *pen, GraphicsPath *path)
{
	if (NULL == m_pCanvas || NULL == pen || NULL == path || path->Points().empty()) return InvalidParameter;
	m_pCanvas->DrawPath(path->Points().data(), (int)path->Points().size(), PenWidth(pen), PenColor(pen), pen->GetStyle());
	return Ok;
}

Status Graphics::FillRectangle(Brush *brush, INT x, INT y, INT w, INT h)
{
	if (NULL == m_pCanvas || NULL == brush) return InvalidParameter;
	RECT rc = { x, y, x + w, y + h };
	Bitmap *pBmp = brush->GetBitmap();
	if (pBmp && pBmp->GetBits())
	{
		RECT rcSrc = { 0, 0, (LONG)pBmp->GetWidth(), (LONG)pBmp->GetHeight() };
		m_pCanvas->DrawImage(pBmp->GetBits(), (int)pBmp->GetWidth(), (int)pBmp->GetHeight(), rc, rcSrc, {}, 255, false, false);
	}
	else
	{
		m_pCanvas->FillRect(rc, brush->GetFillColor(), brush->IsGradient() ? brush->GetGradientColor() : 0);
	}
	return Ok;
}

Status Graphics::FillEllipse(Brush *brush, INT x, INT y, INT w, INT h)
{
	if (NULL == m_pCanvas || NULL == brush) return InvalidParameter;
	RECT rc = { x, y, x + w, y + h };
	m_pCanvas->FillEllipse(rc, brush->GetFillColor(), brush->IsGradient() ? brush->GetGradientColor() : 0);
	return Ok;
}

Status Graphics::FillPath(Brush *brush, GraphicsPath *path)
{
	if (NULL == m_pCanvas || NULL == brush || NULL == path || path->Points().empty()) return InvalidParameter;
	Bitmap *pBmp = brush->GetBitmap();
	if (pBmp && pBmp->GetBits())
	{
		RectF bounds;
		path->GetBounds(&bounds);
		RECT rcDst = { (LONG)bounds.X, (LONG)bounds.Y, (LONG)(bounds.X + bounds.Width), (LONG)(bounds.Y + bounds.Height) };
		RECT rcSrc = { 0, 0, (LONG)pBmp->GetWidth(), (LONG)pBmp->GetHeight() };
		m_pCanvas->Save();
		m_pCanvas->DrawImage(pBmp->GetBits(), (int)pBmp->GetWidth(), (int)pBmp->GetHeight(), rcDst, rcSrc, {}, 255, false, false);
		m_pCanvas->Restore();
	}
	else
	{
		m_pCanvas->FillPolygon(path->Points().data(), (int)path->Points().size(),
			brush->GetFillColor(), brush->IsGradient() ? brush->GetGradientColor() : 0);
	}
	return Ok;
}

bool Graphics::IsWorldIdentity() const
{
	return 1.0f == m_m11 && 0.0f == m_m12 && 0.0f == m_m21 && 1.0f == m_m22 && 0.0f == m_dx && 0.0f == m_dy;
}

void Graphics::MultiplyWorld(float n11, float n12, float n21, float n22, float ndx, float ndy, MatrixOrder order)
{
	const float w11 = m_m11, w12 = m_m12, w21 = m_m21, w22 = m_m22, wdx = m_dx, wdy = m_dy;
	const float a11 = (MatrixOrderAppend == order) ? w11 : n11;
	const float a12 = (MatrixOrderAppend == order) ? w12 : n12;
	const float a21 = (MatrixOrderAppend == order) ? w21 : n21;
	const float a22 = (MatrixOrderAppend == order) ? w22 : n22;
	const float adx = (MatrixOrderAppend == order) ? wdx : ndx;
	const float ady = (MatrixOrderAppend == order) ? wdy : ndy;
	const float b11 = (MatrixOrderAppend == order) ? n11 : w11;
	const float b12 = (MatrixOrderAppend == order) ? n12 : w12;
	const float b21 = (MatrixOrderAppend == order) ? n21 : w21;
	const float b22 = (MatrixOrderAppend == order) ? n22 : w22;
	const float bdx = (MatrixOrderAppend == order) ? ndx : wdx;
	const float bdy = (MatrixOrderAppend == order) ? ndy : wdy;
	m_m11 = a11 * b11 + a12 * b21;
	m_m12 = a11 * b12 + a12 * b22;
	m_m21 = a21 * b11 + a22 * b21;
	m_m22 = a21 * b12 + a22 * b22;
	m_dx = adx * b11 + ady * b21 + bdx;
	m_dy = adx * b12 + ady * b22 + bdy;
}

void Graphics::TransformPoint(float x, float y, float &ox, float &oy) const
{
	ox = x * m_m11 + y * m_m21 + m_dx;
	oy = x * m_m12 + y * m_m22 + m_dy;
}

Status Graphics::TranslateTransform(REAL dx, REAL dy, MatrixOrder order)
{
	MultiplyWorld(1.0f, 0.0f, 0.0f, 1.0f, dx, dy, order);
	return Ok;
}

Status Graphics::RotateTransform(REAL angle, MatrixOrder order)
{
	const float fRad = angle * (float)M_PI / 180.0f;
	const float fCos = cosf(fRad);
	const float fSin = sinf(fRad);
	MultiplyWorld(fCos, fSin, -fSin, fCos, 0.0f, 0.0f, order);
	return Ok;
}

Status Graphics::BlitBitmap(Bitmap *bmp, int x, int y, int w, int h, int srcX, int srcY, int srcW, int srcH)
{
	if (NULL == m_pCanvas || NULL == bmp || NULL == bmp->GetBits()) return InvalidParameter;
	RECT rcDst = { x, y, x + w, y + h };
	RECT rcSrc = { srcX, srcY, srcX + srcW, srcY + srcH };
	m_pCanvas->DrawImage(bmp->GetBits(), (int)bmp->GetWidth(), (int)bmp->GetHeight(), rcDst, rcSrc, {}, 255, false, false);
	return Ok;
}

Status Graphics::DrawImage(Bitmap *bmp, const Rect &dest, INT srcX, INT srcY, INT srcW, INT srcH, Unit)
{
	if (NULL == m_pCanvas || NULL == bmp || NULL == bmp->GetBits()) return InvalidParameter;
	if (IsWorldIdentity())
	{
		return BlitBitmap(bmp, dest.X, dest.Y, dest.Width, dest.Height, srcX, srcY, srcW, srcH);
	}

	float x0 = 0, y0 = 0, x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	TransformPoint((float)dest.X, (float)dest.Y, x0, y0);
	TransformPoint((float)(dest.X + dest.Width), (float)dest.Y, x1, y1);
	TransformPoint((float)dest.X, (float)(dest.Y + dest.Height), x2, y2);
	PointF pts[3] = { PointF(x0, y0), PointF(x1, y1), PointF(x2, y2) };
	return DrawImageWarp(bmp, pts, srcX, srcY, srcW, srcH);
}

static void DuiSamplePremul(const BYTE *pBase, int nStride, int nSrcW, int nSrcH, float u, float v, BYTE cbOut[4])
{
	if (u < 0.0f || v < 0.0f || u >= (float)nSrcW || v >= (float)nSrcH)
	{
		cbOut[0] = cbOut[1] = cbOut[2] = cbOut[3] = 0;
		return;
	}

	const int x0 = (int)u;
	const int y0 = (int)v;
	const int x1 = min(x0 + 1, nSrcW - 1);
	const int y1 = min(y0 + 1, nSrcH - 1);
	const float fx = u - (float)x0;
	const float fy = v - (float)y0;
	const BYTE *p00 = pBase + y0 * nStride + x0 * 4;
	const BYTE *p10 = pBase + y0 * nStride + x1 * 4;
	const BYTE *p01 = pBase + y1 * nStride + x0 * 4;
	const BYTE *p11 = pBase + y1 * nStride + x1 * 4;
	for (int c = 0; c < 4; ++c)
	{
		const float fSample = p00[c] * (1.0f - fx) * (1.0f - fy)
			+ p10[c] * fx * (1.0f - fy)
			+ p01[c] * (1.0f - fx) * fy
			+ p11[c] * fx * fy;
		cbOut[c] = (BYTE)(fSample + 0.5f);
	}
}

Status Graphics::DrawImage(Bitmap *bmp, PointF *pts, INT count)
{
	if (NULL == m_pCanvas || NULL == bmp || NULL == pts || count < 3) return InvalidParameter;

	PointF wpts[3];
	for (int n = 0; n < 3; ++n)
	{
		float x = 0, y = 0;
		TransformPoint(pts[n].X, pts[n].Y, x, y);
		wpts[n] = PointF(x, y);
	}

	const float v1x = wpts[1].X - wpts[0].X;
	const float v1y = wpts[1].Y - wpts[0].Y;
	const float v2x = wpts[2].X - wpts[0].X;
	const float v2y = wpts[2].Y - wpts[0].Y;
	if (0.0f == v1y && 0.0f == v2x && v1x > 0.0f && v2y > 0.0f)
	{
		return BlitBitmap(bmp, (int)floorf(wpts[0].X), (int)floorf(wpts[0].Y),
			max(1, (int)ceilf(v1x)), max(1, (int)ceilf(v2y)),
			0, 0, (int)bmp->GetWidth(), (int)bmp->GetHeight());
	}

	return DrawImageWarp(bmp, wpts, 0, 0, (int)bmp->GetWidth(), (int)bmp->GetHeight());
}

Status Graphics::DrawImageWarp(Bitmap *bmp, const PointF *pts, int srcX, int srcY, int srcW, int srcH)
{
	if (NULL == m_pCanvas || NULL == bmp || NULL == pts) return InvalidParameter;
	if (srcW <= 0 || srcH <= 0) return InvalidParameter;

	const int nBmpSrcW = (int)bmp->GetWidth();
	const int nBmpSrcH = (int)bmp->GetHeight();
	if (nBmpSrcW <= 0 || nBmpSrcH <= 0) return InvalidParameter;

	const float x0 = pts[0].X;
	const float y0 = pts[0].Y;
	const float v1x = pts[1].X - x0;
	const float v1y = pts[1].Y - y0;
	const float v2x = pts[2].X - x0;
	const float v2y = pts[2].Y - y0;
	const float fDet = v1x * v2y - v1y * v2x;
	if (fabsf(fDet) < 0.0001f) return InvalidParameter;

	const float x3 = pts[1].X + v2x;
	const float y3 = pts[1].Y + v2y;
	const float fMinX = min(min(x0, pts[1].X), min(pts[2].X, x3));
	const float fMinY = min(min(y0, pts[1].Y), min(pts[2].Y, y3));
	const float fMaxX = max(max(x0, pts[1].X), max(pts[2].X, x3));
	const float fMaxY = max(max(y0, pts[1].Y), max(pts[2].Y, y3));
	const int nLeft = (int)floorf(fMinX);
	const int nTop = (int)floorf(fMinY);
	const int nBmpW = (int)ceilf(fMaxX) - nLeft;
	const int nBmpH = (int)ceilf(fMaxY) - nTop;
	if (nBmpW <= 0 || nBmpH <= 0) return InvalidParameter;

	BitmapData srcData = {};
	Rect rcSrc(0, 0, nBmpSrcW, nBmpSrcH);
	if (Ok != bmp->LockBits(&rcSrc, ImageLockModeRead, PixelFormat32bppPARGB, &srcData) || NULL == srcData.Scan0)
	{
		return GenericError;
	}

	Bitmap bmpWarp(nBmpW, nBmpH, PixelFormat32bppPARGB);
	BitmapData dstData = {};
	Rect rcDstLock(0, 0, nBmpW, nBmpH);
	if (Ok != bmpWarp.GetLastStatus()
		|| Ok != bmpWarp.LockBits(&rcDstLock, ImageLockModeWrite, PixelFormat32bppPARGB, &dstData)
		|| NULL == dstData.Scan0)
	{
		if (NULL != dstData.Scan0) bmpWarp.UnlockBits(&dstData);
		bmp->UnlockBits(&srcData);
		return GenericError;
	}

	const float fInvDet = 1.0f / fDet;
	const BYTE *pSrc = (const BYTE *)srcData.Scan0;
	for (int y = 0; y < nBmpH; ++y)
	{
		BYTE *pRow = (BYTE *)dstData.Scan0 + y * dstData.Stride;
		const float dy = (nTop + y + 0.5f) - y0;
		for (int x = 0; x < nBmpW; ++x)
		{
			const float dx = (nLeft + x + 0.5f) - x0;
			float s = (dx * v2y - dy * v2x) * fInvDet;
			float t = (v1x * dy - v1y * dx) * fInvDet;
			BYTE cbPixel[4] = {};
			if (s >= 0.0f && t >= 0.0f && s <= 1.0f && t <= 1.0f)
			{
				if (s >= 1.0f) s = 0.9999f;
				if (t >= 1.0f) t = 0.9999f;
				DuiSamplePremul(pSrc, srcData.Stride, nBmpSrcW, nBmpSrcH,
					(float)srcX + s * (float)srcW, (float)srcY + t * (float)srcH, cbPixel);
			}
			BYTE *pDst = pRow + x * 4;
			pDst[0] = cbPixel[0];
			pDst[1] = cbPixel[1];
			pDst[2] = cbPixel[2];
			pDst[3] = cbPixel[3];
		}
	}

	bmpWarp.UnlockBits(&dstData);
	bmp->UnlockBits(&srcData);

	return BlitBitmap(&bmpWarp, nLeft, nTop, nBmpW, nBmpH, 0, 0, nBmpW, nBmpH);
}

Status Graphics::MeasureString(const WCHAR *str, INT len, const Font *font, const RectF &layout, const StringFormat *fmt, RectF *boundingBox)
{
	if (NULL == boundingBox) return InvalidParameter;
	IDuiFont *pFont = font ? DuiFontFromHFONT(font->GetHFONT()) : NULL;
	CMMString text;
	if (str)
	{
		if (len < 0) text = str;
		else text = CMMString(str, len);
	}
	DWORD dwStyle = DT_LEFT | DT_TOP | DT_WORDBREAK;
	if (fmt)
	{
		if (fmt->GetAlignment() == StringAlignmentCenter) dwStyle = (dwStyle & ~DT_LEFT) | DT_CENTER;
		if (fmt->GetAlignment() == StringAlignmentFar) dwStyle = (dwStyle & ~DT_LEFT) | DT_RIGHT;
		if (fmt->GetLineAlignment() == StringAlignmentCenter) dwStyle |= DT_VCENTER;
		if (fmt->GetLineAlignment() == StringAlignmentFar) dwStyle |= DT_BOTTOM;
		if (fmt->GetFormatFlags() & StringFormatFlagsNoWrap) dwStyle = (dwStyle & ~DT_WORDBREAK) | DT_SINGLELINE;
	}
	SIZE sz = {};
	if (m_pCanvas && pFont) sz = m_pCanvas->MeasureText(pFont, text.c_str(), dwStyle, (int)layout.Width);
	else if (pFont) sz = pFont->MeasureText(text.c_str(), (int)layout.Width, dwStyle);
	boundingBox->X = layout.X;
	boundingBox->Y = layout.Y;
	boundingBox->Width = (REAL)sz.cx;
	boundingBox->Height = (REAL)sz.cy;
	return Ok;
}

Status Graphics::DrawString(const WCHAR *str, INT len, const Font *font, const RectF &layout, const StringFormat *fmt, Brush *brush)
{
	if (NULL == m_pCanvas || NULL == brush) return InvalidParameter;
	IDuiFont *pFont = font ? DuiFontFromHFONT(font->GetHFONT()) : NULL;
	if (NULL == pFont) return GenericError;
	CMMString text;
	if (str)
	{
		if (len < 0) text = str;
		else text = CMMString(str, len);
	}
	RECT rc = { (LONG)layout.X, (LONG)layout.Y, (LONG)(layout.X + layout.Width), (LONG)(layout.Y + layout.Height) };
	DWORD dwStyle = DT_LEFT | DT_TOP | DT_WORDBREAK;
	if (fmt)
	{
		if (fmt->GetAlignment() == StringAlignmentCenter) dwStyle = (dwStyle & ~DT_LEFT) | DT_CENTER;
		if (fmt->GetAlignment() == StringAlignmentFar) dwStyle = (dwStyle & ~DT_LEFT) | DT_RIGHT;
		if (fmt->GetLineAlignment() == StringAlignmentCenter) dwStyle |= DT_VCENTER;
		if (fmt->GetLineAlignment() == StringAlignmentFar) dwStyle |= DT_BOTTOM;
		if (fmt->GetFormatFlags() & StringFormatFlagsNoWrap) dwStyle = (dwStyle & ~DT_WORDBREAK) | DT_SINGLELINE;
	}
	m_pCanvas->DrawText(pFont, rc, text.c_str(), brush->GetFillColor(), dwStyle);
	return Ok;
}

} // namespace Gdiplus

#endif
