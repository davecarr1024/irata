lda #$12
clc
adc #$34
cmp #$46
jne fail

lda #$12
sec
adc #$34
cmp #$47
jne fail

lda #$12
clc
adc data
cmp #$68
jne fail

lda #$21
sta $00
lda #$43
clc
adc $00
cmp #$64
jne fail

hlt

fail: crs

data: .byte 0x56
