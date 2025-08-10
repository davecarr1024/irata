lda #$12
sta $0000
lda #$00
lda $0000
cmp #$12
jne fail

hlt

fail: crs
