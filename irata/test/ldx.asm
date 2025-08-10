ldx #$12
txa
cmp #$12
jne fail

ldx data
txa
cmp #$34
jne fail

hlt
fail: crs

data: .byte 0x34
