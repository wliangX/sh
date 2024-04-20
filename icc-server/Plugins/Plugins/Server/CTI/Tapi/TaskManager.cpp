#include "Boost.h"
#include "TaskManager.h"
#include "SysConfig.h"

#define SLEEP_TIMESPAN		300
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
	//
}

//////////////////////////////////////////////////////////////////////////
long CTaskManager::CreateTaskId()
{
	if (m_slTaskId > END_TASKID)
	{
		m_slTaskId = STATR_TASKID;
	}

	return (m_slTaskId++);
}
boost::shared_ptr<CTask> CTaskManager::CreateNewTask(E_TASK_NAME p_nInTaskName, ISwitchNotifPtr p_pInSwitchNotif, long& p_lOutTaskId)
{
	p_lOutTaskId = this->CreateTaskId();

	boost::shared_ptr<CTask> l_pTask = boost::make_shared<CTask>();
	if (l_pTask)
	{
		p_pInSwitchNotif->SetRequestId(p_lOutTaskId);

		l_pTask->SetTaskId(p_lOutTaskId);
		l_pTask->SetTaskName(p_nInTaskName);
		l_pTask->SetSwitchNotif(p_pInSwitchNotif);
	}
	else
	{
		ICC_LOG_WARNING(m_pLog, "TaskId: [%u], Create Task Obj Failed !!!", p_lOutTaskId);
	}

	return l_pTask;
}
boost::shared_ptr<CTask> CTaskManager::CreateNewTask()
{
	long l_lTaskId = this->CreateTaskId();

	boost::shared_ptr<CTask> l_pTask = boost::make_shared<CTask>();
	if (l_pTask)
	{
		l_pTask->SetTaskId(l_lTaskId);
		l_pTask->SetTaskName(Task_NULL);
	}
	else
	{
		ICC_LOG_WARNING(m_pLog, "TaskId: [%u], Create Task Obj Failed !!!", l_lTaskId);
	}

	return l_pTask;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void CTaskManager::AddCmdTask(boost::shared_ptr<CTask> p_pTask)
{
	if (p_pTask)
	{
		long l_lTaskId = p_pTask->GetTaskId();

		std::lock_guard<std::mutex> guard(m_cmdTaskLock);
		m_cmdTaskQueue[l_lTaskId] = p_pTask;

		ICC_LOG_LOWDEBUG(m_pLog, "Add CmdTask To Queue, TaskId: [%u], QueueSize: [%u]", l_lTaskId, m_cmdTaskQueue.size());
	}
}
long CTaskManager::AddCmdTask(E_TASK_NAME p_nTaskName, ISwitchNotifPtr p_pSwitchNotif)
{
	long l_lTaskId = this->CreateTaskId();

	std::lock_guard<std::mutex> guard(m_cmdTaskLock);
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
		std::lock_guard<std::mutex> guard(m_cmdTaskLock);
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

	std::lock_guard<std::mutex> guard(m_cmdTaskLock);
	auto iterTask = m_cmdTaskQueue.find(p_lTaskId);
	if (iterTask != m_cmdTaskQueue.end())
	{
		l_bReturn = true;
		m_cmdTaskQueue.erase(iterTask);

		ICC_LOG_LOWDEBUG(m_pLog, "Delete CmdTask From Queue, TaskId: [%u], QueueSize: [%u]", p_lTaskId, m_cmdTaskQueue.size());
	}

	return l_bReturn;
}

void CTaskManager::ClearCmdTaskQueue()
{
	std::lock_guard<std::mutex> guard(m_cmdTaskLock);
	m_cmdTaskQueue.clear();

	ICC_LOG_DEBUG(m_pLog, "Clear CmdTask Queue");
}

int CTaskManager::GetCmdTaskCount()
{
	int nCount = 0;

	std::lock_guard<std::mutex> guard(m_cmdTaskLock);
	nCount = m_cmdTaskQueue.size();

	return nCount;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void CTaskManager::AddExcutedTask(boost::shared_ptr<CTask> p_pTask)
{
	if (p_pTask)
	{
		p_pTask->SetTime();
		long l_lTaskId = p_pTask->GetTaskId();

		std::lock_guard<std::mutex> guard(m_excuteTaskLock);
		m_excutedTaskQueue[l_lTaskId] = p_pTask;

		ICC_LOG_LOWDEBUG(m_pLog, "Add ExcutedTask To Queue, TaskId: [%u], QueueSize: [%u]",
			l_lTaskId, m_cmdTaskQueue.size());
	}
}

boost::shared_ptr<CTask> CTaskManager::GetExcutedTask(long p_lTaskId)
{
	boost::shared_ptr<CTask> l_pTask = nullptr;

	std::lock_guard<std::mutex> guard(m_excuteTaskLock);
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

	std::lock_guard<std::mutex> guard(m_excuteTaskLock);
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

	std::lock_guard<std::mutex> guard(m_excuteTaskLock);
	auto iterTask = m_excutedTaskQueue.find(l_lTaskId);
	if (iterTask != m_excutedTaskQueue.end())
	{
		l_bReturn = true;
		m_excutedTaskQueue.erase(iterTask);

		ICC_LOG_LOWDEBUG(m_pLog, "Delete ExcutedTask From Queue, TaskId: [%u], QueueSize: [%u]",
			l_lTaskId, m_excutedTaskQueue.size());
	}

	return l_bReturn;
}

bool CTaskManager::DeleteExcusedTask(long p_lTaskId)
{
	bool l_bReturn = false;

	std::lock_guard<std::mutex> guard(m_excuteTaskLock);
	auto iterTask = m_excutedTaskQueue.find(p_lTaskId);
	if (iterTask != m_excutedTaskQueue.end())
	{
		l_bReturn = true;
		m_excutedTaskQueue.erase(iterTask);
		ICC_LOG_LOWDEBUG(m_pLog, "Delete ExcutedTask From Queue, TaskId: [%u], QueueSize: [%u]",
			p_lTaskId, m_excutedTaskQueue.size());
	}

	return l_bReturn;
}

void CTaskManager::ClearExcutedTaskQueue()
{
	std::lock_guard<std::mutex> guard(m_excuteTaskLock);
	m_excutedTaskQueue.clear();

	ICC_LOG_DEBUG(m_pLog, "Clear ExcutedTask Queue");
}
bool CTaskManager::ExcuseTaskTimeout(E_TASK_NAME& p_nTaskName, long& p_lTaskId)
{
	bool l_bRet = false;
	int l_nTimeout = CSysConfig::Instance()->GetSwitchCmdTimeOut();

	std::lock_guard<std::mutex> guard(m_excuteTaskLock);
	for each (auto iterTask in m_excutedTaskQueue)
	{
		if (iterTask.second && iterTask.second->GetTaskTimeSpan() > l_nTimeout/*TASK_TIMEOUT*/)
		{
			//	重置任务开始时间，避免不间断的检测到超时
			iterTask.second->SetTime();

			l_bRet = true;
			p_lTaskId = iterTask.first;
			p_nTaskName = iterTask.second->GetTaskName();
			ICC_LOG_WARNING(m_pLog, "ExcusedTask timeout, TaskName:%s, TaskId: %u",
				iterTask.second->GetTaskNameString(iterTask.second->GetTaskName()).c_str(), p_lTaskId);

			break;
		}
	}

	return l_bRet;
}
void CTaskManager::SetInvokeId(long p_lTaskId, long p_lInvokeId)
{
	std::lock_guard<std::mutex> guard(m_excuteTaskLock);
	auto iterTask = m_excutedTaskQueue.find(p_lTaskId);
	if (iterTask != m_excutedTaskQueue.end())
	{
		if (iterTask->second)
		{
			iterTask->second->SetInvokeId(p_lInvokeId);
		}
	}
}
long CTaskManager::GetTaskIdByInvokeId(long p_lInvokeId)
{
	long l_lTaskId = DEFAULT_TASKID;

	std::lock_guard<std::mutex> guard(m_excuteTaskLock);
	for each (auto iterTask in m_excutedTaskQueue)
	{
		if (iterTask.second->GetInvokeId() == p_lInvokeId)
		{
			l_lTaskId = iterTask.first;

			break;
		}
	}

	return l_lTaskId;
}
E_TASK_NAME CTaskManager::GetTaskNameByInvokeId(long p_lInvokeId)
{
	E_TASK_NAME l_nTaskName = Task_NULL;

	std::lock_guard<std::mutex> guard(m_excuteTaskLock);
	for each (auto iterTask in m_excutedTaskQueue)
	{
		if (iterTask.second && iterTask.second->GetInvokeId() == p_lInvokeId)
		{
			l_nTaskName = iterTask.second->GetTaskName();

			break;
		}
	}

	return l_nTaskName;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void CTaskManager::AddSwitchEventTask(boost::shared_ptr<CTask> p_pTask)
{
	if (p_pTask)
	{
		long l_lTaskId = p_pTask->GetTaskId();

		std::lock_guard<std::mutex> guard(m_switchEventTaskLock);
		m_switchEventTaskQueue[l_lTaskId] = p_pTask;
		ICC_LOG_LOWDEBUG(m_pLog, "Add SwitchEventTask To Queue, TaskId: [%u], QueueSize: [%u]", l_lTaskId, m_switchEventTaskQueue.size());
	}
}
long CTaskManager::AddSwitchEventTask(E_TASK_NAME p_nTaskName, ISwitchNotifPtr p_pSwitchNotif)
{
	long l_lTaskId = this->CreateTaskId();

	std::lock_guard<std::mutex> guard(m_switchEventTaskLock);
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
		std::lock_guard<std::mutex> guard(m_switchEventTaskLock);
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

	std::lock_guard<std::mutex> guard(m_switchEventTaskLock);
	auto iterTask = m_switchEventTaskQueue.find(p_lTaskId);
	if (iterTask != m_switchEventTaskQueue.end())
	{
		l_bReturn = true;
		m_switchEventTaskQueue.erase(iterTask);
		ICC_LOG_LOWDEBUG(m_pLog, "Delete SwitchEventTask From Queue, TaskId: [%u], QueueSize: [%u]", p_lTaskId, m_switchEventTaskQueue.size());
	}

	return l_bReturn;
}

void CTaskManager::ClearSwitchEventTaskQueue()
{
	std::lock_guard<std::mutex> guard(m_switchEventTaskLock);
	m_switchEventTaskQueue.clear();
	ICC_LOG_DEBUG(m_pLog, "Clear SwitchEventTask Queue");
}

int CTaskManager::GetSwitchEventTaskCount()
{
	int l_nCount = 0;

	std::lock_guard<std::mutex> guard(m_switchEventTaskLock);
	l_nCount = m_switchEventTaskQueue.size();

	return l_nCount;
}

//////////////////////////////////////////////////////////////////////////
long CTaskManager::AgentLogin(const std::string& p_strAgent, const std::string& p_strACD)
{
	long l_lNewTaskId = DEFAULT_TASKID;

	ISetAgentStateNotifPtr l_pRequestNotif = boost::make_shared<CSetAgentStateNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetAgentId(p_strAgent);
		l_pRequestNotif->SetACDGrp(p_strACD);
		l_pRequestNotif->SetLoginMode(LoginMode_Login);

		l_lNewTaskId = this->AddCmdTask(Task_AgentLogin, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], AgentLogin, AgentId [%s], ACD [%s]",
			l_lNewTaskId, p_strAgent.c_str(), p_strACD.c_str());
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSetAgentStateNotif Object Failed !!!");
	}

	return l_lNewTaskId;
}
long CTaskManager::AgentLogout(const std::string& p_strAgent, const std::string& p_strACD)
{
	long l_lNewTaskId = DEFAULT_TASKID;

	ISetAgentStateNotifPtr l_pRequestNotif = boost::make_shared<CSetAgentStateNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetAgentId(p_strAgent);
		l_pRequestNotif->SetACDGrp(p_strACD);
		l_pRequestNotif->SetLoginMode(LoginMode_Logout);

		l_lNewTaskId = this->AddCmdTask(Task_AgentLogout, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], AgentLogout, AgentId [%s], ACD [%s]",
			l_lNewTaskId, p_strAgent.c_str(), p_strACD.c_str());
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSetAgentStateNotif Object Failed !!!");
	}

	return l_lNewTaskId;
}

long CTaskManager::SetAgentState(const std::string& p_strAgent, const std::string& p_strACD, const std::string& p_strReadyState, const std::string& p_strLogoutReason)
{
	long l_lNewTaskId = DEFAULT_TASKID;

	ISetAgentStateNotifPtr l_pRequestNotif = boost::make_shared<CSetAgentStateNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetAgentId(p_strAgent);
		l_pRequestNotif->SetACDGrp(p_strACD);
		l_pRequestNotif->SetReadyState(p_strReadyState);

		l_lNewTaskId = this->AddCmdTask(Task_SetAgentState, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], SetAgentState, AgentId [%s], ACD [%s], State [%s]",
			l_lNewTaskId, p_strAgent.c_str(), p_strACD.c_str(), p_strReadyState.c_str());
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSetAgentStateNotif Object Failed !!!");
	}

	return l_lNewTaskId;
}

long CTaskManager::MakeCall(const std::string& p_strCallerId, const std::string& p_strCalledId, const std::string& p_strTargetType, const std::string& p_strCaseId)
{
	long l_lNewTaskId = DEFAULT_TASKID;

	IMakeCallNotifPtr l_pRequestNotif = boost::make_shared<CMakeCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetCallerId(p_strCallerId);
		l_pRequestNotif->SetCalledId(p_strCalledId);
		l_pRequestNotif->SetTargetPhoneType(p_strTargetType);
		l_pRequestNotif->SetCaseId(p_strCaseId);

		l_lNewTaskId = this->AddCmdTask(Task_MakeCall, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], MakeCall, CallerId [%s], CalledId [%s], TargetType [%s]",
			l_lNewTaskId, p_strCallerId.c_str(), p_strCalledId.c_str(), p_strTargetType.c_str());
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CMakeCallNotif Object Failed !!!");
	}

	return l_lNewTaskId;
}

long CTaskManager::AnswerCall(const std::string& p_strCTICallRefId, const std::string& p_strDevice)
{
	long l_lNewTaskId = DEFAULT_TASKID;

	IAnswerCallNotifPtr l_pRequestNotif = boost::make_shared<CAnswerCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetCTICallRefId(p_strCTICallRefId);
		l_pRequestNotif->SetDeviceNum(p_strDevice);

		l_lNewTaskId = this->AddCmdTask(Task_AnswerCall, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], AnswerCall, CallRefId [%s], Device [%s]",
			l_lNewTaskId, p_strCTICallRefId.c_str(), p_strDevice.c_str());
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CAnswerCallNotif Object Failed !!!");
	}

	return l_lNewTaskId;
}

long CTaskManager::RefuseAnswer(const std::string& p_strCTICallRefId, const std::string& p_strDevice)
{
	long l_lNewTaskId = DEFAULT_TASKID;

	IAnswerCallNotifPtr l_pRequestNotif = boost::make_shared<CAnswerCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetCTICallRefId(p_strCTICallRefId);
		l_pRequestNotif->SetDeviceNum(p_strDevice);

		l_lNewTaskId = this->AddCmdTask(Task_RefuseAnswer, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], RefuseAnswer, CallRefId [%s], Device [%s]",
			l_lNewTaskId, p_strCTICallRefId.c_str(), p_strDevice.c_str());
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CAnswerCallNotif Object Failed !!!");
	}

	return l_lNewTaskId;
}

long CTaskManager::Hangup(const std::string& p_strCTICallRefId, const std::string & p_strSponsor, const std::string& p_strDevice)
{
	long l_lNewTaskId = DEFAULT_TASKID;

	IAnswerCallNotifPtr l_pRequestNotif = boost::make_shared<CAnswerCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetCTICallRefId(p_strCTICallRefId);
		l_pRequestNotif->SetSponsor(p_strSponsor);
		l_pRequestNotif->SetDeviceNum(p_strDevice);

		l_lNewTaskId = this->AddCmdTask(Task_Hangup, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], Hangup, CallRefId [%s], Sponsor [%s] Device [%s]",
			l_lNewTaskId, p_strCTICallRefId.c_str(), p_strSponsor.c_str(), p_strDevice.c_str());
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CAnswerCallNotif Object Failed !!!");
	}

	return l_lNewTaskId;
}

long CTaskManager::Hangup(long p_strCallRefId, const std::string & p_strSponsor, const std::string& p_strDevice)
{
	long l_lNewTaskId = DEFAULT_TASKID;

	IAnswerCallNotifPtr l_pRequestNotif = boost::make_shared<CAnswerCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetCSTACallRefId(p_strCallRefId);
		l_pRequestNotif->SetSponsor(p_strSponsor);
		l_pRequestNotif->SetDeviceNum(p_strDevice);

		l_lNewTaskId = this->AddCmdTask(Task_Hangup, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], Hangup, CallRefId [%u], Sponsor [%s] Device [%s]",
			l_lNewTaskId, p_strCallRefId, p_strSponsor.c_str(), p_strDevice.c_str());
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CAnswerCallNotif Object Failed !!!");
	}

	return l_lNewTaskId;
}

long CTaskManager::ForceHangup(long p_strCallRefId, const std::string& p_strSponsor, const std::string& p_strDevice)
{
	long l_lNewTaskId = DEFAULT_TASKID;

	IAnswerCallNotifPtr l_pRequestNotif = boost::make_shared<CAnswerCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetCSTACallRefId(p_strCallRefId);
		l_pRequestNotif->SetSponsor(p_strSponsor);
		l_pRequestNotif->SetDeviceNum(p_strDevice);

		//l_lNewTaskId = this->AddCmdTask(Task_ForceHangup, l_pRequestNotif);
		//ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], ForceHangup, CallRefId [%u], Sponsor [%s] Device [%s]",
		//	l_lNewTaskId, p_strCallRefId, p_strSponsor.c_str(), p_strDevice.c_str());
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CAnswerCallNotif Object Failed !!!");
	}

	return l_lNewTaskId;
}

long CTaskManager::ClearCall(const std::string& p_strCTICallRefId, const std::string & p_strSponsor)
{
	long l_lNewTaskId = DEFAULT_TASKID;

	IClearCallNotifPtr l_pRequestNotif = boost::make_shared<CClearCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetCTICallRefId(p_strCTICallRefId);
		l_pRequestNotif->SetSponsor(p_strSponsor);

		l_lNewTaskId = this->AddCmdTask(Task_ClearCall, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], ClearCall, CallRefId [%s], Sponsor [%s]",
			l_lNewTaskId, p_strCTICallRefId.c_str(), p_strSponsor.c_str());
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CClearCallNotif Object Failed !!!");
	}

	return l_lNewTaskId;
}

long CTaskManager::ListenCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget)
{
	long l_lNewTaskId = DEFAULT_TASKID;

	IListenCallNotifPtr l_pRequestNotif = boost::make_shared<CListenCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetCTICallRefId(p_strCTICallRefId);
		l_pRequestNotif->SetSponsor(p_strSponsor);
		l_pRequestNotif->SetTarget(p_strTarget);

		l_lNewTaskId = this->AddCmdTask(Task_ListenCall, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], ListenCall, CallRefId [%s], Sponsor [%s], Target [%s]",
			l_lNewTaskId, p_strCTICallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create  CListenCallNotifObject Failed !!!");
	}

	return l_lNewTaskId;
}

long CTaskManager::PickupCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget)
{
	long l_lNewTaskId = DEFAULT_TASKID;

	IPickupCallNotifPtr l_pRequestNotif = boost::make_shared<CPickupCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetCTICallRefId(p_strCTICallRefId);
		l_pRequestNotif->SetSponsor(p_strSponsor);
		l_pRequestNotif->SetTarget(p_strTarget);

		l_lNewTaskId = this->AddCmdTask(Task_PickupCall, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], PickupCall, CallRefId [%s], Sponsor [%s], Target [%s]",
			l_lNewTaskId, p_strCTICallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CPickupCallNotif Object Failed !!!");
	}

	return l_lNewTaskId;
}

long CTaskManager::BargeInCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget)
{
	long l_lNewTaskId = DEFAULT_TASKID;

	IBargeInCallNotifPtr l_pRequestNotif = boost::make_shared<CBargeInCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetCTICallRefId(p_strCTICallRefId);
		l_pRequestNotif->SetSponsor(p_strSponsor);
		l_pRequestNotif->SetTarget(p_strTarget);

		l_lNewTaskId = this->AddCmdTask(Task_BargeInCall, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], BargeInCall, CallRefId [%s], Sponsor [%s], Target [%s]",
			l_lNewTaskId, p_strCTICallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CBargeInCallNotif Object Failed !!!");
	}

	return l_lNewTaskId;
}

long CTaskManager::ForcePopCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget)
{
	long l_lNewTaskId = DEFAULT_TASKID;

	IForcePopCallNotifPtr l_pRequestNotif = boost::make_shared<CForcePopCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetCTICallRefId(p_strCTICallRefId);
		l_pRequestNotif->SetSponsor(p_strSponsor);
		l_pRequestNotif->SetTarget(p_strTarget);

		l_lNewTaskId = this->AddCmdTask(Task_ForcePopCall, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], ForcePopCall, CallRefId [%s], Sponsor [%s], Target [%s]",
			l_lNewTaskId, p_strCTICallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CForcePopCallNotif Object Failed !!!");
	}

	return l_lNewTaskId;
}

long CTaskManager::ConsultationCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget, const std::string& p_strTargetDeviceType, bool p_bIsTransferCall)
{
	long l_lNewTaskId = DEFAULT_TASKID;

	IConsultationCallNotifPtr l_pRequestNotif = boost::make_shared<CConsultationCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetCTICallRefId(p_strCTICallRefId);
		l_pRequestNotif->SetSponsor(p_strSponsor);
		l_pRequestNotif->SetTargetDevice(p_strTarget);
		l_pRequestNotif->SetTargetDeviceType(p_strTargetDeviceType);
		l_pRequestNotif->SetIsTransferCall(p_bIsTransferCall);

		l_lNewTaskId = this->AddCmdTask(Task_ConsultationCall, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], ConsultationCall, CallRefId [%s], Sponsor [%s], Target [%s], TargetDeviceType [%s]",
			l_lNewTaskId, p_strCTICallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str(), p_strTargetDeviceType.c_str());
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CConsultationCallNotif Object Failed !!!");
	}

	return l_lNewTaskId;
}

long CTaskManager::TransferCall(const std::string& p_strActiveCTICallRefId, const std::string& p_strHeldCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget)
{
	long l_lNewTaskId = DEFAULT_TASKID;

	ITransferCallNotifPtr l_pRequestNotif = boost::make_shared<CTransferCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetActiveCTICallRefId(p_strActiveCTICallRefId);
		l_pRequestNotif->SetHeldCTICallRefId(p_strHeldCTICallRefId);
		l_pRequestNotif->SetSponsor(p_strSponsor);
		l_pRequestNotif->SetTarget(p_strTarget);

		l_lNewTaskId = this->AddCmdTask(Task_TransferCall, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], TransferCall, ActiveCallRefId [%s], HeldCallRefId [%s], Sponsor [%s], Target [%s]",
			l_lNewTaskId, p_strActiveCTICallRefId.c_str(), p_strHeldCTICallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CTransferCallNotif Object Failed !!!");
	}

	return l_lNewTaskId;
}

long CTaskManager::DeflectCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget)
{
	long l_lNewTaskId = DEFAULT_TASKID;

	IDeflectCallNotifPtr l_pRequestNotif = boost::make_shared<CDeflectCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetCTICallRefId(p_strCTICallRefId);
		l_pRequestNotif->SetSponsor(p_strSponsor);
		l_pRequestNotif->SetTarget(p_strTarget);

		l_lNewTaskId = this->AddCmdTask(Task_DeflectCall, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], DeflectCall, CallRefId [%s], Sponsor [%s], Target [%s]",
			l_lNewTaskId, p_strCTICallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CDeflectCallNotif Object Failed !!!");
	}

	return l_lNewTaskId;
}

long CTaskManager::HoldCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strDevice)
{
	long l_lNewTaskId = DEFAULT_TASKID;

	IHoldCallNotifPtr l_pRequestNotif = boost::make_shared<CHoldCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetCTICallRefId(p_strCTICallRefId);
		l_pRequestNotif->SetSponsor(p_strSponsor);
		l_pRequestNotif->SetDevice(p_strDevice);

		l_lNewTaskId = this->AddCmdTask(Task_HoldCall, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], HoldCall, CallRefId [%s], Sponsor [%s], Device [%s]",
			l_lNewTaskId, p_strCTICallRefId.c_str(), p_strSponsor.c_str(), p_strDevice.c_str());
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CHoldCallNotif Object Failed !!!");
	}

	return l_lNewTaskId;
}
long CTaskManager::RetrieveCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strDevice)
{
	long l_lNewTaskId = DEFAULT_TASKID;

	IRetriveCallNotifPtr l_pRequestNotif = boost::make_shared<CRetriveCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetCTICallRefId(p_strCTICallRefId);
		l_pRequestNotif->SetSponsor(p_strSponsor);
		l_pRequestNotif->SetDevice(p_strDevice);

		l_lNewTaskId = this->AddCmdTask(Task_RetrieveCall, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], RetrieveCall, CallRefId [%s], Sponsor [%s], Device [%s]",
			l_lNewTaskId, p_strCTICallRefId.c_str(), p_strSponsor.c_str(), p_strDevice.c_str());
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CRetriveCallNotif Object Failed !!!");
	}

	return l_lNewTaskId;
}

long CTaskManager::ConferenceCall(const std::string& p_strTarget, const std::string& p_strHeldCTICallRefId, const std::string& p_strActiveCTICallRefId)
{
	long l_lNewTaskId = DEFAULT_TASKID;

	IConferenceCallNotifPtr l_pRequestNotif = boost::make_shared<CConferenceCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetHeldCTICallRefId(p_strHeldCTICallRefId);
		l_pRequestNotif->SetActiveCTICallRefId(p_strActiveCTICallRefId);
		l_pRequestNotif->SetTarget(p_strTarget);

		l_lNewTaskId = this->AddCmdTask(Task_ConferenceCall, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], ConferenceCall, HeldCallRefId [%s], ActiveCallRefId [%s], Target [%s]",
			l_lNewTaskId, p_strHeldCTICallRefId.c_str(), p_strActiveCTICallRefId.c_str(), p_strTarget.c_str());
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CConferenceCallNotif Object Failed !!!");
	}

	return l_lNewTaskId;
}


long CTaskManager::ReconnectCall(const std::string& p_strActiveCTICallRefId, const std::string& p_strHeldCTICallRefId, const std::string& p_strSponsor, const std::string& p_strDevice)
{
	long l_lNewTaskId = DEFAULT_TASKID;

	IReconnectCallNotifPtr l_pRequestNotif = boost::make_shared<CReconnectCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetActiveCTICallRefId(p_strActiveCTICallRefId);
		l_pRequestNotif->SetHeldCTICallRefId(p_strHeldCTICallRefId);
		l_pRequestNotif->SetSponsor(p_strSponsor);
		l_pRequestNotif->SetDevice(p_strDevice);

		l_lNewTaskId = this->AddCmdTask(Task_ReconnectCall, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], ReconnectCall, ActiveCallRefId [%s], HeldCallRefId [%s], Sponsor [%s], Device [%s]",
			l_lNewTaskId, p_strActiveCTICallRefId.c_str(), p_strHeldCTICallRefId.c_str(), p_strSponsor.c_str(), p_strDevice.c_str());
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CReconnectCallNotif Object Failed !!!");
	}

	return l_lNewTaskId;
}
long CTaskManager::RemoveFromConference(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget)
{
	long l_lNewTaskId = DEFAULT_TASKID;

	IDeleteConferencePartyNotifPtr l_pRequestNotif = boost::make_shared<CDeleteConferencePartyNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetCTICallRefId(p_strCTICallRefId);
		l_pRequestNotif->SetCompere(p_strSponsor);
		l_pRequestNotif->SetTarget(p_strTarget);

		l_lNewTaskId = this->AddCmdTask(Task_DeleteConferenceParty, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], DeleteConferenceParty, CTICallRefId [%s], Compere [%s], Target [%s]",
			l_lNewTaskId, p_strCTICallRefId.c_str(), p_strSponsor.c_str(), p_strTarget.c_str());
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create DeleteConferenceParty Object Failed !!!");
	}

	return l_lNewTaskId;
}

long CTaskManager::GetCTIConnectState()
{
	long l_lNewTaskId = DEFAULT_TASKID;

	ISwitchRequestNotifPtr l_pRequestNotif = boost::make_shared<CSwitchRequestNotif>();
	if (l_pRequestNotif)
	{
		l_lNewTaskId = this->AddCmdTask(Task_GetCTIConnState, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], GetAesState", l_lNewTaskId);
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSwitchRequestNotif Object Failed !!!");
	}

	return l_lNewTaskId;
}
long CTaskManager::GetDeviceList()
{
	long l_lNewTaskId = DEFAULT_TASKID;

	ISwitchRequestNotifPtr l_pRequestNotif = boost::make_shared<CSwitchRequestNotif>();
	if (l_pRequestNotif)
	{
		l_lNewTaskId = this->AddCmdTask(Task_GetDeviceList, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], GetDeviceList", l_lNewTaskId);
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSwitchRequestNotif Object Failed !!!");
	}

	return l_lNewTaskId;
}
long CTaskManager::GetACDList()
{
	long l_lNewTaskId = DEFAULT_TASKID;

	IGetACDListRequestNotifPtr l_pRequestNotif = boost::make_shared<CGetACDListRequestNotif>();
	if (l_pRequestNotif)
	{
		l_lNewTaskId = this->AddCmdTask(Task_GetACDList, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], GetACDList", l_lNewTaskId);
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSwitchRequestNotif Object Failed !!!");
	}

	return l_lNewTaskId;
}
long CTaskManager::GetAgentList(const std::string & p_strACDGrp)
{
	long l_lNewTaskId = DEFAULT_TASKID;

	IGetAgentListRequestNotifPtr l_pRequestNotif = boost::make_shared<CGetAgentListRequestNotif>();
	if (l_pRequestNotif)
	{
		l_lNewTaskId = this->AddCmdTask(Task_GetAgentList, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], GetAgentList, ACDGrp [%s]", l_lNewTaskId, p_strACDGrp.c_str());
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSwitchRequestNotif Object Failed !!!");
	}

	return l_lNewTaskId;
}

long CTaskManager::GetCallList()
{
	long l_lNewTaskId = DEFAULT_TASKID;

	ISwitchRequestNotifPtr l_pRequestNotif = boost::make_shared<CSwitchRequestNotif>();
	if (l_pRequestNotif)
	{
		l_lNewTaskId = this->AddCmdTask(Task_GetCallList, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], GetCallList", l_lNewTaskId);
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSwitchRequestNotif Object Failed !!!");
	}

	return l_lNewTaskId;
}

long CTaskManager::GetReadyAgent(const std::string& p_strACDGrp)
{
	long l_lNewTaskId = DEFAULT_TASKID;

	IGetReadyAgentNotifPtr l_pRequestNotif = boost::make_shared<CGetReadyAgentNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetACDGrp(p_strACDGrp);

		l_lNewTaskId = this->AddCmdTask(Task_GetReadyAgent, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], GetReadyAgent", l_lNewTaskId);
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CGetReadyAgentNotif Object Failed !!!");
	}

	return l_lNewTaskId;
}
