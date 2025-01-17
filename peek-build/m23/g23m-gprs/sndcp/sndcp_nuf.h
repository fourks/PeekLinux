/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  sndcp_nuf.h
+----------------------------------------------------------------------------- 
|  Copyright 2002 Texas Instruments Berlin, AG 
|                 All rights reserved. 
| 
|                 This file is confidential and a trade secret of Texas 
|                 Instruments Berlin, AG 
|                 The receipt of or possession of this file does not convey 
|                 any rights to reproduce or disclose its contents or to 
|                 manufacture, use, or sell anything it may describe, in 
|                 whole, or in part, without the specific written consent of 
|                 Texas Instruments Berlin, AG. 
+----------------------------------------------------------------------------- 
|  Purpose :  Definitions for sndcp_nuf.c 
+----------------------------------------------------------------------------- 
*/ 

/*---- HISTORY --------------------------------------------------------------*/
#if 0

  $Log:: /GPRS/Condat/MS/SRC/SNDCP/sndcp_nuf.h                             $
 * 
 * 22    15.05.00 16:55 Hk
 * T H E   E N D.
 * 
 * 21    15.05.00 16:16 Hk
 * backup.
 * 
 * 20    9.05.00 18:59 Hk
 * XID negotiation work in progress. Backup.
 * 
 * 19    18.04.00 18:47 Hk
 * Backup. Work on XID negotiation.
 * 
 * 18    30.03.00 16:04 Hk
 * 
 * 17    17.03.00 18:03 Hk
 * Acknowledged data transfer is now implemented and tested uplink and
 * downlink.
 * 
 * 16    10.03.00 17:48 Hk
 * Backup. First downlink ack data transfer ok (case 700).
 * 
 * 15    7.03.00 18:24 Hk
 * Backup. Last check in before adding extra downlink service for timeout and
 * re-ordering. 
 * 
 * 14    25.02.00 18:27 Hk
 * Backup, test case 346 runs (deactivation of ack context including ll
 * release.
 * 
 * 13    17.02.00 14:32 Hk
 * Last checkin before maybe splitting sd in two services for 1.unack and 2.
 * ack op mode.
 * 
 * 12    11.02.00 18:49 Hk
 * Establishment of LLC acknowledged operation mode. First test case SNDCP035
 * successful. Backup.
 * 
 * 11    9.02.00 15:02 Hk
 * Backup. Unacknowledged data transfer possible. Up to testcase SNDCP034.
 * 
 * 10    7.02.00 18:02 Hk
 * Backup, error situations in service sd, up to testcase 028.
 * 
 * 9     3.02.00 18:08 Hk
 * Backup, downlink transfer up to test case 016.
 * 
 * 8     26.01.00 16:24 Hk
 * Made services sd and nd compilable.
 * 
 * 7     13.01.00 18:07 Hk
 * Backup. Test case SNDCP008 successful.
 * 
 * 6     7.01.00 18:09 Hk
 * Backup. Data transfer in service su in case of LL_READY_IND, test case
 * SNDCP005.
 * 
 * 5     4.01.00 18:56 Hk
 * Work on uplink data transfer, test case SNDCP003 successful.
 * 
 * 4     30.12.99 17:16 Hk
 * Last backup before millennium crash. Continued work on service su, data
 * transfer.
 * 
 * 3     29.12.99 17:21 Hk
 * Started to rework service su, queue of inoming sn_nuitdata_req prims will
 * be designed in more detail in SDL.
 * 
 * 2     28.12.99 18:18 Hk
 * Added test primitives with sdu instead of desc_list and started work on
 * data transfer.
 * 
 * 1     28.12.99 13:07 Hk
 * Initial.

#endif

#ifndef SNDCP_NUF_H
#define SNDCP_NUF_H


EXTERN void nu_buffer_npdu (UBYTE npdu_number, T_SN_DATA_REQ* sn_data_req);

#ifndef NCONFIG

EXTERN void nu_config_send(UBYTE nsapi, U32 octets);
EXTERN void nu_config_discard(UBYTE nsapi);
EXTERN void nu_config_resume (UBYTE nsapi);
EXTERN void nu_config_send_next(UBYTE nsapi);

#endif

EXTERN void nu_delete_npdus (void);

EXTERN void nu_delete_npdu (UBYTE nsapi, UBYTE npdu_number);

EXTERN void nu_init (void);

EXTERN void nu_ready_ind_if_nec(UBYTE nsapi);

EXTERN void nu_unitready_ind_if_nec(UBYTE nsapi);

#endif /* !SNDCP_NUF_H */

