/**/
#pragma once
#include <Protocol/CHeader.h>
#include <PGClient/IResultSet.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CFeekBackInfo
		{
			public:
			bool ParseString(const std::string &p_strPrefix, JsonParser::IJsonPtr &p_pJson)
			{
				m_strMsgSource = p_pJson->GetNodeValue(p_strPrefix+"/msg_source", "");	
				m_strID = p_pJson->GetNodeValue(p_strPrefix+"/id", "");
				m_strReceiptDeptDistrictCode = p_pJson->GetNodeValue(p_strPrefix+"/district", "");
				m_strAlarmID = p_pJson->GetNodeValue(p_strPrefix+"/caseId", "");
				m_strProcessID = p_pJson->GetNodeValue(p_strPrefix+"/dispatchId", "");
				m_strRecordID = p_pJson->GetNodeValue(p_strPrefix+"/record", "");
				m_strFeedbackOrgCode = p_pJson->GetNodeValue(p_strPrefix+"/feedbackOrgCode", "");
				m_strFeedbackCode = p_pJson->GetNodeValue(p_strPrefix+"/feedbackCode", "");
				m_strFeedbackName = p_pJson->GetNodeValue(p_strPrefix+"/feedbackName", "");
				m_strFeedbackTime = p_pJson->GetNodeValue(p_strPrefix+"/feedbackTime", "");
				m_strForceAcceptTime = p_pJson->GetNodeValue(p_strPrefix+"/forceAcceptTime", "");
				m_strArriveTime = p_pJson->GetNodeValue(p_strPrefix+"/arriveTime", "");
				m_strSiteDoneTime = p_pJson->GetNodeValue(p_strPrefix+"/siteDoneTime", "");
				m_strCaseType = p_pJson->GetNodeValue(p_strPrefix+"/caseType", "");
				m_strAlarmSecondType = p_pJson->GetNodeValue(p_strPrefix+"/case2ndType", "");
				m_strAlarmThirdType = p_pJson->GetNodeValue(p_strPrefix+"/case3rdType", "");
				m_strHappenTime = p_pJson->GetNodeValue(p_strPrefix+"/happenTime", "");
				m_strAlarmLongitude = p_pJson->GetNodeValue(p_strPrefix+"/longitude", "");
				m_strAlarmLatitude = p_pJson->GetNodeValue(p_strPrefix+"/latitude", "");
				m_strDisposalDec = p_pJson->GetNodeValue(p_strPrefix+"/disposalDec", "");
				m_strDispatcheVehicles = p_pJson->GetNodeValue(p_strPrefix+"/dispatcheVehicles", "");
				m_strDispatchePeople = p_pJson->GetNodeValue(p_strPrefix+"/dispatchePeople", "");
				m_strRescuers = p_pJson->GetNodeValue(p_strPrefix+"/rescuers", "");
				m_strRescuersDes = p_pJson->GetNodeValue(p_strPrefix+"/rescuersDes", "");
				m_strIinjured = p_pJson->GetNodeValue(p_strPrefix+"/injured", "");
				m_strInjuredDes = p_pJson->GetNodeValue(p_strPrefix+"/injuredDes", "");
				m_strDead = p_pJson->GetNodeValue(p_strPrefix+"/dead", "");
				m_strDeadDes = p_pJson->GetNodeValue(p_strPrefix+"/deadDes", "");
				m_strResultCode = p_pJson->GetNodeValue(p_strPrefix+"/resultCode", "");
				m_strResultDes = p_pJson->GetNodeValue(p_strPrefix+"/resultDes", "");
				m_strWeatherCode = p_pJson->GetNodeValue(p_strPrefix+"/weatherCode", "");
				m_strLossDes = p_pJson->GetNodeValue(p_strPrefix+"/lossDes", "");
				m_strCaught = p_pJson->GetNodeValue(p_strPrefix+"/caught", "");
				m_strInvolved = p_pJson->GetNodeValue(p_strPrefix+"/involved", "");
				m_strEscapees = p_pJson->GetNodeValue(p_strPrefix+"/escapees", "");
				m_strTrafficAccidentCode = p_pJson->GetNodeValue(p_strPrefix+"/trafficAccidentCode", "");
				m_strIsCarryDangerousGoods = p_pJson->GetNodeValue(p_strPrefix+"/isCarryDangerousGoods", "");
				m_strTrafficAccidentCauseCode = p_pJson->GetNodeValue(p_strPrefix+"/trafficAccidentCauseCode", "");
				m_strVisibilityCode = p_pJson->GetNodeValue(p_strPrefix+"/visibilityCode", "");
				m_strRoadConditionCode = p_pJson->GetNodeValue(p_strPrefix+"/roadConditionCode", "");
				m_strDamagedVehicles = p_pJson->GetNodeValue(p_strPrefix+"/damagedVehicles", "");
				m_strDamagedNonMotorVehicles = p_pJson->GetNodeValue(p_strPrefix+"/damagedNonMotorVehicles", "");
				m_strRoadTypeCode = p_pJson->GetNodeValue(p_strPrefix+"/roadTypeCode", "");
				m_strStatus = p_pJson->GetNodeValue(p_strPrefix+"/status", "");
				m_strCreateTime = p_pJson->GetNodeValue(p_strPrefix+"/createTime", "");
				m_strUpdateTime = p_pJson->GetNodeValue(p_strPrefix+"/updateTime", "");
				m_strTownShip = p_pJson->GetNodeValue(p_strPrefix+"/townShip", "");
				m_strTerritorialVillage = p_pJson->GetNodeValue(p_strPrefix+"/territorialVillage", "");
				m_strIsEvidence = p_pJson->GetNodeValue(p_strPrefix+"/isEvidence", "");
				m_strTransferOrgCode = p_pJson->GetNodeValue(p_strPrefix+"/transferOrgCode", "");
				m_strTransferOrgContacts = p_pJson->GetNodeValue(p_strPrefix+"/transferOrgContacts", "");
				m_strTransferOrgFeedbackDesCode = p_pJson->GetNodeValue(p_strPrefix+"/transferOrgFeedbackDesCode", "");
				m_strTransferOrgFeedbackDes = p_pJson->GetNodeValue(p_strPrefix+"/transferOrgFeedbackDes", "");
				m_strCaseNumber = p_pJson->GetNodeValue(p_strPrefix+"/caseNumber", "");
				m_strLable = p_pJson->GetNodeValue(p_strPrefix+"/lable", "");
				m_strOccurredArea = p_pJson->GetNodeValue(p_strPrefix+"/occurredArea", "");
				m_strLocationCode = p_pJson->GetNodeValue(p_strPrefix+"/locationCode", "");
				m_strForceId = p_pJson->GetNodeValue(p_strPrefix+"/forceId", "");
				m_strPoliceType = p_pJson->GetNodeValue(p_strPrefix+"/policeType", "");
				m_strVersion = p_pJson->GetNodeValue(p_strPrefix+"/version", "");
				m_strBllStatus = p_pJson->GetNodeValue(p_strPrefix+"/bllStatus", "");
				m_strFeedbackOrgIdentifier = p_pJson->GetNodeValue(p_strPrefix+"/feedbackOrgIdentifier", "");
				m_strFeedbackOrgId = p_pJson->GetNodeValue(p_strPrefix+"/feedbackOrgId", "");
				m_strFeedbackOrgName = p_pJson->GetNodeValue(p_strPrefix+"/feedbackOrgName", "");
				m_strLiveStatus = p_pJson->GetNodeValue(p_strPrefix+"/liveStatus", "");
				m_strCase4thType = p_pJson->GetNodeValue(p_strPrefix+"/case4thType", "");
				m_strUpdateOrgIdentifier = p_pJson->GetNodeValue(p_strPrefix+"/updateOrgIdentifier", "");
				m_strTimeOutStatus = p_pJson->GetNodeValue(p_strPrefix+"/timeOutStatus", "");
				m_strLaunchBoat = p_pJson->GetNodeValue(p_strPrefix+"/launchBoat", "");
				m_strAlarmAddress = p_pJson->GetNodeValue(p_strPrefix+"/alarmAddress", "");
				m_strAddress = p_pJson->GetNodeValue(p_strPrefix+"/address", "");
				m_strIsInvaild = p_pJson->GetNodeValue(p_strPrefix+"/isInvaild", "");
				m_strTransferOrgName = p_pJson->GetNodeValue(p_strPrefix+"/transferOrgName", "");
				m_strTransferOrgIdentifier = p_pJson->GetNodeValue(p_strPrefix+"/transferOrgIdentifier", "");
				m_strTransferOrgContactsId = p_pJson->GetNodeValue(p_strPrefix+"/transferOrgContactsId", "");
				m_strModifyAreaOrgId = p_pJson->GetNodeValue(p_strPrefix+"/modifyAreaOrgId", "");
				m_strModifyAreaOrgName = p_pJson->GetNodeValue(p_strPrefix+"/modifyAreaOrgName", "");
				m_strModifyAreaOrgIdentifier = p_pJson->GetNodeValue(p_strPrefix+"/modifyAreaOrgIdentifier", "");
				m_strTransferOrgId = p_pJson->GetNodeValue(p_strPrefix+"/transferOrgId", "");
				m_strReceivedTime = p_pJson->GetNodeValue(p_strPrefix + "/received_time", "");
				m_strDistrictName = p_pJson->GetNodeValue(p_strPrefix + "/district_name", "");
				return true;
			}
			
			void ComJson(const std::string& p_strPrefix, JsonParser::IJsonPtr& p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return;
				}
				p_pJson->SetNodeValue(p_strPrefix + "/msg_source", m_strMsgSource);
				p_pJson->SetNodeValue(p_strPrefix + "/id", m_strID);
				p_pJson->SetNodeValue(p_strPrefix + "/destrict", m_strReceiptDeptDistrictCode);
				p_pJson->SetNodeValue(p_strPrefix + "/caseId", m_strAlarmID);
				p_pJson->SetNodeValue(p_strPrefix + "/dispatchId", m_strProcessID);
				p_pJson->SetNodeValue(p_strPrefix + "/record", m_strRecordID);
				p_pJson->SetNodeValue(p_strPrefix + "/feedbackOrgCode", m_strFeedbackOrgCode);
				p_pJson->SetNodeValue(p_strPrefix + "/feedbackCode", m_strFeedbackCode);
				p_pJson->SetNodeValue(p_strPrefix + "/feedbackName", m_strFeedbackName);
				p_pJson->SetNodeValue(p_strPrefix + "/feedbackTime", m_strFeedbackTime);
				p_pJson->SetNodeValue(p_strPrefix + "/forceAcceptTime", m_strForceAcceptTime);
				p_pJson->SetNodeValue(p_strPrefix + "/arriveTime", m_strArriveTime);
				p_pJson->SetNodeValue(p_strPrefix + "/siteDoneTime", m_strSiteDoneTime);
				p_pJson->SetNodeValue(p_strPrefix + "/caseType", m_strCaseType);
				p_pJson->SetNodeValue(p_strPrefix + "/case2ndType", m_strAlarmSecondType);
				p_pJson->SetNodeValue(p_strPrefix + "/case3rdType", m_strAlarmThirdType);
				p_pJson->SetNodeValue(p_strPrefix + "/happenTime", m_strHappenTime);
				p_pJson->SetNodeValue(p_strPrefix + "/longitude", m_strAlarmLongitude);
				p_pJson->SetNodeValue(p_strPrefix + "/latitude", m_strAlarmLatitude);
				p_pJson->SetNodeValue(p_strPrefix + "/disposalDec", m_strDisposalDec);
				p_pJson->SetNodeValue(p_strPrefix + "/dispatcheVehicles", m_strDispatcheVehicles);
				p_pJson->SetNodeValue(p_strPrefix + "/dispatchePeople", m_strDispatchePeople);
				p_pJson->SetNodeValue(p_strPrefix + "/rescuers", m_strRescuers);
				p_pJson->SetNodeValue(p_strPrefix + "/rescuersDes", m_strRescuersDes);
				p_pJson->SetNodeValue(p_strPrefix + "/injured", m_strIinjured);
				p_pJson->SetNodeValue(p_strPrefix + "/injuredDes", m_strInjuredDes);
				p_pJson->SetNodeValue(p_strPrefix + "/dead", m_strDead);
				p_pJson->SetNodeValue(p_strPrefix + "/deadDes", m_strDeadDes);
				p_pJson->SetNodeValue(p_strPrefix + "/resultCode", m_strResultCode);
				p_pJson->SetNodeValue(p_strPrefix + "/resultDes", m_strResultDes);
				p_pJson->SetNodeValue(p_strPrefix + "/weatherCode", m_strWeatherCode);
				p_pJson->SetNodeValue(p_strPrefix + "/lossDes", m_strLossDes);
				p_pJson->SetNodeValue(p_strPrefix + "/caught", m_strCaught);
				p_pJson->SetNodeValue(p_strPrefix + "/involved", m_strInvolved);
				p_pJson->SetNodeValue(p_strPrefix + "/escapees", m_strEscapees);
				p_pJson->SetNodeValue(p_strPrefix + "/trafficAccidentCode", m_strTrafficAccidentCode);
				p_pJson->SetNodeValue(p_strPrefix + "/isCarryDangerousGoods", m_strIsCarryDangerousGoods);
				p_pJson->SetNodeValue(p_strPrefix + "/trafficAccidentCauseCode", m_strTrafficAccidentCauseCode);
				p_pJson->SetNodeValue(p_strPrefix + "/visibilityCode", m_strVisibilityCode);
				p_pJson->SetNodeValue(p_strPrefix + "/roadConditionCode", m_strRoadConditionCode);
				p_pJson->SetNodeValue(p_strPrefix + "/damagedVehicles", m_strDamagedVehicles);
				p_pJson->SetNodeValue(p_strPrefix + "/damagedNonMotorVehicles", m_strDamagedNonMotorVehicles);
				p_pJson->SetNodeValue(p_strPrefix + "/roadTypeCode", m_strRoadTypeCode);
				p_pJson->SetNodeValue(p_strPrefix + "/status", m_strStatus);
				p_pJson->SetNodeValue(p_strPrefix + "/createTime", m_strCreateTime);
				p_pJson->SetNodeValue(p_strPrefix + "/updateTime", m_strUpdateTime);
				p_pJson->SetNodeValue(p_strPrefix + "/townShip", m_strTownShip);
				p_pJson->SetNodeValue(p_strPrefix + "/territorialVillage", m_strTerritorialVillage);
				p_pJson->SetNodeValue(p_strPrefix + "/isEvidence", m_strIsEvidence);
				p_pJson->SetNodeValue(p_strPrefix + "/transferOrgCode", m_strTransferOrgCode);
				p_pJson->SetNodeValue(p_strPrefix + "/transferOrgContacts", m_strTransferOrgContacts);
				p_pJson->SetNodeValue(p_strPrefix + "/transferOrgFeedbackDesCode", m_strTransferOrgFeedbackDesCode);
				p_pJson->SetNodeValue(p_strPrefix + "/transferOrgFeedbackDes", m_strTransferOrgFeedbackDes);
				p_pJson->SetNodeValue(p_strPrefix + "/caseNumber", m_strCaseNumber);
				p_pJson->SetNodeValue(p_strPrefix + "/lable", m_strLable);
				p_pJson->SetNodeValue(p_strPrefix + "/occurredArea", m_strOccurredArea);
				p_pJson->SetNodeValue(p_strPrefix + "/locationCode", m_strLocationCode);
				p_pJson->SetNodeValue(p_strPrefix + "/forceId", m_strForceId);
				p_pJson->SetNodeValue(p_strPrefix + "/policeType", m_strPoliceType);
				p_pJson->SetNodeValue(p_strPrefix + "/version", m_strVersion);
				p_pJson->SetNodeValue(p_strPrefix + "/bllStatus", m_strBllStatus);
				p_pJson->SetNodeValue(p_strPrefix + "/feedbackOrgIdentifier", m_strFeedbackOrgIdentifier);
				p_pJson->SetNodeValue(p_strPrefix + "/feedbackOrgId", m_strFeedbackOrgId);
				p_pJson->SetNodeValue(p_strPrefix + "/feedbackOrgName", m_strFeedbackOrgName);
				p_pJson->SetNodeValue(p_strPrefix + "/liveStatus", m_strLiveStatus);
				p_pJson->SetNodeValue(p_strPrefix + "/case4thType", m_strCase4thType);
				p_pJson->SetNodeValue(p_strPrefix + "/updateOrgIdentifier", m_strUpdateOrgIdentifier);
				p_pJson->SetNodeValue(p_strPrefix + "/timeOutStatus", m_strTimeOutStatus);
				p_pJson->SetNodeValue(p_strPrefix + "/launchBoat", m_strLaunchBoat);
				p_pJson->SetNodeValue(p_strPrefix + "/alarmAddress", m_strAlarmAddress);
				p_pJson->SetNodeValue(p_strPrefix + "/address", m_strAddress);
				p_pJson->SetNodeValue(p_strPrefix + "/isInvaild", m_strIsInvaild);
				p_pJson->SetNodeValue(p_strPrefix + "/transferOrgName", m_strTransferOrgName);
				p_pJson->SetNodeValue(p_strPrefix + "/transferOrgIdentifier", m_strTransferOrgIdentifier);
				p_pJson->SetNodeValue(p_strPrefix + "/transferOrgContactsId", m_strTransferOrgContactsId);
				p_pJson->SetNodeValue(p_strPrefix + "/modifyAreaOrgId", m_strModifyAreaOrgId);
				p_pJson->SetNodeValue(p_strPrefix + "/modifyAreaOrgName", m_strModifyAreaOrgName);
				p_pJson->SetNodeValue(p_strPrefix + "/modifyAreaOrgIdentifier", m_strModifyAreaOrgIdentifier);
				p_pJson->SetNodeValue(p_strPrefix + "/transferOrgId", m_strTransferOrgId);
				p_pJson->SetNodeValue(p_strPrefix + "/district_name", m_strDistrictName);

			}

			bool ParseFeekBackRecord(DataBase::IResultSetPtr &l_pRSet)
			{
				if (NULL == l_pRSet.get())
				{
					return false;
				}
				m_strID = l_pRSet->GetValue("id");
				//m_strMergeID = l_pRSet->GetValue("merge_id");
				return true;
			}

			public:
				std::string m_strMsgSource;
				
				std::string m_strID;	//���������
				std::string m_strReceiptDeptDistrictCode;	//������������
				std::string m_strAlarmID;	//�Ӿ������
				std::string m_strProcessID;	//�ɾ������
				std::string m_strRecordID;	//����¼����
				std::string m_strFeedbackOrgCode;	//������λ����
				std::string m_strFeedbackCode;	//����Ա���
				std::string m_strFeedbackName;	//����Ա����
				std::string m_strFeedbackTime;	//����ʱ��
				std::string m_strForceAcceptTime;	//����ʱ��
				std::string m_strArriveTime;	//�����ֳ�ʱ��
				std::string m_strSiteDoneTime;	//�ֳ��������ʱ��
				std::string m_strCaseType;	//����������
				std::string m_strAlarmSecondType;	//�������ʹ���
				std::string m_strAlarmThirdType;	//����ϸ�����
				std::string m_strHappenTime;	//���鷢��ʱ��
				std::string m_strAlarmLongitude;	//������λX���꣨�����ֳ�λ�ã�
				std::string m_strAlarmLatitude;	//������λY���꣨�����ֳ�λ�ã�
				std::string m_strDisposalDec;	//�����������
				std::string m_strDispatcheVehicles;	//��������
				std::string m_strDispatchePeople;	//�����˴�
				std::string m_strRescuers;	//��������
				std::string m_strRescuersDes;	//��������˵��
				std::string m_strIinjured;	//��������
				std::string m_strInjuredDes;	//��������˵��
				std::string m_strDead;	//��������
				std::string m_strDeadDes;	//��������˵��
				std::string m_strResultCode;	//���鴦��������
				std::string m_strResultDes;	//���鴦����˵��
				std::string m_strWeatherCode;	//�����������
				std::string m_strLossDes;	//��ʧ�������
				std::string m_strCaught;	//ץ������
				std::string m_strInvolved;	//�永����
				std::string m_strEscapees;	//��������
				std::string m_strTrafficAccidentCode;	//��ͨ�¹���̬����
				std::string m_strIsCarryDangerousGoods;	//�Ƿ�װ��Σ��Ʒ
				std::string m_strTrafficAccidentCauseCode;	//��ͨ�¹ʳ���ԭ�����
				std::string m_strVisibilityCode;	//�ܼ��ȴ���
				std::string m_strRoadConditionCode;	//·��״������
				std::string m_strDamagedVehicles;	//�𻵻�������
				std::string m_strDamagedNonMotorVehicles;	//�𻵷ǻ�������
				std::string m_strRoadTypeCode;	//��·���ʹ���
				std::string m_strStatus;	//���鴦��״̬����
				std::string m_strCreateTime;	//����ʱ��
				std::string m_strUpdateTime;	//����ʱ��
				std::string m_strTownShip;	//���򣨽ֵ���
				std::string m_strTerritorialVillage;	//���ش���
				std::string m_strIsEvidence;	//�Ƿ�ȡ֤
				std::string m_strTransferOrgCode;	//�ƽ���λ����
				std::string m_strTransferOrgContacts;	//�ƽ���λ��ϵ��
				std::string m_strTransferOrgFeedbackDesCode;	//�ƽ���λ�������
				std::string m_strTransferOrgFeedbackDes;	//�ƽ���λ�������˵��
				std::string m_strCaseNumber;	//�������
				std::string m_strLable;	//�����ǩ
				std::string m_strOccurredArea;	//���鷢������ 
				std::string m_strLocationCode;	//���鲿λ����
				std::string m_strForceId;	//����id
				std::string m_strPoliceType;	//��������
				std::string m_strVersion;	//���ͬ���汾��
				std::string m_strBllStatus;	//vcs��ǰҵ��ע
				std::string m_strFeedbackOrgIdentifier;	//������λ����
				std::string m_strFeedbackOrgId;	//������λid
				std::string m_strFeedbackOrgName;	//������λ����
				std::string m_strLiveStatus;	//�ֳ�״̬
				std::string m_strCase4thType; 	//�����������
				std::string m_strUpdateOrgIdentifier;	//�ύ��λ����
				std::string m_strTimeOutStatus;	//��ʱ״̬
				std::string m_strLaunchBoat;	//������ͧ
				std::string m_strAlarmAddress;	//������ַ
				std::string m_strAddress;	//�����ַ
				std::string m_strIsInvaild;	//�Ƿ���Ч�����ʶ
				std::string m_strTransferOrgName;	//�ƽ���λ����
				std::string m_strTransferOrgIdentifier;	//�ƽ���λ����
				std::string m_strTransferOrgContactsId;	//�ƽ���λ��ϵ��id  
				std::string m_strModifyAreaOrgId;	//�������Ͻ��λid
				std::string m_strModifyAreaOrgName;	//�������Ͻ��λ����
				std::string m_strModifyAreaOrgIdentifier;	//�������Ͻ��λ����
				std::string m_strTransferOrgId;	//�ƽ���λid
				std::string m_strReceivedTime;		// �Ӿ�ʱ��
				std::string m_strDistrictName;		// ������������
		};	
	}
}
