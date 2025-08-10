ldx #$12
inx
txa
cmp #$13
jne fail

hlt

fail: crs
