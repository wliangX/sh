/*
报警人身份信息解析  请求
*/
#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/IRequest.h>
namespace ICC
{
	namespace PROTOCOL
	{
		class appTokenId
		{
		public:
			std::string m_strappToken;
			std::string m_strexpireAt;   //到期时间
		};

		class userTokenId
		{
		public:
			std::string m_struserToken;
			std::string m_strexpireAt;   //到期时间
		};

		class CGetLoginTokenRequest :
			public IRequest
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				p_pJson->SetNodeValue("/appTokenId", m_oappTokenId);
				p_pJson->SetNodeValue("/userTokenId", m_ouserTokenId);

				p_pJson->SetNodeValue("/message", m_strmessage);
				p_pJson->SetNodeValue("/path", m_strpath);
				p_pJson->SetNodeValue("/error", m_strerror);
				p_pJson->SetNodeValue("/status", m_strstatus);
				p_pJson->SetNodeValue("/timestamp", m_strtimestamp);
				return p_pJson->ToString();
			}

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

			    m_oappTokenId = p_pJson->GetNodeValue("/appTokenId", "");
				m_ouserTokenId = p_pJson->GetNodeValue("/userTokenId", "");

				m_strmessage = p_pJson->GetNodeValue("/message", "");

				m_strpath = p_pJson->GetNodeValue("/path", "");
				m_strerror = p_pJson->GetNodeValue("/error", "");
				m_strstatus = p_pJson->GetNodeValue("/status", "");
				m_strtimestamp = p_pJson->GetNodeValue("/timestamp", "");

				return true;
			}
		public:
			CHeaderEx m_oHeader;
			std::string m_oappTokenId;
			std::string m_ouserTokenId;
			std::string m_strmessage;   
			std::string m_strpath;
			std::string m_strtimestamp;
			std::string m_strstatus; 
			std::string m_strerror;
		};

		class CGetappTokenRequest :
			public IRequest
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				p_pJson->SetNodeValue("/appToken", m_oappToken);
				p_pJson->SetNodeValue("/expireAt", m_oexpireAt);
				return p_pJson->ToString();
			}

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oappToken = p_pJson->GetNodeValue("/appToken", "");
				m_oexpireAt = p_pJson->GetNodeValue("/expireAt", "");

				return true;
			}
		public:
			CHeaderEx m_oHeader;
			std::string m_oappToken;
			std::string m_oexpireAt;
		};

		class CGetuserTokenRequest :
			public IRequest
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				p_pJson->SetNodeValue("/userToken", m_ouserToken);
				p_pJson->SetNodeValue("/expireAt", m_oexpireAt);
				return p_pJson->ToString();
			}

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_ouserToken = p_pJson->GetNodeValue("/userToken", "");
				m_oexpireAt = p_pJson->GetNodeValue("/expireAt", "");

				return true;
			}
		public:
			CHeaderEx m_oHeader;
			std::string m_ouserToken;
			std::string m_oexpireAt;
		};

		class CPushIdInfoRequest :
			public IRequest
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				return p_pJson->ToString();
			}

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_strcode = p_pJson->GetNodeValue("/code", "");
				m_strmsg = p_pJson->GetNodeValue("/msg", "");
				
				size_t tmp_uiDataSize = p_pJson->GetCount("/data");

				if (0 == m_strcode.compare("200"))
				{
					for (size_t i = 0; i < tmp_uiDataSize; ++i)
					{
						std::string tmp_strDataPrefix("/data/" + std::to_string(i));
						m_odata.m_strmobile = p_pJson->GetNodeValue(tmp_strDataPrefix + "/mobile", "");
						m_odata.m_strdomplace = p_pJson->GetNodeValue(tmp_strDataPrefix + "/domplace", "");
						m_odata.m_strlevel = p_pJson->GetNodeValue(tmp_strDataPrefix + "/level", "");
						m_odata.m_stridno = p_pJson->GetNodeValue(tmp_strDataPrefix + "/idno", "");
						m_odata.m_strchname = p_pJson->GetNodeValue(tmp_strDataPrefix + "/chname", "");
						m_odata.m_strsex = p_pJson->GetNodeValue(tmp_strDataPrefix + "/sex", "");
						m_odata.m_strbirthday = p_pJson->GetNodeValue(tmp_strDataPrefix + "/birthday", "");
						m_odata.m_strage = p_pJson->GetNodeValue(tmp_strDataPrefix + "/age", "");
						m_odata.m_strhplace = p_pJson->GetNodeValue(tmp_strDataPrefix + "/hplace", "");

						m_vecData.push_back(m_odata);
					}
				}
				else
				{
					m_strstatus = p_pJson->GetNodeValue("/status", "");
					m_strmessageSequence = p_pJson->GetNodeValue("/messageSequence", "");
					m_odata.m_strtotalRows = p_pJson->GetNodeValue("/data/totalRows", "");
					m_odata.m_strresults = p_pJson->GetNodeValue("/data/results", "");
					m_strqtime = p_pJson->GetNodeValue("/qtime", "");
				}
				
				return true;
			}
		public:
			CHeaderEx m_oHeader;
			std::string m_strcode;
			std::string m_strmsg;
			std::string m_strstatus;
			std::string m_strmessageSequence;
			std::string m_strqtime;

			struct Data
			{
			public:
				std::string m_strmobile;
				std::string m_strdomplace;  //报警人号码
				std::string m_strlevel;
				std::string m_stridno;
				std::string m_strchname;
				std::string m_strsex;
				std::string m_strbirthday;
				std::string m_strage;
				std::string m_strhplace;
				std::string m_strtotalRows;
				std::string m_strresults;
			};
			Data m_odata;
			std::vector<Data> m_vecData;
		};
		//四川省公民身份与案件关系查询
		class CPostQueryNameCaseDataRequest :
			public IRequest
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				return p_pJson->ToString();
			}

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_strstatus = p_pJson->GetNodeValue("/status", "");
				m_strmsg = p_pJson->GetNodeValue("/msg", "");
				m_strmessageSequence = p_pJson->GetNodeValue("/messageSequence", "");
				m_Data.m_strtotalRows = p_pJson->GetNodeValue("/data/totalRows", "");

				if (0 == m_strstatus.compare("200"))
				{
					for (int i = 0; i < std::atoi(m_Data.m_strtotalRows.c_str()); ++i)
					{
						std::string tmp_strDataPrefix("/data/results/" + std::to_string(i));

						m_Data.results.m_strcacsName = p_pJson->GetNodeValue(tmp_strDataPrefix + "/CASE_NAME", "");
						m_Data.results.m_strcaseTime = p_pJson->GetNodeValue(tmp_strDataPrefix + "/CASE_TIME", "");
						m_Data.results.m_strcertNum = p_pJson->GetNodeValue(tmp_strDataPrefix + "/CERT_NUM", "");
						m_Data.results.m_strcaseNo = p_pJson->GetNodeValue(tmp_strDataPrefix + "/CASE_NO", "");
						m_Data.results.m_strname = p_pJson->GetNodeValue(tmp_strDataPrefix + "/NAME", "");
						m_Data.m_vecResults.push_back(m_Data.results);
					}
				}
				else
				{
					m_strqtime = p_pJson->GetNodeValue("/qtime", "");
				}
				return true;
			}
		public:
			CHeaderEx m_oHeader;
			std::string m_strmsg;
			std::string m_strstatus;
			std::string m_strmessageSequence;
			std::string m_strqtime;

			struct Results
			{
				std::string m_strcacsName;      //案件名称
				std::string m_strcaseTime;      //立案时间
				std::string m_strcertNum;       //证件号码
				std::string m_strcaseNo;        //案件编号
				std::string m_strname;          //姓名
			};

			class Data
			{
			public:
				std::string m_strtotalRows;
				std::vector<Results> m_vecResults;
				Results results;
			};

			Data m_Data;
		};

		//四川省交通违法记录表查询服务
		class CPostQueryVehIllegalInfoRequest :
			public IRequest
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				return p_pJson->ToString();
			}

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_strstatus = p_pJson->GetNodeValue("/status", "");
				m_strmsg = p_pJson->GetNodeValue("/msg", "");
				m_strmessageSequence = p_pJson->GetNodeValue("/messageSequence", "");
				m_Data.m_strtotalRows = p_pJson->GetNodeValue("/data/totalRows", "");

				if (0 == m_strstatus.compare("200"))
				{
					for (int i = 0; i < std::atoi(m_Data.m_strtotalRows.c_str()); ++i)
					{
						std::string tmp_strDataPrefix("/data/results/" + std::to_string(i));

						m_Data.results.m_strtrafIllrecTcode = p_pJson->GetNodeValue(tmp_strDataPrefix + "/TRAF_ILLREC_TCODE", "");
						m_Data.results.m_strprocTime = p_pJson->GetNodeValue(tmp_strDataPrefix + "/PROC_TIME", "");
						m_Data.results.m_strvehLicTname = p_pJson->GetNodeValue(tmp_strDataPrefix + "/VEH_LIC_TNAME", "");
						m_Data.results.m_strilleManHandRcode = p_pJson->GetNodeValue(tmp_strDataPrefix + "/ILLE_MAN_HAND_RCODE", "");
						m_Data.results.m_strilleTime = p_pJson->GetNodeValue(tmp_strDataPrefix + "/ILLE_TIME", "");

						m_Data.results.m_strtrafIllrecTname = p_pJson->GetNodeValue(tmp_strDataPrefix + "/TRAF_ILLREC_TNAME", "");
						m_Data.results.m_strIllePlacAddrName = p_pJson->GetNodeValue(tmp_strDataPrefix + "/ILLE_PLAC_ADDR_NAME", "");
						m_Data.results.m_strvehPlateNum = p_pJson->GetNodeValue(tmp_strDataPrefix + "/VEH_PLATE_NUM", "");

						m_Data.m_vecResults.push_back(m_Data.results);
					}
				}
				else
				{
					m_strqtime = p_pJson->GetNodeValue("/qtime", "");
				}
				return true;
			}
		public:
			CHeaderEx m_oHeader;
			std::string m_strmsg;
			std::string m_strstatus;
			std::string m_strmessageSequence;
			std::string m_strqtime;

			struct Results
			{
				std::string m_strtrafIllrecTcode;      //记录类型代码
				std::string m_strtrafIllrecTname;      //记录类型
				std::string m_strIllePlacAddrName;     //违法地址
				std::string m_strvehPlateNum;          //号牌号码
				std::string m_strvehLicTname;          //号牌种类
				std::string m_strilleTime;             //违法时间
				std::string m_strprocTime;             //处理时间
				std::string m_strilleManHandRcode;     //违法行为人处理结果

			};

			class Data
			{
			public:
				std::string m_strtotalRows;
				std::vector<Results> m_vecResults;
				Results results;
			};

			Data m_Data;
		};
		//案件信息查询
		class CPostQueryCaseInfoRequest :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				return p_pJson->ToString();
			}
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return false;
				}
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.code = p_pJson->GetNodeValue("/status", "");
				m_oBody.message = p_pJson->GetNodeValue("/msg", "");
				m_oBody.m_Data.m_strCount = p_pJson->GetNodeValue("/data/totalRows", "");
				if (0 == m_oBody.code.compare("200")) {
					int count = std::atoi(m_oBody.m_Data.m_strCount.c_str());
					for (int i = 0; i < count; i++)
					{
						CData::CList Data;
						std::string tmp_path("/data/results/" + std::to_string(i));
						Data.m_accepTime = p_pJson->GetNodeValue(tmp_path + "/ACCEP_TIME", "");
						Data.m_accepUnitPsag = p_pJson->GetNodeValue(tmp_path + "/ACCEP_UNIT_PSAG", "");
						Data.m_arrsceTime = p_pJson->GetNodeValue(tmp_path + "/ARRSCE_TIME", "");
						Data.m_casClaName = p_pJson->GetNodeValue(tmp_path + "/CASCLA_NAME", "");
						Data.m_caseAddrAddrCode = p_pJson->GetNodeValue(tmp_path + "/CASE_ADDR_ADDR_CODE", "");
						Data.m_caseClasCode = p_pJson->GetNodeValue(tmp_path + "/CASE_CLASS_CODE", "");
						Data.m_caseDate = p_pJson->GetNodeValue(tmp_path + "/CASE_DATE", "");
						Data.m_caseName = p_pJson->GetNodeValue(tmp_path + "/CASE_NAME", "");
						Data.m_caseNo = p_pJson->GetNodeValue(tmp_path + "/CASE_NO", "");
						Data.m_caseOrgaPsag = p_pJson->GetNodeValue(tmp_path + "/CASE_ORGA_PSAG", "");
						Data.m_caseOrgaPsagCode = p_pJson->GetNodeValue(tmp_path + "/CASE_ORGA_PSAG_CODE", "");
						Data.m_caseSourDesc = p_pJson->GetNodeValue(tmp_path + "/CASE_SOUR_DESC", "");
						Data.m_caseTimeTimper = p_pJson->GetNodeValue(tmp_path + "/CASE_TIME_TIMPER", "");
						Data.m_caseTypeCode = p_pJson->GetNodeValue(tmp_path + "/CASE_TYPE_CODE", "");
						Data.m_crimPurpDesc = p_pJson->GetNodeValue(tmp_path + "/CRIM_PURP_DESC", "");
						Data.m_discTime = p_pJson->GetNodeValue(tmp_path + "/DISC_TIME", "");
						Data.m_mainCaseCaseNo = p_pJson->GetNodeValue(tmp_path + "/MAIN_CASE_CASE_NO", "");
						Data.m_minorCaseCaseNo = p_pJson->GetNodeValue(tmp_path + "/MINOR_CASE_CASE_NO", "");
						Data.m_repoCaseTime = p_pJson->GetNodeValue(tmp_path + "/REPO_CASE_TIME", "");
						Data.m_repoCaseUnitAddrName = p_pJson->GetNodeValue(tmp_path + "/REPO_CASE_UNIT_ADDR_NAME", "");
						Data.m_setLawsDate = p_pJson->GetNodeValue(tmp_path + "/SET_LAWS_DATE", "");
						m_oBody.m_Data.m_vecList.push_back(Data);
					}
				}
				return true;
			}

		public:
			CHeaderEx m_oHeader;

			class CData
			{
			public:
				class CList {
				public:
					std::string m_caseClasCode;				//案件类型
					std::string m_repoCaseTime;				//报案时间
					std::string m_caseName;					//案件名称
					std::string m_mainCaseCaseNo;			//从案案件编号
					std::string m_casClaName;				//案件类别
					std::string m_repoCaseUnitAddrName;		//报案单位地址名称
					std::string m_accepUnitPsag;			//受理单位
					std::string m_discTime;					//发现案事件时间
					std::string m_minorCaseCaseNo;			//从案案件编号
					std::string m_caseClassCode;			//案件类型代码
					std::string m_caseNo;					//案件编号
					std::string m_caseOrgaPsag;				//办案单位
					std::string m_caseDate;					//立案日期
					std::string m_accepTime;				//受理时间
					std::string m_caseTimeTimper;			//案发时间段
					std::string m_caseTypeCode;				//案件类别代码
					std::string m_crimPurpDesc;				//作案目的描述
					std::string m_caseSourDesc;				//案件来源
					std::string m_caseAddrAddrCode;			//案发地地址编码
					std::string m_caseOrgaPsagCode;			//办案单位代码
					std::string m_arrsceTime;				//到达现场时间
					std::string m_setLawsDate;				//结案日期
				};
			public:
				std::string m_strCount;
				std::vector<CList> m_vecList;
			};

			class CBody
			{
			public:
				std::string code;
				std::string message;
				CData m_Data;
			};
			CBody m_oBody;
		};

		//4.10	四川省公民身份与户籍关系查询服务
		class CPostQueryCertInfoRequest :
			public IRequest
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				return p_pJson->ToString();
			}

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_strstatus = p_pJson->GetNodeValue("/status", "");
				m_strmsg = p_pJson->GetNodeValue("/msg", "");
				m_strmessageSequence = p_pJson->GetNodeValue("/messageSequence", "");
				m_Data.m_strtotalRows = p_pJson->GetNodeValue("/data/totalRows", "");

				if (0 == m_strstatus.compare("200"))
				{
					for (int i = 0; i < std::atoi(m_Data.m_strtotalRows.c_str()); ++i)
					{
						std::string tmp_strDataPrefix("/data/results/" + std::to_string(i));

						m_Data.results.m_relType = p_pJson->GetNodeValue(tmp_strDataPrefix + "/REL_TYPE", "");
						m_Data.results.m_houHeadRel = p_pJson->GetNodeValue(tmp_strDataPrefix + "/HOU_HEAD_REL", "");
						m_Data.results.m_domicAdmDiv = p_pJson->GetNodeValue(tmp_strDataPrefix + "/DOMIC_ADM_DIV", "");
						m_Data.results.m_predict = p_pJson->GetNodeValue(tmp_strDataPrefix + "/PREDICT", "");
						m_Data.results.m_domicNum = p_pJson->GetNodeValue(tmp_strDataPrefix + "/DOMIC_NUM", "");
						m_Data.results.m_domicAddr = p_pJson->GetNodeValue(tmp_strDataPrefix + "/DOMIC_ADDR", "");
						m_Data.results.m_certNum = p_pJson->GetNodeValue(tmp_strDataPrefix + "/CERT_NUM", "");
						m_Data.results.m_lastTime = p_pJson->GetNodeValue(tmp_strDataPrefix + "/LAST_TIME", "");
						m_Data.m_vecResults.push_back(m_Data.results);
					}
				}
				else
				{
					m_strqtime = p_pJson->GetNodeValue("/qtime", "");
				}
				return true;
			}
		public:
			CHeaderEx m_oHeader;
			std::string m_strmsg;
			std::string m_strstatus;
			std::string m_strmessageSequence;
			std::string m_strqtime;

			struct Results
			{
				std::string m_predict;			//可信度
				std::string m_relType;			//最新关系类型
				std::string m_certNum;			//证件号码
				std::string m_domicNum;			//户号
				std::string m_houHeadRel;		//与户主关系
				std::string m_domicAddr;		//户籍地址
				std::string m_domicAdmDiv;		//所属行政区划
				std::string m_lastTime;         //修改时间
			};
			class Data
			{
			public:
				std::string m_strtotalRows;
				std::vector<Results> m_vecResults;
				Results results;
			};

			Data m_Data;
		};
		//车牌与案件接口
		class CPostQueryCaseVehInfoRequest :
			public IRequest
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				return p_pJson->ToString();
			}
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return false;
				}
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_strCode = p_pJson->GetNodeValue("/status", "");
				m_strMessage = p_pJson->GetNodeValue("/msg", "");
				m_strData.m_strCount = p_pJson->GetNodeValue("/data/totalRows", "");
				if (0 == m_strCode.compare("200")) {
					int count = std::atoi(m_strData.m_strCount.c_str());
					for (int i = 0; i < count; i++)
					{
						CData::CList data;
						std::string tmp_path("/data/results/" + std::to_string(i));
						data.m_strDisPlace = p_pJson->GetNodeValue(tmp_path + "/DIS_PLACE", "");
						data.m_strCaseNo = p_pJson->GetNodeValue(tmp_path + "/CASE_NO", "");
						data.m_strVehPlateNum = p_pJson->GetNodeValue(tmp_path + "/VEH_PLATE_NUM", "");
						data.m_strVehLicTcode = p_pJson->GetNodeValue(tmp_path + "/VEH_LIC_TCODE", "");
						m_strData.m_vecLists.push_back(data);
					}
				}
				else {
					m_strMessage = p_pJson->GetNodeValue("/msg", "params error[ need SenderID and GroupID, but not found in header]");
					m_strData.m_strCount = p_pJson->GetNodeValue("/data/totalRows", "0");
				}
				return true;
			}
		public:
			class CData
			{
			public:
				class CList {
				public:
					std::string m_strDisPlace;			    // 发现地点
					std::string m_strCaseNo;				// 案件编号
					std::string m_strVehPlateNum;			// 车牌号
					std::string m_strVehLicTcode;           // 机动车号牌种类
				};
			public:
				std::string m_strCount;					   // 总数
				std::vector<CList> m_vecLists;			   // 结果
			};
		public:
			CHeader m_oHeader;
			std::string m_strCode;						  // 200 为成功
			std::string m_strMessage;					  // success
			CData m_strData;							  // 数据
		};

		// 全国人口照片查询服务
		class CPersonPhotoInfoRespond :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				p_pJson->SetNodeValue("/code", m_strCode);
				p_pJson->SetNodeValue("/message", m_strMessage);
				p_pJson->SetNodeValue("/data/Count", m_strData.m_strCount);
				int iCount = m_strData.m_vecLists.size();
				for (int i = 0; i < iCount; ++i)
				{
					std::string l_strPrefixPath("/data/List/" + std::to_string(i));
					p_pJson->SetNodeValue(l_strPrefixPath + "/Name", m_strData.m_vecLists.at(i).m_strName);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Photo", m_strData.m_vecLists.at(i).m_strPhoto);
					p_pJson->SetNodeValue(l_strPrefixPath + "/IdCard", m_strData.m_vecLists.at(i).m_strIdCard);
				}
				return p_pJson->ToString();
			}
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return false;
				}
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				CData::CList data;
				data.m_strName = p_pJson->GetNodeValue("/XM", "");
				data.m_strPhoto = p_pJson->GetNodeValue("/ZP", "");
				data.m_strIdCard = p_pJson->GetNodeValue("/SFZH", "");
				if (!data.m_strPhoto.empty()) {
					m_strCode = p_pJson->GetNodeValue("/status", "200");
					m_strMessage = p_pJson->GetNodeValue("/msg", "success");
					m_strData.m_strCount = p_pJson->GetNodeValue("/data/totalRows", "1");
				}
				else {
					m_strCode = p_pJson->GetNodeValue("/status", "");
					m_strMessage = p_pJson->GetNodeValue("/msg", "");
					m_strData.m_strCount = p_pJson->GetNodeValue("/data/totalRows", "");
				}
				m_strData.m_vecLists.push_back(data);
				return true;
			}
		public:
			class CData
			{
			public:
				class CList {
				public:
					std::string m_strName;                    // 姓名    (XM)
					std::string m_strPhoto;                   // 照片    (zp)
					std::string m_strIdCard;                  // 身份证号    (sfzh)
				};
			public:
				std::string m_strCount;					// 总数
				std::vector<CList> m_vecLists;			// 结果
			};
		public:
			CHeader m_oHeader;
			std::string m_strCode;						// 200 为成功
			std::string m_strMessage;					// success
			CData m_strData;							// 数据
		};
		//4.9	四川省机动车基本信息查询服务
		class CPostQueryCredInfoRequest :
			public IRequest
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				return p_pJson->ToString();
			}

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_strstatus = p_pJson->GetNodeValue("/status", "");
				m_strmsg = p_pJson->GetNodeValue("/msg", "");
				m_strmessageSequence = p_pJson->GetNodeValue("/messageSequence", "");
				m_Data.m_strtotalRows = p_pJson->GetNodeValue("/data/totalRows", "");

				if (0 == m_strstatus.compare("200"))
				{
					for (int i = 0; i < std::atoi(m_Data.m_strtotalRows.c_str()); ++i)
					{
						std::string tmp_strDataPrefix("/data/results/" + std::to_string(i));

						m_Data.results.m_vehStatCode = p_pJson->GetNodeValue(tmp_strDataPrefix + "/VEH_STAT_CODE", "");
						m_Data.results.m_mob = p_pJson->GetNodeValue(tmp_strDataPrefix + "/MOB", "");
						m_Data.results.m_vehLicTname = p_pJson->GetNodeValue(tmp_strDataPrefix + "/VEH_LIC_TNAME", "");
						m_Data.results.m_credNum = p_pJson->GetNodeValue(tmp_strDataPrefix + "/CRED_NUM", "");
						m_Data.results.m_inspVeDate = p_pJson->GetNodeValue(tmp_strDataPrefix + "/INS_END_DATE", "");
						m_Data.results.m_vehcBrandModel = p_pJson->GetNodeValue(tmp_strDataPrefix + "/VEHC_BRAND_MODEL", "");
						m_Data.results.m_vehStatName = p_pJson->GetNodeValue(tmp_strDataPrefix + "/VEH_STAT_NAME", "");

						m_Data.results.m_inspVeDate = p_pJson->GetNodeValue(tmp_strDataPrefix + "/INSP_VE_DATE", "");
						m_Data.results.m_vehPlateNum = p_pJson->GetNodeValue(tmp_strDataPrefix + "/VEH_PLATE_NUM", "");
						m_Data.results.m_vehUsagName = p_pJson->GetNodeValue(tmp_strDataPrefix + "/VEH_USAG_NAME", "");
						m_Data.results.m_vehType = p_pJson->GetNodeValue(tmp_strDataPrefix + "/VEH_TYPE", "");
						m_Data.results.m_vehTname = p_pJson->GetNodeValue(tmp_strDataPrefix + "/VEH_TNAME", "");
						m_Data.results.m_firstRegDate = p_pJson->GetNodeValue(tmp_strDataPrefix + "/FIRST_REG_DATE", "");
						m_Data.results.m_engineNo = p_pJson->GetNodeValue(tmp_strDataPrefix + "/ENGINE_NO", "");

						m_Data.results.m_m_vehColorName = p_pJson->GetNodeValue(tmp_strDataPrefix + "/VEH_COLOR_NAME", "");
						m_Data.results.m_regComPsag = p_pJson->GetNodeValue(tmp_strDataPrefix + "/REG_COM_PSAG", "");
						m_Data.m_vecResults.push_back(m_Data.results);
					}
				}
				else
				{
					m_strqtime = p_pJson->GetNodeValue("/qtime", "");
				}
				return true;
			}
		public:
			CHeaderEx m_oHeader;
			std::string m_strmsg;
			std::string m_strstatus;
			std::string m_strmessageSequence;
			std::string m_strqtime;

			struct Results
			{
				std::string m_insEndDate;			//保险终止日期
				std::string m_credNum;				//证件号码
				std::string m_mob;					//手机号码
				std::string m_vehLicTname;			//号牌种类
				std::string m_vehPlateNum;			//号牌号码
				std::string m_vehcBrandModel;		//车辆品牌
				std::string m_vehType;				//车辆型号
				std::string m_m_vehColorName;		//车身颜色
				std::string m_engineNo;				//发动机号
				std::string m_vehUsagName;			//车辆用途
				std::string m_vehStatCode;			//机动车状态代码
				std::string m_vehStatName;			//机动车状态
				std::string m_inspVeDate;			//检验有效期止
				std::string m_firstRegDate;			//初次登记日期
				std::string m_regComPsag;			//登记单位名称
				std::string m_vehTname;				//车辆类型
			};
			class Data
			{
			public:
				std::string m_strtotalRows;
				std::vector<Results> m_vecResults;
				Results results;
			};

			Data m_Data;
		};

		//四川省涉藏群体人员信息查询服务
		class CPostQueryTibetanRelatedGroupsInfoRequest :
			public IRequest
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				return p_pJson->ToString();
			}

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_strstatus = p_pJson->GetNodeValue("/status", "");
				m_strmsg = p_pJson->GetNodeValue("/msg", "");
				m_strmessageSequence = p_pJson->GetNodeValue("/messageSequence", "");
				m_Data.m_strtotalRows = p_pJson->GetNodeValue("/data/totalRows", "");

				if (0 == m_strstatus.compare("200"))
				{
					for (int i = 0; i < std::atoi(m_Data.m_strtotalRows.c_str()); ++i)
					{
						std::string tmp_strDataPrefix("/data/results/" + std::to_string(i));

						m_Data.results.m_strLxdh = p_pJson->GetNodeValue(tmp_strDataPrefix + "/LXDH", "");
						m_Data.results.m_strXm = p_pJson->GetNodeValue(tmp_strDataPrefix + "/XM", "");
						m_Data.results.m_strSacylxdm = p_pJson->GetNodeValue(tmp_strDataPrefix + "/SACYLXDM", "");
						m_Data.results.m_strZjhm = p_pJson->GetNodeValue(tmp_strDataPrefix + "/ZJHM", "");
	
						m_Data.m_vecResults.push_back(m_Data.results);
					}
				}
				else
				{
					m_strqtime = p_pJson->GetNodeValue("/qtime", "");
				}
				return true;
			}
		public:
			CHeaderEx m_oHeader;
			std::string m_strmsg;
			std::string m_strstatus;
			std::string m_strmessageSequence;
			std::string m_strqtime;

			struct Results
			{
				std::string m_strLxdh;         //联系电话
				std::string m_strXm;		   //姓名
				std::string m_strSacylxdm;     //群体类型
				std::string m_strZjhm;         //证件号码
			};

			class Data
			{
			public:
				std::string m_strtotalRows;
				std::vector<Results> m_vecResults;
				Results results;
			};

			Data m_Data;
		};

		//4.13	四川省吸毒人员基本信息查询服务
		class CPostQueryDrugInfoRequest :
			public IRequest
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				return p_pJson->ToString();
			}

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_strstatus = p_pJson->GetNodeValue("/status", "");
				m_strmsg = p_pJson->GetNodeValue("/msg", "");
				m_strmessageSequence = p_pJson->GetNodeValue("/messageSequence", "");
				m_Data.m_strtotalRows = p_pJson->GetNodeValue("/data/totalRows", "");

				if (0 == m_strstatus.compare("200"))
				{
					for (int i = 0; i < std::atoi(m_Data.m_strtotalRows.c_str()); ++i)
					{
						std::string tmp_strDataPrefix("/data/results/" + std::to_string(i));
						m_Data.results.m_strBmch = p_pJson->GetNodeValue(tmp_strDataPrefix + "/BMCH", "");
						m_Data.results.m_strCsrq = p_pJson->GetNodeValue(tmp_strDataPrefix + "/CSRQ", "");
						m_Data.results.m_strHjdzXzqhmc = p_pJson->GetNodeValue(tmp_strDataPrefix + "/HJDZ_XZQHMC", "");
						m_Data.results.m_strSjjzdxzqhmc = p_pJson->GetNodeValue(tmp_strDataPrefix + "/SJJZD_XZQHMC", "");
						m_Data.results.m_strSjjzdgajgmc = p_pJson->GetNodeValue(tmp_strDataPrefix + "/SJJZD_GAJGMC", "");
						m_Data.results.m_strLrsj = p_pJson->GetNodeValue(tmp_strDataPrefix + "/LRSJ", "");
						m_Data.results.m_strSg = p_pJson->GetNodeValue(tmp_strDataPrefix + "/SG", "");
						m_Data.results.m_strHjdzgajgmc = p_pJson->GetNodeValue(tmp_strDataPrefix + "/HJDZ_GAJGMC", "");
						m_Data.results.m_strSjjzddzmc = p_pJson->GetNodeValue(tmp_strDataPrefix + "/SJJZD_DZMC", "");
						m_Data.results.m_strLrdwdwmc = p_pJson->GetNodeValue(tmp_strDataPrefix + "/LRDW_DWMC", "");
						m_Data.results.m_strXm = p_pJson->GetNodeValue(tmp_strDataPrefix + "/XM", "");
						m_Data.results.m_strSfzhm18 = p_pJson->GetNodeValue(tmp_strDataPrefix + "/SFZHM18", "");
						m_Data.results.m_strHjdzdzmc = p_pJson->GetNodeValue(tmp_strDataPrefix + "/HJDZ_DZMC", "");
						m_Data.results.m_strXbdm = p_pJson->GetNodeValue(tmp_strDataPrefix + "/XBDM", "");
						m_Data.results.m_strMzdm = p_pJson->GetNodeValue(tmp_strDataPrefix + "/MZDM", "");

						m_Data.m_vecResults.push_back(m_Data.results);
					}
				}
				else
				{
					m_strqtime = p_pJson->GetNodeValue("/qtime", "");
				}
				return true;
			}
		public:
			CHeaderEx m_oHeader;
			std::string m_strmsg;
			std::string m_strstatus;
			std::string m_strmessageSequence;
			std::string m_strqtime;

			struct Results
			{
				std::string m_strBmch;         //别名/绰号
				std::string m_strCsrq;		   //出生日期
				std::string m_strHjdzXzqhmc;   //户籍地址行政区划名称
				std::string m_strSjjzdxzqhmc;  //实际居住地行政区划
				std::string m_strSjjzdgajgmc;  //实际居住地公安机关
				std::string m_strLrsj;		   //录入时间
				std::string m_strSg;           //身高
				std::string m_strHjdzgajgmc;   //户籍地址公安机关名称
				std::string m_strSjjzddzmc;    //实际居住地详址
				std::string m_strLrdwdwmc;	   //录入单位单位名称
				std::string m_strXm;           //姓名
				std::string m_strSfzhm18;      //身份证号
				std::string m_strHjdzdzmc;     //户籍地址详址
				std::string m_strXbdm;		   //性别
				std::string m_strMzdm;         //民族	
			};

			class Data
			{
			public:
				std::string m_strtotalRows;
				std::vector<Results> m_vecResults;
				Results results;
			};

			Data m_Data;
		};

		// 人员便签
		class CPostQueryPersonTagInfoRespond :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				return p_pJson->ToString();
			}
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return false;
				}
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_strCode = p_pJson->GetNodeValue("/code", "");
				m_strMessage = p_pJson->GetNodeValue("/message", "");
				int iCount = p_pJson->GetCount("/data");
				m_strCount = std::to_string(iCount);
				if (0 == m_strCode.compare("0")) 
				{
					m_strCode = "200";
					for (int i = 0; i < iCount; i++)
					{
						CData data;
						std::string tmp_path("/data/" + std::to_string(i));
						int count2 = p_pJson->GetCount(tmp_path + "/tags");
						data.m_strTagType = p_pJson->GetNodeValue(tmp_path + "/zyylx", "");
						data.m_strTagTypeName = p_pJson->GetNodeValue(tmp_path + "/name", "");

						int size = data.m_vecTags.size();
						for (int j = 0; j < count2; j++)
						{
							CData::CTag tags;
							std::string l_path(tmp_path + "/tags/" + std::to_string(j));
							tags.m_strTagCode = p_pJson->GetNodeValue(l_path + "/bqbsf", "");
							tags.m_strTagName = p_pJson->GetNodeValue(l_path + "/bqzwmc", "");
							data.m_vecTags.push_back(tags);
						}
						m_strData.push_back(data);
					}
				}
				else
				{
					m_strCode = p_pJson->GetNodeValue("/code", "500");
					m_strMessage = p_pJson->GetNodeValue("/msg", "");
					m_strState = p_pJson->GetNodeValue("/state", "");
					m_strErrcode = p_pJson->GetNodeValue("/errcode", "");
				}
				return true;
			}
		public:
			class CData
			{
			public:
				class CTag {
				public:
					std::string m_strTagCode;			    // 便签编码
					std::string m_strTagName;				// 案件编号
				};
			public:
				std::string m_strTagType;					// 便签类型编码
				std::string m_strTagTypeName;			    // 便签类型名称
				std::vector<CTag> m_vecTags;			    // 标签
			};
		public:
			CHeader m_oHeader;
			std::string m_strCode;						   // 200 为成功
			std::string m_strMessage;					   // success
			std::vector<CData> m_strData;				   // 数据
			std::string m_strCount;                        // 数量
			std::string m_strErrcode;
			std::string m_strState;

			std::vector<std::string> m_vecAttentionTag;    // 重点人员接口标签转换为关注人员
			bool m_bTibetanRespond;						   // 涉藏
			bool m_bDrugRespond;						   // 吸毒
			bool m_bEscapeRespond;						   // 在逃
			bool m_bPersonKeyRespond;					   // 重点人员
		};

		//四川省人员主题库信息查询服务
		class CPostQueryPersonInfoRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_strstatus = p_pJson->GetNodeValue("/status", "");
				m_strmsg = p_pJson->GetNodeValue("/msg", "");
				m_strmessageSequence = p_pJson->GetNodeValue("/messageSequence", "");
				m_Data.m_strtotalRows = p_pJson->GetNodeValue("/data/totalRows", "");

				if (0 == m_strstatus.compare("200"))
				{
					for (int i = 0; i < std::atoi(m_Data.m_strtotalRows.c_str()); ++i)
					{
						std::string tmp_strDataPrefix("/data/results/" + std::to_string(i));

						m_Data.results.m_strEscu = p_pJson->GetNodeValue(tmp_strDataPrefix + "/ESCU", "");
						m_Data.results.m_strHomeaddr = p_pJson->GetNodeValue(tmp_strDataPrefix + "/HOMEADDR", "");
						m_Data.results.m_strSex = p_pJson->GetNodeValue(tmp_strDataPrefix + "/SEX", "");
						m_Data.results.m_strEdudegree = p_pJson->GetNodeValue(tmp_strDataPrefix + "/EDUDEGREE", "");
						m_Data.results.m_strDomplace = p_pJson->GetNodeValue(tmp_strDataPrefix + "/DOMPLACE", "");

						m_Data.results.m_strBplace = p_pJson->GetNodeValue(tmp_strDataPrefix + "/BPLACE", "");
						m_Data.results.m_strNation = p_pJson->GetNodeValue(tmp_strDataPrefix + "/NATION", "");
						m_Data.results.m_strServiceplace = p_pJson->GetNodeValue(tmp_strDataPrefix + "/SERVICEPLACE", "");
						m_Data.results.m_strReli = p_pJson->GetNodeValue(tmp_strDataPrefix + "/RELI", "");
						m_Data.results.m_strMarr = p_pJson->GetNodeValue(tmp_strDataPrefix + "/MARR", "");

						m_Data.results.m_strVeh = p_pJson->GetNodeValue(tmp_strDataPrefix + "/VEH", "");
						m_Data.results.m_strHplace = p_pJson->GetNodeValue(tmp_strDataPrefix + "/HPLACE", "");
						m_Data.results.m_strPoli = p_pJson->GetNodeValue(tmp_strDataPrefix + "/POLI", "");
						m_Data.results.m_strProf = p_pJson->GetNodeValue(tmp_strDataPrefix + "/PROF", "");
						m_Data.results.m_strWorkaddr = p_pJson->GetNodeValue(tmp_strDataPrefix + "/WORKADDR", "");

						m_Data.results.m_strIdno = p_pJson->GetNodeValue(tmp_strDataPrefix + "/IDNO", "");
						m_Data.results.m_strBirthday = p_pJson->GetNodeValue(tmp_strDataPrefix + "/BIRTHDAY", "");
						m_Data.results.m_strMobile = p_pJson->GetNodeValue(tmp_strDataPrefix + "/MOBILE", "");
						m_Data.results.m_strChname = p_pJson->GetNodeValue(tmp_strDataPrefix + "/CHNAME", "");
						m_Data.results.m_strHplaceArea = p_pJson->GetNodeValue(tmp_strDataPrefix + "/HPLACEAREA", "");
						m_Data.m_vecResults.push_back(m_Data.results);
					}
				}
				return true;
			}
		public:
			CHeaderEx m_oHeader;
			std::string m_strmsg;
			std::string m_strstatus;
			std::string m_strmessageSequence;

			struct Results
			{
				std::string m_strEscu;               //服役状况
				std::string m_strHomeaddr;           //家庭地址
				std::string m_strSex;                //性别
				std::string m_strEdudegree;          //文化程度
				std::string m_strDomplace;           //居住地址

				std::string m_strBplace;             //出生地址
				std::string m_strNation;             //民族 
				std::string m_strServiceplace;       //服务处所，取最新的学校名称或者工作单位名称。 
				std::string m_strReli;               //宗教信仰
				std::string m_strMarr;               //婚姻状况

				std::string m_strVeh;                //车牌号
				std::string m_strHplace;             //户籍地址
				std::string m_strPoli;               //政治面貌
				std::string m_strProf;               //职业
				std::string m_strWorkaddr;           //工作地址

				std::string m_strIdno;               //证件号码
				std::string m_strBirthday;           //出生日期
				std::string m_strMobile;             //手机号码
				std::string m_strChname;             //中文姓名
				std::string m_strHplaceArea;         //户籍地址的行政区名称 HPLACEAREA
				std::string m_strHplaceAreaCode;     //户籍地址的行政区 code HPLACEAREA
				std::string m_strPhoto;              //图片 
				CPostQueryPersonTagInfoRespond PersonTagInfo;
			};

			class Data
			{
			public:
				std::string m_strtotalRows;
				std::vector<Results> m_vecResults;
				Results results;
			};

			Data m_Data;
		};


		// aia大数据信息
		class CPostQueryAiaPersonInfoRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_strcode = p_pJson->GetNodeValue("/code", "");

				int iCount = p_pJson->GetCount("/data");

				if (0 == m_strcode.compare("200"))
				{
					for (int i = 0; i < iCount; ++i)
					{
						std::string tmp_strDataPrefix("/data/" + std::to_string(i));
						m_Data.results.m_strEscu = p_pJson->GetNodeValue(tmp_strDataPrefix + "/esuc", "");
						m_Data.results.m_strHomeaddr = p_pJson->GetNodeValue(tmp_strDataPrefix + "/homeAddr", "");
						m_Data.results.m_strSex = p_pJson->GetNodeValue(tmp_strDataPrefix + "/sex", "");
						m_Data.results.m_strEdudegree = p_pJson->GetNodeValue(tmp_strDataPrefix + "/eduDegree", "");
						m_Data.results.m_strDomplace = p_pJson->GetNodeValue(tmp_strDataPrefix + "/domPlace", "");
						m_Data.results.m_strBplace = p_pJson->GetNodeValue(tmp_strDataPrefix + "/bplace", "");
						m_Data.results.m_strNation = p_pJson->GetNodeValue(tmp_strDataPrefix + "/nation", "");
						m_Data.results.m_strServiceplace = p_pJson->GetNodeValue(tmp_strDataPrefix + "/servicePlace", "");
						m_Data.results.m_strReli = p_pJson->GetNodeValue(tmp_strDataPrefix + "/reli", "");
						m_Data.results.m_strMarr = p_pJson->GetNodeValue(tmp_strDataPrefix + "/marr", "");

						m_Data.results.m_strVeh = p_pJson->GetNodeValue(tmp_strDataPrefix + "/veh", "");
						m_Data.results.m_strHplace = p_pJson->GetNodeValue(tmp_strDataPrefix + "/hplace", "");
						m_Data.results.m_strHplaceArea = p_pJson->GetNodeValue(tmp_strDataPrefix + "/hplaceArea", "");
						m_Data.results.m_strHplaceAreaCode = p_pJson->GetNodeValue(tmp_strDataPrefix + "/hplaceAreaCode", "");
						m_Data.results.m_strPoli = p_pJson->GetNodeValue(tmp_strDataPrefix + "/poli", "");
						m_Data.results.m_strProf = p_pJson->GetNodeValue(tmp_strDataPrefix + "/prof", "");
						m_Data.results.m_strWorkaddr = p_pJson->GetNodeValue(tmp_strDataPrefix + "/workAddr", "");

						m_Data.results.m_strIdno = p_pJson->GetNodeValue(tmp_strDataPrefix + "/idNo", "");
						m_Data.results.m_strBirthday = p_pJson->GetNodeValue(tmp_strDataPrefix + "/birthday", "");
						m_Data.results.m_strMobile = p_pJson->GetNodeValue(tmp_strDataPrefix + "/mobile", "");
						m_Data.results.m_strChname = p_pJson->GetNodeValue(tmp_strDataPrefix + "/chName", "");
						m_Data.results.m_strPhoto = p_pJson->GetNodeValue(tmp_strDataPrefix + "/photo", "");

						int iCountdataTag = p_pJson->GetCount(tmp_strDataPrefix + "/dataTag");

						for (int j = 0; j < iCountdataTag; ++j)
						{

							CdataTag dataTag;
							std::string l_path(tmp_strDataPrefix + "/dataTag/" + std::to_string(j));
							dataTag.m_strTagType = p_pJson->GetNodeValue(l_path + "/tagType", "");
							dataTag.m_strTagTypeName = p_pJson->GetNodeValue(l_path + "/tagTypeName", "");
							int iCountTag = p_pJson->GetCount(tmp_strDataPrefix + "/tags");
							for (int index = 0; index < iCountTag; ++index)
							{
								CdataTag::CTag tag;
								tag.m_strTagCode = p_pJson->GetNodeValue(l_path + "/tagName", "");
								tag.m_strTagName = p_pJson->GetNodeValue(l_path + "/tagCode", "");
								dataTag.m_vecTags.push_back(tag);
							}

							m_Data.results.m_vecdataTag.push_back(dataTag);
						}
						int iCountattentionTag = p_pJson->GetCount(tmp_strDataPrefix + "/attentionTag");
						for (int i = 0; i < iCountattentionTag; ++i)
						{
							m_Data.results.m_vecAttentionTag.push_back(p_pJson->GetNodeValue(tmp_strDataPrefix + "/", ""));
						}
					
						m_Data.m_vecResults.push_back(m_Data.results);
					}
				}
				return true;
			}
		public:
			CHeaderEx m_oHeader;
			std::string m_strcode;

			class CdataTag
			{
			public:
				class CTag {
				public:
					std::string m_strTagCode;			    // 便签编码
					std::string m_strTagName;				// 案件编号
				};
			public:
				std::string m_strTagType;					// 便签类型编码
				std::string m_strTagTypeName;			    // 便签类型名称
				std::vector<CTag> m_vecTags;			    // 标签
			};


			struct Results
			{
				std::string m_strEscu;                         // 服役状况
				std::string m_strHomeaddr;                     // 家庭地址
				std::string m_strSex;                          // 性别
				std::string m_strEdudegree;                    // 文化程度
				std::string m_strDomplace;                     // 居住地址

				std::string m_strBplace;                       // 出生地址
				std::string m_strNation;                       // 民族 
				std::string m_strServiceplace;                 // 服务处所，取最新的学校名称或者工作单位名称。 
				std::string m_strReli;                         // 宗教信仰
				std::string m_strMarr;                         // 婚姻状况

				std::string m_strVeh;                          // 车牌号
				std::string m_strHplace;                       // 户籍地址
				std::string m_strPoli;                         // 政治面貌
				std::string m_strProf;                         // 职业
				std::string m_strWorkaddr;                     // 工作地址

				std::string m_strIdno;                         // 证件号码
				std::string m_strBirthday;                     // 出生日期
				std::string m_strMobile;                       // 手机号码
				std::string m_strChname;                       // 中文姓名
				std::string m_strHplaceArea;                   // 户籍地址的行政区名称 HPLACEAREA
				std::string m_strHplaceAreaCode;			   // 户籍地址的行政区 code HPLACEAREA
				std::string m_strPhoto;						   // 图片 
				std::vector<std::string> m_vecAttentionTag;    // 重点人员接口标签转换为关注人员
				std::vector<CdataTag>m_vecdataTag;
				//dataTag datatag;
			};

			class Data
			{
			public:
				std::string m_strtotalRows;
				std::vector<Results> m_vecResults;
				Results results;
			};

			Data m_Data;
		};
	}
}