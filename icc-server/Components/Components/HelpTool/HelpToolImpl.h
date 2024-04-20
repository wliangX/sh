#pragma once

namespace ICC
{
	namespace HelpTool
	{
		class CHelpToolImpl :
			public IHelpTool
		{
		public:
			CHelpToolImpl(IResourceManagerPtr p_ResourceManagerPtr);
			 ~CHelpToolImpl();

		public:
			virtual unsigned int HostToNetworkInt(unsigned int p_iValue);
			virtual unsigned short HostToNetworkShort(unsigned short p_sValue);
			virtual unsigned int NetworkToHostInt(unsigned int p_iValue);
			virtual unsigned short NetworkToHostShort(unsigned short p_sValue);
		
		public:
			virtual unsigned int IpToInt(std::string p_strIp);
		public:
			virtual void Sleep(unsigned int p_iMSec);
		public:
			virtual void ScanFilesRecursive(const std::string p_dir, const std::string p_filter, std::list<std::string>& p_Files);
		public:
			virtual IResourceManagerPtr GetResourceManager();

		private:
			IResourceManagerPtr m_pResourceManager;
			StringUtil::IStringUtilPtr m_pString;
		};
	}
}