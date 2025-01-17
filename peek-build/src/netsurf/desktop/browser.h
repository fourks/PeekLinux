/*
 * Copyright 2003 Phil Mellor <monkeyson@users.sourceforge.net>
 * Copyright 2006 James Bursa <bursa@users.sourceforge.net>
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
 * Browser window creation and manipulation (interface).
 */

#ifndef _NETSURF_DESKTOP_BROWSER_H_
#define _NETSURF_DESKTOP_BROWSER_H_

#include <inttypes.h>
#include <stdbool.h>
#include <time.h>

#include "desktop/gui.h"
#include "desktop/mouse.h"
#include "render/html.h"

struct box;
struct hlcache_handle;
struct form;
struct form_control;
struct gui_window;
struct history;
struct selection;
struct browser_window;
struct url_data;
struct bitmap;
struct scroll_msg_data;
struct fetch_multipart_data;

typedef bool (*browser_caret_callback)(struct browser_window *bw,
		uint32_t key, void *p);
typedef bool (*browser_paste_callback)(struct browser_window *bw,
		const char *utf8, unsigned utf8_len, bool last, void *p);
typedef void (*browser_move_callback)(struct browser_window *bw,
		void *p);



/** Browser window data. */
struct browser_window {
	/** Page currently displayed, or 0. Must have status READY or DONE. */
	struct hlcache_handle *current_content;
	/** Page being loaded, or 0. */
	struct hlcache_handle *loading_content;

	/** Window history structure. */
	struct history *history;

	/** Selection state */
	struct selection *sel;

	/** Handler for keyboard input, or 0. */
	browser_caret_callback caret_callback;
	/** Handler for pasting text, or 0. */
	browser_paste_callback paste_callback;
	/** Handler for repositioning caret, or 0. */
	browser_move_callback move_callback;

	/** User parameter for caret_callback and paste_callback */
	void *caret_p;

	/** Platform specific window data. */
	struct gui_window *window;

	/** Busy indicator is active. */
	bool throbbing;
	/** Add loading_content to the window history when it loads. */
	bool history_add;

	/** Fragment identifier for current_content. */
	char *frag_id;

	/** Current drag status. */
	enum {
		DRAGGING_NONE,
		DRAGGING_SELECTION,
		DRAGGING_PAGE_SCROLL,
		DRAGGING_FRAME
	} drag_type;

	/** Mouse position at start of current scroll drag. */
	int drag_start_x;
	int drag_start_y;
	/** Scroll offsets at start of current scroll draw. */
	int drag_start_scroll_x;
	int drag_start_scroll_y;
	/** Frame resize directions for current frame resize drag. */
	unsigned int drag_resize_left : 1;
	unsigned int drag_resize_right : 1;
	unsigned int drag_resize_up : 1;
	unsigned int drag_resize_down : 1;

	/** Scroll capturing all mouse events */
	struct scroll *scroll;

	/** Current fetch is download */
	bool download;

	/** Refresh interval (-1 if undefined) */
	int refresh_interval;

	/** Window has been resized, and content needs reformatting. */
	bool reformat_pending;

	/** Window dimensions */
	int x0;
	int y0;
	int x1;
	int y1;

	/** scale of window contents */
	float scale;

	/** Window characteristics */
	enum {
		BROWSER_WINDOW_NORMAL,
		BROWSER_WINDOW_IFRAME,
		BROWSER_WINDOW_FRAME,
		BROWSER_WINDOW_FRAMESET
	} browser_window_type;

	/** frameset characteristics */
	int rows;
	int cols;

	/** frame dimensions */
	struct frame_dimension frame_width;
	struct frame_dimension frame_height;
	int margin_width;
	int margin_height;

	/** frame name for targetting */
	char *name;

	/** frame characteristics */
	bool no_resize;
	frame_scrolling scrolling;
	bool border;
	colour border_colour;

	/** iframe parent box */
	struct box *box;

	/** [cols * rows] children */
	struct browser_window *children;
	struct browser_window *parent;

	/** [iframe_count] iframes */
	int iframe_count;
	struct browser_window *iframes;

	/** Last time a link was followed in this window */
	unsigned int last_action;

	/** search context for free text search */
	struct search_context *search_context;

	struct form_control *visible_select_menu;

	/** cache of the currently displayed status text. */
	char *status_text; /**< Current status bar text. */
	int status_text_len; /**< Length of the ::status_text buffer. */
	int status_match; /**< Number of times an idempotent status-set operation was performed. */
	int status_miss; /**< Number of times status was really updated. */
};

struct browser_scroll_data {
	struct browser_window *bw;
	struct box *box;
};

extern struct browser_window *current_redraw_browser;
extern bool browser_reformat_pending;

struct browser_window * browser_window_create(const char *url,
		struct browser_window *clone, const char *referrer,
		bool history_add, bool new_tab);
void browser_window_initialise_common(struct browser_window *bw,
		struct browser_window *clone);
void browser_window_go(struct browser_window *bw, const char *url,
		const char *referrer, bool history_add);
void browser_window_go_post(struct browser_window *bw,
		const char *url, char *post_urlenc,
		struct fetch_multipart_data *post_multipart,
		bool add_to_history, const char *referer, bool download,
		bool verifiable, struct hlcache_handle *parent);
void browser_window_go_unverifiable(struct browser_window *bw,
		const char *url, const char *referrer, bool history_add,
		struct hlcache_handle *parent);
void browser_window_download(struct browser_window *bw,
		const char *url, const char *referrer);
void browser_window_update(struct browser_window *bw, bool scroll_to_top);
void browser_window_stop(struct browser_window *bw);
void browser_window_reload(struct browser_window *bw, bool all);
void browser_window_destroy(struct browser_window *bw);
struct browser_window * browser_window_owner(struct browser_window *bw);
void browser_window_reformat(struct browser_window *bw, int width, int height);
void browser_window_set_scale(struct browser_window *bw, float scale, bool all);

void browser_window_refresh_url_bar(struct browser_window *bw, const char *url,
		const char *frag);

void browser_window_mouse_click(struct browser_window *bw,
		browser_mouse_state mouse, int x, int y);
void browser_window_mouse_track(struct browser_window *bw,
		browser_mouse_state mouse, int x, int y);
void browser_window_mouse_drag_end(struct browser_window *bw,
		browser_mouse_state mouse, int x, int y);
struct browser_window *browser_window_find_target(
		struct browser_window *bw, const char *target,
		browser_mouse_state mouse);

bool browser_window_key_press(struct browser_window *bw, uint32_t key);
bool browser_window_paste_text(struct browser_window *bw, const char *utf8,
		unsigned utf8_len, bool last);
void browser_redraw_box(struct hlcache_handle *c, struct box *box);

void browser_scroll_callback(void *client_data,
		struct scroll_msg_data *scroll_data);
void browser_select_menu_callback(void *client_data,
		int x, int y, int width, int height);

void browser_window_redraw_rect(struct browser_window *bw, int x, int y,
		int width, int height);

void browser_window_set_status(struct browser_window *bw, const char *text);
void browser_window_set_pointer(struct gui_window *g, gui_pointer_shape shape);
void browser_window_page_drag_start(struct browser_window *bw, int x, int y);

bool browser_window_back_available(struct browser_window *bw);
bool browser_window_forward_available(struct browser_window *bw);
bool browser_window_reload_available(struct browser_window *bw);
bool browser_window_stop_available(struct browser_window *bw);

/* In platform specific hotlist.c. */
void hotlist_visited(struct hlcache_handle *c);

/* In platform specific global_history.c. */
void global_history_add(const char *url);
void global_history_add_recent(const char *url);
char **global_history_get_recent(int *count);

/* In platform specific thumbnail.c. */
bool thumbnail_create(struct hlcache_handle *content, struct bitmap *bitmap,
		const char *url);

/* In platform specific schedule.c. */
void schedule(int t, void (*callback)(void *p), void *p);
void schedule_remove(void (*callback)(void *p), void *p);

/* In platform specific theme_install.c. */
#ifdef WITH_THEME_INSTALL
void theme_install_start(struct hlcache_handle *c);
#endif

#endif
