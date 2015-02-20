; =========================================================
; minos -- a hobby operating system written for x86-64
; Copyright (C) 2010-2015
;
; Generic 16-bit macros
; ==========================================================

macro print_String_16 {
; Prints a zero-terminated, ds:si pointed string
; IN - ds:si  = Chars to print
print_string:
	; Pushes contents of ax to top of stack
	push	ax
	;Decrements the stack pointer and then stores AX on the top of the stack

		@@:
		; Loads byte from ds:si into AL
		lodsb
		; Check if AL is 0
		or	al, al
		jz	.ret
		mov	ah, 0eh

		; Teletext output, AL = character to write
		int	10h
		jmp	@b

		.ret:
		; Loads the value from the top of the stack to AX, increment pointer
		pop	ax
}


macro check_88h {
	push ax
	mov	ah, 88h
	int	15h
	mov [0x3004], ax
	cmp ax, 0
	je done_check_88h
	mov byte [mem_check], 0
	done_check_88h:
	mov byte [mem_check], 2
	pop	ax
}


