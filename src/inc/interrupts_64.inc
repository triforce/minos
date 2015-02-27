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

; Handle Programmable Interval Timer (PIT) channel 0, IRQ fires every 54ms roughly
irq_0_handler_clock:
	push rax
	inc [k_pit_clocks]
	
	; Flash a 0/1 in the top right corner to show clock ticking
	mov rax, [k_pit_clocks]
	and al, 1
	add al, 48
	mov [0x000B809A], al
	
	; If we have a wait action then handle it
	push rbx
	mov rbx, [k_countdown]
	
	handle_timer:
		or rbx, rbx
		jz done_handle_timer
		dec rbx
		mov [k_countdown], rbx
		
	done_handle_timer:
		pop rbx
	
	mov al, 20h
	out 20h, al
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
	
irq_8_rtc:
	push rdi rax
	add qword [k_clocks], 1
	mov al, 20h
	out 20h, al
	pop rdi rax
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
