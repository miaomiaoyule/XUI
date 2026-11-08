#include "StdAfx.h"
#include "DUIAnimateCtrl.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateControl(CDUIAnimateCtrl)
MMImplement_ClassName(CDUIAnimateCtrl)

CDUIAnimateCtrl::CDUIAnimateCtrl(void)
{
}

CDUIAnimateCtrl::~CDUIAnimateCtrl(void)
{
	return;
}

bool CDUIAnimateCtrl::OnAttributeChange(CDUIAttributeObject *pAttributeObj)
{
	if (__super::OnAttributeChange(pAttributeObj)) return true;

	if (pAttributeObj == &m_AttributeImageAnimate)
	{
		SetImageSectionAnimate(GetImageSectionAnimate());

		return true;
	}
	if (pAttributeObj == &m_AttributeSequenceFrameSize)
	{
		SetSequenceFrameSize(GetSequenceFrameSize());

		return true;
	}
	if (pAttributeObj == &m_AttributeSequenceFrameSpeed)
	{
		SetSequenceFrameSpeed(GetSequenceFrameSpeed());

		return true;
	}

	return false;
}

void CDUIAnimateCtrl::OnDpiChanged(int nScalePre)
{
	__super::OnDpiChanged(nScalePre);

	ConstructAnimateImageInfo();

	return;
}

LPVOID CDUIAnimateCtrl::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIAnimateCtrl, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CMMString CDUIAnimateCtrl::GetDescribe() const
{
	return Dui_Ctrl_Animate;
}

void CDUIAnimateCtrl::SetVisible(bool bVisible)
{
	__super::SetVisible(bVisible);

	if (IsVisible() && IsAutoPlay())
	{
		PlayAnimate();

		return;
	}

	StopAnimate();

	return;
}

tagDuiImageSection CDUIAnimateCtrl::GetImageSectionAnimate()
{
	return m_AttributeImageAnimate.GetImageSection();
}

void CDUIAnimateCtrl::SetImageSectionAnimate(const tagDuiImageSection &ImageSection)
{
	m_AttributeImageAnimate.SetImageSection(ImageSection);

	StopAnimate();

	ConstructAnimateImageInfo();

	//play
	if (IsAutoPlay())
	{
		PlayAnimate();
	}

	Invalidate();

	return;
}

bool CDUIAnimateCtrl::IsAutoPlay()
{
	return m_AttributeAutoPlay.GetValue();
}

void CDUIAnimateCtrl::SetAutoPlay(bool bAutoPlay)
{
	if (bAutoPlay == IsAutoPlay()) return;

	m_AttributeAutoPlay.SetValue(bAutoPlay);

	return;
}

int CDUIAnimateCtrl::GetFrameCount()
{
	return m_AnimateImageInfo.nFrameCount;
}

CDUISize CDUIAnimateCtrl::GetSequenceFrameSize()
{
	return m_AttributeSequenceFrameSize.GetValue();
}

void CDUIAnimateCtrl::SetSequenceFrameSize(CDUISize szItem)
{
#ifndef DUI_DESIGN
	if (szItem == GetSequenceFrameSize()) return;
#endif

	m_AttributeSequenceFrameSize.SetValue(szItem);

	ConstructAnimateImageInfo();

	//play
	if (IsAutoPlay())
	{
		PlayAnimate();
	}

	return;
}

int CDUIAnimateCtrl::GetSequenceFrameSpeed()
{
	return m_AttributeSequenceFrameSpeed.GetValue();
}

void CDUIAnimateCtrl::SetSequenceFrameSpeed(int nSpeed)
{
#ifndef DUI_DESIGN
	if (nSpeed == GetSequenceFrameSpeed()) return;
#endif

	m_AttributeSequenceFrameSpeed.SetValue(nSpeed);

	ConstructAnimateImageInfo();

	//play
	if (IsAutoPlay())
	{
		PlayAnimate();
	}

	return;
}

void CDUIAnimateCtrl::PlayAnimate()
{
	PlayCurFrame();

	return;
}

void CDUIAnimateCtrl::PauseAnimate()
{
	KillTimer(Dui_TimerAnimate_ID);

	Invalidate();

	return;
}

void CDUIAnimateCtrl::StopAnimate()
{
	KillTimer(Dui_TimerAnimate_ID);

	m_nFrameCur = 0;

	Invalidate();

	return;
}

void CDUIAnimateCtrl::OnDuiWndManagerAttach()
{
	__super::OnDuiWndManagerAttach();

	//animate info
	ConstructAnimateImageInfo();

	//play
	if (IsAutoPlay())
	{
		PlayAnimate();
	}

	return;
}

void CDUIAnimateCtrl::OnDuiTimer(UINT uTimerID, const DuiMessage &Msg)
{
	__super::OnDuiTimer(uTimerID, Msg);

	if (Dui_TimerAnimate_ID != uTimerID || m_AnimateImageInfo.nFrameCount <= 0) return;

	Invalidate();

	//next
	m_nFrameCur = (++m_nFrameCur) % m_AnimateImageInfo.nFrameCount;

	PlayCurFrame();

	return;
}

void CDUIAnimateCtrl::InitProperty()
{
	__super::InitProperty();

	DuiCreateGroupAttribute(m_AttributeGroupAnimation, _T("Animation"));
	DuiCreateAttribute(m_AttributeImageAnimate, _T("ImageAnimate"), _T(""), m_AttributeGroupAnimation);
	DuiCreateAttribute(m_AttributeSequenceFrameSize, _T("SequenceFrameSize"), _T(""), m_AttributeGroupAnimation);
	DuiCreateAttribute(m_AttributeSequenceFrameSpeed, _T("SequenceFrameSpeed"), _T(""), m_AttributeGroupAnimation);
	DuiCreateAttribute(m_AttributeAutoPlay, _T("AutoPlay"), _T(""), m_AttributeGroupAnimation);

	return;
}

void CDUIAnimateCtrl::PaintStatusImage(HDC hDC)
{
	__super::PaintStatusImage(hDC);

	CDUIRect rcBorderRound = GetRoundCorner();
	m_AttributeImageAnimate.DrawAnimate(hDC, GetAbsoluteRect(), m_rcPaint, m_AnimateImageInfo, m_nFrameCur, rcBorderRound);

	return;
}

void CDUIAnimateCtrl::SetInternVisible(bool bVisible)
{
	__super::SetInternVisible(bVisible);

	if (IsVisible() && IsAutoPlay())
	{
		PlayAnimate();

		return;
	}

	StopAnimate();

	return;
}

void CDUIAnimateCtrl::ConstructAnimateImageInfo()
{
	CDUIImageBase *pImageBase = m_AttributeImageAnimate.GetCurImageBase();
	if (NULL == pImageBase) return;

	m_AnimateImageInfo = pImageBase->GetAnimateImageInfo(GetScale());
	m_nFrameCur = 0;

	CDUISize szItem = GetSequenceFrameSize();
	if (szItem.cx <= 0 || szItem.cy <= 0) return;

	pImageBase->IsScale(GetScale()) ? szItem = DuiDpiScaleCtrl(szItem) : szItem;
	m_AnimateImageInfo.AnimateImageType = AnimateImage_SequenceFrame;
	m_AnimateImageInfo.nFrameCount = (pImageBase->GetWidth(GetScale()) / szItem.cx) * (pImageBase->GetHeight(GetScale()) / szItem.cy);
	m_AnimateImageInfo.nSequenceFrameSpeed = GetSequenceFrameSpeed();
	m_AnimateImageInfo.szSequenceFrameSize = szItem;

	return;
}

void CDUIAnimateCtrl::PlayCurFrame()
{
	if (m_AnimateImageInfo.nFrameCount <= 0
		|| (AnimateImage_Gif == m_AnimateImageInfo.AnimateImageType && m_AnimateImageInfo.vecFrameElapse.size() <= 0)
		|| (AnimateImage_Webp == m_AnimateImageInfo.AnimateImageType && m_AnimateImageInfo.vecFrameElapse.size() <= 0)
		|| (AnimateImage_SequenceFrame == m_AnimateImageInfo.AnimateImageType && m_AnimateImageInfo.nSequenceFrameSpeed <= 0)
		|| NULL == m_pWndOwner) return;

	int nElapse = 100;
	switch (m_AnimateImageInfo.AnimateImageType)
	{
		case AnimateImage_Gif:
		{
			nElapse = m_AnimateImageInfo.vecFrameElapse[m_nFrameCur];

			break;
		}
		case AnimateImage_Webp:
		{
			if (m_nFrameCur < (int)m_AnimateImageInfo.vecFrameElapse.size())
			{
				nElapse = m_AnimateImageInfo.vecFrameElapse[m_nFrameCur];
			}

			break;
		}
		case AnimateImage_SequenceFrame:
		{
			nElapse = m_AnimateImageInfo.nSequenceFrameSpeed;

			break;
		}
		default:
			break;
	}

	(0 == nElapse) ? nElapse = 100 : nElapse;
	SetTimer(Dui_TimerAnimate_ID, nElapse);

	return;
}