; =========================================================
; minos -- a hobby operating system written for x86-64
; Copyright (C) 2010-2015
;
; Console commands
; =========================================================

com_prefix     db  26h              ; All commands must start with a '&'

command_tbl:
db      6dh, 65h, 6dh               ; 'mem' - show memory table
db	0h
db      73h, 70h, 65h               ; 'spe' - test clock speed
db	0h
db      72h, 65h, 62h               ; 'reb' - reboot the system
db	0h
db      63h, 70h, 75h, 69h, 64h     ; 'cpuid' - show cpu flags