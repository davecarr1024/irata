ldy #$12
tya
cmp #$12
jne fail

ldy data
tya
cmp #$34
jne fail

hlt
fail: crs

data: .byte 0x34
