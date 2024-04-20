#include "Boost.h"
#include "TaskManager.h"
#include "SysConfig.h"

#define TASK_TIMEOUT		6	// 单位：秒，如命令执行失败，TSAPI 6 秒后才返回结果，超时时长不应小于 6 秒

long CTaskManager::m_slTaskId = STATR_TASKID;
boost::shared_ptr<CTaskManager> CTaskManager::m_pInstance = nullptr;
CTaskManager::CTaskManager(void)
{
	//
}
CTaskManager::~CTaskManager(void)
{
	//
}

boost::shared_ptr<CTaskManager> CTaskManager::Instance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = boost::make_shared<CTaskManager>();
	}

	return m_pInstance;
}
void CTaskManager::ExitInstance()
{
	ClearCmdTaskQueue();
	ClearExcutedTaskQueue();
	ClearSwitchEventTaskQueue();

	if (m_pInstance)
	{
		m_pInstance.reset();
	}
}

void CTaskManager::OnInit(IResourceManagerPtr p_pResourceManager)
{
	m_pLog = ICCGetResourceEx(Log::ILogFactory, ICCILogFactoryResourceName, p_pResourceManager)->GetLogger(MODULE_NAME);
}
void CTaskManager::OnStart()
{
	//
}
void CTaskManager::OnStop()
{
	ClearTask();
}

void CTaskManager::ClearTask()
{
	ClearCmdTaskQueue();
	ClearExcutedTaskQueue();
	ClearSwitchEventTaskQueue();
}
//////////////////////////////////////////////////////////////////////////
long CTaskManager::CreateTaskId()
{
	if (m_slTaskId >= END_TASKID)
	{
		m_slTaskId = STATR_TASKID;
	}

	return (m_slTaskId++);
}
boost::shared_ptr<CTask> CTaskManager::CreateNewTask()
{
	long lTaskId = CTaskManager::Instance()->CreateTaskId();
	boost::shared_ptr<CTask> pTask = boost::make_shared<CTask>();
	if (pTask)
	{
		pTask->SetTaskId(lTaskId);
		pTask->SetTaskName(Task_NULL);
	}

	return pTask;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void CTaskManager::AddCmdTask(boost::shared_ptr<CTask> p_pTask)
{
	SAFE_LOCK(m_cmdTaskLock);
	if (p_pTask)
	{
		m_cmdTaskQueue[p_pTask->GetTaskId()] = p_pTask;
	}
}
long CTaskManager::AddCmdTask(E_TASK_NAME p_nTaskName, ISwitchNotifPtr p_pSwitchNotif)
{
	long l_lTaskId = this->CreateTaskId();

	SAFE_LOCK(m_cmdTaskLock);
	boost::shared_ptr<CTask> l_pTask = boost::make_shared<CTask>();
	if (l_pTask)
	{
		p_pSwitchNotif->SetRequestId(l_lTaskId);

		l_pTask->SetTaskId(l_lTaskId);
		l_pTask->SetTaskName(p_nTaskName);
		l_pTask->SetSwitchNotif(p_pSwitchNotif);

		m_cmdTaskQueue[l_lTaskId] = l_pTask;
		ICC_LOG_LOWDEBUG(m_pLog, "Add CmdTask To Queue, TaskId: [%u], TaskName: [%s], QueueSize: [%u]",
			l_lTaskId, TaskNameString[p_nTaskName].c_str(), m_cmdTaskQueue.size());
	}
	else
	{
		ICC_LOG_WARNING(m_pLog, "TaskId: [%u], Create Task Obj Failed !!!", l_lTaskId);
	}

	return l_lTaskId;
}


boost::shared_ptr<CTask> CTaskManager::GetCmdTaskHeader()
{
	boost::shared_ptr<CTask> l_pTask = nullptr;

	if (GetCmdTaskCount() > 0)
	{
		SAFE_LOCK(m_cmdTaskLock);
		auto it = m_cmdTaskQueue.begin();
		if (it != m_cmdTaskQueue.end())
		{
			l_pTask = it->second;
		}
	}

	return l_pTask;
}

bool CTaskManager::DeleteCmdTask(long p_lTaskId)
{
	bool l_bReturn = false;

	SAFE_LOCK(m_cmdTaskLock);
	auto iterTask = m_cmdTaskQueue.find(p_lTaskId);
	if (iterTask != m_cmdTaskQueue.end())
	{
		l_bReturn = true;
		m_cmdTaskQueue.erase(iterTask);
	}

	return l_bReturn;
}

void CTaskManager::ClearCmdTaskQueue()
{
	SAFE_LOCK(m_cmdTaskLock);
	m_cmdTaskQueue.clear();
}

int CTaskManager::GetCmdTaskCount()
{
	int nCount = 0;

	SAFE_LOCK(m_cmdTaskLock);
	nCount = m_cmdTaskQueue.size();

	return nCount;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void CTaskManager::AddExcutedTask(boost::shared_ptr<CTask> p_pTask)
{
	SAFE_LOCK(m_excuteTaskLock);
	if (p_pTask)
	{
		p_pTask->SetTime();
		m_excutedTaskQueue[p_pTask->GetTaskId()] = p_pTask;
	}
}

boost::shared_ptr<CTask> CTaskManager::GetExcutedTask(long p_lTaskId)
{
	boost::shared_ptr<CTask> l_pTask = nullptr;

	SAFE_LOCK(m_excuteTaskLock);
	auto iterTask = m_excutedTaskQueue.find(p_lTaskId);
	if (iterTask != m_excutedTaskQueue.end())
	{
		l_pTask = m_excutedTaskQueue[p_lTaskId];
	}

	return l_pTask;
}

boost::shared_ptr<CTask> CTaskManager::GetExcutedTask(const std::string& p_strTaskId)
{
	boost::shared_ptr<CTask> l_pTask = nullptr;
	long l_lTaskId = atol(p_strTaskId.c_str());

	SAFE_LOCK(m_excuteTaskLock);
	auto iterTask = m_excutedTaskQueue.find(l_lTaskId);
	if (iterTask != m_excutedTaskQueue.end())
	{
		l_pTask = m_excutedTaskQueue[l_lTaskId];
	}

	return l_pTask;
}

bool CTaskManager::DeleteExcusedTask(const std::string& p_strTaskId)
{
	bool l_bReturn = false;
	long l_lTaskId = atol(p_strTaskId.c_str());

	SAFE_LOCK(m_excuteTaskLock);
	auto iterTask = m_excutedTaskQueue.find(l_lTaskId);
	if (iterTask != m_excutedTaskQueue.end())
	{
		l_bReturn = true;
		m_excutedTaskQueue.erase(iterTask);
	}

	return l_bReturn;
}

bool CTaskManager::DeleteExcusedTask(long p_lTaskId)
{
	bool l_bReturn = false;

	SAFE_LOCK(m_excuteTaskLock);
	auto iterTask = m_excutedTaskQueue.find(p_lTaskId);
	if (iterTask != m_excutedTaskQueue.end())
	{
		l_bReturn = true;
		m_excutedTaskQueue.erase(iterTask);
	}

	return l_bReturn;
}

void CTaskManager::ClearExcutedTaskQueue()
{
	SAFE_LOCK(m_excuteTaskLock);
	m_excutedTaskQueue.clear();
}
bool CTaskManager::ExcuseTaskTimeout(E_TASK_NAME& p_nTaskName, long& p_lTaskId)
{
	bool l_bRet = false;
	int l_nTimeout = CSysConfig::Instance()->GetSwitchCmdTimeOut();

	SAFE_LOCK(m_excuteTaskLock);
	for (auto l_taskObj : m_excutedTaskQueue)
	{
		if (l_taskObj.second->GetTaskTimeSpan() > l_nTimeout/*TASK_TIMEOUT*/)
		{
			//	重置任务开始时间，避免不间断的检测到超时
			l_taskObj.second->SetTime();

			l_bRet = true;
			p_lTaskId = l_taskObj.first;
			p_nTaskName = l_taskObj.second->GetTaskName();
			ICC_LOG_WARNING(m_pLog, "ExcusedTask timeout, TaskName:%s, TaskId: %u",
				l_taskObj.second->GetTaskNameString(l_taskObj.second->GetTaskName()).c_str(), p_lTaskId);

			break;
		}
	}

	return l_bRet;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void CTaskManager::AddSwitchEventTask(boost::shared_ptr<CTask> p_pTask)
{
	SAFE_LOCK(m_switchEventTaskLock);
	if (p_pTask)
	{
		m_switchEventTaskQueue[p_pTask->GetTaskId()] = p_pTask;
	}
}
long CTaskManager::AddSwitchEventTask(E_TASK_NAME p_nTaskName, ISwitchNotifPtr p_pSwitchNotif)
{
	long l_lTaskId = this->CreateTaskId();

	SAFE_LOCK(m_switchEventTaskLock);
	boost::shared_ptr<CTask> l_pTask = boost::make_shared<CTask>();
	if (l_pTask)
	{
		l_pTask->SetTaskId(l_lTaskId);
		l_pTask->SetTaskName(p_nTaskName);
		l_pTask->SetSwitchNotif(p_pSwitchNotif);

		m_switchEventTaskQueue[l_lTaskId] = l_pTask;
		ICC_LOG_LOWDEBUG(m_pLog, "Add SwitchEventTask To Queue, TaskId: [%u], TaskName: [%s], QueueSize: [%u]",
			l_lTaskId, TaskNameString[p_nTaskName].c_str(), m_switchEventTaskQueue.size());
	}
	else
	{
		ICC_LOG_WARNING(m_pLog, "TaskId: [%u], Create Task Obj Failed !!!", l_lTaskId);
	}

	return l_lTaskId;
}

boost::shared_ptr<CTask> CTaskManager::GetSwitchEventTaskHeader()
{
	boost::shared_ptr<CTask> l_pTask = nullptr;

//	if (GetSwitchEventTaskCount() > 0)
	{
		SAFE_LOCK(m_switchEventTaskLock);
		auto it = m_switchEventTaskQueue.begin();
		if (it != m_switchEventTaskQueue.end())
		{
			l_pTask = it->second;
		}
	}

	return l_pTask;
}

bool CTaskManager::DeleteSwitchEventTask(long p_lTaskId)
{
	bool l_bReturn = false;

	SAFE_LOCK(m_switchEventTaskLock);
	auto iterTask = m_switchEventTaskQueue.find(p_lTaskId);
	if (iterTask != m_switchEventTaskQueue.end())
	{
		l_bReturn = true;
		m_switchEventTaskQueue.erase(iterTask);
	}

	return l_bReturn;
}

void CTaskManager::ClearSwitchEventTaskQueue()
{
	SAFE_LOCK(m_switchEventTaskLock);
	m_switchEventTaskQueue.clear();
}

int CTaskManager::GetSwitchEventTaskCount()
{
	int l_nCount = 0;

	SAFE_LOCK(m_switchEventTaskLock);
	l_nCount = m_switchEventTaskQueue.size();

	return l_nCount;
}

//////////////////////////////////////////////////////////////////////////
