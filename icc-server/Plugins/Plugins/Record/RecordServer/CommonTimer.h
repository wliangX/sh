/**
 @Copyright Copyright (C), 2014
 @file		CommonTimer.h
 @created	2014/06/27
 @brief		

 @author	xujinghua
*/

#ifndef __CommonTimer_H__
#define __CommonTimer_H__

#include "CommonTimerCore.h"
#include <mutex>
#include <boost/asio/io_service.hpp>
#include <boost/thread/thread.hpp>
#include <map>
#include <atomic>
#include <memory>

struct CommonTimerData
{
	CommonTimerData():ulTimerID(0), pCoreTimer(NULL), u64FirstTime(0), bIsRunnning(false), uInterval(0), timerThread(0)
	{}
	void                   StopThread();       //ֹͣ��ʱ��ִ���߳�
	unsigned long          ulTimerID;          //��ʱ��ID
	CCommonTimerCore*      pCoreTimer;	       //�ص�ָ��
	std::atomic_ullong     u64FirstTime;	   //��ʼ���õ�ʱ��
	std::atomic_bool       bIsRunnning;        //�߳��Ƿ�������
	unsigned long long	   uInterval;		   //��ʱ�����¼��¼���λ����
	boost::thread*         timerThread;        //��ʱ��ִ���߳�
};
typedef std::shared_ptr<CommonTimerData> CommonTimerDataPtr;

#define INVALID_TIMER_ID 0

//ͨ�ö�ʱ����
class CCommonTimer
{
public:
	//����һ����ʱ������ʱ����������ض�ʱ��ID��>0 ��������ӣ� = 0�Ǵ���
	unsigned long AddTimer(CCommonTimerCore *pTimerCore, unsigned long long uInterval);//uInterval,��λ����	  
	//ɾ��һ����ʱ����ָ��MsgId
	void RemoveTimer(unsigned long uMsgId);		

public:
	CCommonTimer();
	virtual ~CCommonTimer();

protected:
	//virtual void run();
	virtual void DoTimer(CommonTimerDataPtr pTimerData);
	bool         _StartThreadPool();
	void         _StopThreadPool();
	
	unsigned long _AddTimer(CommonTimerDataPtr data);
	unsigned long GetCurrentTimerID();
	void          _RemoveTimer(unsigned long uMsgId);
	void GetCommonTimerData(unsigned long& uMsgId, CommonTimerDataPtr& pTimerData);
protected:
	boost::thread_group                      m_threadpool; //�̳߳�
	std::atomic_bool                         m_bIsRunning; //�̳߳��Ƿ�����
	std::atomic_bool                         m_bStopThread; //ֹͣ��ʶ

	std::mutex					              m_mutexTimerInfo;
	std::map<unsigned long, CommonTimerDataPtr> m_mapTimerInfo;

	
	std::mutex                                m_mutexStartThreadPool;
	unsigned long                             m_ulCurrentTimerID; //TimerID��1��ʼ������0�Ǵ���
};
typedef std::shared_ptr<CCommonTimer> CCommonTimerPtr;

class CCommonTimerManager
{
public:
	static CCommonTimerManager& GetInstance();
	//IntervalΪ������
	unsigned long AddTimer(CCommonTimerCore* pTimerCore, unsigned long long uInterval);
	void RemoveTimer(unsigned long uTimerId);
private:
	CCommonTimerManager();

	static CCommonTimerManager m_instance;
	CCommonTimerPtr m_pTimerMgr;
};

#endif