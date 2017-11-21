#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include "conf.h"
#include "npdu.h"
#include "sendframe.h"
#include "bacenum.h"
#include "mstpcrc.h"
#include "make_apdu.h"
#include "platform.h"

#define Npoll 48
#define Nretry_token 1
#define Nmin_octets 4
#define Tno_token 500
#define Treply_delay_max 250
#define Treply_delay 200
#define Treply_timeout 255
#define Tslot 10
#define Tusage_delay 10
#define Tusage_timeout 30
#define AUTO_BOUD_BAUD_FOUND 100

SendFrame::SendFrame(void):m_length(0)
{
	for(unsigned int i = 0; i < sizeof(buffer); i++)
	{
		buffer[i] = 0;
	}
}

void SendFrame::showhex(void)
{
	printf("[");
	for(unsigned int i = 0; i < m_length; i++)
	{
		printf("%02X ", buffer[i]);
	}
	printf("\b]\n");
}

bool SendFrame::MakeReadProperty
(
	unsigned char dst,
	unsigned char src,
	unsigned char invokeId,
	unsigned short object_type,
	unsigned int object_instance,
	unsigned char property
)
{
	Bacnet_npdu_t  npdu_buffer[0x001];
	Bacnet_npdu_t* npdu = npdu_buffer;

	npdu = apdu_read_property(npdu,
							  invokeId,
							  object_type,
							  object_instance,
							  0,
							  0,
							  0,
							  property,
							  0);
	if( NULL == npdu )
	{
		return false;
	}
	add_npdu_header(npdu, 0, 0, 1, 0);
	return MakeFrame(FRAME_TYPE_BACNET_DATA_EXPECTING_REPLY,
					dst,
					src,
					npdu->data,
					npdu->data_len);
}

bool SendFrame::MakeWritePropertyReal
(
	unsigned char dst,
	unsigned char src,
	unsigned char invokeId,
	unsigned short object_type,
	unsigned int object_instance,
	unsigned char property,
	REAL value
)
{
	Bacnet_npdu_t  npdu_buffer[0x001];
	Bacnet_npdu_t* npdu = npdu_buffer;

	npdu = apdu_write_prop_real(npdu,
							  invokeId,
							  object_type,
							  object_instance,
							  property,
							  &value,
							  0,
							  0);
	if( NULL == npdu )
	{
		return false;
	}
	add_npdu_header(npdu, 0, 0, 1, 0);
	return MakeFrame(FRAME_TYPE_BACNET_DATA_EXPECTING_REPLY,
					dst,
					src,
					npdu->data,
					npdu->data_len);
}

bool SendFrame::MakeWritePropertyEnum
(
	unsigned char dst,
	unsigned char src,
	unsigned char invokeId,
	unsigned short object_type,
	unsigned int object_instance,
	unsigned char property,
	unsigned char value
)
{
	Bacnet_npdu_t  npdu_buffer[0x001];
	Bacnet_npdu_t* npdu = npdu_buffer;

	npdu = apdu_write_prop_enum(npdu,
							  invokeId,
							  object_type,
							  object_instance,
							  property,
							  &value,
							  0,
							  0);
	if( NULL == npdu )
	{
		return false;
	}
	add_npdu_header(npdu, 0, 0, 1, 0);
	return MakeFrame(FRAME_TYPE_BACNET_DATA_EXPECTING_REPLY,
					dst,
					src,
					npdu->data,
					npdu->data_len);
}


bool SendFrame::MakeFrame(unsigned char frame_type, 
						  unsigned char dst, 
						  unsigned char src, 
						  unsigned char *data, 
						  unsigned short data_len)
{
    unsigned int index = 0; 
    unsigned char crc8 = 0xFF; 
    unsigned short crc16 = 0xFFFF; 

    if( data_len < 8 )
	{
        return false;
	}

    buffer[0] = 0x55;
    buffer[1] = 0xFF;
    buffer[2] = frame_type;
    crc8 = CRC_Calc_Header(buffer[2],crc8);
    buffer[3] = dst;
    crc8 = CRC_Calc_Header(buffer[3],crc8);
    buffer[4] = src;
    crc8 = CRC_Calc_Header(buffer[4],crc8);
    buffer[5] = (unsigned char)(data_len / 256);
    crc8 = CRC_Calc_Header(buffer[5],crc8);
    buffer[6] = (unsigned char)(data_len % 256);
    crc8 = CRC_Calc_Header(buffer[6],crc8);
    buffer[7] = ~crc8;

    index = 8;

    while (data_len && data && (index < sizeof(buffer)))
    {
        buffer[index] = *data;
        crc16 = CRC_Calc_Data(buffer[index],crc16);
        data++;
        index++;
        data_len--;
    }
    if( index > 8 )
    {
        if ((index + 2) <= sizeof(buffer))
        {
            crc16 = ~crc16;
            buffer[index] = (unsigned char)((crc16 >> 0) & 0xff);
            index++;
            buffer[index] = (unsigned char)((crc16 >> 8) & 0xff);
            index++;
        }
        else
		{
            return false;
		}
    }

    return !!(m_length = index);
}

bool SendFrame::ReadReal(unsigned char dst, 
						 unsigned char src, 
						 unsigned char id, 
						 unsigned int instance)
{
	return MakeReadProperty(dst, 
							src, 
							id, 
							OBJECT_ANALOG_VALUE, 
							instance, 
							PROP_PRESENT_VALUE);
}

bool SendFrame::WriteReal(unsigned char dst, 
						  unsigned char src, 
						  unsigned char id, 
						  unsigned int instance, 
						  REAL value)
{
	return MakeWritePropertyReal(dst, 
								src, 
								id, 
								OBJECT_ANALOG_VALUE, 
								instance, 
								PROP_PRESENT_VALUE,
								value);
}

bool SendFrame::WriteEnum(unsigned char dst, 
						  unsigned char src, 
						  unsigned char id, 
						  unsigned int instance, 
						  unsigned char value)
{
	return MakeWritePropertyEnum(dst, 
								src, 
								id, 
								OBJECT_BINARY_VALUE, 
								instance, 
								PROP_PRESENT_VALUE,
								value);
}
