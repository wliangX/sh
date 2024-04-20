#pragma once

namespace ICC
{
	
    class CBusinessImpl :  public CBusinessBase
    {
    public:
        virtual void OnInit();
        virtual void OnStart();
		virtual void OnStop();
        virtual void OnDestroy();
  

	public:
		//��ȡ���������֤��
		void OnNotifGetBJRSFZH(ObserverPattern::INotificationPtr p_pNotify);

		//��ȡ�����������Ϣ
		void OnNotifGetBJRSFXX(ObserverPattern::INotificationPtr p_pNotify);
	
	private:
		//������֤��Ϣ
		bool _ValidAuthInfo(const ICC::PROTOCOL::CAuthInfo &, std::string &out_strErrMsg);  

		//�����ȡ���������֤��������Ϣ
		bool _ValidGetBJRSFZHReqMsg(const ICC::PROTOCOL::CGetBJRSFZHRequest::CBody &, std::string& out_strErrMsg);

		//�����ȡ�����������Ϣ������Ϣ
		bool _ValidGetBJRSFXXReqMsg(const ICC::PROTOCOL::CGetBJRSFXXRequest::CBody &, std::string& out_strErrMsg);
	
		//BASE64����
		bool _Base64Encode(const std::string& input, std::string* output);

		//ģ�����ݷ���
		std::string _SmulateSFZH(const std::string& in_strPhoneNo);

		std::string _SmulateSFXX(const std::string& in_strSFZH);

		void _InsertSFZXX(const ICC::PROTOCOL::CGetBJRSFZHRequest::CBody& sfzRequestInfo, const ICC::PROTOCOL::CGetBJRSFZHRespond::CBody& sfzResponseInfo);
		void _InsertRYXX(const ICC::PROTOCOL::CGetBJRSFXXRespond::CData& bjrResponseInfo);

	private:
		Log::ILogPtr m_pLog;
		Config::IConfigPtr m_pConfig;

		//StringUtil::IStringUtilPtr m_pString;
		//DateTime::IDateTimePtr m_pDateTime;
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
		Redis::IRedisClientPtr m_pRedisClient;
	
		DataBase::IDBConnPtr m_pDBConn;

		//IHttpClientPtr m_pHttpClient;  ����ʱ��

		JsonParser::IJsonFactoryPtr m_pJsonFty;
	private:
		
		std::string m_strServerIP;  //����IP
		std::string m_strToken;    //Token IDֵ

		std::string m_strServerPort; //����˿�

		std::string m_strSmulateFlag;  //�Ƿ�ģ��

    };
}