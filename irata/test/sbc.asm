lda #$46
clc
sbc #$34
cmp #$11
jne fail

lda #$46
sec
sbc #$34
cmp #$12
jne fail

hlt

fail: crs
