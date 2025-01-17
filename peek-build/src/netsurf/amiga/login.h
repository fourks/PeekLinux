/*
 * Copyright 2008 Chris Young <chris@unsatisfactorysoftware.co.uk>
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

#ifndef AMIGA_LOGIN_H
#define AMIGA_LOGIN_H

#include <stdbool.h>

#include "utils/errors.h"

struct gui_login_window {
	struct nsObject *node;
	struct Window *win;
	Object *objects[GID_LAST];
	nserror (*cb)(bool proceed, void *pw);
	void *cbpw;
	char *url;
	char *realm;
	char *host;
};

void ami_401login_close(struct gui_login_window *lw);
void ami_401login_login(struct gui_login_window *lw);
BOOL ami_401login_event(struct gui_login_window *lw);
#endif
