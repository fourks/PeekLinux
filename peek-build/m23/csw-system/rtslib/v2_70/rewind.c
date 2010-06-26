/*****************************************************************************/
/*  REWIND.C v2.54                                                           */
/*  Copyright (c) 1995-2004 Texas Instruments Incorporated                   */
/*****************************************************************************/

/*****************************************************************************/
/* Functions:                                                                */
/*    REWIND   -  Position a stream's file pointer to the beginning of a     */
/*                file                                                       */
/*****************************************************************************/
#include <stdio.h>


/*****************************************************************************/
/* REWIND   -  Position a stream's file pointer to the beginning of a file   */
/*****************************************************************************/
_CODE_ACCESS void  rewind(register FILE *_fp) { fseek(_fp, 0L, SEEK_SET); }


