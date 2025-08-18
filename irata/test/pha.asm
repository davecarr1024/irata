; Test for PHA and PLA.

; Push and pull one value.
lda #$12
pha
lda #$00
pla
cmp #$12
jne fail

; Push and pull two values.
lda #$34
pha
lda #$56
pha
lda #$00
pla
cmp #$56
jne fail
pla
cmp #$34
jne fail

hlt

fail: crs
