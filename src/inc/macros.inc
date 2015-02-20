; minos Generic Library - General macros

macro mov p1, p2 { 
  if p2 eqtype '' 
    local ..string,..skip 
	jmp ..skip 
      ..string db p2,0 
      ..skip: 
	mov p1,..string 
  else 
	mov p1,p2 
  end if 
}

macro show_menu {
	mov 	ah, 01h 
	int 	50h
	mov 	si, h_option_1
	int 	50h
	mov 	si, h_option_2
	int 	50
	mov 	si, h_option_3
	int 	50h
	mov 	si, h_option_4
	int 	50h	
}

macro show_task_menu {
	mov 	ah, 01h 
	int 	50h
	mov 	si, ht_option_1
	int 	50h
	mov 	si, ht_option_2
	int	50
	mov 	si, ht_option_3
	int 	50h
	mov 	si, ht_option_4
	int 	50h
	mov 	si, ht_option_5
	int 	50h		
}

macro print_chars {
	character_print:
	; Global text printer
	pusha

	mov 	ax, 0x0F0E 
	mov 	dx, 0x03D4 
	out 	dx, al 
	inc 	dx 
	in 	al, dx 
	xchg 	ah,al 
	dec 	dx 
	out 	dx, al 
	inc 	dx 
	in 	al, dx 
	movzx 	ebx, ax

	; Set screen colours
	mov	ah, 0x0F

		.character_printLoop:
		; Print each character
		lodsb 
		cmp 	al, 10
		je 	.nextChar 
		cmp 	al, 13
		je 	.handleCR
		mov 	[es:(ebx*2+0xB8000)], ax

		inc 	bx 
		jmp 	.nextChar 

		.handleCR: 
		; Carriage return handling

		.nextChar: 
		; Loop through buffer
		loop 	.character_printLoop

		mov 	al, 0x0E 
		mov 	ah, bh 
		mov 	dx, 0x03D4 
		out 	dx, ax 
		inc 	ax 
		mov 	ah, bl 
		out 	dx, ax

		popa
		ret 
}

macro stdcall proc,[arg] {
      reverse push arg
      common call proc
}
