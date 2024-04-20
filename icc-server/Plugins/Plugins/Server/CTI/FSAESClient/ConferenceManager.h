#pragma once
#include"Boost.h"
#include "RequestResultEvent.h"
#include "FSAesEventProtocol.h"
#include "CallManager.h"
namespace ICC
{
////////////////////////////////////////////////////////////////////////////////
//
typedef struct tagMember
{
	std::string m_strTarget;			//	会议成员号码
	std::string m_strTargetDeviceType;	//	会议成员类型
	std::string m_strCTICallId;
	std::string m_strCallState;			//会议成员呼叫状态
	std::string m_strAddTime;
	std::string m_strTalkTime;
	std::string m_strCompere;
	bool m_bDisEnable;
	int m_nAddIndex;
}_tagMember;

class CCTIConference
{
public:
	CCTIConference(const std::string& p_strCTIConferenceId,
		const std::string &p_strCompere, 
		const std::string& p_strSrcCTICallId,
		const std::string& p_strSrcCaller,
		const std::string& p_strSrcCalled,
		Log::ILogPtr p_pLogPtr,
		DateTime::IDateTimePtr p_pDateTimePtr);
	virtual ~CCTIConference() {};
public:
	void SetSrcCTICallId(const std::string& p_strSrcCTICallId) { m_strSrcCTICallId = p_strSrcCTICallId; };
	std::string GetSrcCTICallId() const { return m_strSrcCTICallId; };
	std::string GetConferenceId() const { return m_strConferenceId; };
	std::string GetConferenceName() const { return m_strConferenceName; };
	void SetConferenceName(const std::string& p_strConferenceName) { m_strConferenceName = p_strConferenceName; };

	void SetCompere(const std::string& p_strCompere) { m_strCompere = p_strCompere; };
	std::string GetCompere() const { return m_strCompere; };
	std::string GetCreateTime() const { return m_strCreateTime; };
	std::string GetSrcCaller() const { return m_strSrcCaller; };
	std::string GetSrcCalled() const { return m_strSrcCalled; };
	int GetMemberCount();
	bool AddMemberEx(const std::string& p_strTarget, const std::string& p_strTargetType, const std::string& p_strInitState = "", const std::string& p_strCurrTime ="");
	void DeleteMember(const std::string& p_strTarget, const std::string& p_strTargetType);
	std::string GetMemberCTICallId(const std::string& p_strTarget, const std::string& p_strTargetType);

	bool GetMemberInfo(const std::string& p_strTarget, _tagMember& p_member, std::string& p_strSrcCallId);
	bool GetMemberInfoByCallId(const std::string& p_strCTICallId, _tagMember& p_member, std::string& p_strSrcCallId);

	bool SetMemberCTICallId(const std::string& p_strTarget, const std::string& p_strTargetType, const std::string& p_strCTICallId, const std::string& p_strInitState="");

	bool InitCallMember(const std::string& p_strConferenceName);
	bool UpdateMemberCallState(const std::string& p_strMemberNum, const std::string& p_strCallState, const std::string& p_strCurrTime);
	void GetMemberList(std::map<std::string, _tagMember>& p_memberList);

	void ReleaseConference();

	void SetBargeIn(const std::string &p_strCompere, const std::string& p_strSponsor);
	void SetListen(const std::string& p_strCompere, const std::string& p_strTarget);
	void SetTakeover();
	void SetNormalMode();

	bool IsBargeInCreate() { return m_bIsBargeInCreate; };
	bool IsListenCreate() { return m_bIsListenCreate; };
	bool IsTakeoverCreate() { return m_bIsTakeoverCreate; };
	std::string GetBargeInSponsor() {return m_strBargeInSponsor;};
	std::string GetListenTarget() { return m_strListenTarget; };

	std::string GetLastCallOverMember() { return m_strLastCallOverMember; };
	void UpdateLastCallOverMember(const std::string& p_strCallOverMember) { m_strLastCallOverMember = p_strCallOverMember; };

	void SetConferenceHangupType(const std::string& p_strHangupType) { m_strHangupType = p_strHangupType; };
	std::string GetConferenceHangupType() { return m_strHangupType; };

	bool CheckConferenceIsOver(std::string& p_strLastCallingMember);
	void ClearMonitorMember();
private:
	std::string m_strConferenceId;
	std::string m_strConferenceName;

	std::map<std::string, _tagMember> m_members;
private:
	Log::ILogPtr				m_pLogPtr;
	DateTime::IDateTimePtr		m_pDateTimePtr;
	std::string		m_strSrcCalled;
	std::string		m_strSrcCaller;
	std::string		m_strCompere;
	std::string		m_strSrcCTICallId;

	std::string		m_strCreateTime;

	bool m_bIsBargeInCreate;
	bool m_bIsListenCreate;
	bool m_bIsTakeoverCreate;
	std::string m_strBargeInSponsor;
	std::string m_strListenTarget;
	int	m_nAddIndex;

	std::string m_strLastCallOverMember;  //最后挂断的成员
	std::string m_strHangupType;
};

typedef boost::shared_ptr<CCTIConference> IConferencePtr;
////////////////////////////////////////////////////////////////////////////////
//
class CConferenceManager
{
public:
	CConferenceManager(void);
	virtual ~CConferenceManager(void);

	static boost::shared_ptr<CConferenceManager> Instance();
	void ExitInstance();

	void OnInit(IResourceManagerPtr p_pResourceManager);
	void OnStart();
	void OnStop();

	void ReleaseAllConference();

	IConferencePtr CreateCTIConference(const std::string& p_strCompere, ICTICallPtr p_pCTICall);
	IConferencePtr CreateCTIConferenceEx(const std::string& p_strConferenceName, const std::string& p_strHcpCallId);
	void ReleaseConference(const std::string& p_strConferenceId);
	IConferencePtr GetConferenceBySrcCallId(const std::string &p_strSrcCTICallId);
	IConferencePtr GetConferenceById(const std::string& p_strCTIConferenceId);
	IConferencePtr GetConferenceByName(const std::string& p_strConferenceName);

	bool IsBargeInCreate(const std::string& p_strCTIConferenceId, std::string& p_strBargeInSponsor);
	bool IsListenCreate(const std::string& p_strCTIConferenceId);

	void InitCallMember(const std::string& p_strCTIConferenceId, const std::string& p_strConferenceName);
	void UpdateMemberCallState(const std::string& p_strCTIConferenceId, const std::string& p_strMemberNum, const std::string& p_strCallState);

	bool GetMeetingMembers(const std::string& p_strCTIConferenceId, std::string &p_strMembers);
	bool GetMeetingMember(const std::string& p_strCTIConferenceId, const std::string& p_strTarget, _tagMember& p_member, std::string& p_strSrcCallId);

	std::string QueryConferenceByCallid(const std::string& p_strCallRefId, std::string& p_strCompere, std::string& p_strBargeInSponsor);
private:
	static boost::shared_ptr<CConferenceManager> m_pManagerInstance;

private:
	std::mutex	m_conferencesMutex;
	std::map<std::string, IConferencePtr>	m_mapConferences;

	Log::ILogPtr							m_pLogPtr;
	StringUtil::IStringUtilPtr				m_pStrUtil;
	DateTime::IDateTimePtr					m_pDateTimePtr;
	DateTime::CDateTime						m_oLastCheckTime;
	JsonParser::IJsonFactoryPtr				m_pJsonFty;

};

}// end namespace
