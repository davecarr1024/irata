; Test TXA - Transfer X to Accumulator
ldx #$55
txa
cmp #$55
jne fail

; Ensure A changes when X is transferred
ldx #$AA
txa
cmp #$AA
jne fail

hlt
fail: crs
