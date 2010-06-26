/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  sndcp_sds.h
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
|  Purpose :  Definitions for sndcp_sds.c 
+----------------------------------------------------------------------------- 
*/ 

/*---- HISTORY --------------------------------------------------------------*/
#if 0

  $Log:: /GPRS/Condat/MS/SRC/SNDCP/sndcp_sds.h                             $
 * 
 * 16    15.05.00 16:55 Hk
 * T H E   E N D.
 * 
 * 15    15.05.00 16:16 Hk
 * backup.
 * 
 * 14    9.05.00 18:59 Hk
 * XID negotiation work in progress. Backup.
 * 
 * 13    18.04.00 18:47 Hk
 * Backup. Work on XID negotiation.
 * 
 * 12    30.03.00 16:04 Hk
 * 
 * 11    17.03.00 18:03 Hk
 * Acknowledged data transfer is now implemented and tested uplink and
 * downlink.
 * 
 * 10    10.03.00 17:49 Hk
 * Backup. First downlink ack data transfer ok (case 700).
 * 
 * 9     7.03.00 18:24 Hk
 * Backup. Last check in before adding extra downlink service for timeout and
 * re-ordering. 
 * 
 * 8     25.02.00 18:27 Hk
 * Backup, test case 346 runs (deactivation of ack context including ll
 * release.
 * 
 * 7     17.02.00 14:32 Hk
 * Last checkin before maybe splitting sd in two services for 1.unack and 2.
 * ack op mode.
 * 
 * 6     11.02.00 18:49 Hk
 * Establishment of LLC acknowledged operation mode. First test case SNDCP035
 * successful. Backup.
 * 
 * 5     9.02.00 15:03 Hk
 * Backup. Unacknowledged data transfer possible. Up to testcase SNDCP034.
 * 
 * 4     7.02.00 18:02 Hk
 * Backup, error situations in service sd, up to testcase 028.
 * 
 * 3     3.02.00 18:08 Hk
 * Backup, downlink transfer up to test case 016.
 * 
 * 2     26.01.00 16:24 Hk
 * Made services sd and nd compilable.
 * 
 * 1     25.01.00 17:32 Hk
 * Initial.

#endif

#ifndef SNDCP_SDS_H
#define SNDCP_SDS_H


EXTERN void sig_cia_sd_cia_decomp_ind(T_CIA_DECOMP_IND* cia_decomp_ind, UBYTE p_id);

EXTERN void sig_mg_sd_getunitdata (UBYTE sapi, UBYTE nsapi);

EXTERN void sig_mg_sd_delete_npdus (UBYTE nsapi,
                                    UBYTE sapi);

EXTERN void sig_mg_sd_reset_ind (UBYTE nsapi);

EXTERN void sig_pd_sd_unitdata_ind (T_LL_UNITDATA_IND* ll_unitdata_ind);


EXTERN void sig_nd_sd_getunitdata_req (UBYTE sapi,
                                       UBYTE nsapi);

EXTERN void sig_cia_sd_getunitdata (UBYTE sapi, UBYTE nsapi);

#endif /* !SNDCP_SDS_H */

