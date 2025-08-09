lda #$12
and #$34
cmp #$10
jne fail
hlt
fail: crs