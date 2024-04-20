#pragma once

/**
 @Copyright Copyright (C), 2018
 @file		TaskManage.h
 @created	2018/01/19
 @brief		CTaskManager负责管理所有任务
 CTaskManager管理两个队列：
 1，正在等待switchManage执行的任务队列
 2，已经执行，等待返回结果的任务队列

 @author	psy
 */

#include "Task.h"

namespace ICC
{
#define STATR_TASKID		1
#define END_TASKID			0xFFFFF

	class CTaskManager
	{
	public:
		CTaskManager(void);
		virtual ~CTaskManager(void);

		static boost::shared_ptr<CTaskManager> Instance();
		void ExitInstance();

		void OnInit(IResourceManagerPtr p_pResourceManager);
		void OnStart();
		void OnStop();

		void ClearTask();
	private:
		long CreateTaskId();

	public:
		boost::shared_ptr<CTask> CreateNewTask();

		void AddCmdTask(boost::shared_ptr<CTask> p_pTask);
		long AddCmdTask(E_TASK_NAME p_nTaskName, ISwitchNotifPtr p_pSwitchNotif);
		boost::shared_ptr<CTask> GetCmdTaskHeader();
		bool DeleteCmdTask(long p_lTaskId);
		int GetCmdTaskCount();
		void ClearCmdTaskQueue();

		void AddExcutedTask(boost::shared_ptr<CTask> p_pTask);
		boost::shared_ptr<CTask> GetExcutedTask(long p_lTaskId);
		boost::shared_ptr<CTask> GetExcutedTask(const std::string& p_strTaskId);
		bool DeleteExcusedTask(const std::string& p_strTaskId);
		bool DeleteExcusedTask(long p_lTaskId);
		void ClearExcutedTaskQueue();
		bool ExcuseTaskTimeout(E_TASK_NAME& p_nTaskName, long& p_lTaskId);

		void AddSwitchEventTask(boost::shared_ptr<CTask> p_pTask);
		long AddSwitchEventTask(E_TASK_NAME p_nTaskName, ISwitchNotifPtr p_pSwitchNotif);
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
		std::map<long, boost::shared_ptr<CTask>>		m_cmdTaskQueue;
		//维护已经执行的所有任务
		std::map<long, boost::shared_ptr<CTask>>		m_excutedTaskQueue;
		// Switch 主动推送的事件或返回的执行结果
		std::map<long, boost::shared_ptr<CTask>>		m_switchEventTaskQueue;
	};
}	// end namespace

