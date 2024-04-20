#pragma once
#include "UdpHelper.h"
#include "CommonWorkThread.h"

#define CTI_INCOMING "incoming"		//ACD �����룩
#define CTI_WAITING "waiting"		//ACD ���Ŷӣ�
#define CTI_ASSIGN "assign"			//ACD �����䣩
#define CTI_RELEASE "release"		//ACD �����ͣ�
#define CTI_FREESTATE "freestate"	//Device ������̬��
#define CTI_RINGSTATE "ringstate"	//Device ������̬��
#define CTI_RINGBACKSTATE "ringbackstate"	//Device ������̬��
#define CTI_DIALSTATE "dialstate"			//Device ������̬��
#define CTI_TALKSTATE "talkstate"			//Device ��ͨ��̬��
#define CTI_CONFERENCESTATE "conferencestate"	//Device ������̬��
#define CTI_HOLDSTATE "holdstate"			//Device ������̬��
#define CTI_HANGUPSTATE "hangupstate"		//Device ���һ�̬��
#define CTI_WAITHANGUPSTATE "waithangupstate"	//Device ���ȴ��һ�̬��

namespace ICC
{

	class CBusinessImpl	: public CBusinessBase,public CCommonThreadCore
	{
	public:
		CBusinessImpl();
		virtual void OnInit();
		virtual void OnStart();
		virtual void OnStop();
		virtual void OnDestroy();

		void OnTimer(ObserverPattern::INotificationPtr p_pNotifiRequest);
		void CheckClientHeart(ObserverPattern::INotificationPtr p_pNotifiRequest);
		void OnReport(int iCode, const std::string& strMessage);

	private:
		Log::ILogPtr                           m_pLog;
		ObserverPattern::IObserverCenterPtr    m_pObserverCenter;
		Lock::ILockFactoryPtr		           m_pLockFty;
		JsonParser::IJsonFactoryPtr            m_pJsonFty;
		DateTime::IDateTimePtr		           m_pDateTime; 
		StringUtil::IStringUtilPtr             m_pString;
		//IConsumerPtr                           m_pConsumer;
		Config::IConfigPtr                     m_pConfig;
		Config::IConfigPtr					   m_pStaticConfig;
		Udp::IUdpPtr						   m_pUdp;
		//Udp::IUdpServerCallBackPtr             m_pUdpServerCallBack;
		Xml::IXmlFactoryPtr                    m_pXmlFty;
		Timer::ITimerManagerPtr                m_ptimerMgr;
		HelpTool::IHelpToolPtr                 m_pHelpTool;
		Redis::IRedisClientPtr			       m_pRedisClient;

	private:

		void OnNotifiDoubleServerSync(ObserverPattern::INotificationPtr p_pNotifiRequest);

		/********************************************************
		*  @function :  CBusinessImpl::OnCtiCallInfoNotice
		*  @brief    :  �����յ�CTI����״̬��Ϣ,������˵�������Ϣû��ʲô����
		*  @input    :  p_pNotifiCallInfoNotice
		*  @output   :
		*  @return   :
		*  @author   :  z13060  2018/05/30 15:02
		********************************************************/
		void OnCtiCallInfoNotice(ObserverPattern::INotificationPtr p_pNotifiRequest);

		/********************************************************
		*  @function :  CBusinessImpl::OnCtiDeviceInfoNotice
		*  @brief    :  �����յ�CTI�豸״̬��Ϣ
		*  @input    :  p_pNotifiDeviceInfoNotice
		*  @output   :
		*  @return   :
		*  @author   :  z13060  2018/05/30 15:03
		********************************************************/
		void OnCtiDeviceInfoNotice(ObserverPattern::INotificationPtr p_pNotifiRequest);

		/********************************************************
		*  @function :  CBusinessImpl::OnCtiDeviceInfoNotice
		*  @brief    :  �����յ�CTI���н�����Ϣ
		*  @input    :  p_pNotifiDeviceInfoNotice
		*  @output   :
		*  @return   :
		*  @author   :  z13060  2018/05/30 15:03
		********************************************************/
		void OnCtiCallOverSyncNotice(ObserverPattern::INotificationPtr p_pNotifiRequest);

		/********************************************************
		*  @function :  CBusinessImpl::calrCalledNum
		*  @brief    :  �����յ�CTI���н�����Ϣ
		*  @input    :  
		*  @output   :
		*  @return   :
		*  @author   :   
		********************************************************/
		void calrCalledNum(const std::string& p_strPhone, std::string& p_strCallerId, std::string& p_strCalledId);

		/********************************************************
		*  @function :  CBusinessImpl::OnCtiCallRefIdSyncNotice
		*  @brief    :  �����յ�CTIͬ������ID����Ϣ
		*  @input    :  p_pNotifiDeviceInfoNotice
		*  @output   :
		*  @return   :
		*  @author   :  
		********************************************************/
		void OnCtiCallRefIdSyncNotice(ObserverPattern::INotificationPtr p_pNotifiRequest);
		
		/********************************************************
		*  @function :  CBusinessImpl::OnCtiCaseInfoNotice
		*  @brief    :  ��������Ϣ��ֻ�����״ΰ�������ID�ľ�������
		*  @input    :  p_pNotifiCaseInfoNotice
		*  @output   :
		*  @return   :
		*  @author   :  z13060  2018/05/30 15:03
		********************************************************/
		void OnCtiAlarmSyncNotice(ObserverPattern::INotificationPtr p_pNotifiRequest);

		/********************************************************
		*  @function :  CBusinessImpl::OnAuthResponse
		*  @brief    :  �����Ȩ����ظ�
		*  @input    :  p_pAuthRespond
		*  @output   :
		*  @return   :
		*  @author   :  z13060  2018/05/30 15:05
		********************************************************/
		void OnAuthResponse(ObserverPattern::INotificationPtr p_pNotifiRequest);

		/********************************************************
		*  @function :  CBusinessImpl::CheckCallInfoMapLength
		*  @brief    :  ��ֹmap����ռ�ù���ϵͳ�ڴ�
		*  @input    :
		*  @output   :
		*  @return   :  ɾ��Ԫ�ط���ture����ɾ������false
		*  @author   :  ZengW  2018/05/29 11:26
		********************************************************/ 
		bool CheckCallInfoMapLength();

		virtual void ProcMessage(CommonThread_data msg_data);
		void         _BindUdp();

	private:


		UdpHelperPtr						   m_UdpHelper;
		CallInfoMap							   m_CallInfoMap;

		unsigned int                           m_SendTimes;        //���ʹ���
		unsigned int                           m_IntervalTime;     //��ʱ�����
		unsigned int                           m_UDPPort;		   //UDP����˿�
		unsigned int                           m_CheckHeart;		//���ͻ�������ʱ��
		std::string	m_strServiceID;	  //����ID
		bool m_bUDPIsStart;

		CCommonWorkThread m_sendThread;
	};

}