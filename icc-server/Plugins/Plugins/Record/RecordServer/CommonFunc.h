#pragma once
#include <list>
#include <map>
#include <set>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#if defined(WIN32)||defined(WIN64)
#include <WinSock2.h>
#else
#include <memory>
//#include <unistd.h>
//#include <arpa/inet.h>
#endif
#include <Boost.h>
using namespace std;

//CTI�����ȡ�ĺ�����Ϣ
struct CallInfo
{
	CallInfo():m_strAgentID(""), m_strCallID(""), m_strDevice(""), m_strRealCallRefID(""), m_strCaller(""), m_strCalled(""), m_strAgentIP(""), 
		m_usRtpPort(0), m_u64CallStart(0), m_u64CallStop(0), m_strAlarmId("")
	{ };
	std::string     m_strAgentID;		//��ϯID
	std::string	    m_strCallID;		//����ID
	std::string	    m_strDevice;		//�����豸
	std::string	    m_strRealCallRefID; //�Ŷӻ���AES��Ļ���ID
	std::string	    m_strCaller;		//����
	std::string	    m_strCalled;		//����
	std::string     m_strAgentIP;       //��ϯ������IP��ַ
	uint16_t        m_usRtpPort;        //RTP�˿ں�
	uint64_t        m_u64CallStart;		//��ʼʱ��
	uint64_t        m_u64CallStop;		//����ʱ��
	std::string	    m_strAlarmId;		//�绰����
};
typedef std::shared_ptr<CallInfo> CallInfoPtr;


class Semaphore
{
public:
	Semaphore(long count = 0) : count(count) {}
	//V����������
	void signal()
	{
		unique_lock<mutex> unique(mt);
		++count;
		if (count <= 0)
			cond.notify_one();
	}
	//P����������
	void wait()
	{
		unique_lock<mutex> unique(mt);
		--count;
		if (count < 0)
			cond.wait(unique);
	}

private:
	mutex mt;
	condition_variable cond;
	long count;
};

//Base-64���ܺ���
std::string base64_encode(const std::string& input);;

//�滻�ַ�������
std::string replace(std::string str, const std::string& from, const std::string& to);

//�ļ�·���Ƿ����
bool IsFileExits(const std::string& strFile);
//�ݹ鴴���ļ���
void CreateFolder(std::string& msgPath);

//ɾ��nDay��ǰ���ļ�
int RemoveFile(const std::string& filename, int nDay);

//������Ŀ¼��Ŀ¼�е��ļ�,��ɾ��nDay��ǰ���ļ�
int DelFileRecurse(const std::string& dirname, int nDay);

int String2Int(const std::string& strNumber);

bool str2list(std::string str, std::string sep, std::vector<std::string >& strList);

std::string GetFileName(const std::string& strFilePath);

//Http�ͻ��˷���
class CHttpsClient
{
public:
	static int DoPostHttps(const string& host, const string& port, const string& page, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, string& reponse_data);
	static int DoPostHttp(const string& host, const string& port, const string& page, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, string& reponse_data);
};
