#pragma once

namespace ICC
{
	namespace DateTime
	{
		/*
		* class   线程工厂类
		* author  w16314
		* purpose
		* note
		*/
		class CDateTimeFactoryImpl :
			public IDateTimeFactory
		{
			ICCFactoryHeaderInitialize(CDateTimeFactoryImpl)
		public:
			virtual IDateTimePtr CreateDateTime();
		};
	}
}