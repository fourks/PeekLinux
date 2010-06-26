;******************************************************************************
;* FD_TOULL16.ASM  - 16 BIT STATE -  v2.54                                    *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* FD$TOULL - CONVERT AN IEEE 754 FORMAT DOUBLE PRECISION FLOATING 
;*            INTO AN UNSIGNED 64 BIT INTEGER 
;****************************************************************************
;*
;*   o INPUT OP IS IN r0:r1
;*   o RESULT IS RETURNED IN r0:r1 (r1:r0 IF LITTLE ENDIAN)
;*
;*   o SIGNALLING NOT-A-NUMBER (SNaN) AND QUIET NOT-A-NUMBER (QNaN)
;*     ARE TREATED AS INFINITY
;*   o OVERFLOW RETURNS 0xFFFFFFFF:FFFFFFFF
;*   o UNDERFLOW RETURNS ZERO (0x00000000:00000000)
;*   o NEGATIVE VALUE RETURNS ZERO
;*   o ROUNDING MODE:  ROUND TO ZERO
;*
;****************************************************************************
;*
;* +------------------------------------------------------------------+
;* | DOUBLE PRECISION FLOATING POINT FORMAT                           |
;* |   64-bit representation                                          |
;* |   31 30      20 19                  0                            |
;* |   +-+----------+---------------------+                           |
;* |   |S|     E    |        M1           |                           |
;* |   +-+----------+---------------------+                           |
;* |                                                                  |
;* |   31                                0                            |
;* |   +----------------------------------+                           |
;* |   |             M2                   |                           |
;* |   +----------------------------------+                           |
;* |                                                                  |
;* |   <S>  SIGN FIELD    :          0 - POSITIVE VALUE               |
;* |                                 1 - NEGATIVE VALUE               |
;* |                                                                  |
;* |   <E>  EXPONENT FIELD: 0000000000 - ZERO IFF M == 0              |
;* |            0000000001..1111111110 - EXPONENT VALUE(1023 BIAS)    |
;* |                        1111111111 - INFINITY                     |
;* |                                                                  |
;* |   <M1:M2>  MANTISSA FIELDS:  FRACTIONAL MAGNITUDE WITH IMPLIED 1 |
;* +------------------------------------------------------------------+
;*
;****************************************************************************
	.state16

	.global	FD$TOULL, FD$TOLL

ope	.set	r2
tmp	.set	r3

FD$TOULL: .asmfunc stack_usage(12)
        PUSH	{r2-r3, lr}		; SAVE CONTEXT

	CMP	r0, #0			; CHECK FOR A NEGATIVE VALUE
        BPL     pos
        BL      FD$TOLL                 ; CAST TO INT
        POP     {r2, r3, pc}


pos:	LSL	ope, r0, #1		; PUT EXPONENT IN ope
	LSR	ope, ope, #21		;

	MOV	tmp, #0x3		;
	LSL	tmp, tmp, #8		;
	ADD	tmp, #0xFF		;
	SUB	ope, ope, tmp		; ADJUST FOR EXPONENT BIAS AND
	BCC	unfl			; CHECK FOR UNDERFLOW

	MOV	tmp, #0x3F		;
	SUB	ope, tmp, ope		; CHECK FOR OVERFLOW
	BCC	ovfl			; IF OVERFLOW, RETURN 0xFFFFFFFF

	LSL	r0, r0, #11		; PUT MANTISSA IN r0:r1
        LSR     tmp, r1, #21            ;
        ORR     r0, tmp                 ;
        LSL     r1, r1, #11             ;
	MOV	tmp, #0x1		;
	LSL	tmp, tmp, #31		;
	ORR	r0, tmp			; SET IMPLIED ONE IN HI MANTISSA

        ; COMPUTE THE INTEGER VALUE
        MOV     tmp, ope                ;
        SUB     tmp, #32                ; IF ope >= 32 
        BCC     $1                      ;
        MOV     r1, r0                  ;    r0:r1 = 0:r0 >> (ope - 32)
        MOV     r0, #0                  ;
	LSR	r1, tmp		        ;
	B       return                  ;
$1:
        LSR     r1, ope                 ; IF ope < 32
        NEG     tmp, tmp                ;    
        MOV     lr, r0                  ;    r0:r1 = r0:r1 >> ope
        LSL     r0, tmp                 ;
        ORR     r1, r0                  ;
        MOV     r0, lr                  ;
        LSR     r0, ope                 ;

	; IN LITTLE ENDIAN MODE THE OUTPUT LONG LONG VALUE SHOULD BE IN R1:R0.
	; SO SWAP THE REGISTER VALUES BEFORE RETURN.
return:
	.if .TMS470_LITTLE
	MOV	lr, r0			;
	MOV	r0, r1			;
	MOV	r1, lr			;
	.endif

	POP	{r2-r3, pc}		;

unfl:   MOV     r0, #0                  ; UNDERFLOW
        MOV     r1, #0                  ;
	POP	{r2-r3, pc}		;
 
ovfl:	MOV	r0, #0x0		; IF OVERFLOW, 
	SUB	r0, r0, #0x1		; RETURN 0xFFFFFFFF:FFFFFFFF
        MOV     r1, r0                  ;
	POP	{r2-r3, pc}		;
	
	.endasmfunc
	.end
