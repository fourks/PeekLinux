/*
+--------------------------------------------------------------------------+
| PROJECT : PROTOCOL STACK                                                 |
| FILE    : p_mdl.h                                                        |
| SOURCE  : "__out__\LNX_TI_cgt2_70\rel_n5_camera_micron\cdginc\condat\ms\doc\sap\mdl.pdf"                 |
| LastModified : "2002-04-19"                                              |
| IdAndVersion : "6147.111.97.101"                                         |
| SrcFileTime  : "Thu Nov 29 09:45:46 2007"                                |
| Generated by CCDGEN_2.5.5A on Wed Mar 11 09:58:58 2009                   |
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


#ifndef P_MDL_H
#define P_MDL_H


#define CDG_ENTER__P_MDL_H

#define CDG_ENTER__FILENAME _P_MDL_H
#define CDG_ENTER__P_MDL_H__FILE_TYPE CDGINC
#define CDG_ENTER__P_MDL_H__LAST_MODIFIED _2002_04_19
#define CDG_ENTER__P_MDL_H__ID_AND_VERSION _6147_111_97_101

#define CDG_ENTER__P_MDL_H__SRC_FILE_TIME _Thu_Nov_29_09_45_46_2007

#include "CDG_ENTER.h"

#undef CDG_ENTER__P_MDL_H

#undef CDG_ENTER__FILENAME


#include "p_mdl.val"


/*
 * End of substructure section, begin of primitive definition section
 */

#ifndef __T_MDL_ERROR_IND__
#define __T_MDL_ERROR_IND__
/*
 * 
 * CCDGEN:WriteStruct_Count==1786
 */
typedef struct
{
  U8                        ch_type;                  /*<  0:  1> channel type                                       */
  U8                        sapi;                     /*<  1:  1> service access point identifier                    */
  U8                        cs;                       /*<  2:  1> error cause                                        */
  U8                        _align0;                  /*<  3:  1> alignment                                          */
} T_MDL_ERROR_IND;
#endif

#ifndef __T_MDL_RELEASE_REQ__
#define __T_MDL_RELEASE_REQ__
/*
 * 
 * CCDGEN:WriteStruct_Count==1787
 */
typedef struct
{
  U8                        ch_type;                  /*<  0:  1> channel type                                       */
  U8                        sapi;                     /*<  1:  1> service access point identifier                    */
  U8                        _align0;                  /*<  2:  1> alignment                                          */
  U8                        _align1;                  /*<  3:  1> alignment                                          */
} T_MDL_RELEASE_REQ;
#endif


#include "CDG_LEAVE.h"


#endif
