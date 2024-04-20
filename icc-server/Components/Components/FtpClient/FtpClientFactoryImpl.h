namespace ICC
{
	namespace Ftp
	{
		class CFtpClientFactoryImpl :
			public IFtpClientFactory
		{
			ICCFactoryHeaderInitialize(CFtpClientFactoryImpl)
		public:
			virtual IFtpClientPtr CreateFtpClient();
		};
	}
}