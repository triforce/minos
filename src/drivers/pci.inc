; =========================================================
; minos -- a hobby operating system written for x86-64
; Copyright (C) 2010-2015
;
; PCI Configuration
; =========================================================

; I/O locations

PCI_CONFIG_ADDRESS  equ     0xCF8
; 31        Enable Bbit - Set to 1 to start translation of config cycles to PCI_CONFIG_DATA
; 30 - 24   Reserved - Set to 0's
; 23 - 16   Bus number
; 15 - 11   Device number - Set the device on the PCI bus to select
; 10 - 8    Function number - Choose function on the device specified above
; 7 - 2     Register number - Offset into 256-byte config space, allows choice of 64 32-bit words
; 1 - 0     00 - Always leave as zero as register number writes need to be 32-bit aligned

PCI_CONFIG_DATA     equ     0xCFC

; TODO
; IN - EAX =  00000000 00000000 00000000 00000000
;             (Unused) Bus Num  Dev Num  Reg Num
; OUT - EAX = contains pci register information
drv_pci_read:
    push rbx rcx
    ret