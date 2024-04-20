#pragma once
#include "UdpHelper.h"
#include "CommonWorkThread.h"

#define CTI_INCOMING "incoming"		//ACD （呼入）
#define CTI_WAITING "waiting"		//ACD （排队）
#define CTI_ASSIGN "assign"			//ACD （分配）
#define CTI_RELEASE "release"		//ACD （早释）
#define CTI_FREESTATE "freestate"	//Device （空闲态）
#define CTI_RINGSTATE "ringstate"	//Device （振铃态）
#define CTI_RINGBACKSTATE "ringbackstate"	//Device （回铃态）
#define CTI_DIALSTATE "dialstate"			//Device （拨号态）
#define CTI_TALKSTATE "talkstate"			//Device （通话态）
#define CTI_CONFERENCESTATE "conferencestate"	//Device （会议态）
#define CTI_HOLDSTATE "holdstate"			//Device （保留态）
#define CTI_HANGUPSTATE "hangupstate"		//Device （挂机态）
#define CTI_WAITHANGUPSTATE "waithangupstate"	//Device （等待挂机态）

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
		*  @brief    :  处理收到CTI话务状态消息,总体来说，这个消息没有什么意义
		*  @input    :  p_pNotifiCallInfoNotice
		*  @output   :
		*  @return   :
		*  @author   :  z13060  2018/05/30 15:02
		********************************************************/
		void OnCtiCallInfoNotice(ObserverPattern::INotificationPtr p_pNotifiRequest);

		/********************************************************
		*  @function :  CBusinessImpl::OnCtiDeviceInfoNotice
		*  @brief    :  处理收到CTI设备状态消息
		*  @input    :  p_pNotifiDeviceInfoNotice
		*  @output   :
		*  @return   :
		*  @author   :  z13060  2018/05/30 15:03
		********************************************************/
		void OnCtiDeviceInfoNotice(ObserverPattern::INotificationPtr p_pNotifiRequest);

		/********************************************************
		*  @function :  CBusinessImpl::OnCtiDeviceInfoNotice
		*  @brief    :  处理收到CTI呼叫结束消息
		*  @input    :  p_pNotifiDeviceInfoNotice
		*  @output   :
		*  @return   :
		*  @author   :  z13060  2018/05/30 15:03
		********************************************************/
		void OnCtiCallOverSyncNotice(ObserverPattern::INotificationPtr p_pNotifiRequest);

		/********************************************************
		*  @function :  CBusinessImpl::calrCalledNum
		*  @brief    :  处理收到CTI呼叫结束消息
		*  @input    :  
		*  @output   :
		*  @return   :
		*  @author   :   
		********************************************************/
		void calrCalledNum(const std::string& p_strPhone, std::string& p_strCallerId, std::string& p_strCalledId);

		/********************************************************
		*  @function :  CBusinessImpl::OnCtiCallRefIdSyncNotice
		*  @brief    :  处理收到CTI同步话务ID的消息
		*  @input    :  p_pNotifiDeviceInfoNotice
		*  @output   :
		*  @return   :
		*  @author   :  
		********************************************************/
		void OnCtiCallRefIdSyncNotice(ObserverPattern::INotificationPtr p_pNotifiRequest);
		
		/********************************************************
		*  @function :  CBusinessImpl::OnCtiCaseInfoNotice
		*  @brief    :  处理警情消息，只推送首次包含话务ID的警情数据
		*  @input    :  p_pNotifiCaseInfoNotice
		*  @output   :
		*  @return   :
		*  @author   :  z13060  2018/05/30 15:03
		********************************************************/
		void OnCtiAlarmSyncNotice(ObserverPattern::INotificationPtr p_pNotifiRequest);

		/********************************************************
		*  @function :  CBusinessImpl::OnAuthResponse
		*  @brief    :  处理鉴权服务回复
		*  @input    :  p_pAuthRespond
		*  @output   :
		*  @return   :
		*  @author   :  z13060  2018/05/30 15:05
		********************************************************/
		void OnAuthResponse(ObserverPattern::INotificationPtr p_pNotifiRequest);

		/********************************************************
		*  @function :  CBusinessImpl::CheckCallInfoMapLength
		*  @brief    :  防止map超长占用过多系统内存
		*  @input    :
		*  @output   :
		*  @return   :  删除元素返回ture，不删除返回false
		*  @author   :  ZengW  2018/05/29 11:26
		********************************************************/ 
		bool CheckCallInfoMapLength();

		virtual void ProcMessage(CommonThread_data msg_data);
		void         _BindUdp();

	private:


		UdpHelperPtr						   m_UdpHelper;
		CallInfoMap							   m_CallInfoMap;

		unsigned int                           m_SendTimes;        //发送次数
		unsigned int                           m_IntervalTime;     //定时器间隔
		unsigned int                           m_UDPPort;		   //UDP服务端口
		unsigned int                           m_CheckHeart;		//检测客户端心跳时间
		std::string	m_strServiceID;	  //服务ID
		bool m_bUDPIsStart;

		CCommonWorkThread m_sendThread;
	};

}