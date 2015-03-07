; =========================================================
; minos -- a hobby operating system written for x86-64
; Copyright (C) 2010-2015
;
; Kernel data
; =========================================================

SysVars:
sys_base        	equ 0x0000000000001200
sys_vars        	equ 0x0000000000003200
sys_kernel_extra	equ 0x0000000000100000
sys_free_mem		equ 0x0000000000200000
sys_max_user_mem	equ 0x0000000000100000

R0_NULL_SELECTOR	= 0
R0_DATA_SELECTOR	= 1 shl 3
R0_CODE_SELECTOR	= 2 shl 3
R0_LONG_SELECTOR	= 3 shl 3

; Screen
256_white_black		equ 00001111b
256_white_blue		equ 00011111b

; General
k_main_msg	    	db 'minos64 > ', 0
k_main_msg_size		= $ - k_main_msg
k_var_1			dw 0
k_var_2			dw 0
k_var_3			dd 0
k_var_4			dq 0
hex_list		db '0123456789ABCDEF'
dec_list		db '0123456789'
number_buf		dq 0
number_count		dq 0

; Command / keyboard
k_c_char	    	db 0 ; Char/number buffer
k_command_buffer	rb 24 ; Store chars for a command
k_command_buffer_s	dd 0 ; Number of chars in buffer
crlf			equ 0dh, 0ah

; Clock
k_speed_msg		db 'CPU Speed (mhz):', 0
k_speed_msg_size	= $ - k_speed_msg
k_speed	    		dw 0 ; CPU speed
k_clocks	    	dq 0 ; RTC cycles
k_pit_clocks		dq 0 ; PIT cycles
k_countdown		dq 0

; PCI
k_pci_msg		db 'PCI devices:', 0
k_pci_msg_size		= $ - k_pci_msg
k_pci_enabled		db 0
k_pci_devices		dd 0 ; Number of PCI devices
k_pci_list		rd 10
k_pci_device		dd 0

; Memory
k_memory_msg		db 'Free memory available (KiB):', 0
k_memory_msg_size	= $ - k_memory_msg
total_mem    		rd 1
user_mem		rw 0
mem_amount		dd 0
mem_check		db 1 ; 1 = mem check succeeded, 0 = failed
mem_size		db 0

SysVars_size		= $ - SysVars

;*******
; GDT64
;*******

GDTR64:
	dw 4*8-1
	dq 0x00001000

GDT64 rw 4
	dw 0FFFFh,0,9200h,08Fh
	dw 0FFFFh,0,9A00h,0CFh
	dw 0FFFFh,0,9A00h,0AFh
	
GDTR64_size		= $ - GDTR64