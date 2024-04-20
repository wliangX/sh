#include "Boost.h"
#include "TaskManager.h"


#define TASK_TIMEOUT		6	// 单位：秒，如命令执行失败，TSAPI 6 秒后才返回结果，超时时长不应小于 6 秒
//////////////////////////////////////////////////////////////////////////
//
CTask::CTask()
{
	m_lTaskId = 0;	
	m_nTaskType = Task_NULL;
	m_pSwitchNotif = nullptr;

	time(&m_tStartTime);
}


CTask::~CTask(void)
{
	//
}

void CTask::SetTime()
{
	time(&m_tStartTime);
}
long CTask::GetTaskTimeSpan()
{
	time_t l_tCurrentTime;
	time(&l_tCurrentTime);

	return static_cast<long>(difftime(l_tCurrentTime, m_tStartTime));
}
//////////////////////////////////////////////////////////////////////////
//
long CTaskManager::m_slTaskId = STATR_TASKID;
boost::shared_ptr<CTaskManager> CTaskManager::m_pInstance = nullptr;

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
	ClearExecutedTaskQueue();
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
}

void CTaskManager::OnStop()
{
	ClearTask();
}

void CTaskManager::ClearTask()
{
	ClearCmdTaskQueue();
	ClearExecutedTaskQueue();
	ClearSwitchEventTaskQueue();
}

long CTaskManager::CreateTaskId()
{
	if (m_slTaskId >= END_TASKID)
	{
		m_slTaskId = STATR_TASKID;
	}

	return (m_slTaskId++);
}

long CTaskManager::AddCmdTask(int p_nTaskType, ISwitchNotifPtr p_pSwitchNotif)
{
	long l_lTaskId = CreateTaskId();
	SAFE_LOCK(m_cmdTaskLock);
	boost::shared_ptr<CTask> l_pTask = boost::make_shared<CTask>();
	if (l_pTask)
	{
		p_pSwitchNotif->SetRequestId(l_lTaskId);

		l_pTask->SetTaskId(l_lTaskId);
		l_pTask->SetTaskType(p_nTaskType);
		l_pTask->SetSwitchNotif(p_pSwitchNotif);

		m_cmdTaskQueue[l_lTaskId] = l_pTask;
		ICC_LOG_LOWDEBUG(m_pLog, "Add CmdTask To Queue.requestId:[%d],requestType:[%d],QueueSize:[%u]",l_lTaskId, p_nTaskType, m_cmdTaskQueue.size());
	}
	else
	{
		ICC_LOG_WARNING(m_pLog, "Create Task Obj Failed!!!requestId:[%d]", l_lTaskId);
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

void CTaskManager::AddExecutedTask(const std::string &p_strMsgId,boost::shared_ptr<CTask> p_pTask)
{
	SAFE_LOCK(m_excuteTaskLock);
	if (p_pTask)
	{
		p_pTask->SetTime();
		m_excutedTaskQueue[p_strMsgId] = p_pTask;
	}
}

boost::shared_ptr<CTask> CTaskManager::GetExecutedTask(const std::string& p_strMsgId)
{
	boost::shared_ptr<CTask> l_pTask = nullptr;

	SAFE_LOCK(m_excuteTaskLock);
	auto iterTask = m_excutedTaskQueue.find(p_strMsgId);
	if (iterTask != m_excutedTaskQueue.end())
	{
		l_pTask = m_excutedTaskQueue[p_strMsgId];
	}

	return l_pTask;
}


bool CTaskManager::DeleteExecutedTask(const std::string& p_strMsgId)
{
	bool l_bReturn = false;

	SAFE_LOCK(m_excuteTaskLock);
	auto iterTask = m_excutedTaskQueue.find(p_strMsgId);
	if (iterTask != m_excutedTaskQueue.end())
	{
		l_bReturn = true;
		m_excutedTaskQueue.erase(iterTask);
	}

	return l_bReturn;
}


void CTaskManager::ClearExecutedTaskQueue()
{
	SAFE_LOCK(m_excuteTaskLock);
	m_excutedTaskQueue.clear();
}

bool CTaskManager::ExecuteTaskTimeout(int& p_nTaskType, std::string& p_strTaskMsgId)
{
	bool l_bRet = false;
	int l_nTimeout = TASK_TIMEOUT;// CSysConfig::Instance()->GetSwitchCmdTimeOut();

	SAFE_LOCK(m_excuteTaskLock);
	for (auto l_taskObj : m_excutedTaskQueue)
	{
		if (l_taskObj.second->GetTaskTimeSpan() > l_nTimeout/*TASK_TIMEOUT*/)
		{
			//	重置任务开始时间，避免不间断的检测到超时
			l_taskObj.second->SetTime();

			l_bRet = true;
			p_strTaskMsgId = l_taskObj.first;
			p_nTaskType = l_taskObj.second->GetTaskType();
			ICC_LOG_WARNING(m_pLog, "ExcusedTask timeout, TaskNameId:%d, MsgId: %s", p_nTaskType, p_strTaskMsgId.c_str());

			break;
		}
	}

	return l_bRet;
}


long CTaskManager::AddSwitchEventTask(int p_nTaskType, ISwitchNotifPtr p_pSwitchNotif)
{
	long l_lTaskId = this->CreateTaskId();

	SAFE_LOCK(m_switchEventTaskLock);
	boost::shared_ptr<CTask> l_pTask = boost::make_shared<CTask>();
	if (l_pTask)
	{
		l_pTask->SetTaskId(l_lTaskId);
		l_pTask->SetTaskType(p_nTaskType);
		l_pTask->SetSwitchNotif(p_pSwitchNotif);

		m_switchEventTaskQueue[l_lTaskId] = l_pTask;
		ICC_LOG_DEBUG(m_pLog, "Add SwitchEventTask To Queue, EventTaskId: [%u], TaskType: [%d], QueueSize: [%u]",
			l_lTaskId, p_nTaskType, m_switchEventTaskQueue.size());
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
