; =========================================================
; minos -- a hobby operating system written for x86-64
; Copyright (C) 2010-2017
;
; Core system programs
; =========================================================

; Idle system task - always needs to be running!
sys_task_1:
	mov r12, qword[k_process_queue]
	jmp sys_task_1

sys_task_2:
	mov byte[0x000B809A], '1'
	mov byte[0x000B809A], '2'
	mov byte[0x000B809A], '3'
	mov byte[0x000B809A], '4'
	mov byte[0x000B809A], '5'
	mov byte[0x000B809A], '6'
	mov byte[0x000B809A], '7'
	mov byte[0x000B809A], '8'
	jmp sys_task_2

sys_task_3:
	mov al, byte[0x000B809C]
	inc al
	mov byte[0x000B809C], al
	mov rax, 0
	int 90h

sys_task_4:
	mov rax, 0
	int 90h

sys_task_5:
	mov rax, 0
	int 90h

sys_task_6:
	mov rax, 0
	int 90h

sys_task_7:
	mov rax, 0
	int 90h

sys_task_8:
	mov rax, 0
	int 90h

sys_task_9:
	mov rax, 0
	int 90h