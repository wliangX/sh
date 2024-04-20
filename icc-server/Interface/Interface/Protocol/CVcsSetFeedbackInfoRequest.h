#pragma once 
#include <Protocol/IRequest.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
        class CVcsSetFeedBackInfoRequest :
            public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
            {
                if (!m_oHeader.ParseString(p_strReq, p_pJson))
                {
                    return false;
                }
                m_oBody.m_strID = p_pJson->GetNodeValue("/body/id", "");
                m_oBody.m_strCaseId = p_pJson->GetNodeValue("/body/caseid", "");
                m_oBody.m_strDisPosalId = p_pJson->GetNodeValue("/body/disposalid", "");
                m_oBody.m_strCaseDispatchStatus = p_pJson->GetNodeValue("/body/casedispatchstatus", "");
                m_oBody.m_strFBWay = p_pJson->GetNodeValue("/body/fbway", "");
                m_oBody.m_strFBContent = p_pJson->GetNodeValue("/body/fbcontent", "");
				m_oBody.m_strFeedBacker = p_pJson->GetNodeValue("/body/feedbacker", "");
				m_oBody.m_strFeedBackerCode = p_pJson->GetNodeValue("/body/feedbacker_code", "");
				m_oBody.m_strFeedBackDept = p_pJson->GetNodeValue("/body/feedback_dept", "");
				m_oBody.m_strFeedBackDeptCode = p_pJson->GetNodeValue("/body/feedback_dept_code", "");
                m_oBody.m_strFBTime = p_pJson->GetNodeValue("/body/fbtime", "");
				m_oBody.m_strCaseProcessor = p_pJson->GetNodeValue("/body/case_processor", "");
				m_oBody.m_strFileType = p_pJson->GetNodeValue("/body/file_type", "");
				m_oBody.m_strFileGuid = p_pJson->GetNodeValue("/body/file_guid", "");
				m_oBody.m_strLongitude = p_pJson->GetNodeValue("/body/longitude", "");
				m_oBody.m_strLatitude = p_pJson->GetNodeValue("/body/latitude", "");
                
                return true;
            }
			
		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
                std::string m_strID;
                std::string m_strCaseId;					//警情ID
                std::string m_strDisPosalId;				//处警单ID
                std::string m_strCaseDispatchStatus;		//反馈类型                
                std::string m_strFBWay;                     //反馈方式:1终端快捷键；2短信；3电话
                std::string m_strFBContent;                 //反馈内容
				std::string m_strFeedBacker;                //反馈人
				std::string m_strFeedBackerCode;            //反馈人警号
				std::string m_strFeedBackDept;              //反馈人单位
				std::string m_strFeedBackDeptCode;
                std::string m_strFBTime;                    //反馈时间     
				std::string m_strCaseProcessor;				//0有效，1删除
				std::string m_strFileType;					//文件类型 图片0、视频1、文字2、音频3 、位置4、其他5、自定义6（预留）
				std::string m_strFileGuid;				    //文件标识
				std::string m_strLongitude;					//APP 反馈的经度
				std::string m_strLatitude;					//APP 反馈的纬度
			};
			CBody m_oBody;
		};
	}
}
