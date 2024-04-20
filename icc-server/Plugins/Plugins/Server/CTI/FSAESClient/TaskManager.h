#pragma once
#include "Boost.h"
namespace ICC
{
#define STATR_TASKID		1
#define END_TASKID			0xFFFFF

/////////////////////////////////////////////////////////////////////
class CTask
{
public:
	CTask();
	virtual ~CTask(void);
public:
	void SetTaskId(long p_lTaskId) { m_lTaskId = p_lTaskId; }
	long GetTaskId() const { return m_lTaskId; }

	void SetTaskType(int p_nTaskType) { m_nTaskType = p_nTaskType; }
	int GetTaskType() const { return m_nTaskType; }

	void SetSwitchNotif(ISwitchNotifPtr p_pSwitchNotif) { m_pSwitchNotif = p_pSwitchNotif; }
	ISwitchNotifPtr GetSwitchNotif() { return m_pSwitchNotif; }

	void SetTime();
	long GetTaskTimeSpan();

private:
	long				m_lTaskId;
	int					m_nTaskType;
	time_t				m_tStartTime;
	ISwitchNotifPtr		m_pSwitchNotif;
};

typedef boost::shared_ptr<CTask> ITaskPtr;

/////////////////////////////////////////////////////////////////////
class CTaskManager
{
public:
	CTaskManager(void) {};
	virtual ~CTaskManager(void) {};

	static boost::shared_ptr<CTaskManager> Instance();
	void ExitInstance();

	void OnInit(IResourceManagerPtr p_pResourceManager);
	void OnStart();
	void OnStop();

	void ClearTask();
private:
	long CreateTaskId();

public:
	long AddCmdTask(int p_nTaskType, ISwitchNotifPtr p_pSwitchNotif);
	boost::shared_ptr<CTask> GetCmdTaskHeader();
	bool DeleteCmdTask(long p_lTaskId);
	int GetCmdTaskCount();
	void ClearCmdTaskQueue();

	void AddExecutedTask(const std::string& p_strMsgId,boost::shared_ptr<CTask> p_pTask);
	boost::shared_ptr<CTask> GetExecutedTask(const std::string& p_strMsgId);
	bool DeleteExecutedTask(const std::string& p_strMsgId);
	void ClearExecutedTaskQueue();
	bool ExecuteTaskTimeout(int& p_nTaskType, std::string& l_strTaskMsgId);

	long AddSwitchEventTask(int p_nTaskType, ISwitchNotifPtr p_pSwitchNotif);
	boost::shared_ptr<CTask> GetSwitchEventTaskHeader();
	bool DeleteSwitchEventTask(long p_lTaskId);
	int GetSwitchEventTaskCount();
	void ClearSwitchEventTaskQueue();

private:
	static boost::shared_ptr<CTaskManager> m_pInstance;
		
	Log::ILogPtr	m_pLog;

	static long		m_slTaskId;

	std::mutex		m_cmdTaskLock;
	std::mutex		m_excuteTaskLock;
	std::mutex		m_switchEventTaskLock;

	//维护正在等待执行的任务
	std::map<long, boost::shared_ptr<CTask>>			m_cmdTaskQueue;
	//维护已经执行的所有任务
	std::map<std::string, boost::shared_ptr<CTask>>		m_excutedTaskQueue;
	// Switch 主动推送的事件或返回的执行结果
	std::map<long, boost::shared_ptr<CTask>>			m_switchEventTaskQueue;
};
}	// end namespace

