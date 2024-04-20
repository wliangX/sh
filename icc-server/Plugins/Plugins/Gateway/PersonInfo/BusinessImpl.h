#pragma once

namespace ICC
{

	class CBusinessImpl : public CBusinessBase
	{
	public:
		virtual void OnInit();
		virtual void OnStart();
		virtual void OnStop();
		virtual void OnDestroy();
	public:
		void OnNotifGetPersonInfoByIdCard(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifGetPersonInfoByPhone(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifGetLoginToken(ObserverPattern::INotificationPtr p_pNotify);
	private:
		//������֤��Ϣ
		bool _ValidAuthInfo(const ICC::PROTOCOL::CAuthInfo&, std::string& out_strErrMsg);

		//�����ȡ���������֤��������Ϣ
		bool _ValidGetBJRSFZHReqMsg(const ICC::PROTOCOL::CGetBJRSFZHRequest::CBody&, std::string& out_strErrMsg);

		//�����ȡ�����������Ϣ������Ϣ
		bool _ValidGetBJRSFXXReqMsg(const ICC::PROTOCOL::CGetBJRSFXXRequest::CBody&, std::string& out_strErrMsg);

		//BASE64����
		bool _Base64Encode(const std::string& input, std::string* output);

		//��¼�ӿ� /api/zhzxapi/login  
		bool Login();

		void BuildBJRSFHMRRespond(PROTOCOL::CGetBJRSFZHIdentiyInfoRequest& m_oInfoRequest, ObserverPattern::INotificationPtr p_pNotify);

		void BuildBJRSFXXRRespond(PROTOCOL::CGetBJRSFZHIdentiyInfoRequest& m_oInfoRequest, ObserverPattern::INotificationPtr p_pNotify);

		void InsertIdentiyInfo(PROTOCOL::CGetBJRSFZHIdentiyInfoRequest& bjrResponseInfo);

		// �ַ����滻,��souceStr�е�subStr �滻Ϊ newStr
		std::string Subreplace(const std::string souceStr, const std::string subStr, const std::string newStr);
		//ģ�����ݷ���
		std::string GetPostTextByContent(const std::string strSearch);
	private:
		Log::ILogPtr m_pLog;
		Config::IConfigPtr m_pConfig;

		StringUtil::IStringUtilPtr m_pString;
		//DateTime::IDateTimePtr m_pDateTime;
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
		Redis::IRedisClientPtr m_pRedisClient;

		DataBase::IDBConnPtr m_pDBConn;
		IHttpClientPtr m_pHttpClient;

		Timer::ITimerManagerPtr		m_pTimerMgr;
		JsonParser::IJsonFactoryPtr m_pJsonFty;
	private:

		std::string m_strServerIP;  //����IP
		std::string m_strToken;    //Token IDֵ

		std::string m_strServerPort; //����˿�

		std::string m_strSmulateFlag;  //�Ƿ�ģ��

		std::string m_strUsername;    //�û��� ������
		std::string m_strPassword;    //���� ������

		int m_intTimeOut;

	};
}