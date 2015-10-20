/*
 *  connection.c - connection description structures and functions - implementation
 * 
 *  nc6 - an advanced netcat clone
 *  Copyright (C) 2002-2006 Chris Leishman <chris _at_ leishman.org>
 *  Copyright (C) 2001-2006 Mauro Tortonesi <mauro _at_ deepspace6.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */  
#include "system.h"
#include "misc.h"
#include "connection.h"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <assert.h>
#include <netinet/in.h>

RCSID("@(#) $Header: /ds6/cvs/nc6/src/connection.c,v 1.33 2006/01/19 22:46:23 chris Exp $");

/* default buffer size is 8kb */
static const size_t DEFAULT_BUFFER_SIZE = 8192;



void ca_init(connection_attributes_t *attrs)
{
	assert(attrs != NULL);

	attrs->family = PROTO_UNSPECIFIED;
	attrs->protocol = TCP_PROTOCOL;

	address_init(&(attrs->remote_address));
	address_init(&(attrs->local_address));

	/* set default values for everything */
	attrs->flags = 0;
	attrs->buffer_size = DEFAULT_BUFFER_SIZE;
	attrs->remote_mtu = 0;
	attrs->remote_nru = 0;
	attrs->sndbuf_size = 0;
	attrs->rcvbuf_size = 0;
	attrs->connect_timeout = -1;
	attrs->idle_timeout = -1;
	attrs->local_hold_timeout = -1;
	attrs->remote_hold_timeout = 0;
	attrs->remote_half_close_suppress = true;
	attrs->local_half_close_suppress = false;
	attrs->local_exec = NULL;
}



void ca_destroy(connection_attributes_t *attrs)
{
	assert(attrs != NULL);
	ca_set_local_exec(attrs, NULL);
}



void ca_set_local_exec(connection_attributes_t *attrs, const char *exec)
{
	if (attrs->local_exec)
		free(attrs->local_exec);
	attrs->local_exec = exec? xstrdup(exec) : NULL;
}



void ca_to_addrinfo(struct addrinfo *ainfo,
		const connection_attributes_t *attrs)
{
	assert(ainfo != NULL);
	assert(attrs != NULL);

	switch (attrs->family) {
	case PROTO_UNSPECIFIED:
		ainfo->ai_family = PF_UNSPEC;
		break;
	case PROTO_IPv6:
#ifdef ENABLE_IPV6
		ainfo->ai_family = PF_INET6;
#else
		fatal_internal("unavailable IPv6 support required");
#endif
		break;
	case PROTO_IPv4:
		ainfo->ai_family = PF_INET;
		break;
	case PROTO_BLUEZ:
#ifdef ENABLE_BLUEZ
		ainfo->ai_family = PF_BLUETOOTH;
#else
		fatal_internal("unavailable bluez support required");
#endif
		break;
	default:
		fatal_internal("unknown socket domain");
	}
	
	switch (attrs->protocol) {
	case UDP_PROTOCOL:
		ainfo->ai_protocol = IPPROTO_UDP;
		/* strictly speaking, this should not be required since UDP
		 * implies a DGRAM type socket.  However, on some systems
		 * getaddrinfo fails if we set IPPROTO_UDP and don't set this */
		ainfo->ai_socktype = SOCK_DGRAM;
		break;
	case TCP_PROTOCOL:
		ainfo->ai_protocol = IPPROTO_TCP;
		/* strictly speaking, this should not be required since TCP
		 * implies a STREAM type socket.  However, on some systems
		 * getaddrinfo fails if we set IPPROTO_TCP and don't set this */
		ainfo->ai_socktype = SOCK_STREAM;
		break;
	case SCO_PROTOCOL:
#ifdef ENABLE_BLUEZ
		ainfo->ai_protocol = BTPROTO_SCO;
		ainfo->ai_socktype = SOCK_SEQPACKET;
#else
		fatal_internal("unavailable bluez support required");
#endif
		break;
	case L2CAP_PROTOCOL:
#ifdef ENABLE_BLUEZ
		ainfo->ai_protocol = BTPROTO_L2CAP;
		ainfo->ai_socktype = SOCK_SEQPACKET;
#else
		fatal_internal("unavailable bluez support required");
#endif
		break;
	default:
		fatal_internal("unknown socket type");
	}
}
