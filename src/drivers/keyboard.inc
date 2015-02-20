; =========================================================
; minos -- a hobby operating system written for x86-64
; Copyright (C) 2010-2015
;
; Key Handler
; ==========================================================

; TODO
detect_kb:
	; Detect keyboard type
	; Some keys have different scan codes

start_kb:
	xor rax, rax
	xor rbx, rbx
	xor	rcx, rcx
	xor rdx, rdx

get_key:
	in al, 60h
	xor dx, dx
	xor bx, bx
	
check_table:
	mov dx, [scantbl+rbx] ; Check the converter table
	add bx, 2
	cmp bx, 224 ; Size of scan table
	je 	end_kb
	cmp al, dh
	
	jne check_table

	xor ecx, ecx
	mov [k_c_char], dl

	; TODO disabled command buffer
	; call add_to_command_buffer

	mov esi, k_c_char
	mov ecx, 1
	call sys_command_print
	
	jmp end_kb

end_kb:
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
	jmp .done_check

	.kb_buffer_full:
	; Buffer is full!

	.done_check:
	pop rax rbx ax bx
	ret