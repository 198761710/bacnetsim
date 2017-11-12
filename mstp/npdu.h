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

#ifndef BACNET_NPDU_H
#define BACNET_NPDU_H
#include "platform.h"


/* Control Information bits*/
/* Bit7: Is this a network layer message or an APDU */
#define CONTROL_NET_MSG 0x80
/* Bit6 Reserved        0x40 */
/* Bit5: Is DNET, DLEN, DADR and HopCount present */
#define CONTROL_DEST    0x20
/* Bit4 Reserved        0x10 */
/* Bit5: Is SNET, SLEN and SADR present */
#define CONTROL_SRC     0x08
#define CONTROL_REPLY   0x04
/*      priority 1      0x02 */
/*      priority 0      0x01 */

#define PRIORITY_NORMAL      0
#define PRIORITY_URGENT      1
#define PRIORITY_CRITICAL    2
#define PRIORITY_LIFE_SAFETY 3

#define npdu_is_net_msg(control)  ((control & CONTROL_NET_MSG) != 0)
#define npdu_has_dest(control)    ((control & CONTROL_DEST) != 0)
#define npdu_has_src(control)     ((control & CONTROL_SRC) != 0)
#define npdu_needs_reply(control) ((control & CONTROL_REPLY) != 0)
#define npdu_priority(control)    (control & 0x03)

typedef struct
{
    unsigned short net;                /* Network number */
    unsigned char  len;                /* Lenght of MAC address */
    unsigned char  mac[MAC_SIZE_MAX];  /* The MAC address */
} Bacnet_address_t;

typedef struct
{
    unsigned char          base[MAX_NPDU_SIZE]; /* The base for the npdu data */
    bool        has_header;
    unsigned char          *data;        /* Pointer to begining of data */
    unsigned short         data_len;     /* Length of the data */

    unsigned char          version;      /* Protocol Version */
    unsigned char          control;      /* Network Layer Protocol Control Information */
    Bacnet_address_t dest;         /* Destination address */
    Bacnet_address_t src;          /* Source address */
    unsigned char          hop_count;
    /* unsigned char          msg_type;      Network Message Type */
    /* unsigned short         vendor_id;     Only for Proprietary Network Messages */
    unsigned char          broadcast;    /* 0=NO 1=local 2=global */
} Bacnet_npdu_t;

#define NOT_BROADCAST    0
#define LOCAL_BROADCAST  1
#define GLOBAL_BROADCAST 2

/* npdu errors */
#define NPDU_ERR_NO_PDU              0x01
#define NPDU_ERR_NO_NPDU             0x02
#define NPDU_ERR_BAD_MAC_SIZE        0x03
#define NPDU_ERR_HEADER_ALREADY_SET  0x04
#define NPDU_ERR_BAD_PRIORITY        0x05

/* npdu return msg */
#define NPDU_MSG_NETWORK_MSG         0x10


unsigned char parse_npdu(unsigned char *pdu, unsigned short pdulen, Bacnet_npdu_t *npdu);

void init_npdu(Bacnet_npdu_t *npdu);
void init_npdu_data(Bacnet_npdu_t *npdu);

unsigned char add_npdu_header
(
	Bacnet_npdu_t *npdu,
	Bacnet_address_t *dest,
	Bacnet_address_t *src,
	bool need_reply,
	unsigned char priority
);

#endif
