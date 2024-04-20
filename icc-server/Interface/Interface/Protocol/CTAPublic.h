#pragma once

namespace ICC
{
	namespace PROTOCOL
	{
		class CMaterial
		{
		public:
			std::string		m_strFGuid;					//附件ID
			std::string		m_strFName;					//附件名称
			std::string		m_strAttachType;			//附件类型 0:图片，1视频，2语音
			std::string		m_strAttachPath;			//附件路径
		};

		class CThirdAlarm
		{
		public:
			std::string		m_strGuid;					//报警主键
			std::string		m_strType;					//0未知，1市局微信，2省厅微信，3短信
			std::string		m_strAddress;				//事发地址
			std::string		m_strCrcontent;				//报警内容
			std::string   	m_strIsPhone;				//是否方便接听电话
			std::string		m_strCreateDate;			//报警时间
			std::string		m_strUserCode;				//报警人身份证号码
			std::string		m_strUserName;				//报警人姓名
			std::string		m_strUserPhone;				//报警人手机
			std::string		m_strUserAddress;			//报警人联系地址
			std::string		m_strUnitContactor;			//联系人
			std::string		m_strHandPhone;				//联系电话
			std::string		m_strToOrgID;				//接受部门ID
			std::string		m_strToOrgName;				//接受部门名称
			std::string		m_strToUserID;				//处理人警号
			std::string		m_strToUserName;			//处理人姓名
			std::string		m_strState;					//状态，0未读，1已读，2已创警
			std::string		m_strComNo;					//报警编号
			std::string		m_strOpenID;				//交互窗口的编号(微信交互的主键)
			std::string		m_strLong;					//维度
			std::string		m_strLat;					//经度
			std::string		m_strNotReadChangeCount;	//未读交互信息数量
			std::string		m_strRemark1;				//备用1
			std::string		m_strRemark2;				//备用2

			std::vector<PROTOCOL::CMaterial> m_vecMaterial;
		};
	}
}
