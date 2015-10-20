/*
 *  io_stream.h - stream i/o wrapper - header
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
#ifndef IO_STREAM_H
#define IO_STREAM_H

#include "circ_buf.h"
#include <sys/time.h>

typedef struct io_stream
{
	int fd_in;         /* for reading */
	int fd_out;        /* for writing */
	int socktype;      /* type of the socket */

	int flags;         /* flags for status conditions */

	circ_buf_t *buf_in;  /* input buffer */
	circ_buf_t *buf_out; /* output buffer */

	size_t mtu;        /* Maximum Transmition Unit */
	size_t nru;        /* miNimum Receive Unit */

	bool half_close_suppress; /* true if half-closes should be suppressed */

	int idle_timeout;  /* time that the ios can idle for before timeout */
	struct timeval last_active; /* the time the ios was last active */

	int hold_time;     /* time to hold the stream open after read closes,
	                    * -1 means hold indefinately */
	struct timeval read_eof;    /* the time that eof was read */
	
	char *name;        /* the name of this io stream (for logging) */
	size_t rcvd;       /* bytes received */
	size_t sent;       /* bytes sent */
} io_stream_t;

/* status flags */
#define IOS_OK			 00
#define IOS_INPUT_EOF		 01
#define IOS_OUTPUT_EOF		 02
#define IOS_HOLD_TIMEDOUT	 04
#define IOS_IDLE_TIMEDOUT	010


void ios_init_socket(io_stream_t *ios, const char *name, int fd, int socktype,
		circ_buf_t *inbuf, circ_buf_t *outbuf);
void ios_init_stdio(io_stream_t *ios, const char *name,
		circ_buf_t *inbuf, circ_buf_t *outbuf);
void ios_init(io_stream_t *ios, const char *name,
		int fd_in, int fd_out, int socktype,
		circ_buf_t *inbuf, circ_buf_t *outbuf);

void io_stream_destroy(io_stream_t *ios);

/* sets the Maximum Transmition Unit
 * this is the maximum amount that is sent in any write */
#define ios_set_mtu(IOS, U)	((IOS)->mtu = (U))
/* sets the miNimum Receive Unit
 * this is the minimum amount of data that can be handled in any read */
#define ios_set_nru(IOS, U)	((IOS)->nru = (U))

/* sets half closes suppression */
#define ios_suppress_half_close(IOS, B)	((IOS)->half_close_suppress = (B))

/* sets the time (in sec) before an idle timeout occurs */
#define ios_set_idle_timeout(IOS, T)	((IOS)->idle_timeout = (T))

/* sets the time (in sec) after read is shutdown that timeout occurs */
#define ios_set_hold_timeout(IOS, T)	((IOS)->hold_time = (T))


/* returns an fd if the stream should be scheduled for read, -1 otherwise */
int ios_schedule_read(io_stream_t *ios);
/* returns an fd if the stream should be scheduled for write, -1 otherwise */
int ios_schedule_write(io_stream_t *ios);

/* writes the interval to the next timeout into tv and returns a pointer
 * to tv.  If no timeout is active, NULL is returned and tv is unchanged. */
struct timeval *ios_next_timeout(io_stream_t *ios, struct timeval *tv);

/* check what sort of timeout has occured */
#define ios_idle_timedout(IOS)		((IOS)->flags & IOS_IDLE_TIMEDOUT)
#define ios_hold_timedout(IOS)		((IOS)->flags & IOS_HOLD_TIMEDOUT)


/* read into the input buffer.
 * should only be called if ios_schedule_read returned a true value.
 * returns the total bytes read, or a negative error code */
ssize_t ios_read(io_stream_t *ios);
/* write from the output buffer.
 * should only be called if ios_schedule_write returned a true value
 * returns the total bytes read, or a negative error code */
ssize_t ios_write(io_stream_t *ios);

/* error return values from ios_read/ios_write */
#define IOS_FAILED	-1
#define IOS_EOF		-2


/* signal that no more data will be added to the output buffer.  Once the
 * buffer is cleared, the io_stream will shutdown it's write stream */
void ios_write_eof(io_stream_t *ios);


#define is_read_open(IOS)   ((IOS)->fd_in >= 0)
#define is_write_open(IOS)  ((IOS)->fd_out >= 0)

/* shutdown the stream as per shutdown(2) */
void ios_shutdown(io_stream_t *ios, int how);


#define ios_bytes_received(IOS)	((IOS)->rcvd)
#define ios_bytes_sent(IOS)	((IOS)->sent)

/* set the name of the io_stream */
#define ios_name(IOS)		((IOS)->name)


#endif/*IO_STREAM_H*/
