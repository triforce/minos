; =========================================================
; minos -- a hobby operating system written for x86-64
; Copyright (C) 2010-2016
;
; VGA Settings
; =========================================================

ScreenYPos	        dq 0
ScreenWidth		dq 0
ScreenLines		dq 0 ; Memory total requires two lines currently
ScreenBuf1 	        dq 0
ScreenBuf2 	        dq 0
CursorPosition		dq 0

; 80 x 25
MinX			equ 80
MinY			equ 25
TotL 			equ 160
TotS 			equ 400

; VGA Settings
; AX
; 15-12 background colour
; 11-8  foreground colour
; 7-0   char
