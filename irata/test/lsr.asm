lda #$12
lsr
cmp #$09
jne fail
hlt
fail: crs