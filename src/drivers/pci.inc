; =========================================================
; minos -- a hobby operating system written for x86-64
; Copyright (C) 2010-2015
;
; PCI Configuration
; =========================================================

; I/O locations

PCI_CONFIG_ADDRESS  equ     0xCF8
; 31        Enable bit - Set to 1 to start translation of config cycles to PCI_CONFIG_DATA
; 30 - 24   Reserved - Set to 0's
; 23 - 16   Bus number
; 15 - 11   Device number - Set the device on the PCI bus to select
; 10 - 8    Function number - Choose function on the device specified above
; 7 - 2     Register number - Offset into 256-byte config space, allows choice of 64 32-bit words
; 1 - 0     00 - Always leave as zero as register number writes need to be 32-bit aligned

PCI_CONFIG_DATA     equ     0xCFC

; ================================================
; drv_pci_read
; -------------
; IN - EAX =  00000000    00000000    00000000    00000000
;             (Unused)    Bus Num     Dev/Fun Num Reg Num
; OUT - EAX = contains pci register information
; ================================================
drv_pci_read:
	push rax rbx rdx rcx
	and eax, 0x00ffffff ; Set bits 24-31 to zero
	or eax, 0x80000000 ; Set bit 31
	mov dx, PCI_CONFIG_ADDRESS
	out dx, eax
	mov dx, PCI_CONFIG_DATA
	in eax, dx
	
	drv_done_pci_read:
		pop rax rbx rdx rcx
		ret
		
drv_pci_init:
	push rdx rcx rbx rax
	mov eax, 0x80000000
	mov ebx, eax
	mov dx, PCI_CONFIG_ADDRESS
	out dx, eax
	in eax, dx
	xor edx, edx
	cmp eax, ebx
	je drv_set_pci_init
	jmp drv_done_pci_init
	
	drv_set_pci_init:
		mov dl, 1
	
	drv_done_pci_init:
		mov byte [k_pci_enabled], dl
		pop rdx rcx rbx rax
		ret
		
	drv_pci_scan:
		push rdx rcx rbx rax

		xor rcx, rcx
		xor rax, rax
		mov ecx, 0x8000000 ; Set bit 31

	drv_pci_scan_next:
		mov eax, ecx
		mov dx, PCI_CONFIG_ADDRESS
		out dx, eax
		mov dx, PCI_CONFIG_DATA
		in eax, dx
		; If all bits are set in eax then no device exists, if not eax will contain the device/vendor id
		cmp eax, 0xffffffff
		je drv_pci_scan_not_present
		; Print eax
	
	drv_pci_scan_not_present:
		add ecx, 0x800
		cmp ecx, 0x81000000
		jne drv_pci_scan_next

	drv_done_pci_scan:
		pop rdx rcx rbx rax
		ret