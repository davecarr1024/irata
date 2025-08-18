lda #$12
and #$34
cmp #$10
jne fail

lda #$12
and data
cmp #$02
jne fail

lda #$F0
sta $00
lda #$FF
and $00
cmp #$F0
jne fail

hlt

fail: crs

data: .byte 0x02
