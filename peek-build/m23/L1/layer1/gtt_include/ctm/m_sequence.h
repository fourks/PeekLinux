/*
*******************************************************************************
*
*      COPYRIGHT (C) 2000 BY ERICSSON EUROLAB DEUTSCHLAND GmbH
*      90411 NUERNBERG, GERMANY, Tel Int + 49 911 5217 100
*
*      The program(s) may be used and/or copied only with the
*      written permission from Ericsson or in accordance
*      with the terms and conditions stipulated in the agreement or
*      contract under which the program(s) have been supplied.
*
*******************************************************************************
*
*      File             : m_sequence.h
*      Purpose          : Calculation of m-sequences 
*                         (maximum-length sequecnes or pseudo noise)
*
*******************************************************************************
*/

#ifndef m_sequence_h
#define m_sequence_h "$Id: $"

/*
*******************************************************************************
*                         INCLUDE FILES
*******************************************************************************
*/


/*
*******************************************************************************
*                         DECLARATION OF PROTOTYPES
*******************************************************************************
*/

/* -------------------------------------------------------------------- */
/* function m_sequence()                                                */
/* *********************                                                */
/* Calculates one period of an m-sequence (binary pseudo noise).        */
/* The sequence is stored in the vector sequence, which must have a     */
/* of (2^r)-1, where r is an integer number between 2 and 10.           */
/* Therefore, with this release of m_sequence, sequences of length      */
/* 3, 7, 15, 31, 63, 127, 255, 511, or 1023 can be generated.           */
/* The resulting sequence is bipolar, i.e. it has values -1 and +1.     */
/* -------------------------------------------------------------------- */

void m_sequence(WORD16 *sequence, WORD16 length);


#endif


