; =========================================================
; minos -- a hobby operating system written for x86-64
; Copyright (C) 2010-2016
;
; Interrupt Handlers
; ==========================================================

IDTR:
	dw 256*16-1
	dq 0	; linear address

exception_gate:
	dw exception and 0FFFFh, R0_CODE_SELECTOR
	dw 8E00h,exception shr 16
	dd 0,0

interrupt_gate:
	dw interrupt and 0FFFFh, R0_CODE_SELECTOR
	dw 8F00h,interrupt shr 16
	dd 0,0

ring3_interrupt_gate:
	dw interrupt and 0FFFFh, R0_CODE_SELECTOR
	dw 1110111100000000b,interrupt shr 16
	dd 0,0

exception:
	in al, 61h
	or al, 3
	out 61h, al
	jmp exception

interrupt:
	iretq

; Handle Programmable Interval Timer (PIT) channel 0
irq_0_handler_clock:
	; pop rax ; rax contains RIP of task
	; --------------------
	; Debug Register - R14
	; --------------------
	mov rax, [rsp+0] ; RIP
	mov r8, rax
	mov rax, [rsp+24] ; RSP
	mov r9, rax
	push rax ; Save current state
	mov rax, [k_pit_clocks]
	inc rax
	mov [k_pit_clocks], rax

	; Flash a 0/1 in the top corner of the screen to indicate ticks (kernel heartbeat)
	and al, 1
    add al, 48
	mov byte[0x000B809E], al
	mov al, 20h
	out 20h, al
	
	pop rax ; Restore state
	mov r10, 0x0

	; If we are doing a speed test just exit
	cmp [k_speed_test], 0
	je start_queue_check
	iretq

	start_queue_check:
		; If the process queue is empty then just return
		cli
		call sys_check_queue_empty
		test rbx, rbx
		jne start_reschedule
		mov r10, 0xF
		sti
		iretq

	; Find the next task to load in the queue
	start_reschedule:
		call reschedule
		; If something went wrong and we somehow end up back here just return
		cmp r14, 0
		je done_irq0
		sti
		iretq

	; Return to a process
	done_irq0:
		; cli
		mov ax, R0_DATA_SELECTOR
		mov ds, ax
		mov es, ax
		mov fs, ax
		mov gs, ax
		; mov rbp, 0x2fffe2 ;RBP
		; mov rax, 0x300000 ;RSP
		mov rax, [k_current_process.r_rsp]

		push R0_DATA_SELECTOR
		push rax
		pushf

		; Enable interrupts
		pop rax
		or rax, 0x200
		push rax

		push R0_CODE_SELECTOR
		; push rbx ; RIP
		push [k_current_process.r_rip]

	end_irq0:
		iretq

irq_1_handler_keyboard:
	; cli
	push rax
	call start_kb
	in al, 61h
	out 61h, al
	mov al, 20h
	out 20h, al
	out 0A0h, al
	pop rax
	; sti
	iretq
	
irq_8_rtc:
	push rdi rax
	add qword [k_clocks], 1
	mov al, 20h
	out 20h, al
	pop rax rdi
	iretq
	
irq_9_pci_device:
	push rax
	; TODO pci things
	mov al, 20h
	out 20h, al
	out 0A0h, al
	pop rax
	iretq

system_call:
	cli
	push rax
	mov al, 20h
	out 20h, al
	pop rax
	call sys_call
	sti
	iretq

reboot:
	mov al, 0FEh
	out 64h, al
	jmp reboot
