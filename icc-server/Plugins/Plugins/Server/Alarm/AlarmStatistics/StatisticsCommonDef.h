#ifndef __Statistics_CommonDef_H__
#define __Statistics_CommonDef_H__

#include <string>

const std::string STATISTICS_TYPE_RECEIVE_ALARM_CALL              = "S0001";      //电话接警统计
const std::string STATISTICS_TYPE_RECEIVE_ALARM_RATE              = "S0002";      //接警率统计
const std::string STATISTICS_TYPE_RECEIVE_ALARM_RATE_RECEIVECOUNT = "S0002001";   //接警率统计--接警量
const std::string STATISTICS_TYPE_RECEIVE_ALARM_RATE_CALLINCOUNT  = "S0002002";   //接警率统计--呼入总量
const std::string STATISTICS_TYPE_AUXILIARY_ALARM                 = "S0003";      //辅助接警量统计
const std::string STATISTICS_TYPE_VALID_ALARM                     = "S0004";      //有效接警量统计
const std::string STATISTICS_TYPE_REAL_ALARM_RATE                 = "S0005";      //真报比例
const std::string STATISTICS_TYPE_REAL_ALARM_RATE_REALCOUNT       = "S0005001";   //真报比例-真实警情
const std::string STATISTICS_TYPE_REAL_ALARM_RATE_RECEIVECOUNT    = "S0005002";   //真报比例-接收警情
const std::string STATISTICS_TYPE_PROCESS_ALARM_TOTAL             = "S0006";      //处警总量
const std::string STATISTICS_TYPE_FEEDBACK_ALARM_TOTAL            = "S0007";      //反馈总量
const std::string STATISTICS_TYPE_FIRSTTYPE_ALARM_COUNT           = "S0008001";   //各类警情统计---一级警情
const std::string STATISTICS_TYPE_SECONDTYPE_ALARM_COUNT          = "S0008002";   //各类警情统计---二级警情
const std::string STATISTICS_TYPE_THIRDTYPE_ALARM_COUNT           = "S0008003";   //各类警情统计---三级警情
const std::string STATISTICS_TYPE_FOURTHTYPE_ALARM_COUNT          = "S0008004";   //各类警情统计---四级警情
const std::string STATISTICS_TYPE_ALARM_RESULT_PROCESS            = "S0009001";   //按警情结果统计---已处警
const std::string STATISTICS_TYPE_ALARM_RESULT_FEEDBACK           = "S0009002";   //按警情结果统计---已反馈
const std::string STATISTICS_TYPE_ALARM_PROCESS_FEEDBACK          = "S0010";      //统计处警/反馈量
const std::string STATISTICS_TYPE_ALARM_PROCESS_FEEDBACK_PROCESS  = "S0010001";   //统计处警/反馈量--处警量
const std::string STATISTICS_TYPE_ALARM_PROCESS_FEEDBACK_FEEDBACK = "S0010002";   //统计处警/反馈量--反馈量
const std::string STATISTICS_TYPE_ALARM_AREA                      = "S0011";      //按区域统计
const std::string STATISTICS_TYPE_ALARM_AREA_AND_FIRSTTYPE        = "S0011001";   //按区域和一级警情
const std::string STATISTICS_TYPE_ALARM_AREA_AND_SECONDTYPE       = "S0011002";   //按区域和二级警情
const std::string STATISTICS_TYPE_ALARM_AREA_AND_THIRDTYPE        = "S0011003";   //按区域和三级警情
const std::string STATISTICS_TYPE_ALARM_AREA_AND_FOOURTHTYPE      = "S0011004";   //按区域和四级警情
const std::string STATISTICS_TYPE_RECEIVE_ALARM_BY_AREA           = "S0012001";   //按区域统计处警-已签收
const std::string STATISTICS_TYPE_FEEDBACK_ALARM_BY_AREA          = "S0012002";   //按区域统计处警-已反馈
const std::string STATISTICS_TYPE_DISPATCH_ALARM_BY_AREA          = "S0012003";   //按区域统计处警-已下达
const std::string STATISTICS_TYPE_TOTAL_CALL_IN                   = "S0013";      //呼入总量
const std::string STATISTICS_TYPE_TOTAL_110                       = "S0014001";   //呼入110数量     
const std::string STATISTICS_TYPE_TOTAL_119                       = "S0014002";   //呼入119数量    
const std::string STATISTICS_TYPE_TOTAL_122                       = "S0014003";   //呼入122数量    
const std::string STATISTICS_TYPE_NATURE_RECEIVE_CALL             = "S0015001";   //按性质---受理呼叫   
const std::string STATISTICS_TYPE_NATURE_RELEASE_CALL             = "S0015002";   //按性质---早释电话    
const std::string STATISTICS_TYPE_NATURE_DISTURBANCE_CALL         = "S0015003";   //按性质---骚扰电话   
const std::string STATISTICS_TYPE_NATURE_INSIDE_CALL              = "S0015004";   //按性质---内部电话  
const std::string STATISTICS_TYPE_QUEUE_CALL                      = "S0016";      //排队统计  
const std::string STATISTICS_TYPE_SYNTHESIZE_CALLEVENT            = "S0017001";   //综合分析---话务量 
const std::string STATISTICS_TYPE_SYNTHESIZE_CALLTIME             = "S0017002";   //综合分析---通话时间 
const std::string STATISTICS_TYPE_SYNTHESIZE_CALLEVENT_1          = "S0017003001";//综合分析---按话务类型统计，事务 
const std::string STATISTICS_TYPE_SYNTHESIZE_CALLEVENT_2          = "S0017003002";//综合分析---按话务类型统计，勤务
const std::string STATISTICS_TYPE_SYNTHESIZE_CALLEVENT_3          = "S0017003003";//综合分析---按话务类型统计，领导交办
const std::string STATISTICS_TYPE_SYNTHESIZE_SEATNO               = "S0017004";   //综合分析---按席位号统计 


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