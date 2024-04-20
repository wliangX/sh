#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetBJRSFXXRespond :
			public IRespond, public IReceive
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/xm", m_oBody.m_strXM);
				p_pJson->SetNodeValue("/body/sfzh", m_oBody.m_strSFZH);
				
				size_t tmp_uiDataSize = m_oBody.m_vecData.size();
				for (size_t i = 0; i < tmp_uiDataSize; ++i)
				{
					std::string tmp_strDataPrefix("/body/data/" + std::to_string(i));
					p_pJson->SetNodeValue(tmp_strDataPrefix+ "/biaoqian", m_oBody.m_vecData[i].m_strBiaoQian);
					p_pJson->SetNodeValue(tmp_strDataPrefix + "/zjlx", m_oBody.m_vecData[i].m_strZJLX);
					p_pJson->SetNodeValue(tmp_strDataPrefix + "/sfzhm", m_oBody.m_vecData[i].m_strSFZHM);	
                    p_pJson->SetNodeValue(tmp_strDataPrefix + "/cph", m_oBody.m_vecData[i].m_strCPH);
                    p_pJson->SetNodeValue(tmp_strDataPrefix + "/rylb", m_oBody.m_vecData[i].m_strRYLB);
                    p_pJson->SetNodeValue(tmp_strDataPrefix + "/ryxl", m_oBody.m_vecData[i].m_strRYXL);					
                    p_pJson->SetNodeValue(tmp_strDataPrefix + "/xm", m_oBody.m_vecData[i].m_strXM);
					p_pJson->SetNodeValue(tmp_strDataPrefix + "/xb", m_oBody.m_vecData[i].m_strXB);
					p_pJson->SetNodeValue(tmp_strDataPrefix + "/csrq", m_oBody.m_vecData[i].m_strCSRQ);	
                    p_pJson->SetNodeValue(tmp_strDataPrefix + "/mz", m_oBody.m_vecData[i].m_strMZ);
                    p_pJson->SetNodeValue(tmp_strDataPrefix + "/jgssxdm", m_oBody.m_vecData[i].m_strJGSSXDM);	
                    p_pJson->SetNodeValue(tmp_strDataPrefix + "/jggjdqdm", m_oBody.m_vecData[i].m_strJGGJDQDM);
					p_pJson->SetNodeValue(tmp_strDataPrefix + "/hjdqh", m_oBody.m_vecData[i].m_strHJDQH);
					p_pJson->SetNodeValue(tmp_strDataPrefix + "/hjdqhdm", m_oBody.m_vecData[i].m_strHJDQHDM);	
                    p_pJson->SetNodeValue(tmp_strDataPrefix + "/hjdz", m_oBody.m_vecData[i].m_strHJDZ);
                    p_pJson->SetNodeValue(tmp_strDataPrefix + "/hjdgajgdm", m_oBody.m_vecData[i].m_strHJDGAJGDM);
                    p_pJson->SetNodeValue(tmp_strDataPrefix + "/hjdgajg", m_oBody.m_vecData[i].m_strHJDGAJG);					
                    p_pJson->SetNodeValue(tmp_strDataPrefix + "/xzdgajg", m_oBody.m_vecData[i].m_strXZDGAJG);
					p_pJson->SetNodeValue(tmp_strDataPrefix + "/xzdgajgdm", m_oBody.m_vecData[i].m_strXZDGAJGDM);
					p_pJson->SetNodeValue(tmp_strDataPrefix + "/xzdqh", m_oBody.m_vecData[i].m_strXZDQH);	
                    p_pJson->SetNodeValue(tmp_strDataPrefix + "/xzdqhdm", m_oBody.m_vecData[i].m_strXZDQHDM);
                    p_pJson->SetNodeValue(tmp_strDataPrefix + "/xzddz", m_oBody.m_vecData[i].m_strXZDDZ);	
                    p_pJson->SetNodeValue(tmp_strDataPrefix + "/sjhm", m_oBody.m_vecData[i].m_strSJHM);
					p_pJson->SetNodeValue(tmp_strDataPrefix + "/zp", m_oBody.m_vecData[i].m_strZP);			

                    size_t tmp_uiCHJBGSize = m_oBody.m_vecData[i].m_vecHJBG.size();

                    for (size_t j  = 0; j < tmp_uiCHJBGSize; ++j)
					{
						std::string tmp_strPrefix(tmp_strDataPrefix+"/hjbg/"+ std::to_string(j));
						p_pJson->SetNodeValue(tmp_strPrefix + "/bgsj", m_oBody.m_vecData[i].m_vecHJBG[j].m_strBGSJ);	
                        p_pJson->SetNodeValue(tmp_strPrefix + "/yhjd", m_oBody.m_vecData[i].m_vecHJBG[j].m_strYHJD);
						p_pJson->SetNodeValue(tmp_strPrefix + "/mbhjd", m_oBody.m_vecData[i].m_vecHJBG[j].m_strMBHJD);
						p_pJson->SetNodeValue(tmp_strPrefix + "/bgzt", m_oBody.m_vecData[i].m_vecHJBG[j].m_strBGZT);
						
					}						
				}
				return p_pJson->ToString();
			}

			//解析从第三方收到的消息
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return false;
				}

				if (!p_pJson->LoadJson(p_strReq))
				{
					return false;
				}

				if (nullptr == p_pJson)
				{
					return false;
				}

				if (!p_pJson->LoadJson(p_strReq))
				{
					return false;
				}


				m_oBody.m_strCode = p_pJson->GetNodeValue("/code", "");
				m_oBody.m_strMessage = p_pJson->GetNodeValue("/message", "");

				m_oBody.m_strQuery = p_pJson->GetNodeValue("/query", "");

				CData tmp_oData;
				tmp_oData.m_strBiaoQian = p_pJson->GetNodeValue("/data/basicrecord/biaoqian", "");
				tmp_oData.m_strZJLX = p_pJson->GetNodeValue("/data/basicrecord/zjlx", "");
				tmp_oData.m_strSFZHM = p_pJson->GetNodeValue("/data/basicrecord/sfzhm", "");
				tmp_oData.m_strCPH = p_pJson->GetNodeValue("/data/basicrecord/cph", "");
				tmp_oData.m_strRYLB = p_pJson->GetNodeValue("/data/basicrecord/rylb", "");
				tmp_oData.m_strRYXL = p_pJson->GetNodeValue("/data/basicrecord/ryxl", "");
				tmp_oData.m_strXM = p_pJson->GetNodeValue("/data/basicrecord/xm", "");
				tmp_oData.m_strXB = p_pJson->GetNodeValue("/data/basicrecord/xb", "");
				tmp_oData.m_strCSRQ = p_pJson->GetNodeValue("/data/basicrecord/csrq", "");
				tmp_oData.m_strMZ = p_pJson->GetNodeValue("/data/basicrecord/mz", "");
				tmp_oData.m_strJGSSXDM = p_pJson->GetNodeValue("/data/basicrecord/jgssxdm", "");
				tmp_oData.m_strJGGJDQDM = p_pJson->GetNodeValue("/data/basicrecord/jggjdqdm", "");
				tmp_oData.m_strHJDQH = p_pJson->GetNodeValue("/data/basicrecord/hjdqh", "");
				tmp_oData.m_strHJDQHDM = p_pJson->GetNodeValue("/data/basicrecord/hjdqhdm", "");
				tmp_oData.m_strHJDZ = p_pJson->GetNodeValue("/data/basicrecord/hjdz", "");
				tmp_oData.m_strHJDGAJGDM = p_pJson->GetNodeValue("/data/basicrecord/hjdgajgdm", "");
				tmp_oData.m_strHJDGAJG = p_pJson->GetNodeValue("/data/basicrecord/hjdgajg", "");
				tmp_oData.m_strXZDGAJG = p_pJson->GetNodeValue("/data/basicrecord/xzdgajg", "");
				tmp_oData.m_strXZDGAJGDM = p_pJson->GetNodeValue("/data/basicrecord/xzdgajgdm", "");
				tmp_oData.m_strXZDQH = p_pJson->GetNodeValue("/data/basicrecord/xzdqh", "");
				tmp_oData.m_strXZDQHDM = p_pJson->GetNodeValue("/data/basicrecord/xzdqhdm", "");
				tmp_oData.m_strXZDDZ = p_pJson->GetNodeValue("/data/basicrecord/xzddz", "");
				tmp_oData.m_strSJHM = p_pJson->GetNodeValue("/data/basicrecord/sjhm", "");
				tmp_oData.m_strZP = p_pJson->GetNodeValue("/data/basicrecord/zp", "");
				tmp_oData.m_strHJBG = p_pJson->GetNodeValue("/data/basicrecord/hujibiangeng", "");

				int tmp_iCount = p_pJson->GetCount("/data/basicrecord/hujibiangeng");

				CHJBG tmp_oHJBG;
				for (int i = 0; i < tmp_iCount; ++i)
				{
					std::string tmp_strPrefix("/data/basicrecord/hujibiangeng/" + std::to_string(i));
					tmp_oHJBG.m_strBGSJ = p_pJson->GetNodeValue(tmp_strPrefix +"/bgsj", "");
					tmp_oHJBG.m_strYHJD = p_pJson->GetNodeValue(tmp_strPrefix + "/yhjd", "");
					tmp_oHJBG.m_strMBHJD = p_pJson->GetNodeValue(tmp_strPrefix + "/mbhjd", "");
					tmp_oHJBG.m_strBGZT = p_pJson->GetNodeValue(tmp_strPrefix + "/bgzt", "");
					tmp_oData.m_vecHJBG.push_back(tmp_oHJBG);
				}

				m_oBody.m_vecData.push_back(tmp_oData);
				return true;
			}

		public:
			CHeaderEx m_oHeader;
			
			class CHJBG  //户籍变更记录
			{
			public:			
				std::string m_strBGSJ;  //bgsj	变更数据ID
				std::string m_strYHJD;    //yhjd 原户籍地
				std::string m_strMBHJD;  //mbhjd 目标户籍地
				std::string m_strBGZT; //bgzt	变更状态
			};
			
			class CData
			{
			public:			
				std::string m_strBiaoQian;  //标签
				std::string m_strZJLX; //触警时间
				std::string m_strSFZHM; //身份证号码
				std::string m_strCPH;   //车牌信息
				std::string m_strRYLB;  //人员类别
				std::string m_strRYXL;  //人员细类
				std::string m_strXM;  //姓名
				std::string m_strXB;  //性别
				std::string m_strCSRQ;  //出生日期
				std::string m_strMZ;  //民族
				std::string m_strJGSSXDM;  //籍贯省市县代码
				std::string m_strJGGJDQDM; //jggjdqdm 籍贯国籍代码
				std::string m_strHJDQH;  //hjdqh	户籍地区划
				std::string m_strHJDQHDM; //hjdqhdm	户籍地代码
				std::string m_strHJDZ;   //hjdz	户籍地址
				std::string m_strHJDGAJGDM; //hjdgajgdm 户籍地公安机关代码
				std::string m_strHJDGAJG; //hjdgajg	户籍地公安机关
				std::string m_strXZDGAJG; //xzdgajg	行政地公安机关
				std::string m_strXZDGAJGDM; //xzdgajgdm	现住地公安机关代码
				std::string m_strXZDQH; //xzdqh	行政区划
				std::string m_strXZDQHDM; //xzdqhdm	行政区划代码
				std::string m_strXZDDZ; //xzddz	现住地地址
				std::string m_strSJHM; //sjhm	"[{\"sjhm\":\"1341XX042\"},{\"sjhm\":\"158XX7692\"},{\"sjhm\":\"17713XX390\"},{\"sjhm\":\"15XX269\"},{\"sjhm\":\"1528XX481\"}]",    //相关手机号码
				std::string m_strHJBG;
				std::vector<CHJBG>  m_vecHJBG; //户籍变更记录
				std::string m_strZP;   //照片数据

				
			};
			class CBody
			{
			public:	
				std::string m_strQuery; //第三方返回携 带
				std::string m_strCode;  //HTTP的结果码  第三方响应消息才有
				std::string m_strMessage;  //结果消息   第三方响应消息才有

				std::string m_strXM;
				std::string m_strSFZH;
				std::vector<CData> m_vecData;
			};
			CBody m_oBody;
		};
	}
}
