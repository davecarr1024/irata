lda #$12
ora #$34
cmp #$36
jne fail

lda #$12
ora data
cmp #$BB

hlt

fail: crs

data: .byte 0xAB
