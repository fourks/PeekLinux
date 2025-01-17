/*
+--------------------------------------------------------------------------+
| PROJECT : PROTOCOL STACK                                                 |
| FILE    : p_tra.h                                                        |
| SOURCE  : "__out__\LNX_TI_cgt2_70\rel_n5_camera_micron\cdginc\condat\ms\doc\sap\tra.pdf"                 |
| LastModified : "2002-06-10"                                              |
| IdAndVersion : "8411.106.98.101"                                         |
| SrcFileTime  : "Thu Nov 29 09:55:46 2007"                                |
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


#ifndef P_TRA_H
#define P_TRA_H


#define CDG_ENTER__P_TRA_H

#define CDG_ENTER__FILENAME _P_TRA_H
#define CDG_ENTER__P_TRA_H__FILE_TYPE CDGINC
#define CDG_ENTER__P_TRA_H__LAST_MODIFIED _2002_06_10
#define CDG_ENTER__P_TRA_H__ID_AND_VERSION _8411_106_98_101

#define CDG_ENTER__P_TRA_H__SRC_FILE_TIME _Thu_Nov_29_09_55_46_2007

#include "CDG_ENTER.h"

#undef CDG_ENTER__P_TRA_H

#undef CDG_ENTER__FILENAME


#include "p_tra.val"


/*
 * End of substructure section, begin of primitive definition section
 */

#ifndef __T_TRA_ACTIVATE_REQ__
#define __T_TRA_ACTIVATE_REQ__
/*
 * 
 * CCDGEN:WriteStruct_Count==3103
 */
typedef struct
{
  U8                        dummy;                    /*<  0:  1> no parameters                                      */
} T_TRA_ACTIVATE_REQ;
#endif

#ifndef __T_TRA_ACTIVATE_CNF__
#define __T_TRA_ACTIVATE_CNF__
/*
 * 
 * CCDGEN:WriteStruct_Count==3104
 */
typedef struct
{
  U8                        ack_flg;                  /*<  0:  1> acknowledge flag                                   */
  U8                        _align0;                  /*<  1:  1> alignment                                          */
  U8                        _align1;                  /*<  2:  1> alignment                                          */
  U8                        _align2;                  /*<  3:  1> alignment                                          */
} T_TRA_ACTIVATE_CNF;
#endif

#ifndef __T_TRA_DEACTIVATE_REQ__
#define __T_TRA_DEACTIVATE_REQ__
/*
 * 
 * CCDGEN:WriteStruct_Count==3105
 */
typedef struct
{
  U8                        dummy;                    /*<  0:  1> no parameters                                      */
} T_TRA_DEACTIVATE_REQ;
#endif

#ifndef __T_TRA_DEACTIVATE_CNF__
#define __T_TRA_DEACTIVATE_CNF__
/*
 * 
 * CCDGEN:WriteStruct_Count==3106
 */
typedef struct
{
  U8                        dummy;                    /*<  0:  1> no parameters                                      */
} T_TRA_DEACTIVATE_CNF;
#endif

#ifndef __T_TRA_DTI_REQ__
#define __T_TRA_DTI_REQ__
/*
 * 
 * CCDGEN:WriteStruct_Count==3107
 */
typedef struct
{
  U8                        dti_conn;                 /*<  0:  1> DTI connect                                        */
  U8                        _align0;                  /*<  1:  1> alignment                                          */
  U8                        _align1;                  /*<  2:  1> alignment                                          */
  U8                        _align2;                  /*<  3:  1> alignment                                          */
  U32                       link_id;                  /*<  4:  4> link identifier                                    */
  U8                        entity_name[TRA_ENTITY_NAME_LEN]; /*<  8:  6> name of an neighbour entity                        */
  U8                        dti_direction;            /*< 14:  1> DTI direction                                      */
  U8                        _align3;                  /*< 15:  1> alignment                                          */
} T_TRA_DTI_REQ;
#endif

#ifndef __T_TRA_DTI_CNF__
#define __T_TRA_DTI_CNF__
/*
 * 
 * CCDGEN:WriteStruct_Count==3108
 */
typedef struct
{
  U8                        dti_conn;                 /*<  0:  1> DTI connect                                        */
  U8                        _align0;                  /*<  1:  1> alignment                                          */
  U8                        _align1;                  /*<  2:  1> alignment                                          */
  U8                        _align2;                  /*<  3:  1> alignment                                          */
  U32                       link_id;                  /*<  4:  4> link identifier                                    */
} T_TRA_DTI_CNF;
#endif

#ifndef __T_TRA_DTI_IND__
#define __T_TRA_DTI_IND__
/*
 * 
 * CCDGEN:WriteStruct_Count==3109
 */
typedef struct
{
  U32                       link_id;                  /*<  0:  4> link identifier                                    */
} T_TRA_DTI_IND;
#endif


#include "CDG_LEAVE.h"


#endif
