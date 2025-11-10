; Test PLA - Pull Accumulator from stack
; Push a value, change A, then pull it back
lda #$77
pha
lda #$00
pla
cmp #$77
jne fail

; Test multiple push/pull
lda #$11
pha
lda #$22
pha
lda #$00
pla
cmp #$22
jne fail
pla
cmp #$11
jne fail

hlt
fail: crs
