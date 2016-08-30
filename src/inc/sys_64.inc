; =========================================================
; minos -- a hobby operating system written for x86-64
; Copyright (C) 2010-2016
;
; Kernel data
; =========================================================

; Kernel memory
sys_base                equ 0x0000000000001200
sys_vars                equ 0x0000000000004000
sys_mm                  equ 0x0000000000004800
sys_stack               equ 0x0000000000003FFF
; Free memory
sys_kernel_extra        equ 0x0000000000100000
sys_kernel_extra_top    equ 0x00000000001FFFFF
sys_free_mem            equ 0x0000000000200000
sys_max_user_mem        equ 0x0000000000100000

; ---------------------------------------------
; Data Structure Definitions
; ---------------------------------------------

struc k_task r_rip, r_rsp, r_rbp, r_pid
{
        .r_rip dq r_rip ;0
        .r_rsp dq r_rsp ;8
        .r_rbp dq r_rbp ;16
        .r_pid dq r_pid ;24
}

; ---------------------------------------------
; Dynamic Data Pointers
; ---------------------------------------------

; Dynamic data bytes
k_var_1                 = sys_vars + 0
k_id_count              = sys_vars + 1
k_c_char                = sys_vars + 2
k_process_variable1     = sys_vars + 3
k_process_variable2     = sys_vars + 4
k_queue_empty           = sys_vars + 5
k_speed_test            = sys_vars + 6
k_pci_enabled           = sys_vars + 7
k_pci_net_device_irq    = sys_vars + 8
k_mem_check             = sys_vars + 9 ; 1 = mem check succeeded, 0 = failed
k_mem_size              = sys_vars + 10
k_user_mode_enabled     = sys_vars + 11

; Dynamic data words
k_var_2                 = sys_vars + 12
k_pci_net_device_ioaddr = sys_vars + 14

; Dynamic data dwords
k_var_3                 = sys_vars + 16
k_command_buffer_s      = sys_vars + 20 ; Number of chars in buffer
k_pci_devices           = sys_vars + 24 ; Number of PCI devices
k_pci_device            = sys_vars + 28
k_pci_net_device        = sys_vars + 32
k_mem_amount            = sys_vars + 36

; Dynamic data qwords
k_var_4                 = sys_vars + 40
k_number_buf            = sys_vars + 48
k_number_count          = sys_vars + 56
k_clocks                = sys_vars + 64 ; RTC cycles
k_pit_clocks            = sys_vars + 72 ; PIT cycles
k_countdown             = sys_vars + 80
k_loTSC                 = sys_vars + 88
k_hiTSC                 = sys_vars + 96
k_speed                 = sys_vars + 104 ; CPU speed

; ---------------------------------------------
; Constant Data
; ---------------------------------------------

R0_NULL_SELECTOR        = 0
R0_DATA_SELECTOR        = 1 shl 3
R0_CODE_SELECTOR        = 2 shl 3
R3_DATA_SELECTOR        = 3 shl 3
R3_CODE_SELECTOR        = 4 shl 3
R0_TSS                  = 5 shl 3
R3_TSS                  = 6 shl 3
16_white_black          equ 00001111b
16_white_blue           equ 00011111b
k_pci_rtl8139           equ 0x8139
k_pci_rtl8169           equ 0x8169
k_pci_i82557b           equ 0x1209
k_pci_i82551            equ 0x1209
k_pci_i82559er          equ 0x1229
MAX_PROCESSES           equ 10
crlf                    equ 0dh, 0ah
MAX_TASK_DATA           = MAX_PROCESSES * 32

; ---------------------------------------------
; Strings
; ---------------------------------------------

k_main_msg              db 'minOS64 > ', 0
k_main_msg_size		= $ - k_main_msg
k_hex_list              db '0123456789ABCDEF'
k_dec_list              db '0123456789'
k_speed_msg             db 'CPU Speed (Mhz):', 0
k_speed_msg_size	= $ - k_speed_msg
k_pci_msg               db 'PCI devices:', 0
k_pci_msg_size		= $ - k_pci_msg
k_memory_msg            db 'Free memory available (KiB):', 0
k_memory_msg_size	= $ - k_memory_msg
k_config1_msg           db 'Config found on disk: ', 0
k_config1_msg_size	= $ - k_config1_msg

; ---------------------------------------------
; Uninitialised Reserved Data
; ---------------------------------------------

k_command_buffer        rb 24 ; Store chars for a command
k_config_file           rb 8
k_process_queue         rb MAX_PROCESSES ; Contains task PID's

k_task_free_addr	rq 1
k_user_mem              rw 1

k_total_mem             rd 1

k_task_list:            rb MAX_TASK_DATA

k_previous_process      k_task 0,0,0,0
k_current_process       k_task 0,0,0,0

; ---------------------------------------------
; Task State Segment Data
; ---------------------------------------------

k_tss:
        dw 0, 0                 ; Link - reserved
        dd 0                    ; ESP0
        dw 0, 0                 ; SS0 - reserved
        dd 0                    ; ESP1
        dw 0, 0                 ; SS1 - reserved
        dd 0                    ; ESP2
        dw 0, 0                 ; SS2 - reserved
        dd 0                    ; CR3
k_tss_eip:
        dd 0, 0                 ; EIP, EFLAGS
        dd 0, 0, 0, 0           ; EAX, ECX, EDX, EBX
k_tss_esp:
        dd 0, 0, 0, 0           ; ESP, EBP, ESI, EDI
        dw 0, 0                 ; ES - reserved
        dw 0, 0                 ; CS - reserved
        dw 0, 0                 ; SS - reserved
        dw 0, 0                 ; DS - reserved
        dw 0, 0                 ; FS - reserved
        dw 0, 0                 ; GS - reserved
        dw 0, 0                 ; LDT - reserved
        dw 0, 0                 ; IOPB offset - reserved
k_tss_end:

u_tss:
        dw 0, 0                 ; Link - reserved
        dd 0                    ; ESP0
        dw 0, 0                 ; SS0 - reserved
        dd 0                    ; ESP1
        dw 0, 0                 ; SS1 - reserved
        dd 0                    ; ESP2
        dw 0, 0                 ; SS2 - reserved
        dd 0                    ; CR3
u_tss_eip:
        dd 0, 0                 ; EIP, EFLAGS
        dd 0, 0, 0, 0           ; EAX, ECX, EDX, EBX
u_tss_esp:
        dd 0, 0, 0, 0           ; ESP, EBP, ESI, EDI
        dw R3_DATA_SELECTOR, 0  ; ES - reserved
        dw R3_CODE_SELECTOR, 0  ; CS - reserved
        dw R3_DATA_SELECTOR, 0  ; SS - reserved
        dw R3_DATA_SELECTOR, 0  ; DS - reserved
        dw R3_DATA_SELECTOR, 0  ; FS - reserved
        dw R3_DATA_SELECTOR, 0  ; GS - reserved
        dw 0, 0                 ; LDT - reserved
        dw 0, 0                 ; IOPB offset - reserved
u_tss_end:

; ---------------------------------------------
; Global Descriptor Table
; ---------------------------------------------

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

GDTR64_size             = $ - GDTR64
