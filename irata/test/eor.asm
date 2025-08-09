lda #$12
eor #$34
cmp #$26
jne fail
hlt
fail: crs