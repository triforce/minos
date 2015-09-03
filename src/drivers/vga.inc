; =========================================================
; minos -- a hobby operating system written for x86-64
; Copyright (C) 2010-2015
;
; VGA Settings
; =========================================================

ScreenYPos	        dd 0
ScreenWidth		dd 0
ScreenLines		dd 0 ; Memory total requires two lines currently
ScreenBuf1 	        dd 0
ScreenBuf2 	        dd 0
CursorPosition		dd 0

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
