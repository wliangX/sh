#pragma once

namespace ICC
{
	class CFactoryImpl : public CFactoryBase
	{
	public:
		virtual IObjectPtr OnCreateObject();
	};

	ICCLibExport(CFactoryImpl)
}