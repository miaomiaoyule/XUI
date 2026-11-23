#ifndef __DUIATTRIBUTEFLOAT_H__
#define __DUIATTRIBUTEFLOAT_H__

#pragma once
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
#define VER_CDUIAttributeFloat INTERFACE_VERSION(1,1)
static const GUID IID_CDUIAttributeFloat = { 0xAC3A2171,0xD7DB,0x4B3D,0xAC,0x54,0x5C,0xEA,0x8F,0x01,0xAA,0xB2 };
class XUI_API CDUIAttributeFloat
	: public CDUIAttributeObject
{
	MMDeclare_Super(CDUIAttributeObject)
	DuiDeclare_CreateAttribute(CDUIAttributeFloat)
	MMDeclare_ClassName(CDUIAttributeFloat)

public:
	CDUIAttributeFloat(float fValue = 0.0f);
	virtual ~CDUIAttributeFloat(void);

protected:
	float								m_fValue = 0.0f;

	//override
public:
	LPVOID QueryInterface(REFGUID Guid, DWORD dwQueryVer) override;

	//method
public:
	CDUIAttributeFloat & operator = (CDUIAttributeObject &Right) override;

	virtual void SetValue(float fValue);
	virtual float GetValue();

	//attribute
protected:
	bool SetAttribute(LPCSTR lpszName, LPCSTR lpszValue) override;
	bool SaveAttribute(tinyxml2::XMLElement *pNode) override;
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
