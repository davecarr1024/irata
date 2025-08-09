lda #$12
ora #$34
cmp #$36
jne fail
hlt
fail: crs