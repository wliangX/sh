/*
�����������Ϣ����  ����
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
			std::string m_strexpireAt;   //����ʱ��
		};

		class userTokenId
		{
		public:
			std::string m_struserToken;
			std::string m_strexpireAt;   //����ʱ��
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
				std::string m_strdomplace;  //�����˺���
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
		//�Ĵ�ʡ��������밸����ϵ��ѯ
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

		//�Ĵ�ʡ��ͨΥ����¼���ѯ����
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
				Results results;
			};

			Data m_Data;
		};
		//������Ϣ��ѯ
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

		//4.10	�Ĵ�ʡ��������뻧����ϵ��ѯ����
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
				std::string m_predict;			//���Ŷ�
				std::string m_relType;			//���¹�ϵ����
				std::string m_certNum;			//֤������
				std::string m_domicNum;			//����
				std::string m_houHeadRel;		//�뻧����ϵ
				std::string m_domicAddr;		//������ַ
				std::string m_domicAdmDiv;		//������������
				std::string m_lastTime;         //�޸�ʱ��
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
		//�����밸���ӿ�
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
					std::string m_strDisPlace;			    // ���ֵص�
					std::string m_strCaseNo;				// �������
					std::string m_strVehPlateNum;			// ���ƺ�
					std::string m_strVehLicTcode;           // ��������������
				};
			public:
				std::string m_strCount;					   // ����
				std::vector<CList> m_vecLists;			   // ���
			};
		public:
			CHeader m_oHeader;
			std::string m_strCode;						  // 200 Ϊ�ɹ�
			std::string m_strMessage;					  // success
			CData m_strData;							  // ����
		};

		// ȫ���˿���Ƭ��ѯ����
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
					std::string m_strName;                    // ����    (XM)
					std::string m_strPhoto;                   // ��Ƭ    (zp)
					std::string m_strIdCard;                  // ���֤��    (sfzh)
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
		//4.9	�Ĵ�ʡ������������Ϣ��ѯ����
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

		//�Ĵ�ʡ���Ⱥ����Ա��Ϣ��ѯ����
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
				Results results;
			};

			Data m_Data;
		};

		//4.13	�Ĵ�ʡ������Ա������Ϣ��ѯ����
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
				Results results;
			};

			Data m_Data;
		};

		// ��Ա��ǩ
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
			std::string m_strCode;						   // 200 Ϊ�ɹ�
			std::string m_strMessage;					   // success
			std::vector<CData> m_strData;				   // ����
			std::string m_strCount;                        // ����
			std::string m_strErrcode;
			std::string m_strState;

			std::vector<std::string> m_vecAttentionTag;    // �ص���Ա�ӿڱ�ǩת��Ϊ��ע��Ա
			bool m_bTibetanRespond;						   // ���
			bool m_bDrugRespond;						   // ����
			bool m_bEscapeRespond;						   // ����
			bool m_bPersonKeyRespond;					   // �ص���Ա
		};

		//�Ĵ�ʡ��Ա�������Ϣ��ѯ����
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


		// aia��������Ϣ
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
					std::string m_strTagCode;			    // ��ǩ����
					std::string m_strTagName;				// �������
				};
			public:
				std::string m_strTagType;					// ��ǩ���ͱ���
				std::string m_strTagTypeName;			    // ��ǩ��������
				std::vector<CTag> m_vecTags;			    // ��ǩ
			};


			struct Results
			{
				std::string m_strEscu;                         // ����״��
				std::string m_strHomeaddr;                     // ��ͥ��ַ
				std::string m_strSex;                          // �Ա�
				std::string m_strEdudegree;                    // �Ļ��̶�
				std::string m_strDomplace;                     // ��ס��ַ

				std::string m_strBplace;                       // ������ַ
				std::string m_strNation;                       // ���� 
				std::string m_strServiceplace;                 // ��������ȡ���µ�ѧУ���ƻ��߹�����λ���ơ� 
				std::string m_strReli;                         // �ڽ�����
				std::string m_strMarr;                         // ����״��

				std::string m_strVeh;                          // ���ƺ�
				std::string m_strHplace;                       // ������ַ
				std::string m_strPoli;                         // ������ò
				std::string m_strProf;                         // ְҵ
				std::string m_strWorkaddr;                     // ������ַ

				std::string m_strIdno;                         // ֤������
				std::string m_strBirthday;                     // ��������
				std::string m_strMobile;                       // �ֻ�����
				std::string m_strChname;                       // ��������
				std::string m_strHplaceArea;                   // ������ַ������������ HPLACEAREA
				std::string m_strHplaceAreaCode;			   // ������ַ�������� code HPLACEAREA
				std::string m_strPhoto;						   // ͼƬ 
				std::vector<std::string> m_vecAttentionTag;    // �ص���Ա�ӿڱ�ǩת��Ϊ��ע��Ա
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