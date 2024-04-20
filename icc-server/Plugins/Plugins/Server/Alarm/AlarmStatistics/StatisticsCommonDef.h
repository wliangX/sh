#ifndef __Statistics_CommonDef_H__
#define __Statistics_CommonDef_H__

#include <string>

const std::string STATISTICS_TYPE_RECEIVE_ALARM_CALL              = "S0001";      //�绰�Ӿ�ͳ��
const std::string STATISTICS_TYPE_RECEIVE_ALARM_RATE              = "S0002";      //�Ӿ���ͳ��
const std::string STATISTICS_TYPE_RECEIVE_ALARM_RATE_RECEIVECOUNT = "S0002001";   //�Ӿ���ͳ��--�Ӿ���
const std::string STATISTICS_TYPE_RECEIVE_ALARM_RATE_CALLINCOUNT  = "S0002002";   //�Ӿ���ͳ��--��������
const std::string STATISTICS_TYPE_AUXILIARY_ALARM                 = "S0003";      //�����Ӿ���ͳ��
const std::string STATISTICS_TYPE_VALID_ALARM                     = "S0004";      //��Ч�Ӿ���ͳ��
const std::string STATISTICS_TYPE_REAL_ALARM_RATE                 = "S0005";      //�汨����
const std::string STATISTICS_TYPE_REAL_ALARM_RATE_REALCOUNT       = "S0005001";   //�汨����-��ʵ����
const std::string STATISTICS_TYPE_REAL_ALARM_RATE_RECEIVECOUNT    = "S0005002";   //�汨����-���վ���
const std::string STATISTICS_TYPE_PROCESS_ALARM_TOTAL             = "S0006";      //��������
const std::string STATISTICS_TYPE_FEEDBACK_ALARM_TOTAL            = "S0007";      //��������
const std::string STATISTICS_TYPE_FIRSTTYPE_ALARM_COUNT           = "S0008001";   //���ྯ��ͳ��---һ������
const std::string STATISTICS_TYPE_SECONDTYPE_ALARM_COUNT          = "S0008002";   //���ྯ��ͳ��---��������
const std::string STATISTICS_TYPE_THIRDTYPE_ALARM_COUNT           = "S0008003";   //���ྯ��ͳ��---��������
const std::string STATISTICS_TYPE_FOURTHTYPE_ALARM_COUNT          = "S0008004";   //���ྯ��ͳ��---�ļ�����
const std::string STATISTICS_TYPE_ALARM_RESULT_PROCESS            = "S0009001";   //��������ͳ��---�Ѵ���
const std::string STATISTICS_TYPE_ALARM_RESULT_FEEDBACK           = "S0009002";   //��������ͳ��---�ѷ���
const std::string STATISTICS_TYPE_ALARM_PROCESS_FEEDBACK          = "S0010";      //ͳ�ƴ���/������
const std::string STATISTICS_TYPE_ALARM_PROCESS_FEEDBACK_PROCESS  = "S0010001";   //ͳ�ƴ���/������--������
const std::string STATISTICS_TYPE_ALARM_PROCESS_FEEDBACK_FEEDBACK = "S0010002";   //ͳ�ƴ���/������--������
const std::string STATISTICS_TYPE_ALARM_AREA                      = "S0011";      //������ͳ��
const std::string STATISTICS_TYPE_ALARM_AREA_AND_FIRSTTYPE        = "S0011001";   //�������һ������
const std::string STATISTICS_TYPE_ALARM_AREA_AND_SECONDTYPE       = "S0011002";   //������Ͷ�������
const std::string STATISTICS_TYPE_ALARM_AREA_AND_THIRDTYPE        = "S0011003";   //���������������
const std::string STATISTICS_TYPE_ALARM_AREA_AND_FOOURTHTYPE      = "S0011004";   //��������ļ�����
const std::string STATISTICS_TYPE_RECEIVE_ALARM_BY_AREA           = "S0012001";   //������ͳ�ƴ���-��ǩ��
const std::string STATISTICS_TYPE_FEEDBACK_ALARM_BY_AREA          = "S0012002";   //������ͳ�ƴ���-�ѷ���
const std::string STATISTICS_TYPE_DISPATCH_ALARM_BY_AREA          = "S0012003";   //������ͳ�ƴ���-���´�
const std::string STATISTICS_TYPE_TOTAL_CALL_IN                   = "S0013";      //��������
const std::string STATISTICS_TYPE_TOTAL_110                       = "S0014001";   //����110����     
const std::string STATISTICS_TYPE_TOTAL_119                       = "S0014002";   //����119����    
const std::string STATISTICS_TYPE_TOTAL_122                       = "S0014003";   //����122����    
const std::string STATISTICS_TYPE_NATURE_RECEIVE_CALL             = "S0015001";   //������---�������   
const std::string STATISTICS_TYPE_NATURE_RELEASE_CALL             = "S0015002";   //������---���͵绰    
const std::string STATISTICS_TYPE_NATURE_DISTURBANCE_CALL         = "S0015003";   //������---ɧ�ŵ绰   
const std::string STATISTICS_TYPE_NATURE_INSIDE_CALL              = "S0015004";   //������---�ڲ��绰  
const std::string STATISTICS_TYPE_QUEUE_CALL                      = "S0016";      //�Ŷ�ͳ��  
const std::string STATISTICS_TYPE_SYNTHESIZE_CALLEVENT            = "S0017001";   //�ۺϷ���---������ 
const std::string STATISTICS_TYPE_SYNTHESIZE_CALLTIME             = "S0017002";   //�ۺϷ���---ͨ��ʱ�� 
const std::string STATISTICS_TYPE_SYNTHESIZE_CALLEVENT_1          = "S0017003001";//�ۺϷ���---����������ͳ�ƣ����� 
const std::string STATISTICS_TYPE_SYNTHESIZE_CALLEVENT_2          = "S0017003002";//�ۺϷ���---����������ͳ�ƣ�����
const std::string STATISTICS_TYPE_SYNTHESIZE_CALLEVENT_3          = "S0017003003";//�ۺϷ���---����������ͳ�ƣ��쵼����
const std::string STATISTICS_TYPE_SYNTHESIZE_SEATNO               = "S0017004";   //�ۺϷ���---��ϯλ��ͳ�� 


const std::string STATISTICS_FIELD_NAME_STATISTICS_ID = "statistics_id";
const std::string STATISTICS_FIELD_NAME_STATISTICS_TYPE = "statistics_type";
const std::string STATISTICS_FIELD_NAME_STATISTICS_DATE = "statistics_date";
const std::string STATISTICS_FIELD_NAME_STATISTICS_RESULT = "statistics_result";
const std::string STATISTICS_FIELD_NAME_STATISTICS_DEPT_CODE = "dept_code";
const std::string STATISTICS_FIELD_NAME_STATISTICS_DEPT_NAME = "dept_name";
const std::string STATISTICS_FIELD_NAME_STATISTICS_TIME = "statistics_time";
const std::string STATISTICS_FIELD_NAME_STATISTICS_REMARK = "remark";

const std::string STATISTICS_PARAM_NAME_BEGIN_TIME = "begin_time";
const std::string STATISTICS_PARAM_NAME_END_TIME = "end_time";


const std::string FLAG_TRUE = "1";

#endif