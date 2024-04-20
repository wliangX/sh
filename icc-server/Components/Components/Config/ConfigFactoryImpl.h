#pragma once

namespace ICC
{
	namespace Config
	{
		class CConfigFactoryImpl :
			public IConfigFactory
		{
			ICCFactoryHeaderInitialize(CConfigFactoryImpl)
		public:
			//************************************
			// Method:    CreateConfig
			// FullName:  ICC::Config::CConfigFactoryImpl::CreateConfig
			// Access:    virtual public 
			// Returns:   ICC::Config::IConfigPtr
			// Qualifier:
			//************************************
			virtual IConfigPtr CreateConfigConfig();
			virtual IConfigPtr CreateConfig();
			virtual IConfigPtr CreateStaticConfig();
			virtual IConfigPtr CreateSqlConfig();
		public:
			IConfigPtr m_pConfigConfig;
			IConfigPtr m_pConfig;
			IConfigPtr m_pStaticConfig;
			IConfigPtr m_pSqlConfig;
		};
	}
}