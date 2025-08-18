lda #$12
ora #$34
cmp #$36
jne fail

lda #$0F
ora data
cmp #$FF
jne fail

lda #$FF
sta $00
lda #$F0
ora $00
cmp #$FF
jne fail

hlt

fail: crs

data: .byte 0xFF
