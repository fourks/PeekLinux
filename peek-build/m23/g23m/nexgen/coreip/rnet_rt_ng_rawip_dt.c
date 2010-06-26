/*****************************************************************************
 * $Id: rawip_dt.c,v 1.1 2001/05/28 16:28:23 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * Raw socket interface - Global Data
 *----------------------------------------------------------------------------
 *    Copyright (c) 1998,1999,2000 NexGen Software.
 *
 *  All rights reserved. NexGen Software' source code is an unpublished
 *  work and the use of a copyright notice does not imply otherwise.
 *  This source code contains confidential, trade secret material of
 *  NexGen Software. Any attempt or participation in deciphering, decoding,
 *  reverse engineering or in any way altering the source code is
 *  strictly prohibited, unless the prior written consent of
 *  NexGen Software is obtained.
 *
 *    This software is  supplied  under  the terms of a
 *    license agreement or nondisclosure agreement with
 *    NexGen Software, and may not be copied or disclosed
 *    except  in  accordance  with  the  terms of  that
 *    agreement.
 *
 *----------------------------------------------------------------------------
 * 28/05/2001 - Regis Feneon
 *  from rawip.c
 *****************************************************************************/

#include <ngip.h>
#include <ngdsock.h>

NGnode ngRawip_Sockq; /* sockets in use */

