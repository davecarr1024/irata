; Test LDY - Load Y register

; Immediate mode
ldy #$12
tya
cmp #$12
jne fail

; Absolute mode
ldy data
tya
cmp #$34
jne fail

; Zero page mode
lda #$56
sta $10
ldy $10
tya
cmp #$56
jne fail

; Zero page at boundary (0x00)
lda #$AA
sta $00
ldy $00
tya
cmp #$AA
jne fail

; Zero page at boundary (0xFF)
lda #$BB
sta $FF
ldy $FF
tya
cmp #$BB
jne fail

; Absolute mode at different addresses
lda #$CC
sta $0200
ldy $0200
tya
cmp #$CC
jne fail

hlt
fail: crs

data: .byte 0x34
