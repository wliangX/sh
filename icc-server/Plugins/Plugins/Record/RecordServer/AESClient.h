#pragma once
#include <AgentIPMap.h>
#include <Boost.h>

namespace ICC
{

	class CAavayaAESClient
	{
	public:
		//logָ��/aes ip��ַ/port�˿ں�/�Ƿ�ʹ��https/�û���/����
		CAavayaAESClient(Log::ILogPtr pLog, const string& ip, const string& port, bool https, const string& uname, const string& pwd);
		~CAavayaAESClient();
		/* ����Avaya������IP��ַ
		���: ip��ַ/port�˿ں�/�Ƿ�ʹ��https
		����: ��
		����ֵ: �� */
		void setAvayaAddress(string ip, string port, bool https);

		/* ������֤��Ϣ
		���: �û���/����
		����: ��
		����ֵ: �� */
		void setAuth(string uname, string pwd);

		/* ��ȡ���еĻ����ź�IP��ַ�Ķ�Ӧ�б�
		���: ��
		����: ��ȡ�����л���IP��ַ�б�
		����ֵ: 0 �ɹ� >0:ʧ�� �ο�������� */
		//[1,����ʧ�� 2.xml����ʧ�� ]
		int getAllStationIP(map<string, string>& info);

	private:

		//�������ص�xml, ���н�����������IP�Ķ�Ӧ��ϵ���Լ�SessionID
		string parseAllStationIP(const string& strResponse, map<string, string>& info);
		//����-1 XML�������� ����0 �����ɹ�
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