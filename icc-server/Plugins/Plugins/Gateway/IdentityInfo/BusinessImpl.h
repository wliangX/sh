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
		//获取报警人身份证号
		void OnNotifGetBJRSFZH(ObserverPattern::INotificationPtr p_pNotify);

		//获取报警人身份信息
		void OnNotifGetBJRSFXX(ObserverPattern::INotificationPtr p_pNotify);
	
	private:
		//较验认证信息
		bool _ValidAuthInfo(const ICC::PROTOCOL::CAuthInfo &, std::string &out_strErrMsg);  

		//较验获取报警人身份证号请求消息
		bool _ValidGetBJRSFZHReqMsg(const ICC::PROTOCOL::CGetBJRSFZHRequest::CBody &, std::string& out_strErrMsg);

		//较验获取报警人身份信息请求消息
		bool _ValidGetBJRSFXXReqMsg(const ICC::PROTOCOL::CGetBJRSFXXRequest::CBody &, std::string& out_strErrMsg);
	
		//BASE64编码
		bool _Base64Encode(const std::string& input, std::string* output);

		//模拟数据返回
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

		//IHttpClientPtr m_pHttpClient;  用临时的

		JsonParser::IJsonFactoryPtr m_pJsonFty;
	private:
		
		std::string m_strServerIP;  //服务IP
		std::string m_strToken;    //Token ID值

		std::string m_strServerPort; //服务端口

		std::string m_strSmulateFlag;  //是否模拟

    };
}