#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetAlarmLogRespond :
			public IRespond
		{
		public:
			CGetAlarmLogRespond() :m_bVcsSyncFlag(false)
			{

			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson){ return ""; }
			std::string ToString(JsonParser::IJsonPtr p_pJson, JsonParser::IJsonPtr p_pJsonParam, Log::ILogPtr p_pLog)
			{
				if (nullptr == p_pJson || nullptr == p_pJsonParam)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				const std::string MODULE_NAME = "synthetical";
				unsigned int l_uiIndex = 0;
				if (!m_oBody.m_strCount.empty())
				{
					p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);
				}
				else {
					if (m_bVcsSyncFlag) {
						p_pJson->SetNodeValue("/body/count", "0");
					}
				}
				
				if (!m_oBody.m_strAllCount.empty())
				{
					p_pJson->SetNodeValue("/body/all_count", m_oBody.m_strAllCount);
				}
				else {
					if (m_bVcsSyncFlag) {
						p_pJson->SetNodeValue("/body/all_count", "0");
					}
				}

			/*	if (m_oBody.m_vecData.size() == 0) {
					std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex));
					p_pJson->SetNodeValue(l_strPrefixPath, "");
				}*/

				for (CData data : m_oBody.m_vecData)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex) + "/");

					p_pJson->SetNodeValue(l_strPrefixPath + "id", data.m_strID);
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_id", data.m_strAlarmID);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_id", data.m_strProcessID);
					p_pJson->SetNodeValue(l_strPrefixPath + "feedback_id", data.m_strFeedbackID);
					p_pJson->SetNodeValue(l_strPrefixPath + "seat_no", data.m_strSeatNo);
					p_pJson->SetNodeValue(l_strPrefixPath + "operate", data.m_strOperate);
					// ��Content�Ĳ����б�ƴ��Json��
					std::string tmp_strContent(data.m_strOperateContent);
					if (m_bVcsSyncFlag)
					{
						p_pJson->SetNodeValue(l_strPrefixPath + "operate_content", data.m_strOperateContent);
					}
					else
					{
						bool bFlag = true;
						if (!p_pJsonParam->LoadJson(tmp_strContent))
						{
							tmp_strContent = "{\"param\":" + data.m_strOperateContent;
							tmp_strContent += "}";

							if (!p_pJsonParam->LoadJson(tmp_strContent))
							{
								ICC_LOG_ERROR(p_pLog, "Can not parse json: %s, json: %s", data.m_strOperateContent.c_str(), tmp_strContent.c_str());
								bFlag = false;
								//return "";
							}

						}

						if (bFlag)
						{
							int l_iParamNum = p_pJsonParam->GetCount("/param");
							for (int i = 0; i < l_iParamNum; i++)
							{
								//p_pJson->SetNodeValue(l_strPrefixPath + "content/param/" + std::to_string(i)
								p_pJson->SetNodeValue(l_strPrefixPath + "operate_content/" + std::to_string(i)
									, p_pJsonParam->GetNodeValue("/param/" + std::to_string(i), ""));
							}
						}
					}
					
					
					p_pJson->SetNodeValue(l_strPrefixPath + "from_type", data.m_strFromType);
					p_pJson->SetNodeValue(l_strPrefixPath + "from_object", data.m_strFromObject);
					p_pJson->SetNodeValue(l_strPrefixPath + "from_object_name", data.m_strFromObjectName);
					p_pJson->SetNodeValue(l_strPrefixPath + "from_object_org_name", data.m_strFromObjectOrgName);
					p_pJson->SetNodeValue(l_strPrefixPath + "from_object_org_code", data.m_strFromObjectOrgCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "to_type", data.m_strToType);
					p_pJson->SetNodeValue(l_strPrefixPath + "to_object", data.m_strToObject);
					p_pJson->SetNodeValue(l_strPrefixPath + "to_object_name", data.m_strToObjectName);
					p_pJson->SetNodeValue(l_strPrefixPath + "to_object_org_name", data.m_strToObjectOrgName);
					p_pJson->SetNodeValue(l_strPrefixPath + "to_object_org_code", data.m_strToObjectOrgCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "create_user", data.m_strCreateUser);
					p_pJson->SetNodeValue(l_strPrefixPath + "create_time", data.m_strCreateTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "dept_org_code", data.m_strDeptOrgCode);
					std::string l_strSourceName = data.m_strSourceName;
					if (l_strSourceName.empty() || l_strSourceName == "icc")
					{
						l_strSourceName = "3";
					}
					else if (l_strSourceName == "vcs")
					{
						l_strSourceName = "1";
					}
					else if (l_strSourceName == "mpa")
					{
						l_strSourceName = "2";
					}
					else
					{
						l_strSourceName = "3";
					}
					if (m_bVcsSyncFlag)
					{
						int l_nSourceName = atoi(l_strSourceName.c_str());
						p_pJson->SetNodeValue(l_strPrefixPath + "source_name", l_nSourceName);
					}
					else
					{
						p_pJson->SetNodeValue(l_strPrefixPath + "source_name", l_strSourceName);
					}
					
					
					p_pJson->SetNodeValue(l_strPrefixPath + "operate_attach_desc", data.m_strOperateAttachDesc);

					p_pJson->SetNodeValue(l_strPrefixPath + "from_object_org_identifier", data.m_strFromOrgIdentifier);
					p_pJson->SetNodeValue(l_strPrefixPath + "to_object_org_identifier", data.m_strToObjectOrgIdentifier);
					p_pJson->SetNodeValue(l_strPrefixPath + "description", data.m_strDescription);
					p_pJson->SetNodeValue(l_strPrefixPath + "create_org", data.m_strCreateOrg);
					p_pJson->SetNodeValue(l_strPrefixPath + "record_file_id", data.m_strRecordFileID);
					p_pJson->SetNodeValue(l_strPrefixPath + "received_time", data.m_strReceivedTime);
					
					++l_uiIndex;
				}

				return p_pJson->ToString();
			}

		public:
			class CData
			{
			public:
				std::string	m_strID;						//������ˮid
				std::string	m_strAlarmID;					//�Ӿ������
				std::string	m_strProcessID;					//�ɾ������
				std::string	m_strFeedbackID;			    //���������
				std::string	m_strSeatNo;					//������ϯ
				std::string	m_strOperate;				    //��������
				std::string	m_strOperateContent;			//��������
				std::string	m_strFromType;				    //����������
				std::string	m_strFromObject;			    //�����ߴ���
				std::string	m_strFromObjectName;			//����������
				std::string	m_strFromObjectOrgName;			//��������֯����
				std::string	m_strFromObjectOrgCode;			//�����ߵ�λ����
				std::string	m_strToType;				    //����������
				std::string	m_strToObject;				    //�����ߴ���
				std::string	m_strToObjectName;			    //����������
				std::string	m_strToObjectOrgName;			//��������֯����
				std::string	m_strToObjectOrgCode;			//�����ߵ�λ����
				std::string m_strCreateUser;				//������
				std::string m_strCreateTime;				//����ʱ��
				std::string m_strDeptOrgCode;				//�ɼ��ĵ�λ����
				std::string m_strSourceName;				//��־����Դ
				std::string m_strOperateAttachDesc;			//�����б�

				std::string m_strFromOrgIdentifier;         //�����ߵ�λ����
				std::string m_strToObjectOrgIdentifier;     //�����ߵ�λ����
				std::string m_strDescription;				//����
				std::string m_strCreateOrg;					//������λ
				std::string m_strRecordFileID;				//¼���ļ�ID
				std::string m_strReceivedTime;              //�Ӿ�ʱ�� received_time 
			};
			class CBody
			{
			public:
				std::string m_strCount;
				std::string m_strAllCount;
				std::vector<CData> m_vecData;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
			bool	m_bVcsSyncFlag;
		};
	}
}