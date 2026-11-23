#include "StdAfx.h"
#include "DUIAttributeFloat.h"

//////////////////////////////////////////////////////////////////////////
DuiImplement_CreateAttribute(CDUIAttributeFloat)
MMImplement_ClassName(CDUIAttributeFloat)

CDUIAttributeFloat::CDUIAttributeFloat(float fValue)
	: m_fValue(fValue)
{
#ifdef DUI_DESIGN
	m_DuiAttributeType = DuiAttribute_Float;
#endif

	return;
}

CDUIAttributeFloat::~CDUIAttributeFloat(void)
{
}

LPVOID CDUIAttributeFloat::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(CDUIAttributeFloat, Guid, dwQueryVer);

	return __super::QueryInterface(Guid, dwQueryVer);
}

CDUIAttributeFloat & CDUIAttributeFloat::operator = (CDUIAttributeObject &Right)
{
	__super::operator = (Right);

	CDUIAttributeFloat &FloatRight = (CDUIAttributeFloat&)Right;
	m_fValue = FloatRight.m_fValue;

	return *this;
}

void CDUIAttributeFloat::SetValue(float fValue)
{
	if (m_fValue == fValue) return;

	m_fValue = fValue;

	NotifyChange();

	return;
}

float CDUIAttributeFloat::GetValue()
{
	return m_fValue;
}

bool CDUIAttributeFloat::SetAttribute(LPCSTR lpszName, LPCSTR lpszValue)
{
	if (__super::SetAttribute(lpszName, lpszValue)) return true;

	if (0 == strcmp(lpszName, Dui_Key_AttriNumberFloat))
	{
		SetValue(atof(lpszValue));

		return true;
	}

	return false;
}

bool CDUIAttributeFloat::SaveAttribute(tinyxml2::XMLElement* pNode)
{
	if (false == __super::SaveAttribute(pNode)) return false;

	pNode->SetAttribute(Dui_Key_AttriNumberFloat, m_fValue);

	return true;
}