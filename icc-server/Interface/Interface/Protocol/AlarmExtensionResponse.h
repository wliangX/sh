#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/IRequest.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
		class CNotifiAlarmPlanResponse :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}
				p_pJson->SetNodeValue("/code", m_oBody.m_strcode);
				p_pJson->SetNodeValue("/message", m_oBody.m_strmessage);
				if (!m_oBody.m_vecdata.empty())
				{
					for (size_t i = 0; i < m_oBody.m_vecdata.size(); i++)
					{
						std::string l_strPrefixPath("/data/" + std::to_string(i));
						p_pJson->SetNodeValue(l_strPrefixPath , m_oBody.m_vecdata.at(i));
					}
				}
				else
				{
					p_pJson->SetNodeValue("/data", m_oBody.m_strdata);
				}
				return p_pJson->ToString();
			}


		public:
			CHeaderEx m_oHeader;


			class CBody
			{
			public:
				std::string m_strcode;                         // ������Ϣ��,Ĭ��:200
				std::string m_strdata;                         // ���ص�����(Object)
				std::string m_strmessage;                      // ������Ϣ���ݣ�Ĭ��:success
				std::vector<std::string> m_vecdata;
			};
			CBody m_oBody;
		};

		class CNotifiSaveAlarmPlanResponse :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}
				p_pJson->SetNodeValue("/code", m_oBody.m_strcode);
				p_pJson->SetNodeValue("/message", m_oBody.m_strmessage);

				p_pJson->SetNodeValue("/data/alarmId", m_oBody.m_stralarmId);
				p_pJson->SetNodeValue("/data/alarmLevel", m_oBody.m_stralarmLevel);
				p_pJson->SetNodeValue("/data/alarmNum", m_oBody.m_stralarmNum);
				p_pJson->SetNodeValue("/data/alarmType", m_oBody.m_stralarmType);
				p_pJson->SetNodeValue("/data/createOrgCode", m_oBody.m_strcreateOrgCode);
				p_pJson->SetNodeValue("/data/createOrgName", m_oBody.m_strcreateOrgName);
				p_pJson->SetNodeValue("/data/createTime", m_oBody.m_strcreateTime);
				p_pJson->SetNodeValue("/data/createUserId", m_oBody.m_strcreateUserId);
				p_pJson->SetNodeValue("/data/createUserName", m_oBody.m_strcreateUserName);
				p_pJson->SetNodeValue("/data/id", m_oBody.m_strId);

				p_pJson->SetNodeValue("/data/keyWord", m_oBody.m_strkeyWord);
				p_pJson->SetNodeValue("/data/planData", m_oBody.m_strplanData);
				p_pJson->SetNodeValue("/data/planId", m_oBody.m_strplanId);

				p_pJson->SetNodeValue("/data/updateOrgCode", m_oBody.m_strupdateOrgCode);
				p_pJson->SetNodeValue("/data/updateOrgName", m_oBody.m_strupdateOrgName);
				p_pJson->SetNodeValue("/data/updateTime", m_oBody.m_strupdatetime);

				p_pJson->SetNodeValue("/data/updateUserId", m_oBody.m_strupdateUserId);
				p_pJson->SetNodeValue("/data/updateUserName", m_oBody.m_strupdateUserName);
				p_pJson->SetNodeValue("/data/validity", m_oBody.m_strvalidity);

				return p_pJson->ToString();
			}


		public:
			CHeaderEx m_oHeader;


			class CBody
			{
			public:

				std::string m_strId;
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

				std::string m_strplanData;                           // Ԥ��������Ϣ(����ģ����Ϣ,��д��Ϣ)
				std::string m_stralarmNum;                           // ������                 
				std::string m_strkeyWord;                            // �ؼ���
				std::string m_strplanId;                             // Ԥ��id
				std::string m_stralarmType;                          // ��������
				std::string m_stralarmLevel;                         // ���鼶��
				std::string m_strcode;                               // ������Ϣ��,Ĭ��:200
				std::string m_strmessage;                            // ������Ϣ���ݣ�Ĭ��:success
				std::string m_stralarmId;                            //����id
			
				std::vector<std::string> m_vecdata;
			};
			CBody m_oBody;
		};

		// ��ѯ�����Ԥ����Ϣ
		class CNotifiSearchAlarmPlanResponse :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				p_pJson->SetNodeValue("/code", m_oBody.m_strcode);
				p_pJson->SetNodeValue("/message", m_oBody.m_strmessage);

				std::string l_strPrefixPath("/data/");
				p_pJson->SetNodeValue(l_strPrefixPath + "planId", m_oBody.m_strplanId);
				p_pJson->SetNodeValue(l_strPrefixPath + "alarmNum", m_oBody.m_stralarmNum);
				p_pJson->SetNodeValue(l_strPrefixPath + "keyWord", m_oBody.m_strkeyWord);
				p_pJson->SetNodeValue(l_strPrefixPath + "alarmId", m_oBody.m_stralarmId);

				p_pJson->SetNodeValue(l_strPrefixPath + "alarmLevel", m_oBody.m_stralarmLevel);
				p_pJson->SetNodeValue(l_strPrefixPath + "alarmType", m_oBody.m_stralarmType);
				p_pJson->SetNodeValue(l_strPrefixPath + "planData", m_oBody.m_strplanData);

				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;
	

			class CBody
			{
			public:
				std::string m_stralarmId;                       // ����id
				std::string m_stralarmNum;                      // ������
				std::string m_strplanId;                        // Ԥ��id
				std::string m_strkeyWord;                       // Ԥ���ؼ���
				std::string m_strplanName;                      // Ԥ������

				std::string m_stralarmLevel;                    // ���鼶��
				std::string m_stralarmType;                     // ��������
				std::string m_strplanData;                      // Ԥ����д��Ϣ

				std::string m_strcode;                          // ������Ϣ��,Ĭ��:200                       
				std::string m_strmessage;                       // ������Ϣ���ݣ�Ĭ��:success 
	
			};
			CBody m_oBody;
		};

		// ��ѯ����Ԥ��
		class CNotifiSearchCommonUsePlanResponse :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				p_pJson->SetNodeValue("/code", m_oBody.m_strcode);
				p_pJson->SetNodeValue("/message", m_oBody.m_strmessage);

				for (size_t i = 0; i < m_oBody.m_vecdata.size(); ++i)
				{
					std::string l_strPrefixPath("/data/" + std::to_string(i));
					p_pJson->SetNodeValue(l_strPrefixPath + "/baseId", m_oBody.m_vecdata.at(i).m_strbaseId);
					p_pJson->SetNodeValue(l_strPrefixPath + "/enableCode", m_oBody.m_vecdata.at(i).m_strenableCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "/enableName", m_oBody.m_vecdata.at(i).m_strenableName);
					p_pJson->SetNodeValue(l_strPrefixPath + "/keyWord", m_oBody.m_vecdata.at(i).m_strkeyWord);
					p_pJson->SetNodeValue(l_strPrefixPath + "/planName", m_oBody.m_vecdata.at(i).m_strplanName);

					for (size_t j = 0; j < m_oBody.m_vecdata.at(i).m_vecalarmLevelNames.size(); ++j)
					{
						std::string l_stralarmLevelNamesPath(l_strPrefixPath + "/alarmLevelNames/" + std::to_string(j));
						p_pJson->SetNodeValue(l_stralarmLevelNamesPath, m_oBody.m_vecdata.at(i).m_vecalarmLevelNames[j]);
					}

					for (size_t j = 0; j < m_oBody.m_vecdata.at(i).m_vecalarmTipTitles.size(); ++j)
					{
						std::string l_stralarmTipTitlesPath(l_strPrefixPath + "/alarmTipTitles/" + std::to_string(j));
						p_pJson->SetNodeValue(l_stralarmTipTitlesPath, m_oBody.m_vecdata.at(i).m_vecalarmTipTitles[j]);
					}

					for (size_t j = 0; j < m_oBody.m_vecdata.at(i).m_vecalarmTypeNames.size(); ++j)
					{
						std::string l_stralarmTypeNamesPath(l_strPrefixPath + "/alarmTypeNames/" + std::to_string(j));
						p_pJson->SetNodeValue(l_stralarmTypeNamesPath, m_oBody.m_vecdata.at(i).m_vecalarmTypeNames[j]);
					}
				}

				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;

			struct data
			{
				std::vector<std::string> m_vecalarmLevelNames;  // ���鼶��
				std::vector<std::string> m_vecalarmTipTitles;   // ������ʾ�����
				std::vector<std::string> m_vecalarmTypeNames;   // ������������
				std::string m_strbaseId;                        // ������Ϣid
				std::string m_strenableCode;                    // ����״̬���룺01-���ã�02-������
				std::string m_strenableName;                    // ״̬����
				std::string m_strkeyWord;                       // Ԥ���ؼ���
				std::string m_strplanName;                      // Ԥ������
			};

			class CBody
			{
			public:
				std::string m_strcode;                          // ������Ϣ��,Ĭ��:200                       
				std::string m_strmessage;                       // ������Ϣ���ݣ�Ĭ��:�����ɹ� 
				std::vector<data> m_vecdata;                   	// ���ص�����(Object)
				data m_data;                                    
			};
			CBody m_oBody;
		};

		// ��ѯԤ���б�
		class CNotifiSearchPlanListResponse :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				p_pJson->SetNodeValue("/code", m_oBody.m_strcode);
				p_pJson->SetNodeValue("/message", m_oBody.m_strmessage);

				p_pJson->SetNodeValue("/data/current", m_oBody.m_data.m_strcurrent);

				//p_pJson->SetNodeValue("/data/hitCount", m_oBody.m_data.m_strhitCount);
				p_pJson->SetNodeValue("/data/pages", m_oBody.m_data.m_strpages);
				//p_pJson->SetNodeValue("/data/searchCount", m_oBody.m_data.m_strsearchCount);
				p_pJson->SetNodeValue("/data/size", m_oBody.m_data.m_strsize);
				p_pJson->SetNodeValue("/data/total", m_oBody.m_data.m_strtotal);

				for (size_t i = 0; i < m_oBody.m_data.m_vecrecords.size(); ++i)
				{
					std::string l_strPrefixPath("/data/records/" + std::to_string(i));
					p_pJson->SetNodeValue(l_strPrefixPath + "/baseId", m_oBody.m_data.m_vecrecords.at(i).m_strbaseId);
					p_pJson->SetNodeValue(l_strPrefixPath + "/enableCode", m_oBody.m_data.m_vecrecords.at(i).m_strenableCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "/enableName", m_oBody.m_data.m_vecrecords.at(i).m_strenableName);
					p_pJson->SetNodeValue(l_strPrefixPath + "/keyWord", m_oBody.m_data.m_vecrecords.at(i).m_strkeyWord);
					p_pJson->SetNodeValue(l_strPrefixPath + "/planName", m_oBody.m_data.m_vecrecords.at(i).m_strplanName);

					for (size_t j = 0; j < m_oBody.m_data.m_vecrecords.at(i).m_vecalarmLevelNames.size(); ++j)
					{
						std::string l_stralarmLevelNamesPath(l_strPrefixPath + "/alarmLevelNames/" + std::to_string(j));
						p_pJson->SetNodeValue(l_stralarmLevelNamesPath, m_oBody.m_data.m_vecrecords.at(i).m_vecalarmLevelNames[j]);
					}

					for (size_t j = 0; j < m_oBody.m_data.m_vecrecords.at(i).m_vecalarmTipTitles.size(); ++j)
					{
						std::string l_stralarmTipTitlesPath(l_strPrefixPath + "/alarmTipTitles/" + std::to_string(j));
						p_pJson->SetNodeValue(l_stralarmTipTitlesPath, m_oBody.m_data.m_vecrecords.at(i).m_vecalarmTipTitles[j]);
					}

					for (size_t j = 0; j < m_oBody.m_data.m_vecrecords.at(i).m_vecalarmTypeNames.size(); ++j)
					{
						std::string l_stralarmTypeNamesPath(l_strPrefixPath + "/alarmTypeNames/" + std::to_string(j));
						p_pJson->SetNodeValue(l_stralarmTypeNamesPath, m_oBody.m_data.m_vecrecords.at(i).m_vecalarmTypeNames[j]);
					}
				}

				return p_pJson->ToString();
			}


		public:
			CHeaderEx m_oHeader;

			struct records
			{
				std::vector<std::string> m_vecalarmLevelNames;  // ���鼶��
				std::vector<std::string> m_vecalarmTipTitles;   // ������ʾ�����
				std::vector<std::string> m_vecalarmTypeNames;   // ������������
				std::string m_strbaseId;                        // ������Ϣid
				std::string m_strenableCode;                    // ����״̬���룺01-���ã�02-������
				std::string m_strenableName;                    // ״̬����
				std::string m_strkeyWord;                       // Ԥ���ؼ���
				std::string m_strplanName;                      // Ԥ������
			};

			struct data
			{
				std::string m_strcurrent;                       // ҳ��
				//bool m_strhitCount;                             //
				std::string m_strpages;                         // ҳ��
				//bool m_strsearchCount;                          //
				std::string m_strsize;                          // ����
				std::string m_strtotal;                         // ����
				std::vector<records> m_vecrecords;              //  Ԥ������չʾ��Ϣ
				struct records  m_records;                       
			};

			class CBody
			{
			public:
				std::string m_strcode;                          // ������Ϣ��,Ĭ��:200                       
				std::string m_strmessage;                       // ������Ϣ���ݣ�Ĭ��:�����ɹ� 
				std::vector<data> m_vecdata;                   	// ���ص�����(Object)
				data m_data;                                    
			};
			CBody m_oBody;
		};

		class CNotifiSearchPlanInfoResponse :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				p_pJson->SetNodeValue("/code", m_oBody.m_strcode);
				p_pJson->SetNodeValue("/message", m_oBody.m_strmessage);

				std::string l_strPrefixPath("/data/baseInfo/");
				p_pJson->SetNodeValue(l_strPrefixPath + "/baseId", m_oBody.m_strbaseId);
				p_pJson->SetNodeValue(l_strPrefixPath + "/keyWord", m_oBody.m_strkeyWord);
				p_pJson->SetNodeValue(l_strPrefixPath + "/plantName", m_oBody.m_strplantName);

				for (size_t i = 0; i < m_oBody.m_vecalarmTypes.size(); i++)
				{
					p_pJson->SetNodeValue(l_strPrefixPath + "/alarmTypes/code", m_oBody.m_vecalarmTypes.at(i).code);
					p_pJson->SetNodeValue(l_strPrefixPath + "/alarmTypes/name", m_oBody.m_vecalarmTypes.at(i).name);
				}

				for (size_t i = 0; i < m_oBody.m_vecalarmLevels.size(); i++)
				{
					p_pJson->SetNodeValue(l_strPrefixPath + "/alarmLevels/code", m_oBody.m_vecalarmLevels.at(i).code);
					p_pJson->SetNodeValue(l_strPrefixPath + "/alarmLevels/name", m_oBody.m_vecalarmLevels.at(i).name);
				}

				for (size_t i = 0; i < m_oBody.m_vecalarmLabels.size(); i++)
				{
					p_pJson->SetNodeValue(l_strPrefixPath + "/alarmLabels/code", m_oBody.m_vecalarmLabels.at(i).code);
					p_pJson->SetNodeValue(l_strPrefixPath + "/alarmLabels/name", m_oBody.m_vecalarmLabels.at(i).name);
				}

				for (size_t i = 0; i < m_oBody.m_vecdetails.size(); ++i)
				{
					std::string l_strDetailsPath("/data/details/" + std::to_string(i));
					p_pJson->SetNodeValue(l_strDetailsPath + "/detailId", m_oBody.m_vecdetails.at(i).m_strdetailId);
					p_pJson->SetNodeValue(l_strDetailsPath + "/type", m_oBody.m_vecdetails.at(i).m_strtype);

					p_pJson->SetNodeValue(l_strDetailsPath + "/configNotePre", m_oBody.m_vecdetails.at(i).m_strconfigNotePre);
					p_pJson->SetNodeValue(l_strDetailsPath + "/configNoteSuf", m_oBody.m_vecdetails.at(i).m_strconfigNoteSuf);

					p_pJson->SetNodeValue(l_strDetailsPath + "/data", m_oBody.m_vecdetails.at(i).m_strdata);
					p_pJson->SetNodeValue(l_strDetailsPath + "/dataType", m_oBody.m_vecdetails.at(i).m_strdataType);

					p_pJson->SetNodeValue(l_strDetailsPath + "/dataValid", m_oBody.m_vecdetails.at(i).m_strdataValid);
					p_pJson->SetNodeValue(l_strDetailsPath + "/dataView", m_oBody.m_vecdetails.at(i).m_strdataView);

					p_pJson->SetNodeValue(l_strDetailsPath + "/inputData", m_oBody.m_vecdetails.at(i).m_strinputData);
					p_pJson->SetNodeValue(l_strDetailsPath + "/sort", m_oBody.m_vecdetails.at(i).m_strsort);

					p_pJson->SetNodeValue(l_strDetailsPath + "/title", m_oBody.m_vecdetails.at(i).m_strtitle);
					p_pJson->SetNodeValue(l_strDetailsPath + "/viewType", m_oBody.m_vecdetails.at(i).m_strviewType);
				}

				return p_pJson->ToString();
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
				std::string m_strcode;                               // ������Ϣ��,Ĭ��:200                       
				std::string m_strmessage;                            // ������Ϣ���ݣ�Ĭ��:�����ɹ� 
				std::string m_strkeyWord;                            // �ؼ���
				std::string m_strplantName;                          // Ԥ������
				std::string m_strbaseId;                             // ������ϢID
				std::vector<KeyValuePair>m_vecalarmTypes;            // ��������
				std::vector<KeyValuePair>m_vecalarmLevels;           // ���鼶��
				std::vector<KeyValuePair>m_vecalarmLabels;           // �����ǩ

				std::vector<PlanDetail>m_vecdetails;                 // Ԥ������

				KeyValuePair m_keyValuePair;
				PlanDetail m_planDetail;
			};

			CBody m_oBody;
		};
	}
}
