/*
报警人身份信息解析
*/
#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/IRequest.h>
namespace ICC
{
	namespace PROTOCOL
	{
		class CGetBJRSFZHLoginRequest :
			public IReceive
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}
				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/code", m_oBody.m_code);
				p_pJson->SetNodeValue("/body/data", m_oBody.m_data);
				p_pJson->SetNodeValue("/body/msg", m_oBody.m_msg);

				return p_pJson->ToString();
			}
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

				m_oBody.m_code = p_pJson->GetNodeValue("/code", "");
				m_oBody.m_data = p_pJson->GetNodeValue("/data", "");
				m_oBody.m_msg = p_pJson->GetNodeValue("/msg", "");
				return true;
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_code;       //0表示成功，其他表示失败
				std::string m_data;       //token,访问凭证
				std::string m_msg;        //token过期时间，单位是秒
			};
			CBody m_oBody;
		};

		//身份证号码获取人员信息返回
		struct metaAddrDO {
			std::string m_strallFullAddr;		//住址全程
			std::string m_strbuildingName;		//小区(楼房)
			std::string m_strcancelledFlag;	    //注销标识
			std::string m_strcell;				//单元
			std::string m_strdsbm;				//地市编码
			std::string m_strdutyAreaName;		//责任区
			std::string m_strfloor;		        //层
			std::string m_strfloorQz;		    //楼层前缀(H半层，B地址层，Z架空层)
			std::string m_strhaozuo;		    //门牌号
			std::string m_strhaozuoDetail;	    //详址
			std::string m_strhaozuoNo;		    //门牌号码
			std::string m_strmapx;			    //坐标x
			std::string m_strmapy;			    //坐标y
			std::string m_strmetaAddrId;	    //地址编码
			std::string m_strorgName;		    //派出所名称
			std::string m_strqu;			    //区县
			std::string m_strquId;			    //行政区划代码
			std::string m_strqxcunName;	        //区乡村名称
			std::string m_strroom;			    //室
			std::string m_strroomDetail;	    //详室
			std::string m_strseat;			    //楼座
			std::string m_strtoponym;			//地名
			std::string m_strxzjdName;			//乡镇街道名称
		};

		typedef struct Data
		{
			std::string m_strbm;		    //别名
			std::string m_strbyzk; 			//兵役情况
			std::string m_strcsdGjhdq;		//出生地国家地区
			std::string m_strcsdQhnxxdz;	//出生地区划内详细地址
			std::string m_strcsdSsxq;		//出生地省市区县
			std::string m_strcsrq;			//出生日期
			std::string m_strcym;			//曾用名
			std::string m_strcyzkDwmc;		//从业状况-单位名称
			std::string m_strcyzkHylb;		//从业状况-行业类别
			std::string m_strcyzkZy;		//从业状况-职业
			std::string m_strcyzkZylb;		//从业状况-职业类别
			std::string m_strfqCyzj;		//父亲证件
			std::string m_strfqCyzjdm;		//父亲证件种类
			std::string m_strfqGmsfhm;		//父亲公民身份号码
			std::string m_strfqRkbm;		//父亲人口编码
			std::string m_strfqXm;			//父亲姓名
			std::string m_strfqZjhm;		//父亲证件号码
			std::string m_strgjdq;			//国家地区
			std::string m_strgmsfhm;		//公民身份号码
			std::string m_strgxsj;			//更新时间
			std::string m_strhh;			//户号
			std::string m_strhjbm;			//户籍编码
			std::string m_strhjdzBzdzbm;	//户籍地址标准地址编码
			std::string m_strhjdzDz;		//户籍地址地址
			std::string m_strhjdzDzbm;		//户籍地址地址编码
			std::string m_strhjdzPcs;		//户籍地址派出所
			std::string m_strhjdzPcsdm;	    //户籍地址派出所编码
			std::string m_strhjdzQhnxxdz;	//户籍地址区划内详细地址
			std::string m_strhjdzRhyzbs;	//户籍地址人户一致标识，1一致，0不一致
			std::string m_strhkxz;			//户口性质
			std::string m_strhyzk;			//婚姻状况
			std::string m_strjgGjhdq;		//籍贯国家地区
			std::string m_strjgQhnxxdz;	    //籍贯区划内详细地址
			std::string m_strjgSsxq;		//籍贯省市县区
			std::string m_strjhreCyzj;		//监护人二-证件
			std::string m_strjhreCyzjdm;	//监护人二-证件种类
			std::string m_strjhreGmsfhm;	//监护人二-公民身份号码
			std::string m_strjhreJhgx;		//监护人二-监护关系
			std::string m_strjhreLxdh;		//监护人二-联系电话
			std::string m_strjhreRkbm;		//监护人二-人口编码
			std::string m_strjhreXm;		//监护人二-姓名
			std::string m_strjhreZjhm;		//监护人二-证件号码
			std::string m_strjhryCyzj;		//监护人一-证件
			std::string m_strjhryCyzjdm;	//监护人一-证件种类
			std::string m_strjhryGmsfhm;	//监护人一-公民身份号码
			std::string m_strjhryJhgx;		//监护人一-监护关系
			std::string m_strjhryLxdh;		//监护人一-联系电话
			std::string m_strjhryRkbm;		//监护人一-人口编码
			std::string m_strjhryXm;		//监护人一-姓名
			std::string m_strjhryZjhm;		//监护人一-证件号码
			std::string m_strlxdh;			//联系电话
			std::string m_strmqCyzj;		//母亲证件
			std::string m_strmqCyzjdm;		//母亲证件种类
			std::string m_strmqGmsfhm;		//母亲公民身份号码
			std::string m_strmqRkbm;		//母亲人口编码
			std::string m_strmqXm;			//母亲姓名
			std::string m_strmqZjhm;		//母亲证件号码
			std::string m_strmz;			//民族
			std::string m_strpoCyzj;		//配偶证件
			std::string m_strpoCyzjdm;		//配偶证件种类
			std::string m_strpoGmsfhm;		//配偶公民身份号码
			std::string m_strpoRkbm;		//配偶人口编码
			std::string m_strpoXm;			//配偶姓名
			std::string m_strpoZjhm;		//配偶证件号码
			std::string m_strrkbm;			//人口编码
			std::string m_strrkglswyy;		//死亡原因
			std::string m_strrkglzxlb;		//人口注销类别
			std::string m_strrkglzxlbdm;	//人口注销类别代码
			std::string m_strrkxxjb;		//人口信息级别
			std::string m_strsg;			//身高
			std::string m_strswrq;			//死亡日期
			std::string m_strswzmbh;		//死亡证明编号
			std::string m_strxb;			//性别
			std::string m_strxl;			//学历
			std::string m_strxm;			//姓名
			std::string m_strxx;			//血型
			std::string m_strxzb;		    //x坐标
			std::string m_strxzzDz;		    //现住址地址
			std::string m_strxzzDzbm;		//现住址地址编码
			std::string m_stryhzgx;		    //与户主关系
			std::string m_stryzb;			//y坐标
			std::string m_strzjxy;			//宗教信仰
			std::string m_strzpBh;			//照片编号
			std::string m_strzpBz;			//照片标志 0有 1无
			std::string m_strzpUrl;		    //照片地址
			std::string m_strzxsj;			//注销时间
			struct metaAddrDO m_strAddrDO;
		}Data;

		class CGetBJRSFZHIdentiyInfoRequest :
			public IReceive
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				//if (nullptr == p_pJson)
				//{
				//	return "";
				//}
				//m_oHeader.SaveTo(p_pJson);


				p_pJson->SetNodeValue("/code", m_oBody.m_code);
				p_pJson->SetNodeValue("/msg", m_oBody.m_msg);
				if (0 == m_oBody.m_code.compare("200"))
				{
					size_t tmp_uiDataSize = m_oBody.m_vecData.size();
					for (size_t i = 0; i < tmp_uiDataSize; ++i)
					{
						std::string tmp_strDataPrefix("/data/" + std::to_string(i));
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/bm", m_oBody.m_vecData[i].m_strbm);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/byzk", m_oBody.m_vecData[i].m_strbyzk);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/csdGjhdq", m_oBody.m_vecData[i].m_strcsdGjhdq);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/csdQhnxxdz", m_oBody.m_vecData[i].m_strcsdQhnxxdz);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/csdSsxq", m_oBody.m_vecData[i].m_strcsdSsxq);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/csrq", m_oBody.m_vecData[i].m_strcsrq);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/cym", m_oBody.m_vecData[i].m_strcym);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/cyzkDwmc", m_oBody.m_vecData[i].m_strcyzkDwmc);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/cyzkHylb", m_oBody.m_vecData[i].m_strcyzkHylb);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/cyzkZy", m_oBody.m_vecData[i].m_strcyzkZy);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/cyzkZylb", m_oBody.m_vecData[i].m_strcyzkZylb);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/fqCyzj", m_oBody.m_vecData[i].m_strfqCyzj);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/fqCyzjdm", m_oBody.m_vecData[i].m_strfqCyzjdm);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/fqGmsfhm", m_oBody.m_vecData[i].m_strfqGmsfhm);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/fqRkbm", m_oBody.m_vecData[i].m_strfqRkbm);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/fqXm", m_oBody.m_vecData[i].m_strfqXm);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/fqZjhm", m_oBody.m_vecData[i].m_strfqZjhm);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/gjdq", m_oBody.m_vecData[i].m_strgjdq);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/gmsfhm", m_oBody.m_vecData[i].m_strgmsfhm);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/gxsj", m_oBody.m_vecData[i].m_strgxsj);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/hh", m_oBody.m_vecData[i].m_strhh);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/hjbm", m_oBody.m_vecData[i].m_strhjbm);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/hjdzBzdzbm", m_oBody.m_vecData[i].m_strhjdzBzdzbm);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/hjdzDz", m_oBody.m_vecData[i].m_strhjdzDz);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/hjdzDzbm", m_oBody.m_vecData[i].m_strhjdzDzbm);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/hjdzPcs", m_oBody.m_vecData[i].m_strhjdzPcs);


						p_pJson->SetNodeValue(tmp_strDataPrefix + "/hjdzPcsdm", m_oBody.m_vecData[i].m_strhjdzPcsdm);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/hjdzQhnxxdz", m_oBody.m_vecData[i].m_strhjdzQhnxxdz);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/hjdzRhyzbs", m_oBody.m_vecData[i].m_strhjdzRhyzbs);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/hkxz", m_oBody.m_vecData[i].m_strhkxz);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/hyzk", m_oBody.m_vecData[i].m_strhyzk);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/jgGjhdq", m_oBody.m_vecData[i].m_strjgGjhdq);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/jgQhnxxdz", m_oBody.m_vecData[i].m_strjgQhnxxdz);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/jgSsxq", m_oBody.m_vecData[i].m_strjgSsxq);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/jhreCyzj", m_oBody.m_vecData[i].m_strjhreCyzj);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/jhreCyzjdm", m_oBody.m_vecData[i].m_strjhreCyzjdm);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/jhreGmsfhm", m_oBody.m_vecData[i].m_strjhreGmsfhm);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/jhreJhgx", m_oBody.m_vecData[i].m_strjhreJhgx);


						p_pJson->SetNodeValue(tmp_strDataPrefix + "/jhreLxdh", m_oBody.m_vecData[i].m_strjhreLxdh);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/jhreRkbm", m_oBody.m_vecData[i].m_strjhreRkbm);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/jhreXm", m_oBody.m_vecData[i].m_strjhreXm);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/jhreZjhm", m_oBody.m_vecData[i].m_strjhreZjhm);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/jhryCyzj", m_oBody.m_vecData[i].m_strjhryCyzj);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/jhryCyzjdm", m_oBody.m_vecData[i].m_strjhryCyzjdm);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/jhryGmsfhm", m_oBody.m_vecData[i].m_strjhryGmsfhm);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/jhryJhgx", m_oBody.m_vecData[i].m_strjhryJhgx);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/jhryLxdh", m_oBody.m_vecData[i].m_strjhryLxdh);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/jhryRkbm", m_oBody.m_vecData[i].m_strjhryRkbm);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/jhryXm", m_oBody.m_vecData[i].m_strjhryXm);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/jhryZjhm", m_oBody.m_vecData[i].m_strjhryZjhm);

						p_pJson->SetNodeValue(tmp_strDataPrefix + "/lxdh", m_oBody.m_vecData[i].m_strlxdh);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/mqCyzj", m_oBody.m_vecData[i].m_strmqCyzj);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/mqCyzjdm", m_oBody.m_vecData[i].m_strmqCyzjdm);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/mqGmsfhm", m_oBody.m_vecData[i].m_strmqGmsfhm);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/mqRkbm", m_oBody.m_vecData[i].m_strmqRkbm);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/mqXm", m_oBody.m_vecData[i].m_strmqXm);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/mqZjhm", m_oBody.m_vecData[i].m_strmqZjhm);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/mz", m_oBody.m_vecData[i].m_strmz);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/poCyzj", m_oBody.m_vecData[i].m_strpoCyzj);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/poCyzjdm", m_oBody.m_vecData[i].m_strpoCyzjdm);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/poGmsfhm", m_oBody.m_vecData[i].m_strpoGmsfhm);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/poRkbm", m_oBody.m_vecData[i].m_strpoRkbm);


						p_pJson->SetNodeValue(tmp_strDataPrefix + "/poXm", m_oBody.m_vecData[i].m_strpoXm);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/poZjhm", m_oBody.m_vecData[i].m_strpoZjhm);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/rkbm", m_oBody.m_vecData[i].m_strrkbm);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/rkglswyy", m_oBody.m_vecData[i].m_strrkglswyy);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/rkglzxlb", m_oBody.m_vecData[i].m_strrkglzxlb);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/rkglzxlbdm", m_oBody.m_vecData[i].m_strrkglzxlbdm);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/rkxxjb", m_oBody.m_vecData[i].m_strrkxxjb);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/sg", m_oBody.m_vecData[i].m_strsg);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/swrq", m_oBody.m_vecData[i].m_strswrq);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/swzmbh", m_oBody.m_vecData[i].m_strswzmbh);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/xb", m_oBody.m_vecData[i].m_strxb);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/xl", m_oBody.m_vecData[i].m_strxl);


						p_pJson->SetNodeValue(tmp_strDataPrefix + "/xm", m_oBody.m_vecData[i].m_strxm);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/xx", m_oBody.m_vecData[i].m_strxx);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/xzb", m_oBody.m_vecData[i].m_strxzb);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/xzzDz", m_oBody.m_vecData[i].m_strxzzDz);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/xzzDzbm", m_oBody.m_vecData[i].m_strxzzDzbm);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/yhzgx", m_oBody.m_vecData[i].m_stryhzgx);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/yzb", m_oBody.m_vecData[i].m_stryzb);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/zjxy", m_oBody.m_vecData[i].m_strzjxy);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/zpBh", m_oBody.m_vecData[i].m_strzpBh);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/zpBz", m_oBody.m_vecData[i].m_strzpBz);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/zpUrl", m_oBody.m_vecData[i].m_strzpUrl);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/zxsj", m_oBody.m_vecData[i].m_strzxsj);

						p_pJson->SetNodeValue(tmp_strDataPrefix + "/metaAddrDO/allFullAddr", m_oBody.m_vecData[i].m_strAddrDO.m_strallFullAddr);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/metaAddrDO/buildingName", m_oBody.m_vecData[i].m_strAddrDO.m_strbuildingName);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/metaAddrDO/cancelledFlag", m_oBody.m_vecData[i].m_strAddrDO.m_strcancelledFlag);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/metaAddrDO/cell", m_oBody.m_vecData[i].m_strAddrDO.m_strcell);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/metaAddrDO/dsbm", m_oBody.m_vecData[i].m_strAddrDO.m_strdsbm);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/metaAddrDO/dutyAreaName", m_oBody.m_vecData[i].m_strAddrDO.m_strdutyAreaName);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/metaAddrDO/floor", m_oBody.m_vecData[i].m_strAddrDO.m_strfloor);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/metaAddrDO/floorQz", m_oBody.m_vecData[i].m_strAddrDO.m_strfloorQz);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/metaAddrDO/haozuo", m_oBody.m_vecData[i].m_strAddrDO.m_strhaozuo);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/metaAddrDO/haozuoDetail", m_oBody.m_vecData[i].m_strAddrDO.m_strhaozuoDetail);


						p_pJson->SetNodeValue(tmp_strDataPrefix + "/metaAddrDO/haozuoNo", m_oBody.m_vecData[i].m_strAddrDO.m_strhaozuoNo);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/metaAddrDO/mapx", m_oBody.m_vecData[i].m_strAddrDO.m_strmapx);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/metaAddrDO/mapy", m_oBody.m_vecData[i].m_strAddrDO.m_strmapy);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/metaAddrDO/metaAddrId", m_oBody.m_vecData[i].m_strAddrDO.m_strmetaAddrId);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/metaAddrDO/orgName", m_oBody.m_vecData[i].m_strAddrDO.m_strorgName);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/metaAddrDO/qu", m_oBody.m_vecData[i].m_strAddrDO.m_strqu);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/metaAddrDO/quId", m_oBody.m_vecData[i].m_strAddrDO.m_strquId);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/metaAddrDO/qxcunName", m_oBody.m_vecData[i].m_strAddrDO.m_strqxcunName);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/metaAddrDO/room", m_oBody.m_vecData[i].m_strAddrDO.m_strroom);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/metaAddrDO/roomDetail", m_oBody.m_vecData[i].m_strAddrDO.m_strroomDetail);

						p_pJson->SetNodeValue(tmp_strDataPrefix + "/metaAddrDO/seat", m_oBody.m_vecData[i].m_strAddrDO.m_strseat);

						p_pJson->SetNodeValue(tmp_strDataPrefix + "/metaAddrDO/toponym", m_oBody.m_vecData[i].m_strAddrDO.m_strtoponym);
						p_pJson->SetNodeValue(tmp_strDataPrefix + "/metaAddrDO/xzjdName", m_oBody.m_vecData[i].m_strAddrDO.m_strxzjdName);
					}
				}
				return p_pJson->ToString();
			}
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_code = p_pJson->GetNodeValue("/code", "");
				m_oBody.m_msg = p_pJson->GetNodeValue("/msg", "");

				if (0 == m_oBody.m_code.compare("0"))
				{
					m_oBody.m_code = "200";
					int strlen = p_pJson->GetCount("/data""");
					for (int i = 0; i < strlen; i++)
					{
						std::string tmp_path("/data/" + std::to_string(i));
						m_oBody.data.m_strbm = p_pJson->GetNodeValue(tmp_path + "/bm", "");
						m_oBody.data.m_strbyzk = p_pJson->GetNodeValue(tmp_path + "/byzk", "");
						m_oBody.data.m_strcsdGjhdq = p_pJson->GetNodeValue(tmp_path + "/csdGjhdq", "");
						m_oBody.data.m_strcsdQhnxxdz = p_pJson->GetNodeValue(tmp_path + "/csdQhnxxdz", "");
						m_oBody.data.m_strcsdSsxq = p_pJson->GetNodeValue(tmp_path + "/csdSsxq", "");
						m_oBody.data.m_strcsrq = p_pJson->GetNodeValue(tmp_path + "/csrq", "");
						m_oBody.data.m_strcym = p_pJson->GetNodeValue(tmp_path + "/cym", "");
						m_oBody.data.m_strcyzkDwmc = p_pJson->GetNodeValue(tmp_path + "/cyzkDwmc", "");
						m_oBody.data.m_strcyzkHylb = p_pJson->GetNodeValue(tmp_path + "/cyzkHylb", "");
						m_oBody.data.m_strcyzkZy = p_pJson->GetNodeValue(tmp_path + "/cyzkZy", "");
						m_oBody.data.m_strcyzkZylb = p_pJson->GetNodeValue(tmp_path + "/cyzkZylb", "");
						m_oBody.data.m_strfqCyzj = p_pJson->GetNodeValue(tmp_path + "/fqCyzj", "");
						m_oBody.data.m_strfqCyzjdm = p_pJson->GetNodeValue(tmp_path + "/fqCyzjdm", "");
						m_oBody.data.m_strfqGmsfhm = p_pJson->GetNodeValue(tmp_path + "/fqGmsfhm", "");
						m_oBody.data.m_strfqRkbm = p_pJson->GetNodeValue(tmp_path + "/fqRkbm", "");
						m_oBody.data.m_strfqXm = p_pJson->GetNodeValue(tmp_path + "/fqXm", "");
						m_oBody.data.m_strfqZjhm = p_pJson->GetNodeValue(tmp_path + "/fqZjhm", "");
						m_oBody.data.m_strgjdq = p_pJson->GetNodeValue(tmp_path + "/gjdq", "");
						m_oBody.data.m_strgmsfhm = p_pJson->GetNodeValue(tmp_path + "/gmsfhm", "");
						m_oBody.data.m_strgxsj = p_pJson->GetNodeValue(tmp_path + "/gxsj", "");
						m_oBody.data.m_strhh = p_pJson->GetNodeValue(tmp_path + "/hh", "");
						m_oBody.data.m_strhjbm = p_pJson->GetNodeValue(tmp_path + "/hjbm", "");
						m_oBody.data.m_strhjdzBzdzbm = p_pJson->GetNodeValue(tmp_path + "/hjdzBzdzbm", "");
						m_oBody.data.m_strhjdzDz = p_pJson->GetNodeValue(tmp_path + "/hjdzDz", "");
						m_oBody.data.m_strhjdzDzbm = p_pJson->GetNodeValue(tmp_path + "/hjdzDzbm", "");
						m_oBody.data.m_strhjdzPcs = p_pJson->GetNodeValue(tmp_path + "/hjdzPcs", "");

						m_oBody.data.m_strhjdzPcsdm = p_pJson->GetNodeValue(tmp_path + "/hjdzPcsdm", "");
						m_oBody.data.m_strhjdzQhnxxdz = p_pJson->GetNodeValue(tmp_path + "/hjdzQhnxxdz", "");
						m_oBody.data.m_strhjdzRhyzbs = p_pJson->GetNodeValue(tmp_path + "/hjdzRhyzbs", "");
						m_oBody.data.m_strhkxz = p_pJson->GetNodeValue(tmp_path + "/hkxz", "");
						m_oBody.data.m_strhyzk = p_pJson->GetNodeValue(tmp_path + "/hyzk", "");
						m_oBody.data.m_strjgGjhdq = p_pJson->GetNodeValue(tmp_path + "/jgGjhdq", "");
						m_oBody.data.m_strjgQhnxxdz = p_pJson->GetNodeValue(tmp_path + "/jgQhnxxdz", "");
						m_oBody.data.m_strjgSsxq = p_pJson->GetNodeValue(tmp_path + "/jgSsxq", "");
						m_oBody.data.m_strjhreCyzj = p_pJson->GetNodeValue(tmp_path + "/jhreCyzj", "");
						m_oBody.data.m_strjhreCyzjdm = p_pJson->GetNodeValue(tmp_path + "/jhreCyzjdm", "");
						m_oBody.data.m_strjhreGmsfhm = p_pJson->GetNodeValue(tmp_path + "/jhreGmsfhm", "");
						m_oBody.data.m_strjhreJhgx = p_pJson->GetNodeValue(tmp_path + "/jhreJhgx", "");

						m_oBody.data.m_strjhreLxdh = p_pJson->GetNodeValue(tmp_path + "/jhreLxdh", "");
						m_oBody.data.m_strjhreRkbm = p_pJson->GetNodeValue(tmp_path + "/jhreRkbm", "");
						m_oBody.data.m_strjhreXm = p_pJson->GetNodeValue(tmp_path + "/jhreXm", "");
						m_oBody.data.m_strjhreZjhm = p_pJson->GetNodeValue(tmp_path + "/jhreZjhm", "");
						m_oBody.data.m_strjhryCyzj = p_pJson->GetNodeValue(tmp_path + "/jhryCyzj", "");
						m_oBody.data.m_strjhryCyzjdm = p_pJson->GetNodeValue(tmp_path + "/jhryCyzjdm", "");
						m_oBody.data.m_strjhryGmsfhm = p_pJson->GetNodeValue(tmp_path + "/jhryGmsfhm", "");
						m_oBody.data.m_strjhryJhgx = p_pJson->GetNodeValue(tmp_path + "/jhryJhgx", "");
						m_oBody.data.m_strjhryRkbm = p_pJson->GetNodeValue(tmp_path + "/jhryRkbm", "");
						m_oBody.data.m_strjhryXm = p_pJson->GetNodeValue(tmp_path + "/jhryXm", "");
						m_oBody.data.m_strjhryZjhm = p_pJson->GetNodeValue(tmp_path + "/jhryZjhm", "");

						m_oBody.data.m_strlxdh = p_pJson->GetNodeValue(tmp_path + "/lxdh", "");
						m_oBody.data.m_strmqCyzj = p_pJson->GetNodeValue(tmp_path + "/mqCyzj", "");
						m_oBody.data.m_strmqCyzjdm = p_pJson->GetNodeValue(tmp_path + "/mqCyzjdm", "");
						m_oBody.data.m_strmqGmsfhm = p_pJson->GetNodeValue(tmp_path + "/mqGmsfhm", "");
						m_oBody.data.m_strmqRkbm = p_pJson->GetNodeValue(tmp_path + "/mqRkbm", "");
						m_oBody.data.m_strmqXm = p_pJson->GetNodeValue(tmp_path + "/mqXm", "");
						m_oBody.data.m_strmqZjhm = p_pJson->GetNodeValue(tmp_path + "/mqZjhm", "");
						m_oBody.data.m_strmz = p_pJson->GetNodeValue(tmp_path + "/mz", "");
						m_oBody.data.m_strpoCyzj = p_pJson->GetNodeValue(tmp_path + "/poCyzj", "");
						m_oBody.data.m_strpoCyzjdm = p_pJson->GetNodeValue(tmp_path + "/poCyzjdm", "");
						m_oBody.data.m_strpoGmsfhm = p_pJson->GetNodeValue(tmp_path + "/poGmsfhm", "");
						m_oBody.data.m_strpoRkbm = p_pJson->GetNodeValue(tmp_path + "/poRkbm", "");

						m_oBody.data.m_strpoXm = p_pJson->GetNodeValue(tmp_path + "/poXm", "");
						m_oBody.data.m_strpoZjhm = p_pJson->GetNodeValue(tmp_path + "/poZjhm", "");
						m_oBody.data.m_strrkbm = p_pJson->GetNodeValue(tmp_path + "/rkbm", "");
						m_oBody.data.m_strrkglswyy = p_pJson->GetNodeValue(tmp_path + "/rkglswyy", "");
						m_oBody.data.m_strrkglzxlb = p_pJson->GetNodeValue(tmp_path + "/rkglzxlb", "");
						m_oBody.data.m_strrkglzxlbdm = p_pJson->GetNodeValue(tmp_path + "/rkglzxlbdm", "");
						m_oBody.data.m_strrkxxjb = p_pJson->GetNodeValue(tmp_path + "/rkxxjb", "");
						m_oBody.data.m_strsg = p_pJson->GetNodeValue(tmp_path + "/sg", "");
						m_oBody.data.m_strswrq = p_pJson->GetNodeValue(tmp_path + "/swrq", "");
						m_oBody.data.m_strswzmbh = p_pJson->GetNodeValue(tmp_path + "/swzmbh", "");
						m_oBody.data.m_strxb = p_pJson->GetNodeValue(tmp_path + "/xb", "");
						m_oBody.data.m_strxl = p_pJson->GetNodeValue(tmp_path + "/xl", "");

						m_oBody.data.m_strxm = p_pJson->GetNodeValue(tmp_path + "/xm", "");
						m_oBody.data.m_strxx = p_pJson->GetNodeValue(tmp_path + "/xx", "");
						m_oBody.data.m_strxzb = p_pJson->GetNodeValue(tmp_path + "/xzb", "");
						m_oBody.data.m_strxzzDz = p_pJson->GetNodeValue(tmp_path + "/xzzDz", "");
						m_oBody.data.m_strxzzDzbm = p_pJson->GetNodeValue(tmp_path + "/xzzDzbm", "");
						m_oBody.data.m_stryhzgx = p_pJson->GetNodeValue(tmp_path + "/yhzgx", "");
						m_oBody.data.m_stryzb = p_pJson->GetNodeValue(tmp_path + "/yzb", "");
						m_oBody.data.m_strzjxy = p_pJson->GetNodeValue(tmp_path + "/zjxy", "");
						m_oBody.data.m_strzpBh = p_pJson->GetNodeValue(tmp_path + "/zpBh", "");
						m_oBody.data.m_strzpBz = p_pJson->GetNodeValue(tmp_path + "/zpBz", "");
						m_oBody.data.m_strzpUrl = p_pJson->GetNodeValue(tmp_path + "/zpUrl", "");
						m_oBody.data.m_strzxsj = p_pJson->GetNodeValue(tmp_path + "/zxsj", "");

						m_oBody.data.m_strAddrDO.m_strallFullAddr = p_pJson->GetNodeValue(tmp_path + "/metaAddrDO/allFullAddr", "");
						m_oBody.data.m_strAddrDO.m_strbuildingName = p_pJson->GetNodeValue(tmp_path + "/metaAddrDO/buildingName", "");
						m_oBody.data.m_strAddrDO.m_strcancelledFlag = p_pJson->GetNodeValue(tmp_path + "/metaAddrDO/cancelledFlag", "");
						m_oBody.data.m_strAddrDO.m_strcell = p_pJson->GetNodeValue(tmp_path + "/metaAddrDO/cell", "");
						m_oBody.data.m_strAddrDO.m_strdsbm = p_pJson->GetNodeValue(tmp_path + "/metaAddrDO/dsbm", "");
						m_oBody.data.m_strAddrDO.m_strdutyAreaName = p_pJson->GetNodeValue(tmp_path + "/metaAddrDO/dutyAreaName", "");
						m_oBody.data.m_strAddrDO.m_strfloor = p_pJson->GetNodeValue(tmp_path + " /metaAddrDO/floor", "");
						m_oBody.data.m_strAddrDO.m_strfloorQz = p_pJson->GetNodeValue(tmp_path + "/metaAddrDO/floorQz", "");
						m_oBody.data.m_strAddrDO.m_strhaozuo = p_pJson->GetNodeValue(tmp_path + "/metaAddrDO/haozuo", "");
						m_oBody.data.m_strAddrDO.m_strhaozuoDetail = p_pJson->GetNodeValue(tmp_path + "/metaAddrDO/haozuoDetail", "");


						m_oBody.data.m_strAddrDO.m_strhaozuoNo = p_pJson->GetNodeValue(tmp_path + "/metaAddrDO/haozuoNo", "");
						m_oBody.data.m_strAddrDO.m_strmapx = p_pJson->GetNodeValue(tmp_path + "/metaAddrDO/mapx", "");
						m_oBody.data.m_strAddrDO.m_strmapy = p_pJson->GetNodeValue(tmp_path + "/metaAddrDO/mapy", "");
						m_oBody.data.m_strAddrDO.m_strmetaAddrId = p_pJson->GetNodeValue(tmp_path + "/metaAddrDO/metaAddrId", "");
						m_oBody.data.m_strAddrDO.m_strorgName = p_pJson->GetNodeValue(tmp_path + "/metaAddrDO/orgName", "");
						m_oBody.data.m_strAddrDO.m_strqu = p_pJson->GetNodeValue(tmp_path + "/metaAddrDO/qu", "");
						m_oBody.data.m_strAddrDO.m_strquId = p_pJson->GetNodeValue(tmp_path + "/metaAddrDO/quId", "");
						m_oBody.data.m_strAddrDO.m_strqxcunName = p_pJson->GetNodeValue(tmp_path + "/metaAddrDO/qxcunName", "");
						m_oBody.data.m_strAddrDO.m_strroom = p_pJson->GetNodeValue(tmp_path + "/metaAddrDO/room", "");
						m_oBody.data.m_strAddrDO.m_strroomDetail = p_pJson->GetNodeValue(tmp_path + "/metaAddrDO/roomDetail", "");
						m_oBody.data.m_strAddrDO.m_strseat = p_pJson->GetNodeValue(tmp_path + "/metaAddrDO/seat", "");
						m_oBody.data.m_strAddrDO.m_strtoponym = p_pJson->GetNodeValue(tmp_path + "/metaAddrDO/toponym", "");
						m_oBody.data.m_strAddrDO.m_strxzjdName = p_pJson->GetNodeValue(tmp_path + "/metaAddrDO/xzjdName", "");
						m_oBody.m_vecData.push_back(m_oBody.data);
					}
				}
				return true;
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_code;       //0表示成功，其他表示失败
				std::string m_msg;        //说明
				Data data;
				std::vector<Data> m_vecData;
			};
			CBody m_oBody;
		};
	}
}