#pragma once

#include <string>
#include <Json/IJsonFactory.h>
#include "Protocol/CChat.h"

namespace ICC
{
	//using namespace ICC::PROTOCOL::Chat;

	typedef ICC::PROTOCOL::Chat::CInitChatWindow::CBody ChatInitWindow;
	typedef ICC::PROTOCOL::Chat::CInitChatWindow::CBody::Data Record;
	typedef ICC::PROTOCOL::Chat::P2PRecord P2PRecord;
	typedef ICC::PROTOCOL::Chat::GroupRecord GroupRecord;
	typedef ICC::PROTOCOL::Chat::P2PRecords P2PRecords;
	typedef ICC::PROTOCOL::Chat::GroupRecords GroupRecords;

	typedef ICC::PROTOCOL::Chat::Group ChatGroup;

	typedef ICC::PROTOCOL::Chat::Groups ChatGroups;
	typedef ICC::PROTOCOL::Chat::GroupInfo ChatGroupInfo;
	typedef ICC::PROTOCOL::Chat::Member ChatGroupMember;
	typedef ICC::PROTOCOL::Chat::CSendChatMsg CSendChatMsg;
	typedef ICC::PROTOCOL::Chat::CTransforP2PChatMsg CTransforP2PChatMsg;
	typedef ICC::PROTOCOL::Chat::CTransforGroupChatMsg CTransforGroupChatMsg;
	typedef ICC::PROTOCOL::Chat::Member Member;
	typedef ICC::PROTOCOL::Chat::Members Members;
	typedef ICC::PROTOCOL::Chat::CSyncGroupInfo CSyncGroupInfo;

	class CRegister
	{

	public:
		std::string ToJson(JsonParser::IJsonPtr p_pJson)
		{
			std::string l_strRegisterInfo("");
			if (p_pJson)
			{
				p_pJson->SetNodeValue("/client_id", m_strClientID);
				p_pJson->SetNodeValue("/client_name", m_strClientName);
				p_pJson->SetNodeValue("/client_type", m_strClientType);
				p_pJson->SetNodeValue("/client_ip", m_strClientIP);

				l_strRegisterInfo = p_pJson->ToString();
			}
			return l_strRegisterInfo;
		}

		bool Parse(std::string l_strRegisterInfo, JsonParser::IJsonPtr p_pJson)
		{
			if (!p_pJson || !p_pJson->LoadJson(l_strRegisterInfo))
			{
				return false;
			}

			m_strClientID = p_pJson->GetNodeValue("/client_id", "");
			m_strClientName = p_pJson->GetNodeValue("/client_name", "");
			m_strClientType = p_pJson->GetNodeValue("/client_type", "");
			m_strClientIP = p_pJson->GetNodeValue("/client_ip", "");
			return true;
		}

	public:
		std::string m_strClientType;
		std::string m_strClientName;
		std::string m_strClientID;
		std::string m_strClientIP;
	};

}