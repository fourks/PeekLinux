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

#include "desktop/401login.h"
#include "amiga/gui.h"
#include "amiga/object.h"
#include "amiga/login.h"
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include "utils/url.h"
#include "content/urldb.h"
#include "utils/messages.h"

#include <proto/window.h>
#include <proto/layout.h>
#include <proto/string.h>
#include <proto/button.h>
#include <proto/label.h>
#include <classes/window.h>
#include <gadgets/layout.h>
#include <gadgets/string.h>
#include <gadgets/button.h>
#include <images/label.h>
#include <reaction/reaction_macros.h>

void gui_401login_open(const char *url, const char *realm,
		nserror (*cb)(bool proceed, void *pw), void *cbpw)
{
	struct gui_login_window *lw = AllocVec(sizeof(struct gui_login_window),MEMF_PRIVATE | MEMF_CLEAR);
	char *host;

	url_host(url, &host);
	lw->host = host;
	lw->url = url;
	lw->realm = (char *)realm;
	lw->cb = cb;
	lw->cbpw = cbpw;

	lw->objects[OID_MAIN] = WindowObject,
      	    WA_ScreenTitle,nsscreentitle,
           	WA_Title,lw->url,
           	WA_Activate, TRUE,
           	WA_DepthGadget, TRUE,
           	WA_DragBar, TRUE,
           	WA_CloseGadget, FALSE,
           	WA_SizeGadget, TRUE,
			WA_CustomScreen,scrn,
			WINDOW_SharedPort,sport,
			WINDOW_UserData,lw,
			WINDOW_IconifyGadget, FALSE,
			WINDOW_LockHeight,TRUE,
         	WINDOW_Position, WPOS_CENTERSCREEN,
           	WINDOW_ParentGroup, lw->objects[GID_MAIN] = VGroupObject,
				LAYOUT_AddChild, StringObject,
					STRINGA_TextVal,lw->host,
					GA_ReadOnly,TRUE,
				StringEnd,
				CHILD_Label, LabelObject,
					LABEL_Text,messages_get("Host"),
				LabelEnd,
				CHILD_WeightedHeight,0,
				LAYOUT_AddChild, StringObject,
					STRINGA_TextVal,lw->realm,
					GA_ReadOnly,TRUE,
				StringEnd,
				CHILD_Label, LabelObject,
					LABEL_Text,messages_get("Realm"),
				LabelEnd,
				CHILD_WeightedHeight,0,
				LAYOUT_AddChild, lw->objects[GID_USER] = StringObject,
					GA_ID,GID_USER,
					GA_TabCycle,TRUE,
				StringEnd,
				CHILD_Label, LabelObject,
					LABEL_Text,messages_get("Username"),
				LabelEnd,
				CHILD_WeightedHeight,0,
				LAYOUT_AddChild, lw->objects[GID_PASS] = StringObject,
					GA_ID,GID_PASS,
					STRINGA_HookType,SHK_PASSWORD,
					GA_TabCycle,TRUE,
				StringEnd,
				CHILD_Label, LabelObject,
					LABEL_Text,messages_get("Password"),
				LabelEnd,
				CHILD_WeightedHeight,0,
				LAYOUT_AddChild, HGroupObject,
					LAYOUT_AddChild, lw->objects[GID_LOGIN] = ButtonObject,
						GA_ID,GID_LOGIN,
						GA_RelVerify,TRUE,
						GA_Text,messages_get("Login"),
						GA_TabCycle,TRUE,
					ButtonEnd,
					CHILD_WeightedHeight,0,
					LAYOUT_AddChild, lw->objects[GID_CANCEL] = ButtonObject,
						GA_ID,GID_CANCEL,
						GA_RelVerify,TRUE,
						GA_Text,messages_get("Cancel"),
						GA_TabCycle,TRUE,
					ButtonEnd,
				LayoutEnd,
				CHILD_WeightedHeight,0,
			EndGroup,
		EndWindow;

	lw->win = (struct Window *)RA_OpenWindow(lw->objects[OID_MAIN]);

	lw->node = AddObject(window_list,AMINS_LOGINWINDOW);
	lw->node->objstruct = lw;
}

void ami_401login_close(struct gui_login_window *lw)
{
	/* If continuation exists, then forbid refetch */
	if (lw->cb != NULL)
		lw->cb(false, lw->cbpw);

	DisposeObject(lw->objects[OID_MAIN]);
	free(lw->host);
	DelObject(lw->node);
}

void ami_401login_login(struct gui_login_window *lw)
{
	ULONG *user,*pass;
	STRPTR userpass;

	GetAttr(STRINGA_TextVal,lw->objects[GID_USER],(ULONG *)&user);
	GetAttr(STRINGA_TextVal,lw->objects[GID_PASS],(ULONG *)&pass);

	userpass = ASPrintf("%s:%s",user,pass);
	urldb_set_auth_details(lw->url,lw->realm,userpass);
	FreeVec(userpass);

	lw->cb(true, lw->cbpw);

	/* Invalidate continuation */
	lw->cb = NULL;
	lw->cbpw = NULL;

	ami_401login_close(lw);
}

BOOL ami_401login_event(struct gui_login_window *lw)
{
	/* return TRUE if window destroyed */
	ULONG class,result,relevent = 0;
	uint16 code;

	while((result = RA_HandleInput(lw->objects[OID_MAIN], &code)) != WMHI_LASTMSG)
	{
       	switch(result & WMHI_CLASSMASK) // class
		{
			case WMHI_GADGETUP:
				switch(result & WMHI_GADGETMASK)
				{
					case GID_LOGIN:
						ami_401login_login(lw);
						return TRUE;
					break;

					case GID_CANCEL:
						ami_401login_close(lw);
						return TRUE;
					break;
				}
			break;
		}
	}
	return FALSE;
}
