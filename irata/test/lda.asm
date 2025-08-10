lda #$12
cmp #$12
jne fail

lda data
cmp #$34
jne fail

hlt
fail: crs

data: .byte 0x34
