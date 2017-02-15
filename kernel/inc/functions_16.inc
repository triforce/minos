; =========================================================
; minos -- a hobby operating system written for x86-64
; Copyright (C) 2010-2017
;
; Real mode functions
; ==========================================================

print_string:
	; Pushes contents of ax to top of stack
	push ax
	; Decrements the stack pointer and then stores AX on the top of the stack

	@@:
		; Loads byte from ds:si into AL
		lodsb
		; Check if AL is 0
		or al, al
		jz .ret
		mov ah, 0eh

		; Teletext output, AL = character to write
		int 10h
		jmp @b

	.ret:
		; Loads the value from the top of the stack to AX, increment pointer
		pop ax
		ret

print_register:
	pusha
	push 0
	mov bx, 10
	
	@@:
		xor dx, dx
		div bx
		add dx, '0'
		push dx
		or al, al
		jz @f
		jmp @b

	@@:
		pop ax
		or al, al
		jz @f
		mov ah, 0Eh
		int 10h
		jmp @b

	@@:
		popa
		ret