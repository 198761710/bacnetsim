/*####COPYRIGHTBEGIN####
 -------------------------------------------
 Copyright (C) 2005 Wapice Ltd

 Author: Kåre Särs (kare.sars@wapice.com)

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License,or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not,write to:
 The Free Software Foundation,Inc.
 59 Temple Place - Suite 330
 Boston,MA  02111-1307
 USA.

 As a special exception,if other files instantiate templates or
 use macros or inline functions from this file,or you compile
 this file and link it with other works to produce a work based
 on this file,this file does not by itself cause the resulting
 work to be covered by the GNU General Public License. However
 the source code for this file must still be made available in
 accordance with section (3) of the GNU General Public License.

 This exception does not invalidate any other reasons why a work
 based on this file might be covered by the GNU General Public
 License.
 -------------------------------------------
####COPYRIGHTEND####*/


#ifndef BACNET_SEND_APDU_H
#define BACNET_SEND_APDU_H
#include "platform.h"

#ifdef JustForReading
Bacnet_npdu_t *apdu_simple_ack();
Bacnet_npdu_t *apdu_error();
Bacnet_npdu_t *apdu_abort();
Bacnet_npdu_t *apdu_reject();
Bacnet_npdu_t *apdu_iam();
Bacnet_npdu_t *apdu_ihave();
Bacnet_npdu_t *apdu_read_property();
Bacnet_npdu_t *apdu_read_property_ack();
Bacnet_npdu_t *apdu_who_is();
Bacnet_npdu_t *apdu_who_is_ranged();
Bacnet_npdu_t *apdu_who_has();
Bacnet_npdu_t *apdu_who_has_ranged();
Bacnet_npdu_t *apdu_write_prop_real();
Bacnet_npdu_t *apdu_write_prop_enum();
Bacnet_npdu_t *segmented_c_ack_pdu();
Bacnet_npdu_t *segment_ack_pdu();
#endif


Bacnet_npdu_t *apdu_simple_ack(
	Bacnet_npdu_t *npdu,
	unsigned char invoke_id,
	unsigned char service
);

Bacnet_npdu_t *apdu_error
(
	Bacnet_npdu_t *npdu,
	unsigned char invoke_id,
	unsigned char err_choice,
	unsigned char m_class,
	unsigned char code
);

Bacnet_npdu_t *apdu_abort
(
	Bacnet_npdu_t *npdu,
	unsigned char invoke_id,
	unsigned char reason
);

Bacnet_npdu_t *apdu_reject
(
	Bacnet_npdu_t *npdu,
	unsigned char invoke_id,
	unsigned char reason
);

Bacnet_npdu_t *apdu_iam
(
	Bacnet_npdu_t *npdu, 
	unsigned int instance, 
	unsigned short vendorId
);

Bacnet_npdu_t *apdu_read_property
(
	Bacnet_npdu_t *npdu,
	unsigned char InvokeId,
	unsigned short object_type,
	unsigned int instance,
	bool sa,
	unsigned char max_segs,
	unsigned char max_seg_size,
	unsigned char property,
	unsigned char *array
);

Bacnet_npdu_t *apdu_who_is
(
	Bacnet_npdu_t *npdu
);

Bacnet_npdu_t *apdu_who_is_ranged
(
	Bacnet_npdu_t *npdu,
	unsigned int instance_low,
	unsigned int instance_hi
);

Bacnet_npdu_t *apdu_who_has
(
	Bacnet_npdu_t *npdu,
	unsigned short object_type,
	unsigned int instance,
	unsigned char *name
);

Bacnet_npdu_t *apdu_who_has_ranged
(
	Bacnet_npdu_t *npdu,
	unsigned int instance_low,
	unsigned int instance_hi,
	unsigned short object_type,
	unsigned int instance,
	unsigned char *name
);


Bacnet_npdu_t *apdu_write_prop_real
(
	Bacnet_npdu_t *npdu,
	unsigned char InvokeId,
	unsigned short object_type,
	unsigned int instance,
	unsigned char property,
	REAL *value,
	unsigned char *array,
	unsigned char *priority
);

Bacnet_npdu_t *apdu_write_prop_enum
(
	Bacnet_npdu_t *npdu,
	unsigned char InvokeId,
	unsigned short object_type,
	unsigned int instance,
	unsigned char property,
	unsigned char *value,
	unsigned char *array,
	unsigned char *priority
);

Bacnet_npdu_t *segmented_c_ack_pdu
(
	Bacnet_npdu_t *npdu,
	unsigned char invoke_id,
	unsigned char *data,
	unsigned short data_len,
	unsigned short segment_size,
	unsigned char sequence_number,
	unsigned char proposedWS

);

Bacnet_npdu_t *segment_ack_pdu
(
	Bacnet_npdu_t *npdu,
	unsigned char invoke_id,
	unsigned char sequence_number,
	unsigned char actualWS,
	bool nak,
	bool srv
);
#endif
