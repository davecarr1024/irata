ldx #$12
dex
txa
cmp #$11
jne fail

hlt

fail: crs