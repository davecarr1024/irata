; Test STY - Store Y register

; Absolute mode
ldy #$12
sty $0100
ldy #$00
ldy $0100
tya
cmp #$12
jne fail

; Zero page mode
ldy #$34
sty $10
ldy #$00
ldy $10
tya
cmp #$34
jne fail

; Zero page at boundary (0x00)
ldy #$AA
sty $00
ldy #$00
ldy $00
tya
cmp #$AA
jne fail

; Zero page at boundary (0xFF)
ldy #$BB
sty $FF
ldy #$00
ldy $FF
tya
cmp #$BB
jne fail

; Absolute mode at different addresses
ldy #$CC
sty $0200
ldy #$00
ldy $0200
tya
cmp #$CC
jne fail

; Store at high memory address
ldy #$DD
sty $01FF
ldy #$00
ldy $01FF
tya
cmp #$DD
jne fail

hlt

fail: crs
