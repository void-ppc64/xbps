/*-
 * Copyright (c) 2011 Juan Romero Pardines.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_VASPRINTF
# define _GNU_SOURCE    /* for vasprintf(3) */
#endif

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "xbps_api_impl.h"

void HIDDEN
xbps_set_cb_fetch(off_t file_size,
		  off_t file_offset,
		  off_t file_dloaded,
		  const char *file_name,
		  bool cb_start,
		  bool cb_update,
		  bool cb_end)
{
	const struct xbps_handle *xhp = xbps_handle_get();
	struct xbps_fetch_cb_data *xfcd;

	if (xhp->fetch_cb == NULL)
		return;

	xfcd = malloc(sizeof(*xfcd));
	if (xfcd == NULL)
		return;

	xfcd->file_size = file_size;
	xfcd->file_offset = file_offset;
	xfcd->file_dloaded = file_dloaded;
	xfcd->file_name = file_name;
	xfcd->cb_start = cb_start;
	xfcd->cb_update = cb_update;
	xfcd->cb_end = cb_end;
	(*xhp->fetch_cb)(xfcd, xhp->fetch_cb_data);
	free(xfcd);
}

void HIDDEN
xbps_set_cb_state(xbps_state_t state,
		  int err,
		  const char *pkgname,
		  const char *version,
		  const char *fmt,
		  ...)
{
	struct xbps_state_cb_data *xscd;
	const struct xbps_handle *xhp = xbps_handle_get();
	char *buf = NULL;
	va_list va;
	int retval;

	if (xhp->state_cb == NULL)
		return;

	xscd = malloc(sizeof(*xscd));
	if (xscd == NULL)
		return;

	xscd->state = state;
	xscd->err = err;
	xscd->pkgname = pkgname;
	xscd->version = version;
	if (fmt != NULL) {
		va_start(va, fmt);
		retval = vasprintf(&buf, fmt, va);
		va_end(va);
		if (retval == -1)
			xscd->desc = NULL;
		else
			xscd->desc = buf;
	}
	(*xhp->state_cb)(xscd, xhp->fetch_cb_data);
	if (buf != NULL)
		free(buf);

	free(xscd);
}

void HIDDEN
xbps_set_cb_unpack(const char *entry,
		   int64_t entry_size,
		   ssize_t entry_extract_count,
		   ssize_t entry_total_count,
		   bool entry_is_metadata,
		   bool entry_is_conf)
{
	const struct xbps_handle *xhp = xbps_handle_get();
	struct xbps_unpack_cb_data *xucd;

	if (xhp->unpack_cb == NULL)
		return;

	xucd = malloc(sizeof(*xucd));
	if (xucd == NULL)
		return;

	xucd->entry = entry;
	xucd->entry_size = entry_size;
	xucd->entry_extract_count = entry_extract_count;
	xucd->entry_total_count = entry_total_count;
	xucd->entry_is_metadata = entry_is_metadata;
	xucd->entry_is_conf = entry_is_conf;
	(*xhp->unpack_cb)(xucd, xhp->unpack_cb_data);
	free(xucd);
}