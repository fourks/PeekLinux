;******************************************************************************
;* LL_DIV32.ASM  - 32 BIT STATE -  v2.54                                      *
;* Copyright (c) 1996-2004 Texas Instruments Incorporated                     *
;******************************************************************************

;****************************************************************************
;* LL_DIV/LL_MOD - DIVIDE TWO SIGNED 64 BIT NUMBERS.
;*  
;****************************************************************************
;*
;*   o DIVIDEND IS IN r0:r1  (r1:r0 IN LITTLE ENDIAN MODE)
;*   o DIVISOR IS IN r2:r3   (r3:r2 IN LITTLE ENDIAN MODE)
;*
;*   o QUOTIENT IS PLACED IN r2:r3   (r3:r2 IN LITTLE ENDIAN MODE)
;*   o REMAINDER IS PLACED IN r0:r1  (r1:r0 IN LITTLE ENDIAN MODE)
;*
;*   o DIVIDE BY ZERO RETURNS ZERO
;*   o SIGN OF REMAINDER IS THE SIGN OF THE DIVIDEND
;*
;****************************************************************************
	.state32

	.global LL_DIV
	.global LL_MOD

	.if .TMS470_LITTLE

dvnd_hi .set    r1                                  ; HIGH WORD OF DIVIDEND
dvnd_lo .set    r0                                  ; LOW WORD OF DIVIDEND
idvs_hi .set    r3				    ; HIGH WORD OF DIVISOR
idvs_lo .set    r2				    ; LOW WORD OF DIVISOR
rq_hi   .set    r3			  	    ; HIGH WORD OF RESULT QUO
rq_lo   .set    r2			  	    ; LOW WORD OF RESULT QUO
rr_hi   .set    r1				    ; HIGH WORD OF RESULT REM
rr_lo   .set    r0				    ; HIGH WORD OF RESULT REM

	.else

dvnd_hi .set    r0                                  ; HIGH WORD OF DIVIDEND
dvnd_lo .set    r1                                  ; LOW WORD OF DIVIDEND
idvs_hi .set    r2				    ; HIGH WORD OF DIVISOR
idvs_lo .set    r3				    ; LOW WORD OF DIVISOR
rq_hi   .set    r2			  	    ; HIGH WORD OF RESULT QUO
rq_lo   .set    r3			  	    ; LOW WORD OF RESULT QUO
rr_hi   .set    r0				    ; HIGH WORD OF RESULT REM
rr_lo   .set    r1				    ; HIGH WORD OF RESULT REM

	.endif


q_hi    .set    LR                                  ; HIGH WORD OF QUOTIENT
q_lo    .set    r9                                  ; LOW WORD OF QUOTIENT
dvsr_hi .set    r4                                  ; HIGH WORD OF DIVISOR COPY
dvsr_lo .set    r5                                  ; LOW WORD OF DIVISOR COPY
tmp_hi  .set    r6                                  ; HIGH WORD OF TEMP
tmp_lo  .set    r7                                  ; LOW WORD OF TEMP
sign    .set    r8                                  ; SAVED COPY OF SIGNS

LL_DIV:	 .asmfunc stack_usage(28)
LL_MOD:
        STMFD     SP!, {r4-r9, lr}                  ;

        ; STORE THE SIGN OF REMAINDER WHICH IS THE SIGN OF THE DIVIDEND IN 
        ; BIT 31 OF SIGN. AND STORE THE SIGN OF THE QUOTIENT WHICH IS SIGN
        ; OF DIVISOR XORED WITH SIGN OF DIVIDEND IN BIT 30 OF SIGN.

        MOV       sign, idvs_hi, LSR #1             ; 
        EORS      sign, sign, dvnd_hi, ASR #1       ; 
        BPL       $pos_dvnd                         ; 
        RSBS      dvnd_lo, dvnd_lo, #0              ; DVND = ABS(DVND)
        RSC       dvnd_hi, dvnd_hi, #0              ;
$pos_dvnd:
        MOV       q_lo, #0                          ; INITIALIZE THE QUOTIENT
        ANDS      q_hi, idvs_hi, #0x80000000        ;
        BEQ       $pos_dvsr                         ;
        RSBS      idvs_lo, idvs_lo, #0              ; DVSR = ABS(DVSR)
        RSC       idvs_hi, idvs_hi, #0              ;
$pos_dvsr:
        MOV       dvsr_hi, idvs_hi                  ; MAKE A COPY OF THE   
        MOV       dvsr_lo, idvs_lo                  ; DIVISOR (R2:R3) INTO DVSR

        CMP       dvsr_hi, #0                       ; IF DVSR IS 0,
        CMPEQ     dvsr_lo, #0                       ; RETURN Q=0, R=0 
        MOVEQ     rr_lo, #0                         ;
        MOVEQ     rr_hi, #0                         ;
        LDMEQFD   SP!, {r4-r9, pc}                  ;

        ; CALCULATE THE MAXIMUM DIVISOR SHIFT AMOUNT WITH PSEUDO BINARY SEARCH
        ; IF DVND >> 32 > DVSR THEN DVSR = DVSR << 32

        CMP       dvsr_hi, #0                       ; IF (DVSR_HI == 0 AND 
        CMPEQ     dvsr_lo, dvnd_hi                  ;          DVSRLO < DVND_HI)
        MOVLS     dvsr_hi, dvsr_lo                  ;    DVSR_HI = DVSRLO
        MOVLS     dvsr_lo, #0                       ;    DVSR_LO = 0

        ; IF DVND >> 16 > DVSR THEN DVSR = DVSR << 16
        MOV       tmp_lo, dvnd_lo, LSR #16          ; SHIFT DVND BY 16 INTO
        ORR       tmp_lo, tmp_lo, dvnd_hi, LSL #16  ; TMP                   
        MOV       tmp_hi, dvnd_hi, LSR #16          ; 
        CMP       dvsr_hi, tmp_hi                   ; COMPARE DVSR WITH TMP
        CMPEQ     dvsr_lo, tmp_lo                   ;
        MOVLS     dvsr_hi, dvsr_hi, LSL #16         ; SET DVSR = DVSR << 16
        ORRLS     dvsr_hi, dvsr_hi, dvsr_lo, LSR #16;
        MOVLS     dvsr_lo, dvsr_lo, LSL #16         ;

        ; IF DVND >> 16 > DVSR THEN DVSR = DVSR << 16
        MOV       tmp_lo, dvnd_lo, LSR #8           ; SHIFT DVND BY 16 INTO
        ORR       tmp_lo, tmp_lo, dvnd_hi, LSL #24  ; TMP
        MOV       tmp_hi, dvnd_hi, LSR #8           ; 
        CMP       dvsr_hi, tmp_hi                   ; COMPARE DVSR WITH TMP
        CMPEQ     dvsr_lo, tmp_lo                   ;
        MOVLS     dvsr_hi, dvsr_hi, LSL #8          ; SET DVSR = DVSR << 8
        ORRLS     dvsr_hi, dvsr_hi, dvsr_lo, LSR #24;
        MOVLS     dvsr_lo, dvsr_lo, LSL #8          ;

        ; NOW FIND EXACTLY WHERE THE SHIFTED DIVISOR SHOULD BE SO THAT WE CAN
        ; JUMP INTO THE CORRECT LOCATION OF THE UNROLLED DIVIDE LOOP.
        MOV       tmp_lo, dvnd_lo, LSR #1           ;
        ORR       tmp_lo, tmp_lo, dvnd_hi, LSL #31  ;
        MOV       tmp_hi, dvnd_hi, LSR #1           ;
        CMP       dvsr_hi, tmp_hi                   ;
        CMPEQ     dvsr_lo, tmp_lo                   ;
        BHI       mod1                              ;

        MOV       tmp_lo, dvnd_lo, LSR #2           ;
        ORR       tmp_lo, tmp_lo, dvnd_hi, LSL #30  ;
        MOV       tmp_hi, dvnd_hi, LSR #2           ;
        CMP       dvsr_hi, tmp_hi                   ;
        CMPEQ     dvsr_lo, tmp_lo                   ;
        BHI       mod2                              ; BRANCHING INTO DIVIDE LOOP

        MOV       tmp_lo, dvnd_lo, LSR #3           ;
        ORR       tmp_lo, tmp_lo, dvnd_hi, LSL #29  ;
        MOV       tmp_hi, dvnd_hi, LSR #3           ;
        CMP       dvsr_hi, tmp_hi                   ;
        CMPEQ     dvsr_lo, tmp_lo                   ;
        BHI       mod3                              ;

        MOV       tmp_lo, dvnd_lo, LSR #4           ;
        ORR       tmp_lo, tmp_lo, dvnd_hi, LSL #28  ;
        MOV       tmp_hi, dvnd_hi, LSR #4           ;
        CMP       dvsr_hi, tmp_hi                   ;
        CMPEQ     dvsr_lo, tmp_lo                   ;
        BHI       mod4                              ;

        MOV       tmp_lo, dvnd_lo, LSR #5           ;
        ORR       tmp_lo, tmp_lo, dvnd_hi, LSL #27  ;
        MOV       tmp_hi, dvnd_hi, LSR #5           ;
        CMP       dvsr_hi, tmp_hi                   ;
        CMPEQ     dvsr_lo, tmp_lo                   ;
        BHI       mod5                              ;

        MOV       tmp_lo, dvnd_lo, LSR #6           ;
        ORR       tmp_lo, tmp_lo, dvnd_hi, LSL #26  ;
        MOV       tmp_hi, dvnd_hi, LSR #6           ;
        CMP       dvsr_hi, tmp_hi                   ;
        CMPEQ     dvsr_lo, tmp_lo                   ;
        BHI       mod6                              ;

        MOV       tmp_lo, dvnd_lo, LSR #7           ;
        ORR       tmp_lo, tmp_lo, dvnd_hi, LSL #25  ;
        MOV       tmp_hi, dvnd_hi, LSR #7           ;
        CMP       dvsr_hi, tmp_hi                   ;
        CMPEQ     dvsr_lo, tmp_lo                   ;
        BHI       mod7                              ;

        ; THE DIVIDE LOOP IS UNROLLED 8 TIMES.
        ; IF DIVIDEND IS LARGER THAN DIVISOR, SHIFT A 1 INTO THE QUOTIENT
        ; AND SUBTRACT THE DIVISOR, ELSE SHIFT A 0 INTO THE QUOTIENT.
divll:
        MOV       tmp_hi, dvsr_hi, LSL #7           ; LEFT SHIFT DVSR BY 7 
        ORR       tmp_hi, tmp_hi, dvsr_lo, LSR #25  ; INTO TMP
        MOV       tmp_lo, dvsr_lo, LSL #7           ;
        CMP       dvnd_hi, tmp_hi                   ; IF (DVND >= TMP)      
        CMPEQ     dvnd_lo, tmp_lo                   ; 
        BCC       $1                                ; 
        SUBS      dvnd_lo, dvnd_lo, tmp_lo          ;    DVND = DVND - TMP
        SBCS      dvnd_hi, dvnd_hi, tmp_hi          ;
$1:
        ADCS      q_lo, q_lo, q_lo                  ; SHIFT THE CARRY BIT DEFED
        ADC       q_hi, q_hi, q_hi                  ; BY CMP OR SBCS INTO THE 
                                                    ; QUOTIENT

mod7:
        MOV       tmp_hi, dvsr_hi, LSL #6           ; UNROLLED LOOP 
        ORR       tmp_hi, tmp_hi, dvsr_lo, LSR #26  ;
        MOV       tmp_lo, dvsr_lo, LSL #6           ;
        CMP       dvnd_hi, tmp_hi                   ;
        CMPEQ     dvnd_lo, tmp_lo                   ;

        BCC       $2                                ;
        SUBS      dvnd_lo, dvnd_lo, tmp_lo          ;
        SBCS      dvnd_hi, dvnd_hi, tmp_hi          ;
$2:
        ADCS      q_lo, q_lo, q_lo                  ;
        ADC       q_hi, q_hi, q_hi                  ;
        
mod6:
        MOV       tmp_hi, dvsr_hi, LSL #5           ;
        ORR       tmp_hi, tmp_hi, dvsr_lo, LSR #27  ;
        MOV       tmp_lo, dvsr_lo, LSL #5           ;
        CMP       dvnd_hi, tmp_hi                   ;
        CMPEQ     dvnd_lo, tmp_lo                   ;

        BCC       $3                                ;
        SUBS      dvnd_lo, dvnd_lo, tmp_lo          ;
        SBCS      dvnd_hi, dvnd_hi, tmp_hi          ;
$3:
        ADCS      q_lo, q_lo, q_lo                  ;
        ADC       q_hi, q_hi, q_hi                  ;
        
mod5:
        MOV       tmp_hi, dvsr_hi, LSL #4           ;
        ORR       tmp_hi, tmp_hi, dvsr_lo, LSR #28  ;
        MOV       tmp_lo, dvsr_lo, LSL #4           ;
        CMP       dvnd_hi, tmp_hi                   ;
        CMPEQ     dvnd_lo, tmp_lo                   ;

        BCC       $4                                ;
        SUBS      dvnd_lo, dvnd_lo, tmp_lo          ;
        SBCS      dvnd_hi, dvnd_hi, tmp_hi          ;
$4:
        ADCS      q_lo, q_lo, q_lo                  ;
        ADC       q_hi, q_hi, q_hi                  ;
        
mod4:
        MOV       tmp_hi, dvsr_hi, LSL #3           ;
        ORR       tmp_hi, tmp_hi, dvsr_lo, LSR #29  ;
        MOV       tmp_lo, dvsr_lo, LSL #3           ;
        CMP       dvnd_hi, tmp_hi                   ;
        CMPEQ     dvnd_lo, tmp_lo                   ;

        BCC       $5                                ;
        SUBS      dvnd_lo, dvnd_lo, tmp_lo          ;
        SBCS      dvnd_hi, dvnd_hi, tmp_hi          ;
$5:
        ADCS      q_lo, q_lo, q_lo                  ;
        ADC       q_hi, q_hi, q_hi                  ;
        
mod3:
        MOV       tmp_hi, dvsr_hi, LSL #2           ;
        ORR       tmp_hi, tmp_hi, dvsr_lo, LSR #30  ;
        MOV       tmp_lo, dvsr_lo, LSL #2           ;
        CMP       dvnd_hi, tmp_hi                   ;
        CMPEQ     dvnd_lo, tmp_lo                   ;

        BCC       $6                                ;
        SUBS      dvnd_lo, dvnd_lo, tmp_lo          ;
        SBCS      dvnd_hi, dvnd_hi, tmp_hi          ;
$6:
        ADCS      q_lo, q_lo, q_lo                  ;
        ADC       q_hi, q_hi, q_hi                  ;
        
mod2:
        MOV       tmp_hi, dvsr_hi, LSL #1           ;
        ORR       tmp_hi, tmp_hi, dvsr_lo, LSR #31  ;
        MOV       tmp_lo, dvsr_lo, LSL #1           ;
        CMP       dvnd_hi, tmp_hi                   ;
        CMPEQ     dvnd_lo, tmp_lo                   ;

        BCC       $7                                ;
        SUBS      dvnd_lo, dvnd_lo, tmp_lo          ;
        SBCS      dvnd_hi, dvnd_hi, tmp_hi          ;
$7:
        ADCS      q_lo, q_lo, q_lo                  ;
        ADC       q_hi, q_hi, q_hi                  ;

mod1:
        CMP       dvnd_hi, dvsr_hi                  ;
        CMPEQ     dvnd_lo, dvsr_lo                  ;
        
        BCC       $8                                ;
        SUBS      dvnd_lo, dvnd_lo, dvsr_lo         ;
        SBCS      dvnd_hi, dvnd_hi, dvsr_hi         ;
$8:
        ADCS      q_lo, q_lo, q_lo                  ;
        ADC       q_hi, q_hi, q_hi                  ;

        ; IF THERE IS SHIFTED DIVISOR, THEN UNSHIFT THE DIVISOR BY 8 AND
        ; CONTINUE THE LOOP
        CMP       idvs_hi, dvsr_hi                  ; 
        CMPEQ     idvs_lo, dvsr_lo                  ;
        MOVCC     dvsr_lo, dvsr_lo, LSR #8          ;
        ORRCC     dvsr_lo, dvsr_lo, dvsr_hi, LSL #24;
        MOVCC     dvsr_hi, dvsr_hi, LSR #8          ;
        BCC       divll                             ;

        ; ELSE WE ARE DONE. 
	; PLACE THE QUOTIENT INTO rq_hi:rq_lo AND SET THE SIGN.
        ; rr_hi:rr_lo ALREADY CONTAINS THE REMAINDER; SET THE CORRECT SIGN.
        MOV       rq_hi, q_hi                       ;
        MOV       rq_lo, q_lo                       ;
        MOVS      sign, sign                        ;
        BPL       $chk_q_sign                       ;
        RSBS      rr_lo, rr_lo, #0                  ;
        RSC       rr_hi, rr_hi, #0                  ;
$chk_q_sign:
        MOVS      sign, sign, LSL #1                ;
        BPL       return                            ;
        RSBS      rq_lo, rq_lo, #0                  ;
        RSC       rq_hi, rq_hi, #0                  ;
return:

        LDMFD   SP!, {r4-r9, pc}                    ;
	.endasmfunc
        .end

