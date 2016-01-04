; =========================================================
; minos -- a hobby operating system written for x86-64
; Copyright (C) 2010-2016
;
; Detect memory
; =========================================================

e820_buffer: times 32    	db 0
e820_bufferLength        	equ $ - e820_buffer
line				db crlf, 0
ram_e820h_msg			db 'Memory Available Below 1MB (bytes): ', 0
ram_88h_msg			db 'Memory Available Above 1MB (kb): ', 0

detect_memory:

do_e820:
	pusha

	mov [total_mem], dword 0

	mov di, e820_buffer
	xor ebx, ebx
	mov edx, 0534D4150h
	mov ecx, e820_bufferLength
	mov eax, 0E820h
	int 15h
	jc .nsupported

.e820_check_1:
	cmp eax, 0534D4150h
	stc
	jne .e820_check_4

	push ebx
	push ecx

	mov eax, dword [di + 16]
	cmp eax, 1
	jg .not_os
	mov eax, [di + 8]
	add eax, [total_mem]
	mov [total_mem], eax ; Save total mem
	jmp .print_values

.not_os:
	cmp eax,2
	jg .not_reserved
	jmp .print_values

.not_reserved:
	cmp eax, 3
	jg .not_reclaimed
	jmp .print_values

.not_reclaimed:
	cmp eax, 4
	jg .handle_acpinvs
	jmp .print_values

.handle_acpinvs:
	cmp eax, 5
	jg .print_values

.print_values:
	; TODO

.e820_check_2:
	pop ecx
	cmp ecx, 24
	jne .end_record
	test dword [di + 20], 1
	je .ignore
	jmp .non_volatile

.ignore:
	; TODO

.non_volatile:
	test dword [di + 20], 1 shl 1
	jne .end_record

.end_record:
	pop ebx
	or ebx, ebx
	je .e820_check_3

	mov edx, 0534D4150h
	mov ecx, e820_bufferLength
	mov eax, 0E820h
	int 15h

	jnc .e820_check_1

.e820_check_3:
	clc

.e820_check_4:
	jmp .finish_e820

.nsupported:
	mov [mem_check], 0
	mov al, 'n'
	mov ah, 0eh
	int 10h

.finish_e820:
	popa
	call do_e88h
	ret

do_e88h:
	push ax
	mov ah, 88h
	int 15h
	jc .nsupported_88
	mov [user_mem], ax
	pop ax
	ret

	.nsupported_88:
		mov [mem_check], 0
    		mov al, 'n'
    		mov ah, 0eh
    		int 10h
    		ret