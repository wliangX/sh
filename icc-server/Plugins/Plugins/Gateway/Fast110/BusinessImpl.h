#pragma once

#include "FastInfo.h"

#define CORRECTSIZE  3

namespace ICC
{
	typedef struct tCallInfo 
	{
		PROTOCOL::CDeviceStateSync::CBody oCallInfo;
	}tCallInfo;


#pragma pack(push)
#pragma pack(1)
	typedef struct tTDiconnectMSG_t
	{
		unsigned short msgHeader;
		unsigned short platfromID;
		unsigned short cmdType;
		unsigned int len;
	}tTDiconnectMSG;
#pragma pack(pop)

	class CBusinessImpl :
		public CBusinessBase
	{
	public:
		CBusinessImpl();
		~CBusinessImpl();

	public:
		virtual void OnInit();
		virtual void OnStart();
		virtual void OnStop();
		virtual void OnDestroy();

	public:
		void OnCTIDeviceStateNotice(ObserverPattern::INotificationPtr p_pCTIStateInfo);		// 获取CTI呼叫状态 [5/21/2018 w26326]
		void OnAddConferenceRespond(ObserverPattern::INotificationPtr p_pRespInfo);
		void OnDeleteConferenceRespond(ObserverPattern::INotificationPtr p_pDeleteInfo);

		void SendTopicConSync(std::string p_strState, const tCallInfo& p_tCallInfo);
		void SendAddConRequest(const PROTOCOL::CDeviceStateSync& p_oDeStateInfo, std::string p_strSponsor);
		void SendDeleteConRequest(const PROTOCOL::CDeviceStateSync& p_oDeStateInfo, std::string p_strSponsor);
		void SendDisconnectMsg(const PROTOCOL::CDeviceStateSync& p_oDeStateInfo, std::string p_strSponsor);// Socket消息挂断 [5/24/2018 w26326]
		void SetBindMember();

	private:
		Config::IConfigPtr					m_pConfig;
		Log::ILogPtr						m_pLog;
		ObserverPattern::IObserverCenterPtr	m_pObserverCenter;
		Lock::ILockFactoryPtr				m_pLockFty;
		StringUtil::IStringUtilPtr			m_pString;
		JsonParser::IJsonFactoryPtr			m_pJsonFty;
		DateTime::IDateTimePtr				m_pDateTime;
		Tcp::ITcpClientPtr					m_TcpClient;
		HelpTool::IHelpToolPtr				m_pHelpTool;
		Tcp::ITcpClientCallBackPtr			m_ITcpClientCallbackPtr;
		std::string							m_strPUCIP;
		unsigned int						m_PUCPort;
		std::map<std::string, std::string>	m_BindMemberInfo;	// 与PUC绑定的号码 [5/24/2018 w26326]
		std::map<std::string, tCallInfo>	m_mCallInfo;		// 话务消息与RelateID绑定  [5/22/2018 w26326]
	};


	class TcpClientImpl : public Tcp::ITcpClientCallBack
	{
	public:
		TcpClientImpl(Log::ILogPtr l_pLog) :
			m_pLog(l_pLog)
		{

		}
	public:
		void OnReceived(const char* p_pData, unsigned int p_iLength);
		void OnConnected(std::string p_strError);
		void OnDisconnected(std::string p_strError);

	private:
		Log::ILogPtr m_pLog;
	};
}

