#pragma once
#include <IResource.h>
namespace ICC
{
	namespace License
	{
		/*
		* class   License类
		* author  w16314
		* purpose
		* note
		*/
		class ILicense :
			public IResource
		{
		public:
			//是否启用限制时间控制
			virtual bool IsEnableTimeLimit() = 0;
			//是否启用超级狗控制
			virtual bool IsEnableSuperdogCode() = 0;
			//是否启用机器码控制
			virtual bool IsEnableMachineCode() = 0;
			//获取系统编码
			virtual std::string GetSystemCode() = 0;
			//获取系统名称
			virtual std::string GetSystemName() = 0;
			//获取超级狗编码
			virtual std::string GetSuperdogCode() = 0;
			//获取机器码
			virtual std::string GetMachineCode() = 0;
			//获取License key生成日期
			virtual std::string GetGenDate() = 0;
			//获取厂商代码
			virtual std::string GetVenderCode() = 0;
			//获取截止日期
			virtual std::string GetValidDate() = 0;
			//License状态
			virtual std::string GetStatus() = 0;
			//获取客户端授权数量
			virtual unsigned int GetClientNum() = 0;
			//获取有效天数
			virtual unsigned int GetValidDays() = 0;
			//LicenseDesp
			virtual std::string GetLicenseDesp() = 0;
		};

		typedef boost::shared_ptr<ILicense> ILicensePtr;
	}
}
