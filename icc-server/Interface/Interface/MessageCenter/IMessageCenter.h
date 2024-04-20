#pragma once
#include <vector>
#include <IObject.h>

namespace ICC
{
	namespace MsgCenter
	{
		typedef struct receiveObjects
		{
			std::vector<std::string> noticeList;
			std::vector<std::string> syncList;
			std::string orgRule;     //组织推送规则（current：本级；superiors：本级及上级组织；subordinate：本级及下级组织）
			std::string type;
		}ReceiveObject;

		typedef struct MessageSendDTOData
		{
			std::string appCode;
			std::string businessCode;
			int compensateType;
			std::string delayDuration;
			int delayType;
			std::string message;
			std::string moduleCode;
			std::string needStorage;
			std::vector<ReceiveObject> receiveObjects;
			std::string sendType;
			std::string sync;
			std::string title;
		}MessageSendDTOData;
		/*
		* class   MessageCenter接口
		* author  ZhangYixiang
		* purpose
		* note
		*/
		class IMessageCenter :
			public IObject
		{
		public:
			//virtual bool GetMsgCenterService(std::string& p_strIp, std::string& p_strPort) = 0;

			virtual bool Send(std::string l_sendMsg, const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName) = 0;
			virtual bool Send(std::string l_sendMsg, std::string l_alarmId, bool send_to_icc_bz, bool send_to_alarmer, const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName) = 0;
			virtual bool Send(std::string l_sendMsg, std::string l_deptCode, bool send_to_icc_bz, const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName, std::string business_code = "alarminfo") = 0;
			virtual bool Send(std::string l_sendMsg, std::string l_userName, std::string l_userCode, bool send_to_icc_bz, const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName, std::string business_code = "alarminfo") = 0;
			virtual void BuildManualAcceptConfig(std::string l_manualAcceptType, std::string& l_value) = 0;
			virtual bool Start() = 0;
			virtual bool Stop() = 0;
			virtual bool BuildSendMsg(std::string& l_buildMsg, MessageSendDTOData l_sendData) = 0;

			virtual std::vector<std::string> GetGuidbByDept(std::string l_orgGuid, std::string l_roleAttr, const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName) = 0;
			virtual std::string DeptCodeToDeptGuid(std::string l_deptCode) = 0;
			virtual bool GetUserGuid(std::string& l_strUserGuid, std::string& l_strUserDeptGuid, std::string l_strUserCode, std::string l_strUserName) = 0;

			virtual bool Send(std::string l_sendMsg, std::string strSeatNo, std::string strDeptCode, const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName)=0;
			virtual bool SendMsgToCenter(std::string l_sendMsg, std::string l_sendType, std::string strSeatNo, std::string strDeptCode, const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName) = 0;
			virtual bool Send(std::string strBusinessCode, bool send_to_icc_bz, bool send_to_alarmer, const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName,std::string m_strDeviceInfo)=0;
			virtual bool AlarmerCodeToGuid(std::string& strCode, std::string& strGuid)=0;
			virtual bool GetAlarmerGuidBySeat(std::string& l_strSeat, std::string& l_alarmerCode) = 0;
		};

		typedef boost::shared_ptr<IMessageCenter> IMessageCenterPtr;
	}
}
