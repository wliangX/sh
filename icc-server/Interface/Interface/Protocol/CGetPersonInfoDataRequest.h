/*
cmd  请求
*/
#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/IRequest.h>
namespace ICC
{
	namespace PROTOCOL
	{
		struct BodyInfo
		{
			std::string m_strregId;             //由四川公安信息系统应用日志安全审计平台提供的应用系统标识
			std::string m_struserId;            //查询用户的公民身份号码
			std::string m_struserName;          //采用GA/T543，DE00002-姓名
			std::string m_struserType;          //用户所属类型：00:公安民警；01:辅警；02:其它
			std::string m_strorganization;      //用户所属单位的公安机关机构名称，采用GA/T543，DE00065-单位名称
			std::string m_strorganizationId;    //用户所属单位的公安机关机构代码，采用GA/T543，DE00060-公安机关机构代码
			std::string m_strterminalType;      //查询终端类型：00:PC终端（含台式机、笔记本、服务器等类型）；10：移动警务终端（含专用终端和通用终端）；
			std::string m_strinquireType;       //查询对象类型：00:公民身份证号码；01:公民姓名；02：机动车车牌号；03：机动车车架号；04：公民手机号码；05、人脸；ZZ：其它对象（含多个组合和批量查询等）
		};  
		struct InputParm
		{
			std::string m_strMessageSequence;                       //消息流水号
			std::string Condition;                                  //查询条件信
			std::string m_strMaxReturnNum;                          //最多总返回条数  接口响应返回的数据条数，最大支持1000，默认为100
			std::vector<std::string>Parafs;                         //查询的资源列表   服务方提供，需要查询的资源列表
			std::map<std::string, std::string> maporderParafs;      //查询资源编码
			std::string m_strareaCode;                              //字段列表  地区编码510000000000
			std::map<std::string, std::string> mapkey;              //数据为key:value
		};

		class CGetLoginRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strType = p_pJson->GetNodeValue("/body/type", "");
				m_oBody.m_strpoliceNo = p_pJson->GetNodeValue("/body/policeNo", "");
				m_oBody.m_struserId = p_pJson->GetNodeValue("/body/userId", "");
				m_oBody.m_strappId = p_pJson->GetNodeValue("/body/appId", "");

				return true;
			}
		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_strType;        //用户所属类型：00 : 公安民警；01:辅警；02:其它
				std::string m_strpoliceNo;    //警员编号
				std::string m_struserId;      //查询用户的公民身份号码
				std::string m_strappId;       //AppID
			};
			CBody m_oBody;
		};

		class CGetQueryHousRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strType = p_pJson->GetNodeValue("/body/type", "");
				m_oBody.m_strpoliceNo = p_pJson->GetNodeValue("/body/policeNo", "");
				m_oBody.m_struserId = p_pJson->GetNodeValue("/body/userId", "");
				m_oBody.m_strappId = p_pJson->GetNodeValue("/body/appId", "");

				return true;
			}
		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_strType;        //用户所属类型：00 : 公安民警；01:辅警；02:其它
				std::string m_strpoliceNo;    //警员编号
				std::string m_struserId;      //查询用户的公民身份号码
				std::string m_strappId;       //AppID
			};
			CBody m_oBody;
		};
		//四川省实名人员信息查询
		class CGetQueryIdInfoRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strIdNo = p_pJson->GetNodeValue("/body/IdNo", "");
				m_oBody.m_strmobile = p_pJson->GetNodeValue("/body/mobile", "");
				m_oBody.m_oBodyInfo.m_strregId = p_pJson->GetNodeValue("/body/regId", "");
				m_oBody.m_oBodyInfo.m_struserId = p_pJson->GetNodeValue("/body/userId", "");
				m_oBody.m_oBodyInfo.m_struserName = p_pJson->GetNodeValue("/body/userName", "");
				m_oBody.m_oBodyInfo.m_struserType = p_pJson->GetNodeValue("/body/userType", "");
				m_oBody.m_oBodyInfo.m_strorganization = p_pJson->GetNodeValue("/body/organization", "");
				m_oBody.m_oBodyInfo.m_strorganizationId = p_pJson->GetNodeValue("/body/organizationId", "");
				m_oBody.m_oBodyInfo.m_strterminalType = p_pJson->GetNodeValue("/body/terminalType", "");
				m_oBody.m_oBodyInfo.m_strinquireType = p_pJson->GetNodeValue("/body/inquireType", "");

				return true;
			}
		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_strIdNo;          //身份证号码
				std::string m_strmobile;        //手机号码
				BodyInfo m_oBodyInfo;           //公共信息
				//InputParm m_oInputParm;         //输入参数
			};
			CBody m_oBody;
		};

		//公民身份与案件关系查询服务
		class CGetQueryNameCaseDataRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strCertNum = p_pJson->GetNodeValue("/body/CertNum", "");
				m_oBody.m_oBodyInfo.m_strregId = p_pJson->GetNodeValue("/body/regId", "");
				m_oBody.m_oBodyInfo.m_struserId = p_pJson->GetNodeValue("/body/userId", "");
				m_oBody.m_oBodyInfo.m_struserName = p_pJson->GetNodeValue("/body/userName", "");
				m_oBody.m_oBodyInfo.m_struserType = p_pJson->GetNodeValue("/body/userType", "");
				m_oBody.m_oBodyInfo.m_strorganization = p_pJson->GetNodeValue("/body/organization", "");
				m_oBody.m_oBodyInfo.m_strorganizationId = p_pJson->GetNodeValue("/body/organizationId", "");
				m_oBody.m_oBodyInfo.m_strterminalType = p_pJson->GetNodeValue("/body/terminalType", "");
				m_oBody.m_oBodyInfo.m_strinquireType = p_pJson->GetNodeValue("/body/inquireType", "");
				return true;
			}
		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_strCertNum;       //证件号码
				BodyInfo m_oBodyInfo;           //公共信息
			};
			CBody m_oBody;
		};

		//交通违法记录查询
		class CGetQueryVehIllegalInfoRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strvehLicTname = p_pJson->GetNodeValue("/body/vehLicTname", "");
				m_oBody.m_strvehPlateNum = p_pJson->GetNodeValue("/body/vehPlateNum", "");
				m_oBody.m_oBodyInfo.m_strregId = p_pJson->GetNodeValue("/body/regId", "");
				m_oBody.m_oBodyInfo.m_struserId = p_pJson->GetNodeValue("/body/userId", "");
				m_oBody.m_oBodyInfo.m_struserName = p_pJson->GetNodeValue("/body/userName", "");
				m_oBody.m_oBodyInfo.m_struserType = p_pJson->GetNodeValue("/body/userType", "");
				m_oBody.m_oBodyInfo.m_strorganization = p_pJson->GetNodeValue("/body/organization", "");
				m_oBody.m_oBodyInfo.m_strorganizationId = p_pJson->GetNodeValue("/body/organizationId", "");
				m_oBody.m_oBodyInfo.m_strterminalType = p_pJson->GetNodeValue("/body/terminalType", "");
				m_oBody.m_oBodyInfo.m_strinquireType = p_pJson->GetNodeValue("/body/inquireType", "");
				return true;
			}
		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_strvehPlateNum;   //车牌号
				std::string m_strvehLicTname;   //号牌种类   
				BodyInfo m_oBodyInfo;           //公共信息
			};
			CBody m_oBody;
		};

		class CPersonInfoRequest : public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strPoliceNo = p_pJson->GetNodeValue("/body/police_no", "");
				m_oBody.m_strUserCardId = p_pJson->GetNodeValue("/body/user_card_id", "");
				m_oBody.m_strUserDept = p_pJson->GetNodeValue("/body/user_dept", "");
				m_oBody.m_strIdNo = p_pJson->GetNodeValue("/body/zjhm", "");
				m_oBody.m_oBodyInfo.m_strregId = p_pJson->GetNodeValue("/body/regId", "");
				m_oBody.m_oBodyInfo.m_struserId = p_pJson->GetNodeValue("/body/userId", "");
				m_oBody.m_oBodyInfo.m_struserName = p_pJson->GetNodeValue("/body/userName", "");
				m_oBody.m_oBodyInfo.m_struserType = p_pJson->GetNodeValue("/body/userType", "");
				m_oBody.m_oBodyInfo.m_strorganization = p_pJson->GetNodeValue("/body/organization", "");
				m_oBody.m_oBodyInfo.m_strorganizationId = p_pJson->GetNodeValue("/body/organizationId", "");
				m_oBody.m_oBodyInfo.m_strterminalType = p_pJson->GetNodeValue("/body/terminalType", "");
				m_oBody.m_oBodyInfo.m_strinquireType = p_pJson->GetNodeValue("/body/inquireType", "");
				return true;
			}
		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_strPoliceNo;      // 警员编号
				std::string m_strUserCardId;    // 民警身份证号
				std::string m_strUserDept;      // 单位编码
				std::string m_strIdNo;          // 查询身份证号码
				BodyInfo m_oBodyInfo;           //公共信息
			};
			CBody m_oBody;
		};

		class CGetQueryCaseVehInfoRequest : public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_strVehPlateNum = p_pJson->GetNodeValue("/body/VehPlateNum", "");
				m_oBodyInfo.m_strregId = p_pJson->GetNodeValue("/body/regId", "");
				m_oBodyInfo.m_struserId = p_pJson->GetNodeValue("/body/userId", "");
				m_oBodyInfo.m_struserName = p_pJson->GetNodeValue("/body/userName", "");
				m_oBodyInfo.m_struserType = p_pJson->GetNodeValue("/body/userType", "");
				m_oBodyInfo.m_strorganization = p_pJson->GetNodeValue("/body/organization", "");
				m_oBodyInfo.m_strorganizationId = p_pJson->GetNodeValue("/body/organizationId", "");
				m_oBodyInfo.m_strterminalType = p_pJson->GetNodeValue("/body/terminalType", "");
				m_oBodyInfo.m_strinquireType = p_pJson->GetNodeValue("/body/inquireType", "");

				return true;
			}
		public:
			CHeaderEx m_oHeader;
			std::string m_strVehPlateNum;   // 车牌号
			BodyInfo m_oBodyInfo;           //公共信息
		};
		//四川省标准地址层户地址查询服务
		class CAddrInfoRequest : public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_strAddrName = p_pJson->GetNodeValue("/body/AddrName", "");
				m_oBodyInfo.m_strregId = p_pJson->GetNodeValue("/body/regId", "");
				m_oBodyInfo.m_struserId = p_pJson->GetNodeValue("/body/userId", "");
				m_oBodyInfo.m_struserName = p_pJson->GetNodeValue("/body/userName", "");
				m_oBodyInfo.m_struserType = p_pJson->GetNodeValue("/body/userType", "");
				m_oBodyInfo.m_strorganization = p_pJson->GetNodeValue("/body/organization", "");
				m_oBodyInfo.m_strorganizationId = p_pJson->GetNodeValue("/body/organizationId", "");
				m_oBodyInfo.m_strterminalType = p_pJson->GetNodeValue("/body/terminalType", "");
				m_oBodyInfo.m_strinquireType = p_pJson->GetNodeValue("/body/inquireType", "");

				return true;
			}
		public:
			CHeaderEx m_oHeader;
			std::string m_strAddrName;      // 地址名字
			BodyInfo m_oBodyInfo;           //公共信息
		};
		//四川省实有单位查询
		class CCompInfoRequest : public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_strCompName = p_pJson->GetNodeValue("/body/CompName", "");
				m_oBodyInfo.m_strregId = p_pJson->GetNodeValue("/body/regId", "");
				m_oBodyInfo.m_struserId = p_pJson->GetNodeValue("/body/userId", "");
				m_oBodyInfo.m_struserName = p_pJson->GetNodeValue("/body/userName", "");
				m_oBodyInfo.m_struserType = p_pJson->GetNodeValue("/body/userType", "");
				m_oBodyInfo.m_strorganization = p_pJson->GetNodeValue("/body/organization", "");
				m_oBodyInfo.m_strorganizationId = p_pJson->GetNodeValue("/body/organizationId", "");
				m_oBodyInfo.m_strterminalType = p_pJson->GetNodeValue("/body/terminalType", "");
				m_oBodyInfo.m_strinquireType = p_pJson->GetNodeValue("/body/inquireType", "");

				return true;
			}
		public:
			CHeaderEx m_oHeader;
			std::string m_strCompName;      // 单位名称
			BodyInfo m_oBodyInfo;           //公共信息
		};
		//四川省实有房屋查询
		class CHousInfoRequest : public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_strHousOwnerAddrName = p_pJson->GetNodeValue("/body/HousOwnerAddrName", "");
				m_oBodyInfo.m_strregId = p_pJson->GetNodeValue("/body/regId", "");
				m_oBodyInfo.m_struserId = p_pJson->GetNodeValue("/body/userId", "");
				m_oBodyInfo.m_struserName = p_pJson->GetNodeValue("/body/userName", "");
				m_oBodyInfo.m_struserType = p_pJson->GetNodeValue("/body/userType", "");
				m_oBodyInfo.m_strorganization = p_pJson->GetNodeValue("/body/organization", "");
				m_oBodyInfo.m_strorganizationId = p_pJson->GetNodeValue("/body/organizationId", "");
				m_oBodyInfo.m_strterminalType = p_pJson->GetNodeValue("/body/terminalType", "");
				m_oBodyInfo.m_strinquireType = p_pJson->GetNodeValue("/body/inquireType", "");

				return true;
			}
		public:
			CHeaderEx m_oHeader;
			std::string m_strHousOwnerAddrName;      // 房主地址
			BodyInfo m_oBodyInfo;                    //公共信息
		};
		
		// 四川省大数据平台人员标签查询服务
		class CGetQueryPersonTagInfoRequest : public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_strCardId = p_pJson->GetNodeValue("/body/CertNum", "");
				m_oBodyInfo.m_strregId = p_pJson->GetNodeValue("/body/regId", "");
				m_oBodyInfo.m_struserId = p_pJson->GetNodeValue("/body/userId", "");
			    m_oBodyInfo.m_struserName = p_pJson->GetNodeValue("/body/userName", "");
				m_oBodyInfo.m_struserType = p_pJson->GetNodeValue("/body/userType", "");
				m_oBodyInfo.m_strorganization = p_pJson->GetNodeValue("/body/organization", "");
				m_oBodyInfo.m_strorganizationId = p_pJson->GetNodeValue("/body/organizationId", "");
				m_oBodyInfo.m_strterminalType = p_pJson->GetNodeValue("/body/terminalType", "");
				m_oBodyInfo.m_strinquireType = p_pJson->GetNodeValue("/body/inquireType", "");

				return true;
			}
		public:
			CHeaderEx m_oHeader;
			std::string m_strCardId;        //身份证号
			BodyInfo m_oBodyInfo;           //公共信息
		};

		class CGetQueryCaseInfoRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_CaseNo = p_pJson->GetNodeValue("/body/CaseNo", "");
				m_oBody.m_oBodyInfo.m_strregId = p_pJson->GetNodeValue("/body/regId", "");
				m_oBody.m_oBodyInfo.m_struserId = p_pJson->GetNodeValue("/body/userId", "");
				m_oBody.m_oBodyInfo.m_struserName = p_pJson->GetNodeValue("/body/userName", "");
				m_oBody.m_oBodyInfo.m_struserType = p_pJson->GetNodeValue("/body/userType", "");
				m_oBody.m_oBodyInfo.m_strorganization = p_pJson->GetNodeValue("/body/organization", "");
				m_oBody.m_oBodyInfo.m_strorganizationId = p_pJson->GetNodeValue("/body/organizationId", "");
				m_oBody.m_oBodyInfo.m_strterminalType = p_pJson->GetNodeValue("/body/terminalType", "");
				m_oBody.m_oBodyInfo.m_strinquireType = p_pJson->GetNodeValue("/body/inquireType", "");

				return true;
			}
		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_CaseNo;           //案件编号  (编码%)
				BodyInfo m_oBodyInfo;           //公共信息
			};
			CBody m_oBody;
		};
		//4.9	四川省机动车基本信息查询服务
		class CGetQueryCredInfoRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_credNum = p_pJson->GetNodeValue("/body/CredNum", "");
				m_oBody.m_vehPlateNum = p_pJson->GetNodeValue("/body/VehPlateNum", "");
				m_oBody.m_oBodyInfo.m_strregId = p_pJson->GetNodeValue("/body/regId", "");
				m_oBody.m_oBodyInfo.m_struserId = p_pJson->GetNodeValue("/body/userId", "");
				m_oBody.m_oBodyInfo.m_struserName = p_pJson->GetNodeValue("/body/userName", "");
				m_oBody.m_oBodyInfo.m_struserType = p_pJson->GetNodeValue("/body/userType", "");
				m_oBody.m_oBodyInfo.m_strorganization = p_pJson->GetNodeValue("/body/organization", "");
				m_oBody.m_oBodyInfo.m_strorganizationId = p_pJson->GetNodeValue("/body/organizationId", "");
				m_oBody.m_oBodyInfo.m_strterminalType = p_pJson->GetNodeValue("/body/terminalType", "");
				m_oBody.m_oBodyInfo.m_strinquireType = p_pJson->GetNodeValue("/body/inquireType", "");

				return true;
			}
		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_credNum;
				std::string m_vehPlateNum;
				BodyInfo m_oBodyInfo;           //公共信息
			};
			CBody m_oBody;
		};
		//4.10	四川省公民身份与户籍关系查询服务
		class CGetQueryCertInfoRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strCertNum = p_pJson->GetNodeValue("/body/CertNum", "");
				m_oBody.m_oBodyInfo.m_strregId = p_pJson->GetNodeValue("/body/regId", "");
				m_oBody.m_oBodyInfo.m_struserId = p_pJson->GetNodeValue("/body/userId", "");
				m_oBody.m_oBodyInfo.m_struserName = p_pJson->GetNodeValue("/body/userName", "");
				m_oBody.m_oBodyInfo.m_struserType = p_pJson->GetNodeValue("/body/userType", "");
				m_oBody.m_oBodyInfo.m_strorganization = p_pJson->GetNodeValue("/body/organization", "");
				m_oBody.m_oBodyInfo.m_strorganizationId = p_pJson->GetNodeValue("/body/organizationId", "");
				m_oBody.m_oBodyInfo.m_strterminalType = p_pJson->GetNodeValue("/body/terminalType", "");
				m_oBody.m_oBodyInfo.m_strinquireType = p_pJson->GetNodeValue("/body/inquireType", "");
				return true;
			}
		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_strCertNum;       //身份证号 
				std::string m_strDomicNum;      //户号 
				std::string m_strDomic_Adm_Div; //所属行政区划 
				BodyInfo m_oBodyInfo;           //公共信息
			};
			CBody m_oBody;
		};


		//四川省人员主题库信息查询服务
		class CGetQueryPersonInfoRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strCertNum = p_pJson->GetNodeValue("/body/Idno", "");
				m_oBody.m_strmobile = p_pJson->GetNodeValue("/body/Mobile", "");
				m_oBody.m_oBodyInfo.m_strregId = p_pJson->GetNodeValue("/body/regId", "");
				m_oBody.m_oBodyInfo.m_struserId = p_pJson->GetNodeValue("/body/userId", "");
				m_oBody.m_oBodyInfo.m_struserName = p_pJson->GetNodeValue("/body/userName", "");
				m_oBody.m_oBodyInfo.m_struserType = p_pJson->GetNodeValue("/body/userType", "");
				m_oBody.m_oBodyInfo.m_strorganization = p_pJson->GetNodeValue("/body/organization", "");
				m_oBody.m_oBodyInfo.m_strorganizationId = p_pJson->GetNodeValue("/body/organizationId", "");
				m_oBody.m_oBodyInfo.m_strterminalType = p_pJson->GetNodeValue("/body/terminalType", "");
				m_oBody.m_oBodyInfo.m_strinquireType = p_pJson->GetNodeValue("/body/inquireType", "");
				return true;
			}
		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_strCertNum;       //证件号码
				std::string m_strmobile;
				BodyInfo m_oBodyInfo;           //公共信息
			};
			CBody m_oBody;
		};


		//公民身份与案件关系查询服务
		class CGetQueryNameCaseDatAndCaseInfoRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strCertNum = p_pJson->GetNodeValue("/body/CertNum", "");
				m_oBody.m_oBodyInfo.m_strregId = p_pJson->GetNodeValue("/body/regId", "");
				m_oBody.m_oBodyInfo.m_struserId = p_pJson->GetNodeValue("/body/userId", "");
				m_oBody.m_oBodyInfo.m_struserName = p_pJson->GetNodeValue("/body/userName", "");
				m_oBody.m_oBodyInfo.m_struserType = p_pJson->GetNodeValue("/body/userType", "");
				m_oBody.m_oBodyInfo.m_strorganization = p_pJson->GetNodeValue("/body/organization", "");
				m_oBody.m_oBodyInfo.m_strorganizationId = p_pJson->GetNodeValue("/body/organizationId", "");
				m_oBody.m_oBodyInfo.m_strterminalType = p_pJson->GetNodeValue("/body/terminalType", "");
				m_oBody.m_oBodyInfo.m_strinquireType = p_pJson->GetNodeValue("/body/inquireType", "");
				return true;
			}
		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_strCertNum;       //证件号码
				BodyInfo m_oBodyInfo;           //公共信息
			};
			CBody m_oBody;
		};

		//四川省涉藏群体人员信息 
		class CGetQueryTibetanRelatedGroupsInfoRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strCertNum = p_pJson->GetNodeValue("/body/CertNum", "");
				m_oBody.m_oBodyInfo.m_strregId = p_pJson->GetNodeValue("/body/regId", "");
				m_oBody.m_oBodyInfo.m_struserId = p_pJson->GetNodeValue("/body/userId", "");
				m_oBody.m_oBodyInfo.m_struserName = p_pJson->GetNodeValue("/body/userName", "");
				m_oBody.m_oBodyInfo.m_struserType = p_pJson->GetNodeValue("/body/userType", "");
				m_oBody.m_oBodyInfo.m_strorganization = p_pJson->GetNodeValue("/body/organization", "");
				m_oBody.m_oBodyInfo.m_strorganizationId = p_pJson->GetNodeValue("/body/organizationId", "");
				m_oBody.m_oBodyInfo.m_strterminalType = p_pJson->GetNodeValue("/body/terminalType", "");
				m_oBody.m_oBodyInfo.m_strinquireType = p_pJson->GetNodeValue("/body/inquireType", "");
				return true;
			}
		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_strCertNum;       //证件号码
				BodyInfo m_oBodyInfo;           //公共信息
			};
			CBody m_oBody;
		};

		//四川省吸毒人员基本信息查询服务
		class CGetQueryDrugInfoRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strCertNum = p_pJson->GetNodeValue("/body/CertNum", "");
				m_oBody.m_strXm = p_pJson->GetNodeValue("/body/xm", "");
				m_oBody.m_strHjdzXzqhmc = p_pJson->GetNodeValue("/body/hjdzxzqhmc", "");
				m_oBody.m_strXbdm = p_pJson->GetNodeValue("/body/xbdm", "");
				m_oBody.m_oBodyInfo.m_strregId = p_pJson->GetNodeValue("/body/regId", "");
				m_oBody.m_oBodyInfo.m_struserId = p_pJson->GetNodeValue("/body/userId", "");
				m_oBody.m_oBodyInfo.m_struserName = p_pJson->GetNodeValue("/body/userName", "");
				m_oBody.m_oBodyInfo.m_struserType = p_pJson->GetNodeValue("/body/userType", "");
				m_oBody.m_oBodyInfo.m_strorganization = p_pJson->GetNodeValue("/body/organization", "");
				m_oBody.m_oBodyInfo.m_strorganizationId = p_pJson->GetNodeValue("/body/organizationId", "");
				m_oBody.m_oBodyInfo.m_strterminalType = p_pJson->GetNodeValue("/body/terminalType", "");
				m_oBody.m_oBodyInfo.m_strinquireType = p_pJson->GetNodeValue("/body/inquireType", "");
				return true;
			}
		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_strCertNum;       //证件号码
				std::string m_strXm;            //姓名
				std::string m_strHjdzXzqhmc;    //户籍地址行政区划名称
				std::string m_strXbdm;          //性别代码
				BodyInfo m_oBodyInfo;           //公共信息
			};
			CBody m_oBody;
		};

		// 全国在逃人员信息查询服务
		class CGetQueryEscapeInfoRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_strCerdNum = p_pJson->GetNodeValue("/body/zjhm", "");
				m_oBodyInfo.m_strregId = p_pJson->GetNodeValue("/body/regId", "");
				m_oBodyInfo.m_struserId = p_pJson->GetNodeValue("/body/userId", "");
				m_oBodyInfo.m_struserName = p_pJson->GetNodeValue("/body/userName", "");
				m_oBodyInfo.m_struserType = p_pJson->GetNodeValue("/body/userType", "");
				m_oBodyInfo.m_strorganization = p_pJson->GetNodeValue("/body/organization", "");
				m_oBodyInfo.m_strorganizationId = p_pJson->GetNodeValue("/body/organizationId", "");
				m_oBodyInfo.m_strterminalType = p_pJson->GetNodeValue("/body/terminalType", "");
				m_oBodyInfo.m_strinquireType = p_pJson->GetNodeValue("/body/inquireType", "");

				return true;
			}
		public:
			CHeaderEx m_oHeader;
			std::string m_strCerdNum;       // 身份证号码
			BodyInfo m_oBodyInfo;           // 公共信息
		};

		//全国情报重点人员信息查询服务
		class CGetQueryPersonKeyInfoRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_strCerdNum = p_pJson->GetNodeValue("/body/zjhm", "");
				m_strPersonKeyNum = p_pJson->GetNodeValue("/body/bjzdrybh", "");
				m_oBodyInfo.m_strregId = p_pJson->GetNodeValue("/body/regId", "");
				m_oBodyInfo.m_struserId = p_pJson->GetNodeValue("/body/userId", "");
				m_oBodyInfo.m_struserName = p_pJson->GetNodeValue("/body/userName", "");
				m_oBodyInfo.m_struserType = p_pJson->GetNodeValue("/body/userType", "");
				m_oBodyInfo.m_strorganization = p_pJson->GetNodeValue("/body/organization", "");
				m_oBodyInfo.m_strorganizationId = p_pJson->GetNodeValue("/body/organizationId", "");
				m_oBodyInfo.m_strterminalType = p_pJson->GetNodeValue("/body/terminalType", "");
				m_oBodyInfo.m_strinquireType = p_pJson->GetNodeValue("/body/inquireType", "");

				return true;
			}
		public:
			CHeaderEx m_oHeader;
			std::string m_strCerdNum;				// 证件号码
			std::string m_strPersonKeyNum;			// 部级重点人员编号
			BodyInfo m_oBodyInfo;					// 公共信息
		};

		//全国人口照片查询服务
		class CGetQueryPersonPhotoInfoRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_strCerdNum = p_pJson->GetNodeValue("/body/idCard", "");
				m_oBodyInfo.m_strregId = p_pJson->GetNodeValue("/body/regId", "");
				m_oBodyInfo.m_struserId = p_pJson->GetNodeValue("/body/userId", "");
				m_oBodyInfo.m_struserName = p_pJson->GetNodeValue("/body/userName", "");
				m_oBodyInfo.m_struserType = p_pJson->GetNodeValue("/body/userType", "");
				m_oBodyInfo.m_strorganization = p_pJson->GetNodeValue("/body/organization", "");
				m_oBodyInfo.m_strorganizationId = p_pJson->GetNodeValue("/body/organizationId", "");
				m_oBodyInfo.m_strterminalType = p_pJson->GetNodeValue("/body/terminalType", "");
				m_oBodyInfo.m_strinquireType = p_pJson->GetNodeValue("/body/inquireType", "");

				return true;
			}
		public:
			CHeaderEx m_oHeader;
			std::string m_strCerdNum;				// 证件号码
			BodyInfo m_oBodyInfo;					// 公共信息
		};

		//四川省地址查询服务 ---  一标三实
		class CAddressInfoRequest : public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_strAddrName = p_pJson->GetNodeValue("/body/AddrName", "");
				m_strLon = p_pJson->GetNodeValue("/body/Lon", "");
				m_strLat = p_pJson->GetNodeValue("/body/Lat", "");
				m_oBodyInfo.m_strregId = p_pJson->GetNodeValue("/body/regId", "");
				m_oBodyInfo.m_struserId = p_pJson->GetNodeValue("/body/userId", "");
				m_oBodyInfo.m_struserName = p_pJson->GetNodeValue("/body/userName", "");
				m_oBodyInfo.m_struserType = p_pJson->GetNodeValue("/body/userType", "");
				m_oBodyInfo.m_strorganization = p_pJson->GetNodeValue("/body/organization", "");
				m_oBodyInfo.m_strorganizationId = p_pJson->GetNodeValue("/body/organizationId", "");
				m_oBodyInfo.m_strterminalType = p_pJson->GetNodeValue("/body/terminalType", "");
				m_oBodyInfo.m_strinquireType = p_pJson->GetNodeValue("/body/inquireType", "");

				return true;
			}
		public:
			CHeaderEx m_oHeader;
			std::string m_strAddrName;      // 地址名字
			std::string m_strLon;           // 经度
			std::string m_strLat;           // 纬度
			BodyInfo m_oBodyInfo;           // 公共信息
		};


	}
}