#include "Boost.h"

#include "ControlCenter.h"
#include "SysConfig.h"
#include "TaskManager.h"
#include "AvayaSwitchManager.h"
#include "AgentManager.h"
#include "DeviceManager.h"
#include "BlackListManager.h"
#include "CallManager.h"

boost::shared_ptr<CControlCenter> CControlCenter::m_pInstance = nullptr;
CControlCenter::CControlCenter()
{
	//
}
CControlCenter::~CControlCenter()
{
	//ExitInstance();
}

boost::shared_ptr<CControlCenter> CControlCenter::Instance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = boost::make_shared<CControlCenter>();
	}

	return m_pInstance;
}

void CControlCenter::ExitInstance()
{
	if (m_pInstance)
	{
		m_pInstance.reset();
	}
}

void CControlCenter::OnInit(IResourceManagerPtr p_pResourceManager, ISwitchEventCallbackPtr p_pCallback)
{
	CSysConfig::Instance()->OnInit(p_pResourceManager);
	CBlackListManager::Instance()->OnInit(p_pResourceManager);
	CTaskManager::Instance()->OnInit(p_pResourceManager);
	CDeviceManager::Instance()->OnInit(p_pResourceManager);
	CAgentManager::Instance()->OnInit(p_pResourceManager);
	CCallManager::Instance()->OnInit(p_pResourceManager);
	CAvayaSwitchManager::Instance()->OnInit(p_pResourceManager, p_pCallback);
}
void CControlCenter::OnStart()
{
	CBlackListManager::Instance()->OnStart();
	CTaskManager::Instance()->OnStart();
	CDeviceManager::Instance()->OnStart();
	CAgentManager::Instance()->OnStart();
	CCallManager::Instance()->OnStart();
	CAvayaSwitchManager::Instance()->OnStart();
}
void CControlCenter::OnStop()
{
	CAvayaSwitchManager::Instance()->OnStop();
	CCallManager::Instance()->OnStop();
	CAgentManager::Instance()->OnStop();
	CDeviceManager::Instance()->OnStop();
	CTaskManager::Instance()->OnStop();
	CBlackListManager::Instance()->OnStop();
}
void CControlCenter::OnDestroy()
{
	CAvayaSwitchManager::Instance()->ExitInstance();
	CCallManager::Instance()->ExitInstance();
	CAgentManager::Instance()->ExitInstance();
	CDeviceManager::Instance()->ExitInstance();
	CTaskManager::Instance()->ExitInstance();
	CBlackListManager::Instance()->ExitInstance();
	CSysConfig::Instance()->ExitInstance();
}
