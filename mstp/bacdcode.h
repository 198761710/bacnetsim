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
#ifndef BACDCODE_H
#define BACDCODE_H
#include "platform.h"

/* from clause 20.1.2.4 max-segments-accepted */
/* and clause 20.1.2.5 max-APDU-length-accepted */
/* returns the encoded octet */
unsigned char encode_max_segs_max_apdu(unsigned char max_segs, unsigned short max_apdu);

/* from clause 20.2.1 General Rules for Encoding Bacnet Tags */
/* returns the number of apdu bytes consumed */
unsigned char encode_tag(unsigned char *apdu, unsigned char tag_number, bool context_specific, unsigned int len_value_type);

/* from clause 20.2.1.3.2 Constructed Data */
/* returns the number of apdu bytes consumed */
unsigned char encode_opening_tag(unsigned char *apdu, unsigned char tag_number);
unsigned char encode_closing_tag(unsigned char *apdu, unsigned char tag_number);

/* returns the true if the tag is an opening tag and the tag number matches */
bool decode_is_opening_tag_number(unsigned char *apdu, unsigned char tag_number);
/* returns the true if the tag is a closing tag and the tag number matches */
bool decode_is_closing_tag_number(unsigned char *apdu, unsigned char tag_number);


unsigned char decode_tag_number(unsigned char *apdu, unsigned char *tag_number);
unsigned char decode_tag_number_and_lvt(unsigned char *apdu, unsigned char *tag_number, unsigned int *value);

/* returns true if the tag is context specific */
bool decode_is_context_specific(unsigned char *apdu);
/* returns true if the tag is context specific and matches */
bool decode_is_context_tag(unsigned char *apdu, unsigned char tag_number);

/* from clause 20.2.14 Encoding of an Object Identifier Value */
/* and 20.2.1 General Rules for Encoding Bacnet Tags */
/* returns the number of apdu bytes consumed */
unsigned short encode_context_object_id(unsigned char *apdu, unsigned char tag_number, unsigned short object_type, unsigned int instance);
unsigned short encode_tagged_object_id(unsigned char *apdu, unsigned short object_type, unsigned int instance);

unsigned short decode_context_object_id(unsigned char *apdu, unsigned char tag_number, unsigned short *object_type, unsigned int *instance);
unsigned short decode_tagged_object_id(unsigned char *apdu, unsigned short *object_type, unsigned int *instance);

/* from clause 20.2.9 Encoding of a Character String Value */
/* and 20.2.1 General Rules for Encoding Bacnet Tags */
/* returns the number of apdu bytes consumed */
unsigned short encode_context_char_string(unsigned char *apdu, unsigned char tag_number, unsigned char *char_string);
unsigned short encode_tagged_char_string(unsigned char *apdu, unsigned char *char_string);

unsigned short decode_context_char_string(unsigned char *apdu, unsigned char tag_number, unsigned char *char_string, unsigned short str_len);
unsigned short decode_tagged_char_string(unsigned char *apdu, unsigned char *char_string, unsigned short str_len);
unsigned short decode_context_char_str_ptr(unsigned char *apdu, unsigned char tag_number, unsigned char **char_string, unsigned short *str_len);
unsigned short decode_tagged_char_str_ptr(unsigned char *apdu, unsigned char **char_string, unsigned short *str_len);

/* from clause 20.2.10 Encoding of a Bit String Value */
/* returns the number of apdu bytes consumed */
unsigned short encode_tagged_bit_string(unsigned char *apdu, unsigned char *bit_string, unsigned char last_bit);
void   set_bit_in_string(unsigned char *apdu, unsigned char bit, bool enabeled);


/* from clause 20.2.2 Encoding of Null */
/* returns the number of apdu bytes consumed */
unsigned short encode_tagged_null(unsigned char *apdu);

/* from clause 20.2.3 Encoding of a Boolean */
/* returns the number of apdu bytes consumed */
unsigned short encode_tagged_boolean(unsigned char *apdu, bool value);
unsigned short encode_context_boolean(unsigned char *apdu, unsigned char tag_number, bool value);

unsigned short decode_tagged_boolean(unsigned char *apdu, bool *value);

/* from clause 20.2.6 Encoding of a Real Number Value */
/* and 20.2.1 General Rules for Encoding Bacnet Tags */
/* returns the number of apdu bytes consumed */
unsigned short encode_tagged_real(unsigned char *apdu, REAL value);
unsigned short encode_context_real(unsigned char *apdu, unsigned char tag_number, REAL value);

unsigned short decode_tagged_real(unsigned char *apdu, REAL *real_value);

/* from clause 20.2.4 Encoding of an Unsigned Integer Value */
/* and 20.2.1 General Rules for Encoding Bacnet Tags */
/* returns the number of apdu bytes consumed */
unsigned char encode_context_unsigned(unsigned char *apdu, unsigned char tag_number, unsigned int value);
unsigned char encode_tagged_unsigned(unsigned char *apdu, unsigned int value);

unsigned char decode_context_unsigned(unsigned char *apdu, unsigned char tag_number, unsigned int *value);
unsigned char decode_context_unsigned8(unsigned char *apdu, unsigned char tag_number, unsigned char *value);
unsigned char decode_tagged_unsigned(unsigned char *apdu, unsigned int *value);

/* from clause 20.2.5 Encoding of a Signed Integer Value */
/* and 20.2.1 General Rules for Encoding Bacnet Tags */
/* returns the number of apdu bytes consumed */
unsigned short encode_tagged_signed(unsigned char *apdu, int value);
unsigned short encode_context_signed(unsigned char *apdu, unsigned char tag_number, int value);

unsigned short decode_context_signed(unsigned char *apdu, unsigned char tag_number, int *value);
unsigned short decode_tagged_signed(unsigned char *apdu, int *value);

/* from clause 20.2.11 Encoding of an Enumerated Value */
/* and 20.2.1 General Rules for Encoding Bacnet Tags */
/* returns the number of apdu bytes consumed */
unsigned short encode_tagged_enum(unsigned char *apdu, unsigned int value);
unsigned short encode_context_enum(unsigned char *apdu, unsigned char tag_number, unsigned int value);

unsigned short decode_context_enum(unsigned char *apdu, unsigned char tag_number, unsigned int *value);
unsigned short decode_tagged_enum(unsigned char *apdu, unsigned int *value);

/* from clause 20.2.13 Encoding of a Time Value */
/* and 20.2.1 General Rules for Encoding Bacnet Tags */
/* returns the number of apdu bytes consumed */
unsigned short encode_tagged_time(unsigned char *apdu, unsigned char hour, unsigned char min, unsigned char sec, unsigned char hundredths);
unsigned short decode_bacnet_time(unsigned char *apdu, unsigned char *hour, unsigned char *min, unsigned char *sec, unsigned char *hundredths);

/* Bacnet Date */
/* year = years since 1900 */
/* month 1=Jan */
/* day = day of month */
/* wday 1=Monday...7=Sunday */

/* from clause 20.2.12 Encoding of a Date Value */
/* and 20.2.1 General Rules for Encoding Bacnet Tags */
/* returns the number of apdu bytes consumed */
unsigned short encode_tagged_date(unsigned char *apdu, unsigned char year, unsigned char month, unsigned char day, unsigned char wday);
unsigned short decode_date(unsigned char *apdu, unsigned char *year, unsigned char *month, unsigned char *day, unsigned char *wday);

#endif
