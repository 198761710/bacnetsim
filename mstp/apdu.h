/*####COPYRIGHTBEGIN####
 -------------------------------------------
 Copyright (C) 2005 Wapice Ltd

 Author: Kåre Särs (kare.sars@wapice.com)

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
 Boston, MA  02111-1307
 USA.

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
 -------------------------------------------
####COPYRIGHTEND####*/

#ifndef BACNET_APDU_H
#define BACNET_APDU_H

#define APDU_TYPE_MASK 0xF0

typedef enum
{
    APDU_TYPE_CONFIRMED_REQUEST = 0x00,
    APDU_TYPE_UNCONFIRMED_REQUEST = 0x10,
    APDU_TYPE_SIMPLE_ACK = 0x20,
    APDU_TYPE_COMPLEX_ACK = 0x30,
    APDU_TYPE_SEGMENT_ACK = 0x40,
    APDU_TYPE_ERROR = 0x50,
    APDU_TYPE_REJECT = 0x60,
    APDU_TYPE_ABORT = 0x70
} BACNET_APDU_TYPE;

#define APDU_SEGMENTED 0x08
#define APDU_MOR_SEGS  0x04
#define APDU_SEG_ACC   0x02
#define APDU_NAK       0x02
#define APDU_SRV       0x01

#endif
