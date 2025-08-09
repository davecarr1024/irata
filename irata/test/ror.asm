lda #$12
clc
ror
cmp #$09
jne fail

lda #$12
sec
ror
cmp #$89
jne fail

hlt
fail: crs