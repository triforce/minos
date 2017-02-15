; =========================================================
; minos -- a hobby operating system written for x86-64
; Copyright (C) 2010-2016
;
; Intel i8254x Network Driver
; =========================================================

; ======================================
; drv_net_i8254_init
; Initialise an Intel i8254 network card
; ------------
; IN / OUT - None
; ======================================

drv_net_i8254_init:
        push rax rcx rdx
        ; Check device present
        cmp dword [k_pci_net_device], 0
	je drv_net_i8254_init_done
        xor eax, eax

        ; Get IRQ line
        mov eax, [k_pci_net_device]
        mov al, 0x3c
        call drv_pci_read
        mov [k_pci_net_device_irq], al

	drv_net_i8254_init_done:
		pop rax rcx rdx
