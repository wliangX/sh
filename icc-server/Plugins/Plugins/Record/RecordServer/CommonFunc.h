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

//CTI方面获取的呼叫信息
struct CallInfo
{
	CallInfo():m_strAgentID(""), m_strCallID(""), m_strDevice(""), m_strRealCallRefID(""), m_strCaller(""), m_strCalled(""), m_strAgentIP(""), 
		m_usRtpPort(0), m_u64CallStart(0), m_u64CallStop(0), m_strAlarmId("")
	{ };
	std::string     m_strAgentID;		//坐席ID
	std::string	    m_strCallID;		//话务ID
	std::string	    m_strDevice;		//发起设备
	std::string	    m_strRealCallRefID; //排队机的AES里的话务ID
	std::string	    m_strCaller;		//主叫
	std::string	    m_strCalled;		//被叫
	std::string     m_strAgentIP;       //坐席话机的IP地址
	uint16_t        m_usRtpPort;        //RTP端口号
	uint64_t        m_u64CallStart;		//开始时间
	uint64_t        m_u64CallStop;		//结束时间
	std::string	    m_strAlarmId;		//电话报警
};
typedef std::shared_ptr<CallInfo> CallInfoPtr;


class Semaphore
{
public:
	Semaphore(long count = 0) : count(count) {}
	//V操作，唤醒
	void signal()
	{
		unique_lock<mutex> unique(mt);
		++count;
		if (count <= 0)
			cond.notify_one();
	}
	//P操作，阻塞
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

//Base-64加密函数
std::string base64_encode(const std::string& input);;

//替换字符串函数
std::string replace(std::string str, const std::string& from, const std::string& to);

//文件路径是否存在
bool IsFileExits(const std::string& strFile);
//递归创建文件夹
void CreateFolder(std::string& msgPath);

//删除nDay天前的文件
int RemoveFile(const std::string& filename, int nDay);

//遍历子目录和目录中的文件,并删除nDay天前的文件
int DelFileRecurse(const std::string& dirname, int nDay);

int String2Int(const std::string& strNumber);

bool str2list(std::string str, std::string sep, std::vector<std::string >& strList);

std::string GetFileName(const std::string& strFilePath);

//Http客户端方法
class CHttpsClient
{
public:
	static int DoPostHttps(const string& host, const string& port, const string& page, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, string& reponse_data);
	static int DoPostHttp(const string& host, const string& port, const string& page, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, string& reponse_data);
};
