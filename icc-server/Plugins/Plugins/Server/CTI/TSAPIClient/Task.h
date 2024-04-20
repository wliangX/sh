#pragma once

namespace ICC
{
	class CTask
	{
	public:
		CTask();
		virtual ~CTask(void);

	public:
		void SetTaskId(long p_lTaskId){ m_lTaskId = p_lTaskId; }
		long GetTaskId() const { return m_lTaskId; }

		void SetTaskName(E_TASK_NAME p_nTaskName){ m_nTaskName = p_nTaskName; }
		E_TASK_NAME GetTaskName() const { return m_nTaskName; }
		std::string GetTaskNameString(E_TASK_NAME p_nTaskName){ return TaskNameString[p_nTaskName]; }

		void SetSwitchNotif(ISwitchNotifPtr p_pSwitchNotif){ m_pSwitchNotif = p_pSwitchNotif; }
		ISwitchNotifPtr GetSwitchNotif(){ return m_pSwitchNotif; }

		void SetTime();
		long GetTaskTimeSpan();

	private:
		long				m_lTaskId;
		E_TASK_NAME			m_nTaskName;
		time_t				m_tStartTime;

		ISwitchNotifPtr m_pSwitchNotif;
	};

	typedef boost::shared_ptr<CTask> ITaskPtr;

}	// end namespace

