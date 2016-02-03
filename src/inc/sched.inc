; =========================================================
; minos -- a hobby operating system written for x86-64
; Copyright (C) 2010-2016
;
; Scheduler
; =========================================================

reschedule:
	cli ; Interrupts will only get renabled after a successful task
	pushaq
	xor rax, rax
	xor rbx, rbx
	xor rdx, rdx
	xor r14, r14

	; xor rax, rax
	mov [k_queue_empty], 0
	
	; If there is no task currently running then just start the first one in the queue
	cmp [k_current_process.r_pid], 0
	je start_first_task

	check_queue:
		mov al, byte[k_process_queue+rbx]
		; Check if its zero
		cmp al, 0
		jne check_current
		inc rbx
		cmp rbx, 10
		; Are we at the end? If so loop back around ONCE
		je loop_back_once
		jmp check_queue

		check_current:
			cmp al, byte[k_current_process.r_pid]
			je pid_matches

			jmp found_next_task

		loop_back_once:
			cmp rdx, 1
			je exit_0
			mov rdx, 1
			mov rbx, 0
			jmp check_queue

	found_next_task:
		jmp load_task

	reset_count:
		mov rbx, 0
		jmp find_next_free ; TODO If the queue is full we will have a problem!

	pid_matches:
		; Move this pid to next free task slot, must save where it left off first
		mov rcx, rbx
		mov ah, al ; ah = task id

		mov byte[k_process_queue+rbx], 0 ; Clear current byte
		; je reset_count

		find_next_free:
			mov al, byte[k_process_queue+rbx]
			cmp al, 0
			je found_free_add_here
			inc rbx
			cmp rbx, 10
			; Are we at the end? If so the queue is full so leave!
			je exit_02
			jmp find_next_free

		found_free_add_here:
			mov byte[k_process_queue+rbx], ah
			; rcx contains index where we left off before
			mov rbx, rcx
			inc rbx
			cmp rbx, 10
			je loop_back_once
			jmp check_queue

	start_first_task:
		mov al, byte[k_process_queue+0]
		mov [k_queue_empty], 1
		jmp found_next_task

	mov r14, 0xDEAD4 ; Should never get here...

	; Handle swapping the tasks
	load_task:
		mov byte[0x000B8094], al
		mov bl, al
		
		mov rax, k_task_1
		cmp byte[k_task_1.r_pid], bl
		je check_first_task
		mov rax, k_task_2
		cmp byte[k_task_2.r_pid], bl
		je check_first_task
		mov rax, k_task_3
		cmp byte[k_task_3.r_pid], bl
		je check_first_task
		mov rax, k_task_4
		cmp byte[k_task_4.r_pid], bl
		je check_first_task
		mov rax, k_task_5
		cmp byte[k_task_5.r_pid], bl
		je check_first_task
		mov rax, k_task_6
		cmp byte[k_task_6.r_pid], bl
		je check_first_task
		mov rax, k_task_7
		cmp byte[k_task_7.r_pid], bl
		je check_first_task
		mov rax, k_task_8
		cmp byte[k_task_8.r_pid], bl
		je check_first_task
		mov rax, k_task_9
		cmp byte[k_task_9.r_pid], bl
		je check_first_task
		mov rax, k_task_10
		cmp byte[k_task_10.r_pid], bl
		je check_first_task
		jmp exit_01 ; No valid task

	check_first_task:
		cmp [k_queue_empty], 1
		je load_current

		mov rbx, rax
		xor rax, rax
		mov rax, [k_current_process.r_rsp]
		mov [k_previous_process.r_rsp], rax
		mov rax, [k_current_process.r_rip]
		mov [k_previous_process.r_rip], rax
		mov rax, [k_current_process.r_pid]
		mov [k_previous_process.r_pid], rax
		mov rax, rbx

	load_current:
		mov rdx, [rax + 0]
		mov [k_current_process.r_rip], rdx
		mov rdx, [rax + 8]
		mov [k_current_process.r_rsp], rdx
		mov rdx, [rax + 16]
		mov [k_current_process.r_rbp], rdx
		mov rdx, [rax + 24]
		mov [k_current_process.r_pid], rdx

		cmp [k_queue_empty], 1
		je exit_sched

		mov [k_previous_process.r_rsp], r9
		mov [k_previous_process.r_rip], r8

	load_previous:
		; Now move update the previous process from the task list
		xor rax, rax
		mov al, byte[k_previous_process.r_pid]
		mov bl, al

		mov rax, k_task_1
		cmp byte[k_task_1.r_pid], bl
		je update_previous_task
		mov rax, k_task_2
		cmp byte[k_task_2.r_pid], bl
		je update_previous_task
		mov rax, k_task_3
		cmp byte[k_task_3.r_pid], bl
		je update_previous_task
		mov rax, k_task_4
		cmp byte[k_task_4.r_pid], bl
		je update_previous_task
		mov rax, k_task_5
		cmp byte[k_task_5.r_pid], bl
		je update_previous_task
		mov rax, k_task_6
		cmp byte[k_task_6.r_pid], bl
		je update_previous_task
		mov rax, k_task_7
		cmp byte[k_task_7.r_pid], bl
		je update_previous_task
		mov rax, k_task_8
		cmp byte[k_task_8.r_pid], bl
		je update_previous_task
		mov rax, k_task_9
		cmp byte[k_task_9.r_pid], bl
		je update_previous_task
		mov rax, k_task_10
		cmp byte[k_task_10.r_pid], bl
		je update_previous_task

		jmp exit_01 ; No valid task

		update_previous_task:
			mov rdx, [k_previous_process.r_rip]
			mov [rax + 0], rdx
			mov rdx, [k_previous_process.r_rsp]
			mov [rax + 8], rdx
			mov rdx, [k_previous_process.r_rbp]
			mov [rax + 16], rdx
			mov rdx, [k_previous_process.r_pid]
			mov [rax + 24], rdx

		jmp exit_sched

	exit_0:
		mov r14, 0xDEAD0 ; Task queue is empty
		jmp exit_sched
	exit_01:
		mov r14, 0xDEAD1 ; No valid task matching pid
		jmp exit_sched
	exit_02:
		mov r14, 0xDEAD2 ; Task queue is full
		jmp exit_sched
	exit_sched:
		xor rax, rax
		popaq
	ret

; Task demos
; ==========
; Two simple tasks to demonstrate multitasking

sys_task_1:
	; mov al, [k_process_variable1]
	; inc al
	; mov [k_process_variable1], al
	mov byte[0x000B8098], 'a'
	mov byte[0x000B8098], 'b'
	mov byte[0x000B8098], 'c'
	mov byte[0x000B8098], 'd'
	mov byte[0x000B8098], 'e'
	mov byte[0x000B8098], 'f'
	mov byte[0x000B8098], 'g'
	mov byte[0x000B8098], 'h'
	jmp sys_task_1

sys_task_2:
	; mov al, [k_process_variable2]
	; inc al
	; mov [k_process_variable2], al
	mov byte[0x000B809A], '1'
	mov byte[0x000B809A], '2'
	mov byte[0x000B809A], '3'
	mov byte[0x000B809A], '4'
	mov byte[0x000B809A], '5'
	mov byte[0x000B809A], '6'
	mov byte[0x000B809A], '7'
	mov byte[0x000B809A], '8'
	jmp sys_task_2