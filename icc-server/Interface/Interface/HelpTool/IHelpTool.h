#pragma once
#include <IResource.h>
namespace ICC
{
	namespace HelpTool
	{
		/*
		* class   工具类
		* author  w16314
		* purpose
		* note
		*/
		class IHelpTool :
			public IResource
		{
		public:
			
			//网络字节序转换接口
			virtual unsigned int HostToNetworkInt(unsigned int p_iValue) = 0;
			virtual unsigned short HostToNetworkShort(unsigned short p_sValue) = 0;
			virtual unsigned int NetworkToHostInt(unsigned int p_iValue) = 0;
			virtual unsigned short NetworkToHostShort(unsigned short p_sValue) = 0;
			//IP整数互转
			virtual unsigned int IpToInt(std::string p_strIp) = 0;		
			//
			virtual void Sleep(unsigned int p_iMSec) = 0;
			//递归获取指定目录下文件
			//************************************
			// Method:    ScanFilesRecursive
			// FullName:  ICC::HelpTool::IHelpTool::ScanFilesRecursive
			// Access:    virtual public 
			// Returns:   void
			// Qualifier:
			// Parameter: const std::string p_dir 
			// Parameter: const std::string p_filter 文件后缀过滤条件，为空代表全取，如：".txt",".xml"
			// Parameter: std::list<std::string> & p_Files
			//************************************
			virtual void ScanFilesRecursive(const std::string p_dir, const std::string p_filter, std::list<std::string>& p_Files) = 0;
		};

		typedef boost::shared_ptr<IHelpTool> IHelpToolPtr;
	}
}
