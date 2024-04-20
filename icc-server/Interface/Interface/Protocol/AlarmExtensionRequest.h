#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include "Protocol/CHeader.h"


namespace ICC
{
	namespace PROTOCOL
	{
		// �������޸ĽӾ�Ԥ��
		class CNotifiAddOrUpdateAlarmPlanRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strcreateUserId = p_pJson->GetNodeValue("/createuserid", "");
				m_oBody.m_strcreateUserName = p_pJson->GetNodeValue("/createusername", "");
				m_oBody.m_strcreateOrgCode = p_pJson->GetNodeValue("/createorgcode", "");
				m_oBody.m_strcreateOrgName = p_pJson->GetNodeValue("/createorgname", "");
				m_oBody.m_strcreateTime = p_pJson->GetNodeValue("/createtime", "");
				m_oBody.m_strupdateUserId = p_pJson->GetNodeValue("/updateuserid", "");
				m_oBody.m_strupdateUserName = p_pJson->GetNodeValue("/updateusername", "");
				m_oBody.m_strupdateOrgCode = p_pJson->GetNodeValue("/updateorgcode", "");
				m_oBody.m_strupdateOrgName = p_pJson->GetNodeValue("/updateorgname", "");
				m_oBody.m_strupdatetime = p_pJson->GetNodeValue("/updatetime", "");
				m_oBody.m_strvalidity = p_pJson->GetNodeValue("/validity", "");

				m_oBody.m_strkeyWord = p_pJson->GetNodeValue("/baseInfo/keyword", "");
				m_oBody.m_strplantName = p_pJson->GetNodeValue("/baseInfo/plantname", "");
				m_oBody.m_strbaseId = p_pJson->GetNodeValue("/baseInfo/baseId", "");
				m_oBody.m_strenableCode = p_pJson->GetNodeValue("/baseInfo/enablecode", "");
				m_oBody.m_strenableName = p_pJson->GetNodeValue("/baseInfo/enablename", "");

				int uCount = p_pJson->GetCount("/baseInfo/alarmLabels");
				for (int i = 0; i < uCount; ++i)
				{
					std::string l_strPrefixPath("/baseInfo/alarmLabels/" + std::to_string(i));
					m_oBody.m_keyValuePair.code = p_pJson->GetNodeValue(l_strPrefixPath + "/name", "");
					m_oBody.m_keyValuePair.name = p_pJson->GetNodeValue(l_strPrefixPath + "/code", "");
					m_oBody.m_vecalarmLabels.push_back(m_oBody.m_keyValuePair);
				}

				uCount = p_pJson->GetCount("/baseInfo/alarmLevels");
				for (int i = 0; i < uCount; ++i)
				{
					std::string l_strPrefixPath("/baseInfo/alarmLevels/" + std::to_string(i));
					m_oBody.m_keyValuePair.code = p_pJson->GetNodeValue(l_strPrefixPath + "/name", "");
					m_oBody.m_keyValuePair.name = p_pJson->GetNodeValue(l_strPrefixPath + "/code", "");
					m_oBody.m_vecalarmLevels.push_back(m_oBody.m_keyValuePair);
				}

				uCount = p_pJson->GetCount("/baseInfo/alarmTypes");
				for (int i = 0; i < uCount; ++i)
				{
					std::string l_strPrefixPath("/baseInfo/alarmTypes/" + std::to_string(i));
					m_oBody.m_keyValuePair.code = p_pJson->GetNodeValue(l_strPrefixPath + "/name", "");
					m_oBody.m_keyValuePair.name = p_pJson->GetNodeValue(l_strPrefixPath + "/code", "");
					m_oBody.m_vecalarmTypes.push_back(m_oBody.m_keyValuePair);
				}

				uCount = p_pJson->GetCount("/details");
				for (int i = 0; i < uCount; ++i)
				{
					std::string l_strPrefixPath("/details/" + std::to_string(i));
					m_oBody.m_planDetail.m_strdetailId = p_pJson->GetNodeValue(l_strPrefixPath + "/detailId", "");
					m_oBody.m_planDetail.m_strtype = p_pJson->GetNodeValue(l_strPrefixPath + "/type", "");
					m_oBody.m_planDetail.m_strtitle = p_pJson->GetNodeValue(l_strPrefixPath + "/title", "");
					m_oBody.m_planDetail.m_strdata = p_pJson->GetNodeValue(l_strPrefixPath + "/data", "");
					m_oBody.m_planDetail.m_strinputData = p_pJson->GetNodeValue(l_strPrefixPath + "/inputData", "");
					m_oBody.m_planDetail.m_strdataType = p_pJson->GetNodeValue(l_strPrefixPath + "/dataType", "");
					m_oBody.m_planDetail.m_strviewType = p_pJson->GetNodeValue(l_strPrefixPath + "/viewType", "");
					m_oBody.m_planDetail.m_strdataValid = p_pJson->GetNodeValue(l_strPrefixPath + "/dataValid", "");
					m_oBody.m_planDetail.m_strdataView = p_pJson->GetNodeValue(l_strPrefixPath + "/dataView", "");
					m_oBody.m_planDetail.m_strconfigNotePre = p_pJson->GetNodeValue(l_strPrefixPath + "/configNotePre", "");
					m_oBody.m_planDetail.m_strconfigNoteSuf = p_pJson->GetNodeValue(l_strPrefixPath + "/configNoteSuf", "");
					m_oBody.m_planDetail.m_strsort = p_pJson->GetNodeValue(l_strPrefixPath + "/sort", "");

					m_oBody.m_vecdetails.push_back(m_oBody.m_planDetail);
				}

				return true;
			}
		public:
			CHeaderEx m_oHeader;

			struct KeyValuePair
			{
				std::string code;                                    // ����
				std::string name;                                    // ����
			};

			struct PlanDetail
			{
				std::string m_strdetailId;                           // ģ��id������ʱΪ�գ��޸�ʱ����ԭ����id    
				std::string m_strtype;                               // ���ͣ�01-������ʾ��    
				std::string m_strtitle;                              // ����
				std::string m_strdata;                               // ����ֵ
				std::string m_strinputData;                          // ��д������ֵ(��ѡֵ/����ֵ/��ѡֵ)  
				std::string m_strdataType;                           // �������ͣ�String,Array,Date,DateTime�� 
				std::string m_strviewType;                           // �������ͣ�input��select�ȣ�����չʾ��ʽ    
				std::string m_strdataValid;                          // ����У����򣺱����Χ��������Χֵ�ȣ�  
				std::string m_strdataView;                           // ����չʾ��Ϣ����ʽ��
				std::string m_strconfigNotePre;                      // ������ǰ׺
				std::string m_strconfigNoteSuf;                      // �������׺
				std::string m_strsort;                               // ����
			};
			class CBody
			{
			public:

				std::string m_strenableCode;                         // ״̬���룺01-���ã�02-������
				std::string m_strenableName;                         // ״̬���� Ԥ��������Ϣ
				std::string m_strcreateUserId;                       // ������ID
				std::string m_strcreateUserName;                     // ����������
				std::string m_strcreateOrgCode;                      // ������λ����
				std::string m_strcreateOrgName;                      // ������λ����
				std::string m_strcreateTime;                         // ����ʱ��
				std::string m_strupdateUserId;                       // �޸���ID
				std::string m_strupdateUserName;                     // �޸�������
				std::string m_strupdateOrgCode;                      // �޸ĵ�λ����
				std::string m_strupdateOrgName;                      // �޸ĵ�λ����
				std::string m_strupdatetime;                         // �޸�ʱ��
				std::string m_strvalidity;                           // �Ƿ���Ч

				std::string m_strkeyWord;                            // �ؼ���
				std::string m_strplantName;                          // Ԥ������
				std::string m_strbaseId;                             // ������ϢID ����ʱΪ�գ��޸�ʱ����ԭ����id  
				std::vector<KeyValuePair>m_vecalarmTypes;            // ��������
				std::vector<KeyValuePair>m_vecalarmLevels;           // ���鼶��
				std::vector<KeyValuePair>m_vecalarmLabels;           // �����ǩ

				std::vector<PlanDetail>m_vecdetails;                 // Ԥ������

				KeyValuePair m_keyValuePair;
				PlanDetail m_planDetail;
			};

			CBody m_oBody;
		};

		// ɾ���Ӿ�Ԥ��
		class CNotifiDeleteAlarmPlanRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				int uCount = p_pJson->GetCount("/baseIds");

				for (int i = 0; i < uCount; ++i)
				{
					std::string l_strPrefixPath("/baseIds/" + std::to_string(i));
					std::string baseId = p_pJson->GetNodeValue(l_strPrefixPath, "");
					m_oBody.m_vecbaseIds.push_back(baseId);
				}
				return true;
			}
		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::vector<std::string>m_vecbaseIds;     // ������ϢID����
			};

			CBody m_oBody;
		};

		//	����-ͣ�ýӾ�Ԥ��
		class CNotifiEnableAlarmPlanRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strenableCode = p_pJson->GetNodeValue("/enablecode", "");
				m_oBody.m_strenableName = p_pJson->GetNodeValue("/enablename", "");

				int uCount = p_pJson->GetCount("/baseIds");
				for (int i = 0; i < uCount; ++i)
				{
					std::string l_strPrefixPath("/baseIds/" + std::to_string(i));
					std::string baseId = p_pJson->GetNodeValue(l_strPrefixPath, "");
					m_oBody.m_vecbaseId.push_back(baseId);
				}
				return true;
			}
		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strenableCode;             // �Ƿ����ô��룺01-���ã�02-ͣ��
				std::string m_strenableName;             // ״̬����
				std::vector<std::string>m_vecbaseId;     // ������ϢID����
			};

			CBody m_oBody;
		};
		// ���澯���Ԥ����Ϣ
		class CNotifiSaveAlarmPlanRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strId = p_pJson->GetNodeValue("/id", "");

				m_oBody.m_strcreateUserId = p_pJson->GetNodeValue("/createuserid", "");
				m_oBody.m_strcreateUserName = p_pJson->GetNodeValue("/createusername", "");
				m_oBody.m_strcreateOrgCode = p_pJson->GetNodeValue("/createorgcode", "");
				m_oBody.m_strcreateOrgName = p_pJson->GetNodeValue("/createorgname", "");
				m_oBody.m_strcreateTime = p_pJson->GetNodeValue("/createtime", "");
				m_oBody.m_strupdateUserId = p_pJson->GetNodeValue("/updateuserid", "");
				m_oBody.m_strupdateUserName = p_pJson->GetNodeValue("/updateusername", "");
				m_oBody.m_strupdateOrgCode = p_pJson->GetNodeValue("/updateorgcode", "");
				m_oBody.m_strupdateOrgName = p_pJson->GetNodeValue("/updateorgname", "");
				m_oBody.m_strupdatetime = p_pJson->GetNodeValue("/updatetime", "");
				m_oBody.m_strvalidity = p_pJson->GetNodeValue("/validity", "");

				m_oBody.m_stralarmId = p_pJson->GetNodeValue("/alarmId", "");
				m_oBody.m_stralarmNum = p_pJson->GetNodeValue("/alarmNum", "");
				m_oBody.m_strkeyWord = p_pJson->GetNodeValue("/keyWord", "");
				m_oBody.m_strplanId = p_pJson->GetNodeValue("/planId", "");

				m_oBody.m_stralarmLevel = p_pJson->GetNodeValue("/alarmLevel", "");
				m_oBody.m_stralarmType = p_pJson->GetNodeValue("/alarmType", "");
				m_oBody.m_strplanData = p_pJson->GetNodeValue("/planData", "");

				return true;
			}
		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:

				std::string m_strId;                                 // id
				std::string m_strcreateUserId;                       // ������ID
				std::string m_strcreateUserName;                     // ����������
				std::string m_strcreateOrgCode;                      // ������λ����
				std::string m_strcreateOrgName;                      // ������λ����
				std::string m_strcreateTime;                         // ����ʱ��
				std::string m_strupdateUserId;                       // �޸���ID
				std::string m_strupdateUserName;                     // �޸�������
				std::string m_strupdateOrgCode;                      // �޸ĵ�λ����
				std::string m_strupdateOrgName;                      // �޸ĵ�λ����
				std::string m_strupdatetime;                         // �޸�ʱ��
				std::string m_strvalidity;                           // �Ƿ���Ч

				std::string m_stralarmId;                            // ����id
				std::string m_stralarmNum;                           // ������                 
				std::string m_strkeyWord;                            // �ؼ���
				std::string m_strplanId;                             // Ԥ��id

				std::string m_stralarmLevel;                         // ���鼶��(key-value��ʽjson�ַ���)
				std::string m_stralarmType;                          // ��������(key-value��ʽjson�ַ���)
				std::string m_strplanData;                           // Ԥ��������Ϣ(����ģ����Ϣ,��д��Ϣ)
			}; 
			CBody m_oBody;
		};
		// ��ѯ�����Ԥ����Ϣ
		class CNotifiSearchAlarmPlanRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_stralarmId = p_pJson->GetNodeValue("/alarmId", "");
				return true;
			}
		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_stralarmId;                           // �Ӿ�����
			};

			CBody m_oBody;
		};

		// ��ѯ����Ԥ��
		class CNotifiSearchCommonUsePlanRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strorgCode = p_pJson->GetNodeValue("/orgCode", "");
				return true;
			}
		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strorgCode;                           // ��λ����
			};

			CBody m_oBody;
		};

		// ��ѯԤ���б� 
		class CNotifiSearchPlanListRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strenableCode = p_pJson->GetNodeValue("/enablecode", "");
				m_oBody.m_strkeyword = p_pJson->GetNodeValue("/keyword", "");
				m_oBody.m_strcurrent = p_pJson->GetNodeValue("/current", "");
				m_oBody.m_strsize = p_pJson->GetNodeValue("/size", "");

				int uCount = p_pJson->GetCount("/alarmLevelCodes");

				for (int i = 0; i < uCount; ++i)
				{
					std::string l_strPrefixPath("/alarmLevelCodes/" + std::to_string(i));
					std::string l_stralarmLevelCodes = p_pJson->GetNodeValue(l_strPrefixPath, "");
					m_oBody.m_vecalarmLevelCodes.push_back(l_stralarmLevelCodes);
				}

				uCount = p_pJson->GetCount("/alarmTypeCodes");
				for (int i = 0; i < uCount; ++i)
				{
					std::string l_strPrefixPath("/alarmTypeCodes/" + std::to_string(i));
					std::string l_stralarmTypeCodes = p_pJson->GetNodeValue(l_strPrefixPath, "");
					m_oBody.m_vecalarmTypeCodes.push_back(l_stralarmTypeCodes);
				}

				return true;
			}
		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strenableCode;                        // �Ƿ����ô��룺01-���ã�02-ͣ��
				std::string m_strkeyword;                           // �ؼ���
				std::string m_strcurrent;                           // ҳ��,Ĭ��Ϊ1,ʾ��ֵ(1)
				std::string m_strsize;                              // ÿҳ����,Ĭ��Ϊ20,ʾ��ֵ(20)
				std::vector<std::string>m_vecalarmLevelCodes;       // ���鼶����뼯��
				std::vector<std::string>m_vecalarmTypeCodes;        // �������ʹ��뼯��
			};

			CBody m_oBody;
		};
		// ��ѯԤ������
		class CNotifiSearchPlanInfoRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strbaseId = p_pJson->GetNodeValue("/baseId", "");
				return true;
			}
		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strbaseId;                           // ������ϢID
			};

			CBody m_oBody;
		};

	}

}

