#ifndef MSTP_H
#define MSTP_H
#include "platform.h"

#define MAX_MSTP_PDU_SIZE 501

/*  The value 255 is used to denote broadcast when used as a */
/* destination address but is not allowed as a value for a station. */
#define MSTP_BROADCAST_ADDRESS 255

/* MS/TP Frame Type */
/* Frame Types 8 through 127 are reserved by ASHRAE. */
#define FRAME_TYPE_TOKEN 0
#define FRAME_TYPE_POLL_FOR_MASTER 1
#define FRAME_TYPE_REPLY_TO_POLL_FOR_MASTER 2
#define FRAME_TYPE_TEST_REQUEST 3
#define FRAME_TYPE_TEST_RESPONSE 4
#define FRAME_TYPE_BACNET_DATA_EXPECTING_REPLY 5
#define FRAME_TYPE_BACNET_DATA_NOT_EXPECTING_REPLY 6
#define FRAME_TYPE_REPLY_POSTPONED 7
/* Frame Types 128 through 255: Proprietary Frames */
/* These frames are available to vendors as proprietary (non-Bacnet) frames. */
/* The first two octets of the Data field shall specify the unique vendor */
/* identification code, most significant octet first, for the type of */
/* vendor-proprietary frame to be conveyed. The length of the data portion */
/* of a Proprietary frame shall be in the range of 2 to 501 octets. */
#define FRAME_TYPE_PROPRIETARY_MIN 128
#define FRAME_TYPE_PROPRIETARY_MAX 255

/* This parameter represents the value of the Max_Info_Frames property of */
/* the node's Device object. The value of Max_Info_Frames specifies the */
/* maximum number of information frames the node may send before it must */
/* pass the token. Max_Info_Frames may have different values on different */
/* nodes. This may be used to allocate more or less of the available link */
/* bandwidth to particular nodes. If Max_Info_Frames is not writable in a */
/* node, its value shall be 1. */
#define MAX_INFO_FRAMES 1

/* This parameter represents the value of the Max_Master property of the */
/* node's Device object. The value of Max_Master specifies the highest */
/* allowable address for master nodes. The value of Max_Master shall be */
/* less than or equal to 127. If Max_Master is not writable in a node, */
/* its value shall be 127. */
#define MSTP_MAX_MASTER 127

/* receive FSM states */
typedef enum
{
    MSTP_RECEIVE_STATE_IDLE,
    MSTP_RECEIVE_STATE_PREAMBLE,
    MSTP_RECEIVE_STATE_HEADER,
    MSTP_RECEIVE_STATE_HEADER_CRC,
    MSTP_RECEIVE_STATE_DATA,
    MSTP_RECEIVE_STATE_DATA_CRC,
} MSTP_RECEIVE_STATE;

/* master node FSM states */
typedef enum
{
    MSTP_MASTER_STATE_INITIALIZE,           /* 0 */
    MSTP_MASTER_STATE_IDLE,                 /* 1 */
    MSTP_MASTER_STATE_USE_TOKEN,            /* 2 */
    MSTP_MASTER_STATE_WAIT_FOR_REPLY,       /* 3 */
    MSTP_MASTER_STATE_DONE_WITH_TOKEN,      /* 4 */
    MSTP_MASTER_STATE_PASS_TOKEN,           /* 5 */
    MSTP_MASTER_STATE_NO_TOKEN,             /* 6 */
    MSTP_MASTER_STATE_POLL_FOR_MASTER,      /* 7 */
    MSTP_MASTER_STATE_ANSWER_DATA_REQUEST,  /* 8 */
} MSTP_MASTER_STATE;

typedef enum
{
    MSTP_SEND_NOTHING,
    MSTP_SEND_NO_WAIT,
    MSTP_SEND_AND_WAIT,
    MSTP_SEND_REPLY,
    MSTP_SEND_TEST_REQUEST,
    MSTP_SEND_TEST_RESPONSE,
    MSTP_SEND_ERROR
} MSTP_WRITE_TYPE;


/* The minimum number of DataAvailable or ReceiveError events that must be */
/* seen by a receiving node in order to declare the line "active": 4. */
#define Nmin_octets 4

/* Read errors and messages returned in "type" */
/* (bits belonging to frame type 0xff) */
/* no PDU */
#define READ_ERR_LEN          0x0100
#define READ_ERR_REPL_TMO     0x0200
#define READ_TRY_NEXT_BR      0x0300
#define READ_RESTART_PFM      0x0400
/* can contain a pdu */
#define READ_ERR_MISSED_FRAME 0x8000

#define WRITE_ERR_LEN         0x0200
#define WRITE_ERR_WRONG_TYPE  0x0400
#define WRITE_ERR_WRONG_MAC   0x0800
#define WRITE_ERR_PREEMTIED   0x1000

class SendFrame 
{
protected:
	unsigned char buffer[512];
	unsigned int m_length;

public:
	SendFrame(void);

public:
	void showhex(void);
	const unsigned char* data(void)const{return buffer;}
	const unsigned int length(void)const{return m_length;}

protected:
	bool MakeReadProperty(unsigned char dst,
						  unsigned char src,
						  unsigned char invokeId,
						  unsigned short object_type,
						  unsigned int object_instance,
						  unsigned char property);

	bool MakeWritePropertyReal(unsigned char dst,
						   unsigned char src,
						   unsigned char invokeId,
						   unsigned short object_type,
						   unsigned int object_instance,
						   unsigned char property,
						   REAL value);

	bool MakeWritePropertyEnum(unsigned char dst,
						   unsigned char src,
						   unsigned char invokeId,
						   unsigned short object_type,
						   unsigned int object_instance,
						   unsigned char property,
						   unsigned char value);

	bool MakeFrame(unsigned char frame_type, 
				  unsigned char dst, 
				  unsigned char src, 
				  unsigned char *data, 
				  unsigned short data_len);
public:
	bool ReadReal(unsigned char dst, 
				  unsigned char src, 
				  unsigned char id, 
				  unsigned int instance);

	bool WriteReal(unsigned char dst, 
				   unsigned char src, 
				   unsigned char id, 
				   unsigned int instance, 
				   float value);

	bool WriteEnum(unsigned char dst, 
				   unsigned char src, 
				   unsigned char id, 
				   unsigned int instance, 
				   unsigned char value);
};
#endif
