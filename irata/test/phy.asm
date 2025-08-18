ldy #$12
phy
ldy #$34
phy
ldy #$00
ply
tya
cmp #$34
jne fail
ply
tya
cmp #$12
jne fail

hlt

fail: crs
