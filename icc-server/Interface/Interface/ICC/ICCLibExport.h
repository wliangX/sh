#pragma once
#include <ICC/ICCExport.h>
#include <IFactory.h>

#define ICCLibExport(FtyImpl) \
ICCExport IFactory* ICCCreateInstance() \
{ \
	return new FtyImpl; \
}