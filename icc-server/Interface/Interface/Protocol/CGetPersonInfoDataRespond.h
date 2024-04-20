/*
cmd 请求
*/
#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>
#include <Protocol/CAuthInfo.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetIdInfoRespond :
			public IRequest
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				p_pJson->SetNodeValue("/code", m_strcode);
				p_pJson->SetNodeValue("/message", m_strmessage);
				int tmp_uiDataSize = m_vecData.size();
				p_pJson->SetNodeValue("/data/Count", std::to_string(tmp_uiDataSize));
				for (int i = 0; i < tmp_uiDataSize; ++i)
				{
					std::string l_strPrefixPath("/data/List/" + std::to_string(i));
					p_pJson->SetNodeValue(l_strPrefixPath + "/Mobile", m_vecData.at(i).m_strmobile);
					p_pJson->SetNodeValue(l_strPrefixPath + "/DomPlace", m_vecData.at(i).m_strdomplace);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Level", m_vecData.at(i).m_strlevel);
					p_pJson->SetNodeValue(l_strPrefixPath + "/IdNo", m_vecData.at(i).m_stridno);
					p_pJson->SetNodeValue(l_strPrefixPath + "/ChName", m_vecData.at(i).m_strchname);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Sex", m_vecData.at(i).m_strsex);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Birthday", m_vecData.at(i).m_strbirthday);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Age", m_vecData.at(i).m_strage);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Hplace", m_vecData.at(i).m_strhplace);	
				}

				return p_pJson->ToString();
			}

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				m_strcode = p_pJson->GetNodeValue("/Code", "");
				m_strmessage = p_pJson->GetNodeValue("/Message", "");

				m_odata.m_strmobile = p_pJson->GetNodeValue("/Data/mobile", "");
				m_odata.m_strdomplace = p_pJson->GetNodeValue("/Data/domplace", "");
				m_odata.m_strlevel = p_pJson->GetNodeValue("/Data/level", "");
				m_odata.m_stridno = p_pJson->GetNodeValue("/Data/idno", "");
				m_odata.m_strchname = p_pJson->GetNodeValue("/Data/chname", "");
				m_odata.m_strsex = p_pJson->GetNodeValue("/Data/sex", "");
				m_odata.m_strbirthday = p_pJson->GetNodeValue("/Data/birthday", "");
				m_odata.m_strage = p_pJson->GetNodeValue("/Data/age", "");
				m_odata.m_strhplace = p_pJson->GetNodeValue("/Data/hplace", "");

				return true;
			}
		public:

			std::string m_strcode;
			std::string m_strmessage;
			std::string m_strstatus;
			std::string m_strmessageSequence;
			std::string m_strqtime;

			class Data
			{
			public:
				std::string m_strmobile;      //手机号码
				std::string m_strdomplace;    //地址
				std::string m_strlevel;       //等级
				std::string m_stridno;        //身份证号
				std::string m_strchname;      //姓名
				std::string m_strsex;         //性别
				std::string m_strbirthday;    //生日
				std::string m_strage;         //年龄
				std::string m_strhplace;      //住址
			};
			Data m_odata;
			std::vector<Data> m_vecData;
		};
		//人员与案件的关系
		class CGetQueryNameCaseDataRespond :
			public IRequest
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				p_pJson->SetNodeValue("/code", m_strcode);
				p_pJson->SetNodeValue("/message", m_strmessage);
				p_pJson->SetNodeValue("/data/Count", m_Data.m_strtotalRows);
				size_t tmp_uiDataSize = std::atoi(m_Data.m_strtotalRows.c_str());
				for (size_t i = 0; i < tmp_uiDataSize; ++i)
				{
					std::string l_strPrefixPath("/data/List/" + std::to_string(i));
					p_pJson->SetNodeValue(l_strPrefixPath + "/CaseName", m_Data.m_vecResults.at(i).m_strcacsName);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Name", m_Data.m_vecResults.at(i).m_strname);
					p_pJson->SetNodeValue(l_strPrefixPath + "/CertNum", m_Data.m_vecResults.at(i).m_strcertNum);
					p_pJson->SetNodeValue(l_strPrefixPath + "/CaseNo", m_Data.m_vecResults.at(i).m_strcaseNo);
					p_pJson->SetNodeValue(l_strPrefixPath + "/CaseTime", m_Data.m_vecResults.at(i).m_strcaseTime);
				}
				return p_pJson->ToString();
			}
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_strcode = p_pJson->GetNodeValue("/status", "");
				m_strmessage = p_pJson->GetNodeValue("/msg", "");
				m_strMessage = p_pJson->GetNodeValue("/messageSequence", "");
				m_Data.m_strtotalRows = p_pJson->GetNodeValue("/data/totalRows", "");

				if (0 == m_strcode.compare("200"))
				{
					int count = std::atoi(m_Data.m_strtotalRows.c_str());
					for (int i = 0; i < count; i++)
					{
						std::string l_strPrefixPath("/data/results/" + std::to_string(i));
						m_Data.results.m_strcacsName = p_pJson->GetNodeValue(l_strPrefixPath + "/CASE_NAME", "");
						m_Data.results.m_strcaseTime = p_pJson->GetNodeValue(l_strPrefixPath + "/CASE_TIME", "");
						m_Data.results.m_strcertNum = p_pJson->GetNodeValue(l_strPrefixPath + "/CERT_NUM", "");
						m_Data.results.m_strcaseNo = p_pJson->GetNodeValue(l_strPrefixPath + "/CASE_NO", "");
						m_Data.results.m_strname = p_pJson->GetNodeValue(l_strPrefixPath + "/NAME", "");
						m_Data.m_vecResults.push_back(m_Data.results);
					}
				}
				else
				{
					m_strqtime = p_pJson->GetNodeValue("/qtime", "");
				}
				return true;
				
				return true;
			}
		public:
			CHeader m_oHeader;
			std::string m_strCode;						// 200 为成功
			std::string m_strMessage;					// success
			std::string m_strqtime;
			std::string m_strcode;
			std::string m_strmessage;
			class Results
			{
			public:
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

		class CGetQueryVehIllegalInfoRespond :
			public IRequest
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				p_pJson->SetNodeValue("/code", m_strcode);
				p_pJson->SetNodeValue("/message", m_strmessage);
				p_pJson->SetNodeValue("/data/Count", m_Data.m_strtotalRows);
				size_t tmp_uiDataSize = m_Data.m_vecResults.size();
				for (size_t i = 0; i < tmp_uiDataSize; ++i)
				{
					std::string l_strPrefixPath("/data/List/" + std::to_string(i));
					p_pJson->SetNodeValue(l_strPrefixPath + "/TrafIllrecTcode", m_Data.m_vecResults.at(i).m_strtrafIllrecTcode);
					p_pJson->SetNodeValue(l_strPrefixPath + "/TrafIllrecTname", m_Data.m_vecResults.at(i).m_strtrafIllrecTname);
					p_pJson->SetNodeValue(l_strPrefixPath + "/IllePlacAddrName", m_Data.m_vecResults.at(i).m_strIllePlacAddrName);
					p_pJson->SetNodeValue(l_strPrefixPath + "/VehPlateNum", m_Data.m_vecResults.at(i).m_strvehPlateNum);
					p_pJson->SetNodeValue(l_strPrefixPath + "/VehLicTname", m_Data.m_vecResults.at(i).m_strvehLicTname);
					p_pJson->SetNodeValue(l_strPrefixPath + "/IlleTime", m_Data.m_vecResults.at(i).m_strilleTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "/ProcTime", m_Data.m_vecResults.at(i).m_strprocTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "/IlleManHandRcode", m_Data.m_vecResults.at(i).m_strilleManHandRcode);

				}
				return p_pJson->ToString();
			}
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				return true;
			}
		public:

			std::string m_strcode;
			std::string m_strmessage;
			class Results
			{
			public:
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
			};
			Data m_Data;
		};

		class CGetQueryCaseVehInfoRespond :
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
					p_pJson->SetNodeValue(l_strPrefixPath + "/DisPlace", m_strData.m_vecLists.at(i).m_strDisPlace);
					p_pJson->SetNodeValue(l_strPrefixPath + "/CaseNo", m_strData.m_vecLists.at(i).m_strCaseNo);
					p_pJson->SetNodeValue(l_strPrefixPath + "/VehPlateNum", m_strData.m_vecLists.at(i).m_strVehPlateNum);
					p_pJson->SetNodeValue(l_strPrefixPath + "/VehLictcode", m_strData.m_vecLists.at(i).m_strVehLicTcode);
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
				std::string m_strCount;					// 总数
				std::vector<CList> m_vecLists;			// 结果
			};
		public:
			CHeader m_oHeader;
			std::string m_strCode;						// 200 为成功
			std::string m_strMessage;					// success
			CData m_strData;							// 数据
		};

		//四川省大数据平台人员标签查询服务
		class CGetQueryPersonTagInfoRespond :
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
				if (0 == m_strCode.compare("200"))
				{
					int iCount = m_strData.size();
					for (int i = 0; i < iCount; ++i)
					{
						std::string l_strIndex = std::to_string(i);
						p_pJson->SetNodeValue("/data/" + l_strIndex + "/TagType", m_strData.at(i).m_strTagType);
						p_pJson->SetNodeValue("/data/" + l_strIndex + "/TagTypeName", m_strData.at(i).m_strTagTypeName);
						int tagCount = m_strData.at(i).m_vecTags.size();
						for (int j = 0; j < tagCount; ++j) {
							std::string strIndex = std::to_string(j);
							p_pJson->SetNodeValue("/data/" + l_strIndex + "/Tags/" + strIndex + "/TagCode", m_strData.at(i).m_vecTags[j].m_strTagCode);
							p_pJson->SetNodeValue("/data/" + l_strIndex + "/Tags/" + strIndex + "/TagName", m_strData.at(i).m_vecTags[j].m_strTagName);
						}
					}
				}
				else
				{
					p_pJson->GetNodeValue("/state", m_strState);
					p_pJson->GetNodeValue("/errcode", m_strErrcode);
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
			std::string m_strCode;						    // 200 为成功
			std::string m_strMessage;					    // success
			std::vector<CData> m_strData;				    // 数据
			std::string m_strCount;                         // 数量
			std::string m_strState;
			std::string m_strErrcode;
		};

			class CAddrInfoRespond :
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
						p_pJson->SetNodeValue(l_strPrefixPath + "/AddrName", m_strData.m_vecLists.at(i).m_strAddrName);
						p_pJson->SetNodeValue(l_strPrefixPath + "/Lat", m_strData.m_vecLists.at(i).m_strLat);
						p_pJson->SetNodeValue(l_strPrefixPath + "/NormaLat", m_strData.m_vecLists.at(i).m_strNormaLat);
						p_pJson->SetNodeValue(l_strPrefixPath + "/Lon", m_strData.m_vecLists.at(i).m_strLon);
						p_pJson->SetNodeValue(l_strPrefixPath + "/InfoDeleJudgeFlag", m_strData.m_vecLists.at(i).m_strInfoDeleJudgeFlag);
						p_pJson->SetNodeValue(l_strPrefixPath + "/NormaLon", m_strData.m_vecLists.at(i).m_strNormaLon);
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
					m_strCode = p_pJson->GetNodeValue("/status", "");
					m_strMessage = p_pJson->GetNodeValue("/msg", "");
					m_strData.m_strCount = p_pJson->GetNodeValue("/data/totalRows", "");
					if (0 == m_strCode.compare("200")) {
						int count = std::atoi(m_strData.m_strCount.c_str());
						for (int i = 0; i < count; i++)
						{
							CData::CList data;
							std::string tmp_path("/data/results/" + std::to_string(i));
							data.m_strAddrName = p_pJson->GetNodeValue(tmp_path + "/ADDR_NAME", "");

							data.m_strInfoDeleJudgeFlag = p_pJson->GetNodeValue(tmp_path + "/INFO_DELE_JUDGE_FLAG", "");
							data.m_strLat = p_pJson->GetNodeValue(tmp_path + "/LAT", "");
							data.m_strNormaLat = p_pJson->GetNodeValue(tmp_path + "/NORMA_LAT", "");
							data.m_strLon = p_pJson->GetNodeValue(tmp_path + "/LON", "");
							data.m_strNormaLon = p_pJson->GetNodeValue(tmp_path + "/NORMA_LON", "");
							m_strData.m_vecLists.push_back(data);
						}
					}
					return true;
				}
			public:
				class CData
				{
				public:
					class CList {
					public:
						std::string m_strAddrName;			    // 地址名称
						std::string m_strLat;				    // 维度
						std::string m_strNormaLat;			    // 归一化地球纬度
						std::string m_strLon;                   // 经度
						std::string m_strInfoDeleJudgeFlag;	    // 是否删除
						std::string m_strNormaLon;              // 归一化地球经度
						std::string m_strDistance;              // 距离
					};
				public:
					std::string m_strCount;					    // 总数
					std::vector<CList> m_vecLists;			    // 结果
				};
			public:
				CHeader m_oHeader;
				std::string m_strCode;						// 200 为成功
				std::string m_strMessage;					// success
				CData m_strData;							// 数据
			};

			// 实有单位查询
			class CCompInfoRespond :
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
						p_pJson->SetNodeValue(l_strPrefixPath + "/NormaCompAddrLat", m_strData.m_vecLists.at(i).m_strNormaCompAddrLat);
						p_pJson->SetNodeValue(l_strPrefixPath + "/CompAddrLon", m_strData.m_vecLists.at(i).m_strCompAddrLon);
						p_pJson->SetNodeValue(l_strPrefixPath + "/CompName", m_strData.m_vecLists.at(i).m_strCompName);
						p_pJson->SetNodeValue(l_strPrefixPath + "/NormaCompAddrLon", m_strData.m_vecLists.at(i).m_strNormaCompAddrLon);
						p_pJson->SetNodeValue(l_strPrefixPath + "/OwnPolStatPsag", m_strData.m_vecLists.at(i).m_strOwnPolStatPsag);
						p_pJson->SetNodeValue(l_strPrefixPath + "/UnitTname", m_strData.m_vecLists.at(i).m_strUnitTname);
						p_pJson->SetNodeValue(l_strPrefixPath + "/ComAddiDetailAddr", m_strData.m_vecLists.at(i).m_strComAddiDetailAddr);
						p_pJson->SetNodeValue(l_strPrefixPath + "/CompAddrLat", m_strData.m_vecLists.at(i).m_strCompAddrLat);
						p_pJson->SetNodeValue(l_strPrefixPath + "/OwnPolStatPsagCode", m_strData.m_vecLists.at(i).m_strOwnPolStatPsagCode);
						p_pJson->SetNodeValue(l_strPrefixPath + "/UnitTcode", m_strData.m_vecLists.at(i).m_strUnitTcode);
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
					m_strCode = p_pJson->GetNodeValue("/status", "");
					m_strMessage = p_pJson->GetNodeValue("/msg", "");
					m_strData.m_strCount = p_pJson->GetNodeValue("/data/totalRows", "");
					if (0 == m_strCode.compare("200")) {
						int count = std::atoi(m_strData.m_strCount.c_str());
						for (int i = 0; i < count; i++)
						{
							CData::CList data;
							std::string tmp_path("/data/results/" + std::to_string(i));
							data.m_strNormaCompAddrLat = p_pJson->GetNodeValue(tmp_path + "/NORMA_COMP_ADDR_LAT", "");
							data.m_strCompAddrLon = p_pJson->GetNodeValue(tmp_path + "/COMP_ADDR_LON", "");
							data.m_strCompName = p_pJson->GetNodeValue(tmp_path + "/COMP_NAME", "");
							data.m_strNormaCompAddrLon = p_pJson->GetNodeValue(tmp_path + "/NORMA_COMP_ADDR_LON", "");
							data.m_strOwnPolStatPsag = p_pJson->GetNodeValue(tmp_path + "/OWN_POL_STAT_PSAG", "");
							data.m_strUnitTname = p_pJson->GetNodeValue(tmp_path + "/UNIT_TNAME", "");
							data.m_strComAddiDetailAddr = p_pJson->GetNodeValue(tmp_path + "/COM_ADDI_DETAIL_ADDR", "");
							data.m_strCompAddrLat = p_pJson->GetNodeValue(tmp_path + "/COMP_ADDR_LAT", "");
							data.m_strOwnPolStatPsagCode = p_pJson->GetNodeValue(tmp_path + "/OWN_POL_STAT_PSAG_CODE", "");
							data.m_strUnitTcode = p_pJson->GetNodeValue(tmp_path + "/UNIT_TCODE", "");
							m_strData.m_vecLists.push_back(data);
						}
					}
					return true;
				}
			public:
				class CData
				{
				public:
					class CList {
					public:
						std::string m_strCompName;			      // 单位名称
						std::string m_strUnitTcode;				  // 单位类别代码
						std::string m_strUnitTname;			      // 单位类别
						std::string m_strComAddiDetailAddr;       // 单位详址
						std::string m_strCompAddrLon;			  // 单位地址经度
						std::string m_strCompAddrLat;             // 单位地址纬度
						std::string m_strOwnPolStatPsagCode;	  // 所属派出所代码
						std::string m_strOwnPolStatPsag;		  // 所属派出所
						std::string m_strNormaCompAddrLon;        // 归一化单位地址地球
						std::string m_strNormaCompAddrLat;		  // 归一化单位地址地球
						std::string m_strDistance;                // 距离
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

		// 实有房屋查询
		class CHousInfoRespond :
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
					p_pJson->SetNodeValue(l_strPrefixPath + "/HousClasCode", m_strData.m_vecLists.at(i).m_strHousClasCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "/HousClasName", m_strData.m_vecLists.at(i).m_strHousClasName);
					p_pJson->SetNodeValue(l_strPrefixPath + "/HousTcode", m_strData.m_vecLists.at(i).m_strHousTcode);
					p_pJson->SetNodeValue(l_strPrefixPath + "/HousTname", m_strData.m_vecLists.at(i).m_strHousTname);
					p_pJson->SetNodeValue(l_strPrefixPath + "/HousOwnerAddrName", m_strData.m_vecLists.at(i).m_strHousOwnerAddrName);
					p_pJson->SetNodeValue(l_strPrefixPath + "/OwnPolStatPsagCode", m_strData.m_vecLists.at(i).m_strOwnPolStatPsagCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "/OwnPolStatPsag", m_strData.m_vecLists.at(i).m_strOwnPolStatPsag);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Lon", m_strData.m_vecLists.at(i).m_strLon);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Lat", m_strData.m_vecLists.at(i).m_strLat);
					p_pJson->SetNodeValue(l_strPrefixPath + "/DoplaNoSernum", m_strData.m_vecLists.at(i).m_strDoplaNoSernum);
					p_pJson->SetNodeValue(l_strPrefixPath + "/HouseNo", m_strData.m_vecLists.at(i).m_strHouseNo);
					p_pJson->SetNodeValue(l_strPrefixPath + "/CancJudgeFlag", m_strData.m_vecLists.at(i).m_strCancJudgeFlag);
					p_pJson->SetNodeValue(l_strPrefixPath + "/NormaLon", m_strData.m_vecLists.at(i).m_strNormaLon);
					p_pJson->SetNodeValue(l_strPrefixPath + "/NormaLat", m_strData.m_vecLists.at(i).m_strNormaLat);
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
				m_strCode = p_pJson->GetNodeValue("/status", "");
				m_strMessage = p_pJson->GetNodeValue("/msg", "");
				m_strData.m_strCount = p_pJson->GetNodeValue("/data/totalRows", "");
				if (0 == m_strCode.compare("200")) {
					int count = std::atoi(m_strData.m_strCount.c_str());
					for (int i = 0; i < count; i++)
					{
						CData::CList data;
						std::string tmp_path("/data/results/" + std::to_string(i));
						data.m_strOwnPolStatPsag = p_pJson->GetNodeValue(tmp_path + "/OWN_POL_STAT_PSAG", "");
						data.m_strDoplaNoSernum = p_pJson->GetNodeValue(tmp_path + "/DOPLA_NO_SERNUM", "");
						data.m_strHousTname = p_pJson->GetNodeValue(tmp_path + "/HOUS_TNAME", "");
						data.m_strLon = p_pJson->GetNodeValue(tmp_path + "/LON", "");
						data.m_strNormaLon = p_pJson->GetNodeValue(tmp_path + "/NORMA_LON", "");
						data.m_strHousTcode = p_pJson->GetNodeValue(tmp_path + "/HOUS_TCODE", "");
						data.m_strOwnPolStatPsagCode = p_pJson->GetNodeValue(tmp_path + "/OWN_POL_STAT_PSAG_CODE", "");
						data.m_strOwnPolStatPsagCode = p_pJson->GetNodeValue(tmp_path + "/HOUS_CLAS_CODE", "");
						data.m_strHousOwnerAddrName = p_pJson->GetNodeValue(tmp_path + "/HOUS_OWNER_ADDR_NAME", "");
						data.m_strCancJudgeFlag = p_pJson->GetNodeValue(tmp_path + "/CANC_JUDGE_FLAG", "");
						data.m_strHousClasName = p_pJson->GetNodeValue(tmp_path + "/HOUS_CLAS_NAME", "");
						data.m_strLat = p_pJson->GetNodeValue(tmp_path + "/LAT", "");
						data.m_strNormaLat = p_pJson->GetNodeValue(tmp_path + "/NORMA_LAT", "");
						data.m_strHouseNo = p_pJson->GetNodeValue(tmp_path + "/HOUSE_NO", "");
						m_strData.m_vecLists.push_back(data);
					}
				}
				return true;
			}
		public:
			class CData
			{
			public:
				class CList {
				public:
					std::string m_strHousClasCode;					// 房屋类型代码
					std::string m_strHousClasName;		// 房屋类型
					std::string m_strHousTcode;		// 房屋类别代码
					std::string m_strHousTname;		// 房屋类别
					std::string m_strHousOwnerAddrName;		// 房主地址
					std::string m_strOwnPolStatPsagCode;		// 派出所机关代码
					std::string m_strOwnPolStatPsag;		// 派出所机关名称
					std::string m_strLon;		// 经度
					std::string m_strLat;		// 纬度
					std::string m_strDoplaNoSernum;		// 门牌号序列号
					std::string m_strHouseNo;		// 房屋编号
					std::string m_strCancJudgeFlag;		// 是否注销
					std::string m_strNormaLon;		// 归一化地球经度
					std::string m_strNormaLat;		// 归一化地球纬度
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
	
		//案件信息查询
		class CQueryCaseInfoResponse :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				p_pJson->SetNodeValue("/code", m_oBody.code);
				p_pJson->SetNodeValue("/message", m_oBody.message);
				p_pJson->SetNodeValue("/data/Count", m_oBody.m_Data.m_strCount);
				int iCount = m_oBody.m_Data.m_vecList.size();
				for (int i = 0; i < iCount; i++)
				{
					std::string l_strPrefixPath("/data/List/" + std::to_string(i) + "/");
					p_pJson->SetNodeValue(l_strPrefixPath + "CaseClasCode", m_oBody.m_Data.m_vecList.at(i).m_caseClasCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "RepoCaseTime", m_oBody.m_Data.m_vecList.at(i).m_repoCaseTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "CaseName", m_oBody.m_Data.m_vecList.at(i).m_caseName);
					p_pJson->SetNodeValue(l_strPrefixPath + "MainCaseCaseNo", m_oBody.m_Data.m_vecList.at(i).m_mainCaseCaseNo);
					p_pJson->SetNodeValue(l_strPrefixPath + "CasclaName", m_oBody.m_Data.m_vecList.at(i).m_casClaName);
					p_pJson->SetNodeValue(l_strPrefixPath + "RepoCaseUnitAddrName", m_oBody.m_Data.m_vecList.at(i).m_repoCaseUnitAddrName);
					p_pJson->SetNodeValue(l_strPrefixPath + "AccepUnitPsag", m_oBody.m_Data.m_vecList.at(i).m_accepUnitPsag);
					p_pJson->SetNodeValue(l_strPrefixPath + "DiscTime", m_oBody.m_Data.m_vecList.at(i).m_discTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "MinorCaseCaseNo", m_oBody.m_Data.m_vecList.at(i).m_minorCaseCaseNo);
					p_pJson->SetNodeValue(l_strPrefixPath + "CaseClassCode", m_oBody.m_Data.m_vecList.at(i).m_caseClassCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "CaseNo", m_oBody.m_Data.m_vecList.at(i).m_caseNo);
					p_pJson->SetNodeValue(l_strPrefixPath + "CaseOrgaPsag", m_oBody.m_Data.m_vecList.at(i).m_caseOrgaPsag);
					p_pJson->SetNodeValue(l_strPrefixPath + "CaseDate", m_oBody.m_Data.m_vecList.at(i).m_caseDate);
					p_pJson->SetNodeValue(l_strPrefixPath + "AccepTime", m_oBody.m_Data.m_vecList.at(i).m_accepTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "CaseTimeTimper", m_oBody.m_Data.m_vecList.at(i).m_caseTimeTimper);
					p_pJson->SetNodeValue(l_strPrefixPath + "CaseTypeCode", m_oBody.m_Data.m_vecList.at(i).m_caseTypeCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "CrimPurpDesc", m_oBody.m_Data.m_vecList.at(i).m_crimPurpDesc);
					p_pJson->SetNodeValue(l_strPrefixPath + "CaseSourDesc", m_oBody.m_Data.m_vecList.at(i).m_caseSourDesc);
					p_pJson->SetNodeValue(l_strPrefixPath + "CaseAddrAddrCode", m_oBody.m_Data.m_vecList.at(i).m_caseAddrAddrCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "CaseOrgaPsagCode", m_oBody.m_Data.m_vecList.at(i).m_caseOrgaPsagCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "ArrsceTime", m_oBody.m_Data.m_vecList.at(i).m_arrsceTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "SetLawsDate", m_oBody.m_Data.m_vecList.at(i).m_setLawsDate);
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
		
		class CQueryCredInfoResponse :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				p_pJson->SetNodeValue("/code", m_oBody.code);
				p_pJson->SetNodeValue("/message", m_oBody.message);
				p_pJson->SetNodeValue("/data/Count", m_oBody.m_strtotalRows);
				for (int i = 0; i < std::atoi(m_oBody.m_strtotalRows.c_str()); i++)
				{
					std::string l_strPrefixPath("/data/List/" + std::to_string(i) + "/");
					p_pJson->SetNodeValue(l_strPrefixPath + "VehStatCode", m_oBody.m_vecData.at(i).m_vehStatCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "Mob", m_oBody.m_vecData.at(i).m_mob);
					p_pJson->SetNodeValue(l_strPrefixPath + "VehLicTname", m_oBody.m_vecData.at(i).m_vehLicTname);
					p_pJson->SetNodeValue(l_strPrefixPath + "CredNum", m_oBody.m_vecData.at(i).m_credNum);
					p_pJson->SetNodeValue(l_strPrefixPath + "InsEndDate", m_oBody.m_vecData.at(i).m_insEndDate);
					p_pJson->SetNodeValue(l_strPrefixPath + "VehcBrandModel", m_oBody.m_vecData.at(i).m_vehcBrandModel);
					p_pJson->SetNodeValue(l_strPrefixPath + "VehStatName", m_oBody.m_vecData.at(i).m_vehStatName);
					p_pJson->SetNodeValue(l_strPrefixPath + "InspVeDate", m_oBody.m_vecData.at(i).m_inspVeDate);
					p_pJson->SetNodeValue(l_strPrefixPath + "VehPlateNum", m_oBody.m_vecData.at(i).m_vehPlateNum);
					p_pJson->SetNodeValue(l_strPrefixPath + "VehUsagName", m_oBody.m_vecData.at(i).m_vehUsagName);
					p_pJson->SetNodeValue(l_strPrefixPath + "VehType", m_oBody.m_vecData.at(i).m_vehType);
					p_pJson->SetNodeValue(l_strPrefixPath + "VehTname", m_oBody.m_vecData.at(i).m_vehTname);
					p_pJson->SetNodeValue(l_strPrefixPath + "FirstRegDate", m_oBody.m_vecData.at(i).m_firstRegDate);
					p_pJson->SetNodeValue(l_strPrefixPath + "EngineNo", m_oBody.m_vecData.at(i).m_engineNo);
					p_pJson->SetNodeValue(l_strPrefixPath + "VehColorName", m_oBody.m_vecData.at(i).m_m_vehColorName);
					p_pJson->SetNodeValue(l_strPrefixPath + "RegComPsag", m_oBody.m_vecData.at(i).m_regComPsag);
					p_pJson->SetNodeValue(l_strPrefixPath + "VehLicTcode", m_oBody.m_vecData.at(i).m_strVehLicTcode);
				}

				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;

			class Results
			{
			public:
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
				std::string m_strVehLicTcode;		//号牌种类代码
			};

			class CBody
			{
			public:
				std::string code;
				std::string message;
				std::vector<Results>m_vecData;
				Results results;
				std::string m_strtotalRows;
			};
			CBody m_oBody;
		};


		class CQueryCertInfoResponse :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				p_pJson->SetNodeValue("/code", m_oBody.code);
				p_pJson->SetNodeValue("/message", m_oBody.message);
				p_pJson->SetNodeValue("/data/Count", m_oBody.m_strtotalRows);
				size_t tmp_uiDataSize = std::atoi(m_oBody.m_strtotalRows.c_str());
				for (unsigned int i = 0; i < m_oBody.m_vecData.size(); i++)
				{
					std::string l_strPrefixPath("/data/List/" + std::to_string(i) + "/");
					p_pJson->SetNodeValue(l_strPrefixPath + "RelType", m_oBody.m_vecData.at(i).m_relType);
					p_pJson->SetNodeValue(l_strPrefixPath + "HouHeadRel", m_oBody.m_vecData.at(i).m_houHeadRel);
					p_pJson->SetNodeValue(l_strPrefixPath + "DomicAdmDiv", m_oBody.m_vecData.at(i).m_domicAdmDiv);
					p_pJson->SetNodeValue(l_strPrefixPath + "Predict", m_oBody.m_vecData.at(i).m_predict);
					p_pJson->SetNodeValue(l_strPrefixPath + "DomicNum", m_oBody.m_vecData.at(i).m_domicNum);
					p_pJson->SetNodeValue(l_strPrefixPath + "DomicAddr", m_oBody.m_vecData.at(i).m_domicAddr);
					p_pJson->SetNodeValue(l_strPrefixPath + "CertNum", m_oBody.m_vecData.at(i).m_certNum);
				}

				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;

			class Results
			{
			public:
				std::string m_predict;			//可信度
				std::string m_relType;			//最新关系类型
				std::string m_certNum;			//证件号码
				std::string m_domicNum;			//户号
				std::string m_houHeadRel;		//与户主关系
				std::string m_domicAddr;		//户籍地址
				std::string m_domicAdmDiv;		//所属行政区划
			};

			class CBody
			{
			public:
				std::string code;
				std::string message;
				std::vector<Results>m_vecData;
				Results results;
				std::string m_strtotalRows;
			};
			CBody m_oBody;
		};


		class CGetQueryPersonInfoRespond :
			public IRequest
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				p_pJson->SetNodeValue("/code", m_strcode);
				p_pJson->SetNodeValue("/message", m_strmessage);
				p_pJson->SetNodeValue("/data/Count", m_Data.m_strtotalRows);
				size_t tmp_uiDataSize = m_Data.m_vecResults.size();
				for (size_t i = 0; i < tmp_uiDataSize; ++i)
				{
					std::string l_strPrefixPath("/data/List/" + std::to_string(i));
					p_pJson->SetNodeValue(l_strPrefixPath + "/Esuc", m_Data.m_vecResults.at(i).m_strEscu);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Homeaddr", m_Data.m_vecResults.at(i).m_strHomeaddr);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Sex", m_Data.m_vecResults.at(i).m_strSex);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Edudegree", m_Data.m_vecResults.at(i).m_strEdudegree);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Domplace", m_Data.m_vecResults.at(i).m_strDomplace);

					p_pJson->SetNodeValue(l_strPrefixPath + "/Bplace", m_Data.m_vecResults.at(i).m_strBplace);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Nation", m_Data.m_vecResults.at(i).m_strNation);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Serviceplace", m_Data.m_vecResults.at(i).m_strServiceplace);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Reli", m_Data.m_vecResults.at(i).m_strReli);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Marr", m_Data.m_vecResults.at(i).m_strMarr);

					p_pJson->SetNodeValue(l_strPrefixPath + "/Veh", m_Data.m_vecResults.at(i).m_strVeh);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Hplace", m_Data.m_vecResults.at(i).m_strHplace);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Poli", m_Data.m_vecResults.at(i).m_strPoli);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Prof", m_Data.m_vecResults.at(i).m_strProf);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Workaddr", m_Data.m_vecResults.at(i).m_strWorkaddr);

					p_pJson->SetNodeValue(l_strPrefixPath + "/Idno", m_Data.m_vecResults.at(i).m_strIdno);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Birthday", m_Data.m_vecResults.at(i).m_strBirthday);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Mobile", m_Data.m_vecResults.at(i).m_strMobile);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Chname", m_Data.m_vecResults.at(i).m_strChname);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Hplacearea", m_Data.m_vecResults.at(i).m_strHplaceArea);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Hplaceareacode", m_Data.m_vecResults.at(i).m_strHplaceAreaCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Photo", m_Data.m_vecResults.at(i).m_strPhoto);

					if (!m_Data.m_vecResults.at(i).m_vecAttentionTag.empty())
					{
						for (size_t index = 0; index < m_Data.m_vecResults.at(i).m_vecAttentionTag.size(); ++index)
						{
							std::string l_strIndex = std::to_string(index);
							p_pJson->SetNodeValue(l_strPrefixPath + "/Attentiontag/" + l_strIndex, m_Data.m_vecResults.at(i).m_vecAttentionTag.at(index));
						}
					}
					else
					{
						p_pJson->SetNodeValue(l_strPrefixPath + "/Attentiontag/0", "");
					}
					
					size_t iCount = m_Data.m_vecResults.at(i).PersonTagInfo.m_strData.size();
					for (size_t index = 0; index < iCount; ++index)
					{
						std::string l_strIndex = std::to_string(index);
						p_pJson->SetNodeValue(l_strPrefixPath + "/DataTag/" + l_strIndex + "/TagType", m_Data.m_vecResults.at(i).PersonTagInfo.m_strData.at(index).m_strTagType);
						p_pJson->SetNodeValue(l_strPrefixPath + "/DataTag/" + l_strIndex + "/TagTypeName", m_Data.m_vecResults.at(i).PersonTagInfo.m_strData.at(index).m_strTagTypeName);
						size_t tagCount = m_Data.m_vecResults.at(i).PersonTagInfo.m_strData.at(index).m_vecTags.size();
						for (size_t j = 0; j < tagCount; ++j) {
							std::string strIndex = std::to_string(j);
							p_pJson->SetNodeValue(l_strPrefixPath + "/DataTag/" + l_strIndex + "/Tags/" + strIndex + "/TagCode", m_Data.m_vecResults.at(i).PersonTagInfo.m_strData.at(index).m_vecTags.at(j).m_strTagCode);
							p_pJson->SetNodeValue(l_strPrefixPath + "/DataTag/" + l_strIndex + "/Tags/" + strIndex + "/TagName", m_Data.m_vecResults.at(i).PersonTagInfo.m_strData.at(index).m_vecTags.at(j).m_strTagName);
						}
					}

				}
				return p_pJson->ToString();
			}

			virtual std::string SyncToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				size_t tmp_uiDataSize = m_Data.m_vecResults.size();
				for (size_t i = 0; i < tmp_uiDataSize; ++i)
				{
					std::string l_strPrefixPath("/body/List/" + std::to_string(i));
					p_pJson->SetNodeValue(l_strPrefixPath + "/Esuc", m_Data.m_vecResults.at(i).m_strEscu);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Homeaddr", m_Data.m_vecResults.at(i).m_strHomeaddr);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Sex", m_Data.m_vecResults.at(i).m_strSex);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Edudegree", m_Data.m_vecResults.at(i).m_strEdudegree);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Domplace", m_Data.m_vecResults.at(i).m_strDomplace);

					p_pJson->SetNodeValue(l_strPrefixPath + "/Bplace", m_Data.m_vecResults.at(i).m_strBplace);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Nation", m_Data.m_vecResults.at(i).m_strNation);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Serviceplace", m_Data.m_vecResults.at(i).m_strServiceplace);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Reli", m_Data.m_vecResults.at(i).m_strReli);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Marr", m_Data.m_vecResults.at(i).m_strMarr);

					p_pJson->SetNodeValue(l_strPrefixPath + "/Veh", m_Data.m_vecResults.at(i).m_strVeh);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Hplace", m_Data.m_vecResults.at(i).m_strHplace);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Poli", m_Data.m_vecResults.at(i).m_strPoli);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Prof", m_Data.m_vecResults.at(i).m_strProf);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Workaddr", m_Data.m_vecResults.at(i).m_strWorkaddr);

					p_pJson->SetNodeValue(l_strPrefixPath + "/Idno", m_Data.m_vecResults.at(i).m_strIdno);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Birthday", m_Data.m_vecResults.at(i).m_strBirthday);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Mobile", m_Data.m_vecResults.at(i).m_strMobile);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Chname", m_Data.m_vecResults.at(i).m_strChname);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Hplacearea", m_Data.m_vecResults.at(i).m_strHplaceArea);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Hplaceareacode", m_Data.m_vecResults.at(i).m_strHplaceAreaCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Photo", m_Data.m_vecResults.at(i).m_strPhoto);

					if (!m_Data.m_vecResults.at(i).m_vecAttentionTag.empty())
					{
						for (size_t index = 0; index < m_Data.m_vecResults.at(i).m_vecAttentionTag.size(); ++index)
						{
							std::string l_strIndex = std::to_string(index);
							p_pJson->SetNodeValue(l_strPrefixPath + "/Attentiontag/" + l_strIndex, m_Data.m_vecResults.at(i).m_vecAttentionTag.at(index));
						}
					}
					else
					{
						p_pJson->SetNodeValue(l_strPrefixPath + "/Attentiontag/0", "");
					}

					size_t iCount = m_Data.m_vecResults.at(i).PersonTagInfo.m_strData.size();
					for (size_t index = 0; index < iCount; ++index)
					{
						std::string l_strIndex = std::to_string(index);
						p_pJson->SetNodeValue(l_strPrefixPath + "/DataTag/" + l_strIndex + "/TagType", m_Data.m_vecResults.at(i).PersonTagInfo.m_strData.at(index).m_strTagType);
						p_pJson->SetNodeValue(l_strPrefixPath + "/DataTag/" + l_strIndex + "/TagTypeName", m_Data.m_vecResults.at(i).PersonTagInfo.m_strData.at(index).m_strTagTypeName);
						size_t tagCount = m_Data.m_vecResults.at(i).PersonTagInfo.m_strData.at(index).m_vecTags.size();
						for (size_t j = 0; j < tagCount; ++j) {
							std::string strIndex = std::to_string(j);
							p_pJson->SetNodeValue(l_strPrefixPath + "/DataTag/" + l_strIndex + "/Tags/" + strIndex + "/TagCode", m_Data.m_vecResults.at(i).PersonTagInfo.m_strData.at(index).m_vecTags.at(j).m_strTagCode);
							p_pJson->SetNodeValue(l_strPrefixPath + "/DataTag/" + l_strIndex + "/Tags/" + strIndex + "/TagName", m_Data.m_vecResults.at(i).PersonTagInfo.m_strData.at(index).m_vecTags.at(j).m_strTagName);
						}
					}

				}
				return p_pJson->ToString();
			}

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				return true;
			}
		public:

			std::string m_strcode;
			std::string m_strmessage;

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
				std::vector<std::string>m_vecAttentionTag;       //关注人员标签 
				CGetQueryPersonTagInfoRespond PersonTagInfo;
			};

			class Data
			{
			public:
				std::string m_strtotalRows;
				std::vector<Results> m_vecResults;
			};
			CHeader m_oHeader;
			Data m_Data;
		};

		//四川省涉藏群体人员信息 
		class CGetQueryTibetanRelatedGroupsInfoRespond :
			public IRequest
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				p_pJson->SetNodeValue("/code", m_strcode);
				p_pJson->SetNodeValue("/message", m_strmessage);
				p_pJson->SetNodeValue("/data/Count", m_Data.m_strtotalRows);
				size_t tmp_uiDataSize = m_Data.m_vecResults.size();
				for (size_t i = 0; i < tmp_uiDataSize; ++i)
				{
					std::string l_strPrefixPath("/data/List/" + std::to_string(i));
					p_pJson->SetNodeValue(l_strPrefixPath + "/Lxdh", m_Data.m_vecResults.at(i).m_strLxdh);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Xm", m_Data.m_vecResults.at(i).m_strXm);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Zjhm", m_Data.m_vecResults.at(i).m_strZjhm);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Sacylxdm", m_Data.m_vecResults.at(i).m_strSacylxdm);
				}
				return p_pJson->ToString();
			}
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				return true;
			}
		public:

			std::string m_strcode;
			std::string m_strmessage;

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
			};
			Data m_Data;
		};

		//四川省吸毒人员基本信息查询服务 
		class CGetQueryDrugInfoRespond :
			public IRequest
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				p_pJson->SetNodeValue("/code", m_strcode);
				p_pJson->SetNodeValue("/message", m_strmessage);
				p_pJson->SetNodeValue("/data/Count", m_Data.m_strtotalRows);
				size_t tmp_uiDataSize = m_Data.m_vecResults.size();
				for (size_t i = 0; i < tmp_uiDataSize; ++i)
				{
					std::string l_strPrefixPath("/data/List/" + std::to_string(i));
					p_pJson->SetNodeValue(l_strPrefixPath + "/Bmch", m_Data.m_vecResults.at(i).m_strBmch);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Csrq", m_Data.m_vecResults.at(i).m_strCsrq);
					p_pJson->SetNodeValue(l_strPrefixPath + "/HjdzXzqhmc", m_Data.m_vecResults.at(i).m_strHjdzXzqhmc);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Sjjzdxzqhmc", m_Data.m_vecResults.at(i).m_strSjjzdxzqhmc);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Sjjzdgajgmc", m_Data.m_vecResults.at(i).m_strSjjzdgajgmc);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Lrsj", m_Data.m_vecResults.at(i).m_strLrsj);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Sg", m_Data.m_vecResults.at(i).m_strSg);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Hjdzgajgm", m_Data.m_vecResults.at(i).m_strHjdzgajgmc);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Sjjzddzmc", m_Data.m_vecResults.at(i).m_strSjjzddzmc);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Lrdwdwmc", m_Data.m_vecResults.at(i).m_strLrdwdwmc);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Xm", m_Data.m_vecResults.at(i).m_strXm);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Sfzhm18", m_Data.m_vecResults.at(i).m_strSfzhm18);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Hjdzdzmc", m_Data.m_vecResults.at(i).m_strHjdzdzmc);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Xbdm", m_Data.m_vecResults.at(i).m_strXbdm);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Mzdm", m_Data.m_vecResults.at(i).m_strMzdm);

				}
				return p_pJson->ToString();
			}
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				return true;
			}
		public:

			std::string m_strcode;
			std::string m_strmessage;

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
			};
			Data m_Data;
		};

		// 全国在逃人员信息查询服务
		class CEscapeInfoRespond :
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
					p_pJson->SetNodeValue(l_strPrefixPath + "/ArrestCode", m_strData.m_vecLists.at(i).m_strArrestCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "/AccentCode", m_strData.m_vecLists.at(i).m_strAccentCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "/CaseInfo", m_strData.m_vecLists.at(i).m_strCaseInfo);
					p_pJson->SetNodeValue(l_strPrefixPath + "/EscapeDate", m_strData.m_vecLists.at(i).m_strEscapeDate);
					p_pJson->SetNodeValue(l_strPrefixPath + "/EscapeNum", m_strData.m_vecLists.at(i).m_strEscapeNum);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Name", m_strData.m_vecLists.at(i).m_strName);
					p_pJson->SetNodeValue(l_strPrefixPath + "/GenderCode", m_strData.m_vecLists.at(i).m_strGenderCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "/AddressArea", m_strData.m_vecLists.at(i).m_strAddressArea);
					p_pJson->SetNodeValue(l_strPrefixPath + "/AddressInfo", m_strData.m_vecLists.at(i).m_strAddressInfo);
					p_pJson->SetNodeValue(l_strPrefixPath + "/CaseNum", m_strData.m_vecLists.at(i).m_strCaseNum);
					p_pJson->SetNodeValue(l_strPrefixPath + "/CaseCode", m_strData.m_vecLists.at(i).m_strCaseCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "/FilingDepartArea", m_strData.m_vecLists.at(i).m_strFilingDepartArea);
					p_pJson->SetNodeValue(l_strPrefixPath + "/FilingDepartCode", m_strData.m_vecLists.at(i).m_strFilingDepartCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "/CerdNum", m_strData.m_vecLists.at(i).m_strCerdNum);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Height", m_strData.m_vecLists.at(i).m_strHeight);
					p_pJson->SetNodeValue(l_strPrefixPath + "/HostContactInfo", m_strData.m_vecLists.at(i).m_strHostContactInfo);
					p_pJson->SetNodeValue(l_strPrefixPath + "/WantedCircular", m_strData.m_vecLists.at(i).m_strWantedCircular);
					p_pJson->SetNodeValue(l_strPrefixPath + "/PermanentAddress", m_strData.m_vecLists.at(i).m_strPermanentAddress);
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
				m_strCode = p_pJson->GetNodeValue("/status", "");
				m_strMessage = p_pJson->GetNodeValue("/msg", "");
				m_strData.m_strCount = p_pJson->GetNodeValue("/data/totalRows", "");
				if (0 == m_strCode.compare("200")) {
					int count = std::atoi(m_strData.m_strCount.c_str());
					for (int i = 0; i < count; i++)
					{
						CData::CList data;
						std::string tmp_path("/data/results/" + std::to_string(i));
						data.m_strFilingDepartCode = p_pJson->GetNodeValue(tmp_path + "/LADW_GAJGJGDM", "");
						data.m_strAddressInfo = p_pJson->GetNodeValue(tmp_path + "/XZD_QHNXXDZ", "");
						data.m_strCaseCode = p_pJson->GetNodeValue(tmp_path + "/AJLBDM", "");
						data.m_strFilingDepartArea = p_pJson->GetNodeValue(tmp_path + "/LADW_XZQHDM", "");
						data.m_strArrestCode = p_pJson->GetNodeValue(tmp_path + "/DBJB_DM", "");
						data.m_strHostContactInfo = p_pJson->GetNodeValue(tmp_path + "/ZBDW_LXDH", "");
						data.m_strAddressArea = p_pJson->GetNodeValue(tmp_path + "/XZD_XZQHDM", "");
						data.m_strCaseNum = p_pJson->GetNodeValue(tmp_path + "/AJBH", "");
						data.m_strPermanentAddress = p_pJson->GetNodeValue(tmp_path + "/HJDZ_XZQHDM", "");
						data.m_strHeight = p_pJson->GetNodeValue(tmp_path + "/SG", "");
						data.m_strName = p_pJson->GetNodeValue(tmp_path + "/XM", "");
						data.m_strGenderCode = p_pJson->GetNodeValue(tmp_path + "/XBDM", "");
						data.m_strCaseInfo = p_pJson->GetNodeValue(tmp_path + "/JYAQ", "");
						data.m_strEscapeNum = p_pJson->GetNodeValue(tmp_path + "/ZTRYBH", "");
						data.m_strAccentCode = p_pJson->GetNodeValue(tmp_path + "/KY2_DM", "");
						data.m_strEscapeDate = p_pJson->GetNodeValue(tmp_path + "/TP2_RQ", "");
						data.m_strWantedCircular = p_pJson->GetNodeValue(tmp_path + "/TJLBH", "");
						data.m_strCerdNum = p_pJson->GetNodeValue(tmp_path + "/ZJHM", "");
						m_strData.m_vecLists.push_back(data);
					}
				}
				return true;
			}
		public:
			class CData
			{
			public:
				class CList {
				public:
					std::string m_strArrestCode;        // 督捕级别代码
					std::string m_strAccentCode;        // 口音_代码
					std::string m_strCaseInfo;        // 简要案情及附加信息
					std::string m_strEscapeDate;        // 逃跑日期
					std::string m_strEscapeNum;        // 在逃人员编号
					std::string m_strName;        // 姓名
					std::string m_strGenderCode;        // 性别代码
					std::string m_strAddressArea;        // 现住地行政区划代码
					std::string m_strAddressInfo;        // 现住地详址
					std::string m_strCaseNum;        // 案件编号
					std::string m_strCaseCode;        // 案件类别代码
					std::string m_strFilingDepartArea;        // 立案单位区划
					std::string m_strFilingDepartCode;        // 立案单位代码
					std::string m_strCerdNum;        // 证件号码
					std::string m_strHeight;        // 身高
					std::string m_strHostContactInfo;        // 主办联系方式
					std::string m_strWantedCircular;        // 通缉令编号
					std::string m_strPermanentAddress;        // 户籍地区划
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

		// 全国情报重点人员信息查询服务
		class CPersonKeyInfoRespond :
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
					p_pJson->SetNodeValue(l_strPrefixPath + "/PoliceStationCode", m_strData.m_vecLists.at(i).m_strPoliceStationCode);                
					p_pJson->SetNodeValue(l_strPrefixPath + "/Jurisdiction", m_strData.m_vecLists.at(i).m_strJurisdiction);             
					p_pJson->SetNodeValue(l_strPrefixPath + "/JurisdictionCode", m_strData.m_vecLists.at(i).m_strJurisdictionCode);          
					p_pJson->SetNodeValue(l_strPrefixPath + "/FilingDepart", m_strData.m_vecLists.at(i).m_strFilingDepart);            
					p_pJson->SetNodeValue(l_strPrefixPath + "/FilingDepartCode", m_strData.m_vecLists.at(i).m_strFilingDepartCode);             
					p_pJson->SetNodeValue(l_strPrefixPath + "/InputTime", m_strData.m_vecLists.at(i).m_strInputTime);                
					p_pJson->SetNodeValue(l_strPrefixPath + "/PersonKeyInfo", m_strData.m_vecLists.at(i).m_strPersonKeyInfo);              
					p_pJson->SetNodeValue(l_strPrefixPath + "/ChangeTime", m_strData.m_vecLists.at(i).m_strChangeTime);               
					p_pJson->SetNodeValue(l_strPrefixPath + "/IncreaseTime", m_strData.m_vecLists.at(i).m_strIncreaseTime);                
					p_pJson->SetNodeValue(l_strPrefixPath + "/PersonKeyNum", m_strData.m_vecLists.at(i).m_strPersonKeyNum);            
					p_pJson->SetNodeValue(l_strPrefixPath + "/EnglishName", m_strData.m_vecLists.at(i).m_strEnglishName);         
					p_pJson->SetNodeValue(l_strPrefixPath + "/Birthday", m_strData.m_vecLists.at(i).m_strBirthday);          
					p_pJson->SetNodeValue(l_strPrefixPath + "/CerdNum", m_strData.m_vecLists.at(i).m_strCerdNum);             
					p_pJson->SetNodeValue(l_strPrefixPath + "/Address", m_strData.m_vecLists.at(i).m_strAddress);               
					p_pJson->SetNodeValue(l_strPrefixPath + "/PoliceStation", m_strData.m_vecLists.at(i).m_strPoliceStation);                
					p_pJson->SetNodeValue(l_strPrefixPath + "/PersonKeyType", m_strData.m_vecLists.at(i).m_strPersonKeyType);               
					p_pJson->SetNodeValue(l_strPrefixPath + "/FilingTime", m_strData.m_vecLists.at(i).m_strFilingTime);               
					p_pJson->SetNodeValue(l_strPrefixPath + "/Name", m_strData.m_vecLists.at(i).m_strName);              
					p_pJson->SetNodeValue(l_strPrefixPath + "/OtherCerdNum", m_strData.m_vecLists.at(i).m_strOtherCerdNum);               
					p_pJson->SetNodeValue(l_strPrefixPath + "/RegisteredAddress", m_strData.m_vecLists.at(i).m_strRegisteredAddress);               
					p_pJson->SetNodeValue(l_strPrefixPath + "/RegisteredPolice", m_strData.m_vecLists.at(i).m_strRegisteredPolice);                
					p_pJson->SetNodeValue(l_strPrefixPath + "/RegisteredPoliceCode", m_strData.m_vecLists.at(i).m_strRegisteredPoliceCode);              
					p_pJson->SetNodeValue(l_strPrefixPath + "/NativePlace", m_strData.m_vecLists.at(i).m_strNativePlace);              
					p_pJson->SetNodeValue(l_strPrefixPath + "/Nationality", m_strData.m_vecLists.at(i).m_strNationality);               
					p_pJson->SetNodeValue(l_strPrefixPath + "/RegisteredArea", m_strData.m_vecLists.at(i).m_strRegisteredArea);               
					p_pJson->SetNodeValue(l_strPrefixPath + "/NativeArea", m_strData.m_vecLists.at(i).m_strNativeArea);                
					p_pJson->SetNodeValue(l_strPrefixPath + "/OtherCerdType", m_strData.m_vecLists.at(i).m_strOtherCerdType);                
					p_pJson->SetNodeValue(l_strPrefixPath + "/Gender", m_strData.m_vecLists.at(i).m_strGender);              
					p_pJson->SetNodeValue(l_strPrefixPath + "/Nation", m_strData.m_vecLists.at(i).m_strNation);    

					p_pJson->SetNodeValue(l_strPrefixPath + "/PersonKeyInfoName", m_strData.m_vecLists.at(i).m_strPersonKeyInfoName);
					p_pJson->SetNodeValue(l_strPrefixPath + "/PersonKeyTypeName", m_strData.m_vecLists.at(i).m_strPersonKeyTypeName);

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
				m_strCode = p_pJson->GetNodeValue("/status", "");
				m_strMessage = p_pJson->GetNodeValue("/msg", "");
				m_strData.m_strCount = p_pJson->GetNodeValue("/data/totalRows", "");
				if (0 == m_strCode.compare("200")) {
					int count = std::atoi(m_strData.m_strCount.c_str());
					for (int i = 0; i < count; i++)
					{
						CData::CList data;
						std::string tmp_path("/data/results/" + std::to_string(i));
						data.m_strPoliceStationCode = p_pJson->GetNodeValue(tmp_path + "/XZZPCS_GAJGJGDM", "");
						data.m_strJurisdiction = p_pJson->GetNodeValue(tmp_path + "/GXDW_GAJGMC", "");
						data.m_strJurisdictionCode = p_pJson->GetNodeValue(tmp_path + "/GXDW_GAJGJGDM", "");
						data.m_strFilingDepart = p_pJson->GetNodeValue(tmp_path + "/LADW_GAJGMC", "");
						data.m_strFilingDepartCode = p_pJson->GetNodeValue(tmp_path + "/LADW_GAJGJGDM", "");
						data.m_strInputTime = p_pJson->GetNodeValue(tmp_path + "/BJ1_ZDRY_XXRKSJ", "");
						data.m_strPersonKeyInfo = p_pJson->GetNodeValue(tmp_path + "/ZDRY_XL_DM", "");
						data.m_strChangeTime = p_pJson->GetNodeValue(tmp_path + "/ZDRYYWJL_GXSJ", "");
						data.m_strIncreaseTime = p_pJson->GetNodeValue(tmp_path + "/ZDRYYWJL_XZSC_RQSJ", "");
						data.m_strPersonKeyNum = p_pJson->GetNodeValue(tmp_path + "/BJ1_ZDRY_BH", "");
						data.m_strEnglishName = p_pJson->GetNodeValue(tmp_path + "/WWXM", "");
						data.m_strBirthday = p_pJson->GetNodeValue(tmp_path + "/CSRQ", "");
						data.m_strCerdNum = p_pJson->GetNodeValue(tmp_path + "/GMSFHM", "");
						data.m_strAddress = p_pJson->GetNodeValue(tmp_path + "/XZZ_QHNXXDZ", "");
						data.m_strPoliceStation = p_pJson->GetNodeValue(tmp_path + "/XZZPCS_GAJGMC", "");
						data.m_strPersonKeyType = p_pJson->GetNodeValue(tmp_path + "/ZDRY_LB1_BS", "");
						data.m_strFilingTime = p_pJson->GetNodeValue(tmp_path + "/ZX5_LARQ", "");
						data.m_strName = p_pJson->GetNodeValue(tmp_path + "/XM", "");
						data.m_strOtherCerdNum = p_pJson->GetNodeValue(tmp_path + "/QTZJ_CYZJHM", "");
						data.m_strRegisteredAddress = p_pJson->GetNodeValue(tmp_path + "/HJDZ_QHNXXDZ", "");
						data.m_strRegisteredPolice = p_pJson->GetNodeValue(tmp_path + "/HJDPCS_GAJGMC", "");
						data.m_strRegisteredPoliceCode = p_pJson->GetNodeValue(tmp_path + "/HJDPCS_GAJGJGDM", "");
						data.m_strNativePlace = p_pJson->GetNodeValue(tmp_path + "/JGGJDQDM", "");
						data.m_strNationality = p_pJson->GetNodeValue(tmp_path + "/GJDM", "");
						data.m_strRegisteredArea = p_pJson->GetNodeValue(tmp_path + "/HJDZ_XZQHDM", "");
						data.m_strNativeArea = p_pJson->GetNodeValue(tmp_path + "/XZZ_XZQHDM", "");
						data.m_strOtherCerdType = p_pJson->GetNodeValue(tmp_path + "/QTZJ_CYZJDM", "");
						data.m_strGender = p_pJson->GetNodeValue(tmp_path + "/XBDM", "");
						data.m_strNation = p_pJson->GetNodeValue(tmp_path + "/MZDM", "");
						data.m_strPersonKeyTypeName  = p_pJson->GetNodeValue(tmp_path + "/ZDRY_LB1_MC", "");
						data.m_strPersonKeyInfoName = p_pJson->GetNodeValue(tmp_path + "/ZDRY_XL_MC", "");
						m_strData.m_vecLists.push_back(data);
					}
				}
				return true;
			}
		public:
			class CData
			{
			public:
				class CList {
				public:
					std::string m_strPoliceStationCode;       // 现住地派出所代码    (XZZPCS_GAJGJGDM)
					std::string m_strJurisdiction;            // 管辖单位    (GXDW_GAJGMC)
					std::string m_strJurisdictionCode;        // 管辖单位机构代码    (GXDW_GAJGJGDM)
					std::string m_strFilingDepart;            // 立案单位    (LADW_GAJGMC)
					std::string m_strFilingDepartCode;        // 立案单位机构代码    (LADW_GAJGJGDM)
					std::string m_strInputTime;               // 纳入部级重点人员库时间    (BJ1_ZDRY_XXRKSJ)
					std::string m_strPersonKeyInfo;           // 重点人员细类    (ZDRY_XL_DM)
					std::string m_strPersonKeyInfoName;       // 重点人员细类名称    (ZDRY_XL_DM)
					std::string m_strChangeTime;              // 业务记录变更时间    (ZDRYYWJL_GXSJ)
					std::string m_strIncreaseTime;            // 业务记录新增时间    (ZDRYYWJL_XZSC_RQSJ)
					std::string m_strPersonKeyNum;            // 部级重点人员编号    (BJ1_ZDRY_BH)
					std::string m_strEnglishName;             // 外文姓名    (WWXM)
					std::string m_strBirthday;                // 出生日期    (CSRQ)
					std::string m_strCerdNum;                 // 公民身份号码    (GMSFHM)
					std::string m_strAddress;                 // 现住地详址    (XZZ_QHNXXDZ)
					std::string m_strPoliceStation;           // 现住地派出所    (XZZPCS_GAJGMC)
					std::string m_strPersonKeyType;           // 重点人员类别标记(最后一位是1的为涉恐、第一位是1的为涉稳、第二刑事、第三治安)    (ZDRY_LB1_BS)
					std::string m_strPersonKeyTypeName;       // 重点人员类别标记名称（ZDRY_LB1_MC）
					std::string m_strFilingTime;              // 最近立案时间    (ZX5_LARQ)
					std::string m_strName;                    // 姓名    (XM)
					std::string m_strOtherCerdNum;            // 其它证件号码    (QTZJ_CYZJHM)
					std::string m_strRegisteredAddress;       // 户籍地详址    (HJDZ_QHNXXDZ)
					std::string m_strRegisteredPolice;        // 户籍地派出所    (HJDPCS_GAJGMC)
					std::string m_strRegisteredPoliceCode;    // 户籍地派出所代码    (HJDPCS_GAJGJGDM)
					std::string m_strNativePlace;             // 籍贯    (JGGJDQDM)
					std::string m_strNationality;             // 国籍    (GJDM)
					std::string m_strRegisteredArea;          // 户籍地区划    (HJDZ_XZQHDM)
					std::string m_strNativeArea;              // 现住地区划    (XZZ_XZQHDM)
					std::string m_strOtherCerdType;           // 其它证件类型    (QTZJ_CYZJDM)
					std::string m_strGender;                  // 性别    (XBDM)
					std::string m_strNation;                  // 民族    (MZDM)


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

		//人员与案件关系查询接口 与 案件详情接口
		class CGetQueryNameCaseDataAndCaseInfoRespond :
			public IRequest
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				p_pJson->SetNodeValue("/code", m_strCode);
				p_pJson->SetNodeValue("/message", m_strMessage);
				p_pJson->SetNodeValue("/data/Count", m_Data.m_strtotalRows);

				for (int i = 0; i < std::atoi(m_Data.m_strtotalRows.c_str()); ++i)
				{
					std::string l_strPrefixPath("/data/List/" + std::to_string(i));
					p_pJson->SetNodeValue(l_strPrefixPath + "/CaseName", m_Data.m_vecResults.at(i).m_strcacsName);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Name", m_Data.m_vecResults.at(i).m_strname);
					p_pJson->SetNodeValue(l_strPrefixPath + "/CertNum", m_Data.m_vecResults.at(i).m_strcertNum);
					p_pJson->SetNodeValue(l_strPrefixPath + "/CaseNo", m_Data.m_vecResults.at(i).m_strcaseNo);
					p_pJson->SetNodeValue(l_strPrefixPath + "/CaseTime", m_Data.m_vecResults.at(i).m_strcaseTime);

					p_pJson->SetNodeValue(l_strPrefixPath + "/CaseClasCode", m_Data.m_vecResults.at(i).m_caseClasCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "/RepoCaseTime", m_Data.m_vecResults.at(i).m_repoCaseTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "/CaseName", m_Data.m_vecResults.at(i).m_caseName);
					p_pJson->SetNodeValue(l_strPrefixPath + "/MainCaseCaseNo", m_Data.m_vecResults.at(i).m_mainCaseCaseNo);
					p_pJson->SetNodeValue(l_strPrefixPath + "/CasclaName", m_Data.m_vecResults.at(i).m_casClaName);
					p_pJson->SetNodeValue(l_strPrefixPath + "/RepoCaseUnitAddrName", m_Data.m_vecResults.at(i).m_repoCaseUnitAddrName);
					p_pJson->SetNodeValue(l_strPrefixPath + "/AccepUnitPsag", m_Data.m_vecResults.at(i).m_accepUnitPsag);
					p_pJson->SetNodeValue(l_strPrefixPath + "/DiscTime", m_Data.m_vecResults.at(i).m_discTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "/MinorCaseCaseNo", m_Data.m_vecResults.at(i).m_minorCaseCaseNo);
					p_pJson->SetNodeValue(l_strPrefixPath + "/CaseClassCode", m_Data.m_vecResults.at(i).m_caseClassCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "/CaseNo", m_Data.m_vecResults.at(i).m_caseNo);
					p_pJson->SetNodeValue(l_strPrefixPath + "/CaseOrgaPsag", m_Data.m_vecResults.at(i).m_caseOrgaPsag);
					p_pJson->SetNodeValue(l_strPrefixPath + "/CaseDate", m_Data.m_vecResults.at(i).m_caseDate);
					p_pJson->SetNodeValue(l_strPrefixPath + "/AccepTime", m_Data.m_vecResults.at(i).m_accepTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "/CaseTimeTimper", m_Data.m_vecResults.at(i).m_caseTimeTimper);
					p_pJson->SetNodeValue(l_strPrefixPath + "/CaseTypeCode", m_Data.m_vecResults.at(i).m_caseTypeCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "/CrimPurpDesc", m_Data.m_vecResults.at(i).m_crimPurpDesc);
					p_pJson->SetNodeValue(l_strPrefixPath + "/CaseSourDesc", m_Data.m_vecResults.at(i).m_caseSourDesc);
					p_pJson->SetNodeValue(l_strPrefixPath + "/CaseAddrAddrCode", m_Data.m_vecResults.at(i).m_caseAddrAddrCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "/CaseOrgaPsagCode", m_Data.m_vecResults.at(i).m_caseOrgaPsagCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "/ArrsceTime", m_Data.m_vecResults.at(i).m_arrsceTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "/SetLawsDate", m_Data.m_vecResults.at(i).m_setLawsDate);
				}
				return p_pJson->ToString();
			}
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				return true;
			}
		public:
			CHeader m_oHeader;
			std::string m_strCode;						// 200 为成功
			std::string m_strMessage;					// success
			std::string m_strqtime;

			struct Results
			{
			public:
				std::string m_strcacsName;              //案件名称
				std::string m_strcaseTime;              //立案时间
				std::string m_strcertNum;               //证件号码
				std::string m_strcaseNo;                //案件编号
				std::string m_strname;                  //姓名

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

			class Data
			{
			public:
				std::string m_strtotalRows;
				std::vector<Results> m_vecResults;
				Results results;
			};
			Data m_Data;

			PROTOCOL::CPostQueryNameCaseDataRequest m_PostQueryNameCaseDataRequest;
			PROTOCOL::CPostQueryCaseInfoRequest m_PostQueryCaseInfoRequest;
		};

		//户籍关系查询接口 与 人员信息查询主题接口
		class CGetQueryCertInfoAndPersonInfoRespond :
			public IRequest
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				p_pJson->SetNodeValue("/code", m_strCode);
				p_pJson->SetNodeValue("/message", m_strMessage);
				p_pJson->SetNodeValue("/data/Count", m_Data.m_strtotalRows);

				for (int i = 0; i < std::atoi(m_Data.m_strtotalRows.c_str()); ++i)
				{
					std::string l_strPrefixPath("/data/List/" + std::to_string(i));
					p_pJson->SetNodeValue(l_strPrefixPath + "/RelType", m_Data.m_vecResults.at(i).m_relType);
					p_pJson->SetNodeValue(l_strPrefixPath + "/HouHeadRel", m_Data.m_vecResults.at(i).m_houHeadRel);
					p_pJson->SetNodeValue(l_strPrefixPath + "/DomicAdmDiv", m_Data.m_vecResults.at(i).m_domicAdmDiv);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Predict", m_Data.m_vecResults.at(i).m_predict);
					p_pJson->SetNodeValue(l_strPrefixPath + "/DomicNum", m_Data.m_vecResults.at(i).m_domicNum);
					p_pJson->SetNodeValue(l_strPrefixPath + "/DomicAddr", m_Data.m_vecResults.at(i).m_domicAddr);
					p_pJson->SetNodeValue(l_strPrefixPath + "/CertNum", m_Data.m_vecResults.at(i).m_certNum);

					p_pJson->SetNodeValue(l_strPrefixPath + "/Esuc", m_Data.m_vecResults.at(i).m_strEscu);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Homeaddr", m_Data.m_vecResults.at(i).m_strHomeaddr);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Sex", m_Data.m_vecResults.at(i).m_strSex);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Edudegree", m_Data.m_vecResults.at(i).m_strEdudegree);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Domplace", m_Data.m_vecResults.at(i).m_strDomplace);

					p_pJson->SetNodeValue(l_strPrefixPath + "/Bplace", m_Data.m_vecResults.at(i).m_strBplace);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Nation", m_Data.m_vecResults.at(i).m_strNation);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Serviceplace", m_Data.m_vecResults.at(i).m_strServiceplace);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Reli", m_Data.m_vecResults.at(i).m_strReli);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Marr", m_Data.m_vecResults.at(i).m_strMarr);

					p_pJson->SetNodeValue(l_strPrefixPath + "/Veh", m_Data.m_vecResults.at(i).m_strVeh);
	
					p_pJson->SetNodeValue(l_strPrefixPath + "/Poli", m_Data.m_vecResults.at(i).m_strPoli);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Prof", m_Data.m_vecResults.at(i).m_strProf);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Workaddr", m_Data.m_vecResults.at(i).m_strWorkaddr);

					p_pJson->SetNodeValue(l_strPrefixPath + "/Birthday", m_Data.m_vecResults.at(i).m_strBirthday);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Mobile", m_Data.m_vecResults.at(i).m_strMobile);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Chname", m_Data.m_vecResults.at(i).m_strChname);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Hplacearea", m_Data.m_vecResults.at(i).m_strHplaceArea);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Hplaceareacodde", m_Data.m_vecResults.at(i).m_strHplaceAreaCode);

					p_pJson->SetNodeValue(l_strPrefixPath + "/Photo", m_Data.m_vecResults.at(i).m_strPhoto);

					if (!m_Data.m_vecResults.at(i).m_vecAttentionTag.empty())
					{
						for (size_t index = 0; index < m_Data.m_vecResults.at(i).m_vecAttentionTag.size(); ++index)
						{
							std::string l_strIndex = std::to_string(index);
							p_pJson->SetNodeValue(l_strPrefixPath + "/Attentiontag/" + l_strIndex, m_Data.m_vecResults.at(i).m_vecAttentionTag.at(index));
						}
					}
					else
					{
						p_pJson->SetNodeValue(l_strPrefixPath + "/Attentiontag/0", "");
					}

					size_t iCount = m_Data.m_vecResults.at(i).PersonTagInfo.m_strData.size();
					for (size_t index = 0; index < iCount; ++index)
					{
						std::string l_strIndex = std::to_string(index);
						p_pJson->SetNodeValue(l_strPrefixPath + "/DataTag/" + l_strIndex + "/TagType", m_Data.m_vecResults.at(i).PersonTagInfo.m_strData[index].m_strTagType);
						p_pJson->SetNodeValue(l_strPrefixPath + "/DataTag/" + l_strIndex + "/TagTypeName", m_Data.m_vecResults.at(i).PersonTagInfo.m_strData[index].m_strTagTypeName);
						size_t tagCount = m_Data.m_vecResults.at(i).PersonTagInfo.m_strData[index].m_vecTags.size();
						for (size_t j = 0; j < tagCount; ++j) {
							std::string strIndex = std::to_string(j);
							p_pJson->SetNodeValue(l_strPrefixPath + "/DataTag/" + l_strIndex + "/Tags/" + strIndex + "/TagCode", m_Data.m_vecResults.at(i).PersonTagInfo.m_strData[index].m_vecTags[j].m_strTagCode);
							p_pJson->SetNodeValue(l_strPrefixPath + "/DataTag/" + l_strIndex + "/Tags/" + strIndex + "/TagName", m_Data.m_vecResults.at(i).PersonTagInfo.m_strData[index].m_vecTags[j].m_strTagName);
						}
					}

				}
				return p_pJson->ToString();
			}
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				return true;
			}
		public:
			CHeader m_oHeader;
			std::string m_strCode;					 // 200 为成功
			std::string m_strMessage;				 // success
			std::string m_strqtime;

			struct Results
			{
			public:
				std::string m_predict;			     //可信度
				std::string m_relType;			     //最新关系类型
				std::string m_certNum;			     //证件号码
				std::string m_domicNum;			     //户号
				std::string m_houHeadRel;		     //与户主关系
				std::string m_domicAddr;		     //户籍地址
				std::string m_domicAdmDiv;		     //所属行政区划

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
				std::string m_strPoli;               //政治面貌
				std::string m_strProf;               //职业
				std::string m_strWorkaddr;           //工作地址

				std::string m_strBirthday;           //出生日期
				std::string m_strMobile;             //手机号码
				std::string m_strChname;             //中文姓名
				std::string m_strHplaceArea;         //户籍地址的行政区划
				std::string m_strHplaceAreaCode;     //户籍地址的行政区划code
				std::string m_strPhoto;              //图片 

				std::vector<std::string>m_vecAttentionTag;

				CGetQueryPersonTagInfoRespond PersonTagInfo;
			};

			class Data
			{
			public:
				std::string m_strtotalRows;
				std::vector<Results> m_vecResults;
				Results results;
			};
			Data m_Data;

			PROTOCOL::CPostQueryCertInfoRequest m_PostQueryCertInfoRequest;
			PROTOCOL::CPostQueryPersonInfoRequest m_PostQueryPersonInfoRequest;
		};


		//机动车信息查询接口 与 人员信息查询主题接口
		class CGetQueryCredInfoAndPersonInfoRespond :
			public IRequest
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				p_pJson->SetNodeValue("/code", m_strCode);
				p_pJson->SetNodeValue("/message", m_strMessage);
				p_pJson->SetNodeValue("/data/Count", std::to_string(m_Data.m_vecResults.size()));
				
				for (size_t i = 0; i < m_Data.m_vecResults.size(); ++i)
				{
					std::string l_strPrefixPath("/data/List/" + std::to_string(i));
					p_pJson->SetNodeValue(l_strPrefixPath + "/VehStatCode", m_Data.m_vecResults.at(i).m_vehStatCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Mob", m_Data.m_vecResults.at(i).m_mob);
					p_pJson->SetNodeValue(l_strPrefixPath + "/VehLicTname", m_Data.m_vecResults.at(i).m_vehLicTname);
					p_pJson->SetNodeValue(l_strPrefixPath + "/CredNum", m_Data.m_vecResults.at(i).m_credNum);
					p_pJson->SetNodeValue(l_strPrefixPath + "/InsEndDate", m_Data.m_vecResults.at(i).m_insEndDate);
					p_pJson->SetNodeValue(l_strPrefixPath + "/VehcBrandModel", m_Data.m_vecResults.at(i).m_vehcBrandModel);
					p_pJson->SetNodeValue(l_strPrefixPath + "/VehStatName", m_Data.m_vecResults.at(i).m_vehStatName);
					p_pJson->SetNodeValue(l_strPrefixPath + "/InspVeDate", m_Data.m_vecResults.at(i).m_inspVeDate);
					p_pJson->SetNodeValue(l_strPrefixPath + "/VehPlateNum", m_Data.m_vecResults.at(i).m_vehPlateNum);
					p_pJson->SetNodeValue(l_strPrefixPath + "/VehUsagName", m_Data.m_vecResults.at(i).m_vehUsagName);
					p_pJson->SetNodeValue(l_strPrefixPath + "/VehType", m_Data.m_vecResults.at(i).m_vehType);
					p_pJson->SetNodeValue(l_strPrefixPath + "/VehTname", m_Data.m_vecResults.at(i).m_vehTname);
					p_pJson->SetNodeValue(l_strPrefixPath + "/FirstRegDate", m_Data.m_vecResults.at(i).m_firstRegDate);
					p_pJson->SetNodeValue(l_strPrefixPath + "/EngineNo", m_Data.m_vecResults.at(i).m_engineNo);
					p_pJson->SetNodeValue(l_strPrefixPath + "/VehColorName", m_Data.m_vecResults.at(i).m_m_vehColorName);
					p_pJson->SetNodeValue(l_strPrefixPath + "/RegComPsag", m_Data.m_vecResults.at(i).m_regComPsag);
					p_pJson->SetNodeValue(l_strPrefixPath + "/VehLicTcode", m_Data.m_vecResults.at(i).m_strVehLicTcode);

					p_pJson->SetNodeValue(l_strPrefixPath + "/Esuc", m_Data.m_vecResults.at(i).m_strEscu);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Homeaddr", m_Data.m_vecResults.at(i).m_strHomeaddr);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Sex", m_Data.m_vecResults.at(i).m_strSex);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Edudegree", m_Data.m_vecResults.at(i).m_strEdudegree);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Domplace", m_Data.m_vecResults.at(i).m_strDomplace);

					p_pJson->SetNodeValue(l_strPrefixPath + "/Bplace", m_Data.m_vecResults.at(i).m_strBplace);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Nation", m_Data.m_vecResults.at(i).m_strNation);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Serviceplace", m_Data.m_vecResults.at(i).m_strServiceplace);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Reli", m_Data.m_vecResults.at(i).m_strReli);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Marr", m_Data.m_vecResults.at(i).m_strMarr);

					p_pJson->SetNodeValue(l_strPrefixPath + "/Veh", m_Data.m_vecResults.at(i).m_strVeh);

					p_pJson->SetNodeValue(l_strPrefixPath + "/Poli", m_Data.m_vecResults.at(i).m_strPoli);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Prof", m_Data.m_vecResults.at(i).m_strProf);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Workaddr", m_Data.m_vecResults.at(i).m_strWorkaddr);

					p_pJson->SetNodeValue(l_strPrefixPath + "/Birthday", m_Data.m_vecResults.at(i).m_strBirthday);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Mobile", m_Data.m_vecResults.at(i).m_strMobile);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Chname", m_Data.m_vecResults.at(i).m_strChname);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Hplacearea", m_Data.m_vecResults.at(i).m_strHplaceArea);
					p_pJson->SetNodeValue(l_strPrefixPath + "/Hplaceareacode", m_Data.m_vecResults.at(i).m_strHplaceAreaCode);

					p_pJson->SetNodeValue(l_strPrefixPath + "/Photo", m_Data.m_vecResults.at(i).m_strPhoto);

					if (!m_Data.m_vecResults.at(i).m_vecAttentionTag.empty())
					{
						for (size_t index = 0; index < m_Data.m_vecResults.at(i).m_vecAttentionTag.size(); ++index)
						{
							std::string l_strIndex = std::to_string(index);
							p_pJson->SetNodeValue(l_strPrefixPath + "/Attentiontag/" + l_strIndex, m_Data.m_vecResults.at(i).m_vecAttentionTag.at(index));
						}
					}
					else
					{
						p_pJson->SetNodeValue(l_strPrefixPath + "/Attentiontag/0", "");
					}

					size_t iCount = m_Data.m_vecResults.at(i).PersonTagInfo.m_strData.size();
					for (size_t index = 0; index < iCount; ++index)
					{
						std::string l_strIndex = std::to_string(index);
						p_pJson->SetNodeValue(l_strPrefixPath + "/DataTag/" + l_strIndex + "/TagType", m_Data.m_vecResults.at(i).PersonTagInfo.m_strData[index].m_strTagType);
						p_pJson->SetNodeValue(l_strPrefixPath + "/DataTag/" + l_strIndex + "/TagTypeName", m_Data.m_vecResults.at(i).PersonTagInfo.m_strData[index].m_strTagTypeName);
						size_t tagCount = m_Data.m_vecResults.at(i).PersonTagInfo.m_strData[index].m_vecTags.size();
						for (size_t j = 0; j < tagCount; ++j) {
							std::string strIndex = std::to_string(j);
							p_pJson->SetNodeValue(l_strPrefixPath + "/DataTag/" + l_strIndex + "/Tags/" + strIndex + "/TagCode", m_Data.m_vecResults.at(i).PersonTagInfo.m_strData[index].m_vecTags[j].m_strTagCode);
							p_pJson->SetNodeValue(l_strPrefixPath + "/DataTag/" + l_strIndex + "/Tags/" + strIndex + "/TagName", m_Data.m_vecResults.at(i).PersonTagInfo.m_strData[index].m_vecTags[j].m_strTagName);
						}
					}
				}
				return p_pJson->ToString();
			}
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				return true;
			}
		public:
			CHeader m_oHeader;
			std::string m_strCode;				    // 200 为成功
			std::string m_strMessage;			    // success
			std::string m_strqtime;

			struct Results
			{
			public:
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
				std::string m_strVehLicTcode;		//号牌种类代码

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
				std::string m_strPoli;               //政治面貌
				std::string m_strProf;               //职业
				std::string m_strWorkaddr;           //工作地址

				std::string m_strBirthday;           //出生日期
				std::string m_strMobile;             //手机号码
				std::string m_strChname;             //中文姓名
				std::string m_strHplaceArea;         //户籍地址的行政区 HPLACEAREAm_strHplaceAreaCode
				std::string m_strHplaceAreaCode;     //户籍地址的行政区code
				std::string m_strPhoto;              //图片 

				std::vector<std::string>m_vecAttentionTag;
				CGetQueryPersonTagInfoRespond PersonTagInfo;
			};

			class Data
			{
			public:
				std::string m_strtotalRows;
				std::vector<Results> m_vecResults;
				Results results;
				int Index;
			};
			Data m_Data;

			PROTOCOL::CPostQueryCredInfoRequest m_PostQueryCredInfoRequest;
			PROTOCOL::CPostQueryPersonInfoRequest m_PostQueryPersonInfoRequest;
		};


		//车牌与案件接口 与 案件详情接口
		class CGetQueryCaseVehInfoAndCaseInfoRespond :
			public IRequest
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				p_pJson->SetNodeValue("/code", m_strCode);
				p_pJson->SetNodeValue("/message", m_strMessage);
				p_pJson->SetNodeValue("/data/Count", m_Data.m_strtotalRows);

				for (int i = 0; i < std::atoi(m_Data.m_strtotalRows.c_str()); ++i)
				{
					std::string l_strPrefixPath("/data/List/" + std::to_string(i) + "/");

					p_pJson->SetNodeValue(l_strPrefixPath + "DisPlace", m_Data.m_vecResults.at(i).m_strDisPlace);
					p_pJson->SetNodeValue(l_strPrefixPath + "CaseNo", m_Data.m_vecResults.at(i).m_strCaseNo);
					p_pJson->SetNodeValue(l_strPrefixPath + "VehPlateNum", m_Data.m_vecResults.at(i).m_strVehPlateNum);
					p_pJson->SetNodeValue(l_strPrefixPath + "VehLictcode", m_Data.m_vecResults.at(i).m_strVehLicTcode);

					p_pJson->SetNodeValue(l_strPrefixPath + "CaseClasCode", m_Data.m_vecResults.at(i).m_caseClasCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "RepoCaseTime", m_Data.m_vecResults.at(i).m_repoCaseTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "CaseName", m_Data.m_vecResults.at(i).m_caseName);
					p_pJson->SetNodeValue(l_strPrefixPath + "MainCaseCaseNo", m_Data.m_vecResults.at(i).m_mainCaseCaseNo);
					p_pJson->SetNodeValue(l_strPrefixPath + "CasclaName", m_Data.m_vecResults.at(i).m_casClaName);
					p_pJson->SetNodeValue(l_strPrefixPath + "RepoCaseUnitAddrName", m_Data.m_vecResults.at(i).m_repoCaseUnitAddrName);
					p_pJson->SetNodeValue(l_strPrefixPath + "AccepUnitPsag", m_Data.m_vecResults.at(i).m_accepUnitPsag);
					p_pJson->SetNodeValue(l_strPrefixPath + "DiscTime", m_Data.m_vecResults.at(i).m_discTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "MinorCaseCaseNo", m_Data.m_vecResults.at(i).m_minorCaseCaseNo);
					p_pJson->SetNodeValue(l_strPrefixPath + "CaseClassCode", m_Data.m_vecResults.at(i).m_caseClassCode);
					//p_pJson->SetNodeValue(l_strPrefixPath + "CaseNo", m_Data.m_vecResults.at(i).m_caseNo);
					p_pJson->SetNodeValue(l_strPrefixPath + "CaseOrgaPsag", m_Data.m_vecResults.at(i).m_caseOrgaPsag);
					p_pJson->SetNodeValue(l_strPrefixPath + "CaseDate", m_Data.m_vecResults.at(i).m_caseDate);
					p_pJson->SetNodeValue(l_strPrefixPath + "AccepTime", m_Data.m_vecResults.at(i).m_accepTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "CaseTimeTimper", m_Data.m_vecResults.at(i).m_caseTimeTimper);
					p_pJson->SetNodeValue(l_strPrefixPath + "CaseTypeCode", m_Data.m_vecResults.at(i).m_caseTypeCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "CrimPurpDesc", m_Data.m_vecResults.at(i).m_crimPurpDesc);
					p_pJson->SetNodeValue(l_strPrefixPath + "CaseSourDesc", m_Data.m_vecResults.at(i).m_caseSourDesc);
					p_pJson->SetNodeValue(l_strPrefixPath + "CaseAddrAddrCode", m_Data.m_vecResults.at(i).m_caseAddrAddrCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "CaseOrgaPsagCode", m_Data.m_vecResults.at(i).m_caseOrgaPsagCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "ArrsceTime", m_Data.m_vecResults.at(i).m_arrsceTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "SetLawsDate", m_Data.m_vecResults.at(i).m_setLawsDate);
				}
				return p_pJson->ToString();
			}
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				return true;
			}
		public:
			CHeader m_oHeader;
			std::string m_strCode;						// 200 为成功
			std::string m_strMessage;					// success
			std::string m_strqtime;

			struct Results
			{
			public:
				std::string m_strDisPlace;			    // 发现地点
				std::string m_strCaseNo;				// 案件编号
				std::string m_strVehPlateNum;			// 车牌号
				std::string m_strVehLicTcode;           // 机动车号牌种类

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

			class Data
			{
			public:
				std::string m_strtotalRows;
				std::vector<Results> m_vecResults;
				Results results;
			};
			Data m_Data;

			PROTOCOL::CPostQueryCaseVehInfoRequest m_PostQueryCaseVehInfoRequest;
			PROTOCOL::CPostQueryCaseInfoRequest m_PostQueryCaseInfoRequest;
		};

		class CPostespond :
			public ISend
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/code", m_oBody.m_strcode);
				p_pJson->SetNodeValue("/body/message", m_oBody.m_strmessage);

				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
				std::string m_strcode;
				std::string m_strmessage;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};


		class CAddressInfoRespond :
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
				p_pJson->SetNodeValue("/data/Count", std::to_string(m_Data.m_vecResults.size()));

				for (size_t i = 0; i < m_Data.m_vecResults.size(); ++i)
				{
					std::string l_strPrefixPath("/data/List/" + std::to_string(i));
					p_pJson->SetNodeValue(l_strPrefixPath + "/addrname", m_Data.m_vecResults.at(i).m_strAddrName);
					p_pJson->SetNodeValue(l_strPrefixPath + "/lat", m_Data.m_vecResults.at(i).m_strLat);
					p_pJson->SetNodeValue(l_strPrefixPath + "/normalat", m_Data.m_vecResults.at(i).m_strNormaLat);
					p_pJson->SetNodeValue(l_strPrefixPath + "/lon", m_Data.m_vecResults.at(i).m_strLon);
					p_pJson->SetNodeValue(l_strPrefixPath + "/infodelejudgeflag", m_Data.m_vecResults.at(i).m_strInfoDeleJudgeFlag);
					p_pJson->SetNodeValue(l_strPrefixPath + "/normalon", m_Data.m_vecResults.at(i).m_strNormaLon);

					p_pJson->SetNodeValue(l_strPrefixPath + "/comname", m_Data.m_vecResults.at(i).m_strCompName);

					p_pJson->SetNodeValue(l_strPrefixPath + "/ownpolstatpsag", m_Data.m_vecResults.at(i).m_strOwnPolStatPsag);
					p_pJson->SetNodeValue(l_strPrefixPath + "/unittname", m_Data.m_vecResults.at(i).m_strUnitTname);

					p_pJson->SetNodeValue(l_strPrefixPath + "/ownpolstatpsagcode", m_Data.m_vecResults.at(i).m_strOwnPolStatPsagCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "/unittcode", m_Data.m_vecResults.at(i).m_strUnitTcode);

				}
				return p_pJson->ToString();
			}

		public:
			struct Results
			{
				std::string m_strAddrName;			    // 地址名称 or 单位详址
				std::string m_strLat;				    // 维度
				std::string m_strNormaLat;			    // 归一化地球纬度
				std::string m_strLon;                   // 经度
				std::string m_strInfoDeleJudgeFlag;	    // 是否删除
				std::string m_strNormaLon;              // 归一化地球经度
				std::string m_strCompName;			    // 单位名称
				std::string m_strUnitTcode;				// 单位类别代码
				std::string m_strUnitTname;			    // 单位类别
				std::string m_strOwnPolStatPsagCode;    // 所属派出所代码
				std::string m_strOwnPolStatPsag;		// 所属派出所
			};
		public:
			CHeader m_oHeader;
			std::string m_strCode;						// 200 为成功
			std::string m_strMessage;					// success
			class Data
			{
			public:
				std::string m_strtotalRows;
				std::vector<Results> m_vecResults;
			};
			Data m_Data;

		};

	}
}

