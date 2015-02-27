; =========================================================
; minos -- a hobby operating system written for x86-64
; Copyright (C) 2010-2015
;
; SysCalls
; =========================================================

; ===================
; sys_show_mem_amount
; -------------------
; Show system memory
; IN / OUT - none
; ===================

sys_show_mem_amount:
	push rcx
	xor ecx, ecx
	mov esi, total_mem
	mov ecx, 4
	call sys_character_print
	pop rcx
	ret

; ===============================
; sys_hex_print
; -------------
; Print hex numbers to the screen
; IN - al = Number to print
; ===============================

sys_hex_print:
	push rbx rax
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

	pop rbx rax
	ret
	
; ===================================
; sys_dec_convert
; -------------
; Print decimal numbers to the screen
; IN - ax/eax/rax = Decimal to print
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

; ===================================
; sys_command_print
; -------------------
; Handle command line input
; IN - esi = string to print
;      ecx = number of chars to print
; ===================================

sys_command_print:
	call sys_character_print
	
	.sys_command_print_done:
		ret

; ===================================
; sys_character_print
; -------------------
; Print characters to the screen
; IN - esi = string to print
;      ecx = number of chars to print
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
			mov al, 0dh
			mov [es:(ebx*2+0xB8000)], ax
			inc bx

			mov al, 0ah
			mov [es:(ebx*2+0xB8000)], ax
			inc bx
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
; ===============================

sys_clear_screen:
	push rax rbx
	xor rbx, rbx
	xor rax, rax

	mov rax, 160
	mul rcx
	add rbx, rax
	xor rax, rax
	xor rcx, rcx
	mov rcx, 4000

	start_clear:
	add rbx, 2
	mov ah, 256_white_black
	mov [0B8000h + rbx], rax
	cmp rbx, rcx
	jne start_clear
	pop rax rbx
	xor rcx, rcx
	ret

; ==============================================
; sys_pci_dump
; ----------------
; Output PCI device list in device/vendor format
; IN / OUT - none
; ==============================================

sys_pci_dump:
	pushaq
	xor ecx, ecx
	mov esi, k_pci_msg
	mov ecx, k_pci_msg_size
	call sys_character_print
	call drv_pci_scan
	popaq
	ret

; ================
; sys_cpu_speed
; ----------------
; Test CPU speed
; IN / OUT - none
; ================

sys_cpu_speed:
	push rax rbx rcx
	
	xor ecx, ecx
	mov esi, k_speed_msg
	mov ecx, k_speed_msg_size
	call sys_character_print
	
	; Output speed
	mov ax, [k_speed]
	xchg al, ah
	call sys_hex_print
	xchg al, ah
	call sys_hex_print

	.done_sys_cpu_speed:
		pop rax rbx rcx
		ret

test_cpu_speed:
	ret
	
; =============================
; sys_wait
; ----------------
; Wait for x amount of time
; IN - RAX = Time to wait in ms
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

; ===============================
; sys_show_mem
; ----------------
; Show total memory free in bytes
; IN  - None
; OUT - Print total to sreen
; ===============================

sys_show_mem:
	push rax rcx rbx
	mov esi, k_memory_msg
	mov ecx, k_memory_msg_size
	call sys_character_print

	mov eax, [total_mem]
	call sys_dec_convert
	
	mov esi, number_buf
	mov ecx, dword [number_count]
	call sys_character_print
	
	mov [number_count], 0
	mov [number_buf], 0
	
	pop rax rcx rbx
	ret