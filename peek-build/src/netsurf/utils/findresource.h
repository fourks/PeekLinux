/*
 * Copyright 2010 Vincent Sanders <vince@kyllikki.org>
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

#ifndef _NETSURF_UTILS_FINDRESOURCE_H_
#define _NETSURF_UTILS_FINDRESOURCE_H_

/** Create a normalised file name.
 *
 * If the file described by the format exists and is accessible the
 * normalised path is placed in str and a pointer to str returned
 * otherwise NULL is returned. The string in str is always modified.
 *
 * @param str A buffer to contain the normalised file name must be at
 *            least PATH_MAX bytes long.
 * @param format A printf format for the filename.
 * @param ap The list of arguments for the format.
 * @return A pointer to the expanded filename or NULL if the file is
 *         not present or accessible.
 */
char *vsfindfile(char *str, const char *format, va_list ap);

/** Create a normalised file name.
 *
 * Similar to vsfindfile but takes variadic (printf like) parameters
 */
char *sfindfile(char *str, const char *format, ...);

/** Create a normalised file name.
 *
 * Similar to sfindfile but allocates its own storage for the
 * returned string. The caller must free this sorage.
 */
char *findfile(const char *format, ...);

/** Searches an array of resource paths for a file.
 *
 * Iterates through a vector of resource paths and returns the
 * normalised file name of the first acessible file or NULL if no file
 * can be found in any of the resource paths.
 *
 * @param respathv The resource path vector to iterate.
 * @param filepath The buffer to place the result in.
 * @param filename The filename of the resource to search for.
 * @return A pointer to filepath if a target is found or NULL if not.
 */
char *sfindresource(char **respathv, char *filepath, const char *filename);

/** Searches an array of resource paths for a file.
 *
 * Similar to sfindresource except it allocates its own storage for
 * the returned string. The caller must free this sorage.
 */
char *findresource(char **respathv, const char *filename);

/** Searches an array of resource paths for a file optionally forcing a default.
 *
 * Similar to sfindresource except if no resource is found the default
 * is used as an additional path element to search, if that still
 * fails the returned path is set to the concatination of the default
 * path and the filename.
 */
char *sfindresourcedef(char **respathv, char *filepath, const char *filename, const char *def);

/** Merge two string vectors into a resource search path vector.
 *
 * @param pathv A string vector containing path elemets to scan.
 * @param langv A string vector containing language names to enumerate.
 * @return A pointer to a NULL terminated string vector of valid
 *         resource directories.
 */
char **findresource_generate(char * const *pathv, const char * const *langv);

#endif
