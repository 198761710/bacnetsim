#ifndef __MSTPFRAME_H__
#define __MSTPFRAME_H__
#include <sys/time.h>
#include "timeoperator.h"

typedef enum
{
	FrameHeaderH = 0,
	FrameHeaderL = 1,
	FrameType = 2,
	FrameDst = 3,
	FrameSrc = 4,
	FrameDataLenH = 5,
	FrameDataLenL = 6,
	FrameHeadCrc = 7,
	FrameNpdu = 8+0,
	FrameApdu = 8+2,
	FrameMaxLen = 256,
}FrameIndex;

typedef enum
{
	FrameTypeToken = 0,
	FrameTypePollMaster = 1,
	FrameTypePollMasterAck = 2,
	FrameTypeTestRequest = 3,
	FrameTypeTestResponse = 4,
	FrameTypeDataRequest = 5,
	FrameTypeDataResponse = 6,
	FrameTypeReplyPostpened = 7,
}FrameTypeDef;

class MstpFrame
{
protected:
	TimeOperator t;
	signed int index;
	unsigned char *start;
	unsigned char buffer[1024];
public:
	MstpFrame(void);
public:
	int size(void);
	int mdiff(void);
	int udiff(void);
	void clear(void);
	int space(void);
	int length(void);
	int datalength(void);
	int headlength(void);
	bool push(unsigned char c);
	bool push(unsigned char *buf, int len);
	unsigned char* apdu(void);
	unsigned char* npdu(void);
	unsigned char* frame(void);
public:
	void ShowHead(void);
	void ShowData(void);
	void ShowFrame(void);
	bool Check(void);
	bool CheckHead(void);
	bool CheckData(void);
public:
	unsigned short GetFrameHeader(void);
	unsigned char GetFrameType(void);
	unsigned char GetFrameDst(void);
	unsigned char GetFrameSrc(void);
	unsigned short GetFrameDataLen(void);
	unsigned char GetFrameHeadCrc(void);
	unsigned short GetFrameDataCrc(void);
	unsigned char* GetData(void);
public:
	void SetFrameHeader(unsigned short header);
	void SetFrameType(unsigned char type);
	void SetFrameDst(unsigned char dst);
	void SetFrameSrc(unsigned char dst);
	void SetFrameDataLen(unsigned short len);
	void SetFrameHeadCrc(unsigned char crc);
	void SetFrameDataCrc(unsigned short crc);
public:
	unsigned char CalcHeadCrc(void);
	unsigned short CalcDataCrc(void);
};

#endif//__MSTPFRAME_H__
