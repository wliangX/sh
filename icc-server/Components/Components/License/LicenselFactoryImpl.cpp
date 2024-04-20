#include "Boost.h"
#include "LicenseFactoryImpl.h"
#include "LicenseImpl.h"

ICCFactoryCppInitialize(CLicenseFactoryImpl)

License::ILicensePtr CLicenseFactoryImpl::CreateLicense()
{
	if (!m_pLicense)
	{
		m_pLicense = boost::make_shared<CLicenseImpl>(m_IResourceManagerPtr);
	}
	return m_pLicense;
}
