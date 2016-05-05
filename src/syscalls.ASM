; =========================================================
; minos -- a hobby operating system written for x86-64
; Copyright (C) 2010-2016
;
; System Calls & Routines
; =========================================================

; ===========================
; sys_call
; --------
; Standard system call
; IN  - rax = Call number
;	  - rbx = Parameters/Data
; ===========================

sys_call:
	pushaq
	cmp rax, 0
	je call_task_exit
	cmp rax, 1
	je call_sys_character_print
	; No matches - bye!
	jmp sys_call_ret

	call_task_exit:
		xor rax, rax
		xor rbx, rbx
		find_pid_in_queue:
			mov al, byte[k_process_queue+rbx]
			; Check if its zero
			cmp al, byte[k_current_process.r_pid]
			je delete_task
			inc rbx
			cmp rbx, MAX_PROCESSES - 1
			; Cant find it? Major problem..
			je exit_task
			jmp find_pid_in_queue

			delete_task:
				mov byte[k_process_queue+rbx], 0
				jmp exit_task

			exit_task:
				popaq
				sti
				jmp idle

	call_sys_character_print:
		mov rax, rbx
		call sys_get_string_size
		mov esi, ebx
		mov ecx, eax ; Limited to a 32-bit number currently
		call sys_character_print
		jmp sys_call_ret

	sys_call_ret:
		popaq
		ret

; ======================================
; sys_get_string_size
; -------------
; Get size of null terminated char array
; IN - rax = pointer to string byte array
; OUT - rax = size of string
; ======================================

sys_get_string_size:
	push rbx rcx
	mov rbx, rax
	mov rcx, rax
	sys_get_string_size_loop:
		mov rsi, rcx
		lodsb
		cmp al, 0
		je sys_get_string_size_done
		inc rcx
		jmp sys_get_string_size_loop

sys_get_string_size_done:
	sub rcx, rbx
	mov rax, rcx
	pop rcx rbx
	ret

; ===============================
; sys_hex_print
; -------------
; Print hex numbers to the screen
; IN - al = Number to print
; ===============================

sys_hex_print:
	push rax rbx rcx
	mov rbx, hex_list

	push rax
	shr al, 4
	xlatb
	mov [k_c_char], al
	mov esi, k_c_char
	mov ecx, 1
	call sys_character_print

	pop rax
	and al, 0x0f
	xlatb
	mov [k_c_char], al
	mov esi, k_c_char
	mov ecx, 1
	call sys_character_print

	pop rcx rbx rax
	ret
	
; ===================================
; sys_dec_convert
; ---------------
; Print decimal numbers to the screen
; IN  - ax/eax/rax = Decimal to print
; OUT - number_buf = Contains ascii
; ===================================

sys_dec_convert:
	push rdx rcx rbx rax
	mov rbx, 10
	xor rcx, rcx
	
	.sys_dec_print_do_div:
		xor rdx, rdx
		div rbx
		push rdx
		inc rcx
		cmp rax, 0h
		jne .sys_dec_print_do_div
	
	.sys_dec_print_do_digit:
		pop rdx
		add dl, '0'
		push rbx
		mov rbx, [number_count]
		mov byte [number_buf+rbx], dl
		inc rbx
		mov [number_count], rbx
		pop rbx
		loop .sys_dec_print_do_digit

		pop rax rbx rcx rdx
        ret

; ==========================================
; sys_convert_ascii_dec
; ---------------------
; Print decimal numbers to the screen
; IN  - al = ASCII representation of decimal
; OUT - al = Decimal
; ==========================================

sys_convert_ascii_dec:
	cmp al, 31h
	je is_1
	cmp al, 32h
	je is_2
	cmp al, 33h
	je is_3
	cmp al, 34h
	je is_4
	cmp al, 35h
	je is_5
	cmp al, 36h
	je is_6
	cmp al, 37h
	je is_7
	cmp al, 38h
	je is_8
	cmp al, 39h
	je is_9
	cmp al, 30h
	je is_0
	
	is_1:
		mov al, 1
		jmp sys_convert_ascii_dec_done
	is_2:
		mov al, 2
		jmp sys_convert_ascii_dec_done
	is_3:
		mov al, 3
		jmp sys_convert_ascii_dec_done
	is_4:
		mov al, 4
		jmp sys_convert_ascii_dec_done
	is_5:
		mov al, 5
		jmp sys_convert_ascii_dec_done
	is_6:
		mov al, 6
		jmp sys_convert_ascii_dec_done
	is_7:
		mov al, 7
		jmp sys_convert_ascii_dec_done
	is_8:
		mov al, 8
		jmp sys_convert_ascii_dec_done
	is_9:
		mov al, 9
		jmp sys_convert_ascii_dec_done
	is_0:
		mov al, 0
		jmp sys_convert_ascii_dec_done
	
	sys_convert_ascii_dec_done:
		ret

; ===================================
; sys_command_print
; -----------------
; Handle command line input
; IN - esi = string to print
;      ecx = number of chars to print
; OUT - None
; ===================================

sys_command_print:
	call sys_character_print
	
	.sys_command_print_done:
		ret

; ============================
; sys_lb
; ------
; Adds new line to the screen
; IN /OUT - None
; ============================
sys_lb:
	push rax rbx rcx rdx
	mov [ScreenWidth], MinX
	mov ebx, MinX
	mov eax, [ScreenLines]
	mul ebx
	mov [ScreenYPos], eax
	sub [ScreenWidth], ebx
	add ebx, [ScreenWidth]
	add ebx, [ScreenYPos]
	call scroll_screen_check
	inc [ScreenLines]

	; Set cursor position
	mov al, 0x0E
	mov ah, bh
	mov dx, 0x03D4
	out dx, ax
	inc ax
	mov ah, bl
	out dx, ax

	mov esi, k_main_msg
	mov ecx, k_main_msg_size
	call sys_character_print
	pop rdx rcx rbx rax
	ret

; ===================================
; sys_character_print
; -------------------
; Print characters to the screen
; IN - esi = string to print
;      ecx = number of chars to print
; OUT - None
; ===================================

sys_character_print:
    ; Global text printer
	pushaq
        mov ax, 0x0F0E
        mov dx, 0x03D4
        out dx, al
        inc dx
        in al, dx
        xchg ah, al
        dec dx
        out dx, al
        inc dx
        in al, dx
        movzx ebx, ax

       .character_printLoop:
			; Print each character
			lodsb
			; Set blue background and white foreground
			; Set screen colours
			; ax holds screen colors and char to write

			mov ah, 256_white_black
			cmp al, 10

			je .handleCR
			cmp al, 13
			je .handleCR

			mov [es:(ebx*2+0xB8000)], ax
			inc bx
			jmp .nextChar

		.handleCR:
			; Currently this only supports 80x25 mode
			mov [ScreenWidth], MinX
			mov ebx, MinX
			mov eax, [ScreenLines]
			mul ebx
			mov [ScreenYPos], eax

			sub [ScreenWidth], ebx
			add ebx, [ScreenWidth]
			add ebx, [ScreenYPos]

			call scroll_screen_check

			inc [ScreenLines]
			mov esi, k_main_msg
			mov ecx, k_main_msg_size

		.nextChar:
			; Loop through buffer
			; cmp 	ecx, 0
			; jne 	.character_printLoop
			; dec 	ecx
			loop .character_printLoop

			mov al, 0x0E
			mov ah, bh
			mov dx, 0x03D4
			out dx, ax
			inc ax
			mov ah, bl
			out dx, ax

		.doneChar:
			mov [number_buf], 0
			mov [number_count], 0
			popaq
			ret

scroll_screen_check:
	cmp [ScreenLines], MinY - 1
	jne .ret_scroll
	; Clear the entire screen and start from the top
	push rcx
	xor rcx, rcx
	mov rcx, 0
	call sys_clear_screen
	pop rcx
	mov [ScreenLines], -1
	xor ebx, ebx
	
	.ret_scroll:
		ret

; ===============================
; sys_clear_screen
; ----------------
; Clears the screen
; IN - RCX = Lines to clear from
; OUT - None
; ===============================

sys_clear_screen:
	push rax rbx
	xor rbx, rbx
	xor rax, rax

	mov rax, TotL
	mul rcx
	add rbx, rax
	xor rax, rax
	xor rcx, rcx
	mov rcx, TotS

	start_clear:
	add rbx, 2
	mov ah, 256_white_black
	mov [0B8000h + rbx], rax
	cmp rbx, rcx
	jne start_clear
	pop rbx rax
	xor rcx, rcx
	ret

; ==============================================
; sys_pci_list
; ------------
; Output PCI device list in device/vendor format
; IN - al = 0/1 to configure devices as well
; OUT - None
; ==============================================

sys_pci_list:
	pushaq
	mov dl, al
	xor ecx, ecx
	mov esi, k_pci_msg
	mov ecx, k_pci_msg_size
	call sys_character_print
	mov al, dl
	call drv_pci_scan
	call sys_lb
	popaq
	ret

; ================
; sys_cpu_speed
; -------------
; Test CPU speed
; IN / OUT - None
; ================

sys_cpu_speed:
	push rax rbx rcx rdx

	xor ecx, ecx
	mov esi, k_speed_msg
	mov ecx, k_speed_msg_size
	call sys_character_print
	
    xor rax, rax
    xor rbx, rbx
    xor rcx, rcx
    xor rdx, rdx

	mov rax, 1
	cpuid
	test rdx, 0x10
	jz done_sys_cpu_speed
	mov rbx, [k_pit_clocks]

sys_cpu_speed_wait1:
	cmp rbx, [k_pit_clocks]
	jz sys_cpu_speed_wait1
	rdtsc
	mov [k_loTSC], rax
	mov [k_hiTSC], rdx
	add rbx, 2

sys_cpu_speed_wait2:
	cmp rbx, [k_pit_clocks]
	jnz sys_cpu_speed_wait2
	rdtsc
	sub rax, [k_loTSC]
	sbb rdx, [k_hiTSC]
	mov rbx, 10000
	div rbx
	mov [k_speed], rax
	
	; Output speed
	xor rax, rax
	mov rax, [k_speed]
	call sys_dec_convert

	mov esi, number_buf
	mov ecx, dword [number_count]
	call sys_character_print

	mov [number_count], 0
	mov [number_buf], 0

done_sys_cpu_speed:
	call sys_lb
	pop   rdx rcx rbx rax
	ret
	
; =============================
; sys_wait
; --------
; Wait for x amount of time
; IN - rax = Time to wait in ms
; =============================

sys_wait:
	push rax bx
	mov [k_countdown], rax
		
	sys_wait_count:
		mov rax, [k_countdown]
		or rax, rax
		jnz sys_wait_count
		
	sys_wait_done:
		pop rax bx
		ret

; +++++++++++++++++++
; Memory System Calls
; +++++++++++++++++++

; =======================================
; sys_allom
; ---------
; Allocate memory
; IN  - rcx = Number of bytes to allocate
; OUT - rax = Starting address
; =======================================

sys_allom:
	push rsi rdx rbx

	; Must be at least one byte
	cmp rcx, 0
	je sys_allom_fail

	mov [mem_size], cl

	xor rax, rax
	mov rsi, sys_free_mem
	mov rax, sys_max_user_mem
	mov rdx, rsi
	
	sub rsi, 1
	std ; Set direction flag
	add rsi, rax

	sys_allom_start:
		mov rbx, rcx

	sys_allom_check:
		lodsb ; load byte from si into al
		
		; As we are counting downwards check if we are at the start of the user memory section (sys_free_mem)
		cmp rsi, rdx
		je sys_allom_fail

		; Free = 1
		cmp al, 1
		jne sys_allom_start

		dec rbx
		jnz sys_allom_check

	sys_allom_doalloc:
		cld ; Clear direction flag

		xor rdi, rsi
		xor rsi, rdi
		xor rdi, rsi
	
		push rcx
		add rdi, 1
		mov al, 2
		mov rbx, rdi
		rep stosb
		mov rdi, rsi
		sub rbx, rdx
		pop rcx

		shl rbx, 1
		add rbx, rax ; Add total available memory back to calculate address
		mov rax, rbx
		jmp sys_allom_done

	sys_allom_fail:
		; Restore direction flag if failed
		cld
		xor rax, rax

	sys_allom_done:
		pop rbx rdx rsi
		ret

; ====================================
; sys_show_mem
; ------------
; Show total free memory
; IN  - cl = 1 (bytes), 2(KiB), 3(MiB)
; OUT - Print total to sreen
; ====================================

sys_show_mem:
	push rax rcx rbx
	mov [mem_size], cl

	mov esi, k_memory_msg
	mov ecx, k_memory_msg_size
	call sys_character_print
	
	mov eax, [total_mem]
	cmp [mem_size], 1
	je set_b
	cmp [mem_size], 2
	je set_kb
	cmp [mem_size], 3
	je set_mb
	
	set_b:
		jmp sys_show_mem_display
	set_kb:
		shr eax, 10
		jmp sys_show_mem_display
	set_mb:
		shr eax, 20
		jmp sys_show_mem_display

	sys_show_mem_display:
		call sys_dec_convert
	
		mov esi, number_buf
		mov ecx, dword [number_count]
		call sys_character_print
	
		mov [number_count], 0
		mov [number_buf], 0
		call sys_lb
		pop rax rcx rbx
		ret


; ========================
; sys_start_task
; --------------
; Start a system task
; IN  - al = 1 task number
; OUT - None
; ========================

sys_start_task:
	pushaq
	cli
	mov ah, al
	xor rbx, rbx
	xor rcx, rcx
	find_free_slot:
		mov al, byte[k_process_queue+rbx]
		; Check if its zero
		cmp al, 0
		je create_task
		inc rbx
		cmp rbx, MAX_PROCESSES - 1
		je sys_start_task_done
		jmp find_free_slot

		create_task:
			; Create task at index rbx
			mov cl, [k_id_count]
			inc cl
			mov [k_id_count], cl
			mov byte[k_process_queue+rbx], cl

			; Debug
			; mov r15, qword[k_process_queue]
			; jmp $

			; Now configure task slot
			mov bl, ah
			xor rax, rax
			mov rax, rsp ; Just use current kernel stack
			cmp bl, 1
			je start_task_1
			cmp bl, 2
			je start_task_2
			cmp bl, 3
			je start_task_3
			cmp bl, 4
			je start_task_4
			cmp bl, 5
			je start_task_5
			cmp bl, 6
			je start_task_6
			cmp bl, 7
			je start_task_7
			cmp bl, 8
			je start_task_8
			cmp bl, 9
			je start_task_9
			jmp no_task_slot

			start_task_1:
				mov [k_task_1.r_rip], sys_task_1
				mov [k_task_1.r_rsp], rax
				mov [k_task_1.r_rbp], rbp
				mov [k_task_1.r_pid], rcx
				jmp sys_start_task_done
			start_task_2:
				mov [k_task_2.r_rip], sys_task_2
				mov [k_task_2.r_rsp], rax
				mov [k_task_2.r_rbp], rbp
				mov [k_task_2.r_pid], rcx
				jmp sys_start_task_done
			start_task_3:
				mov [k_task_3.r_rip], sys_task_3
				mov [k_task_3.r_rsp], rax
				mov [k_task_3.r_rbp], rbp
				mov [k_task_3.r_pid], rcx
				jmp sys_start_task_done
			start_task_4:
				mov [k_task_4.r_rip], sys_task_4
				mov [k_task_4.r_rsp], rax
				mov [k_task_4.r_rbp], rbp
				mov [k_task_4.r_pid], rcx
				jmp sys_start_task_done
			start_task_5:
				mov [k_task_5.r_rip], sys_task_5
				mov [k_task_5.r_rsp], rax
				mov [k_task_5.r_rbp], rbp
				mov [k_task_5.r_pid], rcx
				jmp sys_start_task_done
			start_task_6:
				mov [k_task_6.r_rip], sys_task_6
				mov [k_task_6.r_rsp], rax
				mov [k_task_6.r_rbp], rbp
				mov [k_task_6.r_pid], rcx
				jmp sys_start_task_done
			start_task_7:
				mov [k_task_7.r_rip], sys_task_7
				mov [k_task_7.r_rsp], rax
				mov [k_task_7.r_rbp], rbp
				mov [k_task_7.r_pid], rcx
				jmp sys_start_task_done
			start_task_8:
				mov [k_task_8.r_rip], sys_task_8
				mov [k_task_8.r_rsp], rax
				mov [k_task_8.r_rbp], rbp
				mov [k_task_8.r_pid], rcx
				jmp sys_start_task_done
			start_task_9:
				mov [k_task_9.r_rip], sys_task_9
				mov [k_task_9.r_rsp], rax
				mov [k_task_9.r_rbp], rbp
				mov [k_task_9.r_pid], rcx
				jmp sys_start_task_done

	no_task_slot:
		mov r15, 0xDEAD1
		jmp $

	sys_start_task_done:
		sti
		popaq
		ret

; ==============================
; sys_check_queue_empty
; ---------------------
; IN  - None
; OUT - rbx = Returns 0 if empty
; ==============================

sys_check_queue_empty:
	push rax
	xor al, al
	xor rbx, rbx

	sys_check_queue_empty_loop:
		mov al, byte[k_process_queue+rbx]
		cmp al, 0
		jne sys_check_queue_empty_isNotEmpty
		inc rbx
		cmp rbx, 9
		je sys_check_queue_empty_isEmpty
		jmp sys_check_queue_empty_loop

	sys_check_queue_empty_isEmpty:
		mov rbx, 0
		jmp sys_check_queue_empty_done

	sys_check_queue_empty_isNotEmpty:
		mov rbx, 1
		jmp sys_check_queue_empty_done

sys_check_queue_empty_done:
	pop rax
	ret

; =================================
; sys_load_program (stub)
; ----------------
; IN  - rax = User program location
; 		rcx = Size of program
; OUT - None
; =================================

sys_load_program:
	pushaq
	; Load ring 3 program into user memory section
	push rax rcx
	mov rsi, rax
	mov rdi, sys_free_mem
	rep movsb
	; Execute ring 3 program
	mov rax, sys_free_mem
	call sys_switch_user_mode
	; Wipe it out
	pop rcx rax
	mov rax, 0x0
	mov rdi, sys_free_mem
	rep stosb

	sys_load_program_done:
		popaq
		ret

; =================================
; sys_switch_user_mode
; --------------------
; IN  - rax = User program location
; OUT - None
; =================================

sys_switch_user_mode:
	; Let's head to ring 3 userland
	mov rbx, rax
	xor rax, rax

	cli
	mov ax, 0x1B ; R3_DATA_SELECTOR | 0x3
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	mov rax, rsp

	push 0x1B ; R3_DATA_SELECTOR | 0x3
	push rax
	pushf

	; Enable interrupts in user mode
	pop rax
	or rax, 0x200
	push rax

	push 0x23 ; R3_CODE_SELECTOR | 0x3
	push rbx
	iretq