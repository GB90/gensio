/*
 *  ioinfo - A program for connecting gensios.
 *  Copyright (C) 2019  Corey Minyard <minyard@acm.org>
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

/*
 * ioinfo is a tool that connects two gensios together and transfers data
 * between them.
 *
 * ioinfo can watch for an escape character read from a gensio and do
 * special handling on the next character(s).  It has a plugin to
 * extend the escape character handling.
 *
 * To use this, you:
 * * (optional) Set up sub handlers for handling escape characters
 *   outside of the normal ones.
 * * Create a struct ioinfo_user_handlers for handling callbacks from
 *   ioinfo
 * * Allocate an ioinfo for each gensio and set the otherinfo so they
 *   point to each other.
 * * Allocate the gensios (or if using an accepting gensio, use the
 *   gensio each provide it.
 * * When a gensio is ready, set the ioinfo ready.  You don't have to
 *   do this for both at the same time, if you do it for one, it will
 *   not use the other until the other is ready.
 *
 * When both are ready, it will start transferring data between the
 * two gensios.
 *
 * The ioinfo handles three escape characters itself.  Any other
 * escape characters are handled by sub handlers.  If an escape
 * character is not recognized, it is ignored.  The ones handled by
 * ioinfo are:
 *
 *  * <escape char> - Send the escape char.  To send it the
 *    escape character requires entering it twice in succession.
 *  * q - Terminate the connection
 *  * b - Send a break on the other gensio.  The meaning of this
 *    depends on the other gensio, it may be ignored.
 */

#ifndef IOINFO_H
#define IOINFO_H

#include <stdarg.h>
#include <gensio/gensio.h>

struct ioinfo;

/*
 * Function calls for handling escape characters and special functions.
 */
struct ioinfo_sub_handlers {
    /*
     * Handle a gensio event that ioinfo does not handle.  This can be
     * used for special serial port handling, for instance.  Should
     * return ENOTSUP if the event handler did not handle the event.
     */
    int (*handle_event)(struct gensio *io, int event,
			unsigned char *buf, gensiods *buflen);

    /*
     * Handle an escape character.  If this returns true, then the
     * ioinfo will go into multichar mode where it collects characters
     * until it gets a \r or \n, then calls handle_multichar_escape with
     * the data.
     */
    bool (*handle_escape)(struct ioinfo *ioinfo, char c);

    /*
     * Handle a multi-character escape sequence after it has been
     * received.
     */
    void (*handle_multichar_escape)(struct ioinfo *ioinfo, char *escape_data);
};

/*
 * Function calls the user of the ioinfo must provide.
 */
struct ioinfo_user_handlers {
    /*
     * Called when an error occurs on the gensios or when escape-q is
     * received.  The user should shut down the gensios.
     */
    void (*shutdown)(struct ioinfo *ioinfo);

    /*
     * Called to report an error received from the gensio.
     */
    void (*err)(struct ioinfo *ioinfo, char *fmt, va_list va);

    /*
     * Called when something in the ioinfo or sub-ioinfo wants to
     * display output to the user.
     */
    void (*out)(struct ioinfo *ioinfo, char *fmt, va_list va);
};

/* Get the ioinfo for the other side of the connection. */
struct gensio *ioinfo_otherio(struct ioinfo *ioinfo);

/* Get the data for the sub handler. */
void *ioinfo_subdata(struct ioinfo *ioinfo);

/* Get the data for the other side's sub handler. */
void *ioinfo_othersubdata(struct ioinfo *ioinfo);

/* Get the user data supplied when the ioinfo was allocated. */
void *ioinfo_userdata(struct ioinfo *ioinfo);

/*
 * Set each other side's ioinfo for a connection.  Both sides are set,
 * so you only need to call this once.
 */
void ioinfo_set_otherioinfo(struct ioinfo *ioinfo, struct ioinfo *otherioinfo);

/*
 * Set the ioinfo as ready.  This sets the gensio for ioinfo, turns on
 * read for the gensio, and marks itself ready.  This means that it
 * will receive data from the gensio and from the other side.  If the
 * other side is not ready, it will drop any received data (though it
 * still does escape and sub handling).
 */
void ioinfo_set_ready(struct ioinfo *ioinfo, struct gensio *io);

/* Send data to the ioinfo user's out function. */
void ioinfo_out(struct ioinfo *ioinfo, char *fmt, ...);

/* Send data to the ioinfo user's err function. */
void ioinfo_err(struct ioinfo *ioinfo, char *fmt, ...);

/*
 * Allocate an ioinfo.
 *
 * If escape_char >= 0, the ioinfo will monitor for that character and
 * if it see it, it will handle the next character as an escape.
 *
 * sh provides a way to plug in special handling for events and escape
 * characters.  It may be NULL, disabling the function.
 *
 * The user must provide a handler.
 */
struct ioinfo *alloc_ioinfo(struct gensio_os_funcs *o,
			    int escape_char,
			    struct ioinfo_sub_handlers *sh, void *subdata,
			    struct ioinfo_user_handlers *uh, void *userdata);

/* Free the ioinfo. */
void free_ioinfo(struct ioinfo *ioinfo);

#endif /* IOINFO_H */