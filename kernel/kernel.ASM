; =========================================================
; minos -- a hobby operating system written for x86-64
; Copyright (C) 2010-2017
;
; Main kernel
; =========================================================

; ---------------
; Debug Registers
; ---------------
; When errors occur the following registers are written to:-
;
; r12 - Misc
; r13 - Application
; r14 - Scheduler
; r15 - System call

format binary

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

	; Set video mode 3 (text) 80 x 25
	mov ax, 03h
	int 10h

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

		jmp TMP_R0_CODE_SELECTOR:intop

	GDTR:
    	dw 4*8-1 ; limit
    	dq GDT ; linear address

	GDT rw 4 ; TMP_R0_NULL_SELECTOR
		dw 0FFFFh,0,9200h,08Fh ; data / ring 0 w / 4KiB blocks / 16-bit - TMP_R0_DATA_SELECTOR
		dw 0FFFFh,0,9A00h,0CFh ; code / ring 0 r / 4KiB blocks / 32-bit - TMP_R0_CODE_SELECTOR

crlf		equ 0dh, 0ah
CR0_PAGING	equ 0x80000000
LONG_CHECK	equ 0x80000001
LONG_TEST	equ 0x20000000
TMP_R0_NULL_SELECTOR	= 0
TMP_R0_DATA_SELECTOR	= 1 shl 3
TMP_R0_CODE_SELECTOR	= 2 shl 3

include 'mem/e820.inc'
include 'inc/functions_16.inc'
include 'dev/vesa.inc'

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
	mov eax, 0x0753074e
	mov [0xb8000], eax
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

	; Build the Page Map Level 4
	mov dword [70000h], 71000h + 111b

	; Build the First Page Directory Pointer Table
	mov dword [71000h], 72000h + 111b

	; Build the First Page Directory
	mov dword [72000h], 73000h + 011b ; Entry #1 Points to first page table
	mov dword [72008h], 74000h + 111b ; Entry #2 Points to second page table

	; ================================================================
	; Create Identity mapped table for first 2 MiB

	; Page table entry (256 x 4 byte entries) one entry addresses 4KiB
	; <-Physical page address->  |        a  wurp
	; 0000 0000 0000 0000 0000     0000 0000 0111
	
	; Address of first page table
	mov edi, 73000h
	mov eax, 0 + 011b ; Supervisor only page table
	mov ecx, 512

	make_page_entries:
		stosd
		add edi, 4
		add eax, 1000h
		loop make_page_entries

	; Check there is memory available for user applications
	cmp word [k_user_mem], 2048
	jl continue_pdp

	; ===============================================
	; Map the remaining 18 MiB
	; Address of second page table
	mov byte [k_user_mode_enabled], 1
	mov edi, 74000h
	mov eax, 200000h + 111b
	
	mov ecx, 4608

	make_user_page_entries:
		stosd
		add edi, 4
		add eax, 1000h
		loop make_user_page_entries
	; ===============================================

	continue_pdp:
		; Load Page Map Level 4 physical address into cr3 register
		mov eax, 70000h
		mov cr3, eax

		mov ecx, 0C0000080h ; EFER MSR
		rdmsr
		or eax, 1 shl 8 ; Enable long mode
		wrmsr

		mov eax, cr0
		or eax, CR0_PAGING
		; Enable paging
		mov cr0, eax

		; Move GDT to final location
		mov esi, GDT64
		mov edi, 0x00001000
		mov ecx, GDTR64_size
		rep movsb

		lgdt [GDTR64]

		; Configure TSS
		lea eax, [k_tss]
		mov [GDT_R0_TSS + 2], ax
		lea eax, [k_tss_end]
		mov [GDT_R0_TSS + 4], al
		mov [GDT_R0_TSS + 7], ah

		lea eax, [u_tss]
		mov [GDT_R3_TSS + 2], ax
		lea eax, [u_tss_end]
		mov [GDT_R3_TSS + 4], al
		mov [GDT_R3_TSS + 7], ah

		mov ax, R3_TSS
		ltr ax

		jmp R0_CODE_SELECTOR:main_64

;***************************
; 64-bit Kernel Entry Point
;***************************

use64

include 'inc/data.inc'

main_64:

	; Save the config file into kernel data before memory gets remapped
	mov esi, 0500h
	mov edi, k_config_file
	mov ecx, 8
	rep movsb
	
	; Clear everything out
	xor rax, rax
	xor rbx, rbx
	xor rcx, rcx
	xor rdx, rdx
	xor rsi, rsi
	xor rdi, rdi
	xor rbp, rbp
	xor r8, r8
	xor r9, r9
	xor r10, r10
	xor r11, r11
	xor r12, r12
	xor r13, r13
	xor r14, r14
	xor r15, r15

	mov ax, R0_DATA_SELECTOR
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov fs, ax
	mov gs, ax
	
	; PIC 1		Command		0x0020
	; PIC 1		Data		0x0021
	; PIC 2		Command		0x00A0
	; PIC 2		Data		0x00A1

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
	
	mov al, 80h ; IRQ 0-7: interrupts 80h-87h
	out 21h, al
	mov al, 88h ; IRQ 8-15: interrupts 88h-8Fh
	out 0A1h, al
	
	mov al, 100b ; Slave connected to IRQ2
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
	; 1      IRQ 1 enabled
	; 0      IRQ 0 enabled
	mov al, 11111100b
	out 21h, al
	in al, 0A1h
	mov al, 11111101b ; Enable IRQ 9
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

make_ring_3_call_gate:
	mov edi, 900h
	mov esi, ring3_interrupt_gate
	movsq
	movsq

	mov word [80h*16], irq_0_handler_clock
	mov word [81h*16], irq_1_handler_keyboard
	mov word [88h*16], irq_8_rtc
	mov word [89h*16], irq_9_pci_device
	mov word [90h*16], system_call

	mov qword [k_pit_clocks], 0
	
	; Set PIT to around 100hz
	mov dx, 2e9bh
	mov al, 36h 
	out 43h, al
	mov ax, dx
	mov al, 9bh
	out 40h, al
	xchg ah, al
	mov al, 2eh
	out 40h, al

	lidt [IDTR]
	sti

; Keyboard buffer must be cleared before IRQ1 will fire
clear_buffer:
	in al, 64h
	test al, 1
	jz finalise_memory
	in al, 60h
	jmp clear_buffer

; Initialise PCI devices
init_pci_devices:
	call drv_pci_init

; Finish setting up memory
finalise_memory:
	mov rsp, sys_stack
	mov rbp, rsp
init_memory_map:
        push rax
        push rcx
        push rdi

        ; Build a fresh memory map for the system
        mov rdi, sys_mm
        push rdi
        xor rcx, rcx
        mov ecx, [k_total_mem]
        shr rcx, 10 ; Convert to KiB
        shr rcx, 2 ; Divide by 4 to get number of 4KiB pages
        mov al, 1
        rep stosb
        pop rdi
        mov rcx, 512
        mov al, 2
        stosb ; Mark the first 2 MiB as in use

        pop rdi
        pop rcx
        pop rax
	mov [k_task_free_addr], k_task_list

main_entry:
	; Clear the screen starting from line 2
	xor rcx, rcx
	mov rcx, 0
	call sys_clear_screen

	; Add config found message
	mov esi, k_config1_msg
	mov ecx, k_config1_msg_size
	call sys_character_print
	
	; Add config file contents
	mov esi, k_config_file
	mov ecx, 8
	call sys_character_print
	call sys_lb
	mov al, 1 ; Configure PCI devices
	call sys_pci_list

	; Clear process queue
	mov edi, k_process_queue
	mov ecx, 10
	mov ax, 0
	rep stosb

	; Setup idle kernel task using the current stack
	cli
	mov rax, sys_task_1
	call sys_start_task
	mov rax, sys_task_2
	call sys_start_task
	sti

	; Run a test user program
	;mov rax, u_prog_1
	;mov rcx, u_prog_1_size
	;call sys_load_program

	idle:
		jmp idle

	hlt

; Core functions
include 'inc/macros.inc'
include 'sys/interrupts.inc'
include 'sys/sched.inc'
include 'cli/cli.inc'
include 'cli/command_list.inc'
; Drivers
include 'dev/vga.inc'
include 'dev/pci.inc'
include 'dev/i8254x.inc'
include 'dev/rtl8169.inc'
include 'dev/key_converter.inc'
; System calls and associated routines
include 'sys/syscalls.ASM'
; Programs
include 'apps/userland.inc'
include 'apps/system.inc'

times   2000h - ($-$$)   db 0 ; Pad out to 8192 bytes long (so we know exactly how large the kernel is)
