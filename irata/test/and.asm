lda #$12
and #$34
cmp #$10
jne fail

lda #$12
and data
cmp #$02
jne fail

hlt

fail: crs

data: .byte 0x02
