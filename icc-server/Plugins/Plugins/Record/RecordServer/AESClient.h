#pragma once
#include <AgentIPMap.h>
#include <Boost.h>

namespace ICC
{

	class CAavayaAESClient
	{
	public:
		//log指针/aes ip地址/port端口号/是否使用https/用户名/密码
		CAavayaAESClient(Log::ILogPtr pLog, const string& ip, const string& port, bool https, const string& uname, const string& pwd);
		~CAavayaAESClient();
		/* 设置Avaya服务器IP地址
		入参: ip地址/port端口号/是否使用https
		出参: 无
		返回值: 无 */
		void setAvayaAddress(string ip, string port, bool https);

		/* 设置认证信息
		入参: 用户名/密码
		出参: 无
		返回值: 无 */
		void setAuth(string uname, string pwd);

		/* 获取所有的话机号和IP地址的对应列表
		入参: 无
		出参: 获取的所有话机IP地址列表
		返回值: 0 成功 >0:失败 参考错误码表 */
		//[1,连接失败 2.xml解析失败 ]
		int getAllStationIP(map<string, string>& info);

	private:

		//解析返回的xml, 从中解析出话机与IP的对应关系，以及SessionID
		string parseAllStationIP(const string& strResponse, map<string, string>& info);
		//返回-1 XML解析出错 返回0 解析成功
		int parseErrorResult(const string& str, string& faultstr);

		int releaseSession(const string& sessionID);

		string m_host;
		string m_port;
		bool   m_ishttps;
		string m_uname;
		string m_pwd;

		Log::ILogPtr m_pLog;
	};
	typedef std::shared_ptr<CAavayaAESClient> CAavayaAESClientPtr;
}