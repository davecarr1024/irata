lda #$12
clc
rol
cmp #$24
jne fail

lda #$12
sec
rol
cmp #$25
jne fail

hlt
fail: crs