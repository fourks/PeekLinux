/*
 * Copyright 2008 François Revol <mmu_man@users.sourceforge.net>
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
 * Page thumbnail creation (implementation).
 *
 * Thumbnails are created by setting the current drawing contexts to a BView
 * attached to the BBitmap we are passed, and plotting the page at a small
 * scale.
 */

#define __STDBOOL_H__	1
#include <assert.h>
#include <Bitmap.h>
#include <View.h>
extern "C" {
#include "content/content.h"
#include "content/urldb.h"
#include "desktop/plotters.h"
#include "desktop/browser.h"
#include "image/bitmap.h"
#include "render/font.h"
#include "utils/log.h"
}
#include "beos/beos_scaffolding.h"
#include "beos/beos_plotters.h"
#include "beos/beos_bitmap.h"

// Zeta PRIVATE: in libzeta for now.
extern status_t ScaleBitmap(const BBitmap& inBitmap, BBitmap& outBitmap);

#warning XXX do we need to set bitmap:shadow ?


/**
 * Create a thumbnail of a page.
 *
 * \param  content  content structure to thumbnail
 * \param  bitmap   the bitmap to draw to
 * \param  url      the URL the thumnail belongs to, or NULL
 */
bool thumbnail_create(hlcache_handle *content, struct bitmap *bitmap,
		const char *url)
{
	BBitmap *thumbnail;
	BBitmap *small;
	BBitmap *big;
	BView *oldView;
	BView *view;
	BView *thumbView;
	float width;
	float height;
	int depth;

	assert(content);
	assert(bitmap);

	thumbnail = nsbeos_bitmap_get_primary(bitmap);
	width = thumbnail->Bounds().Width();
	height = thumbnail->Bounds().Height();
	depth = 32;
	//depth = (gdk_screen_get_system_visual(gdk_screen_get_default()))->depth;

	LOG(("Trying to create a thumbnail bitmap %d x %d for a content of %d x %d @ %d",
		width, height,
		content_get_width(content), content_get_width(content), depth));

	BRect contentRect(0, 0,
		content_get_width(content) - 1,
		content_get_width(content) - 1);
	big = new BBitmap(contentRect,
		B_BITMAP_ACCEPTS_VIEWS, B_RGB32);

	if (big->InitCheck() < B_OK) {
		delete big;
		return false;
	}

	small = new BBitmap(thumbnail->Bounds(), 
		B_BITMAP_ACCEPTS_VIEWS, B_RGB32);

	if (small->InitCheck() < B_OK) {
		delete small;
		delete big;
		return false;
	}

	//XXX: _lock ?
	// backup the current gc
	oldView = nsbeos_current_gc();

	view = new BView(contentRect, "thumbnailer", 
		B_FOLLOW_NONE, B_WILL_DRAW);
	big->AddChild(view);

	thumbView = new BView(small->Bounds(), "thumbnail", 
		B_FOLLOW_NONE, B_WILL_DRAW);
	small->AddChild(thumbView);

	view->LockLooper();

	/* impose our view on the content... */
	nsbeos_current_gc_set(view);

	plot = nsbeos_plotters;
	nsbeos_plot_set_scale(1.0);

	plot.rectangle(0, 0,
		content_get_width(content),
		content_get_width(content),
		plot_style_fill_white);

	/* render the content */
	content_redraw(content, 0, 0,
			content_get_width(content), content_get_width(content),
			0, 0,
			content_get_width(content), content_get_width(content),
			1.0, 0xFFFFFF);

	view->Sync();
	view->UnlockLooper();

	// restore the current gc
	nsbeos_current_gc_set(oldView);


	// now scale it down
//XXX: use Zeta's bilinear scaler ?
//#ifdef B_ZETA_VERSION
//	err = ScaleBitmap(*shot, *scaledBmp);
//#else
	thumbView->LockLooper();
	thumbView->DrawBitmap(big, big->Bounds(), small->Bounds());
	thumbView->Sync();
	thumbView->UnlockLooper();

	small->LockBits();
	thumbnail->LockBits();

	// copy it to the bitmap
	memcpy(thumbnail->Bits(), small->Bits(), thumbnail->BitsLength());

	thumbnail->UnlockBits();
	small->UnlockBits();

	/* register the thumbnail with the URL */
	if (url)
	  urldb_set_thumbnail(url, bitmap);

	bitmap_modified(bitmap);

	// cleanup
	small->RemoveChild(thumbView);
	delete thumbView;
	delete small;
	big->RemoveChild(view);
	delete view;
	delete big;

	return true;
}
