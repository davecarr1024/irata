ldx #$12
stx $0000
ldx #$00
ldx $0000
txa
cmp #$12
jne fail

hlt

fail: crs
