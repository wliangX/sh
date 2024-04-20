#pragma once
namespace ICC
{
	namespace License
	{
		class CLicenseFactoryImpl :
			public ILicenseFactory
		{
			ICCFactoryHeaderInitialize(CLicenseFactoryImpl)

		public:
			virtual ILicensePtr CreateLicense();

		private:
			License::ILicensePtr m_pLicense;
		};
	}
}