; Test NOP instruction - should do nothing
lda #$42
nop
nop
nop
cmp #$42
jne fail

hlt
fail: crs
