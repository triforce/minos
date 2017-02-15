; =========================================================
; minos -- a hobby operating system written for x86-64
; Copyright (C) 2010-2017
;
; Multiboot Loader
; =========================================================

format binary

org 1000h
use32

mbootHeader:
	dd 0x1badb002 ; Multiboot magic
	dd 0x00010003 ; Flags
	dd 0-(0x1badb002+0x00010003) ; Checksum

	dd mbootHeader ; Header address
	dd 1000h ; Kernel start address
	dd k_end ; Kernel end address
	dd 0 ; BSS end address
	dd k_start ; Entry point

LONG_CHECK	equ 0x80000001
LONG_TEST	equ 0x20000000

k_start:
	xor eax, eax
	; Check if CPU long mode capable
	mov eax, LONG_CHECK
	cpuid

	test edx, LONG_TEST
	jnz start_long

	jmp k_end

start_long:
	hlt

k_end:
	mov eax, 0x0753074e
	mov [0xb8000], eax
	hlt
