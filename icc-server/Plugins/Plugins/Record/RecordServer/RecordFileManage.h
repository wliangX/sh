#pragma once
#include "CommonFunc.h"
#include "RTPPacket.h"
struct CRecordFile
{
	CRecordFile(const string& strLocalFileName, const string& startTime, const string& stopTime, const string& strAgentPhoneIP, unsigned short usAgentPort): m_strDBGuid(""),
		m_strLocalFileName(strLocalFileName), m_strStartTime(startTime), m_strStopTime(stopTime), m_strAgentPhoneIP(strAgentPhoneIP), m_usAgentPort(usAgentPort),
		m_strFileUrl(""), m_strFileID(""), m_strAgentPhoneID(""), m_strCaller(""), m_strCalled(""), m_bCTIRefreshed(false), m_strFSFtpIp(""),m_strFSFtpFileName(""), m_bIsHistory(false)
	{

	}
	string m_strDBGuid;
	string m_strLocalFileName;
	string m_strFileUrl;
	string m_strFileID;
	std::set<string> m_setCallID;
	std::string GetCallRefID()
	{
		std::string strCallRefID("");
		std::set<string>::iterator iter = m_setCallID.begin();
		for (;iter != m_setCallID.end();)
		{
			strCallRefID += *iter;
			if (++iter != m_setCallID.end())
			{
				strCallRefID += ";";
			}
		}
		return strCallRefID;
	}
	string m_strAgentPhoneID;
	string m_strAgentPhoneIP;
	unsigned short m_usAgentPort;
	string m_strCaller;
	string m_strCalled;
	bool   m_bCTIRefreshed;
	string m_strStartTime;
	string m_strStopTime;
	string m_strFSFtpIp;//远端 FreeSwitch Ftp IP地址
	string m_strFSFtpFileName;//远端 FreeSwitch Ftp 文件名称
	bool   m_bIsHistory;
};
typedef std::shared_ptr<CRecordFile> CRecordFilePtr;

//本地文件保存管理
class CRecordFileManage
{
public:
	static CRecordFileManage& GetInstance();
	void AddARecordFile(CRecordFilePtr pRecordFile);
	CRecordFilePtr GetANotUploadFile();

	size_t NotUploadFileListSize();

	//设置本地存储文件根路径
	void SetLocalFilePath(const std::string& strLocalFilePath);
	std::string GetLocalFilePath();

private:
	static CRecordFileManage m_instance;
	mutex m_lstLock;
	std::list<CRecordFilePtr> m_lstNotUploadFile; //未上传文件文件，上传后移除

	std::string m_strLocalFilePath;  //本地存储录音文件的根路径
};