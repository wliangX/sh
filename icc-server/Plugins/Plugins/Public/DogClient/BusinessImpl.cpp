#include "Boost.h"
#include "BusinessImpl.h"

void CBusinessImpl::OnInit()
{
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();

	if ("1" != m_pConfig->GetValue("ICC/Plugin/DogClient/IsUsing", "0"))
	{
		ICC_LOG_INFO(m_pLog, "there is no need to load the plugin");
		return;
	}
	
	boost::filesystem::path LibPath("");
	LibPath.append("CommandCenter.DogClientWrapper.dll");
	boost::dll::shared_library ImplLib(LibPath);
	m_dll = ImplLib;

	if (!m_dll.is_loaded())
	{
		ICC_LOG_INFO(m_pLog, "load CommandCenter.DogClientWrapper.dll failed");
		return;
	}

	ICC_LOG_INFO(m_pLog, "");

	if (m_dll.has("Init"))
	{
		try
		{
			auto& l_pInit = m_dll.get<void()>("Init");
			l_pInit();

			ICC_LOG_INFO(m_pLog, "Call DogClientWrapper Init Success");
		}		
		catch (...)
		{
			ICC_LOG_ERROR(m_pLog, "Call DogClientWrapper Init Failed!");
		}
		
	}
}

void CBusinessImpl::OnStart()
{
	if ("1" != m_pConfig->GetValue("ICC/Plugin/DogClient/IsUsing", "0"))
	{
		return;
	}

	if (!m_dll.is_loaded())
	{
		return;
	}

	ICC_LOG_INFO(m_pLog, "");

	if (m_dll.has("Start"))
	{
		try
		{
			auto& l_pStart = m_dll.get<void()>("Start");
			l_pStart();

			ICC_LOG_INFO(m_pLog, "Call DogClientWrapper Start Success");
		}		
		catch (...)
		{
			ICC_LOG_ERROR(m_pLog, "Call DogClientWrapper Start Failed!");
		}
		
	}
}

void CBusinessImpl::OnStop()
{
	if ("1" != m_pConfig->GetValue("ICC/Plugin/DogClient/IsUsing", "0"))
	{
		return;
	}

	if (!m_dll.is_loaded())
	{
		return;
	}

	ICC_LOG_INFO(m_pLog, "");

	if (m_dll.has("Stop"))
	{
		try
		{
			auto& l_pStop = m_dll.get<void()>("Stop");
			l_pStop();

			ICC_LOG_INFO(m_pLog, "Call DogClientWrapper Stop Success");
		}		
		catch (...)
		{
			ICC_LOG_ERROR(m_pLog, "Call DogClientWrapper Stop Failed!");
		}
		
	}
}

void CBusinessImpl::OnDestroy()
{
	if ("1" != m_pConfig->GetValue("ICC/Plugin/DogClient/IsUsing", "0"))
	{
		return;
	}

	if (!m_dll.is_loaded())
	{
		return;
	}

	ICC_LOG_INFO(m_pLog, "");
}
