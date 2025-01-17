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

#include <sys/types.h>
#include "utils/utf8.h"
#include <proto/exec.h>
#include <proto/diskfont.h>
#include <diskfont/diskfonttag.h>

utf8_convert_ret utf8_to_local_encoding(const char *string, size_t len,
	char **result)
{
	LONG charset;
	char *encname;

	charset = GetDiskFontCtrl(DFCTRL_CHARSET);
	encname = ObtainCharsetInfo(DFCS_NUMBER, charset, DFCS_MIMENAME);

	return utf8_to_enc(string,encname,len,result);
}

void ami_utf8_free(char *ptr)
{
	if(ptr) free(ptr);
}

char *ami_utf8_easy(char *string)
{
	char *localtext;

	if(utf8_to_local_encoding(string,strlen(string),&localtext) == UTF8_CONVERT_OK)
	{
		return localtext;
	}
	else
	{
		return strdup(string);
	}
}

char *ami_to_utf8_easy(char *string)
{
	char *localtext;

	if(utf8_from_local_encoding(string,strlen(string),&localtext) == UTF8_CONVERT_OK)
	{
		return localtext;
	}
	else
	{
		return strdup(string);
	}
}

utf8_convert_ret utf8_from_local_encoding(const char *string, size_t len,
	char **result)
{
	LONG charset;
	char *encname;

	charset = GetDiskFontCtrl(DFCTRL_CHARSET);
	encname = ObtainCharsetInfo(DFCS_NUMBER, charset, DFCS_MIMENAME);
	
	return utf8_from_enc(string,encname,len,result);
}
