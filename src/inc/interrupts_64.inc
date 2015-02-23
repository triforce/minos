; =========================================================
; minos -- a hobby operating system written for x86-64
; Copyright (C) 2010-2015
;
; Interrupt Handlers
; ==========================================================

IDTR:
	dw 256*16-1
	dq 0	; linear address

exception_gate:
	dw exception and 0FFFFh,R0_LONG_SELECTOR
	dw 8E00h,exception shr 16
	dd 0,0

interrupt_gate:
	dw interrupt and 0FFFFh,R0_LONG_SELECTOR
	dw 8F00h,interrupt shr 16
	dd 0,0

exception:
	in al, 61h
	or al, 3
	out 61h, al
	jmp exception

interrupt:
	iretq

irq_0_handler_clock:
	push rax
	inc [k_clocks]
	mov al, 20h
	out 20h, al
	out 0A0h, al
	pop rax
	iretq

irq_1_handler_keyboard:
	push rax
	call start_kb
	in al, 61h
	out 61h, al
	mov al, 20h
	out 20h, al
	out 0A0h, al
	pop rax
	iretq
	
irq_9_pci_device:
	push rax
	; TODO
	mov al, 20h
	out 20h, al
	out 0A0h, al
	pop rax
	iretq

reboot:
	mov al, 0FEh
	out 64h, al
	jmp reboot
