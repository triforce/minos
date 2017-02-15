; =========================================================
; minos -- a hobby operating system written for x86-64
; Copyright (C) 2010-2016
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

; ========================================================
; drv_pci_read
; ------------
; IN - EAX =  00000000    00000000    00000000    00000000
;             (Unused)    Bus Num     Dev/Fun Num Reg Num
; OUT - EAX = contains pci register information
; ========================================================

drv_pci_read:
	push rbx rdx rcx
	and eax, 0x00ffffff ; Set bits 24-31 to zero
	or eax, 0x80000000 ; Set bit 31
	mov dx, PCI_CONFIG_ADDRESS
	out dx, eax
	mov dx, PCI_CONFIG_DATA
	in eax, dx
	
	drv_done_pci_read:
		pop rbx rdx rcx
		ret

; ==============================================
; drv_pci_init
; ------------
; IN  - None
; OUT - [k_pci_enabled] = Set to 1 if successful
; ==============================================

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
	mov dl, 0
	jmp drv_done_pci_init
	
	drv_set_pci_init:
		mov dl, 1
	
	drv_done_pci_init:
		mov byte [k_pci_enabled], dl
		pop rdx rcx rbx rax
		ret

; ===========================================================
; drv_pci_scan
; ------------
; Scan for PCI devices and output hex format of device/vendor	
; IN - al = 0/1 whether to configure devices found or not
; OUT - None
; ===========================================================

drv_pci_scan:
	push rdx rcx rbx rax
	mov [k_var_1], al
	xor rcx, rcx
	xor rax, rax
	mov ecx, 0x80000000
	mov dword [k_pci_devices], 0
	mov dword [k_pci_device], 0

	drv_pci_scan_next:
		mov eax, ecx
		mov dx, PCI_CONFIG_ADDRESS
		out dx, eax
		mov dx, PCI_CONFIG_DATA
		in eax, dx
		; If all bits are set in eax then no device exists, if not eax will contain the device/vendor
		cmp eax, 0xffffffff
		je drv_pci_scan_not_present
		
		; Keep a count of devices
		inc dword [k_pci_devices]

		mov [k_pci_device], eax
		push rcx
		
		; Check whether to configure drivers
		cmp byte [k_var_1], 0
		je drve_found_continue

		; Check for supported network driver
                mov ah, byte [k_pci_device+3]
                mov al, byte [k_pci_device+2]
                cmp ax, k_pci_rtl8139
		je drv_found_network_rtl 
		cmp ax, k_pci_rtl8169
		je drv_found_network_rtl
		cmp ax, k_pci_i82557b
		je drv_found_network_int
                cmp ax, k_pci_i82551
                je drv_found_network_int
	        cmp ax, k_pci_i82559er
		je drv_found_network_int
	
	drve_found_continue:
		; Output device / vendor
		mov al, byte [k_pci_device+3]
		call sys_hex_print
		mov al, byte [k_pci_device+2]
		call sys_hex_print
		mov al, byte [k_pci_device+1]
		call sys_hex_print
		mov al, byte [k_pci_device+0]
		call sys_hex_print
		mov esi, '-'
		mov ecx, 1
		call sys_character_print
		pop rcx

	drv_pci_scan_not_present:
		add ecx, 0x800
		cmp ecx, 0x81000000
		jne drv_pci_scan_next

	drv_done_pci_scan:
		pop rdx rcx rbx rax
		ret

	drv_found_network_rtl:
		push rax rcx
		mov [k_pci_net_device], ecx
		mov eax, [k_pci_net_device]
		call drv_net_rtl8169_init
		pop rax rcx
		jmp drve_found_continue

	drv_found_network_int:
		push rax rcx
		mov [k_pci_net_device], ecx
		mov eax, [k_pci_device]
		call drv_net_i8254_init
		pop rax rcx
		jmp drve_found_continue
