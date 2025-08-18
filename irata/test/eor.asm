lda #$12
eor #$34
cmp #$26
jne fail

lda #$12
eor data
cmp #$44
jne fail

lda #$AB
sta $00
lda #$BC
eor $00
cmp #$17
jne fail

hlt

fail: crs

data: .byte 0x56
