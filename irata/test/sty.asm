ldy #$12
sty $0000
ldy #$00
ldy $0000
tya
cmp #$12
jne fail

hlt

fail: crs
