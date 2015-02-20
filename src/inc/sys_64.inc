; =========================================================
; minos -- a hobby operating system written for x86-64
; Copyright (C) 2010-2015
;
; Kernel data
; =========================================================

SysVars:
sys_base        	equ 0x0000000000001200
sys_vars        	equ 0x0000000000003300
e820        		equ 0x0000000000004000

; Messages
k_main_msg	    	db 'minos64 > ', 0
k_main_msg_size = $ - k_main_msg

; Char/number buffer
k_c_char	    	db 0

; General
k_var_1				dw 0
k_var_2				dw 0
k_var_3				dw 0

hex_list			db '0123456789ABCDEF'

; Char buffer
k_command_buffer	rb 24
k_command_buffer_s	db 0

; Store current clock cycles
k_clocks	    	dq 0

; CPU speed
k_speed	    		dw 0

total_mem    		rd 1
user_mem			rw 0
mem_amount			dd 0
mem_check			db 1	; 1 = mem check succeeded, 0 = failed

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