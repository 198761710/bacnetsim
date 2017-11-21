#include <stdio.h>
#include <string.h>
#include "mstpcrc.h"
#include "recvframe.h"

RecvFrame::RecvFrame(void):start(buffer),index(0)
{
	memset(buffer, 0, size());
}
int RecvFrame::udiff(void)
{
	return t.udiff();
}
int RecvFrame::mdiff(void)
{
	return t.mdiff();
}
void RecvFrame::clear(void)
{
	index = 0;
	start = buffer;
}
int RecvFrame::size(void)
{
	return sizeof(buffer);
}
int RecvFrame::space(void)
{
	return size() - index;
}
int RecvFrame::length(void)
{
	return (buffer+index) - start;
}
int RecvFrame::headlength(void)
{
	return FrameNpdu;
}
int RecvFrame::datalength(void)
{
	if( length() <= headlength() )
	{
		return 0;
	}
	return length() - headlength();
}
unsigned char* RecvFrame::apdu(void)
{
	return &start[FrameApdu];
}
unsigned char* RecvFrame::npdu(void)
{
	return &start[FrameNpdu];
}
unsigned char* RecvFrame::data(void)
{
	return start;
}
bool RecvFrame::push(unsigned char c)
{
	if( space() < 1 )
	{
		return false;
	}
	if( (start - buffer) > (size() / 5) )
	{
		memmove(buffer, start, length());
		index = length()-1;
		start = buffer;
	}
	t.init();
	start[index++] = c;
	return true;
}
bool RecvFrame::push(unsigned char *buf, int len)
{
	if( len > space() )
	{
		return false;
	}
	if( (start - buffer) > (size() / 5) )
	{
		memmove(buffer, start, length());
		index = length() - 1;
		start = buffer;
	}
	t.init();
	memcpy(start, buf, len);
	index += len;
	return true;
}
bool RecvFrame::Check(void)
{
	if( CheckHead() && CheckData() )
	{
		return true;
	}
	if( t.mdiff() > 500 )
	{
		start++;
		t.init();
	}
	return false;
}
bool RecvFrame::CheckHead(void)
{
	if( length() < FrameNpdu )
	{
		return false;
	}
	if( 0x55 != start[FrameHeaderH] )
	{
		start++;
		t.init();
		return false;
	}
	if( 0xFF != start[FrameHeaderL] )
	{
		start++;
		t.init();
		return false;
	}
	if( CalcHeadCrc() != start[FrameHeadCrc] )
	{
		start++;
		t.init();
		return false;
	}
	if( length() > FrameMaxLen )
	{
		start++;
		t.init();
		return false;
	}
	return true;
}
bool RecvFrame::CheckData(void)
{
	if( length() == headlength() )
	{
		return (GetFrameDataLen() == 0);
	}
	if( (datalength()-2) != GetFrameDataLen() )
	{
		return false;
	}
	if( CalcDataCrc() != GetFrameDataCrc() )
	{
		start++;
		t.init();
		return false;
	}
	if( length() > FrameMaxLen )
	{
		start++;
		t.init();
		return false;
	}
	return true;
}
void RecvFrame::ShowHead(void)
{
	int len = headlength();

	if( len > 0 )
	{
		printf("[");
	}
	for(int i = 0; i < len; i++)
	{
		printf("%02X ", start[i]);
	}
	if( len > 0 )
	{
		printf("\b]\n");
	}
}
void RecvFrame::ShowData(void)
{
	int len = datalength();
	unsigned char *p = npdu();

	if( len > 0 )
	{
		printf("[");
	}
	for(int i = 0; i < len; i++)
	{
		printf("%02X ", p[i]);
	}
	if( len > 0 )
	{
		printf("\b]\n");
	}
}
void RecvFrame::showhex(void)
{
	ShowFrame();
}
void RecvFrame::ShowFrame(void)
{
	int len = length();
	unsigned char *p = start;

	if( len > 0 )
	{
		printf("[");
	}
	for(int i = 0; i < len; i++)
	{
		printf("%02X ", p[i]);
	}
	if( len > 0 )
	{
		printf("\b]\n");
	}

}
unsigned short RecvFrame::GetFrameHeader(void)
{
	return (start[FrameHeaderH] << 8) | (start[FrameHeaderL] << 0);
}
unsigned char RecvFrame::GetFrameType(void)
{
	return start[FrameType];
}
unsigned char RecvFrame::GetFrameDst(void)
{
	return start[FrameDst];
}
unsigned char RecvFrame::GetFrameSrc(void)
{
	return start[FrameSrc];
}
unsigned short RecvFrame::GetFrameDataLen(void)
{
	return (start[FrameDataLenH] << 8) | (start[FrameDataLenL]);
}
unsigned char RecvFrame::GetFrameHeadCrc(void)
{
	return start[FrameHeadCrc];
}
unsigned short RecvFrame::GetFrameDataCrc(void)
{
	int len = GetFrameDataLen();
	if( len == 0 )
	{
		return 0;
	}
	if( len < (datalength()-2) )
	{
		return 0;
	}
	len = length() % size();
	return (start[len-2] << 0) | (start[len-1] << 8);
}
void RecvFrame::SetFrameHeader(unsigned short header)
{
	start[FrameHeaderH] = 0xff & (header >> 8);
	start[FrameHeaderL] = 0xff & (header >> 0);
}
void RecvFrame::SetFrameType(unsigned char type)
{
	start[FrameType] = type;
}
void RecvFrame::SetFrameDst(unsigned char dst)
{
	start[FrameDst] = dst;
}
void RecvFrame::SetFrameSrc(unsigned char src)
{
	start[FrameSrc] = src;
}
void RecvFrame::SetFrameDataLen(unsigned short len)
{
	start[FrameDataLenH] = (len >> 8);
	start[FrameDataLenL] = (len >> 0);
}
void RecvFrame::SetFrameHeadCrc(unsigned char crc)
{
	start[FrameHeadCrc] = crc;
}
void RecvFrame::SetFrameDataCrc(unsigned short crc)
{
	int len = GetFrameDataLen() % (size() - (start - buffer));

	start[len-2] = 0xff & (crc >> 0);
	start[len-1] = 0xff & (crc >> 8);
}
unsigned char RecvFrame::CalcHeadCrc(void)
{
	unsigned char crc8 = 0xFF;

	for(int i = FrameType; i < FrameHeadCrc; i++)
	{
		crc8 = CRC_Calc_Header(start[i], crc8);
	}
	return ~crc8;
}
unsigned short RecvFrame::CalcDataCrc(void)
{
	int len = length() % size() - 2;
	unsigned short crc16 = 0xFFFF;

	for(int i = FrameNpdu; i < len; i++)
	{
		crc16 = CRC_Calc_Data(start[i], crc16);
	}
	return ~crc16;
}
