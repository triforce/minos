; =========================================================
; minos -- a hobby operating system written for x86-64
; Copyright (C) 2010-2017
;
; Key Handler / CLI
; =========================================================

; TODO
detect_kb:
	; Detect keyboard type
	; Some keys have different scan codes

start_kb:
	push rax rbx rcx rdx
	xor rax, rax
	xor rbx, rbx
	xor rcx, rcx
	xor rdx, rdx

get_key:
	in al, 60h
	xor dx, dx
	xor bx, bx
	
check_table:
	; Check the converter table
	mov dx, [scantbl+rbx]
	add bx, 2
	cmp bx, 320 ; Size of scan table
	je end_kb
	cmp al, dh
	
	jne check_table

	xor ecx, ecx
	mov [k_c_char], dl
	
	; Check if return key was pressed
	cmp byte [k_c_char], 0dh
	jne output_char
	
	handle_command:
		; Output new line
		mov esi, k_c_char
		mov ecx, 1
		call sys_command_print
	
		; Check if buffer contains valid command
		call check_command
		call clear_command_buffer
		jmp end_kb
	
	output_char:
		call add_to_command_buffer
		mov esi, k_c_char
		mov ecx, 1
		call sys_command_print
		jmp end_kb

	end_kb:
		pop rdx rcx rbx rax
		ret

; All commands currently only fit in a dword for simplicity
check_command:
	cmp dword [k_command_buffer], '-spe'
	je sys_cpu_speed_c
	cmp dword [k_command_buffer], '-pci'
	je sys_pci_dump_c
	cmp dword [k_command_buffer], '-mem'
	je sys_show_mem_c
	cmp dword [k_command_buffer], '-alo'
	je sys_alloc_mem_c
	cmp dword [k_command_buffer], '-tas'
	je sys_start_kernel_task

	jmp check_command_done

	sys_start_kernel_task:
		mov al, byte[k_command_buffer+5]
		call sys_convert_ascii_dec
		call sys_start_task
		jmp check_command_done

	sys_cpu_speed_c:
		call sys_cpu_speed
		jmp check_command_done
		
	sys_pci_dump_c:
		mov al, 0 ; Don't configure	
		call sys_pci_list
		jmp check_command_done
		
	sys_show_mem_c:
		mov cl, 2 ; Show in KiB
		call sys_show_mem
		jmp check_command_done
		
	; Allocate (al) bytes of memory, eg -alo 4
	sys_alloc_mem_c:
		mov al, byte [k_command_buffer+5]
		call sys_convert_ascii_dec
		mov cl, al
		call sys_allom
		jmp check_command_done
	
	check_command_done:	
		ret

clear_command_buffer:
	push rbx
	xor rbx, rbx
	
	start_clear_bufer:
		mov [k_command_buffer+rbx], 0
		cmp rbx, 24
		je done_buffer_clear
		inc rbx
		jmp start_clear_bufer
	
	done_buffer_clear:
		mov dword [k_command_buffer_s], 0
		pop rbx
		ret

; dl = char to add to buffer
add_to_command_buffer:
	push rax rbx ax bx
	xor rax, rax
	xor rbx, rbx
	xor ax, ax
	xor bx, bx

	.check_again:
		mov al, byte [k_command_buffer+rbx]
		cmp al, 0
		je .found_free_buffer
		inc rbx
		cmp rbx, 24
		je .kb_buffer_full
		jmp .check_again

	.found_free_buffer:
		mov [k_command_buffer+rbx], dl
		add dword [k_command_buffer_s], 1
		jmp .done_check

	.kb_buffer_full:
		; Buffer is full!

	.done_check:
		pop bx ax rbx rax
		ret
