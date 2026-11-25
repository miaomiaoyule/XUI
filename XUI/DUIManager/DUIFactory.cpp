#include "StdAfx.h"
#include "DUIFactory.h"

//////////////////////////////////////////////////////////////////////////
CDUIFactory::CDUIFactory()
{
	//控件
	DUIInner_RegisterCreateControl(CDUIControlBase);
	DUIInner_RegisterCreateControl(CDUIContainerCtrl);
	DUIInner_RegisterCreateControl(CDUIButtonCtrl);
	DUIInner_RegisterCreateControl(CDUIComboxCtrl);
	DUIInner_RegisterCreateControl(CDUIDateTimeCtrl);
	DUIInner_RegisterCreateControl(CDUIAnimateCtrl);
	DUIInner_RegisterCreateControl(CDUIIPAddressCtrl);
	DUIInner_RegisterCreateControl(CDUIColorPaletteCtrl);
	DUIInner_RegisterCreateControl(CDUICalendarCtrl);
	DUIInner_RegisterCreateControl(CDUIRotateMenuItemCtrl);
	DUIInner_RegisterCreateControl(CDUIRotateMenuCtrl);
	DUIInner_RegisterCreateControl(CDUIStaticCtrl);
	DUIInner_RegisterCreateControl(CDUIListViewCtrl);
	DUIInner_RegisterCreateControl(CDUIListHeaderCtrl);
	DUIInner_RegisterCreateControl(CDUIListHeaderItemCtrl);
	DUIInner_RegisterCreateControl(CDUIListItemCtrl);
	DUIInner_RegisterCreateControl(CDUIMenuCtrl);
	DUIInner_RegisterCreateControl(CDUIMenuItemCtrl);
	DUIInner_RegisterCreateControl(CDUICheckBoxCtrl);
	DUIInner_RegisterCreateControl(CDUIProgressCtrl);
	DUIInner_RegisterCreateControl(CDUIProgressCircleCtrl);
	DUIInner_RegisterCreateControl(CDUIScrollBarCtrl);
	DUIInner_RegisterCreateControl(CDUISliderCtrl);
	DUIInner_RegisterCreateControl(CDUITreeNodeCtrl);
	DUIInner_RegisterCreateControl(CDUITreeViewCtrl);
	DUIInner_RegisterCreateControl(CDUIHorizontalLayoutCtrl);
	DUIInner_RegisterCreateControl(CDUITabLayoutCtrl);
	DUIInner_RegisterCreateControl(CDUIVerticalLayoutCtrl);
	DUIInner_RegisterCreateControl(CDUIRadioBoxCtrl);
	DUIInner_RegisterCreateControl(CDUIXEditCtrl);
	DUIInner_RegisterCreateControl(CDUISpinCtrl);
	DUIInner_RegisterCreateControl(CDUIClockCtrl);
	DUIInner_RegisterCreateControl(CDUIGroupCtrl);
	DUIInner_RegisterCreateControl(CDUIKLineChartCtrl);
	RegistControlClass(("CDUIThinkEditCtrl"), (DUICreateControlObj)CDUIXEditCtrl::CreateControlObj);
#ifndef DuiPlatform_SDL
	DUIInner_RegisterCreateControl(CDUIEditCtrl);
	DUIInner_RegisterCreateControl(CDUIRichEditCtrl);
	DUIInner_RegisterCreateControl(CDUIHotKeyCtrl);
	DUIInner_RegisterCreateControl(CDUIWebBrowserCtrl);
#endif

	//资源
	DUIInner_RegisterCreateResource(CDUIFontBase);
	DUIInner_RegisterCreateResource(CDUIImageBase);
	DUIInner_RegisterCreateResource(CDUIColorBase);

	//属性
	DUIInner_RegisterCreateAttribute(CDUIAttributeGroup);
	DUIInner_RegisterCreateAttribute(CDUIAttributePosition);
	DUIInner_RegisterCreateAttribute(CDUIAttriImageSection);
	DUIInner_RegisterCreateAttribute(CDUIAttributeLong);
	DUIInner_RegisterCreateAttribute(CDUIAttributeFloat);
	DUIInner_RegisterCreateAttribute(CDUIAttributeColor);
	DUIInner_RegisterCreateAttribute(CDUIAttributeColorSwitch);
	DUIInner_RegisterCreateAttribute(CDUIAttributeText);
	DUIInner_RegisterCreateAttribute(CDUIAttributeTextStyle);
	DUIInner_RegisterCreateAttribute(CDUIAttributeRichText);
	DUIInner_RegisterCreateAttribute(CDUIAttributeBool);
	DUIInner_RegisterCreateAttribute(CDUIAttributeCombox);
	DUIInner_RegisterCreateAttribute(CDUIAttributeCursor);
	DUIInner_RegisterCreateAttribute(CDUIAttributeDuiSelect);
	DUIInner_RegisterCreateAttribute(CDUIAttributeTabSelect);
	DUIInner_RegisterCreateAttribute(CDUIAttributeSize);
	DUIInner_RegisterCreateAttribute(CDUIAttributeRect);
	DUIInner_RegisterCreateAttribute(CDUIAttributeHotKey);

	return;
}

CDUIFactory::~CDUIFactory()
{
}

CDUIFactory * CDUIFactory::GetInstance()
{
	static CDUIFactory Obj;

	return &Obj;
}

CDUIControlBase * CDUIFactory::CreateControlObj(LPCSTR lpszClassName)
{
	auto FindIt = m_mapControlClass.find(lpszClassName);

	if (FindIt == m_mapControlClass.end()) return NULL;

	return (CDUIControlBase*)(FindIt->second());
}

CDUIResourceBase * CDUIFactory::CreateResourceObj(LPCSTR lpszClassName)
{
	auto FindIt = m_mapResourceClass.find(lpszClassName);

	if (FindIt == m_mapResourceClass.end()) return NULL;

	return (CDUIResourceBase*)(FindIt->second());
}

CDUIAttributeObject * CDUIFactory::CreateAttributeObj(LPCSTR lpszClassName)
{
	auto FindIt = m_mapAttributeClass.find(lpszClassName);

	if (FindIt == m_mapAttributeClass.end()) return NULL;

	return (CDUIAttributeObject*)(FindIt->second());
}

void CDUIFactory::RegistControlClass(LPCSTR lpszClassName, DUICreateControlObj pFunc)
{
	m_mapControlClass.insert(MapDuiCreateControl::value_type(lpszClassName, pFunc));
}

void CDUIFactory::RegistResourceClass(LPCSTR lpszClassName, DUICreateResourceObj pFunc)
{
	m_mapResourceClass.insert(MapDuiCreateResource::value_type(lpszClassName, pFunc));
}

void CDUIFactory::RegistAttributeClass(LPCSTR lpszClassName, DUICreateAttributeObj pFunc)
{
	m_mapAttributeClass.insert(MapDuiCreateAttribute::value_type(lpszClassName, pFunc));
}

//////////////////////////////////////////////////////////////////////////