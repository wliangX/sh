#pragma once
//stl
#include <map>
#include <string>

//project
#include <Log/ILog.h>

namespace ICC
{
	namespace Log
	{
		/*
		* class   ��־����ʵ����
		* author  w16314
		* purpose
		* note
		*/
		class CLogFactoryImpl :
			public ILogFactory
		{
			ICCFactoryHeaderInitialize(CLogFactoryImpl)
		public:
			virtual ILogPtr GetLogger(const std::string& p_strModuleName);
		private:
			std::map<std::string, ILogPtr> m_mapLog;
		};
	}
}