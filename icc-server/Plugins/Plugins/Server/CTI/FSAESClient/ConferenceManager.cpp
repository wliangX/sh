#include "Boost.h"

#include <Protocol/IRespond.h>
#include <Protocol/CTIServer/GetConferencePartyRespond.h>

#include "ConferenceManager.h"
#include "TaskManager.h"
#include "CTIFSAesDefine.h"
#include "CTIFSAesNotifaction.h"
#include "DeviceManager.h"
#include "SysConfig.h"

////////////////////////////////////////////////////////////////////////////////
//

CCTIConference::CCTIConference(const std::string& p_strCTIConferenceId,
	const std::string& p_strCompere,
	const std::string& p_strSrcCTICallId,
	const std::string& p_strSrcCaller,
	const std::string& p_strSrcCalled,
	Log::ILogPtr l_pLogPtr,
	DateTime::IDateTimePtr l_pDateTimePtr)
	:m_strCompere(p_strCompere),
	m_strSrcCTICallId(p_strSrcCTICallId),
	m_strConferenceId(p_strCTIConferenceId),
	m_strSrcCaller(p_strSrcCaller),
	m_strSrcCalled(p_strSrcCalled),
	m_pLogPtr(l_pLogPtr),
	m_pDateTimePtr(l_pDateTimePtr),
	m_nAddIndex(1),
	m_bIsBargeInCreate(false),
	m_bIsListenCreate(false),
	m_bIsTakeoverCreate(false)
{
	m_strCreateTime = l_pDateTimePtr->CurrentDateTimeStr();
	m_strBargeInSponsor = "";
	m_strListenTarget = "";
}

bool CCTIConference::AddMemberEx(const std::string& p_strTarget, const std::string& p_strTargetType, const std::string& p_strInitState,const std::string & p_strCurrTime)
{
	std::string l_strKey = p_strTarget;// +"_" + p_strTargetType;
	if (m_members.find(l_strKey) == m_members.end())
	{
		if (m_strCompere.empty())
		{
			if (CDeviceManager::Instance()->IsExtension(p_strTarget))
			{
				m_strCompere = p_strTarget;
			}
		}

		_tagMember l_member;
		l_member.m_bDisEnable = false;
		l_member.m_strAddTime = m_pDateTimePtr->CurrentDateTimeStr();
		l_member.m_strCallState = p_strInitState;
		l_member.m_strCTICallId = "";
		l_member.m_strTarget = p_strTarget;
		l_member.m_strTargetDeviceType = p_strTargetType;
		l_member.m_nAddIndex = m_nAddIndex;

		if (p_strInitState == CallStateString[STATE_TALK] || p_strInitState == CallStateString[STATE_CONFERENCE])
		{
			l_member.m_strTalkTime = p_strCurrTime;
		}
		m_members[l_strKey] = l_member;

		ICC_LOG_DEBUG(m_pLogPtr, "add member,number:[%s],type:[%s],index:[%d]", p_strTarget.c_str(), p_strTargetType.c_str(), m_nAddIndex);

		m_nAddIndex++;
	}
	else
	{
		ICC_LOG_WARNING(m_pLogPtr, "add member has joined the meeting,number:[%s],state:[%s]", p_strTarget.c_str(), p_strInitState.c_str());

		return false;
	}

	return true;
}

bool CCTIConference::UpdateMemberCallState(const std::string& p_strMemberNum, const std::string& p_strCallState, const std::string& p_strCurrTime)
{
	bool bRes = false;

	if (p_strMemberNum.empty())
	{
		return false;
	}
	bool l_bDelete = false;
	auto l_iter = m_members.find(p_strMemberNum);
	if (l_iter != m_members.end())
	{
		std::string l_strLastState = l_iter->second.m_strCallState;
		l_iter->second.m_strCallState = p_strCallState;
		if (p_strCallState == CallStateString[STATE_HANGUP])
		{
			l_iter->second.m_strCTICallId = "";
			if (l_strLastState == "" || l_strLastState == CallStateString[STATE_RING] || l_strLastState == CallStateString[STATE_DIAL])
			{
				l_bDelete = true;
			}
		}
		else if (p_strCallState == CallStateString[STATE_TALK])
		{
			if (l_iter->second.m_strTalkTime.empty())
			{
				l_iter->second.m_strTalkTime = p_strCurrTime;
			}
		}

		bRes = true;
	}
	else
	{
		//增加容错处理
		if (p_strCallState == CallStateString[STATE_TALK] || p_strCallState == CallStateString[STATE_RING])
		{
			_tagMember l_member;
			l_member.m_bDisEnable = false;
			l_member.m_strAddTime = m_pDateTimePtr->CurrentDateTimeStr();
			l_member.m_strCallState = p_strCallState;
			if (p_strCallState == CallStateString[STATE_TALK])
			{
				l_member.m_strTalkTime = p_strCurrTime;
			}
			l_member.m_strCTICallId = "";
			l_member.m_strTarget = p_strMemberNum;
			//l_member.m_strTargetDeviceType = p_strTargetType;
			l_member.m_nAddIndex = ++m_nAddIndex;
			m_members[p_strMemberNum] = l_member;
		}
	}

	if (l_bDelete)
	{
		m_members.erase(p_strMemberNum);
	}

	ICC_LOG_DEBUG(m_pLogPtr, "update member state, res:[%d],ConferenceId:[%s],Target:[%s],state:[%s],bDelete:[%d]", bRes, m_strConferenceId.c_str(), p_strMemberNum.c_str(), p_strCallState.c_str(), l_bDelete);
	return bRes;
}

int CCTIConference::GetMemberCount()
{
	int p_nCount = m_members.size();
	return p_nCount;
}

void CCTIConference::DeleteMember(const std::string& p_strTarget, const std::string& p_strTargetType)
{
	std::string l_strKey = p_strTarget;// +"_" + p_strTargetType;
	m_members.erase(l_strKey);
}

std::string CCTIConference::GetMemberCTICallId(const std::string& p_strTarget, const std::string& p_strTargetType)
{
	std::string l_strKey = p_strTarget;// +"_" + p_strTargetType;
	auto l_iter = m_members.find(l_strKey);
	if (l_iter != m_members.end())
	{
		return l_iter->second.m_strCTICallId;
	}
	return "";
}

bool CCTIConference::GetMemberInfo(const std::string& p_strTarget, _tagMember& p_member, std::string& p_strSrcCallId)
{
	auto l_iter = m_members.find(p_strTarget);
	if (l_iter != m_members.end())
	{
		p_member.m_strTarget = l_iter->second.m_strTarget;
		p_member.m_strTargetDeviceType = l_iter->second.m_strTargetDeviceType;
		p_member.m_strCTICallId = l_iter->second.m_strCTICallId;
		p_member.m_strCompere = m_strCompere;
		p_member.m_strCallState = l_iter->second.m_strCallState;
		p_member.m_strTalkTime = l_iter->second.m_strTalkTime;
		p_strSrcCallId = m_strSrcCTICallId;
		return true;
	}
	return false;
}

bool CCTIConference::GetMemberInfoByCallId(const std::string& p_strCTICallId, _tagMember& p_member,std::string &p_strSrcCallId)
{
	for (auto l_Member : m_members)
	{
		if (l_Member.second.m_strCTICallId == p_strCTICallId)
		{
			p_member.m_strTarget = l_Member.second.m_strTarget;
			p_member.m_strTargetDeviceType = l_Member.second.m_strTargetDeviceType;
			p_member.m_strCTICallId = l_Member.second.m_strCTICallId;
			p_member.m_strCompere = m_strCompere;
			p_member.m_strCallState = l_Member.second.m_strCallState;
			p_member.m_strTalkTime = l_Member.second.m_strTalkTime;
			p_strSrcCallId = m_strSrcCTICallId;
			return true;
		}
	}
	return false;
}

void CCTIConference::ReleaseConference()
{
	//
}

void CCTIConference::SetListen(const std::string& p_strCompere, const std::string& p_strTarget)
{
	//修改主持人
	m_strCompere = p_strCompere;
	m_bIsListenCreate = true;
	m_strListenTarget = p_strTarget;

	m_bIsTakeoverCreate = false;
	m_bIsBargeInCreate = false;
}

void CCTIConference::SetBargeIn(const std::string& p_strCompere, const std::string& p_strSponsor)
{
	//修改主持人
	m_strCompere = p_strCompere;
	if (m_bIsListenCreate)
	{
		m_bIsListenCreate = false;
	}
	m_bIsTakeoverCreate = false;

	m_bIsBargeInCreate = true;
	m_strBargeInSponsor = p_strSponsor;
}

void CCTIConference::SetTakeover()
{
	m_bIsListenCreate = false;
	m_bIsBargeInCreate = false;
	m_strBargeInSponsor = "";
	m_bIsTakeoverCreate = true;
}

void CCTIConference::SetNormalMode()
{
	m_bIsListenCreate = false;
	m_bIsBargeInCreate = false;
	m_strBargeInSponsor = "";
	m_bIsTakeoverCreate = false;
}

bool CCTIConference::SetMemberCTICallId(const std::string& p_strTarget, const std::string& p_strTargetType, const std::string& p_strCTICallId, const std::string& p_strInitState)
{
	std::string l_strKey = p_strTarget;// +"_" + p_strTargetType;
	auto l_iter = m_members.find(l_strKey);
	if (l_iter != m_members.end())
	{
		l_iter->second.m_strCTICallId = p_strCTICallId;
		if (!p_strInitState.empty())
		{
			l_iter->second.m_strCallState = p_strInitState;
		}
		ICC_LOG_DEBUG(m_pLogPtr, "Set member call ID, Member:[%s],ctiCallId:[%s],InitState:[%s]",p_strTarget.c_str(), p_strCTICallId.c_str(), p_strInitState.c_str());

		return true;
	}
	return false;
}

bool CCTIConference::InitCallMember(const std::string & p_strConferenceName)
{
	if (!m_strConferenceName.empty())
	{
		return false;
	}
	m_strConferenceName = p_strConferenceName;


	for (auto l_Member : m_members)
	{
		if (!l_Member.second.m_bDisEnable && l_Member.second.m_strCallState.empty())
		{
			IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_CONFERENCE_ADDPARTY_CMD);
			l_pCmdRequest->SetParam(CTI_PARAM_conferencename, p_strConferenceName);
			l_pCmdRequest->SetParam(CTI_PARAM_cti_conferenceid, m_strConferenceId);
			l_pCmdRequest->SetParam(CTI_PARAM_callid, m_strSrcCTICallId);
			l_pCmdRequest->SetParam(CTI_PARAM_member, l_Member.second.m_strTarget);
			if (CDeviceManager::Instance()->IsExtension(l_Member.second.m_strTarget, true))
			{
				l_pCmdRequest->SetParam(CTI_PARAM_member_type, "1");
			}
			else
			{
				l_pCmdRequest->SetParam(CTI_PARAM_member_type, "4");
			}
			l_pCmdRequest->SetParam(CTI_PARAM_calloutPre, CSysConfig::Instance()->GetOutCallHead());

			long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiAsyncRquest_Cmd, l_pCmdRequest);
			ICC_LOG_DEBUG(m_pLogPtr, "AddConferenceParty AddCmdTask.requestId:[%u],CTIConfId:[%s],hcpConfId:[%s],target:[%s]",
				l_lRequestId, m_strSrcCTICallId.c_str(), p_strConferenceName.c_str(), l_Member.second.m_strTarget.c_str());
		}
	}

	return true;
}

void CCTIConference::GetMemberList(std::map<std::string, _tagMember>& p_memberList)
{
	p_memberList = m_members;
}

bool CCTIConference::CheckConferenceIsOver(std::string &p_strLastCallingMember)
{
	int l_nCallingCount = 0;
	std::string l_strLastCallingMember;
	for (auto l_Member : m_members)
	{
		if (l_Member.second.m_strCallState == CallStateString[STATE_SILENTMONITOR])
		{
			continue;
		}

		if (l_Member.second.m_strCallState != CallStateString[STATE_HANGUP] && 
			l_Member.second.m_strCallState != CallStateString[STATE_FREE])
		{
			l_strLastCallingMember = l_Member.second.m_strTarget;
			l_nCallingCount++;
		}
	}

	if (l_nCallingCount == 1)
	{
		p_strLastCallingMember = l_strLastCallingMember;
		return true;
	}

	if (l_nCallingCount == 0)
	{
		return true;
	}

	return false;
}

void CCTIConference::ClearMonitorMember()
{
	int l_nCallingCount = 0;
	std::string l_strLastCallingMember;
	for (auto l_Member : m_members)
	{
		if (l_Member.second.m_strCallState == CallStateString[STATE_SILENTMONITOR])
		{
			IFSAesCmdRequestPtr l_pCmdRequest = boost::make_shared<CFSAesRequestCmd>(REQUEST_REFUSE_CALL_CMD);
			if (l_pCmdRequest)
			{
				l_pCmdRequest->SetParam(CTI_PARAM_callid, m_strSrcCTICallId);
				l_pCmdRequest->SetParam(CTI_PARAM_channelnumber, l_Member.second.m_strTarget);

				long l_lRequestId = CTaskManager::Instance()->AddCmdTask(Task_ctiSyncRquest_Cmd, l_pCmdRequest);
				ICC_LOG_DEBUG(m_pLogPtr, "ClearMonitorMember AddCmdTask.requestId:[%u],auto hangup call,conferenceId:[%s],CallRefId:[%s],Device:[%s]",
					l_lRequestId, m_strConferenceId.c_str(),m_strSrcCTICallId.c_str(), l_Member.second.m_strTarget.c_str());
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////
//
boost::shared_ptr<CConferenceManager> CConferenceManager::m_pManagerInstance = nullptr;
boost::shared_ptr<CConferenceManager> CConferenceManager::Instance()
{
	if (m_pManagerInstance == nullptr)
	{
		m_pManagerInstance = boost::make_shared<CConferenceManager>();
	}

	return m_pManagerInstance;
}

void CConferenceManager::ExitInstance()
{
	if (m_pManagerInstance)
	{
		m_pManagerInstance.reset();
	}
}

CConferenceManager::CConferenceManager(void)
{
	m_pLogPtr = nullptr;
	m_pStrUtil = nullptr;
	m_pDateTimePtr = nullptr;
	m_oLastCheckTime = 0;
}

CConferenceManager::~CConferenceManager(void)
{
	
}

void CConferenceManager::OnInit(IResourceManagerPtr p_pResourceManager)
{
	m_pLogPtr = ICCGetResourceEx(Log::ILogFactory, ICCILogFactoryResourceName, p_pResourceManager)->GetLogger(MODULE_NAME);
	m_pStrUtil = ICCGetResourceEx(StringUtil::IStringFactory, ICCIStringFactoryResourceName, p_pResourceManager)->CreateString();
	m_pDateTimePtr = ICCGetResourceEx(DateTime::IDateTimeFactory, ICCIDateTimeFactoryResourceName, p_pResourceManager)->CreateDateTime();
	m_pJsonFty = ICCGetResourceEx(JsonParser::IJsonFactory, ICCIJsonFactoryResourceName, p_pResourceManager);
	m_oLastCheckTime = m_pDateTimePtr->CurrentDateTime();
}
void CConferenceManager::OnStart()
{
	
}
void CConferenceManager::OnStop()
{
}

void CConferenceManager::ReleaseAllConference()
{
	std::lock_guard<std::mutex> guard(m_conferencesMutex);
	m_mapConferences.clear();
}

IConferencePtr CConferenceManager::GetConferenceBySrcCallId(const std::string& p_strSrcCTICallId)
{
	IConferencePtr l_conferencePtr = nullptr;
	{
		std::lock_guard<std::mutex> guard(m_conferencesMutex);
		for (auto l_Iter : m_mapConferences)
		{
			if (l_Iter.second->GetSrcCTICallId() == p_strSrcCTICallId)
			{
				l_conferencePtr = l_Iter.second;
				break;
			}
		}
	}
	return l_conferencePtr;
}

IConferencePtr CConferenceManager::GetConferenceById(const std::string& p_strCTIConferenceId)
{
	IConferencePtr l_conferencePtr = nullptr;
	{
		std::lock_guard<std::mutex> guard(m_conferencesMutex);
		auto l_iter = m_mapConferences.find(p_strCTIConferenceId);
		if (l_iter != m_mapConferences.end())
		{
			l_conferencePtr = l_iter->second;
		}
	}
	return l_conferencePtr;
}

IConferencePtr CConferenceManager::GetConferenceByName(const std::string& p_strConferenceName)
{
	IConferencePtr l_conferencePtr = nullptr;
	{
		std::lock_guard<std::mutex> guard(m_conferencesMutex);
		for (auto l_Iter : m_mapConferences)
		{
			if (l_Iter.second->GetConferenceName() == p_strConferenceName)
			{
				l_conferencePtr = l_Iter.second;
				break;
			}
		}
	}
	return l_conferencePtr;
}

IConferencePtr CConferenceManager::CreateCTIConferenceEx(const std::string& p_strConferenceName, const std::string& p_strHcpCallId)
{
	time_t l_tCurrentTime = time(0);
	char l_szDayTime[MAX_BUFFER] = { 0 };
	strftime(l_szDayTime, sizeof(l_szDayTime), "%Y%m%d%H%M%S", gmtime(&l_tCurrentTime));//localtime(&l_tCurrentTime));
	std::string l_strConferenceId = std::string(l_szDayTime);

	ICTICallPtr l_pCTICall = CCallManager::Instance()->GetCallByHcpCallId(p_strHcpCallId);

	IConferencePtr l_conferencePtr = nullptr;
	if (l_pCTICall == nullptr)
	{
		ICC_LOG_WARNING(m_pLogPtr, "Create New Conference not find call, ConferenceName:[%s],SrcCTICallId:[%s]",p_strConferenceName.c_str(), p_strHcpCallId.c_str());

		l_conferencePtr = boost::make_shared<CCTIConference>(l_strConferenceId,
			"",
			p_strHcpCallId,
			"",
			"",
			m_pLogPtr,
			m_pDateTimePtr);
	}
	else
	{
		l_conferencePtr = boost::make_shared<CCTIConference>(l_strConferenceId,
			"",
			p_strHcpCallId,
			l_pCTICall->GetCallerId(),
			l_pCTICall->GetCalledId(),
			m_pLogPtr,
			m_pDateTimePtr);

		if (l_conferencePtr)
		{
			l_pCTICall->SetCTIConferenceID(l_strConferenceId);
		}
	}

	if (l_conferencePtr)
	{
		l_conferencePtr->SetConferenceName(p_strConferenceName);
		{
			std::lock_guard<std::mutex> guard(m_conferencesMutex);
			m_mapConferences[l_strConferenceId] = l_conferencePtr;
		}
	}
	
	ICC_LOG_DEBUG(m_pLogPtr, "Create New Conference, ConferenceId:[%s] ConferenceName:[%s],SrcCTICallId:[%s]",
		l_strConferenceId.c_str(), p_strConferenceName.c_str(), p_strHcpCallId.c_str());

	return l_conferencePtr;
}

IConferencePtr CConferenceManager::CreateCTIConference(const std::string& p_strCompere, ICTICallPtr p_pCTICall)
{
	time_t l_tCurrentTime = time(0);
	char l_szDayTime[MAX_BUFFER] = { 0 };
	strftime(l_szDayTime, sizeof(l_szDayTime), "%Y%m%d%H%M%S", gmtime(&l_tCurrentTime));//localtime(&l_tCurrentTime));
	std::string l_strConferenceId = std::string(l_szDayTime);

	std::string l_strSrcCTICallId = "";
	if (p_pCTICall)
	{
		l_strSrcCTICallId = p_pCTICall->GetCTICallRefId();
	}
	IConferencePtr l_conferencePtr = boost::make_shared<CCTIConference>(l_strConferenceId,
		p_strCompere, 
		l_strSrcCTICallId,
		p_pCTICall->GetCallerId(), 
		p_pCTICall->GetCalledId(),
		m_pLogPtr,
		m_pDateTimePtr);
	if (l_conferencePtr)
	{
		if (p_pCTICall)
		{
			p_pCTICall->SetCTIConferenceID(l_strConferenceId, p_strCompere);
		}
		
		{
			std::lock_guard<std::mutex> guard(m_conferencesMutex);
			m_mapConferences[l_strConferenceId] = l_conferencePtr;
		}
		
		ICC_LOG_DEBUG(m_pLogPtr, "Create New Conference, ConferenceId:[%s] Compere:[%s],SrcCTICallId:[%s]", 
			l_strConferenceId.c_str(), p_strCompere.c_str(), l_strSrcCTICallId.c_str());
	}

	return l_conferencePtr;
}

void CConferenceManager::ReleaseConference(const std::string& p_strConferenceId)
{
	//std::string l_strHcpConferenceId;
	//IConferencePtr l_conferencePtr = nullptr;
	std::lock_guard<std::mutex> guard(m_conferencesMutex);
	auto l_iter = m_mapConferences.find(p_strConferenceId);
	if (l_iter != m_mapConferences.end())
	{
		m_mapConferences.erase(l_iter);
	}
}

void CConferenceManager::UpdateMemberCallState(const std::string& p_strCTIConferenceId,const std::string& p_strMemberNum,const std::string &p_strCallState)
{
	std::lock_guard<std::mutex> guard(m_conferencesMutex);
	auto l_iter = m_mapConferences.find(p_strCTIConferenceId);
	if (l_iter != m_mapConferences.end())
	{
		l_iter->second->UpdateMemberCallState(p_strMemberNum, p_strCallState, m_pDateTimePtr->CurrentDateTimeStr());
	}
	else
	{
		ICC_LOG_FATAL(m_pLogPtr, "not find Conference, ConferenceId:[%s]", p_strCTIConferenceId.c_str());
	}
}

void CConferenceManager::InitCallMember(const std::string& p_strCTIConferenceId, const std::string& p_strConferenceName)
{
	IConferencePtr l_conferencePtr = nullptr;
	{
		std::lock_guard<std::mutex> guard(m_conferencesMutex);
		auto l_iter = m_mapConferences.find(p_strCTIConferenceId);
		if (l_iter != m_mapConferences.end())
		{
			l_conferencePtr = l_iter->second;
		}
	}
	if (l_conferencePtr != nullptr)
	{
		l_conferencePtr->InitCallMember(p_strConferenceName);
	}
	else
	{
		ICC_LOG_FATAL(m_pLogPtr, "not find Conference, ConferenceId:[%s]", p_strCTIConferenceId.c_str());
	}
}

bool CConferenceManager::IsListenCreate(const std::string& p_strCTIConferenceId)
{
	IConferencePtr l_conferencePtr = nullptr;
	{
		std::lock_guard<std::mutex> guard(m_conferencesMutex);
		auto l_iter = m_mapConferences.find(p_strCTIConferenceId);
		if (l_iter != m_mapConferences.end())
		{
			l_conferencePtr = l_iter->second;
		}
	}
	if (l_conferencePtr != nullptr)
	{
		return l_conferencePtr->IsListenCreate();
	}
	else
	{
		ICC_LOG_FATAL(m_pLogPtr, "not find Conference, ConferenceId:[%s]", p_strCTIConferenceId.c_str());
	}

	return false;
}

bool CConferenceManager::IsBargeInCreate(const std::string& p_strCTIConferenceId,std::string &p_strBargeInSponsor)
{
	p_strBargeInSponsor = "";
	IConferencePtr l_conferencePtr = nullptr;
	{
		std::lock_guard<std::mutex> guard(m_conferencesMutex);
		auto l_iter = m_mapConferences.find(p_strCTIConferenceId);
		if (l_iter != m_mapConferences.end())
		{
			l_conferencePtr = l_iter->second;
		}
	}
	if (l_conferencePtr != nullptr)
	{
		bool bRes = l_conferencePtr->IsBargeInCreate();
		p_strBargeInSponsor = l_conferencePtr->GetBargeInSponsor();
		return bRes;
	}
	else
	{
		ICC_LOG_FATAL(m_pLogPtr, "not find Conference, ConferenceId:[%s]", p_strCTIConferenceId.c_str());
	}
	return false;
}

bool CConferenceManager::GetMeetingMember(const std::string &p_strCTIConferenceId, const std::string& p_strTarget, _tagMember &p_member, std::string& p_strSrcCallId)
{
	IConferencePtr l_conferencePtr = nullptr;
	{
		std::lock_guard<std::mutex> guard(m_conferencesMutex);
		auto l_iter = m_mapConferences.find(p_strCTIConferenceId);
		if (l_iter != m_mapConferences.end())
		{
			l_conferencePtr = l_iter->second;
		}
	}

	if (l_conferencePtr != nullptr)
	{
		return l_conferencePtr->GetMemberInfo(p_strTarget, p_member, p_strSrcCallId);

		//return l_conferencePtr->GetMemberInfoByCallId(p_strCTICallId, p_member, p_strSrcCallId);
	}

	return false;
}

bool CConferenceManager::GetMeetingMembers(const std::string& p_strCTIConferenceId, std::string& p_strMembers)
{
	IConferencePtr l_conferencePtr = nullptr;
	{
		std::lock_guard<std::mutex> guard(m_conferencesMutex);
		auto l_iter = m_mapConferences.find(p_strCTIConferenceId);
		if (l_iter != m_mapConferences.end())
		{
			l_conferencePtr = l_iter->second;
		}
	}
	if (l_conferencePtr == nullptr)
	{
		return false;
	}

	std::string l_strSrcCTICallId = l_conferencePtr->GetSrcCTICallId();
	std::map<std::string, _tagMember> l_memberList;
	l_conferencePtr->GetMemberList(l_memberList);

	//查询最后一次加入的成员
	int l_nTmpIndex = 0;
	std::string l_strLastTarget;
	for (auto l_Iter : l_memberList)
	{
		if (l_strLastTarget.empty())
		{
			l_strLastTarget = l_Iter.second.m_strTarget;
			l_nTmpIndex = l_Iter.second.m_nAddIndex;
		}
		else
		{
			if (l_Iter.second.m_nAddIndex > l_nTmpIndex)
			{
				l_nTmpIndex = l_Iter.second.m_nAddIndex;
				l_strLastTarget = l_Iter.second.m_strTarget;
			}
		}
	}
	ICC_LOG_DEBUG(m_pLogPtr, "Finally Add member,Index:[%d],Target:[%s],count:[%d]", l_nTmpIndex, l_strLastTarget.c_str(), l_memberList.size());

	PROTOCOL::CGetConferencePartyRespond l_oOutRespond;

	l_oOutRespond.m_oBody.m_strCompere = l_conferencePtr->GetCompere();
	l_oOutRespond.m_oBody.m_strResult = "0";
	l_oOutRespond.m_oBody.m_strConferenceId = p_strCTIConferenceId;
	
	for (auto l_oPartyObj : l_memberList)
	{
		if (l_oPartyObj.second.m_strCallState == CallStateString[STATE_SILENTMONITOR])
		{
			continue;
		}
		PROTOCOL::CGetConferencePartyRespond::CBody::CData l_CData;
		l_CData.m_strTarget = l_oPartyObj.first;
		l_CData.m_strTargetDeviceType = l_oPartyObj.second.m_strTargetDeviceType;
		l_CData.m_strAddTimer = l_oPartyObj.second.m_strAddTime;
		l_CData.m_strState = l_oPartyObj.second.m_strCallState;
		if (l_oPartyObj.second.m_bDisEnable)
		{
			l_CData.m_strDisEnable = "1";
		}
		else
		{
			l_CData.m_strDisEnable = "0";
		}
		if (l_strLastTarget == l_CData.m_strTarget)
		{
			//最后一个加入会议的  Finally add
			l_CData.m_strFinallyAdd = "1";
		}
		l_oOutRespond.m_oBody.m_vecData.push_back(l_CData);
	}
	l_oOutRespond.m_oBody.m_strCount = std::to_string(l_oOutRespond.m_oBody.m_vecData.size());

	p_strMembers = l_oOutRespond.ToString(m_pJsonFty->CreateJson());

	return true;
}

std::string CConferenceManager::QueryConferenceByCallid(const std::string& p_strCallRefId, std::string& p_strCompere, std::string& p_strBargeInSponsor)
{
	std::string l_strConferenceId = "";
	IConferencePtr l_pConferencePtr = GetConferenceBySrcCallId(p_strCallRefId);
	if (l_pConferencePtr == nullptr)
	{
		ICTICallPtr l_pCTICallPtr = CCallManager::Instance()->GetCallByCTICallId(p_strCallRefId);
		if (l_pCTICallPtr)
		{
			string l_strConfId = l_pCTICallPtr->GetCTIConferenceID();
			l_pConferencePtr = GetConferenceById(l_strConfId);
		}
	}

	if (l_pConferencePtr)
	{
		if (!l_pConferencePtr->IsListenCreate() && !l_pConferencePtr->IsTakeoverCreate())
		{
			p_strCompere = l_pConferencePtr->GetCompere();
			if (l_pConferencePtr->IsBargeInCreate())
			{
				p_strBargeInSponsor = l_pConferencePtr->GetBargeInSponsor();
			}

			l_strConferenceId = l_pConferencePtr->GetConferenceId();
		}
	}

	return l_strConferenceId;
}
