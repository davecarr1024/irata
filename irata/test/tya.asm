; Test TYA - Transfer Y to Accumulator
ldy #$66
tya
cmp #$66
jne fail

; Ensure A changes when Y is transferred
ldy #$BB
tya
cmp #$BB
jne fail

hlt
fail: crs
