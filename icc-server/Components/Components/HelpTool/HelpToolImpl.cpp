#include "Boost.h"
#include "HelpToolImpl.h"

CHelpToolImpl::CHelpToolImpl(IResourceManagerPtr p_ResourceManagerPtr) :
m_pResourceManager(p_ResourceManagerPtr)
{
	m_pString = ICCGetIStringFactory()->CreateString();
}

CHelpToolImpl::~CHelpToolImpl()
{

}
unsigned int CHelpToolImpl::HostToNetworkInt(unsigned int p_iValue)
{
	return boost::asio::detail::socket_ops::host_to_network_long(p_iValue);
}

unsigned short CHelpToolImpl::HostToNetworkShort(unsigned short p_sValue)
{
	return boost::asio::detail::socket_ops::host_to_network_short(p_sValue);
}

unsigned int CHelpToolImpl::NetworkToHostInt(unsigned int p_iValue)
{
	return boost::asio::detail::socket_ops::network_to_host_long(p_iValue);
}

unsigned short CHelpToolImpl::NetworkToHostShort(unsigned short p_sValue)
{
	return boost::asio::detail::socket_ops::network_to_host_short(p_sValue);
}

unsigned int CHelpToolImpl::IpToInt(std::string p_strIp)
{
	return NetworkToHostInt(inet_addr(p_strIp.c_str()));
}

void CHelpToolImpl::Sleep(unsigned int p_iMSec)
{
	try
	{
		boost::this_thread::sleep(boost::posix_time::millisec(p_iMSec));
	}
	catch (std::exception &e)
	{
		printf("CHelpToolImpl::Sleep exception: %s\n", e.what());
	}
	catch (...)
	{
		printf("CHelpToolImpl::Sleep exception\n");
	}
}

void CHelpToolImpl::ScanFilesRecursive(const std::string p_dir, const std::string p_filter, std::list<std::string>& p_Files)
{
	namespace fs = boost::filesystem;
	fs::path l_fullpath(p_dir);

	if (!fs::exists(l_fullpath))
	{
		return;
	}

	fs::directory_iterator end_iter;
	for (fs::directory_iterator iter(l_fullpath); iter != end_iter; ++iter)
	{
		try
		{
			if (fs::is_directory(*iter))
			{
				ScanFilesRecursive(iter->path().string(), p_filter, p_Files);
			}
			else
			{
				std::string fileName = iter->path().string();
				if (!p_filter.empty())
				{
					if (m_pString->EndsWith(fileName, p_filter))
					{
						p_Files.push_back(fileName);
					}
				}
				else
				{
					p_Files.push_back(fileName);
				}							
			}
		}
		catch (const std::exception & ex)
		{
			continue;
		}
	}
}

IResourceManagerPtr ICC::HelpTool::CHelpToolImpl::GetResourceManager()
{
	return m_pResourceManager;
}

