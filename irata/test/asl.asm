lda #$12
asl
cmp #$24
jne fail
hlt
fail: crs