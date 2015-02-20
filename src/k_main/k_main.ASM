; =========================================================
; minos -- a hobby operating system written for x86-64
; Copyright (C) 2010-2015
;
; Main kernel
; =========================================================

format binary

include '../constants.inc'

use16
org 1200h

;===============
; 16-bit kernel
;===============
start:
	cli
	xor eax, eax
	xor ebx, ebx
	xor ecx, ecx
	xor edx, edx
	xor esi, esi
	xor edi, edi
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	mov ax, 0fffh
	mov ss, ax ; Segment where stack is located in memory
	mov esp, 0ffffh
	mov bp, sp
	sti

	mov ax, 0e43h
	xor bx, bx
	; Get cursor position and shape, return dl with column, ax = 0
	int 10h

	; Set video mode 3 (text) 80 x 25
	mov ax, 0003h
	int 10h

	; TODO VBE

	call detect_memory

	; Fast A20 gate
    enable_A20:
    	in al, 64h
    	test al, 2h
    	jnz enable_A20
    	mov al, 0D1h
    	out 64h, al
    check_A20:
    	in al, 64h
    	test al, 2h
    	jnz check_A20
    	mov al, 0DFh

	startgdt:
	; Load GDT
	lgdt [cs:GDTR]

	mov eax, cr0
	or al, 0x1
	mov cr0, eax

	jmp R0_CODE_SELECTOR:intop

	GDTR:
    	dw 4*8-1 ; limit
    	dq GDT ; linear address

    GDT rw 4
    	dw 0FFFFh,0,9200h,08Fh
    	dw 0FFFFh,0,9A00h,0CFh
    	dw 0FFFFh,0,9A00h,0AFh

include '../inc/e820.inc'

;===============
; 32-bit kernel
;===============
use32
intop:
	mov eax, R0_DATA_SELECTOR
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov fs, ax
	mov gs, ax
	xor eax, eax
	xor ebx, ebx
	xor ecx, ecx
	xor edx, edx
	xor esi, esi
	xor edi, edi
	xor ebp, ebp
	mov esp, 0ffffh

check_long:
	xor eax, eax
	; Check if CPU long mode capable
	mov eax, LONG_CHECK
	cpuid

	test edx, LONG_TEST
	jnz start_long

main_32:
	; Protected mode not supported with functionality
	; TODO inform user
	hlt

start_long:
	mov eax, cr4
	or eax, 1 shl 5
	; Enable PAE
	mov cr4, eax

	; Clear memory for the page descriptor entries
	mov edi, 70000h
	mov ecx, 4000h shr 2
	xor eax, eax
	rep stosd

	; Page directory table for the MMU
	mov dword [70000h], 71000h + 111b

	; First page directory
	mov dword [71000h], 72000h + 111b

	; Identity mapped table
	mov dword [72000h], 73000h + 111b

	; User table
	mov dword [73000h], 74000h + 111b

	; ========================
	; Fill Identity mapped table

	; Page table entry (256 x 4 byte entries) 4kb entries
	; <-Physical page address->  |
	; 0000 0000 0000 0000 0000     0000 0000 0111 (su/rw/p)

	; Address of first page table
	mov edi, 73000h
	mov eax, 0 + 011b ; Supervisor only page table
	mov ecx, 256

	make_page_entries:
		stosd
		add edi, 4
		add eax, 1000h
		loop make_page_entries
	; ========================

	; Check there is memory above first Mib.
	cmp word [user_mem], 1024
    jg continue_pdp

	; ========================
	; Fill user table
	; Address of second page table
	mov edi, 74000h
	mov eax, 0 + 111b
	mov ecx, 256

	make_user_page_entries:
		stosd
		add edi, 4
		add eax, 1000h
		loop make_user_page_entries
	; ========================

    continue_pdp:

	; Load PDP physical address into cr3 register
	mov eax, 70000h
	mov cr3, eax

	mov ecx, 0C0000080h          ; EFER MSR
	rdmsr
	or eax, 1 shl 8             ; Enable long mode
	wrmsr

	mov eax, cr0
	or eax, CR0_PAGING
	; Enable paging
	mov cr0, eax

	jmp R0_LONG_SELECTOR:main_64

;***************************
; 64-bit Kernel Entry Point
;***************************

use64

include '../inc/sys_64.inc'

main_64:
	; Clear everything out
	xor rax, rax
	xor rbx, rbx
	xor rcx, rcx
	xor rdx, rdx
	xor rsi, rsi
	xor rdi, rdi
	xor rbp, rbp
	mov rsp, 0ffffh
	xor r8, r8
	xor r9, r9
	xor r10, r10
	xor r11, r11
	xor r12, r12
	xor r13, r13
	xor r14, r14
	xor r15, r15

	mov ds, ax
	mov es, ax
	mov ss, ax
	mov fs, ax
	mov gs, ax

	; PIC 1 init
	; Bit   Name
 	;-------------------------------------------------
 	; 7     Unused
 	; 6     Unused
 	; 5     Unused
 	; 4     Init mode
 	; 3     Clear: Edge triggered IRQ (PC,XT,AT)
	;       Set: Level triggered IRQ (MCA)
	; 		Unused on EISA (Controlled from port 4D0h)
	; 2     Unused
 	; 1     Clear: Cascade mode (AT+)
	;     	Set: Single mode (PC/XT)
	; 0     Additional init byte

	mov al, 10001b
	out 20h, al

	; PIC 2 init
	mov al, 10001b
	out 0A0h, al
	mov al, 80h			; IRQ 0-7: interrupts 80h-87h
	out 21h, al
	mov al, 88h			; IRQ 8-15: interrupts 88h-8Fh
	out 0A1h, al
	mov al, 100b 			; slave connected to IRQ2
	out 21h, al
	mov al, 2
	out 0A1h, al

	; Intel EOI
	mov al, 1
	out 21h, al
	out 0A1h, al
	in al, 21h
	
	; Enable IRQ 0 & 1
	; Bit    Name
	; ---------------------
	; 7      IRQ 7 disabled
	; 6      IRQ 6 disabled
	; 5      IRQ 5 disabled
	; 4      IRQ 4 disabled
	; 3      IRQ 3 disabled
	; 2      IRQ 2 disabled
	; 1      IRQ 1 disabled
	; 0      IRQ 0 disabled
	mov al, 11111100b
	out 21h, al
	in al, 0A1h
	mov al, 11111111b
	out 0A1h, al

	xor edi, edi
	mov ecx, 21

make_exception_gates:
	mov esi, exception_gate
	movsq
	movsq
	loop make_exception_gates
	mov ecx, 256-21
	
make_interrupt_gates:
	mov esi, interrupt_gate
	movsq
	movsq
	loop make_interrupt_gates

	mov word [80h*16], irq_0_handler_clock
	mov word [81h*16], irq_1_handler_keyboard

	lidt [IDTR]

	sti

; Keyboard buffer must be cleared before IRQ1 will fire
clear_buffer:
	in al, 64h
	test al, 1
	jz intro_msg
	in al, 60h
	jmp clear_buffer
	
intro_msg:
	; Move GDT
	mov esi, GDT64
	mov edi, 0x00001000
	mov ecx, 1Fh
	rep movsb
	
	; Then load it
	lgdt [GDTR64]

	; cmp byte [mem_check], 0
	; je main_entry
	; call sys_show_mem_amount

main_entry:
	; Clear the screen starting from line 2
	xor rcx, rcx
	mov rcx, 2
	call sys_clear_screen

	; Add main command line message and wait for user input
	mov esi, k_main_msg
	mov ecx, k_main_msg_size
	call sys_character_print

	; call sys_cpu_speed

main_loop:
	jmp main_loop

include '../inc/macros_64.inc'
include '../inc/interrupts_64.inc'
include '../drivers/key_converter.inc'
include '../drivers/keyboard.inc'
include '../drivers/vga.inc'
include '../inc/commandlist.inc'
include '../syscalls.ASM'

load_end_addr:
; align 4
; rb STACK_SIZE
bss_end_addr:
times   2000h - ($-$$)   db 0 ; Pad out to 8192 bytes long (so we know exactly how large the kernel is)
