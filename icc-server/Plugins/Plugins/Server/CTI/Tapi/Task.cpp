#include "Boost.h"
#include "Task.h"

CTask::CTask() 
{
	m_lTaskId = 0;
	m_lInvokeId = 0;
	m_nTaskName = Task_NULL;
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