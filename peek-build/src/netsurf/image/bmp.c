/*
 * Copyright 2006 Richard Wilson <info@tinct.net>
 * Copyright 2008 Sean Fox <dyntryx@gmail.com>
 *
 * This file is part of NetSurf, http://www.netsurf-browser.org/
 *
 * NetSurf is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * NetSurf is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/** \file
 * Content for image/bmp (implementation)
 */

#include "utils/config.h"
#ifdef WITH_BMP

#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <libnsbmp.h>
#include "utils/config.h"
#include "content/content_protected.h"
#include "desktop/plotters.h"
#include "image/bitmap.h"
#include "image/bmp.h"
#include "utils/log.h"
#include "utils/messages.h"
#include "utils/utils.h"

/* The Bitmap callbacks function table;
 * necessary for interaction with nsbmplib.
 */
bmp_bitmap_callback_vt bmp_bitmap_callbacks = {
	nsbmp_bitmap_create,
	bitmap_destroy,
	bitmap_set_suspendable,
	bitmap_get_buffer,
	bitmap_get_bpp
};

bool nsbmp_create(struct content *c, const struct http_parameter *params)
{
	union content_msg_data msg_data;

	c->data.bmp.bmp = calloc(sizeof(struct bmp_image), 1);
	if (!c->data.bmp.bmp) {
		msg_data.error = messages_get("NoMemory");
		content_broadcast(c, CONTENT_MSG_ERROR, msg_data);
		return false;
	}
	bmp_create(c->data.bmp.bmp, &bmp_bitmap_callbacks);
	return true;
}


bool nsbmp_convert(struct content *c)
{
	bmp_result res;
	bmp_image *bmp;
	union content_msg_data msg_data;
	uint32_t swidth;
	const char *data;
	unsigned long size;
	char title[100];

	/* set the bmp data */
	bmp = c->data.bmp.bmp;

	data = content__get_source_data(c, &size);

	/* analyse the BMP */
	res = bmp_analyse(bmp, size, (unsigned char *) data);
	switch (res) {
		case BMP_OK:
			break;
		case BMP_INSUFFICIENT_MEMORY:
			msg_data.error = messages_get("NoMemory");
			content_broadcast(c, CONTENT_MSG_ERROR, msg_data);
			return false;
		case BMP_INSUFFICIENT_DATA:
		case BMP_DATA_ERROR:
			msg_data.error = messages_get("BadBMP");
			content_broadcast(c, CONTENT_MSG_ERROR, msg_data);
			return false;
	}

	/* Store our content width and description */
	c->width = bmp->width;
	c->height = bmp->height;
	LOG(("BMP      width %u       height %u", c->width, c->height));
	snprintf(title, sizeof(title), messages_get("BMPTitle"),
			c->width, c->height, size);
	content__set_title(c, title);
	swidth = bmp->bitmap_callbacks.bitmap_get_bpp(bmp->bitmap) * bmp->width;
	c->size += (swidth * bmp->height) + 16 + 44;

	/* exit as a success */
	c->bitmap = bmp->bitmap;
	bitmap_modified(c->bitmap);
	c->status = CONTENT_STATUS_DONE;

	/* Done: update status bar */
	content_set_status(c, "");
	return true;
}


bool nsbmp_redraw(struct content *c, int x, int y,
		int width, int height,
		int clip_x0, int clip_y0, int clip_x1, int clip_y1,
		float scale, colour background_colour)
{

	if (!c->data.bmp.bmp->decoded)
	  	if (bmp_decode(c->data.bmp.bmp) != BMP_OK)
			return false;
	c->bitmap = c->data.bmp.bmp->bitmap;
 	return plot.bitmap(x, y, width, height,	c->bitmap,
 			background_colour, BITMAPF_NONE);
}


bool nsbmp_redraw_tiled(struct content *c, int x, int y,
		int width, int height,
		int clip_x0, int clip_y0, int clip_x1, int clip_y1,
		float scale, colour background_colour,
		bool repeat_x, bool repeat_y)
{
	bitmap_flags_t flags = BITMAPF_NONE;

	if (!c->data.bmp.bmp->decoded)
		if (bmp_decode(c->data.bmp.bmp) != BMP_OK)
			return false;

	c->bitmap = c->data.bmp.bmp->bitmap;

	if (repeat_x)
		flags |= BITMAPF_REPEAT_X;
	if (repeat_y)
		flags |= BITMAPF_REPEAT_Y;

	return plot.bitmap(x, y, width, height, c->bitmap,
			background_colour, flags);
}


void nsbmp_destroy(struct content *c)
{
	bmp_finalise(c->data.bmp.bmp);
	free(c->data.bmp.bmp);
}



bool nsbmp_clone(const struct content *old, struct content *new_content)
{
	/* We "clone" the old content by replaying creation and conversion */
	if (nsbmp_create(new_content, NULL) == false)
		return false;

	if (old->status == CONTENT_STATUS_READY || 
			old->status == CONTENT_STATUS_DONE) {
		if (nsbmp_convert(new_content) == false)
			return false;
	}

	return true;
}


/**
 * Callback for libnsbmp; forwards the call to bitmap_create()
 *
 * \param  width   width of image in pixels
 * \param  height  width of image in pixels
 * \param  state   a flag word indicating the initial state
 * \return an opaque struct bitmap, or NULL on memory exhaustion
 */
void *nsbmp_bitmap_create(int width, int height, unsigned int bmp_state)
{
	unsigned int bitmap_state = BITMAP_NEW;

	/* set bitmap state based on bmp state */
	bitmap_state |= (bmp_state & BMP_OPAQUE) ? BITMAP_OPAQUE : 0;
	bitmap_state |= (bmp_state & BMP_CLEAR_MEMORY) ?
			BITMAP_CLEAR_MEMORY : 0;

	/* return the created bitmap */
	return bitmap_create(width, height, bitmap_state);
}

#endif
