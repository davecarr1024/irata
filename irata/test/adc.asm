; Tests for ADC immediate

; 0x12 + 0x34 + 0 = 0x46
lda #$12
clc
adc #$34
cmp #$46
jne fail

; 0x12 + 0x34 + 1 = 0x47
lda #$12
sec
adc #$34
cmp #$47
jne fail

hlt

fail: crs
