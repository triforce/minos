; =========================================================
; minos -- a hobby operating system written for x86-64
; Copyright (C) 2010-2016
;
; Memory Map
; =========================================================

; 0x0000000000000000 - 0x0000000000000FFF - idt - 4096 bytes
; 0x0000000000001000 - 0x0000000000001060 - gdt - 96 bytes
; 0x0000000000001200 - 0x00000000000031FF - kernel - 8192 bytes
; 0x0000000000003200 - 0x0000000000003FFF - kernel stack - 3584 bytes
; 0x0000000000004000 - 0x00000000000047FF - system vars - 2048 bytes
; 0x0000000000004800 - 0x0000000000006FFF - memory map - 10240 bytes (can map up to 40 MiB)
; 0x0000000000007000 - 0x0000000000007600 - pdp entries
; 0x0000000000100000 - 0x00000000001FFFFF - additional space for kernel
; 0x0000000000200000 - 0x0000000000300000 - free memory
