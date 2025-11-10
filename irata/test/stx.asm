; Test STX - Store X register

; Absolute mode
ldx #$12
stx $0100
ldx #$00
ldx $0100
txa
cmp #$12
jne fail

; Zero page mode
ldx #$34
stx $10
ldx #$00
ldx $10
txa
cmp #$34
jne fail

; Zero page at boundary (0x00)
ldx #$AA
stx $00
ldx #$00
ldx $00
txa
cmp #$AA
jne fail

; Zero page at boundary (0xFF)
ldx #$BB
stx $FF
ldx #$00
ldx $FF
txa
cmp #$BB
jne fail

; Absolute mode at different addresses
ldx #$CC
stx $0200
ldx #$00
ldx $0200
txa
cmp #$CC
jne fail

; Store at high memory address
ldx #$DD
stx $01FF
ldx #$00
ldx $01FF
txa
cmp #$DD
jne fail

hlt

fail: crs
