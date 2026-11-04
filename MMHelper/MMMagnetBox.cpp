#include "stdafx.h"
#include "MMMagnetBox.h"

#define MIN_OFFSET_MAGNET					(1)

//////////////////////////////////////////////////////////////////////////
CMMMagnetBox::CMMMagnetBox()
{

}

void CMMMagnetBox::SceneInit(int nMagnetType, int nMagnetValue)
{
	m_nMagnetType = nMagnetType;
	m_nMagnetValue = max(1, nMagnetValue);
	m_CurBoxMagnetInfo = {};
	m_bBoxMovingX = false;
	m_bBoxMovingY = false;
	m_ptPullTotal = {};
	m_rcPullTotal = {};
	m_mapMagnetBox.clear();

	return;
}

void CMMMagnetBox::PushBox(int nIndex, CMMRect rcBox)
{
	m_mapMagnetBox[nIndex] = rcBox;

	return;
}

void CMMMagnetBox::RemoveAt(int nIndex)
{
	m_mapMagnetBox.erase(nIndex);

	return;
}

void CMMMagnetBox::RemoveAll()
{
	m_mapMagnetBox.clear();

	return;
}

CMMMagnetBox::tagFindMagnetBox CMMMagnetBox::GetNearestMagnetBox(int nIndex)
{
	tagFindMagnetBox FindMagnetBox = {};
	FindNearestMagnetBox(nIndex, MagnetType_None, MagnetType_None, FindMagnetBox);

	return FindMagnetBox;
}

CMMRect CMMMagnetBox::MoveBox(int nIndex, IN CMMPoint ptOffset)
{
	auto FindIt = m_mapMagnetBox.find(nIndex);
	if (FindIt == m_mapMagnetBox.end())
	{
		assert(false);

		return {};
	}

	CMMRect rcBox = FindIt->second;

	//can move
	if (false == MoveOffset(nIndex, ptOffset, ptOffset))
	{
		return rcBox;
	}

	//move
	rcBox.Offset(ptOffset);
	FindIt->second = rcBox;
	int nMoveDirection = MagnetType_None, nBorderNeares = MagnetType_None;
	ptOffset.x < 0 ? nMoveDirection |= MagnetType_Left, nBorderNeares |= (MagnetType_Left | MagnetType_Right) : 0;
	ptOffset.x > 0 ? nMoveDirection |= MagnetType_Right, nBorderNeares |= (MagnetType_Left | MagnetType_Right) : 0;
	ptOffset.y < 0 ? nMoveDirection |= MagnetType_Top, nBorderNeares |= (MagnetType_Top | MagnetType_Bottom) : 0;
	ptOffset.y > 0 ? nMoveDirection |= MagnetType_Bottom, nBorderNeares |= (MagnetType_Top | MagnetType_Bottom) : 0;

	//magnet
	tagFindMagnetBox FindMagnetBox;
	FindNearestMagnetBox(nIndex, nMoveDirection, nBorderNeares, FindMagnetBox);
	CMMRect rcBoxPre = rcBox;
	
	if (-1 != FindMagnetBox.rcBoxLeft.nIndex)
	{
		tagMagnetBox &rcBoxLeft = FindMagnetBox.rcBoxLeft;
		int nLeft = MagnetType_Left == rcBoxLeft.MagnetType ? rcBoxLeft.left : rcBoxLeft.right + MIN_OFFSET_MAGNET;
		rcBox.Offset(POINT{ nLeft - rcBox.left, 0 });
	}
	if (-1 != FindMagnetBox.rcBoxRight.nIndex)
	{
		tagMagnetBox &rcBoxRight = FindMagnetBox.rcBoxRight;
		int nRight = MagnetType_Left == rcBoxRight.MagnetType ? rcBoxRight.left - MIN_OFFSET_MAGNET : rcBoxRight.right;
		rcBox.Offset(POINT{ nRight - rcBox.right, 0 });
	}
	if (-1 != FindMagnetBox.rcBoxTop.nIndex)
	{
		tagMagnetBox &rcBoxTop = FindMagnetBox.rcBoxTop;
		int nTop = MagnetType_Top == rcBoxTop.MagnetType ? rcBoxTop.top : rcBoxTop.bottom + MIN_OFFSET_MAGNET;
		rcBox.Offset(POINT{ 0, nTop - rcBox.top });
	}
	if (-1 != FindMagnetBox.rcBoxBottom.nIndex)
	{
		tagMagnetBox &rcBoxBottom = FindMagnetBox.rcBoxBottom;
		int nBottom = MagnetType_Top == rcBoxBottom.MagnetType ? rcBoxBottom.top - MIN_OFFSET_MAGNET : rcBoxBottom.bottom;
		rcBox.Offset(POINT{ 0, nBottom - rcBox.bottom });
	}
	
	FindIt->second = rcBox;
	
	//init magnetinfo
	if (-1 != FindMagnetBox.rcBoxLeft.nIndex
		|| -1 != FindMagnetBox.rcBoxTop.nIndex
		|| -1 != FindMagnetBox.rcBoxRight.nIndex
		|| -1 != FindMagnetBox.rcBoxBottom.nIndex)
	{
		m_bBoxMovingX = false == m_bBoxMovingX || -1 != FindMagnetBox.rcBoxLeft.nIndex || -1 != FindMagnetBox.rcBoxRight.nIndex ? false : true;
		m_bBoxMovingY = false == m_bBoxMovingY || -1 != FindMagnetBox.rcBoxTop.nIndex || -1 != FindMagnetBox.rcBoxBottom.nIndex ? false : true;
	}

	return rcBox;
}

CMMRect CMMMagnetBox::StretchBox(int nIndex, IN CMMRect rcStretch, OUT bool &bMagnet)
{
	bMagnet = false;

	auto FindIt = m_mapMagnetBox.find(nIndex);
	if (FindIt == m_mapMagnetBox.end())
	{
		assert(false);

		return {};
	}

	CMMRect rcBox = FindIt->second;

	//can stretch
	if (false == StretchOffset(nIndex, rcStretch, rcStretch))
	{
		return rcBox;
	}

	//stretch
	rcBox.left += rcStretch.left;
	rcBox.top += rcStretch.top;
	rcBox.right += rcStretch.right;
	rcBox.bottom += rcStretch.bottom;
	FindIt->second = rcBox;
	int nMoveDirection = MagnetType_None, nBorderNeares = MagnetType_None;
	rcStretch.left < 0 || rcStretch.right < 0 ? nMoveDirection |= MagnetType_Left : 0;
	rcStretch.left > 0 || rcStretch.right > 0 ? nMoveDirection |= MagnetType_Right : 0;
	rcStretch.top < 0 || rcStretch.bottom < 0 ? nMoveDirection |= MagnetType_Top : 0;
	rcStretch.top > 0 || rcStretch.bottom > 0 ? nMoveDirection |= MagnetType_Bottom : 0;
	rcStretch.left < 0 || rcStretch.left > 0 ? nBorderNeares |= MagnetType_Left : 0;
	rcStretch.right < 0 || rcStretch.right > 0 ? nBorderNeares |= MagnetType_Right : 0;
	rcStretch.top < 0 || rcStretch.top > 0 ? nBorderNeares |= MagnetType_Top : 0;
	rcStretch.bottom < 0 || rcStretch.bottom > 0 ? nBorderNeares |= MagnetType_Bottom : 0;

	//magnet
	tagFindMagnetBox FindMagnetBox;
	FindNearestMagnetBox(nIndex, nMoveDirection, nBorderNeares, FindMagnetBox);
	CMMRect rcBoxPre = rcBox;

	if (-1 != FindMagnetBox.rcBoxLeft.nIndex)
	{
		tagMagnetBox &rcBoxLeft = FindMagnetBox.rcBoxLeft;
		int nLeft = MagnetType_Left == rcBoxLeft.MagnetType ? rcBoxLeft.left : rcBoxLeft.right + MIN_OFFSET_MAGNET;
		rcBox.left = nLeft;
	}
	if (-1 != FindMagnetBox.rcBoxRight.nIndex)
	{
		tagMagnetBox &rcBoxRight = FindMagnetBox.rcBoxRight;
		int nRight = MagnetType_Left == rcBoxRight.MagnetType ? rcBoxRight.left - MIN_OFFSET_MAGNET : rcBoxRight.right;
		rcBox.right = nRight;
	}
	if (-1 != FindMagnetBox.rcBoxTop.nIndex)
	{
		tagMagnetBox &rcBoxTop = FindMagnetBox.rcBoxTop;
		int nTop = MagnetType_Top == rcBoxTop.MagnetType ? rcBoxTop.top : rcBoxTop.bottom + MIN_OFFSET_MAGNET;
		rcBox.top = nTop;
	}
	if (-1 != FindMagnetBox.rcBoxBottom.nIndex)
	{
		tagMagnetBox &rcBoxBottom = FindMagnetBox.rcBoxBottom;
		int nBottom = MagnetType_Top == rcBoxBottom.MagnetType ? rcBoxBottom.top - MIN_OFFSET_MAGNET : rcBoxBottom.bottom;
		rcBox.bottom = nBottom;
	}

	FindIt->second = rcBox;

	//init magnetinfo
	if (-1 != FindMagnetBox.rcBoxLeft.nIndex 
		|| -1 != FindMagnetBox.rcBoxTop.nIndex
		|| -1 != FindMagnetBox.rcBoxRight.nIndex
		|| -1 != FindMagnetBox.rcBoxBottom.nIndex)
	{
		bMagnet = true;
		m_bBoxMovingX = false == m_bBoxMovingX || -1 != FindMagnetBox.rcBoxLeft.nIndex || -1 != FindMagnetBox.rcBoxRight.nIndex ? false : true;
		m_bBoxMovingY = false == m_bBoxMovingY || -1 != FindMagnetBox.rcBoxTop.nIndex || -1 != FindMagnetBox.rcBoxBottom.nIndex ? false : true;
		InitBoxMagnetInfo(nIndex);
	}

	return rcBox;
}

void CMMMagnetBox::InitBoxMagnetInfo(int nIndex)
{
	if (m_bBoxMovingX && m_bBoxMovingY) return;

	m_CurBoxMagnetInfo = {};
	FindNearestMagnetBox(nIndex, MagnetType_None, MagnetType_None, m_CurBoxMagnetInfo);

	return;
}

bool CMMMagnetBox::MoveOffset(int nIndex, IN CMMPoint ptOffset, OUT CMMPoint &ptOffsetRealy)
{
	InitBoxMagnetInfo(nIndex);

	if (false == m_bBoxMovingX && ((m_nMagnetType & MagnetType_Left) || (m_nMagnetType & MagnetType_Right)))
	{
		do
		{
			m_ptPullTotal.x += ptOffset.x;

			if ((-1 != m_CurBoxMagnetInfo.rcBoxLeft.nIndex || -1 != m_CurBoxMagnetInfo.rcBoxRight.nIndex)
				&& abs(m_ptPullTotal.x) <= m_nMagnetValue)
			{
				ptOffset.x = 0;

				break;
			}

			m_bBoxMovingX = true;
			ptOffset.x = m_ptPullTotal.x;
			m_ptPullTotal.x = 0;

		} while (false);
	}
	if (false == m_bBoxMovingY && ((m_nMagnetType & MagnetType_Top) || (m_nMagnetType & MagnetType_Bottom)))
	{
		do
		{
			m_ptPullTotal.y += ptOffset.y;

			if ((-1 != m_CurBoxMagnetInfo.rcBoxTop.nIndex || -1 != m_CurBoxMagnetInfo.rcBoxBottom.nIndex)
				&& abs(m_ptPullTotal.y) <= m_nMagnetValue)
			{
				ptOffset.y = 0;

				break;
			}

			m_bBoxMovingY = true;
			ptOffset.y = m_ptPullTotal.y;
			m_ptPullTotal.y = 0;

		} while (false);
	}
	if (0 == ptOffset.x && 0 == ptOffset.y)
	{
		ptOffsetRealy = ptOffset;

		return false;
	}

	ptOffsetRealy = ptOffset;

	return true;
}

bool CMMMagnetBox::StretchOffset(int nIndex, IN CMMRect rcStretch, OUT CMMRect &rcStretchRealy)
{
	InitBoxMagnetInfo(nIndex);

	if (false == m_bBoxMovingX)
	{
		do
		{
			m_rcPullTotal.left += rcStretch.left;
			m_rcPullTotal.right += rcStretch.right;

			if ((0 == m_rcPullTotal.left && 0 == m_rcPullTotal.right)
				|| (0 != m_rcPullTotal.left && -1 != m_CurBoxMagnetInfo.rcBoxLeft.nIndex && abs(m_rcPullTotal.left) < m_nMagnetValue)
				|| (0 != m_rcPullTotal.right && -1 != m_CurBoxMagnetInfo.rcBoxRight.nIndex && abs(m_rcPullTotal.right) < m_nMagnetValue))
			{
				rcStretch.left = rcStretch.right = 0;

				break;
			}

			m_bBoxMovingX = true;
			rcStretch.left = m_rcPullTotal.left;
			rcStretch.right = m_rcPullTotal.right;
			m_rcPullTotal.left = m_rcPullTotal.right = 0;

		} while (false);
	}
	if (false == m_bBoxMovingY)
	{
		do
		{
			m_rcPullTotal.top += rcStretch.top;
			m_rcPullTotal.bottom += rcStretch.bottom;

			if ((0 == m_rcPullTotal.top && 0 == m_rcPullTotal.bottom)
				|| (0 != m_rcPullTotal.top && -1 != m_CurBoxMagnetInfo.rcBoxTop.nIndex && abs(m_rcPullTotal.top) < m_nMagnetValue)
				|| (0 != m_rcPullTotal.bottom && -1 != m_CurBoxMagnetInfo.rcBoxBottom.nIndex && abs(m_rcPullTotal.bottom) < m_nMagnetValue))
			{
				rcStretch.top = rcStretch.bottom = 0;

				break;
			}

			m_bBoxMovingY = true;
			rcStretch.top = m_rcPullTotal.top;
			rcStretch.bottom = m_rcPullTotal.bottom;
			m_rcPullTotal.top = m_rcPullTotal.bottom = 0;

		} while (false);
	}
	if (0 == rcStretch.left 
		&& 0 == rcStretch.right 
		&& 0 == rcStretch.top 
		&& 0 == rcStretch.bottom)
	{
		rcStretchRealy = rcStretch;

		return false;
	}

	rcStretchRealy = rcStretch;

	return true;
}

void CMMMagnetBox::FindNearestMagnetBox(int nIndex, int nMoveDirection, int nBorderNearest, tagFindMagnetBox &FindMagnetBox)
{
	auto FindIt = m_mapMagnetBox.find(nIndex);
	if (FindIt == m_mapMagnetBox.end()) return;

	CMMRect rcBox = FindIt->second;
	for (auto &BoxItem : m_mapMagnetBox)
	{
		if (BoxItem.first == nIndex) continue;

		CMMRect rcBoxItem = BoxItem.second;

		//no direction
		if (MagnetType_None == nMoveDirection)
		{
			//left
			if ((m_nMagnetType & MagnetType_Left) 
				&& -1 == FindMagnetBox.rcBoxLeft.nIndex
				&& (rcBox.left == rcBoxItem.left || MIN_OFFSET_MAGNET == rcBox.left - rcBoxItem.right))
			{
				FindMagnetBox.rcBoxLeft = rcBoxItem;
				FindMagnetBox.rcBoxLeft.nIndex = BoxItem.first;
				FindMagnetBox.rcBoxLeft.MagnetType = rcBox.left == rcBoxItem.left ? MagnetType_Left : MagnetType_Right;
			}
			//top
			if ((m_nMagnetType & MagnetType_Top) 
				&& -1 == FindMagnetBox.rcBoxTop.nIndex
				&& (rcBox.top == rcBoxItem.top || MIN_OFFSET_MAGNET == rcBox.top - rcBoxItem.bottom))
			{
				FindMagnetBox.rcBoxTop = rcBoxItem;
				FindMagnetBox.rcBoxTop.nIndex = BoxItem.first;
				FindMagnetBox.rcBoxTop.MagnetType = rcBox.top == rcBoxItem.top ? MagnetType_Top : MagnetType_Bottom;
			}
			//right
			if ((m_nMagnetType & MagnetType_Right)
				&& -1 == FindMagnetBox.rcBoxRight.nIndex
				&& (MIN_OFFSET_MAGNET == rcBoxItem.left - rcBox.right || rcBoxItem.right == rcBox.right))
			{
				FindMagnetBox.rcBoxRight = rcBoxItem;
				FindMagnetBox.rcBoxRight.nIndex = BoxItem.first;
				FindMagnetBox.rcBoxRight.MagnetType = rcBoxItem.right == rcBox.right ? MagnetType_Right : MagnetType_Left;
			}
			//bottom
			if ((m_nMagnetType & MagnetType_Bottom)
				&& -1 == FindMagnetBox.rcBoxBottom.nIndex
				&& (MIN_OFFSET_MAGNET == rcBoxItem.top - rcBox.bottom || rcBoxItem.bottom == rcBox.bottom))
			{
				FindMagnetBox.rcBoxBottom = rcBoxItem;
				FindMagnetBox.rcBoxBottom.nIndex = BoxItem.first;
				FindMagnetBox.rcBoxBottom.MagnetType = rcBoxItem.bottom == rcBox.bottom ? MagnetType_Bottom : MagnetType_Top;
			}

			continue;
		}

		//left direction
		if ((nMoveDirection & MagnetType_Left) && (m_nMagnetType & MagnetType_Left))
		{
			VerifyBoxLeftDirection(BoxItem.first, rcBoxItem, rcBox, nBorderNearest, FindMagnetBox);
		}
		//top direction
		if ((nMoveDirection & MagnetType_Top) && (m_nMagnetType & MagnetType_Top))
		{
			VerifyBoxTopDirection(BoxItem.first, rcBoxItem, rcBox, nBorderNearest, FindMagnetBox);
		}
		//right direction
		if ((nMoveDirection & MagnetType_Right) && (m_nMagnetType & MagnetType_Right))
		{
			VerifyBoxRightDirection(BoxItem.first, rcBoxItem, rcBox, nBorderNearest, FindMagnetBox);
		}
		//bottom direction
		if ((nMoveDirection & MagnetType_Bottom) && (m_nMagnetType & MagnetType_Bottom))
		{
			VerifyBoxBottomDirection(BoxItem.first, rcBoxItem, rcBox, nBorderNearest, FindMagnetBox);
		}
	}

	return;
}

void CMMMagnetBox::VerifyBoxLeftDirection(int nIndex, CMMRect rcBoxItem, CMMRect rcBox, int nBorderNearest, tagFindMagnetBox &FindMagnetBox)
{
	//left
	auto &FindBoxLeft = FindMagnetBox.rcBoxLeft;
	if (nBorderNearest & MagnetType_Left)
	{
		if ((rcBoxItem.right <= rcBox.left && rcBoxItem.right + m_nMagnetValue >= rcBox.left && (-1 == FindBoxLeft.nIndex || (MagnetType_Left == FindBoxLeft.MagnetType && rcBoxItem.right > FindBoxLeft.left)))
			|| (rcBoxItem.right <= rcBox.left && rcBoxItem.right + m_nMagnetValue >= rcBox.left && (-1 == FindBoxLeft.nIndex || (MagnetType_Right == FindBoxLeft.MagnetType && rcBoxItem.right > FindBoxLeft.right))))
		{
			FindBoxLeft = rcBoxItem;
			FindBoxLeft.nIndex = nIndex;
			FindBoxLeft.MagnetType = MagnetType_Right;
		}
		if ((rcBoxItem.left <= rcBox.left && rcBoxItem.left + m_nMagnetValue >= rcBox.left && (-1 == FindBoxLeft.nIndex || (MagnetType_Left == FindBoxLeft.MagnetType && rcBoxItem.left > FindBoxLeft.left)))
			|| (rcBoxItem.left <= rcBox.left && rcBoxItem.left + m_nMagnetValue >= rcBox.left && (-1 == FindBoxLeft.nIndex || (MagnetType_Right == FindBoxLeft.MagnetType && rcBoxItem.left > FindBoxLeft.right))))
		{
			FindBoxLeft = rcBoxItem;
			FindBoxLeft.nIndex = nIndex;
			FindBoxLeft.MagnetType = MagnetType_Left;
		}
	}

	//right
	auto &FindBoxRight = FindMagnetBox.rcBoxRight;
	if (nBorderNearest & MagnetType_Right)
	{
		if ((rcBoxItem.right <= rcBox.right && rcBoxItem.right + m_nMagnetValue >= rcBox.right && (-1 == FindBoxRight.nIndex || (MagnetType_Left == FindBoxRight.MagnetType && rcBoxItem.right > FindBoxRight.left)))
			|| (rcBoxItem.right <= rcBox.right && rcBoxItem.right + m_nMagnetValue >= rcBox.right && (-1 == FindBoxRight.nIndex || (MagnetType_Right == FindBoxRight.MagnetType && rcBoxItem.right > FindBoxRight.right))))
		{
			FindBoxRight = rcBoxItem;
			FindBoxRight.nIndex = nIndex;
			FindBoxRight.MagnetType = MagnetType_Right;
		}
		if ((rcBoxItem.left <= rcBox.right && rcBoxItem.left + m_nMagnetValue >= rcBox.right && (-1 == FindBoxRight.nIndex || (MagnetType_Left == FindBoxRight.MagnetType && rcBoxItem.left > FindBoxRight.left)))
			|| (rcBoxItem.left <= rcBox.right && rcBoxItem.left + m_nMagnetValue >= rcBox.right && (-1 == FindBoxRight.nIndex || (MagnetType_Right == FindBoxRight.MagnetType && rcBoxItem.left > FindBoxRight.right))))
		{
			FindBoxRight = rcBoxItem;
			FindBoxRight.nIndex = nIndex;
			FindBoxRight.MagnetType = MagnetType_Left;
		}
	}

	//only one
	if (MagnetType_None != FindBoxLeft.MagnetType && MagnetType_None != FindBoxRight.MagnetType)
	{
		int nOffsetLeft = MagnetType_Left == FindBoxLeft.MagnetType ? abs(rcBox.left - FindBoxLeft.left) : abs(rcBox.left - FindBoxLeft.right);
		int nOffsetRight = MagnetType_Right == FindBoxRight.MagnetType ? abs(rcBox.right - FindBoxRight.right) : abs(rcBox.right - FindBoxRight.left);
		if (nOffsetLeft <= nOffsetRight)
		{
			FindBoxRight.nIndex = -1;
			FindBoxRight.MagnetType = MagnetType_None;
		}
		else
		{
			FindBoxLeft.nIndex = -1;
			FindBoxLeft.MagnetType = MagnetType_None;
		}
	}

	return;
}

void CMMMagnetBox::VerifyBoxRightDirection(int nIndex, CMMRect rcBoxItem, CMMRect rcBox, int nBorderNearest, tagFindMagnetBox &FindMagnetBox)
{
	//right
	auto &FindBoxRight = FindMagnetBox.rcBoxRight;
	if (nBorderNearest & MagnetType_Right)
	{
		if ((rcBoxItem.left >= rcBox.right && rcBoxItem.left - m_nMagnetValue <= rcBox.right && (-1 == FindBoxRight.nIndex || (MagnetType_Left == FindBoxRight.MagnetType && rcBoxItem.left < FindBoxRight.left)))
			|| (rcBoxItem.left >= rcBox.right && rcBoxItem.left - m_nMagnetValue <= rcBox.right && (-1 == FindBoxRight.nIndex || (MagnetType_Right == FindBoxRight.MagnetType && rcBoxItem.left < FindBoxRight.right))))
		{
			FindBoxRight = rcBoxItem;
			FindBoxRight.nIndex = nIndex;
			FindBoxRight.MagnetType = MagnetType_Left;
		}
		if ((rcBoxItem.right >= rcBox.right && rcBoxItem.right - m_nMagnetValue <= rcBox.right && (-1 == FindBoxRight.nIndex || (MagnetType_Left == FindBoxRight.MagnetType && rcBoxItem.right > FindBoxRight.left)))
			|| (rcBoxItem.right >= rcBox.right && rcBoxItem.right - m_nMagnetValue <= rcBox.right && (-1 == FindBoxRight.nIndex || (MagnetType_Right == FindBoxRight.MagnetType && rcBoxItem.right > FindBoxRight.right))))
		{
			FindBoxRight = rcBoxItem;
			FindBoxRight.nIndex = nIndex;
			FindBoxRight.MagnetType = MagnetType_Right;
		}
	}

	//left
	auto &FindBoxLeft = FindMagnetBox.rcBoxLeft;
	if (nBorderNearest & MagnetType_Left)
	{
		if ((rcBoxItem.right >= rcBox.left && rcBoxItem.right - m_nMagnetValue <= rcBox.left && (-1 == FindBoxLeft.nIndex || (MagnetType_Left == FindBoxLeft.MagnetType && rcBoxItem.right < FindBoxLeft.left)))
			|| (rcBoxItem.right >= rcBox.left && rcBoxItem.right - m_nMagnetValue <= rcBox.left && (-1 == FindBoxLeft.nIndex || (MagnetType_Right == FindBoxLeft.MagnetType && rcBoxItem.right < FindBoxLeft.right))))
		{
			FindBoxLeft = rcBoxItem;
			FindBoxLeft.nIndex = nIndex;
			FindBoxLeft.MagnetType = MagnetType_Right;
		}
		if ((rcBoxItem.left >= rcBox.left && rcBoxItem.left - m_nMagnetValue <= rcBox.left && (-1 == FindBoxLeft.nIndex || (MagnetType_Left == FindBoxLeft.MagnetType && rcBoxItem.left < FindBoxLeft.left)))
			|| (rcBoxItem.left >= rcBox.left && rcBoxItem.left - m_nMagnetValue <= rcBox.left && (-1 == FindBoxLeft.nIndex || (MagnetType_Right == FindBoxLeft.MagnetType && rcBoxItem.left < FindBoxLeft.right))))
		{
			FindBoxLeft = rcBoxItem;
			FindBoxLeft.nIndex = nIndex;
			FindBoxLeft.MagnetType = MagnetType_Left;
		}
	}

	//only one
	if (MagnetType_None != FindBoxLeft.MagnetType && MagnetType_None != FindBoxRight.MagnetType)
	{
		int nOffsetLeft = MagnetType_Left == FindBoxLeft.MagnetType ? abs(rcBox.left - FindBoxLeft.left) : abs(rcBox.left - FindBoxLeft.right);
		int nOffsetRight = MagnetType_Right == FindBoxRight.MagnetType ? abs(rcBox.right - FindBoxRight.right) : abs(rcBox.right - FindBoxRight.left);
		if (nOffsetLeft <= nOffsetRight)
		{
			FindBoxRight.nIndex = -1;
			FindBoxRight.MagnetType = MagnetType_None;
		}
		else
		{
			FindBoxLeft.nIndex = -1;
			FindBoxLeft.MagnetType = MagnetType_None;
		}
	}

	return;
}

void CMMMagnetBox::VerifyBoxTopDirection(int nIndex, CMMRect rcBoxItem, CMMRect rcBox, int nBorderNearest, tagFindMagnetBox &FindMagnetBox)
{
	//top
	auto &FindBoxTop = FindMagnetBox.rcBoxTop;
	if (nBorderNearest & MagnetType_Top)
	{
		if ((rcBoxItem.bottom <= rcBox.top && rcBoxItem.bottom + m_nMagnetValue >= rcBox.top && (-1 == FindBoxTop.nIndex || (MagnetType_Top == FindBoxTop.MagnetType && rcBoxItem.bottom > FindBoxTop.top)))
			|| (rcBoxItem.bottom <= rcBox.top && rcBoxItem.bottom + m_nMagnetValue >= rcBox.top && (-1 == FindBoxTop.nIndex || (MagnetType_Bottom == FindBoxTop.MagnetType && rcBoxItem.bottom > FindBoxTop.bottom))))
		{
			FindBoxTop = rcBoxItem;
			FindBoxTop.nIndex = nIndex;
			FindBoxTop.MagnetType = MagnetType_Bottom;
		}
		if ((rcBoxItem.top <= rcBox.top && rcBoxItem.top + m_nMagnetValue >= rcBox.top && (-1 == FindBoxTop.nIndex || (MagnetType_Top == FindBoxTop.MagnetType && rcBoxItem.top > FindBoxTop.top)))
			|| (rcBoxItem.top <= rcBox.top && rcBoxItem.top + m_nMagnetValue >= rcBox.top && (-1 == FindBoxTop.nIndex || (MagnetType_Bottom == FindBoxTop.MagnetType && rcBoxItem.top > FindBoxTop.bottom))))
		{
			FindBoxTop = rcBoxItem;
			FindBoxTop.nIndex = nIndex;
			FindBoxTop.MagnetType = MagnetType_Top;
		}
	}

	//bottom
	auto &FindBoxBottom = FindMagnetBox.rcBoxBottom;
	if (nBorderNearest & MagnetType_Bottom)
	{
		if ((rcBoxItem.bottom <= rcBox.bottom && rcBoxItem.bottom + m_nMagnetValue >= rcBox.bottom && (-1 == FindBoxBottom.nIndex || (MagnetType_Top == FindBoxBottom.MagnetType && rcBoxItem.bottom > FindBoxBottom.top)))
			|| (rcBoxItem.bottom <= rcBox.bottom && rcBoxItem.bottom + m_nMagnetValue >= rcBox.bottom && (-1 == FindBoxBottom.nIndex || (MagnetType_Bottom == FindBoxBottom.MagnetType && rcBoxItem.bottom > FindBoxBottom.bottom))))
		{
			FindBoxBottom = rcBoxItem;
			FindBoxBottom.nIndex = nIndex;
			FindBoxBottom.MagnetType = MagnetType_Bottom;
		}
		if ((rcBoxItem.top <= rcBox.bottom && rcBoxItem.top + m_nMagnetValue >= rcBox.bottom && (-1 == FindBoxBottom.nIndex || (MagnetType_Top == FindBoxBottom.MagnetType && rcBoxItem.top > FindBoxBottom.top)))
			|| (rcBoxItem.top <= rcBox.bottom && rcBoxItem.top + m_nMagnetValue >= rcBox.bottom && (-1 == FindBoxBottom.nIndex || (MagnetType_Bottom == FindBoxBottom.MagnetType && rcBoxItem.top > FindBoxBottom.bottom))))
		{
			FindBoxBottom = rcBoxItem;
			FindBoxBottom.nIndex = nIndex;
			FindBoxBottom.MagnetType = MagnetType_Top;
		}
	}

	//only one
	if (MagnetType_None != FindBoxTop.MagnetType && MagnetType_None != FindBoxBottom.MagnetType)
	{
		int nOffsetTop = MagnetType_Top == FindBoxTop.MagnetType ? abs(rcBox.top - FindBoxTop.top) : abs(rcBox.top - FindBoxTop.bottom);
		int nOffsetBottom = MagnetType_Bottom == FindBoxBottom.MagnetType ? abs(rcBox.bottom - FindBoxBottom.bottom) : abs(rcBox.bottom - FindBoxBottom.top);
		if (nOffsetTop <= nOffsetBottom)
		{
			FindBoxBottom.nIndex = -1;
			FindBoxBottom.MagnetType = MagnetType_None;
		}
		else
		{
			FindBoxTop.nIndex = -1;
			FindBoxTop.MagnetType = MagnetType_None;
		}
	}

	return;
}

void CMMMagnetBox::VerifyBoxBottomDirection(int nIndex, CMMRect rcBoxItem, CMMRect rcBox, int nBorderNearest, tagFindMagnetBox &FindMagnetBox)
{
	//bottom
	auto &FindBoxBottom = FindMagnetBox.rcBoxBottom;
	if (nBorderNearest & MagnetType_Bottom)
	{
		if ((rcBoxItem.top >= rcBox.bottom && rcBoxItem.top - m_nMagnetValue <= rcBox.bottom && (-1 == FindBoxBottom.nIndex || (MagnetType_Top == FindBoxBottom.MagnetType && rcBoxItem.top < FindBoxBottom.top)))
			|| (rcBoxItem.top >= rcBox.bottom && rcBoxItem.top - m_nMagnetValue <= rcBox.bottom && (-1 == FindBoxBottom.nIndex || (MagnetType_Bottom == FindBoxBottom.MagnetType && rcBoxItem.top < FindBoxBottom.bottom))))
		{
			FindBoxBottom = rcBoxItem;
			FindBoxBottom.nIndex = nIndex;
			FindBoxBottom.MagnetType = MagnetType_Top;
		}
		if ((rcBoxItem.bottom >= rcBox.bottom && rcBoxItem.bottom - m_nMagnetValue <= rcBox.bottom && (-1 == FindBoxBottom.nIndex || (MagnetType_Top == FindBoxBottom.MagnetType && rcBoxItem.bottom < FindBoxBottom.top)))
			|| (rcBoxItem.bottom >= rcBox.bottom && rcBoxItem.bottom - m_nMagnetValue <= rcBox.bottom && (-1 == FindBoxBottom.nIndex || (MagnetType_Bottom == FindBoxBottom.MagnetType && rcBoxItem.bottom < FindBoxBottom.bottom))))
		{
			FindBoxBottom = rcBoxItem;
			FindBoxBottom.nIndex = nIndex;
			FindBoxBottom.MagnetType = MagnetType_Bottom;
		}
	}

	//top
	auto &FindBoxTop = FindMagnetBox.rcBoxTop;
	if (nBorderNearest & MagnetType_Top)
	{
		if ((rcBoxItem.bottom >= rcBox.top && rcBoxItem.bottom - m_nMagnetValue <= rcBox.top && (-1 == FindBoxTop.nIndex || (MagnetType_Top == FindBoxTop.MagnetType && rcBoxItem.bottom > FindBoxTop.top)))
			|| (rcBoxItem.bottom >= rcBox.top && rcBoxItem.bottom - m_nMagnetValue <= rcBox.top && (-1 == FindBoxTop.nIndex || (MagnetType_Bottom == FindBoxTop.MagnetType && rcBoxItem.bottom > FindBoxTop.bottom))))
		{
			FindBoxTop = rcBoxItem;
			FindBoxTop.nIndex = nIndex;
			FindBoxTop.MagnetType = MagnetType_Bottom;
		}
		if ((rcBoxItem.top >= rcBox.top && rcBoxItem.top - m_nMagnetValue <= rcBox.top && (-1 == FindBoxTop.nIndex || (MagnetType_Top == FindBoxTop.MagnetType && rcBoxItem.top > FindBoxTop.top)))
			|| (rcBoxItem.top >= rcBox.top && rcBoxItem.top - m_nMagnetValue <= rcBox.top && (-1 == FindBoxTop.nIndex || (MagnetType_Bottom == FindBoxTop.MagnetType && rcBoxItem.top > FindBoxTop.bottom))))
		{
			FindBoxTop = rcBoxItem;
			FindBoxTop.nIndex = nIndex;
			FindBoxTop.MagnetType = MagnetType_Top;
		}
	}

	//only one
	if (MagnetType_None != FindBoxTop.MagnetType && MagnetType_None != FindBoxBottom.MagnetType)
	{
		int nOffsetTop = MagnetType_Top == FindBoxTop.MagnetType ? abs(rcBox.top - FindBoxTop.top) : abs(rcBox.top - FindBoxTop.bottom);
		int nOffsetBottom = MagnetType_Bottom == FindBoxBottom.MagnetType ? abs(rcBox.bottom - FindBoxBottom.bottom) : abs(rcBox.bottom - FindBoxBottom.top);
		if (nOffsetTop <= nOffsetBottom)
		{
			FindBoxBottom.nIndex = -1;
			FindBoxBottom.MagnetType = MagnetType_None;
		}
		else
		{
			FindBoxTop.nIndex = -1;
			FindBoxTop.MagnetType = MagnetType_None;
		}
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
CMMRectPlace::CMMRectPlace()
{

}

void CMMRectPlace::SetLimit(IN CMMRect rcLimit)
{
	m_rcLimit = rcLimit;

	return;
}

void CMMRectPlace::PushRect(IN CMMRect rcRect)
{
	if (rcRect.GetWidth() <= 0 || rcRect.GetHeight() <= 0) return;

	m_vecRect.push_back(rcRect);

	return;
}

void CMMRectPlace::RemoveAll()
{
	m_vecRect.clear();

	return;
}

bool CMMRectPlace::CalcPlace(IN CMMRect rcRect, OUT CMMRect &rcPlace)
{
	rcPlace.Clear();

	const int nWidth = rcRect.GetWidth();
	const int nHeight = rcRect.GetHeight();
	if (nWidth <= 0 || nHeight <= 0) return false;
	if (m_rcLimit.GetWidth() < nWidth || m_rcLimit.GetHeight() < nHeight) return false;

	const long nXMin = m_rcLimit.left;
	const long nXMax = m_rcLimit.right - nWidth;
	const long nYMin = m_rcLimit.top;
	const long nYMax = m_rcLimit.bottom - nHeight;

	std::vector<long> vecX;
	vecX.reserve(m_vecRect.size() * 2 + 2);
	vecX.push_back(nXMin);
	vecX.push_back(nXMax);
	for (auto &rcItem : m_vecRect)
	{
		vecX.push_back(rcItem.right);
		vecX.push_back(rcItem.left - nWidth);
	}
	std::sort(vecX.begin(), vecX.end(), [](long nLeft, long nRight) { return nLeft > nRight; });
	vecX.erase(std::unique(vecX.begin(), vecX.end()), vecX.end());

	struct tagSpan
	{
		long						nBegin = 0;
		long						nEnd = 0;
	};
	for (long nX : vecX)
	{
		if (nX > nXMax) continue;
		if (nX < nXMin) break;

		const long nRight = nX + nWidth;
		std::vector<tagSpan> vecSpan;
		vecSpan.reserve(m_vecRect.size());
		for (auto &rcItem : m_vecRect)
		{
			if (nX >= rcItem.right || nRight <= rcItem.left) continue;

			tagSpan Span;
			Span.nBegin = rcItem.top - nHeight + 1;
			Span.nEnd = rcItem.bottom - 1;
			if (Span.nBegin > Span.nEnd) continue;
			if (Span.nEnd < nYMin || Span.nBegin > nYMax) continue;

			vecSpan.push_back(Span);
		}
		std::sort(vecSpan.begin(), vecSpan.end(), [](const tagSpan &Left, const tagSpan &Right)
		{
			if (Left.nBegin != Right.nBegin) return Left.nBegin < Right.nBegin;

			return Left.nEnd < Right.nEnd;
		});

		long nY = nYMin;
		for (auto &Span : vecSpan)
		{
			if (nY > nYMax) break;
			if (nY < Span.nBegin) break;
			if (nY <= Span.nEnd) nY = Span.nEnd + 1;
		}
		if (nY > nYMax) continue;

		rcPlace.left = nX;
		rcPlace.top = nY;
		rcPlace.right = nRight;
		rcPlace.bottom = nY + nHeight;

		return true;
	}

	return false;
}