; =========================================================
; minos -- a hobby operating system written for x86-64
; Copyright (C) 2010-2015
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
dd 3200h ; Kernel end address
dd 0 ; BSS end address
dd k_start ; Entry point

k_start:
cli
mov eax, cr0
xor eax, 0x80000000
mov cr0, eax

xor eax, eax
mov cr3, eax

mov eax, cr0
xor eax, 1
mov cr0, eax

jmp 0000:start_real

use16

start_real:
; Head to the main kernel (should already be in memory)
jmp 0000:1200h
