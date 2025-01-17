/*
+--------------------------------------------------------------------------+
| PROJECT : PROTOCOL STACK                                                 |
| FILE    : m_ss.h                                                         |
| SOURCE  : "g23m-doc\doc\msg\ss.mdf"                                      |
| LastModified : "2003-02-04"                                              |
| IdAndVersion : "6147.601.97.104"                                         |
| SrcFileTime  : "Wed Nov 28 10:21:24 2007"                                |
| Generated by CCDGEN_2.5.5A on Wed Mar 11 09:58:57 2009                   |
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


#ifndef M_SS_H
#define M_SS_H


#define CDG_ENTER__M_SS_H

#define CDG_ENTER__FILENAME _M_SS_H
#define CDG_ENTER__M_SS_H__FILE_TYPE CDGINC
#define CDG_ENTER__M_SS_H__LAST_MODIFIED _2003_02_04
#define CDG_ENTER__M_SS_H__ID_AND_VERSION _6147_601_97_104

#define CDG_ENTER__M_SS_H__SRC_FILE_TIME _Wed_Nov_28_10_21_24_2007

#include "CDG_ENTER.h"

#undef CDG_ENTER__M_SS_H

#undef CDG_ENTER__FILENAME


#include "m_ss.val"

#ifndef __T_ss_facility__
#define __T_ss_facility__
/*
 * SS facility
 * CCDGEN:WriteStruct_Count==411
 */
typedef struct
{
  U8                        c_fac_info;               /*<  0:  1> counter                                            */
  U8                        fac_info[L3MAX];          /*<  1:251> Facility Information                               */
} T_ss_facility;
#endif

#ifndef __T_ss_version__
#define __T_ss_version__
/*
 * SS version
 * CCDGEN:WriteStruct_Count==412
 */
typedef struct
{
  U8                        c_ver;                    /*<  0:  1> counter                                            */
  U8                        ver[MAX_SS_VERSION];      /*<  1:  1> Version Content                                    */
  U8                        _align0;                  /*<  2:  1> alignment                                          */
  U8                        _align1;                  /*<  3:  1> alignment                                          */
} T_ss_version;
#endif

#ifndef __T_ss_cause__
#define __T_ss_cause__
/*
 * Cause
 * CCDGEN:WriteStruct_Count==413
 */
typedef struct
{
  U8                        v_cs2;                    /*<  0:  1> valid-flag                                         */
  U8                        cs2;                      /*<  1:  1> Coding standard                                    */
  U8                        v_loc;                    /*<  2:  1> valid-flag                                         */
  U8                        loc;                      /*<  3:  1> Location                                           */
  U8                        v_rec;                    /*<  4:  1> valid-flag                                         */
  U8                        rec;                      /*<  5:  1> Recommendation                                     */
  U8                        v_cs;                     /*<  6:  1> valid-flag                                         */
  U8                        cs;                       /*<  7:  1> Cause                                              */
  U8                        c_diag;                   /*<  8:  1> counter                                            */
  U8                        diag[27];                 /*<  9: 27> Diagnostics                                        */
} T_ss_cause;
#endif


/*
 * End of substructure section, begin of message definition section
 */

#ifndef __T_D_SS_FACILITY__
#define __T_D_SS_FACILITY__
/*
 * 
 * CCDGEN:WriteStruct_Count==414
 */
typedef struct
{
  U8                        msg_type;                 /*<  0:  1> Message Type                                       */
  U8                        _align0;                  /*<  1:  1> alignment                                          */
  U8                        _align1;                  /*<  2:  1> alignment                                          */
  U8                        _align2;                  /*<  3:  1> alignment                                          */
  T_ss_facility             ss_facility;              /*<  4:252> SS facility                                        */
} T_D_SS_FACILITY;
#endif

#ifndef __T_U_SS_FACILITY__
#define __T_U_SS_FACILITY__
/*
 * 
 * CCDGEN:WriteStruct_Count==415
 */
typedef struct
{
  U8                        msg_type;                 /*<  0:  1> Message Type                                       */
  U8                        _align0;                  /*<  1:  1> alignment                                          */
  U8                        _align1;                  /*<  2:  1> alignment                                          */
  U8                        _align2;                  /*<  3:  1> alignment                                          */
  T_ss_facility             ss_facility;              /*<  4:252> SS facility                                        */
} T_U_SS_FACILITY;
#endif

#ifndef __T_D_SS_REGISTER__
#define __T_D_SS_REGISTER__
/*
 * 
 * CCDGEN:WriteStruct_Count==416
 */
typedef struct
{
  U8                        msg_type;                 /*<  0:  1> Message Type                                       */
  U8                        _align0;                  /*<  1:  1> alignment                                          */
  U8                        _align1;                  /*<  2:  1> alignment                                          */
  U8                        v_ss_facility;            /*<  3:  1> valid-flag                                         */
  T_ss_facility             ss_facility;              /*<  4:252> SS facility                                        */
} T_D_SS_REGISTER;
#endif

#ifndef __T_U_SS_REGISTER__
#define __T_U_SS_REGISTER__
/*
 * 
 * CCDGEN:WriteStruct_Count==417
 */
typedef struct
{
  U8                        msg_type;                 /*<  0:  1> Message Type                                       */
  U8                        _align0;                  /*<  1:  1> alignment                                          */
  U8                        _align1;                  /*<  2:  1> alignment                                          */
  U8                        v_ss_facility;            /*<  3:  1> valid-flag                                         */
  T_ss_facility             ss_facility;              /*<  4:252> SS facility                                        */
  U8                        _align2;                  /*<256:  1> alignment                                          */
  U8                        _align3;                  /*<257:  1> alignment                                          */
  U8                        _align4;                  /*<258:  1> alignment                                          */
  U8                        v_ss_version;             /*<259:  1> valid-flag                                         */
  T_ss_version              ss_version;               /*<260:  4> SS version                                         */
} T_U_SS_REGISTER;
#endif

#ifndef __T_B_SS_REL_COMP__
#define __T_B_SS_REL_COMP__
/*
 * 
 * CCDGEN:WriteStruct_Count==418
 */
typedef struct
{
  U8                        msg_type;                 /*<  0:  1> Message Type                                       */
  U8                        _align0;                  /*<  1:  1> alignment                                          */
  U8                        _align1;                  /*<  2:  1> alignment                                          */
  U8                        v_ss_cause;               /*<  3:  1> valid-flag                                         */
  T_ss_cause                ss_cause;                 /*<  4: 36> Cause                                              */
  U8                        _align2;                  /*< 40:  1> alignment                                          */
  U8                        _align3;                  /*< 41:  1> alignment                                          */
  U8                        _align4;                  /*< 42:  1> alignment                                          */
  U8                        v_ss_facility;            /*< 43:  1> valid-flag                                         */
  T_ss_facility             ss_facility;              /*< 44:252> SS facility                                        */
} T_B_SS_REL_COMP;
#endif


#include "CDG_LEAVE.h"


#endif
