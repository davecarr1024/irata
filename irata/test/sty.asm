ldy #$12
sty $0000
ldy #$00
ldy $0000
tya
cmp #$12
jne fail

ldy #$34
sty $00
ldy #$00
ldy $00
tya
cmp #$34
jne fail

hlt

fail: crs
