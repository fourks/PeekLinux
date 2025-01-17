/*
+--------------------------------------------------------------------------+
| PROJECT : PROTOCOL STACK                                                 |
| FILE    : p_rlp.h                                                        |
| SOURCE  : "__out__\LNX_TI_cgt2_70\rel_n5_camera_micron\cdginc\condat\ms\doc\sap\rlp.pdf"                 |
| LastModified : "2002-04-22"                                              |
| IdAndVersion : "8411.101.98.115"                                         |
| SrcFileTime  : "Thu Nov 29 09:51:48 2007"                                |
| Generated by CCDGEN_2.5.5A on Wed Mar 11 09:58:59 2009                   |
|           !!DO NOT MODIFY!!DO NOT MODIFY!!DO NOT MODIFY!!                |
+--------------------------------------------------------------------------+
*/

/* PRAGMAS
 * PREFIX                 : NONE
 * COMPATIBILITY_DEFINES  : NO (require PREFIX)
 * ALWAYS_ENUM_IN_VAL_FILE: NO
 * ENABLE_GROUP: NO
 * CAPITALIZE_TYPENAME: NO
 */


#ifndef P_RLP_H
#define P_RLP_H


#define CDG_ENTER__P_RLP_H

#define CDG_ENTER__FILENAME _P_RLP_H
#define CDG_ENTER__P_RLP_H__FILE_TYPE CDGINC
#define CDG_ENTER__P_RLP_H__LAST_MODIFIED _2002_04_22
#define CDG_ENTER__P_RLP_H__ID_AND_VERSION _8411_101_98_115

#define CDG_ENTER__P_RLP_H__SRC_FILE_TIME _Thu_Nov_29_09_51_48_2007

#include "CDG_ENTER.h"

#undef CDG_ENTER__P_RLP_H

#undef CDG_ENTER__FILENAME


#include "p_rlp.val"


/*
 * End of substructure section, begin of primitive definition section
 */

#ifndef __T_RLP_ATTACH_REQ__
#define __T_RLP_ATTACH_REQ__
/*
 * 
 * CCDGEN:WriteStruct_Count==2856
 */
typedef struct
{
  U16                       k_ms_iwf;                 /*<  0:  2> window size ms -> iwf                              */
  U16                       k_iwf_ms;                 /*<  2:  2> window size iwf -> ms                              */
  U8                        t1;                       /*<  4:  1> acknowledge timer                                  */
  U8                        t2;                       /*<  5:  1> reply delay                                        */
  U8                        n2;                       /*<  6:  1> maximum numer of retransmission attempts           */
  U8                        pt;                       /*<  7:  1> type of data compression                           */
  U8                        p0;                       /*<  8:  1> v.42bis data compression direction                 */
  U8                        _align0;                  /*<  9:  1> alignment                                          */
  U16                       p1;                       /*< 10:  2> v.42 bis number of possible codewords              */
  U8                        p2;                       /*< 12:  1> v.42 bis maximum encodable string length           */
  U8                        _align1;                  /*< 13:  1> alignment                                          */
  U16                       bytes_per_prim;           /*< 14:  2> bytes per primitive                                */
  U8                        rate;                     /*< 16:  1> transmission rate                                  */
  U8                        _align2;                  /*< 17:  1> alignment                                          */
  U8                        _align3;                  /*< 18:  1> alignment                                          */
  U8                        _align4;                  /*< 19:  1> alignment                                          */
} T_RLP_ATTACH_REQ;
#endif

#ifndef __T_RLP_DETACH_REQ__
#define __T_RLP_DETACH_REQ__
/*
 * 
 * CCDGEN:WriteStruct_Count==2857
 */
typedef struct
{
  U8                        dummy;                    /*<  0:  1> no parameters                                      */
} T_RLP_DETACH_REQ;
#endif

#ifndef __T_RLP_DETACH_CNF__
#define __T_RLP_DETACH_CNF__
/*
 * 
 * CCDGEN:WriteStruct_Count==2858
 */
typedef struct
{
  U8                        dummy;                    /*<  0:  1> no parameters                                      */
} T_RLP_DETACH_CNF;
#endif

#ifndef __T_RLP_CONNECT_REQ__
#define __T_RLP_CONNECT_REQ__
/*
 * 
 * CCDGEN:WriteStruct_Count==2859
 */
typedef struct
{
  U8                        dummy;                    /*<  0:  1> no parameters                                      */
} T_RLP_CONNECT_REQ;
#endif

#ifndef __T_RLP_CONNECT_IND__
#define __T_RLP_CONNECT_IND__
/*
 * 
 * CCDGEN:WriteStruct_Count==2860
 */
typedef struct
{
  U8                        dummy;                    /*<  0:  1> no parameters                                      */
} T_RLP_CONNECT_IND;
#endif

#ifndef __T_RLP_CONNECT_RES__
#define __T_RLP_CONNECT_RES__
/*
 * 
 * CCDGEN:WriteStruct_Count==2861
 */
typedef struct
{
  U8                        ack_flg;                  /*<  0:  1> acknowledge flag                                   */
  U8                        _align0;                  /*<  1:  1> alignment                                          */
  U8                        _align1;                  /*<  2:  1> alignment                                          */
  U8                        _align2;                  /*<  3:  1> alignment                                          */
} T_RLP_CONNECT_RES;
#endif

#ifndef __T_RLP_CONNECT_CNF__
#define __T_RLP_CONNECT_CNF__
/*
 * 
 * CCDGEN:WriteStruct_Count==2862
 */
typedef struct
{
  U8                        ack_flg;                  /*<  0:  1> acknowledge flag                                   */
  U8                        _align0;                  /*<  1:  1> alignment                                          */
  U8                        _align1;                  /*<  2:  1> alignment                                          */
  U8                        _align2;                  /*<  3:  1> alignment                                          */
} T_RLP_CONNECT_CNF;
#endif

#ifndef __T_RLP_DISC_REQ__
#define __T_RLP_DISC_REQ__
/*
 * 
 * CCDGEN:WriteStruct_Count==2863
 */
typedef struct
{
  U8                        dummy;                    /*<  0:  1> no parameters                                      */
} T_RLP_DISC_REQ;
#endif

#ifndef __T_RLP_DISC_IND__
#define __T_RLP_DISC_IND__
/*
 * 
 * CCDGEN:WriteStruct_Count==2864
 */
typedef struct
{
  U8                        dummy;                    /*<  0:  1> no parameters                                      */
} T_RLP_DISC_IND;
#endif

#ifndef __T_RLP_DISC_CNF__
#define __T_RLP_DISC_CNF__
/*
 * 
 * CCDGEN:WriteStruct_Count==2865
 */
typedef struct
{
  U8                        dummy;                    /*<  0:  1> no parameters                                      */
} T_RLP_DISC_CNF;
#endif

#ifndef __T_RLP_RESET_REQ__
#define __T_RLP_RESET_REQ__
/*
 * 
 * CCDGEN:WriteStruct_Count==2866
 */
typedef struct
{
  U8                        dummy;                    /*<  0:  1> no parameters                                      */
} T_RLP_RESET_REQ;
#endif

#ifndef __T_RLP_RESET_IND__
#define __T_RLP_RESET_IND__
/*
 * 
 * CCDGEN:WriteStruct_Count==2867
 */
typedef struct
{
  U8                        dummy;                    /*<  0:  1> no parameters                                      */
} T_RLP_RESET_IND;
#endif

#ifndef __T_RLP_RESET_RES__
#define __T_RLP_RESET_RES__
/*
 * 
 * CCDGEN:WriteStruct_Count==2868
 */
typedef struct
{
  U8                        dummy;                    /*<  0:  1> no parameters                                      */
} T_RLP_RESET_RES;
#endif

#ifndef __T_RLP_RESET_CNF__
#define __T_RLP_RESET_CNF__
/*
 * 
 * CCDGEN:WriteStruct_Count==2869
 */
typedef struct
{
  U8                        dummy;                    /*<  0:  1> no parameters                                      */
} T_RLP_RESET_CNF;
#endif

#ifndef __T_RLP_READY_IND__
#define __T_RLP_READY_IND__
/*
 * 
 * CCDGEN:WriteStruct_Count==2870
 */
typedef struct
{
  U8                        dummy;                    /*<  0:  1> no parameters                                      */
} T_RLP_READY_IND;
#endif

#ifndef __T_RLP_DATA_REQ__
#define __T_RLP_DATA_REQ__
/*
 * 
 * CCDGEN:WriteStruct_Count==2871
 */
typedef struct
{
  U8                        reserved[4];              /*<  0:  4> Reserved                                           */
  T_sdu                     sdu;                      /*<  4: ? > Service Data Unit                                  */
} T_RLP_DATA_REQ;
#endif

#ifndef __T_RLP_GETDATA_REQ__
#define __T_RLP_GETDATA_REQ__
/*
 * 
 * CCDGEN:WriteStruct_Count==2872
 */
typedef struct
{
  U8                        dummy;                    /*<  0:  1> no parameters                                      */
} T_RLP_GETDATA_REQ;
#endif

#ifndef __T_RLP_DATA_IND__
#define __T_RLP_DATA_IND__
/*
 * 
 * CCDGEN:WriteStruct_Count==2873
 */
typedef struct
{
  U16                       data_size;                /*<  0:  2> data size                                          */
  U8                        reserved[2];              /*<  2:  2> Reserved                                           */
  T_sdu                     sdu;                      /*<  4: ? > Service Data Unit                                  */
} T_RLP_DATA_IND;
#endif

#ifndef __T_RLP_UI_REQ__
#define __T_RLP_UI_REQ__
/*
 * 
 * CCDGEN:WriteStruct_Count==2874
 */
typedef struct
{
  T_sdu                     sdu;                      /*<  0: ? > Service Data Unit                                  */
} T_RLP_UI_REQ;
#endif

#ifndef __T_RLP_UI_IND__
#define __T_RLP_UI_IND__
/*
 * 
 * CCDGEN:WriteStruct_Count==2875
 */
typedef struct
{
  T_sdu                     sdu;                      /*<  0: ? > Service Data Unit                                  */
} T_RLP_UI_IND;
#endif

#ifndef __T_RLP_XID_IND__
#define __T_RLP_XID_IND__
/*
 * 
 * CCDGEN:WriteStruct_Count==2876
 */
typedef struct
{
  U8                        rlp_vers;                 /*<  0:  1> rlp version number                                 */
  U8                        _align0;                  /*<  1:  1> alignment                                          */
  U16                       k_ms_iwf;                 /*<  2:  2> window size ms -> iwf                              */
  U16                       k_iwf_ms;                 /*<  4:  2> window size iwf -> ms                              */
  U8                        t1;                       /*<  6:  1> acknowledge timer                                  */
  U8                        t2;                       /*<  7:  1> reply delay                                        */
  U8                        n2;                       /*<  8:  1> maximum numer of retransmission attempts           */
  U8                        pt;                       /*<  9:  1> type of data compression                           */
  U8                        p0;                       /*< 10:  1> v.42bis data compression direction                 */
  U8                        _align1;                  /*< 11:  1> alignment                                          */
  U16                       p1;                       /*< 12:  2> v.42 bis number of possible codewords              */
  U8                        p2;                       /*< 14:  1> v.42 bis maximum encodable string length           */
  U8                        _align2;                  /*< 15:  1> alignment                                          */
} T_RLP_XID_IND;
#endif

#ifndef __T_RLP_ERROR_IND__
#define __T_RLP_ERROR_IND__
/*
 * 
 * CCDGEN:WriteStruct_Count==2877
 */
typedef struct
{
  U16                       cause;                    /*<  0:  2> cause                                              */
  U8                        _align0;                  /*<  2:  1> alignment                                          */
  U8                        _align1;                  /*<  3:  1> alignment                                          */
} T_RLP_ERROR_IND;
#endif

#ifndef __T_RLP_STATISTIC_IND__
#define __T_RLP_STATISTIC_IND__
/*
 * 
 * CCDGEN:WriteStruct_Count==2878
 */
typedef struct
{
  U32                       error_rate;               /*<  0:  4> error_rate                                         */
} T_RLP_STATISTIC_IND;
#endif

#ifndef __T_RLP_REMAP_REQ__
#define __T_RLP_REMAP_REQ__
/*
 * 
 * CCDGEN:WriteStruct_Count==2879
 */
typedef struct
{
  U8                        rate;                     /*<  0:  1> transmission rate                                  */
  U8                        _align0;                  /*<  1:  1> alignment                                          */
  U8                        _align1;                  /*<  2:  1> alignment                                          */
  U8                        _align2;                  /*<  3:  1> alignment                                          */
} T_RLP_REMAP_REQ;
#endif

#ifndef __T_RLP_REMAP_CNF__
#define __T_RLP_REMAP_CNF__
/*
 * 
 * CCDGEN:WriteStruct_Count==2880
 */
typedef struct
{
  U8                        dummy;                    /*<  0:  1> no parameters                                      */
} T_RLP_REMAP_CNF;
#endif

#ifndef __T_RLP_REMAP_DATA_IND__
#define __T_RLP_REMAP_DATA_IND__
/*
 * 
 * CCDGEN:WriteStruct_Count==2881
 */
typedef struct
{
  U16                       data_size;                /*<  0:  2> data size                                          */
  U8                        reserved[2];              /*<  2:  2> Reserved                                           */
  T_sdu                     sdu;                      /*<  4: ? > Service Data Unit                                  */
} T_RLP_REMAP_DATA_IND;
#endif

#ifndef __T_RLP_REMAP_DATA_RES__
#define __T_RLP_REMAP_DATA_RES__
/*
 * 
 * CCDGEN:WriteStruct_Count==2882
 */
typedef struct
{
  U8                        dummy;                    /*<  0:  1> no parameters                                      */
} T_RLP_REMAP_DATA_RES;
#endif


#include "CDG_LEAVE.h"


#endif
