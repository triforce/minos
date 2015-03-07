; =========================================================
; minos -- a hobby operating system written for x86-64
; Copyright (C) 2010-2015
;
; Scan code to ascii table
; =========================================================

; Currently only supports IBM PC XT

;	stan   shft   ctrl   caps   key
scantbl:
dw	2e63h, 2e43h, 2e03h, 2e43h  ; C
dw	2c7ah, 2c5ah, 2c1ah, 2c5ah  ; Z
dw	2d78h, 2d58h, 2d18h, 2d58h  ; X
dw	2f76h, 2f56h, 2f16h, 2f56h  ; V
dw	3062h, 3042h, 3002h, 3042h  ; B
dw	316eh, 314eh, 310eh, 314eh  ; N
dw	326dh, 324dh, 320dh, 324dh  ; M
dw	1e61h, 1e41h, 1e9eh, 1e41h  ; A
dw	1f73h, 1f53h, 1f0dh, 1f53h  ; S
dw	2064h, 2044h, 20a0h, 2044h  ; D
dw	2166h, 2146h, 21a1h, 2146h  ; F
dw	2267h, 2247h, 22a2h, 2247h  ; G
dw	2368h, 2348h, 23a3h, 2348h  ; H
dw	246Ah, 244Ah, 24a4h, 244Ah  ; J
dw	256Bh, 254Bh, 25a5h, 254Bh  ; K
dw	266Ch, 264Ch, 26a6h, 264Ch  ; L
dw	1071h, 1051h, 1090h, 1051h  ; Q
dw	1177h, 1157h, 1191h, 1157h  ; W
dw	1265h, 1245h, 1292h, 1245h  ; E
dw	1372h, 1352h, 1393h, 1352h  ; R
dw	1474h, 1454h, 1494h, 1454h  ; T
dw	1579h, 1559h, 1595h, 1559h  ; Y
dw	1675h, 1655h, 1696h, 1655h  ; U
dw	1769h, 1749h, 1797h, 1749h  ; I
dw	186fh, 184fh, 1898h, 184fh  ; O
dw	1970h, 1950h, 1999h, 1950h  ; P

dw	0231h, 0231h, 0231h, 0231h  ; 1
dw	0332h, 0332h, 0332h, 0332h  ; 2
dw	0433h, 0433h, 0433h, 0433h  ; 3
dw	0534h, 0534h, 0534h, 0534h  ; 4
dw	0635h, 0635h, 0635h, 0635h  ; 5
dw	0736h, 0736h, 0736h, 0736h  ; 6
dw	0837h, 0837h, 0837h, 0837h  ; 7
dw	0938h, 0938h, 0938h, 0938h  ; 8
dw	0a39h, 0a39h, 0a39h, 0a39h  ; 9
dw	0b30h, 0b30h, 0b30h, 0b30h  ; 0

dw	3920h, 3920h, 3920h, 3920h  ; SPACEBAR
dw	1c0dh, 1c0dh, 1c0dh, 1c0dh  ; ENTER
dw	0c2dh, 0c2dh, 0c2dh, 0c2dh  ; -
