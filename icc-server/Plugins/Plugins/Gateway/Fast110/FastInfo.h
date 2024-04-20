#pragma once


#define DEVICE_STATE_SYNC				"device_state_sync"			//CTI设备状态同步
#define TALKSTATE						"talkstate"
#define HANGEUP							"hangupstate"

#define  TOPIC_CONFERENCE_SYNC			"topic_make_conference_sync"
#define  QUEUE_CTI_CONTROL				"queue_cti_control"
#define  MAKE_CONFERENCE_SYNC			"make_conference_sync"

#define ADD_CONFERENCE_PARTY_REQUEST	"add_conference_party_request"
#define DELETE_CONFERENCE_PARTY_REQUEST "delete_conference_party_request"

#define ADD_CONFERENCE_PARTY_RESPOND	"add_conference_party_respond"
#define DELETE_CONFERENCE_PARTY_RESPOND "delete_conference_party_respond"

#define	MAKE_CONFERENCE_SYNC			"make_conference_sync"

#define TARGET_DEVICE_TYPE_INSIDE		"inside"
#define TARGET_DEVICE_TYPE_OUTSIDE		"outside"
#define TARGET_DEVICE_TYPE_WIRELESS		"wireless"

#define CALL_DIRECTION_IN				"in"

#define RESULT_STATE_FAIL				"1"						//失败
#define RESULT_STATE_SUCCESS			"0"	

#define MQTYPE_TOPIC					"1"
#define MQTYPE_QUEUE					"0"

#define COUNFERENCE_COUNT				 "1"

#define SOCKET_MSG_SEPARATE				"$$"
#define SOCKET_MSG_HEADER				 0xAAAA
#define SOCKET_MSG_PLATFROMID 			 0
#define SOCKET_MSG_CMDTYPE				 4