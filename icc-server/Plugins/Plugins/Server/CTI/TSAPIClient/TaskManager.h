#pragma once

/**
 @Copyright Copyright (C), 2018
 @file		TaskManage.h
 @created	2018/01/19
 @brief		CTaskManager���������������
 CTaskManager�����������У�
 1�����ڵȴ�switchManageִ�е��������
 2���Ѿ�ִ�У��ȴ����ؽ�����������

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

		//ά�����ڵȴ�ִ�е�����
		std::map<long, boost::shared_ptr<CTask>>		m_cmdTaskQueue;
		//ά���Ѿ�ִ�е���������
		std::map<long, boost::shared_ptr<CTask>>		m_excutedTaskQueue;
		// Switch �������͵��¼��򷵻ص�ִ�н��
		std::map<long, boost::shared_ptr<CTask>>		m_switchEventTaskQueue;
	};
}	// end namespace

