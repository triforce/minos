; =========================================================
; minos -- a hobby operating system written for x86-64
; Copyright (C) 2010-2017
;
; Userland programs
; =========================================================

; Hello World example - Output simple text and exit
u_prog_1:
    ; Print some text
	mov rbx, app_data_1
	mov rax, 1
	int 90h
	; Exit
	mov rax, 0
	int 90h
	mov r13, 0xEDEAD ; Debug - Should NEVER reach this point
	jmp $

app_data_1	    	db 'Hello World I am executing in User Mode!', 0
u_prog_1_size		= $ - u_prog_1