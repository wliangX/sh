#pragma once
#include <IObject.h>

class CFactoryImpl :
	public CFactoryBase
{
public:
	virtual IObjectPtr OnCreateObject();
};

ICCLibExport(CFactoryImpl)
