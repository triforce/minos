; =========================================================
; minos -- a hobby operating system written for x86-64
; Copyright (C) 2010-2015
;
; RTL8169 Network Driver
; =========================================================

RTL8169_CMD_REG 	equ 0x37 ; Command Register
RTL8169_IMR_REG 	equ 0x3C ; Interrupt Mask Register
RTL8169_ISR_REG         equ 0x3E ; Interrupt Status Register
RTL8169_CFG1_REG        equ 0x52 ; Configuration Register 1

; ===============================
; drv_net_rtl8169_init
; ------------
; Initialise an RTL network card 
; IN / OUT - None 
; ===============================

drv_net_rtl8169_init:
	push rax rcx rdx
	; Check device present
	cmp [k_pci_net_device], 0
	je drv_net_rtl8169_init_done
	xor eax, eax
	
	; Get IRQ line
	mov eax, [k_pci_net_device]
	mov al, 0x3c
	call drv_pci_read
	mov [k_pci_net_device_irq], al

	; Get base i/o address
        mov eax, [k_pci_net_device]
        mov al, 0x10
        call drv_pci_read
	mov [k_pci_net_device_ioaddr], ax

	; Turn on device
	mov dx, [k_pci_net_device_ioaddr]
	add dx, RTL8169_CFG1_REG
        mov al, 0x10
	out dx, al

	; Reset device
	mov dx, [k_pci_net_device_ioaddr]
	add dx, RTL8169_CMD_REG
	mov al, 0x10
	out dx, al
	mov cx, 1000

	drv_net_rtl8169_init_wait_reset:
		in al, dx
		test al, 0x10
		jz drv_net_rtl8169_init_done
		dec cx
		jns drv_net_rtl8169_init_wait_reset

	drv_net_rtl8169_init_done:
		pop rax rcx rdx
		ret
