ldy #$12
iny
tya
cmp #$13
jne fail

hlt

fail: crs
