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
		//较验认证信息
		bool _ValidAuthInfo(const ICC::PROTOCOL::CAuthInfo&, std::string& out_strErrMsg);

		//较验获取报警人身份证号请求消息
		bool _ValidGetBJRSFZHReqMsg(const ICC::PROTOCOL::CGetBJRSFZHRequest::CBody&, std::string& out_strErrMsg);

		//较验获取报警人身份信息请求消息
		bool _ValidGetBJRSFXXReqMsg(const ICC::PROTOCOL::CGetBJRSFXXRequest::CBody&, std::string& out_strErrMsg);

		//BASE64编码
		bool _Base64Encode(const std::string& input, std::string* output);

		//登录接口 /api/zhzxapi/login  
		bool Login();

		void BuildBJRSFHMRRespond(PROTOCOL::CGetBJRSFZHIdentiyInfoRequest& m_oInfoRequest, ObserverPattern::INotificationPtr p_pNotify);

		void BuildBJRSFXXRRespond(PROTOCOL::CGetBJRSFZHIdentiyInfoRequest& m_oInfoRequest, ObserverPattern::INotificationPtr p_pNotify);

		void InsertIdentiyInfo(PROTOCOL::CGetBJRSFZHIdentiyInfoRequest& bjrResponseInfo);

		// 字符串替换,将souceStr中的subStr 替换为 newStr
		std::string Subreplace(const std::string souceStr, const std::string subStr, const std::string newStr);
		//模拟数据返回
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

		std::string m_strServerIP;  //服务IP
		std::string m_strToken;    //Token ID值

		std::string m_strServerPort; //服务端口

		std::string m_strSmulateFlag;  //是否模拟

		std::string m_strUsername;    //用户名 ，必填
		std::string m_strPassword;    //密码 ，必填

		int m_intTimeOut;

	};
}