/*
 * Copyright 2005-2007 James Bursa <bursa@users.sourceforge.net>
 * Copyright 2003 Phil Mellor <monkeyson@users.sourceforge.net>
 * Copyright 2005 John M Bell <jmb202@ecs.soton.ac.uk>
 * Copyright 2008 Michael Drake <tlsa@netsurf-browser.org>
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
 * Box tree manipulation (implementation).
 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "content/content_protected.h"
#include "content/hlcache.h"
#include "css/css.h"
#include "css/dump.h"
#include "desktop/scroll.h"
#include "desktop/options.h"
#include "render/box.h"
#include "render/form.h"
#include "render/html.h"
#include "utils/log.h"
#include "utils/talloc.h"
#include "utils/utils.h"

static bool box_contains_point(struct box *box, int x, int y, bool *physically);
static bool box_nearer_text_box(struct box *box, int bx, int by,
		int x, int y, int dir, struct box **nearest, int *tx, int *ty,
		int *nr_xd, int *nr_yd);
static bool box_nearest_text_box(struct box *box, int bx, int by,
		int fx, int fy, int x, int y, int dir, struct box **nearest,
		int *tx, int *ty, int *nr_xd, int *nr_yd);

#define box_is_float(box) (box->type == BOX_FLOAT_LEFT || \
		box->type == BOX_FLOAT_RIGHT)

typedef struct box_duplicate_llist box_duplicate_llist;
struct box_duplicate_llist {
	struct box_duplicate_llist *prev;
	struct box *box;
};
static struct box_duplicate_llist *box_duplicate_last = NULL;

/**
 * Allocator
 *
 * \param ptr   Pointer to reallocate, or NULL for new allocation
 * \param size  Number of bytes requires
 * \param pw    Allocation context
 * \return Pointer to allocated block, or NULL on failure
 */
void *box_style_alloc(void *ptr, size_t len, void *pw)
{
	if (len == 0) {
		free(ptr);
		return NULL;
	}

	return realloc(ptr, len);
}

/**
 * Destructor for box nodes which own styles
 *
 * @param b The box being destroyed.
 * @return 0 to allow talloc to continue destroying the tree.
 */
static int
free_box_style(struct box *b)
{
	if (b->style != NULL) {
		css_computed_style_destroy(b->style);
	}
	
	return 0;
}

/**
 * Create a box tree node.
 *
 * \param  style     style for the box (not copied)
 * \param  href      href for the box (not copied), or 0
 * \param  target    target for the box (not copied), or 0
 * \param  title     title for the box (not copied), or 0
 * \param  id        id for the box (not copied), or 0
 * \param  context   context for allocations
 * \return  allocated and initialised box, or 0 on memory exhaustion
 */

struct box * box_create(css_computed_style *style, bool style_owned,
		char *href, const char *target, char *title, char *id,
		void *context)
{
	unsigned int i;
	struct box *box;

	box = talloc(context, struct box);
	if (!box) {
		return 0;
	}
	
	if (style_owned == true)
		talloc_set_destructor(box, free_box_style);
	
	box->type = BOX_INLINE;
	box->style = style;
	box->x = box->y = 0;
	box->width = UNKNOWN_WIDTH;
	box->height = 0;
	box->descendant_x0 = box->descendant_y0 = 0;
	box->descendant_x1 = box->descendant_y1 = 0;
	for (i = 0; i != 4; i++)
		box->margin[i] = box->padding[i] = box->border[i].width = 0;
	box->scroll_x = box->scroll_y = NULL;
	box->min_width = 0;
	box->max_width = UNKNOWN_MAX_WIDTH;
	box->byte_offset = 0;
	box->text = NULL;
	box->length = 0;
	box->space = 0;
	box->clone = 0;
	box->strip_leading_newline = 0;
	box->href = href;
	box->target = target;
	box->title = title;
	box->columns = 1;
	box->rows = 1;
	box->start_column = 0;
	box->inline_new_line = false;
	box->printed = false;
	box->next = NULL;
	box->prev = NULL;
	box->children = NULL;
	box->last = NULL;
	box->parent = NULL;
	box->fallback = NULL;
	box->inline_end = NULL;
	box->float_children = NULL;
	box->float_container = NULL;
	box->next_float = NULL;
	box->list_marker = NULL;
	box->col = NULL;
	box->gadget = NULL;
	box->usemap = NULL;
	box->id = id;
	box->background = NULL;
	box->object = NULL;
	box->object_params = NULL;

	return box;
}

/**
 * Add a child to a box tree node.
 *
 * \param  parent  box giving birth
 * \param  child   box to link as last child of parent
 */

void box_add_child(struct box *parent, struct box *child)
{
	assert(parent);
	assert(child);

	if (parent->children != 0) {	/* has children already */
		parent->last->next = child;
		child->prev = parent->last;
	} else {			/* this is the first child */
		parent->children = child;
		child->prev = 0;
	}

	parent->last = child;
	child->parent = parent;
}


/**
 * Insert a new box as a sibling to a box in a tree.
 *
 * \param  box      box already in tree
 * \param  new_box  box to link into tree as next sibling
 */

void box_insert_sibling(struct box *box, struct box *new_box)
{
	new_box->parent = box->parent;
	new_box->prev = box;
	new_box->next = box->next;
	box->next = new_box;
	if (new_box->next)
		new_box->next->prev = new_box;
	else if (new_box->parent)
		new_box->parent->last = new_box;
}


/**
 * Unlink a box from the box tree and then free it recursively.
 *
 * \param  box  box to unlink and free recursively.
 */

void box_unlink_and_free(struct box *box)
{
	struct box *parent = box->parent;
	struct box *next = box->next;
	struct box *prev = box->prev;

	if (parent) {
		if (parent->children == box)
			parent->children = next;
		if (parent->last == box)
			parent->last = next ? next : prev;
	}

	if (prev)
		prev->next = next;
	if (next)
		next->prev = prev;

	box_free(box);
}


/**
 * Free a box tree recursively.
 *
 * \param  box  box to free recursively
 *
 * The box and all its children is freed.
 */

void box_free(struct box *box)
{
	struct box *child, *next;

	/* free children first */
	for (child = box->children; child; child = next) {
		next = child->next;
		box_free(child);
	}

	/* last this box */
	box_free_box(box);
}


/**
 * Free the data in a single box structure.
 *
 * \param  box  box to free
 */

void box_free_box(struct box *box)
{
	if (!box->clone) {
		if (box->gadget)
			form_free_control(box->gadget);
		if (box->scroll_x != NULL)
			scroll_destroy(box->scroll_x);
		if (box->scroll_y != NULL)
			scroll_destroy(box->scroll_y);
	}

	talloc_free(box);
}


/**
 * Find the absolute coordinates of a box.
 *
 * \param  box  the box to calculate coordinates of
 * \param  x    updated to x coordinate
 * \param  y    updated to y coordinate
 */

void box_coords(struct box *box, int *x, int *y)
{
	*x = box->x;
	*y = box->y;
	while (box->parent) {
		if (box_is_float(box)) {
			do {
				box = box->parent;
			} while (!box->float_children);
		} else
			box = box->parent;
		*x += box->x - scroll_get_offset(box->scroll_x);
		*y += box->y - scroll_get_offset(box->scroll_y);
	}
}


/**
 * Find the bounds of a box.
 *
 * \param  box  the box to calculate bounds of
 * \param  r    receives bounds
 */

void box_bounds(struct box *box, struct rect *r)
{
	int width, height;

	box_coords(box, &r->x0, &r->y0);

	width = box->padding[LEFT] + box->width + box->padding[RIGHT];
	height = box->padding[TOP] + box->height + box->padding[BOTTOM];

	r->x1 = r->x0 + width;
	r->y1 = r->y0 + height;
}


/**
 * Find the boxes at a point.
 *
 * \param  box      box to search children of
 * \param  x        point to find, in global document coordinates
 * \param  y        point to find, in global document coordinates
 * \param  box_x    position of box, in global document coordinates, updated
 *                  to position of returned box, if any
 * \param  box_y    position of box, in global document coordinates, updated
 *                  to position of returned box, if any
 * \param  content  updated to content of object that returned box is in, if any
 * \return  box at given point, or 0 if none found
 *
 * To find all the boxes in the hierarchy at a certain point, use code like
 * this:
 * \code
 *	struct box *box = top_of_document_to_search;
 *	int box_x = 0, box_y = 0;
 *	struct content *content = document_to_search;
 *
 *	while ((box = box_at_point(box, x, y, &box_x, &box_y, &content))) {
 *		// process box
 *	}
 * \endcode
 */

struct box *box_at_point(struct box *box, const int x, const int y,
		int *box_x, int *box_y,
		hlcache_handle **content)
{
	int bx = *box_x, by = *box_y;
	struct box *child, *sibling;
	bool physically;

	assert(box);

	/* drill into HTML objects */
	if (box->object != NULL) {
		struct box *layout;

		if (content_get_type(box->object) == CONTENT_HTML &&
				(layout = html_get_box_tree(box->object)) != 
				NULL) {
			*content = box->object;
			box = layout;
		} else {
			goto siblings;
		}
	}

	/* consider floats second, since they will often overlap other boxes */
	for (child = box->float_children; child; child = child->next_float) {
		if (box_contains_point(child, x - bx, y - by, &physically)) {
			*box_x = bx + child->x -
					scroll_get_offset(child->scroll_x);
			*box_y = by + child->y -
					scroll_get_offset(child->scroll_y);

			if (physically)
				return child;
			else
				return box_at_point(child, x, y, box_x, box_y,
						content);
		}
	}

non_float_children:
	/* non-float children */
	for (child = box->children; child; child = child->next) {
		if (box_is_float(child))
			continue;
		if (box_contains_point(child, x - bx, y - by, &physically)) {
			*box_x = bx + child->x -
					scroll_get_offset(child->scroll_x);
			*box_y = by + child->y -
					scroll_get_offset(child->scroll_y);

			if (physically)
				return child;
			else
				return box_at_point(child, x, y, box_x, box_y,
						content);
		}
	}

	/* marker boxes */
	if (box->list_marker) {
		if (box_contains_point(box->list_marker, x - bx, y - by,
				&physically)) {
			*box_x = bx + box->list_marker->x;
			*box_y = by + box->list_marker->y;
			return box->list_marker;
		}
	}

siblings:
	/* siblings and siblings of ancestors */
	while (box) {
		if (box_is_float(box)) {
			bx -= box->x - scroll_get_offset(box->scroll_x);
			by -= box->y - scroll_get_offset(box->scroll_y);
			for (sibling = box->next_float; sibling;
					sibling = sibling->next_float) {
				if (box_contains_point(sibling,
						x - bx, y - by, &physically)) {
					*box_x = bx + sibling->x -
							scroll_get_offset(
							sibling->scroll_x);
					*box_y = by + sibling->y -
							scroll_get_offset(
							sibling->scroll_y);

					if (physically)
						return sibling;
					else
						return box_at_point(sibling,
								x, y,
								box_x, box_y,
								content);
				}
			}
			/* ascend to float's parent */
			do {
				box = box->parent;
			} while (!box->float_children);
			/* process non-float children of float's parent */
			goto non_float_children;

		} else {
			bx -= box->x - scroll_get_offset(box->scroll_x);
			by -= box->y - scroll_get_offset(box->scroll_y);
			for (sibling = box->next; sibling;
					sibling = sibling->next) {
				if (box_is_float(sibling))
					continue;
				if (box_contains_point(sibling, x - bx, y - by,
						&physically)) {
					*box_x = bx + sibling->x -
							scroll_get_offset(
							sibling->scroll_x);
					*box_y = by + sibling->y -
							scroll_get_offset(
							sibling->scroll_y);

					if (physically)
						return sibling;
					else
						return box_at_point(sibling,
								x, y,
								box_x, box_y,
								content);
				}
			}
			box = box->parent;
		}
	}

	return 0;
}


/**
 * Determine if a point lies within a box.
 *
 * \param  box		box to consider
 * \param  x		coordinate relative to box parent
 * \param  y		coordinate relative to box parent
 * \param  physically	if function returning true, physically is set true if
 *			point is within the box's physical dimensions and false
 *			if the point is not within the box's physical dimensions
 *			but is in the area defined by the box's descendants.
 *			if function returning false, physically is undefined.
 * \return  true if the point is within the box or a descendant box
 *
 * This is a helper function for box_at_point().
 */

bool box_contains_point(struct box *box, int x, int y, bool *physically)
{
	if (box->x <= x + box->border[LEFT].width &&
			x < box->x + box->padding[LEFT] + box->width +
			box->border[RIGHT].width + box->padding[RIGHT] &&
			box->y <= y + box->border[TOP].width &&
			y < box->y + box->padding[TOP] + box->height +
			box->border[BOTTOM].width + box->padding[BOTTOM]) {
		*physically = true;
		return true;
	}
	if (box->list_marker && box->list_marker->x <= x +
			box->list_marker->border[LEFT].width &&
			x < box->list_marker->x +
			box->list_marker->padding[LEFT] +
			box->list_marker->width +
			box->list_marker->border[RIGHT].width +
			box->list_marker->padding[RIGHT] &&
			box->list_marker->y <= y +
			box->list_marker->border[TOP].width &&
			y < box->list_marker->y +
			box->list_marker->padding[TOP] +
			box->list_marker->height +
			box->list_marker->border[BOTTOM].width +
			box->list_marker->padding[BOTTOM]) {
		*physically = true;
		return true;
	}
	if ((box->style && css_computed_overflow(box->style) == 
			CSS_OVERFLOW_VISIBLE) || !box->style) {
		if (box->x + box->descendant_x0 <= x &&
				x < box->x + box->descendant_x1 &&
				box->y + box->descendant_y0 <= y &&
				y < box->y + box->descendant_y1) {
			*physically = false;
			return true;
		}
	}
	return false;
}


/**
 * Find the box containing an object at the given coordinates, if any.
 *
 * \param  h  content to search, must have type CONTENT_HTML
 * \param  x  coordinates in document units
 * \param  y  coordinates in document units
 */

struct box *box_object_at_point(hlcache_handle *h, int x, int y)
{
	struct content *c = hlcache_handle_get_content(h);
	struct box *box;
	int box_x = 0, box_y = 0;
	hlcache_handle *content = h;
	struct box *object_box = 0;

	assert(c != NULL);
	assert(c->type == CONTENT_HTML);

	box = c->data.html.layout;

	while ((box = box_at_point(box, x, y, &box_x, &box_y, &content))) {
		if (box->style && css_computed_visibility(box->style) ==
				CSS_VISIBILITY_HIDDEN)
			continue;

		if (box->object)
			object_box = box;
	}

	return object_box;
}


/**
 * Find the box containing an href at the given coordinates, if any.
 *
 * \param  h  content to search, must have type CONTENT_HTML
 * \param  x  coordinates in document units
 * \param  y  coordinates in document units
 */

struct box *box_href_at_point(hlcache_handle *h, int x, int y)
{
	struct content *c = hlcache_handle_get_content(h);
	struct box *box;
	int box_x = 0, box_y = 0;
	hlcache_handle *content = h;
	struct box *href_box = 0;

	assert(c != NULL);
	assert(c->type == CONTENT_HTML);

	box = c->data.html.layout;

	while ((box = box_at_point(box, x, y, &box_x, &box_y, &content))) {
		if (box->style && css_computed_visibility(box->style) ==
				CSS_VISIBILITY_HIDDEN)
			continue;

		if (box->href)
			href_box = box;
	}

	return href_box;
}


/**
 * Check whether box is nearer mouse coordinates than current nearest box
 *
 * \param  box      box to test
 * \param  bx	    position of box, in global document coordinates
 * \param  by	    position of box, in global document coordinates
 * \param  x	    mouse point, in global document coordinates
 * \param  y	    mouse point, in global document coordinates
 * \param  dir      direction in which to search (-1 = above-left,
 *						  +1 = below-right)
 * \param  nearest  nearest text box found, or NULL if none
 *		    updated if box is nearer than existing nearest
 * \param  tx	    position of text_box, in global document coordinates
 *		    updated if box is nearer than existing nearest
 * \param  ty	    position of text_box, in global document coordinates
 *		    updated if box is nearer than existing nearest
 * \param  nr_xd    distance to nearest text box found
 *		    updated if box is nearer than existing nearest
 * \param  ny_yd    distance to nearest text box found
 *		    updated if box is nearer than existing nearest
 * \return true if mouse point is inside box
 */

bool box_nearer_text_box(struct box *box, int bx, int by,
		int x, int y, int dir, struct box **nearest, int *tx, int *ty,
		int *nr_xd, int *nr_yd)
{
	int w = box->padding[LEFT] + box->width + box->padding[RIGHT];
	int h = box->padding[TOP] + box->height + box->padding[BOTTOM];
	int y1 = by + h;
	int x1 = bx + w;
	int yd = INT_MAX;
	int xd = INT_MAX;

	if (x >= bx && x1 > x && y >= by && y1 > y) {
		*nearest = box;
		*tx = bx;
		*ty = by;
		return true;
	}

	if (box->parent->list_marker != box) {
		if (dir < 0) {
			/* consider only those children (partly) above-left */
			if (by <= y && bx < x) {
				yd = y <= y1 ? 0 : y - y1;
				xd = x <= x1 ? 0 : x - x1;
			}
		} else {
			/* consider only those children (partly) below-right */
			if (y1 > y && x1 > x) {
				yd = y > by ? 0 : by - y;
				xd = x > bx ? 0 : bx - x;
			}
		}

		/* give y displacement precedence over x */
		if (yd < *nr_yd || (yd == *nr_yd && xd <= *nr_xd)) {
			*nr_yd = yd;
			*nr_xd = xd;
			*nearest = box;
			*tx = bx;
			*ty = by;
		}
	}
	return false;
}


/**
 * Pick the text box child of 'box' that is closest to and above-left
 * (dir -ve) or below-right (dir +ve) of the point 'x,y'
 *
 * \param  box      parent box
 * \param  bx	    position of box, in global document coordinates
 * \param  by	    position of box, in global document coordinates
 * \param  fx	    position of float parent, in global document coordinates
 * \param  fy	    position of float parent, in global document coordinates
 * \param  x	    mouse point, in global document coordinates
 * \param  y	    mouse point, in global document coordinates
 * \param  dir      direction in which to search (-1 = above-left,
 *						  +1 = below-right)
 * \param  nearest  nearest text box found, or NULL if none
 *		    updated if a descendant of box is nearer than old nearest
 * \param  tx	    position of nearest, in global document coordinates
 *		    updated if a descendant of box is nearer than old nearest
 * \param  ty	    position of nearest, in global document coordinates
 *		    updated if a descendant of box is nearer than old nearest
 * \param  nr_xd    distance to nearest text box found
 *		    updated if a descendant of box is nearer than old nearest
 * \param  ny_yd    distance to nearest text box found
 *		    updated if a descendant of box is nearer than old nearest
 * \return true if mouse point is inside text_box
 */

bool box_nearest_text_box(struct box *box, int bx, int by,
		int fx, int fy, int x, int y, int dir, struct box **nearest,
		int *tx, int *ty, int *nr_xd, int *nr_yd)
{
	struct box *child = box->children;
	int c_bx, c_by;
	int c_fx, c_fy;
	bool in_box = false;

	if (*nearest == NULL) {
		*nr_xd = INT_MAX / 2; /* displacement of 'nearest so far' */
		*nr_yd = INT_MAX / 2;
	}
	if (box->type == BOX_INLINE_CONTAINER) {
		int bw = box->padding[LEFT] + box->width + box->padding[RIGHT];
		int bh = box->padding[TOP] + box->height + box->padding[BOTTOM];
		int b_y1 = by + bh;
		int b_x1 = bx + bw;
		if (x >= bx && b_x1 > x && y >= by && b_y1 > y) {
			in_box = true;
		}
	}

	while (child) {
		if (child->type == BOX_FLOAT_LEFT ||
				child->type == BOX_FLOAT_RIGHT) {
			c_bx = fx + child->x -
					scroll_get_offset(child->scroll_x);
			c_by = fy + child->y -
					scroll_get_offset(child->scroll_y);
		} else {
			c_bx = bx + child->x -
					scroll_get_offset(child->scroll_x);
			c_by = by + child->y -
					scroll_get_offset(child->scroll_y);
		}
		if (child->float_children) {
			c_fx = c_bx;
			c_fy = c_by;
		} else {
			c_fx = fx;
			c_fy = fy;
		}
		if (in_box && child->text && !child->object) {
			if (box_nearer_text_box(child,
					c_bx, c_by, x, y, dir, nearest,
					tx, ty, nr_xd, nr_yd))
				return true;
		} else {
			if (child->list_marker) {
				if (box_nearer_text_box(
						child->list_marker,
						c_bx + child->list_marker->x,
						c_by + child->list_marker->y,
						x, y, dir, nearest,
						tx, ty, nr_xd, nr_yd))
					return true;
			}
			if (box_nearest_text_box(child, c_bx, c_by,
					c_fx, c_fy, x, y, dir, nearest, tx, ty,
					nr_xd, nr_yd))
				return true;
		}
		child = child->next;
	}

	return false;
}


/**
 * Peform pick text on browser window contents to locate the box under
 * the mouse pointer, or nearest in the given direction if the pointer is
 * not over a text box.
 *
 * \param h	html content's high level cache handle
 * \param x	coordinate of mouse
 * \param y	coordinate of mouse
 * \param dir	direction to search (-1 = above-left, +1 = below-right)
 * \param dx	receives x ordinate of mouse relative to text box
 * \param dy	receives y ordinate of mouse relative to text box
 */

struct box *box_pick_text_box(hlcache_handle *h,
		int x, int y, int dir, int *dx, int *dy)
{
	struct box *text_box = NULL;

	if (h && content_get_type(h) == CONTENT_HTML) {
		struct box *box = html_get_box_tree(h);
		int nr_xd, nr_yd;
		int bx = box->margin[LEFT];
		int by = box->margin[TOP];
		int fx = bx;
		int fy = by;
		int tx, ty;

		if (!box_nearest_text_box(box, bx, by, fx, fy, x, y,
				dir, &text_box, &tx, &ty, &nr_xd, &nr_yd)) {
			if (text_box && text_box->text && !text_box->object) {
				int w = (text_box->padding[LEFT] +
						text_box->width +
						text_box->padding[RIGHT]);
				int h = (text_box->padding[TOP] +
						text_box->height +
						text_box->padding[BOTTOM]);
				int x1, y1;

				y1 = ty + h;
				x1 = tx + w;

				/* ensure point lies within the text box */
				if (x < tx) x = tx;
				if (y < ty) y = ty;
				if (y > y1) y = y1;
				if (x > x1) x = x1;
			}
		}

		/* return coordinates relative to box */
		*dx = x - tx;
		*dy = y - ty;
	}

	return text_box;
}


/**
 * Find a box based upon its id attribute.
 *
 * \param  box  box tree to search
 * \param  id   id to look for
 * \return  the box or 0 if not found
 */

struct box *box_find_by_id(struct box *box, const char *id)
{
	struct box *a, *b;

	if (box->id != NULL && strcmp(id, box->id) == 0)
		return box;

	for (a = box->children; a; a = a->next) {
		if ((b = box_find_by_id(a, id)) != NULL)
			return b;
	}

	return NULL;
}


/**
 * Determine if a box is visible when the tree is rendered.
 *
 * \param  box  box to check
 * \return  true iff the box is rendered
 */

bool box_visible(struct box *box)
{
	struct box *fallback;

	/* visibility: hidden */
	if (box->style && css_computed_visibility(box->style) == 
			CSS_VISIBILITY_HIDDEN)
		return false;

	/* check if a fallback */
	while (box->parent) {
		for (fallback = box->parent->fallback; fallback;
				fallback = fallback->next)
			if (fallback == box)
				return false;
		box = box->parent;
	}

	return true;
}


/**
 * Print a box tree to a file.
 */

void box_dump(FILE *stream, struct box *box, unsigned int depth)
{
	unsigned int i;
	struct box *c, *prev;

	for (i = 0; i != depth; i++)
		fprintf(stream, "  ");

	fprintf(stream, "%p ", box);
	fprintf(stream, "x%i y%i w%i h%i ", box->x, box->y,
			box->width, box->height);
	if (box->max_width != UNKNOWN_MAX_WIDTH)
		fprintf(stream, "min%i max%i ", box->min_width, box->max_width);
	fprintf(stream, "(%i %i %i %i) ",
			box->descendant_x0, box->descendant_y0,
			box->descendant_x1, box->descendant_y1);

	fprintf(stream, "m(%i %i %i %i) ",
			box->margin[TOP], box->margin[LEFT],
			box->margin[BOTTOM], box->margin[RIGHT]);

	switch (box->type) {
	case BOX_BLOCK:            fprintf(stream, "BLOCK "); break;
	case BOX_INLINE_CONTAINER: fprintf(stream, "INLINE_CONTAINER "); break;
	case BOX_INLINE:           fprintf(stream, "INLINE "); break;
	case BOX_INLINE_END:       fprintf(stream, "INLINE_END "); break;
	case BOX_INLINE_BLOCK:     fprintf(stream, "INLINE_BLOCK "); break;
	case BOX_TABLE:            fprintf(stream, "TABLE [columns %i] ",
					   box->columns); break;
	case BOX_TABLE_ROW:        fprintf(stream, "TABLE_ROW "); break;
	case BOX_TABLE_CELL:       fprintf(stream, "TABLE_CELL [columns %i, "
					   "start %i, rows %i] ", box->columns,
					   box->start_column, box->rows); break;
	case BOX_TABLE_ROW_GROUP:  fprintf(stream, "TABLE_ROW_GROUP "); break;
	case BOX_FLOAT_LEFT:       fprintf(stream, "FLOAT_LEFT "); break;
	case BOX_FLOAT_RIGHT:      fprintf(stream, "FLOAT_RIGHT "); break;
	case BOX_BR:               fprintf(stream, "BR "); break;
	case BOX_TEXT:             fprintf(stream, "TEXT "); break;
	default:                   fprintf(stream, "Unknown box type ");
	}

	if (box->text)
		fprintf(stream, "%li '%.*s' ", (unsigned long) box->byte_offset,
				(int) box->length, box->text);
	if (box->space)
		fprintf(stream, "space ");
	if (box->object) {
		fprintf(stream, "(object '%s') ", 
				content_get_url(box->object));
	}
	if (box->gadget)
		fprintf(stream, "(gadget) ");
	if (box->style)
		nscss_dump_computed_style(stream, box->style);
	if (box->href)
		fprintf(stream, " -> '%s'", box->href);
	if (box->target)
		fprintf(stream, " |%s|", box->target);
	if (box->title)
		fprintf(stream, " [%s]", box->title);
	if (box->id)
		fprintf(stream, " <%s>", box->id);
	if (box->type == BOX_INLINE || box->type == BOX_INLINE_END)
		fprintf(stream, " inline_end %p", box->inline_end);
	if (box->float_children)
		fprintf(stream, " float_children %p", box->float_children);
	if (box->next_float)
		fprintf(stream, " next_float %p", box->next_float);
	if (box->col) {
		static const char *boxType[] = {"UNKNOWN", "FIXED", "AUTO", "PERCENT", "RELATIVE"};
		static const char *boxPos[] = {"normal", "positioned"};

		fprintf(stream, " (columns");
		for (i = 0; i != box->columns; i++)
			fprintf(stream, " (%s %s %i %i %i)", 
					boxType[box->col[i].type],
					boxPos[box->col[i].positioned],
					box->col[i].width,
					box->col[i].min, box->col[i].max);
		fprintf(stream, ")");
	}
	fprintf(stream, "\n");

	if (box->list_marker) {
		for (i = 0; i != depth; i++)
			fprintf(stream, "  ");
		fprintf(stream, "list_marker:\n");
		box_dump(stream, box->list_marker, depth + 1);
	}

	for (c = box->children; c && c->next; c = c->next)
		;
	if (box->last != c)
		fprintf(stream, "warning: box->last %p (should be %p) "
				"(box %p)\n", box->last, c, box);
	for (prev = 0, c = box->children; c; prev = c, c = c->next) {
		if (c->parent != box)
			fprintf(stream, "warning: box->parent %p (should be "
					"%p) (box on next line)\n",
					c->parent, box);
		if (c->prev != prev)
			fprintf(stream, "warning: box->prev %p (should be "
					"%p) (box on next line)\n",
					c->prev, prev);
		box_dump(stream, c, depth + 1);
	}
	if (box->fallback) {
		for (i = 0; i != depth; i++)
			fprintf(stream, "  ");
		fprintf(stream, "fallback:\n");
		for (c = box->fallback; c; c = c->next)
			box_dump(stream, c, depth + 1);
	}
}

/* Box tree duplication below
*/

/** structure for translating addresses in the box tree */
struct box_dict_element{
	struct box *old, *new;
};

static bool box_duplicate_main_tree(struct box *box, struct content *c,
		int *count);
static void box_duplicate_create_dict(struct box *old_box, struct box *new_box,
		struct box_dict_element **dict);
static void box_duplicate_update( struct box *box,
		struct box_dict_element *dict,
		int n);

static int box_compare_dict_elements(const struct box_dict_element *a,
		const struct box_dict_element *b);

int box_compare_dict_elements(const struct box_dict_element *a,
		const struct box_dict_element *b)
{
	return (a->old < b->old) ? -1 : ((a->old > b->old) ? 1 : 0);
}

/** Duplication of a box tree. We assume that all the content is fetched,
fallbacks have been applied where necessary and we reuse a lot of content
like strings, fetched objects etc - just replicating all we need to create
two different layouts.
\return true on success, false on lack of memory
*/
struct box* box_duplicate_tree(struct box *root, struct content *c)
{
	struct box *new_root;/**< Root of the new box tree*/
	int box_number = 0;
	struct box_dict_element *box_dict, *box_dict_end;

	box_duplicate_last = NULL;

	/* 1. Duplicate parent - children structure, list_markers*/
	new_root = talloc_memdup(c, root, sizeof (struct box));
	if (!box_duplicate_main_tree(new_root, c, &box_number))
		return NULL;

	/* 2. Create address translation dictionary*/
	/*TODO: dont save unnecessary addresses*/

	box_dict_end = box_dict = malloc(box_number *
			sizeof(struct box_dict_element));

	if (box_dict == NULL)
		return NULL;
	box_duplicate_create_dict(root, new_root, &box_dict_end);

	assert((box_dict_end - box_dict) == box_number);

	/*3. Sort it*/

	qsort(box_dict, (box_dict_end - box_dict), sizeof(struct box_dict_element),
	      (int (*)(const void *, const void *))box_compare_dict_elements);

	/* 4. Update inline_end and float_children pointers */

	box_duplicate_update(new_root, box_dict, (box_dict_end - box_dict));

	free(box_dict);

	return new_root;
}

/**
 * Recursively duplicates children of an element, and also if present - its
 * list_marker, style and text.
 * \param box Current box to duplicate its children
 * \param c talloc memory pool
 * \param count number of boxes seen so far
 * \return true if successful, false otherwise (lack of memory)
*/
bool box_duplicate_main_tree(struct box *box, struct content *c, int *count)
{
	struct box *b, *prev;

	prev = NULL;

	for (b = box->children; b; b = b->next) {
		struct box *copy;

		/*Copy child*/
		copy = talloc_memdup(c, b, sizeof (struct box));
		if (copy == NULL)
			return false;

		copy->parent = box;

		if (prev != NULL)
			prev->next = copy;
		else
			box->children = copy;

		if (copy->type == BOX_INLINE) {
			struct box_duplicate_llist *temp;

			temp = malloc(sizeof(struct box_duplicate_llist));
			if (temp == NULL)
				return false;
			temp->prev = box_duplicate_last;
			temp->box = copy;
			box_duplicate_last = temp;
		}
		else if (copy->type == BOX_INLINE_END) {
			struct box_duplicate_llist *temp;

			box_duplicate_last->box->inline_end = copy;
			copy->inline_end = box_duplicate_last->box;

			temp = box_duplicate_last;
			box_duplicate_last = temp->prev;
			free(temp);
		}

		/* Recursively visit child */
		if (!box_duplicate_main_tree(copy, c, count))
			return false;

		prev = copy;
	}

	box->last = prev;

	if (box->object != NULL && option_suppress_images && (
#ifdef WITH_JPEG
			content_get_type(box->object) == CONTENT_JPEG ||
#endif
#ifdef WITH_GIF
			content_get_type(box->object) == CONTENT_GIF ||
#endif
#ifdef WITH_BMP
			content_get_type(box->object) ==  CONTENT_BMP ||
			content_get_type(box->object) == CONTENT_ICO ||
#endif
#if defined(WITH_MNG) || defined(WITH_PNG)
			content_get_type(box->object) == CONTENT_PNG ||
#endif
#ifdef WITH_MNG
			content_get_type(box->object) == CONTENT_JNG ||
			content_get_type(box->object) == CONTENT_MNG ||
#endif
#if defined(WITH_SPRITE) || defined(WITH_NSSPRITE)
			content_get_type(box->object) == CONTENT_SPRITE ||
#endif
#ifdef WITH_DRAW
			content_get_type(box->object) == CONTENT_DRAW ||
#endif
#ifdef WITH_PLUGIN
			content_get_type(box->object) == CONTENT_PLUGIN ||
#endif
#ifdef WITH_THEME_INSTALL
			content_get_type(box->object) == CONTENT_THEME ||
#endif
#ifdef WITH_ARTWORKS
			content_get_type(box->object) == CONTENT_ARTWORKS ||
#endif
#if defined(WITH_NS_SVG) || defined(WITH_RSVG)
			content_get_type(box->object) == CONTENT_SVG ||
#endif
			false))
		box->object = NULL;

	if (box->list_marker) {
		box->list_marker = talloc_memdup(c, box->list_marker,
				sizeof *box->list_marker);
		if (box->list_marker == NULL)
			return false;
		box->list_marker->parent = box;
	}

	if (box->text) {
		box->text = talloc_memdup(c, box->text, box->length);
		if (box->text == NULL)
			return false;
	}

	if (box->style) {
		box->style = talloc_memdup(c, box->style, sizeof *box->style);
		if (box->style == NULL)
			return false;
	}

	/*Make layout calculate the size of this element later
	(might change because of font change etc.) */
	box->width = UNKNOWN_WIDTH;
	box->min_width = 0;
	box->max_width = UNKNOWN_MAX_WIDTH;

	(*count)++;

	return true;
}

/**
 * Recursively creates a dictionary of addresses - binding the address of a box
 * with its copy.
 * \param old_box original box
 * \param new_box copy of the original box
 * \param dict pointer to a pointer to the current position in the dictionary
 */
void box_duplicate_create_dict(struct box *old_box, struct box *new_box,
		struct box_dict_element **dict)
{
	/**Children of the old and new boxes*/
	struct box *b_old, *b_new;

	for (b_old = old_box->children, b_new = new_box->children;
	     b_old != NULL && b_new != NULL;
	     b_old = b_old->next, b_new = b_new->next)
		box_duplicate_create_dict(b_old, b_new, dict);

	/*The new tree should be a exact copy*/
	assert(b_old == NULL && b_new == NULL);

	(*dict)->old = old_box;
	(*dict)->new = new_box;
	(*dict)++;
}

/**
 * Recursively updates pointers in box tree.
 * \param box current box in the new box tree
 * \param box_dict box pointers dictionary
 * \param n number of memory addresses in the dictionary
 */
void box_duplicate_update(struct box *box,
		struct box_dict_element *box_dict,
  		int n)
{
	struct box_dict_element *box_dict_element;
	struct box *b;
	struct box_dict_element element;

	for (b = box->children; b; b = b->next)
		box_duplicate_update(b, box_dict, n);

	if (box->float_children) {
		element.old = box->float_children;
		box_dict_element = bsearch(&element,
				box_dict, n,
				sizeof(struct box_dict_element),
				(int (*)(const void *, const void *))box_compare_dict_elements);
		box->float_children = box_dict_element->new;
	}

	if (box->next_float) {
		element.old = box->next_float;
		box_dict_element = bsearch(&element,
				box_dict, n,
				sizeof(struct box_dict_element),
				(int (*)(const void *, const void *))box_compare_dict_elements);
		box->next_float = box_dict_element->new;
	}
}

/**
 * Applies the given scroll setup to a box. This includes scroll
 * creation/deletion as well as scroll dimension updates.
 *
 * \param bw		browser window in which the box is located
 * \param box		the box to handle the scrolls for
 * \param bottom	whether the horizontal scrollbar should be present
 * \param right		whether the vertical scrollbar should be present
 * \return		true on success false otherwise
 */
bool box_handle_scrollbars(struct browser_window *bw, struct box *box,
		bool bottom, bool right)
{
	struct browser_scroll_data *data;
	int padding_width, padding_height;
	
	padding_width = box->width + box->padding[RIGHT] + box->padding[LEFT];
	padding_height = box->height + box->padding[TOP] + box->padding[BOTTOM];
	
	if (!bottom && box->scroll_x != NULL) {
		data = scroll_get_data(box->scroll_x);
		scroll_destroy(box->scroll_x);
		free(data);
		box->scroll_x = NULL;
	}
	
	if (!right && box->scroll_y != NULL) {
		data = scroll_get_data(box->scroll_y);
		scroll_destroy(box->scroll_y);
		free(data);
		box->scroll_y = NULL;
	}
	
	if (!bottom && !right)
		return true;
	
	if (right) {
		if (box->scroll_y == NULL) {
			data = malloc(sizeof(struct browser_scroll_data));
			if (data == NULL) {
				LOG(("malloc failed"));
				warn_user("NoMemory", 0);
				return false;
			}
			data->bw = bw;
			data->box = box;
			if (!scroll_create(false,
					padding_height,
					box->descendant_y1 - box->descendant_y0,
     					box->height,
					data,
     					html_overflow_scroll_callback,
					&(box->scroll_y)))
				return false;
		} else 
			scroll_set_extents(box->scroll_y,
					padding_height, box->height,
     					box->descendant_y1 -
					box->descendant_y0);
	}
	if (bottom) {
		if (box->scroll_x == NULL) {
			data = malloc(sizeof(struct browser_scroll_data));
			if (data == NULL) {
				LOG(("malloc failed"));
				warn_user("NoMemory", 0);
				return false;
			}
			data->bw = bw;
			data->box = box;
			if (!scroll_create(true,
					padding_width -
					(right ? SCROLLBAR_WIDTH : 0),
					box->descendant_x1 - box->descendant_x0,
     					box->width,
					data,
     					html_overflow_scroll_callback,
					&box->scroll_x))
				return false;
		} else
			scroll_set_extents(box->scroll_x,
					padding_width -
					(right ? SCROLLBAR_WIDTH : 0),
					box->width,
     					box->descendant_x1 -
					box->descendant_x0);
	}
	
	if (right && bottom)
		scroll_make_pair(box->scroll_x, box->scroll_y);
	
	return true;
}

/**
 * Determine if a box has a vertical scrollbar.
 *
 * \param  box  scrolling box
 * \return the box has a vertical scrollbar
 */

bool box_vscrollbar_present(const struct box * const box)
{
	return box->descendant_y0 < -box->border[TOP].width ||
			box->padding[TOP] + box->height + box->padding[BOTTOM] +
			box->border[BOTTOM].width < box->descendant_y1;
}


/**
 * Determine if a box has a horizontal scrollbar.
 *
 * \param  box  scrolling box
 * \return the box has a horizontal scrollbar
 */

bool box_hscrollbar_present(const struct box * const box)
{
	return box->descendant_x0 < -box->border[LEFT].width ||
			box->padding[LEFT] + box->width + box->padding[RIGHT] +
			box->border[RIGHT].width < box->descendant_x1;
}
