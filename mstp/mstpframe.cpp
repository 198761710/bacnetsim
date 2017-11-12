#include <stdio.h>
#include <string.h>
#include "mstpcrc.h"
#include "mstpframe.h"

MstpFrame::MstpFrame(void):start(buffer),index(0)
{
	memset(buffer, 0, size());
}
int MstpFrame::udiff(void)
{
	return t.udiff();
}
int MstpFrame::mdiff(void)
{
	return t.mdiff();
}
void MstpFrame::clear(void)
{
	index = 0;
	start = buffer;
}
int MstpFrame::size(void)
{
	return sizeof(buffer);
}
int MstpFrame::space(void)
{
	return size() - index;
}
int MstpFrame::length(void)
{
	return (buffer+index) - start;
}
int MstpFrame::headlength(void)
{
	return FrameNpdu;
}
int MstpFrame::datalength(void)
{
	if( length() <= headlength() )
	{
		return 0;
	}
	return length() - headlength();
}
unsigned char* MstpFrame::apdu(void)
{
	return &start[FrameApdu];
}
unsigned char* MstpFrame::npdu(void)
{
	return &start[FrameNpdu];
}
unsigned char* MstpFrame::frame(void)
{
	return start;
}
bool MstpFrame::push(unsigned char c)
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
bool MstpFrame::push(unsigned char *buf, int len)
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
bool MstpFrame::Check(void)
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
bool MstpFrame::CheckHead(void)
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
bool MstpFrame::CheckData(void)
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
void MstpFrame::ShowHead(void)
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
void MstpFrame::ShowData(void)
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
void MstpFrame::ShowFrame(void)
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
unsigned short MstpFrame::GetFrameHeader(void)
{
	return (start[FrameHeaderH] << 8) | (start[FrameHeaderL] << 0);
}
unsigned char MstpFrame::GetFrameType(void)
{
	return start[FrameType];
}
unsigned char MstpFrame::GetFrameDst(void)
{
	return start[FrameDst];
}
unsigned char MstpFrame::GetFrameSrc(void)
{
	return start[FrameSrc];
}
unsigned short MstpFrame::GetFrameDataLen(void)
{
	return (start[FrameDataLenH] << 8) | (start[FrameDataLenL]);
}
unsigned char MstpFrame::GetFrameHeadCrc(void)
{
	return start[FrameHeadCrc];
}
unsigned short MstpFrame::GetFrameDataCrc(void)
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
void MstpFrame::SetFrameHeader(unsigned short header)
{
	start[FrameHeaderH] = 0xff & (header >> 8);
	start[FrameHeaderL] = 0xff & (header >> 0);
}
void MstpFrame::SetFrameType(unsigned char type)
{
	start[FrameType] = type;
}
void MstpFrame::SetFrameDst(unsigned char dst)
{
	start[FrameDst] = dst;
}
void MstpFrame::SetFrameSrc(unsigned char src)
{
	start[FrameSrc] = src;
}
void MstpFrame::SetFrameDataLen(unsigned short len)
{
	start[FrameDataLenH] = (len >> 8);
	start[FrameDataLenL] = (len >> 0);
}
void MstpFrame::SetFrameHeadCrc(unsigned char crc)
{
	start[FrameHeadCrc] = crc;
}
void MstpFrame::SetFrameDataCrc(unsigned short crc)
{
	int len = GetFrameDataLen() % (size() - (start - buffer));

	start[len-2] = 0xff & (crc >> 0);
	start[len-1] = 0xff & (crc >> 8);
}
unsigned char MstpFrame::CalcHeadCrc(void)
{
	unsigned char crc8 = 0xFF;

	for(int i = FrameType; i < FrameHeadCrc; i++)
	{
		crc8 = CRC_Calc_Header(start[i], crc8);
	}
	return ~crc8;
}
unsigned short MstpFrame::CalcDataCrc(void)
{
	int len = length() % size() - 2;
	unsigned short crc16 = 0xFFFF;

	for(int i = FrameNpdu; i < len; i++)
	{
		crc16 = CRC_Calc_Data(start[i], crc16);
	}
	return ~crc16;
}
