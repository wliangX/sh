#pragma once
#include "CommonWorkThread.h"

#define THREADCOUNT 4

namespace ICC
{
	class CBusinessImpl;
	typedef void (CBusinessImpl::* ProcNotify)(ObserverPattern::INotificationPtr p_pNotifiRequest);

	class CBusinessImpl : public CBusinessBase, CCommonThreadCore
	{
	public:
		virtual void OnInit();
		virtual void OnStart();
		virtual void OnStop();
		virtual void OnDestroy();
	public:
		//登录 token 认证
		void OnNotifGetLoginToken(ObserverPattern::INotificationPtr p_pNotify);
		//四川省实名人员信息查询
		void OnNotifQueryIdInfo(ObserverPattern::INotificationPtr p_pNotify);
		//公民身份与案件关系查询服务
		void OnNotifQueryNameCaseData(ObserverPattern::INotificationPtr p_pNotify);
		//四川省交通违法记录表查询服务
		void OnNotifQueryVehIllegalInfo(ObserverPattern::INotificationPtr p_pNotify);
		//四川省人员主题库信息查询服务
		void OnNotifQueryPersonInfo(ObserverPattern::INotificationPtr p_pNotify);
		//四川省车牌与案件接口
		void OnNotifQueryCaseVehInfo(ObserverPattern::INotificationPtr p_pNotify);
		// 四川省大数据平台人员标签查询服务
		void OnNotifQueryPersonTagInfo(ObserverPattern::INotificationPtr p_pNotify);
		//四川省标准地址层户地址查询服务
		void OnNotifQueryAddrInfo(ObserverPattern::INotificationPtr p_pNotify);
		//四川省实有房屋查询
		void OnNotifQueryHousInfo(ObserverPattern::INotificationPtr p_pNotify);
		//四川省实有单位查询
		void OnNotifQueryCompInfo(ObserverPattern::INotificationPtr p_pNotify);
		//案件信息查询
		void OnNotifQueryCaseInfo(ObserverPattern::INotificationPtr p_pNotify);
		//四川省机动车基本信息查询服务
		void OnNotifQueryCredInfo(ObserverPattern::INotificationPtr p_pNotify);
		//四川省公民身份与户籍关系查询服务
		void OnNotifQueryCertInfo(ObserverPattern::INotificationPtr p_pNotify);
		//四川省涉藏群体人员信息 
		void OnNotifQueryTibetanRelatedGroupsInfo(ObserverPattern::INotificationPtr p_pNotify);
		//四川省吸毒人员基本信息查询服务
		void OnNotifQueryDrugInfo(ObserverPattern::INotificationPtr p_pNotify);
		// 全国在逃人员信息查询服务
		void OnNotifQueryEscapeInfo(ObserverPattern::INotificationPtr p_pNotify);
		//全国情报重点人员信息查询服务
		void OnNotifQueryPersonKeyInfo(ObserverPattern::INotificationPtr p_pNotify);
		//全国人口照片查询服务
		void OnNotifQueryPersonPhotoInfo(ObserverPattern::INotificationPtr p_pNotify);
		//人员与案件关系查询接口 与 案件详情接口
		void OnNotifQueryNameCaseDataAndCaseInfo(ObserverPattern::INotificationPtr p_pNotify);
		//户籍关系查询接口 与 人员信息查询主题接口
		void OnNotifQueryCertInfoAndPersonInfo(ObserverPattern::INotificationPtr p_pNotify);
		//机动车信息查询接口 与 人员信息查询主题接口
		void OnNotifQueryCredInfoAndPersonInfo(ObserverPattern::INotificationPtr p_pNotify);
		//车牌与案件接口 与 案件详情接口
		void OnNotifQueryCaseVehInfoAndCaseInfo(ObserverPattern::INotificationPtr p_pNotify);

		//查询地址
		void OnNotifQueryAddressInfo(ObserverPattern::INotificationPtr p_pNotify);

		//获取Nacos服务参数
		void OnReceiveSynNacosParams(ObserverPattern::INotificationPtr p_pNotifiRequest);

		void OnNotifGetServiceInfo(ObserverPattern::INotificationPtr p_pNotify);

	private:
		void BuildAddrInfoRespond(PROTOCOL::CAddrInfoRequest& p_oRequest, PROTOCOL::CAddrInfoRespond& p_oRespond);
		void BuildCompInfoRespond(PROTOCOL::CCompInfoRequest& p_oRequest, PROTOCOL::CCompInfoRespond& p_oRespond);
		void BuildHousInfoRespond(PROTOCOL::CHousInfoRequest& p_oRequest, PROTOCOL::CHousInfoRespond& p_oRespond);
		void BuildEscapeInfoRespond(PROTOCOL::CGetQueryEscapeInfoRequest& p_oRequest, PROTOCOL::CEscapeInfoRespond& p_oRespond);
		void BuildPersonKeyInfoRespond(PROTOCOL::CGetQueryPersonKeyInfoRequest& p_oRequest, PROTOCOL::CPersonKeyInfoRespond& p_oRespond);
		void BuildPersonPhotoInfoRespond(PROTOCOL::BodyInfo& p_oRequest, const std::string strIdCard, PROTOCOL::CPersonPhotoInfoRespond& p_oRespond);

		std::string PersonKeyToString(PROTOCOL::CPersonKeyInfoRespond& p_oRespond);
		// 字符串替换,将souceStr中的subStr 替换为 newStr
		std::string Subreplace(const std::string souceStr, const std::string subStr, const std::string newStr);
		std::string GetPostTextByContent(const std::string strSearch);
		std::string GetCarCodeFromDataBase(std::string strvehLicTname);
		bool OutputChineseLib(std::vector<std::string> strVecSource, std::map<std::string, std::string>& MapResult);
		void GetHouHeadRelFromConfig();

		//构建消息 回复前端 
		void BuildGetQueryIdInfoRespond(PROTOCOL::CPushIdInfoRequest p_pPushIdInfoRequest, ObserverPattern::INotificationPtr p_pNotify);
		void BuildGetQueryNameCaseDataRespond(PROTOCOL::CPostQueryNameCaseDataRequest p_pPostQueryNameCaseDataRequest, ObserverPattern::INotificationPtr p_pNotify);
		void BuildGetQueryVehIllegalInfoRespond(PROTOCOL::CPostQueryVehIllegalInfoRequest p_pPostQueryVehIllegalInfoRequest, ObserverPattern::INotificationPtr p_pNotify);
		void BuildGetQueryCertInfoRespond(PROTOCOL::CPostQueryCertInfoRequest p_pPostQueryCertInfooRequest, ObserverPattern::INotificationPtr p_pNotify);
		void BuildGetQueryCredInfoRespond(PROTOCOL::CPostQueryCredInfoRequest p_pPostQueryCredInfoRequest, ObserverPattern::INotificationPtr p_pNotify);
		void BuildGetQueryPersonInfoRespond(PROTOCOL::CPostQueryPersonInfoRequest p_pPostQueryPersonInfoRequest, ObserverPattern::INotificationPtr p_pNotify);
		
		void BuildGetQueryAiaPersonInfoRespond(PROTOCOL::CPostQueryAiaPersonInfoRequest p_pPostQueryPersonInfoRequest, ObserverPattern::INotificationPtr p_pNotify);
		
		void BuildQueryCaseInfoRespond(PROTOCOL::CPostQueryCaseInfoRequest& p_pPostCaseVehInfoRequest, ObserverPattern::INotificationPtr p_pNotify);
		void BuildGetCaseVehInfoRespond(PROTOCOL::CPostQueryCaseVehInfoRequest& p_oRequest, ObserverPattern::INotificationPtr p_pNotify);
		void BuildPersonTagInfo(PROTOCOL::BodyInfo& p_oRequest, const std::string strIdCard, PROTOCOL::CPostQueryPersonTagInfoRespond& l_oRespond);
		void BuildGetPersonTagRespond(PROTOCOL::CPostQueryPersonTagInfoRespond& p_pPostQueryPersonTagInfo, ObserverPattern::INotificationPtr p_pNotify);

		void BuildGetQueryTibetanRelatedGroupsInfoRespond(PROTOCOL::CPostQueryTibetanRelatedGroupsInfoRequest p_pPostQueryTibetanRelatedGroupsInfoRequest, ObserverPattern::INotificationPtr p_pNotify);
		void BuildGetQueryDrugInfoRespond(PROTOCOL::CPostQueryDrugInfoRequest p_pPostQueryDrugInfoRRequest, ObserverPattern::INotificationPtr p_pNotify);

		void BuildGetQueryNameCaseDataAndCaseInfoRespond(PROTOCOL::CGetQueryNameCaseDataAndCaseInfoRespond& p_pRespond, int Index);
		void BuildGetQueryCertInfoAndPersonInfoRespond(PROTOCOL::CGetQueryCertInfoAndPersonInfoRespond& p_pRespond, int Index);

		void BuildGetQueryCredInfoAndPersonInfoRespond(PROTOCOL::CGetQueryCredInfoAndPersonInfoRespond& p_pRespond, int Index);
		void BuildGetQueryCaseVehInfoAndCaseInfoRespond(PROTOCOL::CGetQueryCaseVehInfoAndCaseInfoRespond& p_pRespond, int Index);

		void BuildGetQueryAddressRespond(PROTOCOL::CAddrInfoRespond p_AddrInfoRespond, PROTOCOL::CCompInfoRespond p_CompInfoRespond, PROTOCOL::CAddressInfoRespond& p_strRespond);
		void BuildRealDistanceAndSort(PROTOCOL::CAddressInfoRequest p_pRequest, PROTOCOL::CAddrInfoRespond p_AddrInfoRespond, PROTOCOL::CCompInfoRespond p_CompInfoRespond);
		void SetNacosParams(const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName);
	private:
		// 大数据 post请求
		void PostQueryIdInfo(PROTOCOL::CGetQueryIdInfoRequest& p_pRequest, PROTOCOL::CPushIdInfoRequest& p_pSponse);
		void PostQueryNameCaseData(PROTOCOL::CGetQueryNameCaseDataRequest& p_pRequest, PROTOCOL::CPostQueryNameCaseDataRequest& p_pSponse);
		void PostQueryCaseInfo(PROTOCOL::CGetQueryCaseInfoRequest& p_pRequest, PROTOCOL::CPostQueryCaseInfoRequest& p_pSponse);
		void PostQueryCredInfo(PROTOCOL::CGetQueryCredInfoRequest& p_pRequest, PROTOCOL::CPostQueryCredInfoRequest& p_pSponse);
		void PostQueryCertInfo(PROTOCOL::CGetQueryCertInfoRequest& p_pRequest, PROTOCOL::CPostQueryCertInfoRequest& p_pSponse);

		void PostQueryCaseVehInfo(PROTOCOL::CGetQueryCaseVehInfoRequest& p_pRequest, PROTOCOL::CPostQueryCaseVehInfoRequest& p_pSponse);

		void PostQueryPersonInfo(PROTOCOL::CGetQueryPersonInfoRequest& p_pRequest, PROTOCOL::CPostQueryPersonInfoRequest& p_pSponse);

		void PostQueryTibetanRelatedGroupsInfo(PROTOCOL::CGetQueryTibetanRelatedGroupsInfoRequest& p_pGetRequest, PROTOCOL::CPostQueryTibetanRelatedGroupsInfoRequest& p_pPostRequest);
		void PostQueryDrugInfo(PROTOCOL::CGetQueryDrugInfoRequest& p_pGetRequest, PROTOCOL::CPostQueryDrugInfoRequest& p_pPostRequest);
		void PostQueryPersonTagInfo(PROTOCOL::BodyInfo& p_oRequest, const std::string strIDCard, PROTOCOL::CPostQueryPersonTagInfoRespond& p_pPostRequest);


		void PostQueryAddrInfoRespond(PROTOCOL::CAddressInfoRequest& p_pRequest, PROTOCOL::CAddrInfoRespond& p_Respond, std::string p_strGuid = "");
		void PostQueryCompInfoRespond(PROTOCOL::CAddressInfoRequest& p_pRequest, PROTOCOL::CCompInfoRespond& p_pRespond);
		// 查询aia大数据信息
		void PostQueryAiaPersonInfo(PROTOCOL::CGetQueryPersonInfoRequest& p_pRequest, PROTOCOL::CPostQueryAiaPersonInfoRequest& p_pSponse);

		// 构建大数据接口 body Content
		std::string BuildPostContentRespond(PROTOCOL::BodyInfo& p_pBodyInfoReques, PROTOCOL::InputParm& p_pInputParm);
		// 构建大数据接口 Header
		std::map<std::string, std::string> PostHeader(std::string p_pServiceId, std::string p_pcmd);
		bool GetServiceInfo(std::string& p_strServiceName, std::string& p_strIp, std::string& p_strPort);
	private:
		//配置文件读取函数
		bool ReadConfig();
		//登录 token 认证
		bool GetLoginToken();
		// 时间戳转换
		std::string ToStringTime(std::string p_pDateTime);
		// 删除指定字符
		void StringCar();
		//行政区划名称转code
		void GetDistrictCodeMapFromConfig();

		//post url请求可配置化 icc_t_post_url_map
		void GetPostUrlMapFromConfig();

		//获取cmd 对应的url
		std::string GetPostUrl(std::string p_pcmd);
		//获取cmd 对应的 serviceid
		std::string GetPostServiceId(std::string p_pcmd);

		//post url请求可配置化 icc_t_post_url_map
		void GetKeyInfoMapFromConfig();

		//获取cmd 对应的url
		std::string GetKeyInfo(std::string p_keyinfo_name);

		// 计算两个经纬度之间的距离
		double rad(double d);
		double RealDistance(double lat1, double lng1, double lat2, double lng2);

	private:
		void    _OnReceiveNotify(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		void    _DispatchNotify(ObserverPattern::INotificationPtr p_pNotifiReqeust, const std::string& strCmdName);
		virtual void ProcMessage(CommonThread_data msg_data);
		CommonWorkThreadPtr _GetThread();
		void    _InitProcNotifys();
		void    _CreateThreads();
		void    _DestoryThreads();

		unsigned int                        m_uCurrentThreadIndex;
		unsigned int                        m_uProcThreadCount;
		std::string                         m_strDispatchMode;

		std::mutex                          m_mutexThread;
		std::vector<CommonWorkThreadPtr>    m_vecProcThreads;
		std::map<std::string, ProcNotify>   m_mapFuncs;

	private:
		Log::ILogPtr m_pLog;
		Config::IConfigPtr m_pConfig;

		StringUtil::IStringUtilPtr m_pString;
		DateTime::IDateTimePtr m_pDateTime;
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
		Redis::IRedisClientPtr m_pRedisClient;

		DataBase::IDBConnPtr m_pDBConn;
		IHttpClientPtr m_pHttpClient;
		JsonParser::IJsonFactoryPtr m_pJsonFty;

		Timer::ITimerManagerPtr		m_pTimerMgr;
	private:
		std::string m_strIsUsing;
		std::string m_strServerIP;                          //服务IP
		std::string m_strToken;                             //Token ID值
		std::string m_strServerPort;                        //服务端口
		std::string m_strSmulateFlag;                       //是否模拟
		std::string m_strtype;
		std::string m_strpoliceNo;
		std::string m_strUserId;
		std::string m_strAppId;
		std::string m_strapptoken;
		std::string m_strusertoken;
		std::string m_strSenderId;
		std::string m_strGroupId;
		std::string m_strpublicKey;                         //公钥信息
		std::string m_strTagAppId;
		std::string m_strIp;
		std::string m_strMac;
		std::string m_strareaCode;
		std::string m_strConfig;
		std::string m_strEscapeInfo;
		std::string m_strbody;
		std::map<std::string, std::string> m_strHouHeadRel;               // 户主关系
		std::map<std::string, std::string> m_strChineseLib;               // 中文字库

		std::string m_strcar;                                             // 车牌

		std::map<std::string, std::string> m_strDistrictCodeMap;          // 行政区划

		std::map<std::string, std::string> m_mapPostUrl;                  // cmd与cur对应关系
		std::map<std::string, std::string> m_mapPostServiceId;            // cmd与ServiceId对应关系
		std::map<std::string, std::string> m_mapKeyInfo;                  // keyinfo_name与attention_tag对应关系
		std::string m_strOtherKeyInfo;                                    // 其他重点人员

		std::string m_strCityName;                                        // 地区名称
		std::string m_strReturnNum;                                       // 大数据返回数量
		std::string m_strnormaLocation;                                   // 归一化经纬度
		size_t m_intaddrSum;                                              // 大数据地址返回最多条数  
		std::map<std::string, PROTOCOL::CAddrInfoRespond> m_mapPostQueryAddrInfoRespond;
		std::mutex m_mutexPostQueryAddrInfoRespond;

		int m_intTimeOut;
		std::string m_strNewKeyInfo;
	private://Nacos
		std::string					m_strAiaAlarmServerName;
		std::string					m_strNacosServerIp;
		std::string					m_strNacosServerPort;
		std::string					m_strNacosServerNamespace;
		std::string					m_strNacosServerGroupName;
		std::string                 m_strAiaAlarmSendServerIp;
		std::string                 m_strAiaAlarmSendServerPort;
		boost::mutex			    m_mutexNacosParams;
		bool        m_bFlag;

	};
}