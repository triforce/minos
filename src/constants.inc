; =========================================================
; minos -- a hobby operating system written for x86-64
; Copyright (C) 2010-2015
;
; Kernel constants
; ==========================================================

CR0_PAGING  equ 0x80000000
LONG_CHECK  equ 0x80000001
LONG_TEST   equ 0x20000000

R0_NULL_SELECTOR = 0
R0_DATA_SELECTOR = 1 shl 3
R0_CODE_SELECTOR = 2 shl 3
R0_LONG_SELECTOR = 3 shl 3

; Task States
NULL_T		equ (0)
FREE_T		equ (1)
WAIT_T		equ (2)
EXEC_T		equ (3)

; Task Memory
TOTAL_T		equ (10)
STACK_T		equ (128)

; Carriage Return
crlf		equ 0dh, 0ah