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
		//��¼ token ��֤
		void OnNotifGetLoginToken(ObserverPattern::INotificationPtr p_pNotify);
		//�Ĵ�ʡʵ����Ա��Ϣ��ѯ
		void OnNotifQueryIdInfo(ObserverPattern::INotificationPtr p_pNotify);
		//��������밸����ϵ��ѯ����
		void OnNotifQueryNameCaseData(ObserverPattern::INotificationPtr p_pNotify);
		//�Ĵ�ʡ��ͨΥ����¼���ѯ����
		void OnNotifQueryVehIllegalInfo(ObserverPattern::INotificationPtr p_pNotify);
		//�Ĵ�ʡ��Ա�������Ϣ��ѯ����
		void OnNotifQueryPersonInfo(ObserverPattern::INotificationPtr p_pNotify);
		//�Ĵ�ʡ�����밸���ӿ�
		void OnNotifQueryCaseVehInfo(ObserverPattern::INotificationPtr p_pNotify);
		// �Ĵ�ʡ������ƽ̨��Ա��ǩ��ѯ����
		void OnNotifQueryPersonTagInfo(ObserverPattern::INotificationPtr p_pNotify);
		//�Ĵ�ʡ��׼��ַ�㻧��ַ��ѯ����
		void OnNotifQueryAddrInfo(ObserverPattern::INotificationPtr p_pNotify);
		//�Ĵ�ʡʵ�з��ݲ�ѯ
		void OnNotifQueryHousInfo(ObserverPattern::INotificationPtr p_pNotify);
		//�Ĵ�ʡʵ�е�λ��ѯ
		void OnNotifQueryCompInfo(ObserverPattern::INotificationPtr p_pNotify);
		//������Ϣ��ѯ
		void OnNotifQueryCaseInfo(ObserverPattern::INotificationPtr p_pNotify);
		//�Ĵ�ʡ������������Ϣ��ѯ����
		void OnNotifQueryCredInfo(ObserverPattern::INotificationPtr p_pNotify);
		//�Ĵ�ʡ��������뻧����ϵ��ѯ����
		void OnNotifQueryCertInfo(ObserverPattern::INotificationPtr p_pNotify);
		//�Ĵ�ʡ���Ⱥ����Ա��Ϣ 
		void OnNotifQueryTibetanRelatedGroupsInfo(ObserverPattern::INotificationPtr p_pNotify);
		//�Ĵ�ʡ������Ա������Ϣ��ѯ����
		void OnNotifQueryDrugInfo(ObserverPattern::INotificationPtr p_pNotify);
		// ȫ��������Ա��Ϣ��ѯ����
		void OnNotifQueryEscapeInfo(ObserverPattern::INotificationPtr p_pNotify);
		//ȫ���鱨�ص���Ա��Ϣ��ѯ����
		void OnNotifQueryPersonKeyInfo(ObserverPattern::INotificationPtr p_pNotify);
		//ȫ���˿���Ƭ��ѯ����
		void OnNotifQueryPersonPhotoInfo(ObserverPattern::INotificationPtr p_pNotify);
		//��Ա�밸����ϵ��ѯ�ӿ� �� ��������ӿ�
		void OnNotifQueryNameCaseDataAndCaseInfo(ObserverPattern::INotificationPtr p_pNotify);
		//������ϵ��ѯ�ӿ� �� ��Ա��Ϣ��ѯ����ӿ�
		void OnNotifQueryCertInfoAndPersonInfo(ObserverPattern::INotificationPtr p_pNotify);
		//��������Ϣ��ѯ�ӿ� �� ��Ա��Ϣ��ѯ����ӿ�
		void OnNotifQueryCredInfoAndPersonInfo(ObserverPattern::INotificationPtr p_pNotify);
		//�����밸���ӿ� �� ��������ӿ�
		void OnNotifQueryCaseVehInfoAndCaseInfo(ObserverPattern::INotificationPtr p_pNotify);

		//��ѯ��ַ
		void OnNotifQueryAddressInfo(ObserverPattern::INotificationPtr p_pNotify);

		//��ȡNacos�������
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
		// �ַ����滻,��souceStr�е�subStr �滻Ϊ newStr
		std::string Subreplace(const std::string souceStr, const std::string subStr, const std::string newStr);
		std::string GetPostTextByContent(const std::string strSearch);
		std::string GetCarCodeFromDataBase(std::string strvehLicTname);
		bool OutputChineseLib(std::vector<std::string> strVecSource, std::map<std::string, std::string>& MapResult);
		void GetHouHeadRelFromConfig();

		//������Ϣ �ظ�ǰ�� 
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
		// ������ post����
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
		// ��ѯaia��������Ϣ
		void PostQueryAiaPersonInfo(PROTOCOL::CGetQueryPersonInfoRequest& p_pRequest, PROTOCOL::CPostQueryAiaPersonInfoRequest& p_pSponse);

		// ���������ݽӿ� body Content
		std::string BuildPostContentRespond(PROTOCOL::BodyInfo& p_pBodyInfoReques, PROTOCOL::InputParm& p_pInputParm);
		// ���������ݽӿ� Header
		std::map<std::string, std::string> PostHeader(std::string p_pServiceId, std::string p_pcmd);
		bool GetServiceInfo(std::string& p_strServiceName, std::string& p_strIp, std::string& p_strPort);
	private:
		//�����ļ���ȡ����
		bool ReadConfig();
		//��¼ token ��֤
		bool GetLoginToken();
		// ʱ���ת��
		std::string ToStringTime(std::string p_pDateTime);
		// ɾ��ָ���ַ�
		void StringCar();
		//������������תcode
		void GetDistrictCodeMapFromConfig();

		//post url��������û� icc_t_post_url_map
		void GetPostUrlMapFromConfig();

		//��ȡcmd ��Ӧ��url
		std::string GetPostUrl(std::string p_pcmd);
		//��ȡcmd ��Ӧ�� serviceid
		std::string GetPostServiceId(std::string p_pcmd);

		//post url��������û� icc_t_post_url_map
		void GetKeyInfoMapFromConfig();

		//��ȡcmd ��Ӧ��url
		std::string GetKeyInfo(std::string p_keyinfo_name);

		// ����������γ��֮��ľ���
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
		std::string m_strServerIP;                          //����IP
		std::string m_strToken;                             //Token IDֵ
		std::string m_strServerPort;                        //����˿�
		std::string m_strSmulateFlag;                       //�Ƿ�ģ��
		std::string m_strtype;
		std::string m_strpoliceNo;
		std::string m_strUserId;
		std::string m_strAppId;
		std::string m_strapptoken;
		std::string m_strusertoken;
		std::string m_strSenderId;
		std::string m_strGroupId;
		std::string m_strpublicKey;                         //��Կ��Ϣ
		std::string m_strTagAppId;
		std::string m_strIp;
		std::string m_strMac;
		std::string m_strareaCode;
		std::string m_strConfig;
		std::string m_strEscapeInfo;
		std::string m_strbody;
		std::map<std::string, std::string> m_strHouHeadRel;               // ������ϵ
		std::map<std::string, std::string> m_strChineseLib;               // �����ֿ�

		std::string m_strcar;                                             // ����

		std::map<std::string, std::string> m_strDistrictCodeMap;          // ��������

		std::map<std::string, std::string> m_mapPostUrl;                  // cmd��cur��Ӧ��ϵ
		std::map<std::string, std::string> m_mapPostServiceId;            // cmd��ServiceId��Ӧ��ϵ
		std::map<std::string, std::string> m_mapKeyInfo;                  // keyinfo_name��attention_tag��Ӧ��ϵ
		std::string m_strOtherKeyInfo;                                    // �����ص���Ա

		std::string m_strCityName;                                        // ��������
		std::string m_strReturnNum;                                       // �����ݷ�������
		std::string m_strnormaLocation;                                   // ��һ����γ��
		size_t m_intaddrSum;                                              // �����ݵ�ַ�����������  
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