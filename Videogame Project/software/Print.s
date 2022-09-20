; Print.s
; Ethan Litchauer and Kevin Tong
; Last modification date: change this to the last modification date or look very silly
; Runs on TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; SSD1306_OutChar   outputs a single 8-bit ASCII character
; SSD1306_OutString outputs a null-terminated string 

    IMPORT   SSD1306_OutChar
    IMPORT   SSD1306_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix
    PRESERVE8
    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB

;symbolic binding
CNT EQU 0
N EQU 4
FP RN 11

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutDec
		;INIT 1
		STR FP, [SP]		;push R11 first on stack as per AASCP
		SUB SP, SP, #4		;allocation for R0
		STR R0, [SP]		;R0 (#N) next
		SUB SP, SP, #4		;allocation for CNT
		ADD FP, SP, #0		;make R11 the stack pointer (where CNT will be)
		SUB SP, SP, #4		;allocation for LR
		;make FP = SP
		
		;INIT 2
		STR LR, [SP]		;store LR on stack
		MOV R3, #0			
		STR R3, [FP, #CNT]	;0 in for R3, then store for CNT
		MOV R3, #10			;used to divide/multiply by 10 later
	
readLoop
		;#1
		LDR R0, [FP,#CNT]	
		ADD R0, R0, #1
		STR R0, [FP,#CNT]	;adds one to CNT for every digit tested
		
		;#2
		LDR R1, [FP, #N]
		ADD R2, R1, #0
		UDIV R0, R1, R3		;divide N by 10, deleting ones digit and moving all others over to the right
		STR R0, [FP, #N]	;store new number back into N
		MUL R1, R0, R3
		SUB R0, R2, R1
		SUB SP, SP, #4		;allocation
		STR R0, [SP]		;store ones digit onto the stack
	
		;#3
		LDR R0, [FP, #N]
		CMP R0, #0			;check whether the ones digit is zero
		BEQ writeLoop		;display all characters on stack
		B readLoop			;keep reading the N
	
writeLoop
		;#4
		LDR R0,[SP]			;load N number to display
		ADD SP, SP, #4		;deallocate
		ADD R0, R0, #0x30	;turn into ASCII
		BL SSD1306_OutChar	;display
		
		;#5
		LDR R3, [FP, #CNT]
		SUB R3, R3, #1
		STR R3, [FP, #CNT]	;decrement countdown after each display
		
		;#6
		CMP R3, #0			;if countdown is zero, wrap it up. Otherwise, keep displaying
		BEQ continue
		B writeLoop
	
continue
		;#7
		LDR LR, [SP]		;pop LR to return to previous program
		ADD SP, SP, #8
		LDR FP, [SP]		;pop R11 as per AASCP
		BX  LR
;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.01, range 0.00 to 9.99
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.00 "
;       R0=3,    then output "0.03 "
;       R0=89,   then output "0.89 "
;       R0=123,  then output "1.23 "
;       R0=999,  then output "9.99 "
;       R0>999,  then output "*.** "
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutFix
		;INIT 1 (same steps as OUTDEC)
		STR FP, [SP]
		SUB SP, SP, #4
		STR R0, [SP]
		SUB SP, SP, #4
		ADD FP, SP, #0
		SUB SP, SP, #4
		;make FP = SP
		
		;INIT 2 (same steps as OUTDEC)
		STR LR, [SP] 		;store LR on stack
		MOV R3, #0
		STR R3, [FP, #CNT]
		MOV R3, #10
		
		;#1
		LDR R2, [FP, #N]
		CMP R2, #1000
		BLO range			;if greater than 1000, display asterisk symbols. Otherwise, read for floating point
		
		;#2
		MOV R0, #0x2A		;asterisk 1
		BL SSD1306_OutChar
		
		;#3
		MOV R0, #0x2E		;decimal point
		BL SSD1306_OutChar
		
		;#4
		MOV R0, #0x2A		;asterisk 2
		BL SSD1306_OutChar
		
		;#5
		MOV R0, #0x2A		;asterisk 3
		BL SSD1306_OutChar
		B exit
		
range	;#6 for OUTFIX, #1 from OUTDEC
		LDR R0, [FP,#CNT]
		ADD R0, R0, #1
		STR R0, [FP,#CNT]
		
		;#7 for OUTFIX, #2 from OUTDEC
		LDR R1, [FP, #N]
		ADD R2, R1, #0
		UDIV R0, R1, R3
		STR R0, [FP, #N]
		MUL R1, R0, R3
		SUB R0, R2, R1
		SUB SP, SP, #4
		STR R0, [SP]

		;#8
		LDR R1, [FP, #CNT]
		CMP R1, #3
		BLO range			;check if number can be converted into floating point with ones, tenths and hundredths places
		
		;#9
		LDR R0,[SP]
		ADD SP, SP, #4
		ADD R0, R0, #0x30
		BL SSD1306_OutChar	;first character
		
		MOV R0, #0x2E
		BL SSD1306_OutChar	;decimal point
		
		LDR R0,[SP]
		ADD SP, SP, #4
		ADD R0, R0, #0x30
		BL SSD1306_OutChar	;second character
		
		LDR R0,[SP]
		ADD SP, SP, #4
		ADD R0, R0, #0x30
		BL SSD1306_OutChar	;third character

exit	;#10 (same as OUTDEC #7)
		LDR LR, [SP]
		ADD SP, SP, #8
		LDR FP, [SP]
		BX   LR
		ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *

		ALIGN          ; make sure the end of this section is aligned
		END            ; end of file
