#pragma once
#include "Boost.h"

namespace ICC
{
	namespace MsgCenter
	{
		/*
		* class   MessageCenterʵ����
		* author  ZhangYixiang
		* purpose
		* note
		*/ 
		class CMessageCenterImpl
			: public IMessageCenter
		{
		public:
			CMessageCenterImpl(IResourceManagerPtr p_pIResourceManager);
			virtual ~CMessageCenterImpl();

			virtual bool Send(std::string l_sendMsg, const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName);
			virtual bool Send(std::string l_sendMsg, std::string l_alarmId, bool send_to_icc_bz, bool send_to_alarmer, const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName);
			virtual bool Send(std::string l_sendMsg, std::string l_deptCode, bool send_to_icc_bz, const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName, std::string business_code = "alarminfo");
			virtual bool Send(std::string l_sendMsg, std::string l_userName, std::string l_userCode, bool send_to_icc_bz, const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName, std::string business_code = "alarminfo");
			virtual void BuildManualAcceptConfig(std::string l_manualAcceptType, std::string& l_value);//��Redis��ParamInfo�ڵ��л�ȡ��Ϣ������ʾ���
			virtual bool Start();
			virtual bool Stop();
			virtual bool BuildSendMsg(std::string& l_buildMsg, MessageSendDTOData l_sendData);//belong
			virtual std::string DeptCodeToDeptGuid(std::string l_deptCode);

			virtual bool GetUserGuid(std::string& l_strUserGuid, std::string& l_strUserDeptGuid, std::string l_strUserCode, std::string l_strUserName);
			virtual std::vector<std::string> GetGuidbByDept(std::string l_orgGuid, std::string l_roleAttr, const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName);

			//�����澯����ʱ�����ϯ״̬�Ļ����Ƿ����û��󶨣����û�У������澯��umc��֪ͨ�೤ϯ��
			virtual bool Send(std::string l_sendMsg, std::string strSeatNo,std::string strDeptCode, const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName);
			//��������ʱ�����ϯ״̬���������������С���趨��������������umc��֪ͨ�೤ϯ��
			virtual bool SendMsgToCenter(std::string l_sendMsg, std::string l_sendType, std::string strSeatNo, std::string strDeptCode, const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName);

			//��umc��Ϣ��֪ͨ�೤ϯ�;�Ա���ݲ����ɸı�(send_to_icc_bz��send_to_alarmer),��������Ϣ���͸��ݲ���strBusinessCode�ɱ�
			virtual bool Send(std::string strBusinessCode,  bool send_to_icc_bz, bool send_to_alarmer, const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName,std::string m_strDeviceInfo);
			//ͨ���Ӿ�Ա����ھ�Ա������ƥ�����֤�ҵ���Ա��guid
			virtual bool AlarmerCodeToGuid(std::string &strCode,std::string &strGuid);
			virtual bool GetAlarmerGuidBySeat(std::string& l_strSeat, std::string& l_alarmerCode);
		private:
			bool GetMsgCenterService(std::string& p_strIp, std::string& p_strPort);
			bool GetSmpService(std::string& p_strIp, std::string& p_strPort, std::string l_serverName);
			bool GetDeptCode(std::string& l_strDeptCode, std::string& l_strAlarmerCode, std::string l_strAlarmId);

			void SetNacosParams(const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName);
			bool SendMsgTo12345(std::string l_strMsg);
			void WorkThread(void* l_curMessageCenter);
			void AddWorkMsg(std::string l_strMsg);

		public:
			IResourceManagerPtr GetResourceManager();

		private:
			Config::IConfigPtr					m_pConfig;
			Log::ILogPtr						m_LogPtr;
			IResourceManagerPtr					m_pResourceManager;
			StringUtil::IStringUtilPtr			m_pString;
			IHttpClientPtr 						m_pHttpClient;
			JsonParser::IJsonFactoryPtr			m_JsonFactoryPtr;
			DataBase::IDBConnPtr				m_pDBConn;
			DateTime::IDateTimePtr				m_pDateTime;
			Redis::IRedisClientPtr				m_pRedisClient;

			//���ڴ���Nacos����������Ϣ
			std::string m_strNacosServerIp;
			std::string m_strNacosServerPort;
			std::string m_strNacosServerNamespace;
			std::string m_strNacosServerGroupName;
			boost::mutex     m_mutexNacosParams;

			//�������
			std::shared_ptr<boost::thread> l_oThreadWorkList;
			bool m_workThreadStop;
			bool m_workThreadStart;
			std::vector<std::string> m_msgWorkList;

			//����������Լ��̲߳�������
			boost::mutex m_mtxMsg;
			boost::mutex m_mutexWait;
			boost::condition_variable_any m_cvMsg;
		};
	}
}