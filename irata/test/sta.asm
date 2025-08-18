lda #$12
sta $0170
lda #$00
lda $0170
cmp #$12
jne fail

lda #$34
sta $80
lda #$00
lda $80
cmp #$34
jne fail

lda #$00
lda $0080
cmp #$34
jne fail

hlt

fail: crs
