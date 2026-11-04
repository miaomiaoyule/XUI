#ifndef __MAGNETBOX_H__
#define __MAGNETBOX_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
//code by 城南花已开 2020-1-25 QQ:284168136
//////////////////////////////////////////////////////////////////////////
#define MagnetType_None					(0x00)
#define MagnetType_Left					(0x01)
#define MagnetType_Top					(0x02)
#define MagnetType_Right				(0x04)
#define MagnetType_Bottom				(0x08)

typedef std::unordered_map<int, CMMRect> MapMagnetBox;

//////////////////////////////////////////////////////////////////////////
class MMHELPER_API CMMMagnetBox
{
public:
	struct tagMagnetBox : public CMMRect
	{
		MMDeclare_Super(CMMRect)

		int							nIndex = -1;
		int							MagnetType = MagnetType_None;

		virtual tagMagnetBox & operator = (_In_ const CMMRect &rcRight)
		{
			__super::operator = (rcRight);
			
			return *this;
		}
	};
	struct tagFindMagnetBox
	{
		tagMagnetBox				rcBoxLeft;
		tagMagnetBox				rcBoxTop;
		tagMagnetBox				rcBoxRight;
		tagMagnetBox				rcBoxBottom;
	};

public:
	CMMMagnetBox();
	virtual ~CMMMagnetBox() {}

protected:
	MapMagnetBox					m_mapMagnetBox;
	tagFindMagnetBox				m_CurBoxMagnetInfo;
	int								m_nMagnetType = MagnetType_Left | MagnetType_Top | MagnetType_Right | MagnetType_Bottom;
	int								m_nMagnetValue = 12;
	bool							m_bBoxMovingX = false;
	bool							m_bBoxMovingY = false;
	CMMPoint						m_ptPullTotal = {};
	CMMRect							m_rcPullTotal = {};

public:
	void SceneInit(int nMagnetType = MagnetType_Left | MagnetType_Top | MagnetType_Right | MagnetType_Bottom, int nMagnetValue = 12);
	void PushBox(int nIndex, CMMRect rcBox);
	void RemoveAt(int nIndex);
	void RemoveAll();
	tagFindMagnetBox GetNearestMagnetBox(int nIndex);

	CMMRect MoveBox(int nIndex, IN CMMPoint ptOffset);
	CMMRect StretchBox(int nIndex, IN CMMRect rcStretch, OUT bool &bMagnet);

	//help
protected:
	void InitBoxMagnetInfo(int nIndex);
	bool MoveOffset(int nIndex, IN CMMPoint ptOffset, OUT CMMPoint &ptOffsetRealy);
	bool StretchOffset(int nIndex, IN CMMRect rcStretch, OUT CMMRect &rcStretchRealy);
	void FindNearestMagnetBox(int nIndex, int nMoveDirection, int nBorderNearest, tagFindMagnetBox &FindMagnetBox);
	void VerifyBoxLeftDirection(int nIndex, CMMRect rcBoxItem, CMMRect rcBox, int nBorderNearest, tagFindMagnetBox &FindMagnetBox);
	void VerifyBoxRightDirection(int nIndex, CMMRect rcBoxItem, CMMRect rcBox, int nBorderNearest, tagFindMagnetBox &FindMagnetBox);
	void VerifyBoxTopDirection(int nIndex, CMMRect rcBoxItem, CMMRect rcBox, int nBorderNearest, tagFindMagnetBox &FindMagnetBox);
	void VerifyBoxBottomDirection(int nIndex, CMMRect rcBoxItem, CMMRect rcBox, int nBorderNearest, tagFindMagnetBox &FindMagnetBox);
};

//////////////////////////////////////////////////////////////////////////
class MMHELPER_API CMMRectPlace
{
public:
	CMMRectPlace();
	virtual ~CMMRectPlace() {}

protected:
	CMMRect							m_rcLimit;
	std::vector<CMMRect>			m_vecRect;

public:
	void SetLimit(IN CMMRect rcLimit);
	void PushRect(IN CMMRect rcRect);
	void RemoveAll();
	bool CalcPlace(IN CMMRect rcRect, OUT CMMRect &rcPlace);
};

//////////////////////////////////////////////////////////////////////////
#endif
