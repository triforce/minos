; =========================================================
; minos -- a hobby operating system written for x86-64
; Copyright (C) 2010-2016
;
; Scheduler
; =========================================================

reschedule:
	cli ; Interrupts will only get renabled after a successful task switch
	pushaq
	xor rax, rax
	xor rbx, rbx
	xor rcx, rcx
	xor rdx, rdx
	xor r14, r14
	xor r15, r15

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
		cmp rbx, MAX_PROCESSES - 1
		; Are we at the end? If so loop back around ONCE
		jge loop_back_once
		jmp check_queue

		check_current:
			cmp al, byte[k_current_process.r_pid]
			je pid_matches
			jmp found_next_task

		loop_back_once:
			cmp dh, 1
			je exit_00
			mov dh, 1
			mov rbx, 0
			jmp check_queue

	found_next_task:
		jmp load_task

	reset_count_once:
		cmp dl, 1
		je exit_02
		mov dl, 1
		mov rbx, 0
		jmp find_next_free

	check_empty_once:
		cmp dl, 1
		je exit_02
		mov dl, 1
		mov rbx, 0
		jmp check_empty

	pid_matches:
		; Move this pid to next free task slot, must save where it left off first
		mov rcx, rbx
		mov ah, al ; ah = task id
		push rbx
		; Check if anything else in the queue, if not then just exit
		inc rbx
		check_empty:
			mov al, byte[k_process_queue+rbx]
			cmp al, 0
			jne pid_matches_cont
			inc rbx
			cmp rbx, MAX_PROCESSES - 1
			je check_empty_once
			jmp check_empty

		pid_matches_cont:
			; cmp ah, al
			; je exit_sched
			mov dl, 0 ; Reset the count
			pop rbx
			mov byte[k_process_queue+rbx], 0 ; Clear current byte
			inc rbx
			cmp rbx, MAX_PROCESSES - 1
			jle find_next_free
			mov rbx, 0
			mov dl, 1

		find_next_free:
			mov al, byte[k_process_queue+rbx]
			cmp al, 0
			je found_free_add_here
			inc rbx
			cmp rbx, MAX_PROCESSES - 1
			; Are we at the end? If so loop back around once
			jge reset_count_once
			jmp find_next_free

		found_free_add_here:
			mov byte[k_process_queue+rbx], ah
			; rcx contains index where we left off before
			mov rbx, rcx
			inc rbx
			cmp rbx, MAX_PROCESSES - 1
			jge loop_back_once
			; mov rbx, 0
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
		xor rax, rax
		
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

		jmp exit_sched ; No previous task

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

	exit_00:
		mov r14, 0xDEAD0 ; Task queue is empty
		mov r15, qword[k_process_queue]
		jmp exit_sched
	exit_01:
		mov r14, 0xDEAD1 ; Failed to load task
		mov r15, qword[k_process_queue]
		jmp $
	exit_02:
		mov r14, 0xDEAD2 ; Task queue is full
		mov r15, qword[k_process_queue]
		jmp $
	exit_sched:
		xor rax, rax
		popaq
	ret