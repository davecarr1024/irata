; Test LDX - Load X register

; Immediate mode
ldx #$12
txa
cmp #$12
jne fail

; Absolute mode
ldx data
txa
cmp #$34
jne fail

; Zero page mode
lda #$56
sta $10
ldx $10
txa
cmp #$56
jne fail

; Zero page at boundary (0x00)
lda #$AA
sta $00
ldx $00
txa
cmp #$AA
jne fail

; Zero page at boundary (0xFF)
lda #$BB
sta $FF
ldx $FF
txa
cmp #$BB
jne fail

; Absolute mode at different addresses
lda #$CC
sta $0200
ldx $0200
txa
cmp #$CC
jne fail

hlt
fail: crs

data: .byte 0x34
