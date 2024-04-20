#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include "Protocol/CHeader.h"
#include <vector>
#include <map>
#include <boost/algorithm/string.hpp>
namespace ICC
{
	namespace PROTOCOL
	{
        class CFeedBackRequest :  public IRequest, public IRespond
		{            

        public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
            {
                if (!m_oHeader.ParseString(p_strReq, p_pJson))
                {
                    return false;
                }

                for (unsigned int i = 0; i < m_oBody.m_vecNodeNames.size(); ++i)
                {            
                    std::string strName = m_oBody.m_vecNodeNames[i];
                    boost::algorithm::to_upper(strName);
                    std::string strPath = "/body/";
                    strPath += strName;
                    std::string strValue = p_pJson->GetNodeValue(strPath, "");
                    m_oBody.m_mapDatas.insert(std::make_pair(strName, strValue));
                }
            
                return true;
            }

            virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
            {
				if (nullptr == p_pJson)
				{
					return "";
				}

                m_oHeader.SaveTo(p_pJson);

                std::map<std::string, std::string>::const_iterator itr;
                for (itr = m_oBody.m_mapDatas.begin(); itr != m_oBody.m_mapDatas.end(); ++itr)
                {
                    std::string strTmpPath = "/body/";
                    p_pJson->SetNodeValue(strTmpPath + itr->first, itr->second);
                }                
                return p_pJson->ToString();
            }
		public:
			CHeader m_oHeader;
			class CBody
			{
            public:
                CBody()
                {
					m_vecNodeNames.push_back("msg_source");
					m_vecNodeNames.push_back("xzqhdm");
					m_vecNodeNames.push_back("fkdbh");
					m_vecNodeNames.push_back("jjdbh");
					m_vecNodeNames.push_back("pjdbh");
					m_vecNodeNames.push_back("fklyh");
					m_vecNodeNames.push_back("fkdwdm");
					m_vecNodeNames.push_back("fkybh");
					m_vecNodeNames.push_back("fkyxm");
					m_vecNodeNames.push_back("fksj");
					m_vecNodeNames.push_back("cjsj01");
					m_vecNodeNames.push_back("ddxcsj");
					m_vecNodeNames.push_back("xcclwbsj");
					m_vecNodeNames.push_back("jqlbdm");
					m_vecNodeNames.push_back("jqlxdm");
					m_vecNodeNames.push_back("jqxldm");
					m_vecNodeNames.push_back("jqzldm");
					m_vecNodeNames.push_back("jqfssj");
					m_vecNodeNames.push_back("bjdz");
					m_vecNodeNames.push_back("jqdz");
					m_vecNodeNames.push_back("fkdwxzb");
					m_vecNodeNames.push_back("fkdwyzb");
					m_vecNodeNames.push_back("cjczqk");
					m_vecNodeNames.push_back("cdcc");
					m_vecNodeNames.push_back("cdrc");
					m_vecNodeNames.push_back("cdct");
					m_vecNodeNames.push_back("jzrs");
					m_vecNodeNames.push_back("jzrssm");
					m_vecNodeNames.push_back("ssrs");
					m_vecNodeNames.push_back("ssrssm");
					m_vecNodeNames.push_back("swrs");
					m_vecNodeNames.push_back("swrssm");
					m_vecNodeNames.push_back("jqcljgdm");
					m_vecNodeNames.push_back("jqcljgsm");
					m_vecNodeNames.push_back("tqqkdm");
					m_vecNodeNames.push_back("ssqkms");
					m_vecNodeNames.push_back("zhrs");
					m_vecNodeNames.push_back("sars");
					m_vecNodeNames.push_back("tprs");
					m_vecNodeNames.push_back("jtsgxtdm");
					m_vecNodeNames.push_back("sfzzwxp");
					m_vecNodeNames.push_back("jtsgccyydm");
					m_vecNodeNames.push_back("njddm");
					m_vecNodeNames.push_back("lmzkdm");
					m_vecNodeNames.push_back("shjdcs");
					m_vecNodeNames.push_back("shfjdcs");
					m_vecNodeNames.push_back("dllxdm");
					m_vecNodeNames.push_back("jqclztdm");
					m_vecNodeNames.push_back("cjsj");
					m_vecNodeNames.push_back("gxsj");
					m_vecNodeNames.push_back("jdxz");
					m_vecNodeNames.push_back("sdcs");
					m_vecNodeNames.push_back("sfqz");
					m_vecNodeNames.push_back("yjdwdm");
					m_vecNodeNames.push_back("yjdwlxr");
					m_vecNodeNames.push_back("yjdwfkqk");
					m_vecNodeNames.push_back("yjdwfkqksm");
					m_vecNodeNames.push_back("ajbh");
					m_vecNodeNames.push_back("nrbq");
					m_vecNodeNames.push_back("jqbwdm");
					m_vecNodeNames.push_back("fklx");
					m_vecNodeNames.push_back("tbzt");
                }

			public:
				std::string m_strMsgSource;                         //消息来源
                std::string m_str_xzqhdm;                            //警情所在地行政区划代码，与对应接警单行政区划
                std::string m_str_fkdbh;                             //反馈单编号，产生规则为警情行政区划代码+系统反馈单编号，以免不同地区产生同样编号；唯一号，主索引键
                std::string m_str_jjdbh;                             //关联接警单表中的接警单编号字段
                std::string m_str_pjdbh;                             //关联派警单表中的派警单编号字段
                std::string m_str_fklyh;                             //反馈录音号，由录音系统自动产生，接处警系统自动关联
                std::string m_str_fkdwdm;                            //反馈单位机构代码
                std::string m_str_fkybh;                             //反馈员编号，用来标识反馈员身份
                std::string m_str_fkyxm;                             //反馈人员姓名
                std::string m_str_fksj;                              //反馈员填写完成反馈单的时间，格式:YYYYMMDDHHMMSS(24小时制)
                std::string m_str_cjsj01;                            //国标为CJSJ和创建时间冲突了，汇聚平台使用CJSJ01
                std::string m_str_ddxcsj;                            //到达现场时间，格式:YYYYMMDDHHMMSS(24小时制)
                std::string m_str_xcclwbsj;                          //处理完毕时间，格式:YYYYMMDDHHMMSS(24小时制)
                std::string m_str_jqlbdm;                            //警情类别代码，在警情处理完毕后，应根据实际情况录入
                std::string m_str_jqlxdm;                            //警情类型代码，在警情处理完毕后，应根据实际情况录入
                std::string m_str_jqxldm;                            //警情细类代码，在警情处理完毕后，应根据实际情况录入。8.5《警情类别、类型、细类、子类代码表》中无细类的不填
                std::string m_str_jqzldm;                            //警情子类代码，在警情处理完毕后，应根据实际情况录入。8.5《警情类别、类型、细类、子类代码表》中无子类的不填
                std::string m_str_jqfssj;                            //警情发生时间，格式:YYYYMMDDHHMMSS(24小时制)
                std::string m_str_bjdz;                              //报警人所在地址、单位名等
                std::string m_str_jqdz;                              //警情发生的具体地址
                std::string m_str_fkdwxzb;                           //处置现场所在地点的经度
                std::string m_str_fkdwyzb;                           //处置现场所在地点的纬度
                std::string m_str_cjczqk;                            //记录出警、警情处置过程
                std::string m_str_cdcc;                              //出动车辆数(车次)
                std::string m_str_cdrc;                              //出动人员数(人次)
                std::string m_str_cdct;                              //记录出警船艇的详细信息
                std::string m_str_jzrs;                              //救助人员数量(人)
                std::string m_str_jzrssm;                            //救助人数的详细说明
                std::string m_str_ssrs;                              //受伤人员数量(人)
                std::string m_str_ssrssm;                            //受伤人数详细说明
                std::string m_str_swrs;                              //死亡人员数量(人)
                std::string m_str_swrssm;                            //死亡人数详细说明
                std::string m_str_jqcljgdm;                          //警情的处理结果代码
                std::string m_str_jqcljgsm;                          //对警情要素及现场处理等情况的文字描述
                std::string m_str_tqqkdm;                            //天气情况代码
                std::string m_str_ssqkms;                            //对造成的损失情况详细描述
                std::string m_str_zhrs;                              //现场抓获犯罪嫌疑人数量(人)
                std::string m_str_sars;                              //涉案人员数量(人)
                std::string m_str_tprs;                              //逃跑人员数量(人)
                std::string m_str_jtsgxtdm;                          //交通事故形态代码
                std::string m_str_sfzzwxp;                           //0：否，1：是，默认为0
                std::string m_str_jtsgccyydm;                        //初步查明的交通事故原因代码
                std::string m_str_njddm;                             //事故发生时的能见度
                std::string m_str_lmzkdm;                            //事故发生路段的路面状况
                std::string m_str_shjdcs;                            //事故造成机动车损坏数量
                std::string m_str_shfjdcs;                           //事故造成非机动车损坏数量
                std::string m_str_dllxdm;                            //事故发生路段的道路类型
                std::string m_str_jqclztdm;                          //记录该条警情目前的业务状态
                std::string m_str_cjsj;                              //记录创建的时间，格式:YYYYMMDDHHMMSS(24小时制)
                std::string m_str_gxsj;                              //记录新增、变更的时间，格式:YYYYMMDDHHMMSS(24小时制)
                std::string m_str_jdxz;                              //警情发生所在的乡镇街道
                std::string m_str_sdcs;                              //警情发生所在的村社
                std::string m_str_sfqz;                              //0：否，1：是，默认为0
                std::string m_str_yjdwdm;                            //警情移交中接收单位的代码
                std::string m_str_yjdwlxr;                           //警情移交后的接收单位联系人名称
                std::string m_str_yjdwfkqk;                          //移交后移交单位方面反馈内容
                std::string m_str_yjdwfkqksm;                        //警情移交后的接收单位处置结果的描述
                std::string m_str_ajbh;                              //执法办案系统中自动生成的案件编号
                std::string m_str_nrbq;                              //警情标签是对警情内容关键要素的提取和标注
                std::string m_str_jqbwdm;                            //警情发生具体部位的代码
                std::string m_str_fklx;                              //10代表到场30代表处理反馈
                std::string m_str_tbzt;                              //0表示已同步；1表示处警系统需要同步，同步后更新为0；2表示接警系统需要同步，同步后更新为0    

                std::map<std::string, std::string> m_mapDatas;
                std::vector<std::string> m_vecNodeNames;
			};
			CBody m_oBody;
		};
	}
}
