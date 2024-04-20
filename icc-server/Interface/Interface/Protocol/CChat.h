#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		namespace Chat
		{
			typedef enum Result_Code
			{
				Result_Unknown = -1,
				Result_Success = 0,
				Result_User_Not_Exist,
				Result_Group_Not_Exist,
				Result_Group_Already_Exist,
				Result_There_Is_No_Group,
				Result_User_Is_Already_Group_Member,
				Result_User_Is_Not_Group_Member,
				Result_User_Is_Already_Exist,

				Result_DB_Operater_Failed = 100

			}Result_Code;

 			class Group;
 			class Member;
 			class GroupInfo;

			class Member
			{
			public:
				std::string m_strStaffCode;
				std::string m_strStaffName;
			};
			typedef std::vector<Member> Members;

			class Group
			{
			public:
				std::string m_strGroupID;        //组id
				std::string m_strGroupName;      //组名
				std::string m_strAdminCode;
			};
			typedef std::vector<Group> Groups;

			class GroupInfo
			{
			public:
				std::string m_strGroupID;        //组id
				std::string m_strGroupName;      //组名
				std::string m_strAdminCode;      //群主
				Members m_vecMembers;
			};

			class P2PRecord
			{
			public:
				std::string m_strMsgID;        //消息id
				std::string m_strTime;         //时间
				std::string m_strSender;       //发送者
				std::string m_strSenderName;   //发送者名称
				std::string m_strReceiver;     //接收者
				std::string m_strReceiverName; //接收者名称
				std::string m_strSeatNo;       //席位号
				std::string m_strIsRead;       //是否已读（0：否， 1：是）
				std::string m_strContent;      //消息内容
				std::string m_strType;		   //消息类型
			};

			class GroupRecord
			{
			public:
				std::string m_strMsgID;        //消息id
				std::string m_strTime;         //时间
				std::string m_strSender;       //发送者
				std::string m_strSenderName;   //发送者名称
				std::string m_strGroupID;      //组ID
				std::string m_strGroupName;    //组名
				std::string m_strSeatNo;       //席位号
				std::string m_strContent;      //消息内容
				std::string m_strType;		   //消息类型

				Members m_unReadList; //未读列表
				Members m_readList; //已读列表
			};

			typedef std::map<std::string, P2PRecord> P2PRecords;
			typedef std::map<std::string, GroupRecord> GroupRecords;

			/************************************************************************/
			/*                    初始化聊天窗口                                    */
			/************************************************************************/

			class CInitChatWindow : public IRequest, public IRespond
			{
			public:
				virtual bool ParseString(std::string p_strJson, JsonParser::IJsonPtr p_pJson)
				{
					if (!m_oHeader.ParseString(p_strJson, p_pJson))
					{
						return false;
					}
					m_strStaffCode = p_pJson->GetNodeValue("/body/staff_code", "");
					m_strPageSize = p_pJson->GetNodeValue("/body/page_size", "");
					m_strPageIndex = p_pJson->GetNodeValue("/body/page_index", "");
					m_strMsgSize = p_pJson->GetNodeValue("/body/msg_size", "");
					return true;
				}

				virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
				{
					if (nullptr == p_pJson)
					{
						return "";
					}

					m_oHeader.SaveTo(p_pJson);
					p_pJson->SetNodeValue("/body/all_count", m_oBody.m_strAllCount);
					p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);

					unsigned int l_uiRecordIndex = 0;
					for (auto l_record : m_oBody.m_mapRecords)
					{
						std::string l_strRecordPath("/body/data/" + std::to_string(l_uiRecordIndex) + "/");
						p_pJson->SetNodeValue(l_strRecordPath + "item_id", l_record.second.m_strItemID);
						p_pJson->SetNodeValue(l_strRecordPath + "item_name", l_record.second.m_strItemName);
						p_pJson->SetNodeValue(l_strRecordPath + "last_time", l_record.second.m_strLastTime);
						p_pJson->SetNodeValue(l_strRecordPath + "last_sender", l_record.second.m_strLastSender);
						p_pJson->SetNodeValue(l_strRecordPath + "last_content", l_record.second.m_strLastContent);
						p_pJson->SetNodeValue(l_strRecordPath + "is_group_msg", l_record.second.m_strIsGroupMsg);
						p_pJson->SetNodeValue(l_strRecordPath + "unread_count", l_record.second.m_strUnReadCount);
						
						unsigned int l_uiGroupMgsIndex = 0;
						for (auto l_groupRecord : l_record.second.m_groupRecords)
						{
							std::string l_strMsgPath("/body/data/" + std::to_string(l_uiRecordIndex) + "/group_records/" + std::to_string(l_uiGroupMgsIndex) + "/");
							p_pJson->SetNodeValue(l_strMsgPath + "msg_id", l_groupRecord.second.m_strMsgID);
							p_pJson->SetNodeValue(l_strMsgPath + "time", l_groupRecord.second.m_strTime);
							p_pJson->SetNodeValue(l_strMsgPath + "sender", l_groupRecord.second.m_strSender);
							p_pJson->SetNodeValue(l_strMsgPath + "sender_name", l_groupRecord.second.m_strSenderName);
							p_pJson->SetNodeValue(l_strMsgPath + "group_id", l_groupRecord.second.m_strGroupID);
							p_pJson->SetNodeValue(l_strMsgPath + "group_name", l_groupRecord.second.m_strGroupName);
							p_pJson->SetNodeValue(l_strMsgPath + "seat_no", l_groupRecord.second.m_strSeatNo);
							p_pJson->SetNodeValue(l_strMsgPath + "content", l_groupRecord.second.m_strContent);
							p_pJson->SetNodeValue(l_strMsgPath + "type", l_groupRecord.second.m_strType);

							unsigned int l_uiReadIndex = 0;
							for (auto l_read : l_groupRecord.second.m_readList)
							{
								std::string l_strMsgPath("/body/data/" + std::to_string(l_uiRecordIndex) + "/group_records/" + std::to_string(l_uiGroupMgsIndex) + "/reads/" + std::to_string(l_uiReadIndex) + "/");
								p_pJson->SetNodeValue(l_strMsgPath + "staff_code", l_read.m_strStaffCode);
								p_pJson->SetNodeValue(l_strMsgPath + "staff_name", l_read.m_strStaffName);

								++l_uiReadIndex;
							}

							unsigned int l_uiUnReadIndex = 0;
							for (auto l_unread : l_groupRecord.second.m_unReadList)
							{
								std::string l_strMsgPath("/body/data/" + std::to_string(l_uiRecordIndex) + "/group_records/" + std::to_string(l_uiGroupMgsIndex) + "/unreads/" + std::to_string(l_uiUnReadIndex) + "/");
								p_pJson->SetNodeValue(l_strMsgPath + "staff_code", l_unread.m_strStaffCode);
								p_pJson->SetNodeValue(l_strMsgPath + "staff_name", l_unread.m_strStaffName);

								++l_uiUnReadIndex;
							}

							++l_uiGroupMgsIndex;
						}

						unsigned int l_uiP2PMgsIndex = 0;
						for (auto var : l_record.second.m_p2pRecords)
						{
							std::string l_strMsgPath("/body/data/" + std::to_string(l_uiRecordIndex) + "/p2p_records/" + std::to_string(l_uiP2PMgsIndex) + "/");
							p_pJson->SetNodeValue(l_strMsgPath + "msg_id", var.second.m_strMsgID);
							p_pJson->SetNodeValue(l_strMsgPath + "time", var.second.m_strTime);
							p_pJson->SetNodeValue(l_strMsgPath + "sender", var.second.m_strSender);
							p_pJson->SetNodeValue(l_strMsgPath + "sender_name", var.second.m_strSenderName);
							p_pJson->SetNodeValue(l_strMsgPath + "receiver", var.second.m_strReceiver);
							p_pJson->SetNodeValue(l_strMsgPath + "receiver_name", var.second.m_strReceiverName);
							p_pJson->SetNodeValue(l_strMsgPath + "seat_no", var.second.m_strSeatNo);
							p_pJson->SetNodeValue(l_strMsgPath + "is_read", var.second.m_strIsRead);
							p_pJson->SetNodeValue(l_strMsgPath + "content", var.second.m_strContent);
							p_pJson->SetNodeValue(l_strMsgPath + "type", var.second.m_strType);

							++l_uiP2PMgsIndex;
						}

						++l_uiRecordIndex;
					}

					return p_pJson->ToString();
				}

			public:
				std::string m_strStaffCode;
				std::string m_strPageSize;
				std::string m_strPageIndex;
				std::string m_strMsgSize;
			public:

				class CBody
				{
				public:
					std::string m_strAllCount;//符合条件总记录数
					std::string m_strCount;//返回总数量

					class Data
					{
					public:
						std::string m_strItemID;      
						std::string m_strItemName;
						std::string m_strLastTime;
						std::string m_strLastSender;
						std::string m_strLastContent;
						std::string m_strIsGroupMsg;
						std::string m_strUnReadCount;
						P2PRecords m_p2pRecords;
						GroupRecords m_groupRecords;
					};
					std::map<std::string, Data> m_mapRecords;
				};
				CHeader m_oHeader;
				CBody m_oBody;
			};

			/************************************************************************/
			/*                    创建群聊组                                         */
			/************************************************************************/
			class CAddChatGroup : public IRequest, public IRespond
			{
			public:
				virtual bool ParseString(std::string p_strJson, JsonParser::IJsonPtr p_pJson)
				{
					if (!m_oHeader.ParseString(p_strJson, p_pJson))
					{
						return false;
					}
					m_strOperator = p_pJson->GetNodeValue("/body/operator", "");
					m_strGroupName = p_pJson->GetNodeValue("/body/group_name", "");

					std::string l_strPath = "/body/members";
					unsigned int l_iCount = p_pJson->GetCount(l_strPath);
					for (unsigned int i = 0; i < l_iCount; ++i)
					{
						std::string l_strNum = std::to_string(i);
						Member l_member;
						l_member.m_strStaffCode = p_pJson->GetNodeValue(l_strPath + "/" + l_strNum + "/staff_code", "");
						l_member.m_strStaffName = p_pJson->GetNodeValue(l_strPath + "/" + l_strNum + "/staff_name", "");
						m_members.push_back(l_member);
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
					p_pJson->SetNodeValue("/body/group_id", m_oBody.m_strGroupID);

					return p_pJson->ToString();
				}

			public:
				std::string m_strOperator;
				std::string m_strGroupName;
				Members m_members;
			public:
				class CBody
				{
				public:
					std::string m_strGroupID;
				};
				CHeader m_oHeader;
				CBody m_oBody;
			};


			/************************************************************************/
			/*                    编辑群聊组                                         */
			/************************************************************************/
			class CEditChatGroup : public IRequest, public IRespond
			{
			public:
				virtual bool ParseString(std::string p_strJson, JsonParser::IJsonPtr p_pJson)
				{
					if (!m_oHeader.ParseString(p_strJson, p_pJson))
					{
						return false;
					}
					m_strOperator = p_pJson->GetNodeValue("/body/operator", "");
					m_strGroupID = p_pJson->GetNodeValue("/body/group_id", "");
					m_strGroupName = p_pJson->GetNodeValue("/body/group_name", "");
					return true;
				}

				virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
				{
					if (nullptr == p_pJson)
					{
						return "";
					}

					m_oHeader.SaveTo(p_pJson);
					p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);

					return p_pJson->ToString();
				}

			public:
				std::string m_strOperator;
				std::string m_strGroupID;
				std::string m_strGroupName;
			public:
				class CBody
				{
				public:
					std::string m_strResult;
				};
				CHeader m_oHeader;
				CBody m_oBody;
			};
		

			/************************************************************************/
			/*                    获取群聊组列表                                     */
			/************************************************************************/
			class CGetChatGroups : public IRequest, public IRespond
			{
			public:
				virtual bool ParseString(std::string p_strJson, JsonParser::IJsonPtr p_pJson)
				{
					if (!m_oHeader.ParseString(p_strJson, p_pJson))
					{
						return false;
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
					p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);

					unsigned int l_uiGroupIndex = 0;
					for (auto var : m_oBody.m_vecGroups)
					{
						std::string l_strMsgPath("/body/data/" + std::to_string(l_uiGroupIndex) + "/");
						p_pJson->SetNodeValue(l_strMsgPath + "group_id", var.m_strGroupID);
						p_pJson->SetNodeValue(l_strMsgPath + "group_name", var.m_strGroupName);
						p_pJson->SetNodeValue(l_strMsgPath + "admin_code", var.m_strAdminCode);

						++l_uiGroupIndex;
					}

					return p_pJson->ToString();
				}

			public:
				class CBody
				{
				public:
					std::string m_strResult;
					Groups m_vecGroups;
				};
				CHeader m_oHeader;
				CBody m_oBody;
			};


			/************************************************************************/
			/*                    获取群聊组详细信息                                 */
			/************************************************************************/
			class CGetChatGroupInfo : public IRequest, public IRespond
			{
			public:
				virtual bool ParseString(std::string p_strJson, JsonParser::IJsonPtr p_pJson)
				{
					if (!m_oHeader.ParseString(p_strJson, p_pJson))
					{
						return false;
					}
					m_strGroupID = p_pJson->GetNodeValue("/body/group_id", "");

					return true;
				}

				virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
				{
					if (nullptr == p_pJson)
					{
						return "";
					}

					m_oHeader.SaveTo(p_pJson);
					p_pJson->SetNodeValue("/body/group_id", m_oBody.m_groupInfo.m_strGroupID);
					p_pJson->SetNodeValue("/body/group_name", m_oBody.m_groupInfo.m_strGroupName);
					p_pJson->SetNodeValue("/body/admin_code", m_oBody.m_groupInfo.m_strAdminCode);

					unsigned int l_uiMemberIndex = 0;
					for (auto l_member : m_oBody.m_groupInfo.m_vecMembers)
					{
						std::string l_strMemberPath("/body/members/" + std::to_string(l_uiMemberIndex) + "/");
						p_pJson->SetNodeValue(l_strMemberPath + "staff_code", l_member.m_strStaffCode);
						p_pJson->SetNodeValue(l_strMemberPath + "staff_name", l_member.m_strStaffName);
						++l_uiMemberIndex;
					}
					return p_pJson->ToString();
				}

			public:
				std::string m_strGroupID;
			public:
				class CBody
				{
				public:
					GroupInfo m_groupInfo;
				};
				CHeader m_oHeader;
				CBody m_oBody;
			};

			/************************************************************************/
			/*                    添加群聊成员                                      */
			/************************************************************************/
			class CAddChatGroupMember : public IRequest, public IRespond
			{
			public:
				virtual bool ParseString(std::string p_strJson, JsonParser::IJsonPtr p_pJson)
				{
					if (!m_oHeader.ParseString(p_strJson, p_pJson))
					{
						return false;
					}
					m_strOperator = p_pJson->GetNodeValue("/body/operator", "");
					m_strGroupID = p_pJson->GetNodeValue("/body/group_id", "");

					std::string l_strPath = "/body/members";
					unsigned int l_iCount = p_pJson->GetCount(l_strPath);
					for (unsigned int i = 0; i < l_iCount; ++i)
					{
						std::string l_strNum = std::to_string(i);
						Member l_member;
						l_member.m_strStaffCode = p_pJson->GetNodeValue(l_strPath + "/" + l_strNum + "/staff_code", "");
						l_member.m_strStaffName = p_pJson->GetNodeValue(l_strPath + "/" + l_strNum + "/staff_name", "");
						m_members.push_back(l_member);
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
					p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);

					return p_pJson->ToString();
				}

			public:
				std::string m_strOperator;
				std::string m_strGroupID;
				Members m_members;
			public:
				class CBody
				{
				public:
					std::string m_strResult;
				};
				CHeader m_oHeader;
				CBody m_oBody;
			};


			/************************************************************************/
			/*                    移出群聊组列表                                     */
			/************************************************************************/
			class CRemoveChatGroupMember : public IRequest, public IRespond
			{
			public:
				virtual bool ParseString(std::string p_strJson, JsonParser::IJsonPtr p_pJson)
				{
					if (!m_oHeader.ParseString(p_strJson, p_pJson))
					{
						return false;
					}
					m_strOperator = p_pJson->GetNodeValue("/body/operator", "");
					m_strGroupID = p_pJson->GetNodeValue("/body/group_id", "");
					m_strStaffCode = p_pJson->GetNodeValue("/body/staff_code", "");

					return true;
				}

				virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
				{
					if (nullptr == p_pJson)
					{
						return "";
					}

					m_oHeader.SaveTo(p_pJson);
					p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);

					return p_pJson->ToString();
				}

			public:
				std::string m_strOperator;
				std::string m_strGroupID;
				std::string m_strStaffCode;
			public:
				class CBody
				{
				public:
					std::string m_strResult;
				};
				CHeader m_oHeader;
				CBody m_oBody;
			};

			/************************************************************************/
			/*                    退出群聊组列表                                     */
			/************************************************************************/
			class CExitChatGroup : public IRequest, public IRespond
			{
			public:
				virtual bool ParseString(std::string p_strJson, JsonParser::IJsonPtr p_pJson)
				{
					if (!m_oHeader.ParseString(p_strJson, p_pJson))
					{
						return false;
					}
					m_strStaffCode = p_pJson->GetNodeValue("/body/staff_code", "");
					m_strGroupID = p_pJson->GetNodeValue("/body/group_id", "");

					return true;
				}

				virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
				{
					if (nullptr == p_pJson)
					{
						return "";
					}

					m_oHeader.SaveTo(p_pJson);
					p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);

					return p_pJson->ToString();
				}

			public:
				std::string m_strStaffCode;
				std::string m_strGroupID;
			public:
				class CBody
				{
				public:
					std::string m_strResult;
				};
				CHeader m_oHeader;
				CBody m_oBody;
			};


			/************************************************************************/
			/*                    发送聊天消息                                       */
			/************************************************************************/
			class CSendChatMsg : public IRequest, public IRespond
			{
			public:
				virtual bool ParseString(std::string p_strJson, JsonParser::IJsonPtr p_pJson)
				{
					if (!m_oHeader.ParseString(p_strJson, p_pJson))
					{
						return false;
					}
					m_strSender = p_pJson->GetNodeValue("/body/sender", "");
					m_strReceiver = p_pJson->GetNodeValue("/body/receiver", "");
					m_strSeatNo = p_pJson->GetNodeValue("/body/seat_no", "");
					m_strTime = p_pJson->GetNodeValue("/body/time", "");
					m_strContent = p_pJson->GetNodeValue("/body/content", "");
					m_strIsGroupMsg = p_pJson->GetNodeValue("/body/is_group_msg", "");
					m_strType = p_pJson->GetNodeValue("/body/type", "");

					return true;
				}

				virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
				{
					if (nullptr == p_pJson)
					{
						return "";
					}

					m_oHeader.SaveTo(p_pJson);
					p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);

					return p_pJson->ToString();
				}

			public:
				std::string m_strSender;     //发送者
				std::string m_strReceiver;   //接收者
				std::string m_strSeatNo;     //席位号
				std::string m_strTime;       //发送时间
				std::string m_strContent;    //消息内容
				std::string m_strIsGroupMsg; //是否是群消息(0:否,1:是)
				std::string m_strType;		 //消息类型

			public:
				class CBody
				{
				public:
					std::string m_strResult;
				};
				CHeader m_oHeader;
				CBody m_oBody;
			};

			/************************************************************************/
			/*                    转发点对点消息                                     */
			/************************************************************************/
			class CTransforP2PChatMsg : public IRespond
			{
			public:

				virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
				{
					if (nullptr == p_pJson)
					{
						return "";
					}

					m_oHeader.SaveTo(p_pJson);
					p_pJson->SetNodeValue("/body/msg", m_oBody.m_strMsgID);
					p_pJson->SetNodeValue("/body/sender", m_oBody.m_strSender);
					p_pJson->SetNodeValue("/body/sender_name", m_oBody.m_strSender);
					p_pJson->SetNodeValue("/body/receiver", m_oBody.m_strReceiver);
					p_pJson->SetNodeValue("/body/receiver_name", m_oBody.m_strReceiverName);
					p_pJson->SetNodeValue("/body/seat_no", m_oBody.m_strSeatNo);
					p_pJson->SetNodeValue("/body/time", m_oBody.m_strTime);
					p_pJson->SetNodeValue("/body/content", m_oBody.m_strContent);
					p_pJson->SetNodeValue("/body/type", m_oBody.m_strType);

					return p_pJson->ToString();
				}

			public:
				class CBody
				{
				public:
					std::string m_strMsgID;			//消息id
					std::string m_strSender;		//发送者
					std::string m_strSenderName;    //发送者
					std::string m_strReceiver;		//接收者
					std::string m_strReceiverName;  //接收者
					std::string m_strSeatNo;		//席位号
					std::string m_strTime;			//发送时间
					std::string m_strContent;		//消息内容
					std::string m_strType;			//消息类型
				};
				CHeader m_oHeader;
				CBody m_oBody;
			};

			/************************************************************************/
			/*                    转发组消息                                         */
			/************************************************************************/
			class CTransforGroupChatMsg : public IRespond
			{
			public:

				virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
				{
					if (nullptr == p_pJson)
					{
						return "";
					}

					m_oHeader.SaveTo(p_pJson);

					p_pJson->SetNodeValue("/body/msg", m_oBody.m_strMsgID);
					p_pJson->SetNodeValue("/body/sender", m_oBody.m_strSender);
					p_pJson->SetNodeValue("/body/sender_name", m_oBody.m_strSender);
					p_pJson->SetNodeValue("/body/group_id", m_oBody.m_strGroupID);
					p_pJson->SetNodeValue("/body/group_name", m_oBody.m_strGroupName);
					p_pJson->SetNodeValue("/body/seat_no", m_oBody.m_strSeatNo);
					p_pJson->SetNodeValue("/body/time", m_oBody.m_strTime);
					p_pJson->SetNodeValue("/body/content", m_oBody.m_strContent);
					p_pJson->SetNodeValue("/body/type", m_oBody.m_strType);

					return p_pJson->ToString();
				}

			public:
				class CBody
				{
				public:
					std::string m_strMsgID;      //消息id
					std::string m_strSender;     //发送者
					std::string m_strSenderName;     //发送者
					std::string m_strGroupID;    //组ID
					std::string m_strGroupName;  //组名
					std::string m_strSeatNo;     //席位号
					std::string m_strTime;       //发送时间
					std::string m_strContent;    //消息内容
					std::string m_strType;		 //消息类型
				};
				CHeader m_oHeader;
				CBody m_oBody;
			};

			/************************************************************************/
			/*                    获取点对点聊天记录                                 */
			/************************************************************************/
			class CGetP2PChatRecords : public IRequest, public IRespond
			{
			public:
				virtual bool ParseString(std::string p_strJson, JsonParser::IJsonPtr p_pJson)
				{
					if (!m_oHeader.ParseString(p_strJson, p_pJson))
					{
						return false;
					}
					m_strSender = p_pJson->GetNodeValue("/body/sender", "");
					m_strReceiver = p_pJson->GetNodeValue("/body/receiver", "");
					m_strPageSize = p_pJson->GetNodeValue("/body/page_size", "");
					m_strPageIndex = p_pJson->GetNodeValue("/body/page_index", "");

					return true;
				}

				virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
				{
					if (nullptr == p_pJson)
					{
						return "";
					}

					m_oHeader.SaveTo(p_pJson);

					p_pJson->SetNodeValue("/body/all_count", m_oBody.m_strAllCount);
					p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);

					unsigned int l_uiRecordIndex = 0;
					for (auto l_record : m_oBody.m_p2pRecords)
					{
						std::string l_strRecordPath("/body/data/" + std::to_string(l_uiRecordIndex) + "/");

						p_pJson->SetNodeValue(l_strRecordPath + "msg_id", l_record.first);
						p_pJson->SetNodeValue(l_strRecordPath + "time", l_record.second.m_strTime);
						p_pJson->SetNodeValue(l_strRecordPath + "sender", l_record.second.m_strSender);
						p_pJson->SetNodeValue(l_strRecordPath + "sender_name", l_record.second.m_strSenderName);
						p_pJson->SetNodeValue(l_strRecordPath + "receiver", l_record.second.m_strReceiver);
						p_pJson->SetNodeValue(l_strRecordPath + "receiver_name", l_record.second.m_strReceiverName);
						p_pJson->SetNodeValue(l_strRecordPath + "seat_no", l_record.second.m_strSeatNo);
						p_pJson->SetNodeValue(l_strRecordPath + "is_read", l_record.second.m_strIsRead);
						p_pJson->SetNodeValue(l_strRecordPath + "content", l_record.second.m_strContent);
						p_pJson->SetNodeValue(l_strRecordPath + "type", l_record.second.m_strType);

						++l_uiRecordIndex;
					}

					return p_pJson->ToString();
				}

			public:
				std::string m_strSender;     //发送者
				std::string m_strReceiver;   //接收者
				std::string m_strPageSize;   //发送时间
				std::string m_strPageIndex;  //消息内容
			public:
				class CBody
				{
				public:
					std::string m_strAllCount;
					std::string m_strCount;

					class Record
					{
					public:
						P2PRecord m_p2pRecord;
					};

					P2PRecords m_p2pRecords;
				};
				CHeader m_oHeader;
				CBody m_oBody;
			};


			/************************************************************************/
			/*                    获取群聊天记录                                     */
			/************************************************************************/
			class CGetGroupChatRecords : public IRequest, public IRespond
			{
			public:
				virtual bool ParseString(std::string p_strJson, JsonParser::IJsonPtr p_pJson)
				{
					if (!m_oHeader.ParseString(p_strJson, p_pJson))
					{
						return false;
					}
					m_strGroupID = p_pJson->GetNodeValue("/body/group_id", "");
					m_strPageSize = p_pJson->GetNodeValue("/body/page_size", "");
					m_strPageIndex = p_pJson->GetNodeValue("/body/page_index", "");

					return true;
				}

				virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
				{
					if (nullptr == p_pJson)
					{
						return "";
					}

					m_oHeader.SaveTo(p_pJson);

					p_pJson->SetNodeValue("/body/all_count", m_oBody.m_strAllCount);
					p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);

					unsigned int l_uiRecordIndex = 0;
					for (auto l_record : m_oBody.m_groupRecords)
					{
						std::string l_strRecordPath("/body/data/" + std::to_string(l_uiRecordIndex) + "/");
						p_pJson->SetNodeValue(l_strRecordPath + "msg_id", l_record.second.m_strMsgID);
						p_pJson->SetNodeValue(l_strRecordPath + "time", l_record.second.m_strTime);

						p_pJson->SetNodeValue(l_strRecordPath + "sender", l_record.second.m_strSender);
						p_pJson->SetNodeValue(l_strRecordPath + "sender_name", l_record.second.m_strSenderName);
						p_pJson->SetNodeValue(l_strRecordPath + "group_id", l_record.second.m_strGroupID);
						p_pJson->SetNodeValue(l_strRecordPath + "group_name", l_record.second.m_strGroupName);
						p_pJson->SetNodeValue(l_strRecordPath + "seat_no", l_record.second.m_strSeatNo);
						p_pJson->SetNodeValue(l_strRecordPath + "type", l_record.second.m_strType);
						p_pJson->SetNodeValue(l_strRecordPath + "content", l_record.second.m_strContent);

						unsigned int l_uiReadIndex = 0;
						for (auto l_read : l_record.second.m_readList)
						{
							std::string l_strMsgPath("/body/data/" + std::to_string(l_uiRecordIndex) + "/reads/" + std::to_string(l_uiReadIndex) + "/");
							p_pJson->SetNodeValue(l_strMsgPath + "staff_code", l_read.m_strStaffCode);
							p_pJson->SetNodeValue(l_strMsgPath + "staff_name", l_read.m_strStaffName);

							++l_uiReadIndex;
						}

						unsigned int l_uiUnReadIndex = 0;
						for (auto l_unread : l_record.second.m_unReadList)
						{
							std::string l_strMsgPath("/body/data/" + std::to_string(l_uiRecordIndex) + "/unreads/" + std::to_string(l_uiUnReadIndex) + "/");
							p_pJson->SetNodeValue(l_strMsgPath + "staff_code", l_unread.m_strStaffCode);
							p_pJson->SetNodeValue(l_strMsgPath + "staff_name", l_unread.m_strStaffName);

							++l_uiUnReadIndex;
						}

						++l_uiRecordIndex;
					}

					return p_pJson->ToString();
				}

			public:
				std::string m_strGroupID;    //群ID
				std::string m_strPageSize;   //发送时间
				std::string m_strPageIndex;  //消息内容
			public:
				class CBody
				{
				public:
					std::string m_strAllCount;
					std::string m_strCount;
					std::string m_strSender;
					std::string m_strGroupID;
					std::string m_strSeatID;

					class Record
					{
					public:
						GroupRecord m_groupRecord;
					};

					GroupRecords m_groupRecords;
				};
				CHeader m_oHeader;
				CBody m_oBody;
			};

			/************************************************************************/
			/*                    退出群聊组列表                                     */
			/************************************************************************/
			class CSetMsgState : public IRequest, public IRespond
			{
			public:
				virtual bool ParseString(std::string p_strJson, JsonParser::IJsonPtr p_pJson)
				{
					if (!m_oHeader.ParseString(p_strJson, p_pJson))
					{
						return false;
					}
					m_strMsgID = p_pJson->GetNodeValue("/body/msg_id", "");
					m_strReceiver = p_pJson->GetNodeValue("/body/receiver", "");

					return true;
				}

				virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
				{
					if (nullptr == p_pJson)
					{
						return "";
					}

					m_oHeader.SaveTo(p_pJson);
					p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);

					return p_pJson->ToString();
				}

			public:
				std::string m_strMsgID;
				std::string m_strReceiver;
			public:
				class CBody
				{
				public:
					std::string m_strResult;
				};
				CHeader m_oHeader;
				CBody m_oBody;
			};

			/************************************************************************/
			/*                    同步群组信息                                      */
			/************************************************************************/
			class CSyncGroupInfo : public IRequest, public IRespond
			{
			public:
				virtual bool ParseString(std::string p_strJson, JsonParser::IJsonPtr p_pJson)
				{
					return true;
				}

				virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
				{
					if (nullptr == p_pJson)
					{
						return "";
					}

					m_oHeader.SaveTo(p_pJson);
					p_pJson->SetNodeValue("/body/group_id", m_oBody.m_groupInfo.m_strGroupID);
					p_pJson->SetNodeValue("/body/group_name", m_oBody.m_groupInfo.m_strGroupName);
					p_pJson->SetNodeValue("/body/sync_type", m_oBody.m_strSyncType);

					unsigned int l_uiMemberIndex = 0;
					for (auto l_member : m_oBody.m_groupInfo.m_vecMembers)
					{
						std::string l_strMemberPath("/body/members/" + std::to_string(l_uiMemberIndex) + "/");
						p_pJson->SetNodeValue(l_strMemberPath + "staff_code", l_member.m_strStaffCode);
						p_pJson->SetNodeValue(l_strMemberPath + "staff_name", l_member.m_strStaffName);
						++l_uiMemberIndex;
					}
					return p_pJson->ToString();
				}

			public:
				class CBody
				{
				public:
					std::string m_strSyncType;//同步类型 1：创建群2：移除群成员3：退出群4：修改群名称
					GroupInfo m_groupInfo;
				};
				CHeader m_oHeader;
				CBody m_oBody;
			};
		}
	}
}
