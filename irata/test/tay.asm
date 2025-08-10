lda #$12
tay
lda #$00
tya
cmp #$12
jne fail

ldy #$34
tya
cmp #$34
jne fail

hlt

fail: crs
