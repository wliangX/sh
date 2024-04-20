/*
cmd ����
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
				std::string m_strmobile;      //�ֻ�����
				std::string m_strdomplace;    //��ַ
				std::string m_strlevel;       //�ȼ�
				std::string m_stridno;        //���֤��
				std::string m_strchname;      //����
				std::string m_strsex;         //�Ա�
				std::string m_strbirthday;    //����
				std::string m_strage;         //����
				std::string m_strhplace;      //סַ
			};
			Data m_odata;
			std::vector<Data> m_vecData;
		};
		//��Ա�밸���Ĺ�ϵ
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
			std::string m_strCode;						// 200 Ϊ�ɹ�
			std::string m_strMessage;					// success
			std::string m_strqtime;
			std::string m_strcode;
			std::string m_strmessage;
			class Results
			{
			public:
				std::string m_strcacsName;      //��������
				std::string m_strcaseTime;      //����ʱ��
				std::string m_strcertNum;       //֤������
				std::string m_strcaseNo;        //�������
				std::string m_strname;          //����
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
				std::string m_strtrafIllrecTcode;      //��¼���ʹ���
				std::string m_strtrafIllrecTname;      //��¼����
				std::string m_strIllePlacAddrName;     //Υ����ַ
				std::string m_strvehPlateNum;          //���ƺ���
				std::string m_strvehLicTname;          //��������
				std::string m_strilleTime;             //Υ��ʱ��
				std::string m_strprocTime;             //����ʱ��
				std::string m_strilleManHandRcode;     //Υ����Ϊ�˴�����
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
					std::string m_strDisPlace;			    // ���ֵص�
					std::string m_strCaseNo;				// �������
					std::string m_strVehPlateNum;			// ���ƺ�
					std::string m_strVehLicTcode;           // ��������������
				};
			public:
				std::string m_strCount;					// ����
				std::vector<CList> m_vecLists;			// ���
			};
		public:
			CHeader m_oHeader;
			std::string m_strCode;						// 200 Ϊ�ɹ�
			std::string m_strMessage;					// success
			CData m_strData;							// ����
		};

		//�Ĵ�ʡ������ƽ̨��Ա��ǩ��ѯ����
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
					std::string m_strTagCode;			    // ��ǩ����
					std::string m_strTagName;				// �������
				};
			public:
				std::string m_strTagType;					// ��ǩ���ͱ���
				std::string m_strTagTypeName;			    // ��ǩ��������
				std::vector<CTag> m_vecTags;			    // ��ǩ
			};
		public:
			CHeader m_oHeader;
			std::string m_strCode;						    // 200 Ϊ�ɹ�
			std::string m_strMessage;					    // success
			std::vector<CData> m_strData;				    // ����
			std::string m_strCount;                         // ����
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
						std::string m_strAddrName;			    // ��ַ����
						std::string m_strLat;				    // ά��
						std::string m_strNormaLat;			    // ��һ������γ��
						std::string m_strLon;                   // ����
						std::string m_strInfoDeleJudgeFlag;	    // �Ƿ�ɾ��
						std::string m_strNormaLon;              // ��һ�����򾭶�
						std::string m_strDistance;              // ����
					};
				public:
					std::string m_strCount;					    // ����
					std::vector<CList> m_vecLists;			    // ���
				};
			public:
				CHeader m_oHeader;
				std::string m_strCode;						// 200 Ϊ�ɹ�
				std::string m_strMessage;					// success
				CData m_strData;							// ����
			};

			// ʵ�е�λ��ѯ
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
						std::string m_strCompName;			      // ��λ����
						std::string m_strUnitTcode;				  // ��λ������
						std::string m_strUnitTname;			      // ��λ���
						std::string m_strComAddiDetailAddr;       // ��λ��ַ
						std::string m_strCompAddrLon;			  // ��λ��ַ����
						std::string m_strCompAddrLat;             // ��λ��ַγ��
						std::string m_strOwnPolStatPsagCode;	  // �����ɳ�������
						std::string m_strOwnPolStatPsag;		  // �����ɳ���
						std::string m_strNormaCompAddrLon;        // ��һ����λ��ַ����
						std::string m_strNormaCompAddrLat;		  // ��һ����λ��ַ����
						std::string m_strDistance;                // ����
					};
				public:
					std::string m_strCount;					// ����
					std::vector<CList> m_vecLists;			// ���
				};
			public:
				CHeader m_oHeader;
				std::string m_strCode;						// 200 Ϊ�ɹ�
				std::string m_strMessage;					// success
				CData m_strData;							// ����
			};

		// ʵ�з��ݲ�ѯ
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
					std::string m_strHousClasCode;					// �������ʹ���
					std::string m_strHousClasName;		// ��������
					std::string m_strHousTcode;		// ����������
					std::string m_strHousTname;		// �������
					std::string m_strHousOwnerAddrName;		// ������ַ
					std::string m_strOwnPolStatPsagCode;		// �ɳ������ش���
					std::string m_strOwnPolStatPsag;		// �ɳ�����������
					std::string m_strLon;		// ����
					std::string m_strLat;		// γ��
					std::string m_strDoplaNoSernum;		// ���ƺ����к�
					std::string m_strHouseNo;		// ���ݱ��
					std::string m_strCancJudgeFlag;		// �Ƿ�ע��
					std::string m_strNormaLon;		// ��һ�����򾭶�
					std::string m_strNormaLat;		// ��һ������γ��
				};
			public:
				std::string m_strCount;					// ����
				std::vector<CList> m_vecLists;			// ���
			};
		public:
			CHeader m_oHeader;
			std::string m_strCode;						// 200 Ϊ�ɹ�
			std::string m_strMessage;					// success
			CData m_strData;							// ����
		};
	
		//������Ϣ��ѯ
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
					std::string m_caseClasCode;				//��������
					std::string m_repoCaseTime;				//����ʱ��
					std::string m_caseName;					//��������
					std::string m_mainCaseCaseNo;			//�Ӱ��������
					std::string m_casClaName;				//�������
					std::string m_repoCaseUnitAddrName;		//������λ��ַ����
					std::string m_accepUnitPsag;			//����λ
					std::string m_discTime;					//���ְ��¼�ʱ��
					std::string m_minorCaseCaseNo;			//�Ӱ��������
					std::string m_caseClassCode;			//�������ʹ���
					std::string m_caseNo;					//�������
					std::string m_caseOrgaPsag;				//�참��λ
					std::string m_caseDate;					//��������
					std::string m_accepTime;				//����ʱ��
					std::string m_caseTimeTimper;			//����ʱ���
					std::string m_caseTypeCode;				//����������
					std::string m_crimPurpDesc;				//����Ŀ������
					std::string m_caseSourDesc;				//������Դ
					std::string m_caseAddrAddrCode;			//�����ص�ַ����
					std::string m_caseOrgaPsagCode;			//�참��λ����
					std::string m_arrsceTime;				//�����ֳ�ʱ��
					std::string m_setLawsDate;				//�᰸����
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
				std::string m_insEndDate;			//������ֹ����
				std::string m_credNum;				//֤������
				std::string m_mob;					//�ֻ�����
				std::string m_vehLicTname;			//��������
				std::string m_vehPlateNum;			//���ƺ���
				std::string m_vehcBrandModel;		//����Ʒ��
				std::string m_vehType;				//�����ͺ�
				std::string m_m_vehColorName;		//������ɫ
				std::string m_engineNo;				//��������
				std::string m_vehUsagName;			//������;
				std::string m_vehStatCode;			//������״̬����
				std::string m_vehStatName;			//������״̬
				std::string m_inspVeDate;			//������Ч��ֹ
				std::string m_firstRegDate;			//���εǼ�����
				std::string m_regComPsag;			//�Ǽǵ�λ����
				std::string m_vehTname;				//��������
				std::string m_strVehLicTcode;		//�����������
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
				std::string m_predict;			//���Ŷ�
				std::string m_relType;			//���¹�ϵ����
				std::string m_certNum;			//֤������
				std::string m_domicNum;			//����
				std::string m_houHeadRel;		//�뻧����ϵ
				std::string m_domicAddr;		//������ַ
				std::string m_domicAdmDiv;		//������������
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
				std::string m_strEscu;               //����״��
				std::string m_strHomeaddr;           //��ͥ��ַ
				std::string m_strSex;                //�Ա�
				std::string m_strEdudegree;          //�Ļ��̶�
				std::string m_strDomplace;           //��ס��ַ

				std::string m_strBplace;             //������ַ
				std::string m_strNation;             //���� 
				std::string m_strServiceplace;       //��������ȡ���µ�ѧУ���ƻ��߹�����λ���ơ� 
				std::string m_strReli;               //�ڽ�����
				std::string m_strMarr;               //����״��

				std::string m_strVeh;                //���ƺ�
				std::string m_strHplace;             //������ַ
				std::string m_strPoli;               //������ò
				std::string m_strProf;               //ְҵ
				std::string m_strWorkaddr;           //������ַ

				std::string m_strIdno;               //֤������
				std::string m_strBirthday;           //��������
				std::string m_strMobile;             //�ֻ�����
				std::string m_strChname;             //��������
				std::string m_strHplaceArea;         //������ַ������������ HPLACEAREA
				std::string m_strHplaceAreaCode;     //������ַ�������� code HPLACEAREA
				std::string m_strPhoto;              //ͼƬ 
				std::vector<std::string>m_vecAttentionTag;       //��ע��Ա��ǩ 
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

		//�Ĵ�ʡ���Ⱥ����Ա��Ϣ 
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
				std::string m_strLxdh;         //��ϵ�绰
				std::string m_strXm;		   //����
				std::string m_strSacylxdm;     //Ⱥ������
				std::string m_strZjhm;         //֤������			
			};

			class Data
			{
			public:
				std::string m_strtotalRows;
				std::vector<Results> m_vecResults;
			};
			Data m_Data;
		};

		//�Ĵ�ʡ������Ա������Ϣ��ѯ���� 
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
				std::string m_strBmch;         //����/�º�
				std::string m_strCsrq;		   //��������
				std::string m_strHjdzXzqhmc;   //������ַ������������
				std::string m_strSjjzdxzqhmc;  //ʵ�ʾ�ס����������
				std::string m_strSjjzdgajgmc;  //ʵ�ʾ�ס�ع�������
				std::string m_strLrsj;		   //¼��ʱ��
				std::string m_strSg;           //���
				std::string m_strHjdzgajgmc;   //������ַ������������
				std::string m_strSjjzddzmc;    //ʵ�ʾ�ס����ַ
				std::string m_strLrdwdwmc;	   //¼�뵥λ��λ����
				std::string m_strXm;           //����
				std::string m_strSfzhm18;      //���֤��
				std::string m_strHjdzdzmc;     //������ַ��ַ
				std::string m_strXbdm;		   //�Ա�
				std::string m_strMzdm;         //����		
			};

			class Data
			{
			public:
				std::string m_strtotalRows;
				std::vector<Results> m_vecResults;
			};
			Data m_Data;
		};

		// ȫ��������Ա��Ϣ��ѯ����
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
					std::string m_strArrestCode;        // �����������
					std::string m_strAccentCode;        // ����_����
					std::string m_strCaseInfo;        // ��Ҫ���鼰������Ϣ
					std::string m_strEscapeDate;        // ��������
					std::string m_strEscapeNum;        // ������Ա���
					std::string m_strName;        // ����
					std::string m_strGenderCode;        // �Ա����
					std::string m_strAddressArea;        // ��ס��������������
					std::string m_strAddressInfo;        // ��ס����ַ
					std::string m_strCaseNum;        // �������
					std::string m_strCaseCode;        // ����������
					std::string m_strFilingDepartArea;        // ������λ����
					std::string m_strFilingDepartCode;        // ������λ����
					std::string m_strCerdNum;        // ֤������
					std::string m_strHeight;        // ���
					std::string m_strHostContactInfo;        // ������ϵ��ʽ
					std::string m_strWantedCircular;        // ͨ������
					std::string m_strPermanentAddress;        // ����������
				};
			public:
				std::string m_strCount;					// ����
				std::vector<CList> m_vecLists;			// ���
			};
		public:
			CHeader m_oHeader;
			std::string m_strCode;						// 200 Ϊ�ɹ�
			std::string m_strMessage;					// success
			CData m_strData;							// ����
		};

		// ȫ���鱨�ص���Ա��Ϣ��ѯ����
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
					std::string m_strPoliceStationCode;       // ��ס���ɳ�������    (XZZPCS_GAJGJGDM)
					std::string m_strJurisdiction;            // ��Ͻ��λ    (GXDW_GAJGMC)
					std::string m_strJurisdictionCode;        // ��Ͻ��λ��������    (GXDW_GAJGJGDM)
					std::string m_strFilingDepart;            // ������λ    (LADW_GAJGMC)
					std::string m_strFilingDepartCode;        // ������λ��������    (LADW_GAJGJGDM)
					std::string m_strInputTime;               // ���벿���ص���Ա��ʱ��    (BJ1_ZDRY_XXRKSJ)
					std::string m_strPersonKeyInfo;           // �ص���Աϸ��    (ZDRY_XL_DM)
					std::string m_strPersonKeyInfoName;       // �ص���Աϸ������    (ZDRY_XL_DM)
					std::string m_strChangeTime;              // ҵ���¼���ʱ��    (ZDRYYWJL_GXSJ)
					std::string m_strIncreaseTime;            // ҵ���¼����ʱ��    (ZDRYYWJL_XZSC_RQSJ)
					std::string m_strPersonKeyNum;            // �����ص���Ա���    (BJ1_ZDRY_BH)
					std::string m_strEnglishName;             // ��������    (WWXM)
					std::string m_strBirthday;                // ��������    (CSRQ)
					std::string m_strCerdNum;                 // ������ݺ���    (GMSFHM)
					std::string m_strAddress;                 // ��ס����ַ    (XZZ_QHNXXDZ)
					std::string m_strPoliceStation;           // ��ס���ɳ���    (XZZPCS_GAJGMC)
					std::string m_strPersonKeyType;           // �ص���Ա�����(���һλ��1��Ϊ��֡���һλ��1��Ϊ���ȡ��ڶ����¡������ΰ�)    (ZDRY_LB1_BS)
					std::string m_strPersonKeyTypeName;       // �ص���Ա��������ƣ�ZDRY_LB1_MC��
					std::string m_strFilingTime;              // �������ʱ��    (ZX5_LARQ)
					std::string m_strName;                    // ����    (XM)
					std::string m_strOtherCerdNum;            // ����֤������    (QTZJ_CYZJHM)
					std::string m_strRegisteredAddress;       // ��������ַ    (HJDZ_QHNXXDZ)
					std::string m_strRegisteredPolice;        // �������ɳ���    (HJDPCS_GAJGMC)
					std::string m_strRegisteredPoliceCode;    // �������ɳ�������    (HJDPCS_GAJGJGDM)
					std::string m_strNativePlace;             // ����    (JGGJDQDM)
					std::string m_strNationality;             // ����    (GJDM)
					std::string m_strRegisteredArea;          // ����������    (HJDZ_XZQHDM)
					std::string m_strNativeArea;              // ��ס������    (XZZ_XZQHDM)
					std::string m_strOtherCerdType;           // ����֤������    (QTZJ_CYZJDM)
					std::string m_strGender;                  // �Ա�    (XBDM)
					std::string m_strNation;                  // ����    (MZDM)


				};
			public:
				std::string m_strCount;					// ����
				std::vector<CList> m_vecLists;			// ���
			};
		public:
			CHeader m_oHeader;
			std::string m_strCode;						// 200 Ϊ�ɹ�
			std::string m_strMessage;					// success
			CData m_strData;							// ����
		};

		//��Ա�밸����ϵ��ѯ�ӿ� �� ��������ӿ�
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
			std::string m_strCode;						// 200 Ϊ�ɹ�
			std::string m_strMessage;					// success
			std::string m_strqtime;

			struct Results
			{
			public:
				std::string m_strcacsName;              //��������
				std::string m_strcaseTime;              //����ʱ��
				std::string m_strcertNum;               //֤������
				std::string m_strcaseNo;                //�������
				std::string m_strname;                  //����

				std::string m_caseClasCode;				//��������
				std::string m_repoCaseTime;				//����ʱ��
				std::string m_caseName;					//��������
				std::string m_mainCaseCaseNo;			//�Ӱ��������
				std::string m_casClaName;				//�������
				std::string m_repoCaseUnitAddrName;		//������λ��ַ����
				std::string m_accepUnitPsag;			//����λ
				std::string m_discTime;					//���ְ��¼�ʱ��
				std::string m_minorCaseCaseNo;			//�Ӱ��������
				std::string m_caseClassCode;			//�������ʹ���
				std::string m_caseNo;					//�������
				std::string m_caseOrgaPsag;				//�참��λ
				std::string m_caseDate;					//��������
				std::string m_accepTime;				//����ʱ��
				std::string m_caseTimeTimper;			//����ʱ���
				std::string m_caseTypeCode;				//����������
				std::string m_crimPurpDesc;				//����Ŀ������
				std::string m_caseSourDesc;				//������Դ
				std::string m_caseAddrAddrCode;			//�����ص�ַ����
				std::string m_caseOrgaPsagCode;			//�참��λ����
				std::string m_arrsceTime;				//�����ֳ�ʱ��
				std::string m_setLawsDate;				//�᰸����
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

		//������ϵ��ѯ�ӿ� �� ��Ա��Ϣ��ѯ����ӿ�
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
			std::string m_strCode;					 // 200 Ϊ�ɹ�
			std::string m_strMessage;				 // success
			std::string m_strqtime;

			struct Results
			{
			public:
				std::string m_predict;			     //���Ŷ�
				std::string m_relType;			     //���¹�ϵ����
				std::string m_certNum;			     //֤������
				std::string m_domicNum;			     //����
				std::string m_houHeadRel;		     //�뻧����ϵ
				std::string m_domicAddr;		     //������ַ
				std::string m_domicAdmDiv;		     //������������

				std::string m_strEscu;               //����״��
				std::string m_strHomeaddr;           //��ͥ��ַ
				std::string m_strSex;                //�Ա�
				std::string m_strEdudegree;          //�Ļ��̶�
				std::string m_strDomplace;           //��ס��ַ

				std::string m_strBplace;             //������ַ
				std::string m_strNation;             //���� 
				std::string m_strServiceplace;       //��������ȡ���µ�ѧУ���ƻ��߹�����λ���ơ� 
				std::string m_strReli;               //�ڽ�����
				std::string m_strMarr;               //����״��

				std::string m_strVeh;                //���ƺ�
				std::string m_strPoli;               //������ò
				std::string m_strProf;               //ְҵ
				std::string m_strWorkaddr;           //������ַ

				std::string m_strBirthday;           //��������
				std::string m_strMobile;             //�ֻ�����
				std::string m_strChname;             //��������
				std::string m_strHplaceArea;         //������ַ����������
				std::string m_strHplaceAreaCode;     //������ַ����������code
				std::string m_strPhoto;              //ͼƬ 

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


		//��������Ϣ��ѯ�ӿ� �� ��Ա��Ϣ��ѯ����ӿ�
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
			std::string m_strCode;				    // 200 Ϊ�ɹ�
			std::string m_strMessage;			    // success
			std::string m_strqtime;

			struct Results
			{
			public:
				std::string m_insEndDate;			//������ֹ����
				std::string m_credNum;				//֤������
				std::string m_mob;					//�ֻ�����
				std::string m_vehLicTname;			//��������
				std::string m_vehPlateNum;			//���ƺ���
				std::string m_vehcBrandModel;		//����Ʒ��
				std::string m_vehType;				//�����ͺ�
				std::string m_m_vehColorName;		//������ɫ
				std::string m_engineNo;				//��������
				std::string m_vehUsagName;			//������;
				std::string m_vehStatCode;			//������״̬����
				std::string m_vehStatName;			//������״̬
				std::string m_inspVeDate;			//������Ч��ֹ
				std::string m_firstRegDate;			//���εǼ�����
				std::string m_regComPsag;			//�Ǽǵ�λ����
				std::string m_vehTname;				//��������
				std::string m_strVehLicTcode;		//�����������

				std::string m_strEscu;               //����״��
				std::string m_strHomeaddr;           //��ͥ��ַ
				std::string m_strSex;                //�Ա�
				std::string m_strEdudegree;          //�Ļ��̶�
				std::string m_strDomplace;           //��ס��ַ

				std::string m_strBplace;             //������ַ
				std::string m_strNation;             //���� 
				std::string m_strServiceplace;       //��������ȡ���µ�ѧУ���ƻ��߹�����λ���ơ� 
				std::string m_strReli;               //�ڽ�����
				std::string m_strMarr;               //����״��

				std::string m_strVeh;                //���ƺ�
				std::string m_strPoli;               //������ò
				std::string m_strProf;               //ְҵ
				std::string m_strWorkaddr;           //������ַ

				std::string m_strBirthday;           //��������
				std::string m_strMobile;             //�ֻ�����
				std::string m_strChname;             //��������
				std::string m_strHplaceArea;         //������ַ�������� HPLACEAREAm_strHplaceAreaCode
				std::string m_strHplaceAreaCode;     //������ַ��������code
				std::string m_strPhoto;              //ͼƬ 

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


		//�����밸���ӿ� �� ��������ӿ�
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
			std::string m_strCode;						// 200 Ϊ�ɹ�
			std::string m_strMessage;					// success
			std::string m_strqtime;

			struct Results
			{
			public:
				std::string m_strDisPlace;			    // ���ֵص�
				std::string m_strCaseNo;				// �������
				std::string m_strVehPlateNum;			// ���ƺ�
				std::string m_strVehLicTcode;           // ��������������

				std::string m_caseClasCode;				//��������
				std::string m_repoCaseTime;				//����ʱ��
				std::string m_caseName;					//��������
				std::string m_mainCaseCaseNo;			//�Ӱ��������
				std::string m_casClaName;				//�������
				std::string m_repoCaseUnitAddrName;		//������λ��ַ����
				std::string m_accepUnitPsag;			//����λ
				std::string m_discTime;					//���ְ��¼�ʱ��
				std::string m_minorCaseCaseNo;			//�Ӱ��������
				std::string m_caseClassCode;			//�������ʹ���
				std::string m_caseNo;					//�������
				std::string m_caseOrgaPsag;				//�참��λ
				std::string m_caseDate;					//��������
				std::string m_accepTime;				//����ʱ��
				std::string m_caseTimeTimper;			//����ʱ���
				std::string m_caseTypeCode;				//����������
				std::string m_crimPurpDesc;				//����Ŀ������
				std::string m_caseSourDesc;				//������Դ
				std::string m_caseAddrAddrCode;			//�����ص�ַ����
				std::string m_caseOrgaPsagCode;			//�참��λ����
				std::string m_arrsceTime;				//�����ֳ�ʱ��
				std::string m_setLawsDate;				//�᰸����
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
				std::string m_strAddrName;			    // ��ַ���� or ��λ��ַ
				std::string m_strLat;				    // ά��
				std::string m_strNormaLat;			    // ��һ������γ��
				std::string m_strLon;                   // ����
				std::string m_strInfoDeleJudgeFlag;	    // �Ƿ�ɾ��
				std::string m_strNormaLon;              // ��һ�����򾭶�
				std::string m_strCompName;			    // ��λ����
				std::string m_strUnitTcode;				// ��λ������
				std::string m_strUnitTname;			    // ��λ���
				std::string m_strOwnPolStatPsagCode;    // �����ɳ�������
				std::string m_strOwnPolStatPsag;		// �����ɳ���
			};
		public:
			CHeader m_oHeader;
			std::string m_strCode;						// 200 Ϊ�ɹ�
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

