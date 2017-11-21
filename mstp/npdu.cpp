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

/***************************************************************/
/* For details read clause 6.2 Network Layer PDU structure */
/***************************************************************/

#include <sys/types.h>
#include <string.h>
#include "platform.h"
#include "conf.h"
#include "npdu.h"


/***************************************************************/
unsigned char parse_npdu(unsigned char *pdu, 
						 unsigned short pdulen, 
						 Bacnet_npdu_t *npdu)
{
    unsigned short i = 0;
	unsigned short j = 0;

    if( pdu == NULL )
	{
		return NPDU_ERR_NO_PDU;
	}
    if( npdu == NULL )
	{
		return NPDU_ERR_NO_NPDU;
	}

    npdu->data_len = pdulen;
    npdu->version = pdu[0];
    npdu->control = pdu[1];
    i = 2;

    if( npdu_is_net_msg(npdu->control) )
    {
        /* This IS a network message */
        return NPDU_MSG_NETWORK_MSG;
    }
    else
    {
        /* This is NOT a network message */
        /* Destination address */
        if (npdu_has_dest(npdu->control))
        {
            npdu->dest.net = (pdu[i] * 256) + pdu[i+1];
            npdu->dest.len = pdu[i+2];
            if (npdu->dest.len > MAC_SIZE_MAX)
			{
				return NPDU_ERR_BAD_MAC_SIZE;
			}
            i += 3;
            for(j = 0; j < npdu->dest.len; j++)
            {
                npdu->dest.mac[j] = pdu[i];
                i++;
            }
        }
        else
        {
            npdu->dest.net = 0;
            npdu->dest.len = 0;
        }

        /* Source address */
        if( npdu_has_src(npdu->control) )
        {
            npdu->src.net = (pdu[i] * 256) + pdu[i+1];
            npdu->src.len = pdu[i+2];
            if( npdu->src.len > MAC_SIZE_MAX )
			{
				return NPDU_ERR_BAD_MAC_SIZE;
			}
            i += 3;
            for(j = 0; j < npdu->src.len; j++)
            {
                npdu->src.mac[j] = pdu[i];
                i++;
            }
        }
        else
        {
            npdu->src.net = 0;
            npdu->src.len = 0;
        }

        /* Hop Count */
        if( npdu_has_dest(npdu->control) )
        {
            npdu->hop_count = pdu[i];
            i++;
        }

        /* APDU */
        npdu->data = &pdu[i];
        npdu->data_len -= i;
    }

    return 0;
}

/***************************************************************/
void init_npdu(Bacnet_npdu_t *npdu)
{
    npdu->has_header = FALSE;
    /* We leave space for a header */
    npdu->data = npdu->base + NPDU_HEADER_MAX;
    npdu->data_len = 0;

    npdu->version = BACNET_PROTOCOL_VERSION;
    npdu->control = 0;
    npdu->dest.net = 0;
    npdu->dest.len = 0;
    npdu->dest.mac[0] = 0;
    npdu->dest.mac[1] = 0;
    npdu->dest.mac[2] = 0;
    npdu->dest.mac[3] = 0;
    npdu->dest.mac[4] = 0;
    npdu->dest.mac[5] = 0;
    npdu->dest.mac[6] = 0;
    npdu->src.net = 0;
    npdu->src.len = 0;
    npdu->src.mac[0] = 0;
    npdu->src.mac[1] = 0;
    npdu->src.mac[2] = 0;
    npdu->src.mac[3] = 0;
    npdu->src.mac[4] = 0;
    npdu->src.mac[5] = 0;
    npdu->src.mac[6] = 0;
    npdu->hop_count = 0xff;
    npdu->broadcast = NOT_BROADCAST;

    return;
}


/***************************************************************/
void init_npdu_data(Bacnet_npdu_t *npdu)
{
    npdu->has_header = FALSE;
    /* We leave space for a header */
    npdu->data = npdu->base + NPDU_HEADER_MAX;
    npdu->data_len = 0;
    npdu->broadcast = NOT_BROADCAST;

    return;
}


/***************************************************************/
unsigned char add_npdu_header
(
	Bacnet_npdu_t *npdu,
	Bacnet_address_t *dest,
	Bacnet_address_t *src,
	bool need_reply,
	unsigned char priority
)
{
    unsigned char i = 0;
	unsigned char j = 0;
    unsigned char header_size=2; /* version + control */

    if (npdu == NULL)             return NPDU_ERR_NO_NPDU;
    if (npdu->has_header == TRUE) return NPDU_ERR_HEADER_ALREADY_SET;
    if ((priority & ~0x03) != 0)  return NPDU_ERR_BAD_PRIORITY;

    /* Clear the control data */
    npdu->control = 0;
    /* Set the control data */
    /* destination */
    if (dest != NULL)
    {
        npdu->control |= CONTROL_DEST;
        header_size += 4 + dest->len; /* DNET(2)+DLEN(1)+DADR(N)+HopCount */
    }
    /* source */
    if (src  != NULL)
    {
        npdu->control |= CONTROL_SRC;
        header_size += 3 + src->len; /* SNET(2)+SLEN(1)+SADR(N) */
    }
    /* reply */
    if (need_reply == TRUE) npdu->control |= CONTROL_REPLY;
    /* priority */
    npdu->control |= priority;

    /* move start of data back "header_size" bytes */
    npdu->data -= header_size;

    npdu->data[0] = BACNET_PROTOCOL_VERSION;
    npdu->data[1] = npdu->control;
    i = 2;



    if (npdu_has_dest(npdu->control))
    {
        /* Add Destination Address */
        npdu->data[i] = ((dest->net & 0xff00) >> 8);
        npdu->data[i+1] = dest->net & 0x00ff;
        npdu->data[i+2] = dest->len;
        if (dest->len > MAC_SIZE_MAX) return NPDU_ERR_BAD_MAC_SIZE;
        i += 3;
        for(j = 0; j < dest->len; j++)
        {
            npdu->data[i] = dest->mac[j];
            i++;
        }
    }

    if( npdu_has_src(npdu->control) )
    {
        /* Add Source address */
        npdu->data[i] = (src->net & 0xff00) >> 8;
        npdu->data[i+1] = src->net & 0x00ff;
        npdu->data[i+2] = src->len;
        if( src->len > MAC_SIZE_MAX )
		{
			return NPDU_ERR_BAD_MAC_SIZE;
		}
        i += 3;
        for(j = 0; j < src->len; j++)
        {
            npdu->data[i] = src->mac[j];
            i++;
        }
    }

    if( npdu_has_dest(npdu->control) )
    {
        /* Add Hop Count */
        npdu->data[i] = npdu->hop_count; /* should be 0xff */
        i++;
    }
    npdu->data_len += header_size;

    return 0;
}
