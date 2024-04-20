#pragma once

/*!
 * \file AvayaResult.h
 *	@Copyright Copyright (C), 2012-2013
 * \author PSY
 * \date һ�� 2018
 *@brief �������¼���ʽ����
 *	���𽫶�Ӧ���¼�����ʽ������Ӧ���ַ���,������log��ʾ�͵���ʹ��
 *	������Avaya��������ƥ��,��AvayaEvent�����ʹ��
 *
 */

namespace ICC
{
	class CAvayaResult
	{
	public:
		CAvayaResult(void);
		virtual ~CAvayaResult(void);

		static boost::shared_ptr<CAvayaResult> Instance();
		void ExitInstance();

		void Init(IResourceManagerPtr pResourceManager);

	private:
		static boost::shared_ptr<CAvayaResult> m_pResultInstance;

		StringUtil::IStringUtilPtr	m_pStrUtil;

	public:
		std::string GetEventClassString(EventClass_t eventClass);
		std::string GetConnectionID_tString(ConnectionID_t conn);
		std::string GetAgentModeString(AgentMode_t agentmode);

	//	std::string GetActionTypeString(E_ACTION_TYPE actionType);
		std::string GetEventTypeString(EventType_t eventType);
		std::string GetACSUnsoliEventString(ACSUniversalFailure_t error);
		std::string GetCSTAUnsoliEventString(CSTAUniversalFailure_t error);
		std::string GetConnectionStateString(LocalConnectionState_t state);
		std::string GetCSTAEventCauseString(CSTAEventCause_t eventCause);
		std::string GetForwardTypeString(ForwardingType_t forwardType);
		std::string GetACSERRStr(int iErrCode);
		std::string GetErrorString(std::string strErrorModule, int iErrCode);
		std::string GetAgentStateString(CSTAQueryAgentStateConfEvent_t queryAgentState);

		int GetCSTAUnsoliEventErrorCode(CSTAUniversalFailure_t error);
	};

}	// end namespace


