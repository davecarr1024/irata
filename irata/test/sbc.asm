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

lda #$46
sec
sbc data
cmp #$41
jne fail

lda #$53
sta $00
lda #$64
sec
sbc $00
cmp #$11
jne fail

hlt

fail: crs

data: .byte 0x05
