; =========================================================
; minos -- a hobby operating system written for x86-64
; Copyright (C) 2010-2015
;
; Bootloader FORTIS Disk
; =========================================================

format binary as 'img'
use16

; The origin address which FASM will assume the program begins at when it is loaded into memory
org 7c00h

; Jump over disk info
jmp start
nop

; File System Info, legacy BIOS parameter block (bpb)
OEM:				db "MIN OS"
bpbBytesPerSector:		dw 512
bpbSectorsPerTrack:		dw 18
bpbHeadsPerCylinder:		dw 2
bsDriveNumber:			db 0
bsVolumeLabel:			db "minOS    "

; Custom filesystem
bsFileSystem:			db "FORTIS  "

start:
	; Set up stack and segment registers
	; Stop interrupts
	cli
	xor ax, ax  
	mov ds, ax 
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	mov ax, 0fffh
	mov ss, ax ; Segment where stack is located in memory
	mov esp, 0ffffh
	mov bp, sp
	; Enable interrupts
	sti

	reset_floppy:
		mov ah, 00h 
		int 13h
		jc reset_floppy
	
	; Load kernel from disk into memory     
	read_kernel:
		mov ax, 0000h
		mov es, ax
		mov bx, 1200h

		mov ax, 11
		call configure_int13
		mov ah, 2
		mov al, 16 ; Sectors to read into buffer
	
		; Read AL number of sectors from drive into memory ES:BX
		int 13h
		jc read_kernel
	
	; Load config from disk into memory     
	read_config:
		mov ax, 0000h
		mov es, ax
		mov bx, 0500h

		mov ax, 27
		call configure_int13
		mov ah, 2
		mov al, 1 ; Sectors to read into buffer
	
		; Read AL number of sectors from drive into memory ES:BX
		int 13h
		jc read_config
	
	; Check config is set
	; TODO shutdown if not set
	check_config:
		cmp byte [es:0500h], 00110001b
		jne check_config
	
	; Jump to location where kernel starts in memory
	jmp 0000:1200h
	
	; Calculate head, track and sector
	configure_int13:
		push bx
		push ax

		mov bx, ax
		mov dx, 0
		div word [bpbSectorsPerTrack]
		add dl, 01h

		mov cl, dl
		mov ax, bx

		mov dx, 0
		div word [bpbSectorsPerTrack]
		mov dx, 0
		div word [bpbHeadsPerCylinder]
		mov dh, dl ; Head
		mov ch, al ; Track

		pop ax
		pop bx
		mov dl, byte [bsDriveNumber]
		ret

; Pad out to 512 bytes
times 510 - ($-$$) db 0
; Boot signature
dw 0aa55h

; Table of contents
directories:
	db 01h		; First entry
	db "k_main"	; Filename of first data
	db "BIN"	; Extension of file
	dw 2000h	; Size of file
	
	db 02h		; Second entry
	db "config"	; Filename of second data
	dw 01h		; Size of file
	
	times 512 * 10 -($-directories) db 0

; Files on disk
disk_data:
	file 'k_main/k_main.BIN'
	file 'config'
