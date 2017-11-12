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

#include <sys/types.h>
#include <string.h>
#include "conf.h"
#include "npdu.h"
#include "apdu.h"
#include "bacenum.h"
#include "bacdcode.h"
#include "make_apdu.h"

Bacnet_npdu_t *apdu_simple_ack(Bacnet_npdu_t *npdu, unsigned char invoke_id, unsigned char service)
{
    init_npdu_data(npdu);

    npdu->data[0] = APDU_TYPE_SIMPLE_ACK;
    npdu->data[1] = invoke_id;
    npdu->data[2] = service;
    npdu->data_len = 3;

    return npdu;
}

Bacnet_npdu_t *apdu_error(Bacnet_npdu_t *npdu, 
						  unsigned char invoke_id, 
						  unsigned char err_choice, 
						  unsigned char m_class, unsigned char code)
{
    init_npdu_data(npdu);

    npdu->data[0] = APDU_TYPE_ERROR;
    npdu->data[1] = invoke_id;
    npdu->data[2] = err_choice;
    npdu->data_len = 3;

    npdu->data_len += encode_tagged_enum(&npdu->data[npdu->data_len], m_class);
    npdu->data_len += encode_tagged_enum(&npdu->data[npdu->data_len], code);

    return npdu;
}

Bacnet_npdu_t *apdu_abort(Bacnet_npdu_t *npdu, unsigned char invoke_id, unsigned char reason)
{
    init_npdu_data(npdu);

    npdu->data[0] = APDU_TYPE_ABORT | APDU_SRV;
    npdu->data[1] = invoke_id;
    npdu->data[2] = reason;
    npdu->data_len = 3;

    return npdu;
}

Bacnet_npdu_t *apdu_reject(Bacnet_npdu_t *npdu, unsigned char invoke_id, unsigned char reason)
{
    init_npdu_data(npdu);

    npdu->data[0] = APDU_TYPE_REJECT;
    npdu->data[1] = invoke_id;
    npdu->data[2] = reason;
    npdu->data_len = 3;

    return npdu;
}

Bacnet_npdu_t *apdu_iam(Bacnet_npdu_t *npdu, unsigned int instance, unsigned short vendorId)
{
    init_npdu_data(npdu);

    npdu->data[0] = APDU_TYPE_UNCONFIRMED_REQUEST;
    npdu->data[1] = SERVICE_UNCONFIRMED_I_AM;
    npdu->data_len = 2;

    npdu->data_len += encode_tagged_object_id(&(npdu->data[npdu->data_len]),
											  OBJECT_DEVICE, 
											  instance);
    npdu->data_len += encode_tagged_unsigned(&(npdu->data[npdu->data_len]), 
											MAX_APDU_LEN);
    npdu->data_len += encode_tagged_enum(&(npdu->data[npdu->data_len]), 
										SUPPORTED_SEGMENTATION);
    npdu->data_len += encode_tagged_unsigned(&(npdu->data[npdu->data_len]), 
											vendorId);
    npdu->broadcast = GLOBAL_BROADCAST;

    return npdu;
}

Bacnet_npdu_t *apdu_read_property(Bacnet_npdu_t *npdu, 
								  unsigned char InvokeId,
                                  unsigned short object_type, 
								  unsigned int instance,
                                  bool sa, 
								  unsigned char max_segs, 
								  unsigned char max_seg_size,
                                  unsigned char property, 
								  unsigned char *array)
{
    if( npdu == NULL )
	{
		return NULL;
	}

    init_npdu_data(npdu);

    npdu->data[0] = APDU_TYPE_CONFIRMED_REQUEST;
    if( sa == TRUE )
    {
        npdu->data[0] |= APDU_SEG_ACC;
        if( (max_segs > 7) || (max_seg_size > 15) )
		{
			return NULL;
		}
        npdu->data[1] = (max_segs << 4) | max_seg_size;
    }
    else
    {
        npdu->data[1] = 0;
    }
    npdu->data[2] = InvokeId;
    npdu->data[3] = SERVICE_CONFIRMED_READ_PROPERTY;
    npdu->data_len = 4;

    npdu->data_len += encode_context_object_id(&npdu->data[npdu->data_len], 
												0, 
												object_type, 
												instance);
    npdu->data_len += encode_context_unsigned(&npdu->data[npdu->data_len], 
												1, 
												property);

    if (array != NULL)
    {
        npdu->data_len += encode_context_unsigned(&npdu->data[npdu->data_len], 
													2, 
													*array);
    }

    return npdu;
}


Bacnet_npdu_t *apdu_who_is(Bacnet_npdu_t *npdu)
{
    if( npdu == NULL )
	{
		return NULL;
	}

    init_npdu_data(npdu);

    npdu->data[0] = APDU_TYPE_UNCONFIRMED_REQUEST;
    npdu->data[1] = SERVICE_UNCONFIRMED_WHO_IS;
    npdu->data_len = 2;

    return npdu;
}

Bacnet_npdu_t *apdu_who_is_ranged(Bacnet_npdu_t *npdu, 
								  unsigned int instance_low, 
								  unsigned int instance_hi)
{
    if( npdu == NULL )
	{
		return NULL;
	}

    init_npdu_data(npdu);

    npdu->data[0] = APDU_TYPE_UNCONFIRMED_REQUEST;
    npdu->data[1] = SERVICE_UNCONFIRMED_WHO_IS;
    npdu->data_len = 2;

    npdu->data_len += encode_context_unsigned(&npdu->data[npdu->data_len], 
												0, 
												instance_low);
    npdu->data_len += encode_context_unsigned(&npdu->data[npdu->data_len], 
												1, 
												instance_hi);

    return npdu;
}


Bacnet_npdu_t *apdu_who_has(Bacnet_npdu_t *npdu, 
							unsigned short object_type, 
							unsigned int instance, 
							unsigned char *name)
{
    if( npdu == NULL )
	{
		return NULL;
	}

    init_npdu_data(npdu);

    npdu->data[0] = APDU_TYPE_UNCONFIRMED_REQUEST;
    npdu->data[1] = SERVICE_UNCONFIRMED_WHO_HAS;
    npdu->data_len = 2;

    if (name == NULL)
    {
        npdu->data_len += encode_context_object_id(&npdu->data[npdu->data_len], 
													2, 
													object_type, instance);
    }
    else
    {
        npdu->data_len += encode_context_char_string(&npdu->data[npdu->data_len], 
													3, 
													name);
    }

    return npdu;
}

/* from clause 20.1.3.3 (Unconfirmed Request) */
/* and clause 16.9.1 and clause 21 Formal description... */
/**********************************************************************/
Bacnet_npdu_t *apdu_who_has_ranged(Bacnet_npdu_t *npdu, 
								   unsigned int instance_low, 
								   unsigned int instance_hi,
                                   unsigned short object_type, 
								   unsigned int instance, 
								   unsigned char *name)
{
    if( npdu == NULL )
	{
		return NULL;
	}

    init_npdu_data(npdu);

    npdu->data[0] = APDU_TYPE_UNCONFIRMED_REQUEST;
    npdu->data[1] = SERVICE_UNCONFIRMED_WHO_HAS;
    npdu->data_len = 2;

    npdu->data_len += encode_context_unsigned(&npdu->data[npdu->data_len], 
											  0, 
											  instance_low);
    npdu->data_len += encode_context_unsigned(&npdu->data[npdu->data_len], 
											  1, 
											  instance_hi);

    if (name == NULL)
    {
        npdu->data_len += encode_context_object_id(&npdu->data[npdu->data_len], 
												   2, 
												   object_type, instance);
    }
    else
    {
        npdu->data_len += encode_context_char_string(&npdu->data[npdu->data_len], 
													3, 
													name);
    }

    return npdu;
}



Bacnet_npdu_t *apdu_write_prop_real(Bacnet_npdu_t *npdu, 
									unsigned char InvokeId,
                                    unsigned short object_type, 
									unsigned int instance, 
									unsigned char property,
                                    REAL *value, 
									unsigned char *array, 
									unsigned char *priority)
{
    if( npdu == NULL )
	{
		return NULL;
	}

    init_npdu_data(npdu);

    npdu->data[0] = APDU_TYPE_CONFIRMED_REQUEST;
    npdu->data[1] = 0;
    npdu->data[2] = InvokeId;
    npdu->data[3] = SERVICE_CONFIRMED_WRITE_PROPERTY;
    npdu->data_len= 4;

    npdu->data_len += encode_context_object_id(&npdu->data[npdu->data_len], 
												0, 
												object_type, instance);
    npdu->data_len += encode_context_unsigned(&npdu->data[npdu->data_len], 
												1, 
												property);
    if( array != NULL )
    {
        npdu->data_len += encode_context_unsigned(&npdu->data[npdu->data_len], 
												  2, 
												  *array);
    }
    npdu->data_len += encode_opening_tag(&npdu->data[npdu->data_len], 3);
    if( value != NULL )
	{
		npdu->data_len += encode_tagged_real(&npdu->data[npdu->data_len], *value);
	}
    else
	{
		npdu->data_len += encode_tagged_null(&npdu->data[npdu->data_len]);
	}
    npdu->data_len += encode_closing_tag(&npdu->data[npdu->data_len], 3);

    if( priority != NULL )
    {
        npdu->data_len += encode_context_unsigned(&npdu->data[npdu->data_len], 
													4, 
													*priority);
    }

    return npdu;
}


Bacnet_npdu_t *apdu_write_prop_enum(Bacnet_npdu_t *npdu, 
									unsigned char InvokeId,
                                    unsigned short object_type, 
									unsigned int instance, 
									unsigned char property,
                                    unsigned char *value, 
									unsigned char *array, 
									unsigned char *priority)
{
    if( npdu == NULL )
	{
		return NULL;
	}

    init_npdu_data(npdu);

    npdu->data[0] = APDU_TYPE_CONFIRMED_REQUEST;
    npdu->data[1] = 0;
    npdu->data[2] = InvokeId;
    npdu->data[3] = SERVICE_CONFIRMED_WRITE_PROPERTY;
    npdu->data_len= 4;

    npdu->data_len += encode_context_object_id(&npdu->data[npdu->data_len], 
												0, 
												object_type, instance);
    npdu->data_len += encode_context_unsigned(&npdu->data[npdu->data_len], 
												1, 
												property);
    if (array != NULL)
    {
        npdu->data_len += encode_context_unsigned(&npdu->data[npdu->data_len], 
													2, 
													*array);
    }
    npdu->data_len += encode_opening_tag(&npdu->data[npdu->data_len], 3);
    if (value != NULL)
	{
		npdu->data_len += encode_tagged_enum(&npdu->data[npdu->data_len], *value);
	}
    else
	{
		npdu->data_len += encode_tagged_null(&npdu->data[npdu->data_len]);
	}
    npdu->data_len += encode_closing_tag(&npdu->data[npdu->data_len], 3);

    if (priority != NULL)
    {
        npdu->data_len += encode_context_unsigned(&npdu->data[npdu->data_len], 
													4, 
													*priority);
    }

    return npdu;
}



/* from clause 20.1.5 (Complex ACK PDU) */
/**********************************************************************/
Bacnet_npdu_t *segmented_c_ack_pdu(Bacnet_npdu_t *npdu, 
								   unsigned char invoke_id, 
								   unsigned char *data, 
								   unsigned short data_len,
                                   unsigned short segment_size, 
								   unsigned char sequence_number, 
								   unsigned char proposedWS)
{
    unsigned short i = 0;
	unsigned short data_start = 0;
	unsigned short data_end = 0;
	unsigned short segment_data_size = 0;

    if( npdu == NULL )
	{
		return NULL;
	}
    if( data == NULL )
	{
		return NULL;
	}
    /* clear the old npdu data */
    init_npdu_data(npdu);

    /* calculate starting point and end point */
    /* FIX_ME this has to be recalculated to correct sizes !!!!!!! */
    segment_data_size = segment_size - 5;
    data_start = ((sequence_number+0) * (segment_data_size)) + 3;
    data_end   = ((sequence_number+1) * (segment_data_size)) + 3;

    if( data_end > data_len )
	{
		data_end = data_len;
	}
    if( data_start >= data_len )
	{
		return NULL;
	}

    npdu->data[0] = data[0] | APDU_SEGMENTED;
    if( data_end < data_len )
	{
		npdu->data[0] |= APDU_MOR_SEGS;
	}
    npdu->data[1] = invoke_id;
    npdu->data[2] = sequence_number;
    npdu->data[3] = proposedWS;
    npdu->data[4] = data[2];
    npdu->data_len = 5;

    /* Service Ack */
    for (i=0; i<(data_end - data_start); i++)
    {
        npdu->data[i+5] = data[i + data_start];
        npdu->data_len++;
    }

    /* now return this segment for sending */
    return npdu;
}


/* from clause 20.1.6 (Segment ACK PDU) */
/**********************************************************************/
Bacnet_npdu_t *segment_ack_pdu(Bacnet_npdu_t *npdu, 
							   unsigned char invoke_id, 
							   unsigned char sequence_number,
                               unsigned char actualWS, 
							   bool nak, 
							   bool srv)
{
    if( npdu == NULL )
	{
		return NULL;
	}
    /* clear the old npdu data */
    init_npdu_data(npdu);

    npdu->data[0] = APDU_TYPE_SEGMENT_ACK;
    if( nak == TRUE )
	{
		npdu->data[0] |= APDU_NAK;
	}
    if( srv == TRUE )
	{
		npdu->data[0] |= APDU_SRV;
	}
    npdu->data[1] = invoke_id;
    npdu->data[2] = sequence_number;
    npdu->data[3] = actualWS;
    npdu->data_len = 4;

    /* now return this for sending */
    return npdu;
}

