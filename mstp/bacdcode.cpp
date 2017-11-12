/*####COPYRIGHTBEGIN####
 -------------------------------------------
 Copyright (C) 2004 Steve Karg

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to:
 The Free Software Foundation, Inc.
 59 Temple Place - Suite 330
 Boston, MA  02111-1307, USA.

 As a special exception, if other files instantiate templates or
 use macros or inline functions from this file, or you compile
 this file and link it with other works to produce a work based
 on this file, this file does not by itself cause the resulting
 work to be covered by the GNU General Public License. However
 the source code for this file must still be made available in
 accordance with section (3) of the GNU General Public License.

 This exception does not invalidate any other reasons why a work
 based on this file might be covered by the GNU General Public
 License.

 Modified by Kåre Särs (kare.sars@wapice.com) March 2005
 -------------------------------------------
####COPYRIGHTEND####*/

#include <string.h>
#include "platform.h"
#include "conf.h"
#include "bacdcode.h"
#include "bacenum.h"

#ifndef MAX_APDU_LEN
#define MAX_APDU_LEN 480
#endif

#define TAG_CLASS_BIT (0x08)

/**********************************************************************/
unsigned short bac_strlen(unsigned char *str)
{
    unsigned char *s=str;

    while(*str++);
    return (str - s - 1);
}

/* copy four bytes into an 32 bit integer and return the number of bytes used */
/**********************************************************************/
unsigned char chars_to_int32(unsigned char *ch_str, unsigned int *val)
{
    union
    {
        unsigned char byte[4];
        unsigned int value;
    } data = {{0}};

#ifdef IS_BIGENDIAN
    data.byte[0] = ch_str[0];
    data.byte[1] = ch_str[1];
    data.byte[2] = ch_str[2];
    data.byte[3] = ch_str[3];
#else
    data.byte[0] = ch_str[3];
    data.byte[1] = ch_str[2];
    data.byte[2] = ch_str[1];
    data.byte[3] = ch_str[0];
#endif
    *val=data.value;
    return 4;
}

/* copy two bytes into an 16 bit integer and return the number of bytes used */
/**********************************************************************/
unsigned char chars_to_int16(unsigned char *ch_str, unsigned short *val)
{
    union
    {
        unsigned char byte[2];
        unsigned short value;
    } data = { { 0}};

#ifdef IS_BIGENDIAN
    data.byte[0] = ch_str[0];
    data.byte[1] = ch_str[1];
#else
    data.byte[0] = ch_str[1];
    data.byte[1] = ch_str[0];
#endif
    *val=data.value;
    return 2;
}

/* copy four bytes from an 32 bit integer to an array of chars and */
/* return the number of bytes used */
/**********************************************************************/
unsigned char int32_to_chars(unsigned char *ch_str, unsigned int val)
{
	if( ch_str )
	{
		ch_str[0] = 0xFF & (val >> 24);
		ch_str[1] = 0xFF & (val >> 16);
		ch_str[2] = 0xFF & (val >>  8);
		ch_str[3] = 0xFF & (val >>  0);
	}
    return 4;
}

/* copy four bytes from an 16 bit integer to an array of chars and */
/* return the number of bytes used */
/**********************************************************************/
unsigned char int16_to_chars(unsigned char *ch_str, unsigned short val)
{
	if( ch_str )
	{
		ch_str[0] = 0xFF & (val >> 8);
		ch_str[1] = 0xFF & (val >> 0);
	}
    return 2;
}

/* NOTE: byte order plays a role in decoding multibyte values */
/* http://www.unixpapa.com/incnote/byteorder.html */

/* max-segments-accepted
   B'000'      Unspecified number of segments accepted.
   B'001'      2 segments accepted.
   B'010'      4 segments accepted.
   B'011'      8 segments accepted.
   B'100'      16 segments accepted.
   B'101'      32 segments accepted.
   B'110'      64 segments accepted.
   B'111'      Greater than 64 segments accepted.
*/

/* max-APDU-length-accepted
   B'0000'  Up to MinimumMessageSize (50 octets)
   B'0001'  Up to 128 octets
   B'0010'  Up to 206 octets (fits in a LonTalk frame)
   B'0011'  Up to 480 octets (fits in an ARCNET frame)
   B'0100'  Up to 1024 octets
   B'0101'  Up to 1476 octets (fits in an ISO 8802-3 frame)
   B'0110'  reserved by ASHRAE
   B'0111'  reserved by ASHRAE
   B'1000'  reserved by ASHRAE
   B'1001'  reserved by ASHRAE
   B'1010'  reserved by ASHRAE
   B'1011'  reserved by ASHRAE
   B'1100'  reserved by ASHRAE
   B'1101'  reserved by ASHRAE
   B'1110'  reserved by ASHRAE
   B'1111'  reserved by ASHRAE
*/
/* from clause 20.1.2.4 max-segments-accepted */
/* and clause 20.1.2.5 max-APDU-length-accepted */
/* returns the encoded octet */
/**********************************************************************/
unsigned char encode_max_segs_max_apdu(unsigned char max_segs, unsigned short max_apdu)
{
    unsigned char octet = 0;

    if (max_segs < 2)
        octet = 0;
    else if (max_segs < 4)
        octet = 0x10;
    else if (max_segs < 8)
        octet = 0x20;
    else if (max_segs < 16)
        octet = 0x30;
    else if (max_segs < 32)
        octet = 0x40;
    else if (max_segs < 64)
        octet = 0x50;
    else if (max_segs == 64)
        octet = 0x60;
    else
        octet = 0x70;

    /* max_apdu must be 50 octets minimum */
    if (max_apdu <= 50)
        octet |= 0x00;
    else if (max_apdu <= 128)
        octet |= 0x01;
    /*fits in a LonTalk frame */
    else if (max_apdu <= 206)
        octet |= 0x02;
    /*fits in an ARCNET or MS/TP frame */
    else if (max_apdu <= 480)
        octet |= 0x03;
    else if (max_apdu <= 1024)
        octet |= 0x04;
    /* fits in an ISO 8802-3 frame */
    else if (max_apdu <= 1476)
        octet |= 0x05;


    return octet;
}


/* from clause 20.2.1 General Rules for Encoding Bacnet Tags */
/* returns the number of apdu bytes consumed */
/**********************************************************************/
unsigned char encode_tag(unsigned char *apdu, unsigned char tag_number, bool context_specific, unsigned int len_value_type)
{
    unsigned char len = 1;                /* return value */

    apdu[0] = 0;
    if (context_specific == TRUE) apdu[0] = TAG_CLASS_BIT;

    /* additional tag byte after this byte */
    /* for extended tag byte */
    if (tag_number <= 14)
    {
        apdu[0] |= (tag_number << 4);
    }
    else
    {
        apdu[0] |= 0xF0;
        apdu[1] = tag_number;
        len++;
    }

    /* NOTE: additional len byte(s) after extended tag byte */
    /* if larger than 4 */
    if (len_value_type <= 4)
	{
        apdu[0] |= len_value_type;
	}
    else
    {
        apdu[0] |= 5;
        if (len_value_type <= 253)
        {
            apdu[len] = len_value_type & 0xff;
            len++;
        }
        else if (len_value_type <= 65535)
        {
            apdu[len] = 254;
            len++;
            len += int16_to_chars(&apdu[len], len_value_type & 0xffff);
        }
        else
        {
            apdu[len] = 255;
            len++;
            len += int32_to_chars(&apdu[len], len_value_type);
        }
    }

    return len;
}

/* from clause 20.2.1.3.2 Constructed Data */
/* returns the number of apdu bytes consumed */
/**********************************************************************/
unsigned char encode_opening_tag(unsigned char *apdu, unsigned char tag_number)
{
    unsigned char len = 1;

    apdu[0] = TAG_CLASS_BIT;
    /* additional tag byte after this byte for extended tag byte */
    if (tag_number <= 14)
	{
        apdu[0] |= (tag_number << 4);
	}
    else
    {
        apdu[0] |= 0xF0;
        apdu[1] = tag_number;
        len++;
    }

    /* type indicates opening tag */
    apdu[0] |= 6;

    return len;
}

/* from clause 20.2.1.3.2 Constructed Data */
/* returns the number of apdu bytes consumed */
/**********************************************************************/
unsigned char encode_closing_tag(unsigned char *apdu, unsigned char tag_number)
{
    unsigned char len = 1;

    apdu[0] = TAG_CLASS_BIT;
    /* additional tag byte after this byte for extended tag byte */
    if (tag_number <= 14)
	{
        apdu[0] |= (tag_number << 4);
	}
    else
    {
        apdu[0] |= 0xF0;
        apdu[1] = tag_number;
        len++;
    }

    /* type indicates closing tag */
    apdu[0] |= 7;

    return len;
}

/**********************************************************************/
static bool decode_is_extended_tag_number(unsigned char *apdu)
{
    return ((apdu[0] & 0xF0) == 0xF0) ? TRUE : FALSE;
}

/**********************************************************************/
static bool decode_is_extended_value(unsigned char *apdu)
{
    return ((apdu[0] & 0x07) == 5) ? TRUE : FALSE;
}

/**********************************************************************/
bool decode_is_context_specific(unsigned char *apdu)
{
    return ((apdu[0] & TAG_CLASS_BIT) != 0) ? TRUE : FALSE;
}

/**********************************************************************/
static bool decode_is_opening_tag(unsigned char *apdu)
{
    return ((apdu[0] & 0x07) == 6) ? TRUE : FALSE;
}

/**********************************************************************/
static bool decode_is_closing_tag(unsigned char *apdu)
{
    return ((apdu[0] & 0x07) == 7) ? TRUE : FALSE;
}

/**********************************************************************/
unsigned char decode_tag_number(unsigned char *apdu, unsigned char *tag_number)
{
    unsigned char len = 1; /* return value */

    /* decode the tag number first */
    if (decode_is_extended_tag_number(&apdu[0]))
    {
        /* extended tag */
        if( tag_number != NULL )
		{
			*tag_number = apdu[1];
		}
        len++;
    }
    else
    {
        if( tag_number != NULL )
		{
			*tag_number = (apdu[0] >> 4);
		}
    }

    return len;
}

/* from clause 20.2.1.3.2 Constructed Data */
/* returns the number of apdu bytes consumed */
/**********************************************************************/
unsigned char decode_tag_number_and_lvt(unsigned char *apdu, 
										unsigned char *tag_number, 
										unsigned int *value)
{
    unsigned char  len = 1;
    unsigned short int16v;
    unsigned int int32v;

    len = decode_tag_number(&apdu[0], tag_number);
    /* decode the value */
    if (decode_is_extended_value(&apdu[0]))
    {
        /* tagged as unsigned int */
        if (apdu[len] == 255)
        {
            len++;
            len += chars_to_int32(&apdu[len], &int32v);
            if (value != NULL)
			{
				*value=int32v;
			}
        }
        else if (apdu[len] == 254)
        {
            len++;
            len += chars_to_int16(&apdu[len], &int16v);
            if( value != NULL )
			{
				*value=int16v;
			}
        }
        else
        {
            if( value != NULL )
			{
				*value = apdu[len];
			}
            len++;
        }
    }
    else if (decode_is_opening_tag(&apdu[0]) && value)
	{
		*value = 0;
	}
    else if (decode_is_closing_tag(&apdu[0]) && value)
	{
		*value = 0;
	}
    else if (value != NULL)
	{
		*value = apdu[0] & 0x07;
	}

    return len;
}

/* from clause 20.2.1.3.2 Constructed Data */
/* returns true if the tag is context specific and matches */
/**********************************************************************/
bool decode_is_context_tag(unsigned char *apdu, unsigned char tag_number)
{
    unsigned char my_tag_number = 0;
    bool context_specific = FALSE;

    context_specific = decode_is_context_specific(apdu);
    (void)decode_tag_number(apdu,&my_tag_number);

    return ((context_specific == TRUE) && (my_tag_number == tag_number)) ? TRUE : FALSE;
}

/* from clause 20.2.1.3.2 Constructed Data */
/* returns the true if the tag is an opening tag and the tag number matches */
/**********************************************************************/
bool decode_is_opening_tag_number(unsigned char *apdu, unsigned char tag_number)
{
    unsigned char my_tag_number = 0;
    bool opening_tag = FALSE;

    opening_tag = decode_is_opening_tag(apdu);
    (void)decode_tag_number(apdu,&my_tag_number);

    return ((opening_tag == TRUE) && (my_tag_number == tag_number)) ? TRUE : FALSE;
}

/* from clause 20.2.1.3.2 Constructed Data */
/* returns the true if the tag is a closing tag and the tag number matches */
/**********************************************************************/
bool decode_is_closing_tag_number(unsigned char *apdu, unsigned char tag_number)
{
    unsigned char my_tag_number = 0;
    bool closing_tag = FALSE;

    closing_tag = decode_is_closing_tag(apdu);
    (void)decode_tag_number(apdu,&my_tag_number);

    return ((closing_tag == TRUE) && (my_tag_number == tag_number)) ? TRUE : FALSE;
}


/* from clause 20.2.2 Encoding of Null */
/* returns the number of apdu bytes consumed */
/**********************************************************************/
unsigned short encode_tagged_null(unsigned char *apdu)
{
    apdu[0]=0;
    return 1;
}


/* from clause 20.2.3 Encoding of a Boolean */
/* returns the number of apdu bytes consumed */
/**********************************************************************/
unsigned short encode_tagged_boolean(unsigned char *apdu, bool value)
{
    return encode_tag(&apdu[0], BACNET_APPLICATION_TAG_bool, FALSE, ((value == TRUE) ? 1 : 0));
}

/**********************************************************************/
unsigned short encode_context_boolean(unsigned char *apdu, unsigned char tag_number, bool value)
{
    unsigned short len;

    len = encode_tag(&apdu[0], tag_number, TRUE, 1);
    apdu[len] = ((value == TRUE) ? 1 : 0);
    len ++;
    return len;
}


/* returns the number of apdu bytes consumed 0 on failure */
/**********************************************************************/
unsigned short decode_tagged_boolean(unsigned char *apdu, bool *value)
{
    if (decode_is_context_specific(&apdu[0]) == TRUE) return 0;

    if ((apdu[0]&0xf0) != 0x10) return 0; 

    *value = ((apdu[0]&0x01) == 0x1) ? TRUE : FALSE;

    return 1;
}

/* from clause 20.2.6 Encoding of a Real Number Value */
/* returns the number of apdu bytes consumed */
/**********************************************************************/
unsigned char chars_to_real(unsigned char *apdu, REAL *real_value)
{
    union
    {
        unsigned char byte[4];
        REAL real_value;
    } my_data;

#ifdef IS_BIGENDIAN
    my_data.byte[0] = apdu[0];
    my_data.byte[1] = apdu[1];
    my_data.byte[2] = apdu[2];
    my_data.byte[3] = apdu[3];
#else
    my_data.byte[0] = apdu[3];
    my_data.byte[1] = apdu[2];
    my_data.byte[2] = apdu[1];
    my_data.byte[3] = apdu[0];
#endif

    *real_value = my_data.real_value;

    return 4;
}

/* returns the number of apdu bytes consumed, 0 on error */
/**********************************************************************/
unsigned short decode_tagged_real(unsigned char *apdu, REAL *real_value)
{
    unsigned char tag;
    unsigned short len;

    if (decode_is_context_specific(apdu) == TRUE)
	{
		return 0;
	}

    len = decode_tag_number(apdu, &tag);
    if( tag != BACNET_APPLICATION_TAG_REAL )
	{
		return 0;
	}

    len += chars_to_real(&apdu[len], real_value);

    return len;
}


/* from clause 20.2.6 Encoding of a Real Number Value */
/* returns the number of apdu bytes consumed */
/**********************************************************************/
unsigned char real_to_chars(unsigned char *apdu, REAL value)
{
    union
    {
        unsigned char byte[4];
        REAL real_value;
    } my_data;

    my_data.real_value = value;

#ifdef IS_BIGENDIAN
    apdu[0] = my_data.byte[0];
    apdu[1] = my_data.byte[1];
    apdu[2] = my_data.byte[2];
    apdu[3] = my_data.byte[3];
#else
    apdu[0] = my_data.byte[3];
    apdu[1] = my_data.byte[2];
    apdu[2] = my_data.byte[1];
    apdu[3] = my_data.byte[0];
#endif

    return 4;
}

/* from clause 20.2.6 Encoding of a Real Number Value */
/* and 20.2.1 General Rules for Encoding Bacnet Tags */
/* returns the number of apdu bytes consumed */
/**********************************************************************/
unsigned short encode_tagged_real(unsigned char *apdu, REAL value)
{
    unsigned short len = 0;

    /* assumes that the tag only consumes 1 octet */
    len = real_to_chars(&apdu[1], value);
    len += encode_tag(&apdu[0], BACNET_APPLICATION_TAG_REAL, FALSE, len);

    return len;
}

/**********************************************************************/
unsigned short encode_context_real(unsigned char *apdu, unsigned char tag_number, REAL value)
{
    unsigned short len = 0;

    /* assumes that the tag only consumes 1 octet */
    len = real_to_chars(&apdu[1], value);
    len += encode_tag(&apdu[0], tag_number, TRUE, len);

    return len;
}

/* from clause 20.2.14 Encoding of an Object Identifier Value */
/* returns the number of apdu bytes consumed */
/**********************************************************************/
unsigned short encode_bacnet_object_id(unsigned char *apdu, 
										unsigned short object_type, 
										unsigned int instance)
{
    return int32_to_chars(apdu, ((unsigned int)(object_type & 0x3FF) << 22) | (instance & 0x3FFFFF));
}

/* from clause 20.2.14 Encoding of an Object Identifier Value */
/* and 20.2.1 General Rules for Encoding Bacnet Tags */
/* returns the number of apdu bytes consumed */
/**********************************************************************/
unsigned short encode_context_object_id(unsigned char *apdu, 
										unsigned char tag_number, 
										unsigned short object_type, 
										unsigned int instance)
{
    unsigned short len = 0;

    /* assumes that the tag only consumes 1 octet */
    len = encode_bacnet_object_id(&apdu[1], object_type, instance);
    len += encode_tag(&apdu[0], tag_number, TRUE, len);

    return len;
}

/* from clause 20.2.14 Encoding of an Object Identifier Value */
/* and 20.2.1 General Rules for Encoding Bacnet Tags */
/* returns the number of apdu bytes consumed */
/**********************************************************************/
unsigned short encode_tagged_object_id(unsigned char *apdu, 
										unsigned short object_type, 
										unsigned int instance)
{
    unsigned short len = 0;

    /* assumes that the tag only consumes 1 octet */
    len = encode_bacnet_object_id(&apdu[1], object_type, instance);
    len += encode_tag(&apdu[0], BACNET_APPLICATION_TAG_OBJECT_ID, FALSE, len);

    return len;
}


/* from clause 20.2.14 Encoding of an Object Identifier Value */
/* returns the number of apdu bytes consumed */
/**********************************************************************/
unsigned short decode_object_id(unsigned char *apdu, 
								unsigned short *object_type, 
								unsigned int *instance)
{
    unsigned int int32v = 0;

    (void)chars_to_int32(apdu, &int32v); /* returns 4 */

    *object_type = ((int32v >> 22) & 0x3FF);
    *instance = (int32v & 0x3FFFFF);

    return 4;
}


/**********************************************************************/
unsigned short decode_context_object_id(unsigned char *apdu, 
										unsigned char tag_number, 
										unsigned short *object_type, 
										unsigned int *instance)
{
    unsigned short len = 0;                /* return value */
    unsigned int len_value = 0;
    unsigned char tag_num = 0;

    len = decode_tag_number_and_lvt(&apdu[0], &tag_num, &len_value);
    if (tag_num != tag_number)
	{
		return 0; /* this is not a the expected tag */
	}

    len += decode_object_id(&apdu[len], object_type, instance);

    return len;
}
/**********************************************************************/
unsigned short decode_tagged_object_id(unsigned char *apdu, 
									   unsigned short *object_type, 
									   unsigned int *instance)
{
    unsigned short len = 0;                /* return value */
    unsigned int len_value = 0;
    unsigned char tag_num = 0;

    len = decode_tag_number_and_lvt(&apdu[0], &tag_num, &len_value);
    if( tag_num != BACNET_APPLICATION_TAG_OBJECT_ID )
	{
		return 0; /* this is not a the expected tag */
	}

    len += decode_object_id(&apdu[len], object_type, instance);

    return len;
}


/* from clause 20.2.9 Encoding of a Character String Value */
/* returns the number of apdu bytes consumed */
/**********************************************************************/
unsigned short encode_bacnet_string(unsigned char *apdu, 
									unsigned char *char_string, 
									unsigned short len)
{
    unsigned short i;

    /* limit - 6 octets is the most our tag and type could be */
    if (len > (MAX_APDU_LEN - 6)) len = MAX_APDU_LEN - 6;

    for (i = 0; i < len; i++)
    {
        apdu[1 + i] = char_string[i];
    }
    apdu[0] = CHARACTER_ANSI;
    len++;

    return len;
}

/* from clause 20.2.9 Encoding of a Character String Value */
/* returns the number of apdu bytes consumed */
/**********************************************************************/
unsigned short encode_bacnet_char_string(unsigned char *apdu, unsigned char *char_string)
{
    unsigned short len = 0;

    len = bac_strlen(char_string);
    len = encode_bacnet_string(&apdu[0], char_string, len);

    return len;
}

/* from clause 20.2.9 Encoding of a Character String Value */
/* returns the number of apdu bytes consumed */
/**********************************************************************/
unsigned short encode_context_char_string(unsigned char *apdu, 
										  unsigned char tag_number, 
										  unsigned char *char_string)
{
    unsigned short len = 0;
    unsigned short string_len = 0;

    string_len = bac_strlen(char_string);

    len = encode_tag(&apdu[0], tag_number, TRUE, string_len + 1);
    len += encode_bacnet_string(&apdu[len], char_string, string_len);

    return len;
}

/**********************************************************************/
unsigned short encode_tagged_char_string(unsigned char *apdu, unsigned char *char_string)
{
    unsigned short len = 0;
    unsigned short string_len = 0;

    string_len = bac_strlen(char_string);

    len = encode_tag(&apdu[0], 
					BACNET_APPLICATION_TAG_CHARACTER_STRING, 
					FALSE, 
					string_len + 1);
    len += encode_bacnet_string(&apdu[len], char_string, string_len);

    return len;
}

/* from clause 20.2.9 Encoding of a Character String Value */
/* and 20.2.1 General Rules for Encoding Bacnet Tags */
/* returns the number of apdu bytes consumed */
/**********************************************************************/
unsigned short decode_context_char_string(unsigned char *apdu, 
										  unsigned char tag_number, 
										  unsigned char *char_string, 
										  unsigned short str_len)
{
    unsigned short len = 0;
	unsigned short tmp_len = 0;
    unsigned int i = 0;
    unsigned int len_value = 0;
    unsigned char tag_num = 0;

    len = decode_tag_number_and_lvt(&apdu[0], &tag_num, &len_value);
    if( tag_num != tag_number )
	{
		return 0; /* this is not a the expected tag */
	}
    if( len_value > 0xFFFF )
	{
		return 0; /* we do not support this long strings */
	}

    if (len_value != 0)
    {
        /* decode ANSI X3.4 */
        if (apdu[len] == 0)
        {
            len++;       /* the one type octet */
            len_value--; /* the one type octet */
            tmp_len = (len_value < str_len) ? (unsigned short)(len_value&0xFFFF) : str_len-1;
            for (i = 0; i < tmp_len; i++)
            {
                char_string[i] = apdu[len + i];
            }
            /* terminate the c string */
            char_string[i] = 0;
        }
        len += (unsigned short)(len_value&0xFFFF);
    }

    return len;
}

/**********************************************************************/
unsigned short decode_tagged_char_string(unsigned char *apdu, 
										 unsigned char *char_string, 
										 unsigned short str_len)
{
    unsigned short len = 0;
	unsigned short tmp_len = 0;
    unsigned int i = 0;
    unsigned int len_value = 0;
    unsigned char tag_number = 0;

    len = decode_tag_number_and_lvt(&apdu[0], &tag_number, &len_value);
    /* check tag number */
    if( tag_number != BACNET_APPLICATION_TAG_CHARACTER_STRING )
	{
		return 0;
	}
    if( len_value > 0xFFFF )
	{
		return 0; /* we do not support this long strings */
	}

    if (len_value != 0)
    {
        /* decode ANSI X3.4 */
        if (apdu[len] == 0)
        {
            len++;       /* the one type octet */
            len_value--; /* the one type octet */
            tmp_len = (len_value < str_len) ? (unsigned short)(len_value&0xFFFF) : str_len-1;
            for (i = 0; i < tmp_len; i++)
            {
                char_string[i] = apdu[len + i];
            }
            /* terminate the c string */
            char_string[i] = 0;
        }
    }
    len += (unsigned short)(len_value&0xFFFF);

    return len;
}

/* This function is NOT SAFE!!! */
/* - The string shares memory with the apdu */
/* - No string termination */
/* + Saves memory */
/**********************************************************************/
unsigned short decode_context_char_str_ptr(unsigned char *apdu, 
										   unsigned char tag_number, 
										   unsigned char **char_string, 
										   unsigned short *str_len)
{
    unsigned short len = 0;
    unsigned int len_value = 0;
    unsigned char tag_num = 0;

    len = decode_tag_number_and_lvt(&apdu[0], &tag_num, &len_value);
    if( tag_num != tag_number )
	{
		return 0; /* this is not the expected tag */
	}
    if( len_value > 0xFFFF )
	{
		return 0; /* we do not support this long strings */
	}

    if( len_value != 0 )
    {
        /* decode ANSI X3.4 */
        if (apdu[len] == 0)
        {
            len++;
            len_value--;
            *char_string = &apdu[len];
            *str_len = (unsigned short)(len_value&0xFFFF);
        }
        /* Unsupported string format */
        else
        {
            *char_string = NULL;
            return 0;
        }
    }

    return (unsigned short)(len_value&0xFFFF)+len;
}

/* This function is NOT SAFE!!! */
/* - The string shares memory with the apdu */
/* - No string termination */
/* + Saves memory */
/**********************************************************************/
unsigned short decode_tagged_char_str_ptr(unsigned char *apdu, 
										  unsigned char **char_string, 
										  unsigned short *str_len)
{
    unsigned short len = 0;
    unsigned int len_value = 0;
    unsigned char tag_number = 0;

    len = decode_tag_number_and_lvt(&apdu[0], &tag_number, &len_value);
    if( tag_number != BACNET_APPLICATION_TAG_CHARACTER_STRING )
	{
		return 0; /* this is not the expected tag */
	}
    if( len_value > 0xFFFF )
	{
		return 0; /* we do not support this long strings */
	}

    if (len_value != 0)
    {
        /* decode ANSI X3.4 */
        if (apdu[len] == 0)
        {
            len++;
            len_value--;
            *char_string = &apdu[len];
            *str_len = (unsigned short)(len_value&0xFFFF);
        }
        /* Unsupported string format */
        else
        {
            *char_string = NULL;
            return 0;
        }
    }

    return (unsigned short)(len_value&0xFFFF)+len;
}

/* from clause 20.2.10 Encoding of a Bit String Value */
/* returns the number of apdu bytes consumed */
/* calculate the length of the bitstring and the empty bits at the end */
/**********************************************************************/
unsigned short encode_tagged_bit_string(unsigned char *apdu, 
										unsigned char *bit_string, 
										unsigned char last_bit)
{
	unsigned short i = 0;
    unsigned short len = 0;
	unsigned short strlen = 0;

    strlen = (last_bit / 8) + 1;

    /* an inital octet contains the number of unused bits in the final octet */
    len = encode_tag(&apdu[0], BACNET_APPLICATION_TAG_BIT_STRING, FALSE, strlen+1);

    apdu[len] = (strlen * 8) - last_bit - 1;

    for (i=0; i<strlen; i++)
    {
        apdu[i+len+1]=bit_string[i];
    }
    len += i+1;

    return len;
}

/* set the specifyed bit to 1 if TRUE or 0 if FALSE */
/**********************************************************************/
void set_bit_in_string(unsigned char *apdu, unsigned char bit, bool enabeled)
{
    unsigned char byte = 0;
	unsigned char bbit = 0;
	unsigned char mask = 0;

    /* find byte to modify */
    byte = (unsigned char)(bit / 8);

    /* find bit to change (bit 0 is the most significant) */
    bbit = ((byte + 1) * 8) - bit - 1;

    /* generate the mask */
    mask = 1 << bbit;

    if (enabeled==TRUE)
    {
        /* set the bit */
        apdu[byte] |= mask;
    }
    else
    {
        /* clear the bit */
        apdu[byte] &= ~mask;
    }
    return;
}


/* from clause 20.2.4 Encoding of an Unsigned Integer Value */
/* and 20.2.1 General Rules for Encoding Bacnet Tags */
/* returns the number of apdu bytes consumed */
/**********************************************************************/
unsigned char encode_bacnet_unsigned(unsigned char *apdu, unsigned int value)
{
    if (value < 0x100)
    {
        apdu[0] = 0xFf & value;
        return 1;
    }
    else if (value < 0x10000)
    {
        apdu[0] = 0xFF & (value >> 8);
        apdu[1] = 0xFF & (value >> 0);
        return 2;
    }
    else if (value < 0x1000000)
    {
        apdu[0] = 0xFF & (value >> 16);
        apdu[1] = 0xFF & (value >>  8);
        apdu[2] = 0xFF & (value >>  0);
        return 3;
    }
    else
    {
        apdu[0] = 0xFF & (value >> 26);
        apdu[1] = 0xFF & (value >> 16);
        apdu[2] = 0xFF & (value >>  8);
        apdu[3] = 0xFF & (value >>  0);
        return 4;
    }
}

/* from clause 20.2.4 Encoding of an Unsigned Integer Value */
/* and 20.2.1 General Rules for Encoding Bacnet Tags */
/* returns the number of apdu bytes consumed */
/**********************************************************************/
unsigned char encode_context_unsigned(unsigned char *apdu, 
									  unsigned char tag_number, 
									  unsigned int value)
{
    unsigned char len = 0;

    len = encode_bacnet_unsigned(&apdu[1], value);
    len += encode_tag(&apdu[0], tag_number, TRUE, len);

    return len;
}

/* from clause 20.2.4 Encoding of an Unsigned Integer Value */
/* and 20.2.1 General Rules for Encoding Bacnet Tags */
/* returns the number of apdu bytes consumed */
/**********************************************************************/
unsigned char encode_tagged_unsigned(unsigned char *apdu, unsigned int value)
{
    unsigned char len = 0;

    len = encode_bacnet_unsigned(&apdu[1], value);
    len += encode_tag(&apdu[0], BACNET_APPLICATION_TAG_UNSIGNED_INT, FALSE, len);

    return len;
}

/* from clause 20.2.4 Encoding of an Unsigned Integer Value */
/* and 20.2.1 General Rules for Encoding Bacnet Tags */
/* returns the number of apdu bytes consumed. (0==error) */
/**********************************************************************/
unsigned char decode_tagged_unsigned(unsigned char *apdu, unsigned int *value)
{
    unsigned char len = 0;                /* return value */
    unsigned int len_value = 0;
    unsigned char tag_number = 0;

    len = decode_tag_number_and_lvt(&apdu[0], &tag_number, &len_value);
    if (tag_number != BACNET_APPLICATION_TAG_UNSIGNED_INT )
	{
		return 0;
	}

    switch (len_value)
    {
    case 1:
        *value = apdu[len];
        break;
    case 2:
        *value  = (unsigned int)apdu[len+0]<<8;
        *value += (unsigned int)apdu[len+1];
        break;
    case 3:
        *value  = (unsigned int)apdu[len+0]<<16;
        *value += (unsigned int)apdu[len+1]<<8;
        *value += (unsigned int)apdu[len+2];
        break;
    case 4:
        *value  = (unsigned int)apdu[len+0]<<24;
        *value  = (unsigned int)apdu[len+1]<<16;
        *value += (unsigned int)apdu[len+2]<<8;
        *value += (unsigned int)apdu[len+3];
        break;
    default:
        *value = 0;
        break;
    }
    len += (unsigned char)(len_value&0xFF);

    return len;
}

/* returns the number of apdu bytes consumed */
/* IF it is a context_specific unsigned value */
/* on error return 0 */
/**********************************************************************/
unsigned char decode_context_unsigned(unsigned char *apdu, 
									  unsigned char tag_number, 
									  unsigned int *value)
{
    unsigned char i = 0;
    unsigned int  len = 0;
    unsigned char  tag_num = 0;

    /* context_specific */
    if( decode_is_context_tag(apdu, tag_number) == FALSE )
	{
		return 0;
	}

    /* get the tag number and data length */
    i += decode_tag_number_and_lvt(&apdu[i], &tag_num, &len);
    if( tag_num != tag_number )
	{
		return 0;
	}

    /* get the value */
    switch (len)
    {
    case 1:
        *value = apdu[i];
        break;
    case 2:
        *value  = (unsigned int)apdu[i+0]<<8;
        *value += (unsigned int)apdu[i+1];
        break;
    case 3:
        *value  = (unsigned int)apdu[i+0]<<16;
        *value += (unsigned int)apdu[i+1]<<8;
        *value += (unsigned int)apdu[i+2];
        break;
    case 4:
        *value  = (unsigned int)apdu[i+0]<<24;
        *value  = (unsigned int)apdu[i+1]<<16;
        *value += (unsigned int)apdu[i+2]<<8;
        *value += (unsigned int)apdu[i+3];
        break;
    default:
        *value = 0;
        break;
    }
    i += (unsigned char)len;


    return i;
}

/**********************************************************************/
unsigned char decode_context_unsigned8(unsigned char *apdu, 
									   unsigned char tag_number, 
									   unsigned char *value)
{
    unsigned char  len = 0;
    unsigned char  tag_num = 0;

    /* context_specific */
    if( decode_is_context_tag(apdu, tag_number) == FALSE )
	{
		return 0;
	}

    /* get the tag number and data length */
    len = decode_tag_number(&apdu[0], &tag_num);

    if( len != 1 )
	{
		return 0;  /* tag must be one octet */
	}

    if( tag_num != tag_number )
	{
		return 0;
	}

    if( (apdu[0] & 0x07) != 1 )
	{
		return 0; /* only one octet long payload accepted */
	}

    *value = apdu[1];

    return 2;
}


/* from clause 20.2.11 Encoding of an Enumerated Value */
/* and 20.2.1 General Rules for Encoding Bacnet Tags */
/* returns the number of apdu bytes consumed */
/**********************************************************************/
unsigned short decode_tagged_enum(unsigned char *apdu, unsigned int *value)
{
    unsigned short len = 0;                /* return value */
    unsigned int len_value = 0;
    unsigned char tag_number = 0;

    len = decode_tag_number_and_lvt(&apdu[0], &tag_number, &len_value);
    if( tag_number != BACNET_APPLICATION_TAG_ENUMERATED )
	{
		return 0;
	}


    switch (len_value)
    {
    case 1:
        *value = apdu[len];
        break;
    case 2:
        *value  = (unsigned int)apdu[len+0]<<8;
        *value += (unsigned int)apdu[len+1];
        break;
    case 3:
        *value  = (unsigned int)apdu[len+0]<<16;
        *value += (unsigned int)apdu[len+1]<<8;
        *value += (unsigned int)apdu[len+2];
        break;
    case 4:
        *value  = (unsigned int)apdu[len+0]<<24;
        *value  = (unsigned int)apdu[len+1]<<16;
        *value += (unsigned int)apdu[len+2]<<8;
        *value += (unsigned int)apdu[len+3];
        break;
    default:
        *value = 0;
        break;
    }
    len += (unsigned short)len_value;

    return len;
}

/**********************************************************************/
unsigned short decode_context_enum(unsigned char *apdu, 
									unsigned char tag_number, 
									unsigned int *value)
{
    return decode_context_unsigned(apdu, tag_number, value);
}


/* from clause 20.2.11 Encoding of an Enumerated Value */
/* and 20.2.1 General Rules for Encoding Bacnet Tags */
/* returns the number of apdu bytes consumed */
/**********************************************************************/
unsigned short encode_bacnet_enum(unsigned char *apdu, unsigned int value)
{
    return encode_bacnet_unsigned(apdu, value);
}

/* from clause 20.2.11 Encoding of an Enumerated Value */
/* and 20.2.1 General Rules for Encoding Bacnet Tags */
/* returns the number of apdu bytes consumed */
/**********************************************************************/
unsigned short encode_tagged_enum(unsigned char *apdu, unsigned int value)
{
    unsigned short len = 0;                /* return value */

    /* assumes that the tag only consumes 1 octet */
    len = encode_bacnet_enum(&apdu[1], value);
    len += encode_tag(&apdu[0], BACNET_APPLICATION_TAG_ENUMERATED, FALSE, len);

    return len;
}

/* from clause 20.2.11 Encoding of an Enumerated Value */
/* and 20.2.1 General Rules for Encoding Bacnet Tags */
/* returns the number of apdu bytes consumed */
/**********************************************************************/
unsigned short encode_context_enum(unsigned char *apdu, 
									unsigned char tag_number, 
									unsigned int value)
{
    unsigned short len = 0;                /* return value */

    /* assumes that the tag only consumes 1 octet */
    len = encode_bacnet_enum(&apdu[1], value);
    len += encode_tag(&apdu[0], tag_number, TRUE, len);

    return len;
}

/* from clause 20.2.5 Encoding of a Signed Integer Value */
/* and 20.2.1 General Rules for Encoding Bacnet Tags */
/* returns the number of apdu bytes consumed */
/**********************************************************************/
unsigned short decode_tagged_signed(unsigned char *apdu, int *value)
{
    return decode_tagged_unsigned(apdu, (unsigned int *)value);
}

/**********************************************************************/
unsigned short decode_context_signed(unsigned char *apdu, 
									 unsigned char tag_number, 
									 int *value)
{
    return decode_context_unsigned(apdu, tag_number, (unsigned int *)value);
}

/* from clause 20.2.5 Encoding of a Signed Integer Value */
/* and 20.2.1 General Rules for Encoding Bacnet Tags */
/* returns the number of apdu bytes consumed */
/**********************************************************************/
unsigned short encode_bacnet_signed(unsigned char *apdu, int value)
{
    return encode_bacnet_unsigned(apdu, value);
}

/* from clause 20.2.5 Encoding of a Signed Integer Value */
/* and 20.2.1 General Rules for Encoding Bacnet Tags */
/* returns the number of apdu bytes consumed */
/**********************************************************************/
unsigned short encode_tagged_signed(unsigned char *apdu, int value)
{
    unsigned short len = 0;                /* return value */

    /* assumes that the tag only consumes 1 octet */
    len = encode_bacnet_signed(&apdu[1], value);
    len += encode_tag(&apdu[0], BACNET_APPLICATION_TAG_SIGNED_INT, FALSE, len);

    return len;
}

/* from clause 20.2.5 Encoding of a Signed Integer Value */
/* and 20.2.1 General Rules for Encoding Bacnet Tags */
/* returns the number of apdu bytes consumed */
/**********************************************************************/
unsigned short encode_context_signed(unsigned char *apdu, 
									 unsigned char tag_number, 
									 int value)
{
    unsigned short len = 0;                /* return value */

    /* assumes that the tag only consumes 1 octet */
    len = encode_bacnet_signed(&apdu[1], value);
    len += encode_tag(&apdu[0], tag_number, TRUE, len);

    return len;
}

/* from clause 20.2.13 Encoding of a Time Value */
/* and 20.2.1 General Rules for Encoding Bacnet Tags */
/* returns the number of apdu bytes consumed */
/**********************************************************************/
unsigned short encode_bacnet_time(unsigned char *apdu, 
								  unsigned char hour, 
								  unsigned char min, 
								  unsigned char sec, 
								  unsigned char hundredths)
{
    apdu[0] = hour;
    apdu[1] = min;
    apdu[2] = sec;
    apdu[3] = hundredths;

    return 4;
}

/* from clause 20.2.13 Encoding of a Time Value */
/* and 20.2.1 General Rules for Encoding Bacnet Tags */
/* returns the number of apdu bytes consumed */
/**********************************************************************/
unsigned short encode_tagged_time(unsigned char *apdu, 
								  unsigned char hour, 
								  unsigned char min, 
								  unsigned char sec, 
								  unsigned char hundredths)
{
    unsigned short len = 0;

    /* assumes that the tag only consumes 1 octet */
    len = encode_bacnet_time(&apdu[1], hour, min, sec, hundredths);
    len += encode_tag(&apdu[0], BACNET_APPLICATION_TAG_TIME, FALSE, len);

    return len;

}

/* from clause 20.2.13 Encoding of a Time Value */
/* and 20.2.1 General Rules for Encoding Bacnet Tags */
/* returns the number of apdu bytes consumed */
/**********************************************************************/
unsigned short decode_bacnet_time(unsigned char *apdu, 
								  unsigned char *hour, 
								  unsigned char *min, 
								  unsigned char *sec, 
								  unsigned char *hundredths)
{
    *hour = apdu[0];
    *min = apdu[1];
    *sec = apdu[2];
    *hundredths = apdu[3];

    return 4;
}

/* Bacnet Date */
/* year = years since 1900 */
/* month 1=Jan */
/* day = day of month */
/* wday 1=Monday...7=Sunday */

/* from clause 20.2.12 Encoding of a Date Value */
/* and 20.2.1 General Rules for Encoding Bacnet Tags */
/* returns the number of apdu bytes consumed */
/**********************************************************************/
unsigned short encode_bacnet_date(unsigned char *apdu, 
								  unsigned char year, 
								  unsigned char month, 
								  unsigned char day, 
								  unsigned char wday)
{
    apdu[0] = year;
    apdu[1] = month;
    apdu[2] = day;
    apdu[3] = wday;

    return 4;
}

/* from clause 20.2.12 Encoding of a Date Value */
/* and 20.2.1 General Rules for Encoding Bacnet Tags */
/* returns the number of apdu bytes consumed */
/**********************************************************************/
unsigned short encode_tagged_date(unsigned char *apdu, 
								  unsigned char year, 
								  unsigned char month, 
								  unsigned char day, 
								  unsigned char wday)
{
    unsigned short len = 0;

    /* assumes that the tag only consumes 1 octet */
    len = encode_bacnet_date(&apdu[1], year, month, day, wday);
    len += encode_tag(&apdu[0], BACNET_APPLICATION_TAG_DATE, FALSE, len);

    return len;

}

/* from clause 20.2.12 Encoding of a Date Value */
/* and 20.2.1 General Rules for Encoding Bacnet Tags */
/* returns the number of apdu bytes consumed */
/**********************************************************************/
unsigned short decode_date(unsigned char *apdu, 
							unsigned char *year, 
							unsigned char *month, 
							unsigned char *day, 
							unsigned char *wday)
{
    *year = apdu[0];
    *month = apdu[1];
    *day = apdu[2];
    *wday = apdu[3];

    return 4;
}
