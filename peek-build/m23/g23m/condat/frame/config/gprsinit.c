/* 
+----------------------------------------------------------------------------- 
|  Project :  
|  Modul   :  
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
|  Purpose :  Nucleus Entry Function Application_Initialize()
+----------------------------------------------------------------------------- 
*/ 

#ifndef _TARGET_
#define NEW_ENTITY
#endif

/*==== INCLUDES ===================================================*/

#ifdef _TARGET_
 #include "uart/serialswitch.h"
#endif

#include "nucleus.h"
#include "typedefs.h"
#include "os.h"
#include "vsi.h"
#include "os_types.h"
#include "pcon.h"
#include "p_mem.h"

/*==== CONSTANTS ==================================================*/

/*==== TYPES ======================================================*/

/*==== EXTERMALS ==================================================*/

#ifndef _TARGET_
extern void GpInitTarget (void);
extern void Cust_Init_Layer1 (void);
extern void GpUnmaskInterrupts (void);
extern void GpInitExternalDevices (void);
#endif

/*==== PROTOTYPES =================================================*/

short StartFrame (void);

/*==== VARIABLES ==================================================*/

#ifndef _TARGET_
extern T_PCON_PROPERTIES pcon_export;
T_PCON_PROPERTIES *pcon = &pcon_export;
#else
T_PCON_PROPERTIES *pcon = NULL;
#endif

T_MEM_PROPERTIES *mem = NULL;

/*==== FUNCTIONS ==================================================*/

#ifdef _TARGET_

void DummyCallback ( void )
{
}

#endif
/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6147)          MODULE  : GPRSINIT                |
| STATE   : code                   ROUTINE : Application_Initialize  |
+--------------------------------------------------------------------+

  PURPOSE : Main entry function for NUCLEUS

*/
/*
 * NOTE: Application_Initalize() must not be used when compiling for target,
 * because it is already defined in a TI lib (as of TI 5.1.1).
 */
#ifndef _TARGET_
void Application_Initialize (void *first_available_memory)
{

#ifdef _TARGET_
  GpInitTarget ();
  Cust_Init_Layer1 ();
  GpInitExternalDevices ();
  SER_tr_Init ( 0, 2, DummyCallback );
#endif

  StartFrame();

#ifdef _TARGET_
  GpUnmaskInterrupts ();
#endif
}
#endif /* !_TARGET_ */

/*
+----------------------------------------------------------------------+
| PROJECT : xxx                        MODULE  : GPRSINIT              |
| STATE   : code                       ROUTINE : InitializeApplication |
+----------------------------------------------------------------------+

  PURPOSE : General initialization function to be filled with
            application specific initializations. Function is
            called by the frame after creation of all tasks
            prior to the starting of the tasks.

*/
void InitializeApplication ( void )
{
  /*
   * It has to defined if the allocated partition memory shall be
   * initialized with a dedicated pattern. Select 
   * ENABLE_PARTITON_INIT or DISABLE_PARTITON_INIT
   * and a pattern to be used for initialization
   */
#ifdef _TARGET_
  vsi_m_init ( DISABLE_PARTITON_INIT, (char)0x00 );
#else
  vsi_m_init ( ENABLE_PARTITON_INIT, (char)0x00 );
#endif

#ifdef _TARGET_
  /*
   * The RTOS tick has to be set.Currently it can be set to
   * SYSTEM_TICK_TDMA_FRAME for the TDMA frame system tick of 4.615ms
   * or
   * SYSTEM_TICK_10_MS for the 10ms tick used for UMTS
   */
  os_set_tick ( SYSTEM_TICK_TDMA_FRAME );
#endif
}

