; Test PLY - Pull Y register from stack
; Push Y, change it, then pull it back
ldy #$99
phy
ldy #$00
ply
tya
cmp #$99
jne fail

; Test multiple push/pull
ldy #$55
phy
ldy #$66
phy
ldy #$00
ply
tya
cmp #$66
jne fail
ply
tya
cmp #$55
jne fail

hlt
fail: crs
