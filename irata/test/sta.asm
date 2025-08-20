; Tests for sta/lda

; absolute
lda #$12
sta $0170
lda #$00
lda $0170
cmp #$12
jne fail

; zero page
lda #$34
sta $80
lda #$00
lda $80
cmp #$34
jne fail

; absolute double check for zero page
lda #$00
lda $0080
cmp #$34
jne fail

; zero page indexed by x
; store target 0x56 at 0x0025
lda #$56
sta $25
; x = 0x05
ldx #$05
; clear a
lda #$00
; load a from 0x20 + x
lda $20,x
; should be the target
cmp #$56
jne fail

; zero page indexed by y
; store target 0x78 at 0x0025
lda #$78
sta $36
; y = 0x06
ldy #$06
; clear a
lda #$00
; load a from 0x30 + y
lda $30,y
; should be the target
cmp #$78
jne fail

hlt

fail: crs
