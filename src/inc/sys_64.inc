; =========================================================
; minos -- a hobby operating system written for x86-64
; Copyright (C) 2010-2016
;
; Kernel data
; =========================================================

SysVars:
sys_base        		equ 0x0000000000001200
sys_vars        		equ 0x0000000000003200
sys_kernel_extra		equ 0x0000000000100000
sys_kernel_extra_top	equ 0x00000000001FFFFF
sys_free_mem			equ 0x0000000000200000
sys_max_user_mem		equ 0x0000000000100000

R0_NULL_SELECTOR	= 0
R0_DATA_SELECTOR	= 1 shl 3
R0_CODE_SELECTOR	= 2 shl 3
R3_DATA_SELECTOR	= 3 shl 3
R3_CODE_SELECTOR	= 4 shl 3
R0_TSS				= 5 shl 3
R3_TSS				= 6 shl 3

; Screen
256_white_black		equ 00001111b
256_white_blue		equ 00011111b

; General
k_main_msg	    	db 'minOS64 > ', 0
k_main_msg_size		= $ - k_main_msg
k_var_1				db 0
k_var_2				dw 0
k_var_3				dd 0
k_var_4				dq 0
hex_list			db '0123456789ABCDEF'
dec_list			db '0123456789'
number_buf			dq 0
number_count		dq 0

; Task handling
struc k_task r_rip, r_rsp, r_rbp, r_pid
{
	.r_rip dq r_rip ;0
	.r_rsp dq r_rsp ;8
	.r_rbp dq r_rbp ;16
	.r_pid dq r_pid ;24
}

k_id_count			db 0
MAX_PROCESSES		equ 10
k_process_queue		rb MAX_PROCESSES ; Contains task PID's
k_previous_process	k_task 0,0,0,0
k_current_process	k_task 0,0,0,0
k_task_1			k_task 0,0,0,0
k_task_2			k_task 0,0,0,0
k_task_3			k_task 0,0,0,0
k_task_4			k_task 0,0,0,0
k_task_5			k_task 0,0,0,0
k_task_6			k_task 0,0,0,0
k_task_7			k_task 0,0,0,0
k_task_8			k_task 0,0,0,0
k_task_9			k_task 0,0,0,0
k_task_10			k_task 0,0,0,0
k_process_variable1 db 0
k_process_variable2 db 0
k_queue_empty 		db 0

; Command / keyboard
k_c_char	    	db 0 ; Char/number buffer
k_command_buffer	rb 24 ; Store chars for a command
k_command_buffer_s	dd 0 ; Number of chars in buffer
crlf				equ 0dh, 0ah

; Clock
k_speed_msg			db 'CPU Speed (Mhz):', 0
k_speed_msg_size	= $ - k_speed_msg
k_speed	    		dq 0 ; CPU speed
k_clocks	    	dq 0 ; RTC cycles
k_pit_clocks		dq 0 ; PIT cycles
k_countdown			dq 0
k_loTSC				dq 0
k_hiTSC				dq 0
k_speed_test		db 0

; PCI
k_pci_msg				db 'PCI devices:', 0
k_pci_msg_size			= $ - k_pci_msg
k_pci_enabled			db 0
k_pci_devices			dd 0 ; Number of PCI devices
k_pci_device			dd 0
k_pci_net_device 		dd 0
k_pci_net_device_irq 	db 0
k_pci_net_device_ioaddr dw 0
k_pci_rtl8139 			equ 0x8139
k_pci_rtl8169           equ 0x8169
k_pci_i82557b			equ 0x1209
k_pci_i82551            equ 0x1209
k_pci_i82559er          equ 0x1229

; Memory
k_memory_msg		db 'Free memory available (KiB):', 0
k_memory_msg_size	= $ - k_memory_msg
total_mem    		rd 1
user_mem			rw 0
mem_amount			dd 0
mem_check			db 1 ; 1 = mem check succeeded, 0 = failed
mem_size			db 0
user_mode_enabled 	db 0

; Config 
k_config_file  			rb 8
k_config1_msg           db 'Config found on disk: ', 0
k_config1_msg_size      = $ - k_config1_msg

k_tss:
	dw 0, 0					; Link - reserved
	dd 0					; ESP0
	dw 0, 0					; SS0 - reserved
	dd 0					; ESP1
	dw 0, 0					; SS1 - reserved
	dd 0					; ESP2
	dw 0, 0					; SS2 - reserved
	dd 0					; CR3
k_tss_eip:
	dd 0, 0					; EIP, EFLAGS
	dd 0, 0, 0, 0			; EAX, ECX, EDX, EBX
k_tss_esp:
	dd 0, 0, 0, 0			; ESP, EBP, ESI, EDI
	dw 0, 0					; ES - reserved
	dw 0, 0 				; CS - reserved
	dw 0, 0					; SS - reserved
	dw 0, 0					; DS - reserved
	dw 0, 0					; FS - reserved
	dw 0, 0					; GS - reserved
	dw 0, 0					; LDT - reserved
	dw 0, 0					; IOPB offset - reserved
k_tss_end:

u_tss:
	dw 0, 0					; Link - reserved
	dd 0					; ESP0
	dw 0, 0					; SS0 - reserved
	dd 0					; ESP1
	dw 0, 0					; SS1 - reserved
	dd 0					; ESP2
	dw 0, 0					; SS2 - reserved
	dd 0					; CR3
u_tss_eip:
	dd 0, 0					; EIP, EFLAGS
	dd 0, 0, 0, 0			; EAX, ECX, EDX, EBX
u_tss_esp:
	dd 0, 0, 0, 0			; ESP, EBP, ESI, EDI
	dw R3_DATA_SELECTOR, 0	; ES - reserved
	dw R3_CODE_SELECTOR, 0	; CS - reserved
	dw R3_DATA_SELECTOR, 0	; SS - reserved
	dw R3_DATA_SELECTOR, 0	; DS - reserved
	dw R3_DATA_SELECTOR, 0	; FS - reserved
	dw R3_DATA_SELECTOR, 0	; GS - reserved
	dw 0, 0					; LDT - reserved
	dw 0, 0					; IOPB offset - reserved
u_tss_end:

SysVars_size		= $ - SysVars

;*******
; GDT64
;*******

GDTR64:
	dw GDTR64_size
	dq 0x00001000

GDT64 rw 4 ; R0_NULL_SELECTOR
	dw 0FFFFh,0,9200h,0AFh ; data / ring 0 w / 4KiB blocks / 64-bit - R0_DATA_SELECTOR
	dw 0FFFFh,0,9A00h,0AFh ; code / ring 0 r / 4KiB blocks / 64-bit - R0_CODE_SELECTOR
	dw 0FFFFh,0,1111001000000000b,0AFh ; data / ring 3 w / 4KiB blocks / 64-bit - R3_DATA_SELECTOR
	dw 0FFFFh,0,1111101000000000b,0AFh ; code / ring 3 r / 4KiB blocks / 64-bit - R3_CODE_SELECTOR
	GDT_R0_TSS:
	dw 0FFFFh,0,8900h,020h ; R0_TSS
	GDT_R3_TSS:
	dw 0FFFFh,0,1110100100000000b,020h ; R3_TSS
	
GDTR64_size		= $ - GDTR64

k_stack_begin:
	rb 1024
k_stack_end:
