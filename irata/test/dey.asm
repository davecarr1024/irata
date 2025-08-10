ldy #$12
dey
tya
cmp #$11
jne fail

hlt

fail: crs